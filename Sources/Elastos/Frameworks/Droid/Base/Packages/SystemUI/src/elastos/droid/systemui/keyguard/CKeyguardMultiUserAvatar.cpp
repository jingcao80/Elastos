
#include "elastos/droid/systemui/keyguard/CKeyguardMultiUserAvatar.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CKeyguardMultiUserAvatar::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener)

ECode CKeyguardMultiUserAvatar::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    Float r;
    animation->GetAnimatedFraction(&r);
    Float scale = (1 - r) * mInitScale + r * mFinalScale;
    Float alpha = (1 - r) * mInitAlpha + r * mFinalAlpha;
    Int32 textAlpha = (Int32) ((1 - r) * initTextAlpha + r * finalTextAlpha);
    mHost->mFramed->SetScale(scale);
    mHost->mUserImage->SetAlpha(alpha);
    AutoPtr<IColor> helper;
    CColor::AcquireSingleton((IColor**)&helper);
    Int32 argb;
    helper->Argb(textAlpha, 255, 255, 255, &argb)
    mHost->mUserName->SetTextColor(argb);
    return mHost->mUserImage->Invalidate();
}

ECode CKeyguardMultiUserAvatar::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mOnComplete != NULL) {
        mOnComplete->Run();
    }
    return NOERROR;
}

const String CKeyguardMultiUserAvatar::TAG("KeyguardMultiUserAvatar");
const Boolean CKeyguardMultiUserAvatar::DEBUG = IKeyguardConstants::DEBUG;

const Float CKeyguardMultiUserAvatar::ACTIVE_ALPHA = 1.0f;
const Float CKeyguardMultiUserAvatar::INACTIVE_ALPHA = 1.0f;
const Float CKeyguardMultiUserAvatar::ACTIVE_SCALE = 1.5f;
const Float CKeyguardMultiUserAvatar::ACTIVE_TEXT_ALPHA = 0f;
const Float CKeyguardMultiUserAvatar::INACTIVE_TEXT_ALPHA = 0.5f;
const Int32 CKeyguardMultiUserAvatar::SWITCH_ANIMATION_DURATION = 150;

CAR_OBJECT_IMPL(CKeyguardMultiUserAvatar)

CKeyguardMultiUserAvatar::CKeyguardMultiUserAvatar()
    : mActiveAlpha(0.0f)
    , mActiveScale(0.0f)
    , mActiveTextAlpha(0.0f)
    , mInactiveAlpha(0.0f)
    , mInactiveTextAlpha(0.0f)
    , mShadowRadius(0.0f)
    , mStroke(0.0f)
    , mIconSize(0.0f)
    , mFrameColor(0.0f)
    , mFrameShadowColor(0.0f)
    , mTextColor(0.0f)
    , mHighlightColor(0.0f)
    , mTouched(FALSE)
    , mActive(FALSE)
    , mInit(TRUE)
    , mPressLock(TRUE)
{
}

AutoPtr<IKeyguardMultiUserAvatar> CKeyguardMultiUserAvatar::FromXml(
    /* [in] */ Int32 resId,
    /* [in] */ IContext* context,
    /* [in] */ IKeyguardMultiUserSelectorView* userSelector,
    /* [in] */ IUserInfo* info)
{
    AutoPtr<ILayoutInflaterHelper> helper;
    CLayoutInflaterHelper::AcquireSingleton((ILayoutInflaterHelper**)&helper);
    AutoPtr<ILayoutInflater> flater;
    helper->From(context, (ILayoutInflater**)&flater);
    AutoPtr<IView> view;
    flater->Inflate(resId, userSelector, FALSE, (IView**)&view);
    AutoPtr<IKeyguardMultiUserAvatar> icon = IKeyguardMultiUserAvatar::Probe(view);
    icon->Init(info, userSelector);
    return icon;
}

ECode CKeyguardMultiUserAvatar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardMultiUserAvatar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardMultiUserAvatar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetColor(R::color::keyguard_avatar_nick_color, &mTextColor);
    res->GetDimension(R::dimen::keyguard_avatar_size, &mIconSize);
    res->GetDimension(R::dimen::keyguard_avatar_frame_stroke_width, &mStroke);
    res->GetDimension(R::dimen::keyguard_avatar_frame_shadow_radius, &mShadowRadius);
    res->GetColor(R::color::keyguard_avatar_frame_color, &mFrameColor);
    res->GetColor(R::color::keyguard_avatar_frame_shadow_color, &mFrameShadowColor);
    res->GetColor(R::color::keyguard_avatar_frame_pressed_color, &mHighlightColor);
    mActiveTextAlpha = ACTIVE_TEXT_ALPHA;
    mInactiveTextAlpha = INACTIVE_TEXT_ALPHA;
    mActiveScale = ACTIVE_SCALE;
    mActiveAlpha = ACTIVE_ALPHA;
    mInactiveAlpha = INACTIVE_ALPHA;

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    mUserManager = IUserManager::Probe(obj);

    mTouched = FALSE;

    return SetLayerType(IView::LAYER_TYPE_SOFTWARE, NULL);
}

