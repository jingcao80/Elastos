
#include "elastos/droid/systemui/statusbar/CExpandableNotificationRow.h"
#include "../R.h"
#include <elastos/droid/R.h>

using Elastos::Droid::Graphics::Drawable::IAnimatable;
using Elastos::Droid::Graphics::Drawable::IAnimationDrawable;
using Elastos::Droid::Graphics::Drawable::IAnimatedVectorDrawable;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IViewStubOnInflateListener;
using Elastos::Droid::View::IViewPropertyAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_INTERFACE_IMPL(CExpandableNotificationRow::InflateListener, Object, IViewStubOnInflateListener);
CExpandableNotificationRow::InflateListener::InflateListener(
    /* [in] */ CExpandableNotificationRow* host)
    : mHost(host)
{}

ECode CExpandableNotificationRow::InflateListener::OnInflate(
    /* [in] */ IViewStub* stub,
    /* [in] */ IView* inflated)
{
    mHost->mGuts = INotificationGuts::Probe(inflated);
    Int32 value = 0;
    mHost->GetClipTopAmount(&value);
    mHost->mGuts->SetClipTopAmount(value);
    mHost->GetActualHeight(&value);
    mHost->mGuts->SetActualHeight(value);
    return NOERROR;
}

CExpandableNotificationRow::Runnable1::Runnable1(
    /* [in] */ IView* view)
    : mView(view)
{}

ECode CExpandableNotificationRow::Runnable1::Run()
{
    mView->SetVisibility(IView::INVISIBLE);
    return NOERROR;
}

CAR_OBJECT_IMPL(CExpandableNotificationRow);
CAR_INTERFACE_IMPL(CExpandableNotificationRow, ActivatableNotificationView, IExpandableNotificationRow);
CExpandableNotificationRow::CExpandableNotificationRow()
    : mRowMinHeight(0)
    , mRowMaxHeight(0)
    , mExpandable(FALSE)
    , mHasUserChangedExpansion(FALSE)
    , mUserExpanded(FALSE)
    , mUserLocked(FALSE)
    , mShowingPublic(FALSE)
    , mSensitive(FALSE)
    , mShowingPublicInitialized(FALSE)
    , mShowingPublicForIntrinsicHeight(FALSE)
    , mIsSystemExpanded(FALSE)
    , mExpansionDisabled(FALSE)
    , mMaxExpandHeight(0)
    , mClearable(FALSE)
    , mWasReset(FALSE)
{}

ECode CExpandableNotificationRow::SetIconAnimationRunning(
    /* [in] */ Boolean running)
{
    SetIconAnimationRunning(running, mPublicLayout);
    SetIconAnimationRunning(running, mPrivateLayout);
    return NOERROR;
}

void CExpandableNotificationRow::SetIconAnimationRunning(
    /* [in] */ Boolean running,
    /* [in] */ INotificationContentView* layout)
{
    if (layout != NULL) {
        AutoPtr<IView> contractedChild;
        layout->GetContractedChild((IView**)&contractedChild);
        AutoPtr<IView> expandedChild;
        layout->GetExpandedChild((IView**)&expandedChild);
        SetIconAnimationRunningForChild(running, contractedChild);
        SetIconAnimationRunningForChild(running, expandedChild);
    }
}

void CExpandableNotificationRow::SetIconAnimationRunningForChild(
    /* [in] */ Boolean running,
    /* [in] */ IView* child)
{
    if (child != NULL) {
        AutoPtr<IView> view;
        child->FindViewById(Elastos::Droid::R::id::icon, (IView**)&view);
        AutoPtr<IImageView> icon = IImageView::Probe(view);
        SetIconRunning(icon, running);

        view = NULL;
        child->FindViewById(Elastos::Droid::R::id::right_icon, (IView**)&view);
        AutoPtr<IImageView> rightIcon = IImageView::Probe(view);
        SetIconRunning(rightIcon, running);
    }
}

void CExpandableNotificationRow::SetIconRunning(
    /* [in] */ IImageView* imageView,
    /* [in] */ Boolean running)
{
    if (imageView != NULL) {
        AutoPtr<IDrawable> drawable;
        imageView->GetDrawable((IDrawable**)&drawable);
        if (IAnimationDrawable::Probe(drawable)) {
            AutoPtr<IAnimationDrawable> animationDrawable = IAnimationDrawable::Probe(drawable);
            if (running) {
                IAnimatable::Probe(animationDrawable)->Start();
            }
            else {
                IAnimatable::Probe(animationDrawable)->Stop();
            }
        }
        else if (IAnimatedVectorDrawable::Probe(drawable)) {
            AutoPtr<IAnimatedVectorDrawable> animationDrawable = IAnimatedVectorDrawable::Probe(drawable);
            if (running) {
                IAnimatable::Probe(animationDrawable)->Start();
            }
            else {
                IAnimatable::Probe(animationDrawable)->Stop();
            }
        }
    }
}

ECode CExpandableNotificationRow::SetStatusBarNotification(
    /* [in] */ IStatusBarNotification* statusBarNotification)
{
    mStatusBarNotification = statusBarNotification;
    return NOERROR;
}

ECode CExpandableNotificationRow::GetStatusBarNotification(
    /* [out] */ IStatusBarNotification** notification)
{
    VALIDATE_NOT_NULL(notification);
    *notification = mStatusBarNotification;
    REFCOUNT_ADD(*notification);
    return NOERROR;
}

ECode CExpandableNotificationRow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ActivatableNotificationView::constructor(context, attrs);
}

ECode CExpandableNotificationRow::Reset()
{
    ActivatableNotificationView::Reset();
    mRowMinHeight = 0;
    Boolean wasExpanded = IsExpanded();
    mRowMaxHeight = 0;
    mExpandable = FALSE;
    mHasUserChangedExpansion = FALSE;
    mUserLocked = FALSE;
    mShowingPublic = FALSE;
    mSensitive = FALSE;
    mShowingPublicInitialized = FALSE;
    mIsSystemExpanded = FALSE;
    mExpansionDisabled = FALSE;
    mPublicLayout->Reset();
    mPrivateLayout->Reset();
    ResetHeight();
    LogExpansionEvent(FALSE, wasExpanded);
    return NOERROR;
}

ECode CExpandableNotificationRow::ResetHeight()
{
    mMaxExpandHeight = 0;
    mWasReset = TRUE;
    OnHeightReset();
    RequestLayout();
    return NOERROR;
}

ECode CExpandableNotificationRow::OnFinishInflate()
{
    ActivatableNotificationView::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::expandedPublic, (IView**)&view);
    mPublicLayout = INotificationContentView::Probe(view);
    view = NULL;
    FindViewById(R::id::expanded, (IView**)&view);
    mPrivateLayout = INotificationContentView::Probe(view);
    view = NULL;
    FindViewById(R::id::notification_guts_stub, (IView**)&view);
    AutoPtr<IViewStub> gutsStub = IViewStub::Probe(view);

    AutoPtr<InflateListener> listener = new InflateListener(this);
    gutsStub->SetOnInflateListener(listener);

    FindViewById(R::id::veto, (IView**)&mVetoButton);
    return NOERROR;
}

ECode CExpandableNotificationRow::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    if (ActivatableNotificationView::OnRequestSendAccessibilityEvent(child, event, &tmp), tmp) {
        // Add a record for the entire layout since its content is somehow small.
        // The event comes from a leaf view that is interacted with.
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> record;
        helper->Obtain((IAccessibilityEvent**)&record);
        OnInitializeAccessibilityEvent(record);
        DispatchPopulateAccessibilityEvent(record, &tmp);
        event->AppendRecord(IAccessibilityRecord::Probe(record));
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetDark(
    /* [in] */ Boolean dark,
    /* [in] */ Boolean fade)
{
    ActivatableNotificationView::SetDark(dark, fade);
    AutoPtr<INotificationContentView> showing = GetShowingLayout();
    if (showing != NULL) {
        showing->SetDark(dark, fade);
    }
    return NOERROR;
}

ECode CExpandableNotificationRow::SetHeightRange(
    /* [in] */ Int32 rowMinHeight,
    /* [in] */ Int32 rowMaxHeight)
{
    mRowMinHeight = rowMinHeight;
    mRowMaxHeight = rowMaxHeight;
    return NOERROR;
}

ECode CExpandableNotificationRow::IsExpandable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExpandable;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetExpandable(
    /* [in] */ Boolean expandable)
{
    mExpandable = expandable;
    return NOERROR;
}

ECode CExpandableNotificationRow::HasUserChangedExpansion(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHasUserChangedExpansion;
    return NOERROR;
}

ECode CExpandableNotificationRow::IsUserExpanded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUserExpanded;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetUserExpanded(
    /* [in] */ Boolean userExpanded)
{
    if (userExpanded && !mExpandable) return NOERROR;
    Boolean wasExpanded = IsExpanded();
    mHasUserChangedExpansion = TRUE;
    mUserExpanded = userExpanded;
    LogExpansionEvent(TRUE, wasExpanded);
    return NOERROR;
}

ECode CExpandableNotificationRow::ResetUserExpansion()
{
    mHasUserChangedExpansion = FALSE;
    mUserExpanded = FALSE;
    return NOERROR;
}

ECode CExpandableNotificationRow::IsUserLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUserLocked;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetUserLocked(
    /* [in] */ Boolean userLocked)
{
    mUserLocked = userLocked;
    return NOERROR;
}

ECode CExpandableNotificationRow::IsSystemExpanded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsSystemExpanded;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetSystemExpanded(
    /* [in] */ Boolean expand)
{
    if (expand != mIsSystemExpanded) {
        Boolean wasExpanded = IsExpanded();
        mIsSystemExpanded = expand;
        NotifyHeightChanged();
        LogExpansionEvent(FALSE, wasExpanded);
    }
    return NOERROR;
}

ECode CExpandableNotificationRow::SetExpansionDisabled(
    /* [in] */ Boolean expansionDisabled)
{
    if (expansionDisabled != mExpansionDisabled) {
        Boolean wasExpanded = IsExpanded();
        mExpansionDisabled = expansionDisabled;
        LogExpansionEvent(FALSE, wasExpanded);
        if (wasExpanded != IsExpanded()) {
            NotifyHeightChanged();
        }
    }
    return NOERROR;
}

ECode CExpandableNotificationRow::IsClearable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mClearable;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetClearable(
    /* [in] */ Boolean clearable)
{
    mClearable = clearable;
    UpdateVetoButton();
    return NOERROR;
}

ECode CExpandableNotificationRow::ApplyExpansionToLayout()
{
    Boolean expand = IsExpanded();
    if (expand && mExpandable) {
        SetActualHeight(mMaxExpandHeight);
    }
    else {
        SetActualHeight(mRowMinHeight);
    }
    return NOERROR;
}

ECode CExpandableNotificationRow::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    Boolean tmp = FALSE;
    if (IsUserLocked(&tmp), tmp) {
        return GetActualHeight(height);
    }
    Boolean inExpansionState = IsExpanded();
    if (!inExpansionState) {
        // not expanded, so we return the collapsed size
        *height = mRowMinHeight;
        return NOERROR;
    }

    *height = mShowingPublicForIntrinsicHeight ? mRowMinHeight : (GetMaxExpandHeight(height), *height);
    return NOERROR;
}

Boolean CExpandableNotificationRow::IsExpanded()
{
    Boolean t1 = FALSE, t2 = FALSE, t3 = FALSE;
    return !mExpansionDisabled
            && (((HasUserChangedExpansion(&t1), !t1) && (IsSystemExpanded(&t2), t2))
            || (IsUserExpanded(&t3), t3));
}

ECode CExpandableNotificationRow::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    ActivatableNotificationView::OnLayout(changed, left, top, right, bottom);
    Boolean updateExpandHeight = mMaxExpandHeight == 0 && !mWasReset;
    UpdateMaxExpandHeight();
    if (updateExpandHeight) {
        ApplyExpansionToLayout();
    }
    mWasReset = FALSE;
    return NOERROR;
}

void CExpandableNotificationRow::UpdateMaxExpandHeight()
{
    Int32 intrinsicBefore = 0;
    GetIntrinsicHeight(&intrinsicBefore);
    mPrivateLayout->GetMaxHeight(&mMaxExpandHeight);
    Int32 tmp = 0;
    if (intrinsicBefore != (GetIntrinsicHeight(&tmp), tmp)) {
        NotifyHeightChanged();
    }
}

ECode CExpandableNotificationRow::SetSensitive(
    /* [in] */ Boolean sensitive)
{
    mSensitive = sensitive;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetHideSensitiveForIntrinsicHeight(
    /* [in] */ Boolean hideSensitive)
{
    mShowingPublicForIntrinsicHeight = mSensitive && hideSensitive;
    return NOERROR;
}

ECode CExpandableNotificationRow::SetHideSensitive(
    /* [in] */ Boolean hideSensitive,
    /* [in] */ Boolean animated,
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration)
{
    Boolean oldShowingPublic = mShowingPublic;
    mShowingPublic = mSensitive && hideSensitive;
    if (mShowingPublicInitialized && mShowingPublic == oldShowingPublic) {
        return NOERROR;
    }

    // bail out if no public version
    Int32 value = 0;
    if ((IViewGroup::Probe(mPublicLayout)->GetChildCount(&value), value) == 0) return NOERROR;

    if (!animated) {
        AutoPtr<IViewPropertyAnimator> anim;
        IView::Probe(mPublicLayout)->Animate((IViewPropertyAnimator**)&anim);
        anim->Cancel();

        anim = NULL;
        IView::Probe(mPrivateLayout)->Animate((IViewPropertyAnimator**)&anim);
        anim->Cancel();
        IView::Probe(mPublicLayout)->SetAlpha(1.f);
        IView::Probe(mPrivateLayout)->SetAlpha(1.f);
        IView::Probe(mPublicLayout)->SetVisibility(mShowingPublic ? IView::VISIBLE : IView::INVISIBLE);
        IView::Probe(mPrivateLayout)->SetVisibility(mShowingPublic ? IView::INVISIBLE : IView::VISIBLE);
    }
    else {
        AnimateShowingPublic(delay, duration);
    }

    UpdateVetoButton();
    mShowingPublicInitialized = TRUE;
    return NOERROR;
}

void CExpandableNotificationRow::AnimateShowingPublic(
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration)
{
    AutoPtr<IView> source = mShowingPublic ? IView::Probe(mPrivateLayout) : IView::Probe(mPublicLayout);
    AutoPtr<IView> target = mShowingPublic ? IView::Probe(mPublicLayout) : IView::Probe(mPrivateLayout);
    source->SetVisibility(IView::VISIBLE);
    target->SetVisibility(IView::VISIBLE);
    target->SetAlpha(0.f);

    AutoPtr<IViewPropertyAnimator> anim;
    source->Animate((IViewPropertyAnimator**)&anim);
    anim->Cancel();

    anim = NULL;
    target->Animate((IViewPropertyAnimator**)&anim);
    anim->Cancel();

    anim = NULL;
    source->Animate((IViewPropertyAnimator**)&anim);
    anim->Alpha(0.f);
    anim->SetStartDelay(delay);
    anim->SetDuration(duration);
    AutoPtr<Runnable1> run = new Runnable1(source);
    anim->WithEndAction(run);

    anim = NULL;
    target->Animate((IViewPropertyAnimator**)&anim);
    anim->Alpha(1.f);
    anim->SetStartDelay(delay);
    anim->SetDuration(duration);
}

void CExpandableNotificationRow::UpdateVetoButton()
{
    // public versions cannot be dismissed
    Boolean tmp = FALSE;
    IsClearable(&tmp);
    mVetoButton->SetVisibility(tmp && !mShowingPublic ? IView::VISIBLE : IView::GONE);
}

ECode CExpandableNotificationRow::GetMaxExpandHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mShowingPublicForIntrinsicHeight ? mRowMinHeight : mMaxExpandHeight;
    return NOERROR;
}

ECode CExpandableNotificationRow::IsContentExpandable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<INotificationContentView> showingLayout = GetShowingLayout();
    return showingLayout->IsContentExpandable(result);
}

ECode CExpandableNotificationRow::SetActualHeight(
    /* [in] */ Int32 height,
    /* [in] */ Boolean notifyListeners)
{
    mPrivateLayout->SetActualHeight(height);
    mPublicLayout->SetActualHeight(height);
    if (mGuts != NULL) {
        mGuts->SetActualHeight(height);
    }
    Invalidate();
    ActivatableNotificationView::SetActualHeight(height, notifyListeners);
    return NOERROR;
}

ECode CExpandableNotificationRow::GetMaxHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    AutoPtr<INotificationContentView> showingLayout = GetShowingLayout();
    return showingLayout->GetMaxHeight(height);
}

ECode CExpandableNotificationRow::GetMinHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    AutoPtr<INotificationContentView> showingLayout = GetShowingLayout();
    return showingLayout->GetMinHeight(height);
}

ECode CExpandableNotificationRow::SetClipTopAmount(
    /* [in] */ Int32 clipTopAmount)
{
    ActivatableNotificationView::SetClipTopAmount(clipTopAmount);
    mPrivateLayout->SetClipTopAmount(clipTopAmount);
    mPublicLayout->SetClipTopAmount(clipTopAmount);
    if (mGuts != NULL) {
        mGuts->SetClipTopAmount(clipTopAmount);
    }
    return NOERROR;
}

ECode CExpandableNotificationRow::NotifyContentUpdated()
{
    mPublicLayout->NotifyContentUpdated();
    mPrivateLayout->NotifyContentUpdated();
    return NOERROR;
}

ECode CExpandableNotificationRow::IsMaxExpandHeightInitialized(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMaxExpandHeight != 0;
    return NOERROR;
}

AutoPtr<INotificationContentView> CExpandableNotificationRow::GetShowingLayout()
{
    return mShowingPublic ? mPublicLayout : mPrivateLayout;
}

ECode CExpandableNotificationRow::SetExpansionLogger(
    /* [in] */ IExpansionLogger* logger,
    /* [in] */ const String& key)
{
    mLogger = logger;
    mLoggingKey = key;
    return NOERROR;
}

ECode CExpandableNotificationRow::LogExpansionEvent(
    /* [in] */ Boolean userAction,
    /* [in] */ Boolean wasExpanded)
{
    Boolean nowExpanded = IsExpanded();
    if (wasExpanded != nowExpanded && mLogger != NULL) {
        mLogger->LogNotificationExpansion(mLoggingKey, userAction, nowExpanded) ;
    }
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
