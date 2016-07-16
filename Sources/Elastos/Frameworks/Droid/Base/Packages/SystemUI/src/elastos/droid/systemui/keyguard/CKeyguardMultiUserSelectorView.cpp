
#include "elastos/droid/systemui/keyguard/CKeyguardMultiUserSelectorView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardMultiUserSelectorView::MyRunnable::Run()
{
    return mHost->FinalizeActiveUserNow(TRUE);
}

ECode CKeyguardMultiUserSelectorView::MyRunnable2::Run()
{
    mHost->mActiveUserAvatar = mAvatar;
    //try {
    AutoPtr<IIActivityManager> manager = ActivityManagerNative::GetDefault();
    AutoPtr<IUserInfo> info;
    mAvatar->GetUserInfo((IUserInfo**)&info);
    Int32 id;
    info->GetId(&id);
    ECode ec = manager->SwitchUser(id);
    //} catch (RemoteException re) {
    if (ec == (ECode)RemoteException) {
        Logger::E(TAG, "Couldn't switch user %d", ec);
    }
    //}
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardMultiUserSelectorView::TaskComparator, Object, IComparator)

ECode CKeyguardMultiUserSelectorView::TaskComparator::Compare(
    /* [in] */ IInterface* _task,
    /* [in] */ IInterface* _task2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 namuber1;
    IUserInfo::Probe(lhs)->GetserialNumber(&namuber1);
    Int32 namuber2;
    IUserInfo::Probe(rhs)->GetserialNumber(&namuber2);

    *result = (namuber1 - namuber2);
    return NOERROR;
}

const String CKeyguardMultiUserSelectorView::TAG("KeyguardMultiUserSelectorView");

const Int32 CKeyguardMultiUserSelectorView::FADE_OUT_ANIMATION_DURATION = 100;

CAR_OBJECT_IMPL(CKeyguardMultiUserSelectorView)

CAR_INTERFACE_IMPL(CKeyguardMultiUserSelectorView, FrameLayout, IViewOnClickListener)

CKeyguardMultiUserSelectorView::CKeyguardMultiUserSelectorView()
{
    mOrderAddedComparator = new TaskComparator();
}

ECode CKeyguardMultiUserSelectorView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardMultiUserSelectorView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardMultiUserSelectorView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return FrameLayout::constructor(context, attrs, defStyle);
}

ECode CKeyguardMultiUserSelectorView::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::keyguard_users_grid, (IView**)&view);
    mUsersGrid = IViewGroup::Probe(view);
    mUsersGrid->RemoveAllViews();
    SetClipChildren(FALSE);
    return SetClipToPadding(FALSE);
}

ECode CKeyguardMultiUserSelectorView::SetCallback(
    /* [in] */ IKeyguardHostViewUserSwitcherCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CKeyguardMultiUserSelectorView::AddUsers(
    /* [in] */ ICollection* userList)
{
    AutoPtr<IUserInfo> activeUser;
    //try {
    AutoPtr<IIActivityManager> manager = ActivityManagerNative::GetDefault();
    ECode ec = manager->GetCurrentUser((IUserInfo**)&activeUser);
    //} catch (RemoteException re) {
    if (ec == (ECode)RemoteException) {
        activeUser = NULL;
    }

    AutoPtr<IArrayList> users;
    CArrayList::New(userList, (IArrayList**)&users);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->Sort(users, mOrderAddedComparator);

    Int32 size;
    users->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        users->Get(i, (IInterface**)&obj);
        AutoPtr<IUserInfo> user = IUserInfo::Probe(obj);

        Boolean res;
        if (user->SupportsSwitchTo(&res), res) {
            AutoPtr<KeyguardMultiUserAvatar> uv = CreateAndAddUser(user);
            Int32 id1;
            user->GetId(&id1);
            Int32 id2;
            activeUser->GetId(&id2);
            if (id1 == id2) {
                mActiveUserAvatar = uv;
            }
            uv->SetActive(FALSE, FALSE, NULL);
        }
    }

    return mActiveUserAvatar->LockPressed(TRUE);
}

ECode CKeyguardMultiUserSelectorView::FinalizeActiveUserView(
    /* [in] */ Boolean animate)
{
    if (animate) {
        AutoPtr<IHandler> handler;
        GetHandler((IHandler**)&handler);

        AutoPtr<IRunnable> r = new MyRunnable(this);
        handler->PostDelayed(r, 500);
    }
    else {
        FinalizeActiveUserNow(animate);
    }
    return NOERROR;
}

ECode CKeyguardMultiUserSelectorView::FinalizeActiveUserNow(
    /* [in] */ Boolean animate)
{
    mActiveUserAvatar->LockPressed(FALSE);
    return mActiveUserAvatar->SetActive(TRUE, animate, NULL);
}

AutoPtr<IKeyguardMultiUserAvatar> CKeyguardMultiUserSelectorView::CreateAndAddUser(
    /* [in] */ IUserInfo* user)
{
    AutoPtr<IKeyguardMultiUserAvatar> uv = CKeyguardMultiUserAvatar::FromXml(
            R::layout::keyguard_multi_user_avatar, mContext, this, user);
    mUsersGrid->AddView(uv);
    return uv;
}

ECode CKeyguardMultiUserSelectorView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 mask;
    event->GetActionMasked(&mask);
    if(mask != IMotionEvent::ACTION_CANCEL && mCallback != NULL) {
        mCallback->UserActivity();
    }
    *result = FALSE;
    return NOERROR;
}

void CKeyguardMultiUserSelectorView::SetAllClickable(
    /* [in] */ Boolean clickable)
{
    Int32 count;
    mUsersGrid->GetChildCount(&count);
    for(Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        mUsersGrid->GetChildAt(i, (IView**)&v);
        v->SetClickable(clickable);
        v->SetPressed(FALSE);
    }
}

ECode CKeyguardMultiUserSelectorView::OnClick(
    /* [in] */ IView* v)
{
    if (IKeyguardMultiUserAvatar::Probe(v) == NULL) return NOERROR;

    AutoPtr<IKeyguardMultiUserAvatar> avatar = IKeyguardMultiUserAvatar::Probe(v);
    Boolean res;
    avatar->IsClickable(&res);
    if (res) { // catch race conditions
        if (mActiveUserAvatar == avatar) {
            // If they click the currently active user, show the unlock hint
            mCallback->ShowUnlockHint();
            return NOERROR;
        }
        else {
            // Reset the previously active user to appear inactive
            mCallback->HideSecurityView(FADE_OUT_ANIMATION_DURATION);
            SetAllClickable(FALSE);
            avatar->LockPressed(TRUE);

            AutoPtr<IRunnable> r = new MyRunnable2(this, avatar);
            mActiveUserAvatar->SetActive(FALSE, TRUE, r);
        }
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