ECode CKeyguardMultiUserAvatar::RewriteIconPath(
    /* [in] */ const String& path,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = path;
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::Init(
    /* [in] */ IUserInfo* user,
    /* [in] */ IKeyguardMultiUserSelectorView* userSelector)
{
    mUserInfo = user;
    mUserSelector = userSelector;

    AutoPtr<IView> view;
    FindViewById(R::id::keyguard_user_avatar, (IView**)&view);
    mUserImage = IImageView::Probe(view);

    AutoPtr<IView> view2;
    FindViewById(R::id::keyguard_user_name, (IView**)&view2);
    mUserName = ITextView::Probe(view2);

    AutoPtr<MultiUserAvatarCache> cache = MultiUserAvatarCache::GetInstance();
    Int32 id;
    user->GetId(&id);
    AutoPtr<IDrawable> drawable;
    cache->Get(id, (IDrawable**)&drawable);
    mFramed = IKeyguardCircleFramedDrawable::Probe(drawable);

    // If we can't find it or the params don't match, create the drawable again
    Boolean res;
    if (mFramed == NULL
            || (mFramed->VerifyParams(mIconSize, mFrameColor, mStroke, mFrameShadowColor,
                    mShadowRadius, mHighlightColor, &res), !res)) {
        AutoPtr<IBitmap> icon;
        //try {
        mUserManager->GetUserIcon(id, (IBitmap**)&icon);
        //} catch (Exception e) {
        //    if (DEBUG) Log.d(TAG, "failed to get profile icon " + user, e);
        //}

        if (icon == NULL) {
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            AutoPtr<IBitmapFactory> helper;
            CBitmapFactory::AcquireSingleton((IBitmapFactory**)&helper);
            helper->DecodeResource(resources, Elastos::Droid::R::drawable::ic_contact_picture, (IBitmap**)&icon);
        }

        mFramed = new KeyguardCircleFramedDrawable(icon, (Int32)mIconSize, mFrameColor, mStroke,
                mFrameShadowColor, mShadowRadius, mHighlightColor);
        MultiUserAvatarCache::GetInstance()->Put(id, mFramed);
    }

    mFramed->Reset();

    mUserImage->SetImageDrawable(mFramed);
    String name;
    mUserInfo->GetName(&name);
    mUserName->SetText(name);
    SetOnClickListener(mUserSelector);
    mInit = FALSE;
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::SetActive(
    /* [in] */ Boolean active,
    /* [in] */ Boolean animate,
    /* [in] */ IRunnable* onComplete)
{
    if (mActive != active || mInit) {
        mActive = active;

        if (active) {
            AutoPtr<IViewParent> res;
            GetParent((IViewParent**)&res);
            AutoPtr<IKeyguardLinearLayout> parent = IKeyguardLinearLayout::Probe(res);
            parent->SetTopChild(this);
            // TODO: Create an appropriate asset when string changes are possible.
            String text;
            mUserName->GetText(&text);
            String str;
            mContext->GetString(R::string::user_switched, String(""), &str);
            SetContentDescription(text + String(". ") + str);
        }
        else {
            String text;
            mUserName->GetText(&text);
            SetContentDescription(text);
        }
    }
    return UpdateVisualsForActive(mActive, animate, SWITCH_ANIMATION_DURATION, onComplete);
}

ECode CKeyguardMultiUserAvatar::UpdateVisualsForActive(
    /* [in] */ Boolean active,
    /* [in] */ Boolean animate,
    /* [in] */ Int32 duration,
    /* [in] */ IRunnable* onComplete)
{
    Float finalAlpha = active ? mActiveAlpha : mInactiveAlpha;
    Float initAlpha = active ? mInactiveAlpha : mActiveAlpha;
    Float finalScale = active ? 1f : 1f / mActiveScale;
    Float initScale;
    mFramed->GetScale(&initScale);
    Int32 finalTextAlpha = active ? (Int32) (mActiveTextAlpha * 255) :
            (Int32) (mInactiveTextAlpha * 255);
    Int32 initTextAlpha = active ? (Int32) (mInactiveTextAlpha * 255) :
            (Int32) (mActiveTextAlpha * 255);
    Int32 textColor = mTextColor;
    mUserName->SetTextColor(textColor);

    if (animate && mTouched) {
        AutoPtr<IValueAnimatorHelper> helper;
        CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
        AutoPtr<IValueAnimator> va;
        helper->OfFloat(0f, 1f, (IValueAnimator**)&va);

        AutoPtr<IAnimatorUpdateListener> lis = new MyAnimatorUpdateListener(this,
                finalAlpha, finalScale, initAlpha, initScale);
        va->AddUpdateListener(lis);
        AutoPtr<IAnimatorListener> lis2 = new MyAnimatorListenerAdapter(onComplete);
        va->AddListener(lis2);
        va->SetDuration(duration);
        va->Start();
    }
    else {
        mFramed->SetScale(finalScale);
        mUserImage->SetAlpha(finalAlpha);
        AutoPtr<IColor> helper;
        CColor::AcquireSingleton((IColor**)&helper);
        Int32 argb;
        helper->Argb(finalTextAlpha, 255, 255, 255, &argb)
        mUserName->SetTextColor(argb);
        if (onComplete != NULL) {
            Post(onComplete);
        }
    }

    mTouched = TRUE;
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::SetPressed(
    /* [in] */ Boolean pressed)
{
    if (mPressLock && !pressed) {
        return NOERROR;
    }

    if (mPressLock || !pressed || IsClickable()) {
        FrameLayout::SetPressed(pressed);
        mFramed->SetPressed(pressed);
        mUserImage->Invalidate();
    }
    return NOERROR;
}

ECode CKeyguardMultiUserAvatar::LockPressed(
    /* [in] */ Boolean pressed)
{
    mPressLock = pressed;
    return SetPressed(pressed);
}

ECode CKeyguardMultiUserAvatar::GetUserInfo(
    /* [out] */ IUserInfo** info)
{
    VALIDATE_NOT_NULL(info)

    *info = mUserInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos