//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/statusbar/phone/CNotificationPanelView.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBarView.h"
#include "elastos/droid/systemui/statusbar/phone/ScrimController.h"
#include "elastos/droid/systemui/statusbar/phone/SecureCameraLaunchManager.h"
#include "elastos/droid/systemui/statusbar/BaseStatusBar.h"
#include "elastos/droid/systemui/keyguard/CKeyguardStatusView.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "../R.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/utility/MathUtils.h>
#include <elastos/droid/R.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::SystemUI::Keyguard::CKeyguardStatusView;
using Elastos::Droid::SystemUI::StatusBar::BaseStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Policy::IScrollAdapter;
using Elastos::Droid::SystemUI::StatusBar::Stack::EIID_IOnOverscrollTopChangedListener;
using Elastos::Droid::SystemUI::StatusBar::Stack::IStackStateAnimator;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

static const String TAG("CNotificationPanelView");

CNotificationPanelView::Runnable1::Runnable1(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::Runnable1::Run()
{
    mHost->mKeyguardStatusViewAnimating = FALSE;
    IView::Probe(mHost->mKeyguardStatusView)->SetVisibility(IView::GONE);
    return NOERROR;
}

CNotificationPanelView::Runnable2::Runnable2(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::Runnable2::Run()
{
    mHost->mKeyguardStatusViewAnimating = FALSE;
    return NOERROR;
}

CNotificationPanelView::AnimatorListenerAdapter1::AnimatorListenerAdapter1(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::AnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mHeaderAnimatingIn = FALSE;
    mHost->mQsContainerAnimator = NULL;
    mHost->mQsContainer->RemoveOnLayoutChangeListener(mHost->mQsContainerAnimatorUpdater);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNotificationPanelView::QsContainerAnimatorUpdaterListener, Object, IViewOnLayoutChangeListener)

CNotificationPanelView::QsContainerAnimatorUpdaterListener::QsContainerAnimatorUpdaterListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::QsContainerAnimatorUpdaterListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    Int32 oldHeight = oldBottom - oldTop;
    Int32 height = bottom - top;
    if (height != oldHeight && mHost->mQsContainerAnimator != NULL) {
        IValueAnimator* va = IValueAnimator::Probe(mHost->mQsContainerAnimator);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > values;
        va->GetValues((ArrayOf<IPropertyValuesHolder*>**)&values);
        Int32 h = 0;
        mHost->mHeader->GetCollapsedHeight(&h);
        Float newEndValue = h + mHost->mQsPeekHeight - height - top;
        Float newStartValue = -height - top;
        AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
        (*fvs)[0] = newStartValue;
        (*fvs)[1] = newEndValue;
        (*values)[0]->SetFloatValues(fvs);
        Int64 t = 0;
        va->GetCurrentPlayTime(&t);
        va->SetCurrentPlayTime(t);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNotificationPanelView::OnPreDrawListener, Object, IOnPreDrawListener)

CNotificationPanelView::OnPreDrawListener::OnPreDrawListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::OnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IViewTreeObserver> vto;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(this);
    Int32 h = 0;
    mHost->mHeader->GetCollapsedHeight(&h);
    IView::Probe(mHost->mHeader)->SetTranslationY(- h - mHost->mQsPeekHeight);
    AutoPtr<IViewPropertyAnimator> vpa;
    IView::Probe(mHost->mHeader)->Animate((IViewPropertyAnimator**)&vpa);
    vpa->TranslationY(0.f);
    Int64 t = 0;
    mHost->mStatusBar->CalculateGoingToFullShadeDelay(&t);
    vpa->SetStartDelay(t);
    vpa->SetDuration(IStackStateAnimator::ANIMATION_DURATION_GO_TO_FULL_SHADE);
    vpa->SetInterpolator(ITimeInterpolator::Probe(mHost->mFastOutSlowInInterpolator));
    vpa->Start();
    AutoPtr<IView> mQsContainer = mHost->mQsContainer;
    Int32 tmp = 0;
    mQsContainer->GetHeight(&tmp);
    mQsContainer->SetY(-tmp);

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);

    Float y = 0;
    mQsContainer->GetTranslationY(&y);
    Int32 ch = 0, top = 0;
    mHost->mHeader->GetCollapsedHeight(&ch);
    mQsContainer->GetTop(&top);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = y;
    (*fvs)[1] = ch + mHost->mQsPeekHeight - tmp - top;
    mHost->mQsContainerAnimator = NULL;
    helper->OfFloat(mQsContainer, View::TRANSLATION_Y, fvs, (IObjectAnimator**)&mHost->mQsContainerAnimator);

    mHost->mStatusBar->CalculateGoingToFullShadeDelay(&t);
    IAnimator* animator = IAnimator::Probe(mHost->mQsContainerAnimator);
    animator->SetStartDelay(t);
    animator->SetDuration(IStackStateAnimator::ANIMATION_DURATION_GO_TO_FULL_SHADE);
    animator->SetInterpolator(ITimeInterpolator::Probe(mHost->mFastOutSlowInInterpolator));
    animator->AddListener(mHost->mAnimateHeaderSlidingInListener);
    animator->Start();
    mQsContainer->AddOnLayoutChangeListener(mHost->mQsContainerAnimatorUpdater);
    *result = TRUE;
    return NOERROR;
}

CNotificationPanelView::KeyguardStatusBarVisibilityRunnable::KeyguardStatusBarVisibilityRunnable(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::KeyguardStatusBarVisibilityRunnable::Run()
{
    IView::Probe(mHost->mKeyguardStatusBar)->SetVisibility(IView::INVISIBLE);
    return NOERROR;
}

CNotificationPanelView::KeyguardBottomAreaVisibilityRunnable::KeyguardBottomAreaVisibilityRunnable(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::KeyguardBottomAreaVisibilityRunnable::Run()
{
    IView::Probe(mHost->mKeyguardBottomArea)->SetVisibility(IView::GONE);
    return NOERROR;
}

CNotificationPanelView::HeaderUpdateEverythingRunnable::HeaderUpdateEverythingRunnable(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::HeaderUpdateEverythingRunnable::Run()
{
    mHost->mHeader->UpdateEverything();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNotificationPanelView::ViewOnClickListener, Object, IViewOnClickListener)

CNotificationPanelView::ViewOnClickListener::ViewOnClickListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::ViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

CAR_INTERFACE_IMPL(CNotificationPanelView::ScrollViewListener, Object, IObservableScrollViewListener)

CNotificationPanelView::ScrollViewListener::ScrollViewListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::ScrollViewListener::OnScrollChanged()
{
    return mHost->OnScrollChanged();
}

ECode CNotificationPanelView::ScrollViewListener::OnOverscrolled(
    /* [in] */ Float lastX,
    /* [in] */ Float lastY,
    /* [in] */ Int32 amount)
{
    return mHost->OnOverscrolled(lastX, lastY, amount);
}

CAR_INTERFACE_IMPL(CNotificationPanelView::HeightChangedListener, Object, IExpandableViewOnHeightChangedListener)

CNotificationPanelView::HeightChangedListener::HeightChangedListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::HeightChangedListener::OnHeightChanged(
    /* [in] */ IExpandableView* view)
{
    return mHost->OnHeightChanged(view);
}

ECode CNotificationPanelView::HeightChangedListener::OnReset(
    /* [in] */ IExpandableView* view)
{
    return mHost->OnReset(view);
}

CAR_INTERFACE_IMPL(CNotificationPanelView::TopChangedListener, Object, IOnOverscrollTopChangedListener)

CNotificationPanelView::TopChangedListener::TopChangedListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::TopChangedListener::OnOverscrollTopChanged(
    /* [in] */ Float amount,
    /* [in] */ Boolean isRubberbanded)
{
    return mHost->OnOverscrollTopChanged(amount, isRubberbanded);
}

ECode CNotificationPanelView::TopChangedListener::FlingTopOverscroll(
    /* [in] */ Float velocity,
    /* [in] */ Boolean open)
{
    return mHost->FlingTopOverscroll(velocity, open);
}

CAR_INTERFACE_IMPL(CNotificationPanelView::QsContainerOnLayoutChangeListener, Object, IViewOnLayoutChangeListener)

CNotificationPanelView::QsContainerOnLayoutChangeListener::QsContainerOnLayoutChangeListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::QsContainerOnLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    const Int32 height = bottom - top;
    const Int32 oldHeight = oldBottom - oldTop;
    if (height != oldHeight) {
        mHost->OnScrollChanged();
    }
    return NOERROR;
}

CNotificationPanelView::OnPreDrawListener2::AnimatorListenerAdapter2::AnimatorListenerAdapter2(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::OnPreDrawListener2::AnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mClockAnimator = NULL;
    mHost->mClockAnimationTarget = -1;
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNotificationPanelView::OnPreDrawListener2, Object, IOnPreDrawListener)

CNotificationPanelView::OnPreDrawListener2::OnPreDrawListener2(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::OnPreDrawListener2::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IViewTreeObserver> vto;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(this);
    if (mHost->mClockAnimator != NULL) {
        IAnimator::Probe(mHost->mClockAnimator)->RemoveAllListeners();
        IAnimator::Probe(mHost->mClockAnimator)->Cancel();
    }

    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(1);
    (*fvs)[0] = mHost->mClockAnimationTarget;

    mHost->mClockAnimator = NULL;
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    helper->OfFloat(mHost->mKeyguardStatusView, View::Y, fvs, (IObjectAnimator**)&mHost->mClockAnimator);
    IAnimator* animator = IAnimator::Probe(mHost->mClockAnimator);
    animator->SetInterpolator(ITimeInterpolator::Probe(mHost->mFastOutSlowInInterpolator));
    animator->SetDuration(IStackStateAnimator::ANIMATION_DURATION_STANDARD);
    AutoPtr<AnimatorListenerAdapter2> listener = new AnimatorListenerAdapter2(mHost);
    animator->AddListener(listener);
    animator->Start();
    *result = TRUE;
    return NOERROR;
}

CNotificationPanelView::Runnable6::Runnable6(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::Runnable6::Run()
{
    mHost->mStackScrollerOverscrolling = FALSE;
    mHost->mQsExpansionFromOverscroll = FALSE;
    mHost->UpdateQsState();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNotificationPanelView::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

CNotificationPanelView::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Float v = 0;
    IFloat::Probe(obj)->GetValue(&v);
    mHost->SetQsExpansion(v);
    return NOERROR;
}

CNotificationPanelView::AnimatorListenerAdapter3::AnimatorListenerAdapter3(
    /* [in] */ CNotificationPanelView* host,
    /* [in] */ IRunnable* onFinishRunnable)
    : mHost(host)
    , mOnFinishRunnable(onFinishRunnable)
{}

ECode CNotificationPanelView::AnimatorListenerAdapter3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mScrollView->SetBlockFlinging(FALSE);
    mHost->mScrollYOverride = -1;
    mHost->mQsExpansionAnimator = NULL;
    if (mOnFinishRunnable != NULL) {
        mOnFinishRunnable->Run();
    }
    return NOERROR;
}

CNotificationPanelView::Runnable7::Runnable7(
    /* [in] */ CNotificationPanelView* host)
    : mHost(host)
{}

ECode CNotificationPanelView::Runnable7::Run()
{
    mHost->mHintAnimationRunning = FALSE;
    mHost->mStatusBar->OnHintFinished();
    return NOERROR;
}

CNotificationPanelView::Runnable8::Runnable8(
    /* [in] */ CNotificationPanelView* host,
    /* [in] */ IKeyguardAffordanceView* icon)
    : mHost(host)
    , mIcon(icon)
{}

ECode CNotificationPanelView::Runnable8::Run()
{
    mIcon->SetImageAlpha(KeyguardAffordanceHelper::SWIPE_RESTING_ALPHA_AMOUNT,
            TRUE, KeyguardAffordanceHelper::HINT_PHASE1_DURATION,
            mHost->mFastOutSlowInInterpolator, NULL);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNotificationPanelView::SetBgColorAlphaAnimatorUpdateListener, Object, IAnimatorUpdateListener)

CNotificationPanelView::SetBgColorAlphaAnimatorUpdateListener::SetBgColorAlphaAnimatorUpdateListener(
    /* [in] */ IView* target,
    /* [in] */ Int32 r,
    /* [in] */ Int32 g,
    /* [in] */ Int32 b)
    : mTarget(target)
    , mR(r)
    , mG(g)
    , mB(b)
{}

ECode CNotificationPanelView::SetBgColorAlphaAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Int32 value = 0;
    IInteger32::Probe(obj)->GetValue(&value);

    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 argb = 0;
    color->Argb(value, mR, mG, mB, &argb);
    mTarget->SetBackgroundColor(argb);
    return NOERROR;
}

CNotificationPanelView::SetBgColorAlphaAnimatorListenerAdapter::SetBgColorAlphaAnimatorListenerAdapter(
    /* [in] */ IView* target)
    : mTarget(target)
{}

ECode CNotificationPanelView::SetBgColorAlphaAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mTarget->SetTag(TAG_KEY_ANIM, NULL);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNotificationPanelView::AffordanceHelperCallback, Object, IKeyguardAffordanceHelperCallback)

const Int32 CNotificationPanelView::CAP_HEIGHT = 1456;
const Int32 CNotificationPanelView::FONT_HEIGHT = 2163;
const Float CNotificationPanelView::HEADER_RUBBERBAND_FACTOR = 2.05f;
const Float CNotificationPanelView::LOCK_ICON_ACTIVE_SCALE = 1.2f;
const Int32 CNotificationPanelView::DOZE_BACKGROUND_COLOR = 0xff000000;
const Int32 CNotificationPanelView::TAG_KEY_ANIM = R::id::scrim;
const Int64 CNotificationPanelView::DOZE_BACKGROUND_ANIM_DURATION = ScrimController::ANIMATION_DURATION;

CAR_OBJECT_IMPL(CNotificationPanelView)

CAR_INTERFACE_IMPL(CNotificationPanelView, PanelView, INotificationPanelView);

CNotificationPanelView::CNotificationPanelView()
    : mNotificationTopPadding(0)
    , mAnimateNextTopPaddingChange(FALSE)
    , mTrackingPointer(0)
    , mQsTracking(FALSE)
    , mConflictingQsExpansionGesture(FALSE)
    , mIntercepting(FALSE)
    , mQsExpanded(FALSE)
    , mQsExpandedWhenExpandingStarted(FALSE)
    , mQsFullyExpanded(FALSE)
    , mKeyguardShowing(FALSE)
    , mDozing(FALSE)
    , mStatusBarState(0)
    , mInitialHeightOnTouch(0)
    , mInitialTouchX(0)
    , mInitialTouchY(0)
    , mLastTouchX(0)
    , mLastTouchY(0)
    , mQsExpansionHeight(0)
    , mQsMinExpansionHeight(0)
    , mQsMaxExpansionHeight(0)
    , mQsPeekHeight(0)
    , mStackScrollerOverscrolling(FALSE)
    , mQsExpansionFromOverscroll(FALSE)
    , mLastOverscroll(0)
    , mQsExpansionEnabled(TRUE)
    , mStatusBarMinHeight(0)
    , mUnlockIconActive(FALSE)
    , mNotificationsHeaderCollideDistance(0)
    , mUnlockMoveDistance(0)
    , mEmptyDragAmount(0)
    , mClockAnimationTarget(-1)
    , mTopPaddingAdjustment(0)
    , mIsExpanding(FALSE)
    , mBlockTouches(FALSE)
    , mNotificationScrimWaitDistance(0)
    , mTwoFingerQsExpand(FALSE)
    , mTwoFingerQsExpandPossible(FALSE)
    , mScrollYOverride(-1)
    , mQsAnimatorExpand(FALSE)
    , mIsLaunchTransitionFinished(FALSE)
    , mIsLaunchTransitionRunning(FALSE)
    , mOnlyAffordanceInThisMotion(FALSE)
    , mKeyguardStatusViewAnimating(FALSE)
    , mHeaderAnimatingIn(FALSE)
    , mShadeEmpty(FALSE)
    , mQsScrimEnabled(TRUE)
    , mLastAnnouncementWasQuickSettings(FALSE)
    , mQsTouchAboveFalsingThreshold(FALSE)
    , mQsFalsingThreshold(0)
{
}

ECode CNotificationPanelView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    mAnimateKeyguardStatusViewInvisibleEndRunnable = new Runnable1(this);
    mAnimateKeyguardStatusViewVisibleEndRunnable = new Runnable2(this);
    mAnimateHeaderSlidingInListener = new AnimatorListenerAdapter1(this);
    mQsContainerAnimatorUpdater = new QsContainerAnimatorUpdaterListener(this);
    mStartHeaderSlidingIn = new OnPreDrawListener(this);
    mAnimateKeyguardStatusBarInvisibleEndRunnable = new KeyguardStatusBarVisibilityRunnable(this);
    mAnimateKeyguardBottomAreaInvisibleEndRunnable = new KeyguardBottomAreaVisibilityRunnable(this);
    mUpdateHeader = new HeaderUpdateEverythingRunnable(this);
    mClockPositionAlgorithm = new KeyguardClockPositionAlgorithm();
    mClockPositionResult = new KeyguardClockPositionAlgorithm::Result();

    return PanelView::constructor(context, attrs);
}

ECode CNotificationPanelView::SetStatusBar(
    /* [in] */ IPhoneStatusBar* bar)
{
    mStatusBar = bar;
    return NOERROR;
}

ECode CNotificationPanelView::OnFinishInflate()
{
    PanelView::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::header, (IView**)&view);
    mHeader = IStatusBarHeaderView::Probe(view);

    AutoPtr<ViewOnClickListener> listener = new ViewOnClickListener(this);
    IView::Probe(mHeader)->SetOnClickListener(listener);

    view = NULL;
    FindViewById(R::id::keyguard_header, (IView**)&view);
    mKeyguardStatusBar = IKeyguardStatusBarView::Probe(view);

    view = NULL;
    FindViewById(R::id::keyguard_status_view, (IView**)&view);
    mKeyguardStatusView = IGridLayout::Probe(view);
    FindViewById(R::id::quick_settings_container, (IView**)&mQsContainer);

    view = NULL;
    FindViewById(R::id::quick_settings_panel, (IView**)&view);
    mQsPanel = IQSPanel::Probe(view);

    view = NULL;
    FindViewById(R::id::clock_view, (IView**)&view);
    mClockView = ITextView::Probe(view);

    view = NULL;
    FindViewById(R::id::scroll_view, (IView**)&view);
    mScrollView = IObservableScrollView::Probe(view);

    AutoPtr<ScrollViewListener> l = new ScrollViewListener(this);
    mScrollView->SetListener(l);
    IView::Probe(mScrollView)->SetFocusable(FALSE);
    FindViewById(R::id::reserve_notification_space, (IView**)&mReserveNotificationSpace);
    FindViewById(R::id::notification_container_parent, (IView**)&mNotificationContainerParent);

    view = NULL;
    FindViewById(R::id::notification_stack_scroller, (IView**)&view);
    mNotificationStackScroller = INotificationStackScrollLayout::Probe(view);

    AutoPtr<HeightChangedListener> hl = new HeightChangedListener(this);
    mNotificationStackScroller->SetOnHeightChangedListener(hl);

    AutoPtr<TopChangedListener> tl = new TopChangedListener(this);
    mNotificationStackScroller->SetOverscrollTopChangedListener(tl);
    mNotificationStackScroller->SetScrollView(IViewGroup::Probe(mScrollView));

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AnimationUtils::LoadInterpolator(ctx, Elastos::Droid::R::interpolator::fast_out_slow_in,
            (IInterpolator**)&mFastOutSlowInInterpolator);
    AnimationUtils::LoadInterpolator(ctx, Elastos::Droid::R::interpolator::fast_out_linear_in,
            (IInterpolator**)&mFastOutLinearInterpolator);

    view = NULL;
    FindViewById(R::id::keyguard_bottom_area, (IView**)&view);
    mKeyguardBottomArea = IKeyguardBottomAreaView::Probe(view);
    FindViewById(R::id::qs_navbar_scrim, (IView**)&mQsNavbarScrim);
    AutoPtr<AffordanceHelperCallback> cb = new AffordanceHelperCallback(this);
    mAfforanceHelper = new KeyguardAffordanceHelper(cb, ctx);

    mSecureCameraLaunchManager = new SecureCameraLaunchManager(ctx, mKeyguardBottomArea);

    // recompute internal state when qspanel height changes
    AutoPtr<QsContainerOnLayoutChangeListener> lcl = new QsContainerOnLayoutChangeListener(this);
    mQsContainer->AddOnLayoutChangeListener(lcl);
    return NOERROR;
}

void CNotificationPanelView::LoadDimens()
{
    PanelView::LoadDimens();
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    res->GetDimensionPixelSize(R::dimen::notifications_top_padding, &mNotificationTopPadding);
    mFlingAnimationUtils = new FlingAnimationUtils(ctx, 0.4f);
    res->GetDimensionPixelSize(Elastos::Droid::R::dimen::status_bar_height, &mStatusBarMinHeight);
    res->GetDimensionPixelSize(R::dimen::qs_peek_height, &mQsPeekHeight);
    res->GetDimensionPixelSize(R::dimen::header_notifications_collide_distance, &mNotificationsHeaderCollideDistance);
    res->GetDimensionPixelOffset(R::dimen::unlock_move_distance, &mUnlockMoveDistance);
    mClockPositionAlgorithm->LoadDimens(res);
    res->GetDimensionPixelSize(R::dimen::notification_scrim_wait_distance, &mNotificationScrimWaitDistance);
    res->GetDimensionPixelSize(R::dimen::qs_falsing_threshold, &mQsFalsingThreshold);
}

ECode CNotificationPanelView::UpdateResources()
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 panelWidth = 0;
    res->GetDimensionPixelSize(R::dimen::notification_panel_width, &panelWidth);
    Int32 panelGravity = 0;
    res->GetInteger(R::integer::notification_panel_layout_gravity, &panelGravity);

    AutoPtr<IViewGroupLayoutParams> lp;
    IView::Probe(mHeader)->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 width = 0;
    lp->GetWidth(&width);
    if (width != panelWidth) {
        lp->SetWidth(panelWidth);
        IFrameLayoutLayoutParams::Probe(lp)->SetGravity(panelGravity);
        IView::Probe(mHeader)->SetLayoutParams(lp);
        Boolean tmp = FALSE;
        IView::Probe(mHeader)->Post(mUpdateHeader, &tmp);
    }

    lp = NULL;
    IView::Probe(mNotificationStackScroller)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    lp->GetWidth(&width);
    if (width != panelWidth) {
        lp->SetWidth(panelWidth);
        IFrameLayoutLayoutParams::Probe(lp)->SetGravity(panelGravity);
        IView::Probe(mNotificationStackScroller)->SetLayoutParams(lp);
    }

    lp = NULL;
    IView::Probe(mScrollView)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    lp->GetWidth(&width);
    if (width != panelWidth) {
        lp->SetWidth(panelWidth);
        IFrameLayoutLayoutParams::Probe(lp)->SetGravity(panelGravity);
        IView::Probe(mScrollView)->SetLayoutParams(lp);
    }
    return NOERROR;
}

ECode CNotificationPanelView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    PanelView::OnLayout(changed, left, top, right, bottom);

    // Update Clock Pivot
    Int32 w = 0;
    GetWidth(&w);
    IView::Probe(mKeyguardStatusView)->SetPivotX(w / 2);
    Float size = 0;
    mClockView->GetTextSize(&size);
    IView::Probe(mKeyguardStatusView)->SetPivotY((FONT_HEIGHT - CAP_HEIGHT) / 2048.f * size);

    // Calculate quick setting heights.
    Int32 h = 0, h2 = 0;
    mQsMinExpansionHeight = mKeyguardShowing ? 0 : (mHeader->GetCollapsedHeight(&h), h) + mQsPeekHeight;
    mQsMaxExpansionHeight = (mHeader->GetExpandedHeight(&h), h) + (mQsContainer->GetHeight(&h2), h2);
    PositionClockAndNotifications();
    if (mQsExpanded) {
        if (mQsFullyExpanded) {
            mQsExpansionHeight = mQsMaxExpansionHeight;
            RequestScrollerTopPaddingUpdate(FALSE /* animate */);
        }
    }
    else {
        SetQsExpansion(mQsMinExpansionHeight + mLastOverscroll);
        Float eh = 0;
        GetExpandedHeight(&eh);
        mNotificationStackScroller->SetStackHeight(eh);
        UpdateHeader();
    }
    mHeader->GetCollapsedHeight(&h);
    mNotificationStackScroller->UpdateIsSmallScreen(h + mQsPeekHeight);
    return NOERROR;
}

ECode CNotificationPanelView::OnAttachedToWindow()
{
    mSecureCameraLaunchManager->Create();
    return NOERROR;
}

ECode CNotificationPanelView::OnDetachedFromWindow()
{
    mSecureCameraLaunchManager->Destroy();
    return NOERROR;
}

void CNotificationPanelView::PositionClockAndNotifications()
{
    Boolean animate = FALSE;
    mNotificationStackScroller->IsAddOrRemoveAnimationPending(&animate);
    Int32 stackScrollerPadding;
    if (mStatusBarState != IStatusBarState::KEYGUARD) {
        Int32 bottom = 0, h = 0;
        mHeader->GetCollapsedHeight(&bottom);
        stackScrollerPadding = mStatusBarState == IStatusBarState::SHADE
                ? bottom + mQsPeekHeight + mNotificationTopPadding
                : (IView::Probe(mKeyguardStatusBar)->GetHeight(&h), h) + mNotificationTopPadding;
        mTopPaddingAdjustment = 0;
    }
    else {
        Int32 value = 0, h = 0, h2 = 0;
        mNotificationStackScroller->GetNotGoneChildCount(&value);
        GetHeight(&h);
        IView::Probe(mKeyguardStatusView)->GetHeight(&h2);
        Float fv = 0;
        mClockPositionAlgorithm->Setup(
                ((BaseStatusBar*)IBaseStatusBar::Probe(mStatusBar))->GetMaxKeyguardNotifications(),
                GetMaxPanelHeight(),
                (GetExpandedHeight(&fv), fv),
                value,
                h,
                h2,
                mEmptyDragAmount);
        mClockPositionAlgorithm->Run(mClockPositionResult);
        if (animate || mClockAnimator != NULL) {
            StartClockAnimation(mClockPositionResult->mClockY);
        }
        else {
            IView::Probe(mKeyguardStatusView)->SetY(mClockPositionResult->mClockY);
        }
        UpdateClock(mClockPositionResult->mClockAlpha, mClockPositionResult->mClockScale);
        stackScrollerPadding = mClockPositionResult->mStackScrollerPadding;
        mTopPaddingAdjustment = mClockPositionResult->mStackScrollerPaddingAdjustment;
    }

    mNotificationStackScroller->SetIntrinsicPadding(stackScrollerPadding);
    RequestScrollerTopPaddingUpdate(animate);
}

void CNotificationPanelView::StartClockAnimation(
    /* [in] */ Int32 y)
{
    if (mClockAnimationTarget == y) {
        return;
    }
    mClockAnimationTarget = y;
    AutoPtr<IViewTreeObserver> vto;
    GetViewTreeObserver((IViewTreeObserver**)&vto);
    AutoPtr<OnPreDrawListener2> l2 = new OnPreDrawListener2(this);
    vto->AddOnPreDrawListener(l2);
}

void CNotificationPanelView::UpdateClock(
    /* [in] */ Float alpha,
    /* [in] */ Float scale)
{
    if (!mKeyguardStatusViewAnimating) {
        IView::Probe(mKeyguardStatusView)->SetAlpha(alpha);
    }
    IView::Probe(mKeyguardStatusView)->SetScaleX(scale);
    IView::Probe(mKeyguardStatusView)->SetScaleY(scale);
}

ECode CNotificationPanelView::AnimateToFullShade(
    /* [in] */ Int64 delay)
{
    mAnimateNextTopPaddingChange = TRUE;
    mNotificationStackScroller->GoToFullShade(delay);
    RequestLayout();
    return NOERROR;
}

ECode CNotificationPanelView::SetQsExpansionEnabled(
    /* [in] */ Boolean qsExpansionEnabled)
{
    mQsExpansionEnabled = qsExpansionEnabled;
    IView::Probe(mHeader)->SetClickable(qsExpansionEnabled);
    return NOERROR;
}

ECode CNotificationPanelView::ResetViews()
{
    mIsLaunchTransitionFinished = FALSE;
    mBlockTouches = FALSE;
    mUnlockIconActive = FALSE;
    mAfforanceHelper->Reset(TRUE);
    CloseQs();
    IBaseStatusBar::Probe(mStatusBar)->DismissPopups();
    mNotificationStackScroller->SetOverScrollAmount(0.f, TRUE /* onTop */, FALSE /* animate */,
            TRUE /* cancelAnimators */);
    return NOERROR;
}

ECode CNotificationPanelView::CloseQs()
{
    CancelAnimation();
    SetQsExpansion(mQsMinExpansionHeight);
    return NOERROR;
}

ECode CNotificationPanelView::AnimateCloseQs()
{
    if (mQsExpansionAnimator != NULL) {
        if (!mQsAnimatorExpand) {
            return NOERROR;
        }
        Float height = mQsExpansionHeight;
        IAnimator::Probe(mQsExpansionAnimator)->Cancel();
        SetQsExpansion(height);
    }
    FlingSettings(0 /* vel */, FALSE);
    return NOERROR;
}

ECode CNotificationPanelView::OpenQs()
{
    CancelAnimation();
    if (mQsExpansionEnabled) {
        SetQsExpansion(mQsMaxExpansionHeight);
    }
    return NOERROR;
}

void CNotificationPanelView::Fling(
    /* [in] */ Float vel,
    /* [in] */ Boolean expand)
{
    AutoPtr<IGestureRecorder> gr;
    ((CPhoneStatusBarView*) mBar.Get())->mBar->GetGestureRecorder((IGestureRecorder**)&gr);
    if (gr != NULL) {
        gr->Tag(String("fling ") + ((vel > 0) ? String("open") : String("closed"))
                , String("notifications,v=") + StringUtils::ToString(vel));
    }
    PanelView::Fling(vel, expand);
}

ECode CNotificationPanelView::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 type = 0;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED) {
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);

        AutoPtr<ICharSequence> value;
        CString::New(GetKeyguardOrLockScreenString(), (ICharSequence**)&value);
        list->Add(value);
        mLastAnnouncementWasQuickSettings = FALSE;
        *result = TRUE;
        return NOERROR;
    }

    return PanelView::DispatchPopulateAccessibilityEvent(event, result);
}

ECode CNotificationPanelView::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mBlockTouches) {
        *result = FALSE;
        return NOERROR;
    }
    ResetDownStates(event);
    Int32 pointerIndex = 0;
    event->FindPointerIndex(mTrackingPointer, &pointerIndex);
    if (pointerIndex < 0) {
        pointerIndex = 0;
        event->GetPointerId(pointerIndex, &mTrackingPointer);
    }
    Float x, y;
    event->GetX(pointerIndex, &x);
    event->GetY(pointerIndex, &y);

    Int32 masked = 0;
    event->GetActionMasked(&masked);
    switch (masked) {
        case IMotionEvent::ACTION_DOWN:
            mIntercepting = TRUE;
            mInitialTouchY = y;
            mInitialTouchX = x;
            InitVelocityTracker();
            TrackMovement(event);
            if (ShouldQuickSettingsIntercept(mInitialTouchX, mInitialTouchY, 0)) {
                AutoPtr<IViewParent> vp;
                GetParent((IViewParent**)&vp);
                vp->RequestDisallowInterceptTouchEvent(TRUE);
            }
            if (mQsExpansionAnimator != NULL) {
                OnQsExpansionStarted();
                mInitialHeightOnTouch = mQsExpansionHeight;
                mQsTracking = TRUE;
                mIntercepting = FALSE;
                mNotificationStackScroller->RemoveLongPressCallback();
            }
            break;
        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 v = 0, upPointer;
            event->GetActionIndex(&v);
            event->GetPointerId(v, &upPointer);
            if (mTrackingPointer == upPointer) {
                // gesture is ongoing, find a new pointer to track
                const Int32 newIndex = (event->GetPointerId(0, &v), v) != upPointer ? 0 : 1;
                event->GetPointerId(newIndex, &mTrackingPointer);
                event->GetX(newIndex, &mInitialTouchX);
                event->GetY(newIndex, &mInitialTouchY);
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            const Float h = y - mInitialTouchY;
            TrackMovement(event);
            if (mQsTracking) {
                // Already tracking because onOverscrolled was called. We need to update here
                // so we don't stop for a frame until the next touch event gets handled in
                // onTouchEvent.
                SetQsExpansion(h + mInitialHeightOnTouch);
                TrackMovement(event);
                mIntercepting = FALSE;
                *result = TRUE;
                return NOERROR;
            }
            using Elastos::Core::Math;
            if (Math::Abs(h) > mTouchSlop && Math::Abs(h) > Math::Abs(x - mInitialTouchX)
                && ShouldQuickSettingsIntercept(mInitialTouchX, mInitialTouchY, h)) {
                OnQsExpansionStarted();
                mInitialHeightOnTouch = mQsExpansionHeight;
                mInitialTouchY = y;
                mInitialTouchX = x;
                mQsTracking = TRUE;
                mIntercepting = FALSE;
                mNotificationStackScroller->RemoveLongPressCallback();
                *result = TRUE;
                return NOERROR;
            }
            break;
        }

        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
            TrackMovement(event);
            if (mQsTracking) {
                FlingQsWithCurrentVelocity();
                mQsTracking = FALSE;
            }
            mIntercepting = FALSE;
            break;
    }

    // Allow closing the whole panel when in SHADE state.
    if (mStatusBarState == IStatusBarState::SHADE) {
        Boolean tmp;
        PanelView::OnInterceptTouchEvent(event, &tmp);
        *result = tmp;
    }
    else {
        Boolean tmp;
        *result = !mQsExpanded && (PanelView::OnInterceptTouchEvent(event, &tmp), tmp);
    }

    return NOERROR;
}

void CNotificationPanelView::ResetDownStates(
    /* [in] */ IMotionEvent* event)
{
    Int32 v = 0;
    if ((event->GetActionMasked(&v), v) == IMotionEvent::ACTION_DOWN) {
        mOnlyAffordanceInThisMotion = FALSE;
        mQsTouchAboveFalsingThreshold = mQsFullyExpanded;
    }
}

ECode CNotificationPanelView::RequestDisallowInterceptTouchEvent(
    /* [in] */ Boolean disallowIntercept)
{
    // Block request when interacting with the scroll view so we can still intercept the
    // scrolling when QS is expanded.
    Boolean tmp = FALSE;
    if (mScrollView->IsHandlingTouchEvent(&tmp), tmp) {
        return NOERROR;
    }
    return PanelView::RequestDisallowInterceptTouchEvent(disallowIntercept);
}

void CNotificationPanelView::FlingQsWithCurrentVelocity()
{
    Float vel = GetCurrentVelocity();
    FlingSettings(vel, FlingExpandsQs(vel));
}

Boolean CNotificationPanelView::FlingExpandsQs(
    /* [in] */ Float vel)
{
    if (IsBelowFalsingThreshold()) {
        return FALSE;
    }
    if (Elastos::Core::Math::Abs(vel) < mFlingAnimationUtils->GetMinVelocityPxPerSecond()) {
        return GetQsExpansionFraction() > 0.5f;
    }
    else {
        return vel > 0;
    }
}

Boolean CNotificationPanelView::IsBelowFalsingThreshold()
{
    return !mQsTouchAboveFalsingThreshold && mStatusBarState == IStatusBarState::KEYGUARD;
}

Float CNotificationPanelView::GetQsExpansionFraction()
{
    return Elastos::Core::Math::Min(1.f, (mQsExpansionHeight - mQsMinExpansionHeight)
            / (GetTempQsMaxExpansion() - mQsMinExpansionHeight));
}

ECode CNotificationPanelView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mBlockTouches) {
        *result = FALSE;
        return NOERROR;
    }

    ResetDownStates(event);
    Int32 state = 0;
    mStatusBar->GetBarState(&state);

    if ((!mIsExpanding || mHintAnimationRunning)
            && !mQsExpanded
            && state != IStatusBarState::SHADE) {
        mAfforanceHelper->OnTouchEvent(event);
    }
    if (mOnlyAffordanceInThisMotion) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 masked = 0;
    event->GetActionMasked(&masked);
    Float fv = 0;
    if (masked == IMotionEvent::ACTION_DOWN && (GetExpandedFraction(&fv), fv) == 1.f
            && state != IStatusBarState::KEYGUARD && !mQsExpanded
            && mQsExpansionEnabled) {
        // Down in the empty area while fully expanded - go to QS.
        mQsTracking = TRUE;
        mConflictingQsExpansionGesture = TRUE;
        OnQsExpansionStarted();
        mInitialHeightOnTouch = mQsExpansionHeight;
        event->GetX(&mInitialTouchY);
        event->GetY(&mInitialTouchX);
    }
    if (mExpandedHeight != 0) {
        HandleQsDown(event);
    }
    if (!mTwoFingerQsExpand && mQsTracking) {
        OnQsTouch(event);
        if (!mConflictingQsExpansionGesture) {
            *result = TRUE;
            return NOERROR;
        }
    }
    if (masked == IMotionEvent::ACTION_CANCEL
            || masked == IMotionEvent::ACTION_UP) {
        mConflictingQsExpansionGesture = FALSE;
    }
    if (masked == IMotionEvent::ACTION_DOWN && mExpandedHeight == 0
            && mQsExpansionEnabled) {
        mTwoFingerQsExpandPossible = TRUE;
    }

    Int32 v = 0, index = 0;
    event->GetActionIndex(&index);
    if (mTwoFingerQsExpandPossible && masked == IMotionEvent::ACTION_POINTER_DOWN
            && (event->GetPointerCount(&v), v) == 2
            && (event->GetY(index, &fv), fv) < mStatusBarMinHeight) {
        mTwoFingerQsExpand = TRUE;
        RequestPanelHeightUpdate();

        // Normally, we start listening when the panel is expanded, but here we need to start
        // earlier so the state is already up to date when dragging down.
        SetListening(TRUE);
    }
    PanelView::OnTouchEvent(event, result);
    *result = TRUE;
    return NOERROR;
}

Boolean CNotificationPanelView::IsInQsArea(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 l = 0, r = 0, h = 0;
    Float f = 0, y0 = 0;
    return mStatusBarState != IStatusBarState::SHADE ||
            ((x >= (IView::Probe(mScrollView)->GetLeft(&l), l) && x <= (IView::Probe(mScrollView)->GetRight(&r), r))
            && (y <= (mNotificationStackScroller->GetBottomMostNotificationBottom(&f), f)
                || y <= (IView::Probe(mQsContainer)->GetY(&y0), y0) + (IView::Probe(mQsContainer)->GetHeight(&h), h)));
}

void CNotificationPanelView::HandleQsDown(
    /* [in] */ IMotionEvent* event)
{
    Int32 masked = 0;
    event->GetActionMasked(&masked);
    Float x = 0, y = 0;
    if (masked == IMotionEvent::ACTION_DOWN
            && ShouldQuickSettingsIntercept((event->GetX(&x), x), (event->GetY(&y), y), -1)) {
        mQsTracking = TRUE;
        OnQsExpansionStarted();
        mInitialHeightOnTouch = mQsExpansionHeight;
        event->GetX(&mInitialTouchY);
        event->GetY(&mInitialTouchX);

        // If we interrupt an expansion gesture here, make sure to update the state correctly.
        if (mIsExpanding) {
            OnExpandingFinished();
        }
    }
}

Boolean CNotificationPanelView::FlingExpands(
    /* [in] */ Float vel,
    /* [in] */ Float vectorVel)
{
    Boolean expands = PanelView::FlingExpands(vel, vectorVel);

    // If we are already running a QS expansion, make sure that we keep the panel open.
    if (mQsExpansionAnimator != NULL) {
        expands = TRUE;
    }
    return expands;
}

Boolean CNotificationPanelView::HasConflictingGestures()
{
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    return state != IStatusBarState::SHADE;
}

void CNotificationPanelView::OnQsTouch(
    /* [in] */ IMotionEvent* event)
{
    Int32 pointerIndex = 0;
    event->FindPointerIndex(mTrackingPointer, &pointerIndex);
    if (pointerIndex < 0) {
        pointerIndex = 0;
        event->GetPointerId(pointerIndex, &mTrackingPointer);
    }
    Float y = 0;
    event->GetY(pointerIndex, &y);
    Float x = 0;
    event->GetX(pointerIndex, &x);

    Int32 masked = 0;
    event->GetActionMasked(&masked);
    switch (masked) {
        case IMotionEvent::ACTION_DOWN:
            mQsTracking = TRUE;
            mInitialTouchY = y;
            mInitialTouchX = x;
            OnQsExpansionStarted();
            mInitialHeightOnTouch = mQsExpansionHeight;
            InitVelocityTracker();
            TrackMovement(event);
            break;

        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 index = 0;
            event->GetActionIndex(&index);
            Int32 upPointer = 0;
            event->GetPointerId(index, &upPointer);
            if (mTrackingPointer == upPointer) {
                // gesture is ongoing, find a new pointer to track
                const Int32 newIndex = (event->GetPointerId(0, &index), index) != upPointer ? 0 : 1;
                Float newY = 0;
                event->GetY(newIndex, &newY);
                Float newX = 0;
                event->GetX(newIndex, &newX);
                event->GetPointerId(newIndex, &mTrackingPointer);
                mInitialHeightOnTouch = mQsExpansionHeight;
                mInitialTouchY = newY;
                mInitialTouchX = newX;
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            const Float h = y - mInitialTouchY;
            SetQsExpansion(h + mInitialHeightOnTouch);
            if (h >= GetFalsingThreshold()) {
                mQsTouchAboveFalsingThreshold = TRUE;
            }
            TrackMovement(event);
            break;
        }

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL:
            mQsTracking = FALSE;
            mTrackingPointer = -1;
            TrackMovement(event);
            Float fraction = GetQsExpansionFraction();
            if ((fraction != 0.f || y >= mInitialTouchY)
                    && (fraction != 1.f || y <= mInitialTouchY)) {
                FlingQsWithCurrentVelocity();
            }
            else {
                mScrollYOverride = -1;
            }
            if (mVelocityTracker != NULL) {
                mVelocityTracker->Recycle();
                mVelocityTracker = NULL;
            }
            break;
    }
}

Int32 CNotificationPanelView::GetFalsingThreshold()
{
    Boolean tmp = FALSE;
    Float factor = (mStatusBar->IsScreenOnComingFromTouch(&tmp), tmp) ? 1.5f : 1.0f;
    return (Int32) (mQsFalsingThreshold * factor);
}

ECode CNotificationPanelView::OnOverscrolled(
    /* [in] */ Float lastTouchX,
    /* [in] */ Float lastTouchY,
    /* [in] */ Int32 amount)
{
    if (mIntercepting && ShouldQuickSettingsIntercept(lastTouchX, lastTouchY,
            -1 /* yDiff: Not relevant here */)) {
        OnQsExpansionStarted(amount);
        mInitialHeightOnTouch = mQsExpansionHeight;
        mInitialTouchY = mLastTouchY;
        mInitialTouchX = mLastTouchX;
        mQsTracking = TRUE;
    }
    return NOERROR;
}

ECode CNotificationPanelView::OnOverscrollTopChanged(
    /* [in] */ Float amount,
    /* [in] */ Boolean isRubberbanded)
{
    CancelAnimation();
    if (!mQsExpansionEnabled) {
        amount = 0.f;
    }
    Float rounded = amount >= 1.f ? amount : 0.f;
    mStackScrollerOverscrolling = rounded != 0.f && isRubberbanded;
    mQsExpansionFromOverscroll = rounded != 0.f;
    mLastOverscroll = rounded;
    UpdateQsState();
    SetQsExpansion(mQsMinExpansionHeight + rounded);
    return NOERROR;
}

ECode CNotificationPanelView::FlingTopOverscroll(
    /* [in] */ Float velocity,
    /* [in] */ Boolean open)
{
    mLastOverscroll = 0.f;
    SetQsExpansion(mQsExpansionHeight);
    AutoPtr<Runnable6> run = new Runnable6(this);
    FlingSettings(!mQsExpansionEnabled && open ? 0.f : velocity, open && mQsExpansionEnabled, run);
    return NOERROR;
}

void CNotificationPanelView::OnQsExpansionStarted()
{
    OnQsExpansionStarted(0);
}

void CNotificationPanelView::OnQsExpansionStarted(
    /* [in] */ Int32 overscrollAmount)
{
    CancelAnimation();

    // Reset scroll position and apply that position to the expanded height.
    Int32 y = 0;
    Float height = mQsExpansionHeight - (IView::Probe(mScrollView)->GetScrollY(&y), y) - overscrollAmount;
    if (y != 0) {
        mScrollYOverride = y;
    }
    IView::Probe(mScrollView)->ScrollTo(0, 0);
    SetQsExpansion(height);
}

void CNotificationPanelView::SetQsExpanded(
    /* [in] */ Boolean expanded)
{
    Boolean changed = mQsExpanded != expanded;
    if (changed) {
        mQsExpanded = expanded;
        UpdateQsState();
        RequestPanelHeightUpdate();
        mNotificationStackScroller->SetInterceptDelegateEnabled(expanded);
        mStatusBar->SetQsExpanded(expanded);
    }
}

ECode CNotificationPanelView::SetBarState(
    /* [in] */ Int32 statusBarState,
    /* [in] */ Boolean keyguardFadingAway,
    /* [in] */ Boolean goingToFullShade)
{
    Boolean keyguardShowing = statusBarState == IStatusBarState::KEYGUARD
            || statusBarState == IStatusBarState::SHADE_LOCKED;
    if (!mKeyguardShowing && keyguardShowing) {
        SetQsTranslation(mQsExpansionHeight);
        IView::Probe(mHeader)->SetTranslationY(0.f);
    }
    SetKeyguardStatusViewVisibility(statusBarState, keyguardFadingAway, goingToFullShade);
    SetKeyguardBottomAreaVisibility(statusBarState, goingToFullShade);
    if (goingToFullShade) {
        AnimateKeyguardStatusBarOut();
    }
    else {
        IView::Probe(mKeyguardStatusBar)->SetAlpha(1.f);
        IView::Probe(mKeyguardStatusBar)->SetVisibility(keyguardShowing ? IView::VISIBLE : IView::INVISIBLE);
    }
    mStatusBarState = statusBarState;
    mKeyguardShowing = keyguardShowing;
    UpdateQsState();
    if (goingToFullShade) {
        AnimateHeaderSlidingIn();
    }
    return NOERROR;
}

void CNotificationPanelView::AnimateHeaderSlidingIn()
{
    mHeaderAnimatingIn = TRUE;
    AutoPtr<IViewTreeObserver> vto;
    GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->AddOnPreDrawListener(mStartHeaderSlidingIn);
}

void CNotificationPanelView::AnimateKeyguardStatusBarOut()
{
    AutoPtr<IViewPropertyAnimator> vpa;
    IView::Probe(mKeyguardStatusBar)->Animate((IViewPropertyAnimator**)&vpa);
    vpa->Alpha(0.f);
    Int64 t = 0;
    mStatusBar->GetKeyguardFadingAwayDelay(&t);
    vpa->SetStartDelay(t);

    mStatusBar->GetKeyguardFadingAwayDuration(&t);
    vpa->SetDuration(t / 2);
    vpa->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT));
    vpa->WithEndAction(mAnimateKeyguardStatusBarInvisibleEndRunnable);
    vpa->Start();
}

void CNotificationPanelView::SetKeyguardBottomAreaVisibility(
    /* [in] */ Int32 statusBarState,
    /* [in] */ Boolean goingToFullShade)
{
    IView* area = IView::Probe(mKeyguardBottomArea);
    AutoPtr<IViewPropertyAnimator> vpa;
    if (goingToFullShade) {
        area->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();

        vpa = NULL;
        area->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Alpha(0.0f);
        Int64 t = 0;
        vpa->SetStartDelay((mStatusBar->GetKeyguardFadingAwayDelay(&t), t));
        vpa->SetDuration((mStatusBar->GetKeyguardFadingAwayDuration(&t), t) / 2);

        vpa->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT));
        vpa->WithEndAction(mAnimateKeyguardBottomAreaInvisibleEndRunnable);
        vpa->Start();
    }
    else if (statusBarState == IStatusBarState::KEYGUARD
            || statusBarState == IStatusBarState::SHADE_LOCKED) {
        vpa = NULL;
        area->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        area->SetVisibility(IView::VISIBLE);
        area->SetAlpha(1.f);
    }
    else {
        vpa = NULL;
        area->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        area->SetVisibility(IView::GONE);
        area->SetAlpha(1.f);
    }
}

void CNotificationPanelView::SetKeyguardStatusViewVisibility(
    /* [in] */ Int32 statusBarState,
    /* [in] */ Boolean keyguardFadingAway,
    /* [in] */ Boolean goingToFullShade)
{
    AutoPtr<IViewPropertyAnimator> vpa;
    if ((!keyguardFadingAway && mStatusBarState == IStatusBarState::KEYGUARD
            && statusBarState != IStatusBarState::KEYGUARD) || goingToFullShade) {
        IView::Probe(mKeyguardStatusView)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        mKeyguardStatusViewAnimating = TRUE;

        vpa = NULL;
        IView::Probe(mKeyguardStatusView)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Alpha(0.f);
        vpa->SetStartDelay(0);
        vpa->SetDuration(160);
        vpa->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT));
        vpa->WithEndAction(mAnimateKeyguardStatusViewInvisibleEndRunnable);
        if (keyguardFadingAway) {
            vpa = NULL;
            IView::Probe(mKeyguardStatusView)->Animate((IViewPropertyAnimator**)&vpa);
            Int64 t = 0;
            vpa->SetStartDelay((mStatusBar->GetKeyguardFadingAwayDelay(&t), t));
            vpa->SetDuration((mStatusBar->GetKeyguardFadingAwayDuration(&t), t) / 2);
            vpa->Start();
        }
    }
    else if (mStatusBarState == IStatusBarState::SHADE_LOCKED
            && statusBarState == IStatusBarState::KEYGUARD) {
        vpa = NULL;
        IView::Probe(mKeyguardStatusView)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        IView::Probe(mKeyguardStatusView)->SetVisibility(IView::VISIBLE);
        mKeyguardStatusViewAnimating = TRUE;
        IView::Probe(mKeyguardStatusView)->SetAlpha(0.f);

        vpa = NULL;
        IView::Probe(mKeyguardStatusView)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Alpha(1.f);
        vpa->SetStartDelay(0);
        vpa->SetDuration(320);

        vpa->SetInterpolator(ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_IN));
        vpa->WithEndAction(mAnimateKeyguardStatusViewVisibleEndRunnable);
    }
    else if (statusBarState == IStatusBarState::KEYGUARD) {
        vpa = NULL;
        IView::Probe(mKeyguardStatusView)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        mKeyguardStatusViewAnimating = FALSE;
        IView::Probe(mKeyguardStatusView)->SetVisibility(IView::VISIBLE);
        IView::Probe(mKeyguardStatusView)->SetAlpha(1.f);
    }
    else {
        vpa = NULL;
        IView::Probe(mKeyguardStatusView)->Animate((IViewPropertyAnimator**)&vpa);
        vpa->Cancel();
        mKeyguardStatusViewAnimating = FALSE;
        IView::Probe(mKeyguardStatusView)->SetVisibility(IView::GONE);
        IView::Probe(mKeyguardStatusView)->SetAlpha(1.f);
    }
}

void CNotificationPanelView::UpdateQsState()
{
    Boolean expandVisually = mQsExpanded || mStackScrollerOverscrolling;
    IView::Probe(mHeader)->SetVisibility((mQsExpanded || !mKeyguardShowing) ? IView::VISIBLE : IView::INVISIBLE);
    mHeader->SetExpanded(mKeyguardShowing || (mQsExpanded && !mStackScrollerOverscrolling));
    mNotificationStackScroller->SetScrollingEnabled(
            mStatusBarState != IStatusBarState::KEYGUARD && (!mQsExpanded
                    || mQsExpansionFromOverscroll));
    IView::Probe(mQsPanel)->SetVisibility(expandVisually ? IView::VISIBLE : IView::INVISIBLE);
    mQsContainer->SetVisibility(
            mKeyguardShowing && !expandVisually ? IView::INVISIBLE : IView::VISIBLE);
    mScrollView->SetTouchEnabled(mQsExpanded);
    UpdateEmptyShadeView();
    mQsNavbarScrim->SetVisibility(mStatusBarState == IStatusBarState::SHADE && mQsExpanded
            && !mStackScrollerOverscrolling && mQsScrimEnabled
                    ? IView::VISIBLE
                    : IView::INVISIBLE);
    if (mKeyguardUserSwitcher != NULL && mQsExpanded && !mStackScrollerOverscrolling) {
        mKeyguardUserSwitcher->Hide(TRUE /* animate */);
    }
}

void CNotificationPanelView::SetQsExpansion(
    /* [in] */ Float height)
{
    height = Elastos::Core::Math::Min(
        Elastos::Core::Math::Max(height, (Float)mQsMinExpansionHeight), (Float)mQsMaxExpansionHeight);
    mQsFullyExpanded = height == mQsMaxExpansionHeight;
    if (height > mQsMinExpansionHeight && !mQsExpanded && !mStackScrollerOverscrolling) {
        SetQsExpanded(TRUE);
    }
    else if (height <= mQsMinExpansionHeight && mQsExpanded) {
        SetQsExpanded(FALSE);
        if (mLastAnnouncementWasQuickSettings && !mTracking) {
            AutoPtr<ICharSequence> value;
            CString::New(GetKeyguardOrLockScreenString(), (ICharSequence**)&value);
            AnnounceForAccessibility(value);
            mLastAnnouncementWasQuickSettings = FALSE;
        }
    }
    mQsExpansionHeight = height;
    mHeader->SetExpansion(GetHeaderExpansionFraction());
    SetQsTranslation(height);
    RequestScrollerTopPaddingUpdate(FALSE /* animate */);
    UpdateNotificationScrim(height);
    if (mKeyguardShowing) {
        UpdateHeaderKeyguard();
    }

    if (mStatusBarState == IStatusBarState::SHADE && mQsExpanded
            && !mStackScrollerOverscrolling && mQsScrimEnabled) {
        mQsNavbarScrim->SetAlpha(GetQsExpansionFraction());
    }

    // Upon initialisation when we are not layouted yet we don't want to announce that we are
    // fully expanded, hence the != 0.0f check.
    if (height != 0.0f && mQsFullyExpanded && !mLastAnnouncementWasQuickSettings) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        String str;
        ctx->GetString(R::string::accessibility_desc_quick_settings, &str);
        AnnounceForAccessibility(CoreUtils::Convert(str));
        mLastAnnouncementWasQuickSettings = TRUE;
    }
}

String CNotificationPanelView::GetKeyguardOrLockScreenString()
{
    String value;
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    if (mStatusBarState == IStatusBarState::KEYGUARD) {
        ctx->GetString(R::string::accessibility_desc_lock_screen, &value);
    }
    else {
        ctx->GetString(R::string::accessibility_desc_notification_shade, &value);
    }
    return value;
}

void CNotificationPanelView::UpdateNotificationScrim(
    /* [in] */ Float height)
{
    Int32 startDistance = mQsMinExpansionHeight + mNotificationScrimWaitDistance;
    Float progress = (height - startDistance) / (mQsMaxExpansionHeight - startDistance);
    progress = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(progress, 1.0f));
}

Float CNotificationPanelView::GetHeaderExpansionFraction()
{
    if (!mKeyguardShowing) {
        return GetQsExpansionFraction();
    }
    return 1.f;
}

void CNotificationPanelView::SetQsTranslation(
    /* [in] */ Float height)
{
    if (!mHeaderAnimatingIn) {
        Int32 h = 0;
        mQsContainer->GetHeight(&h);
        mQsContainer->SetY(height - h + GetHeaderTranslation());
    }
    if (mKeyguardShowing) {
        Int32 h = 0;
        IView::Probe(mHeader)->GetHeight(&h);
        IView::Probe(mHeader)->SetY(Interpolate(GetQsExpansionFraction(), -h, 0));
    }
}

Float CNotificationPanelView::CalculateQsTopPadding()
{
    // We can only do the smoother transition on Keyguard when we also are not collapsing from a
    // scrolled quick settings.
    if (mKeyguardShowing && mScrollYOverride == -1) {
        Int32 p = 0;
        mNotificationStackScroller->GetIntrinsicPadding(&p);
        return Interpolate(GetQsExpansionFraction(), p - mNotificationTopPadding, mQsMaxExpansionHeight);
    }
    return mQsExpansionHeight;
}

void CNotificationPanelView::RequestScrollerTopPaddingUpdate(
    /* [in] */ Boolean animate)
{
    Int32 y = 0;
    IView::Probe(mScrollView)->GetScrollY(&y);
    mNotificationStackScroller->UpdateTopPadding(CalculateQsTopPadding(), y,
            mAnimateNextTopPaddingChange || animate);
    mAnimateNextTopPaddingChange = FALSE;
}

void CNotificationPanelView::TrackMovement(
    /* [in] */ IMotionEvent* event)
{
    if (mVelocityTracker != NULL) mVelocityTracker->AddMovement(event);
    event->GetX(&mLastTouchX);
    event->GetY(&mLastTouchY);
}

void CNotificationPanelView::InitVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
    }
    AutoPtr<IVelocityTrackerHelper> helper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
    helper->Obtain((IVelocityTracker**)&mVelocityTracker);
}

Float CNotificationPanelView::GetCurrentVelocity()
{
    if (mVelocityTracker == NULL) {
        return 0;
    }
    mVelocityTracker->ComputeCurrentVelocity(1000);
    Float fv = 0;
    mVelocityTracker->GetYVelocity(&fv);
    return fv;
}

void CNotificationPanelView::CancelAnimation()
{
    if (mQsExpansionAnimator != NULL) {
        IAnimator::Probe(mQsExpansionAnimator)->Cancel();
    }
}

void CNotificationPanelView::FlingSettings(
    /* [in] */ Float vel,
    /* [in] */ Boolean expand)
{
    FlingSettings(vel, expand, NULL);
}

void CNotificationPanelView::FlingSettings(
    /* [in] */ Float vel,
    /* [in] */ Boolean expand,
    /* [in] */ IRunnable* onFinishRunnable)
{
    Float target = expand ? mQsMaxExpansionHeight : mQsMinExpansionHeight;
    if (target == mQsExpansionHeight) {
        mScrollYOverride = -1;
        if (onFinishRunnable != NULL) {
            onFinishRunnable->Run();
        }
        return;
    }
    Boolean belowFalsingThreshold = IsBelowFalsingThreshold();
    if (belowFalsingThreshold) {
        vel = 0;
    }
    mScrollView->SetBlockFlinging(TRUE);

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = mQsExpansionHeight;
    (*fvs)[1] = target;
    helper->OfFloat(fvs, (IValueAnimator**)&animator);
    mFlingAnimationUtils->Apply(IAnimator::Probe(animator), mQsExpansionHeight, target, vel);
    if (belowFalsingThreshold) {
        IAnimator::Probe(animator)->SetDuration(350);
    }

    AutoPtr<AnimatorUpdateListener> ul = new AnimatorUpdateListener(this);
    animator->AddUpdateListener(ul);

    AutoPtr<AnimatorListenerAdapter3> al = new AnimatorListenerAdapter3(this, onFinishRunnable);
    IAnimator::Probe(animator)->AddListener(al);
    IAnimator::Probe(animator)->Start();
    mQsExpansionAnimator = animator;
    mQsAnimatorExpand = expand;
}

Boolean CNotificationPanelView::ShouldQuickSettingsIntercept(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float yDiff)
{
    if (!mQsExpansionEnabled) {
        return FALSE;
    }
    AutoPtr<IView> header = mKeyguardShowing ? IView::Probe(mKeyguardStatusBar) : IView::Probe(mHeader);
    Int32 l = 0, r = 0, t = 0, b = 0;
    Boolean onHeader = x >= (header->GetLeft(&l), l) && x <= (header->GetRight(&r), r)
            && y >= (header->GetTop(&t), t) && y <= (header->GetBottom(&b), b);
    if (mQsExpanded) {
        Boolean tmp = FALSE;
        return onHeader || (((mScrollView->IsScrolledToBottom(&tmp), tmp) && yDiff < 0) && IsInQsArea(x, y));
    }
    return onHeader;
}

Boolean CNotificationPanelView::IsScrolledToBottom()
{
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    if (state == IStatusBarState::KEYGUARD) {
        return TRUE;
    }
    Boolean tmp = FALSE;
    if (!IsInSettings()) {
        IScrollAdapter::Probe(mNotificationStackScroller)->IsScrolledToBottom(&tmp);
    }
    else {
        mScrollView->IsScrolledToBottom(&tmp);
    }
    return tmp;
}

Int32 CNotificationPanelView::GetMaxPanelHeight()
{
    Int32 min = mStatusBarMinHeight;

    Int32 state = 0, c = 0;
    mStatusBar->GetBarState(&state);
    if (state != IStatusBarState::KEYGUARD
            && (mNotificationStackScroller->GetNotGoneChildCount(&c), c) == 0) {
        Int32 minHeight = (Int32) ((mQsMinExpansionHeight + GetOverExpansionAmount())
                * HEADER_RUBBERBAND_FACTOR);
        min = Elastos::Core::Math::Max(min, minHeight);
    }

    Int32 maxHeight;
    if (mTwoFingerQsExpand || mQsExpanded || (mIsExpanding && mQsExpandedWhenExpandingStarted)) {
        maxHeight = Elastos::Core::Math::Max(CalculatePanelHeightQsExpanded(), CalculatePanelHeightShade());
    }
    else {
        maxHeight = CalculatePanelHeightShade();
    }
    maxHeight = Elastos::Core::Math::Max(maxHeight, min);
    return maxHeight;
}

Boolean CNotificationPanelView::IsInSettings()
{
    return mQsExpanded;
}

void CNotificationPanelView::OnHeightUpdated(
    /* [in] */ Float expandedHeight)
{
    if (!mQsExpanded) {
        PositionClockAndNotifications();
    }
    if (mTwoFingerQsExpand || (mQsExpanded && !mQsTracking && mQsExpansionAnimator == NULL
                && !mQsExpansionFromOverscroll)) {
        Int32 v1 = 0, v2 = 0, v3 = 0;
        Float panelHeightQsCollapsed = (mNotificationStackScroller->GetIntrinsicPadding(&v1), v1)
                + (mNotificationStackScroller->GetMinStackHeight(&v2), v2)
                + (mNotificationStackScroller->GetNotificationTopPadding(&v3), v3);
        Float panelHeightQsExpanded = CalculatePanelHeightQsExpanded();
        Float t = (expandedHeight - panelHeightQsCollapsed)
                / (panelHeightQsExpanded - panelHeightQsCollapsed);

        SetQsExpansion(mQsMinExpansionHeight
                + t * (GetTempQsMaxExpansion() - mQsMinExpansionHeight));
    }
    mNotificationStackScroller->SetStackHeight(expandedHeight);
    UpdateHeader();
    UpdateUnlockIcon();
    UpdateNotificationTranslucency();
}

Int32 CNotificationPanelView::GetTempQsMaxExpansion()
{
    Int32 qsTempMaxExpansion = mQsMaxExpansionHeight;
    if (mScrollYOverride != -1) {
        qsTempMaxExpansion -= mScrollYOverride;
    }
    return qsTempMaxExpansion;
}

Int32 CNotificationPanelView::CalculatePanelHeightShade()
{
    Int32 emptyBottomMargin = 0, v = 0;
    mNotificationStackScroller->GetEmptyBottomMargin(&emptyBottomMargin);
    IView::Probe(mNotificationStackScroller)->GetHeight(&v);
    Int32 maxHeight = v - emptyBottomMargin - mTopPaddingAdjustment;

    Float f = 0;
    maxHeight += (mNotificationStackScroller->GetTopPaddingOverflow(&f), f);
    return maxHeight;
}

Int32 CNotificationPanelView::CalculatePanelHeightQsExpanded()
{
    Int32 h = 0, bm = 0, t = 0, p = 0;
    IView::Probe(mNotificationStackScroller)->GetHeight(&h);
    mNotificationStackScroller->GetNotificationTopPadding(&p);
    Float notificationHeight = h
            - (mNotificationStackScroller->GetEmptyBottomMargin(&bm), bm)
            - (mNotificationStackScroller->GetTopPadding(&t), t);
    Float totalHeight = mQsMaxExpansionHeight + notificationHeight + p;
    if (totalHeight > h) {
        Float fullyCollapsedHeight = mQsMaxExpansionHeight
                + (mNotificationStackScroller->GetMinStackHeight(&h), h) + p
                - GetScrollViewScrollY();
        totalHeight = Elastos::Core::Math::Max(fullyCollapsedHeight, (Float)h);
    }
    return (Int32) totalHeight;
}

Int32 CNotificationPanelView::GetScrollViewScrollY()
{
    if (mScrollYOverride != -1) {
        return mScrollYOverride;
    }
    Int32 y = 0;
    IView::Probe(mScrollView)->GetScrollY(&y);
    return y;
}

void CNotificationPanelView::UpdateNotificationTranslucency()
{
    using Elastos::Core::Math;
    Int32 v1 = 0, v2 = 0, v3 = 0;
    Float y = GetNotificationsTopY();
    mNotificationStackScroller->GetItemHeight(&v1);
    mNotificationStackScroller->GetBottomStackPeekSize(&v2);
    mNotificationStackScroller->GetCollapseSecondCardPadding(&v3);

    Float alpha = (y + v1) / (mQsMinExpansionHeight + v2 - v3);
    alpha = Math::Max(0.0f, Math::Min(alpha, (Float)1.0f));
    alpha = (Float) Math::Pow(alpha, 0.75);

    IView* view = IView::Probe(mNotificationStackScroller);
    Int32 type = 0;
    if (alpha != 1.0f && (view->GetLayerType(&type), type) != LAYER_TYPE_HARDWARE) {
        view->SetLayerType(LAYER_TYPE_HARDWARE, NULL);
    }
    else if (alpha == 1.0f && (view->GetLayerType(&type), type) == LAYER_TYPE_HARDWARE) {
        view->SetLayerType(LAYER_TYPE_NONE, NULL);
    }
    view->SetAlpha(alpha);
}

Float CNotificationPanelView::GetOverExpansionAmount()
{
    Float fv = 0;
    mNotificationStackScroller->GetCurrentOverScrollAmount(TRUE /* top */, &fv);
    return fv;
}

Float CNotificationPanelView::GetOverExpansionPixels()
{
    Float fv = 0;
    mNotificationStackScroller->GetCurrentOverScrolledPixels(TRUE /* top */, &fv);
    return fv;
}

void CNotificationPanelView::UpdateUnlockIcon()
{
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    if (state == IStatusBarState::KEYGUARD
            || state == IStatusBarState::SHADE_LOCKED) {
        Float v = 0;
        GetExpandedHeight(&v);
        Boolean active = GetMaxPanelHeight() - v > mUnlockMoveDistance;
        AutoPtr<IKeyguardAffordanceView> lockIcon;
        mKeyguardBottomArea->GetLockIcon((IKeyguardAffordanceView**)&lockIcon);
        if (active && !mUnlockIconActive && mTracking) {
            lockIcon->SetImageAlpha(1.0f, TRUE, 150, mFastOutLinearInterpolator, NULL);
            lockIcon->SetImageScale(LOCK_ICON_ACTIVE_SCALE, TRUE, 150,
                    mFastOutLinearInterpolator);
        }
        else if (!active && mUnlockIconActive && mTracking) {
            lockIcon->SetImageAlpha(KeyguardAffordanceHelper::SWIPE_RESTING_ALPHA_AMOUNT, TRUE,
                    150, mFastOutLinearInterpolator, NULL);
            lockIcon->SetImageScale(1.0f, TRUE, 150,
                    mFastOutLinearInterpolator);
        }
        mUnlockIconActive = active;
    }
}

void CNotificationPanelView::UpdateHeader()
{
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    if (state == IStatusBarState::KEYGUARD
            || state == IStatusBarState::SHADE_LOCKED) {
        UpdateHeaderKeyguard();
    }
    else {
        UpdateHeaderShade();
    }
}

void CNotificationPanelView::UpdateHeaderShade()
{
    if (!mHeaderAnimatingIn) {
        IView::Probe(mHeader)->SetTranslationY(GetHeaderTranslation());
    }
    SetQsTranslation(mQsExpansionHeight);
}

Float CNotificationPanelView::GetHeaderTranslation()
{
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    if (state == IStatusBarState::KEYGUARD
            || state == IStatusBarState::SHADE_LOCKED) {
        return 0;
    }

    Int32 v = 0;
    if ((mNotificationStackScroller->GetNotGoneChildCount(&v), v) == 0) {
        if (mExpandedHeight / HEADER_RUBBERBAND_FACTOR >= mQsMinExpansionHeight) {
            return 0;
        }
        else {
            return mExpandedHeight / HEADER_RUBBERBAND_FACTOR - mQsMinExpansionHeight;
        }
    }

    Float fv = 0;
    IView::Probe(mNotificationStackScroller)->GetTranslationY(&fv);
    return Elastos::Core::Math::Min((Float)0, fv) / HEADER_RUBBERBAND_FACTOR;
}

void CNotificationPanelView::UpdateHeaderKeyguard()
{
    Float alphaNotifications;
    Int32 state = 0, v = 0;
    mStatusBar->GetBarState(&state);
    IView::Probe(mKeyguardStatusBar)->GetHeight(&v);
    if (state == IStatusBarState::KEYGUARD) {
        // When on Keyguard, we hide the header as soon as the top card of the notification
        // stack scroller is close enough (collision distance) to the bottom of the header.
        alphaNotifications = GetNotificationsTopY()
                / (v + mNotificationsHeaderCollideDistance);
    }
    else {

        // In SHADE_LOCKED, the top card is already really close to the header. Hide it as
        // soon as we start translating the stack.
        alphaNotifications = GetNotificationsTopY() / v;
    }
    alphaNotifications = MathUtils::Constrain(alphaNotifications, (Float)0, (Float)1);
    alphaNotifications = (Float) Elastos::Core::Math::Pow(alphaNotifications, 0.75);
    Float alphaQsExpansion = 1 - Elastos::Core::Math::Min((Float)1, GetQsExpansionFraction() * 2);
    IView::Probe(mKeyguardStatusBar)->SetAlpha(Elastos::Core::Math::Min(alphaNotifications, alphaQsExpansion));
    IView::Probe(mKeyguardBottomArea)->SetAlpha(Elastos::Core::Math::Min(1 - GetQsExpansionFraction(), alphaNotifications));
    SetQsTranslation(mQsExpansionHeight);
}

Float CNotificationPanelView::GetNotificationsTopY()
{
    Int32 c = 0;
    Float f = 0;
    if ((mNotificationStackScroller->GetNotGoneChildCount(&c), c) == 0) {
        GetExpandedHeight(&f);
        return f;
    }
    mNotificationStackScroller->GetNotificationsTopY(&f);
    return f;
}

void CNotificationPanelView::OnExpandingStarted()
{
    PanelView::OnExpandingStarted();
    mNotificationStackScroller->OnExpansionStarted();
    mIsExpanding = TRUE;
    mQsExpandedWhenExpandingStarted = mQsExpanded;
    if (mQsExpanded) {
        OnQsExpansionStarted();
    }
}

void CNotificationPanelView::OnExpandingFinished()
{
    PanelView::OnExpandingFinished();
    mNotificationStackScroller->OnExpansionStopped();
    mIsExpanding = FALSE;
    mScrollYOverride = -1;
    if (mExpandedHeight == 0.f) {
        SetListening(FALSE);
    }
    else {
        SetListening(TRUE);
    }
    mTwoFingerQsExpand = FALSE;
    mTwoFingerQsExpandPossible = FALSE;
}

void CNotificationPanelView::SetListening(
    /* [in] */ Boolean listening)
{
    mHeader->SetListening(listening);
    mKeyguardStatusBar->SetListening(listening);
    mQsPanel->SetListening(listening);
}

ECode CNotificationPanelView::InstantExpand()
{
    PanelView::InstantExpand();
    SetListening(TRUE);
    return NOERROR;
}

void CNotificationPanelView::SetOverExpansion(
    /* [in] */ Float overExpansion,
    /* [in] */ Boolean isPixels)
{
    if (mConflictingQsExpansionGesture || mTwoFingerQsExpand) {
        return;
    }

    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    if (state != IStatusBarState::KEYGUARD) {
        mNotificationStackScroller->SetOnHeightChangedListener(NULL);
        if (isPixels) {
            mNotificationStackScroller->SetOverScrolledPixels(
                    overExpansion, TRUE /* onTop */, FALSE /* animate */);
        }
        else {
            mNotificationStackScroller->SetOverScrollAmount(
                    overExpansion, TRUE /* onTop */, FALSE /* animate */);
        }
        AutoPtr<HeightChangedListener> hl = new HeightChangedListener(this);
        mNotificationStackScroller->SetOnHeightChangedListener(hl);
    }
}

void CNotificationPanelView::OnTrackingStarted()
{
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    PanelView::OnTrackingStarted();
    if (state == IStatusBarState::KEYGUARD
            || state == IStatusBarState::SHADE_LOCKED) {
        mAfforanceHelper->AnimateHideLeftRightIcon();
    }
    else if (mQsExpanded) {
        mTwoFingerQsExpand = TRUE;
    }
}

void CNotificationPanelView::OnTrackingStopped(
    /* [in] */ Boolean expand)
{
    PanelView::OnTrackingStopped(expand);
    if (expand) {
        mNotificationStackScroller->SetOverScrolledPixels(
                0.0f, TRUE /* onTop */, TRUE /* animate */);
    }
    Int32 state = 0;
    mStatusBar->GetBarState(&state);
    if (expand && (state == IStatusBarState::KEYGUARD
            || state == IStatusBarState::SHADE_LOCKED)) {
        if (!mHintAnimationRunning) {
            mAfforanceHelper->Reset(TRUE);
        }
    }
    if (!expand && (state == IStatusBarState::KEYGUARD
            || state == IStatusBarState::SHADE_LOCKED)) {
        AutoPtr<IKeyguardAffordanceView> lockIcon;
        mKeyguardBottomArea->GetLockIcon((IKeyguardAffordanceView**)&lockIcon);
        lockIcon->SetImageAlpha(0.0f, TRUE, 100, mFastOutLinearInterpolator, NULL);
        lockIcon->SetImageScale(2.0f, TRUE, 100, mFastOutLinearInterpolator);
    }
}

ECode CNotificationPanelView::OnHeightChanged(
    /* [in] */ IExpandableView* view)
{
    // Block update if we are in quick settings and just the top padding changed
    // (i.e. view == NULL).
    if (view == NULL && mQsExpanded) {
        return NOERROR;
    }
    RequestPanelHeightUpdate();
    return NOERROR;
}

ECode CNotificationPanelView::OnReset(
    /* [in] */ IExpandableView* view)
{
    return NOERROR;
}

ECode CNotificationPanelView::OnScrollChanged()
{
    if (mQsExpanded) {
        RequestScrollerTopPaddingUpdate(FALSE /* animate */);
        RequestPanelHeightUpdate();
    }
    return NOERROR;
}

ECode CNotificationPanelView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    PanelView::OnConfigurationChanged(newConfig);
    mAfforanceHelper->OnConfigurationChanged();
    return NOERROR;
}

ECode CNotificationPanelView::OnClick(
    /* [in] */ IView* v)
{
    if (v == IView::Probe(mHeader)) {
        OnQsExpansionStarted();
        if (mQsExpanded) {
            FlingSettings(0 /* vel */, FALSE /* expand */);
        }
        else if (mQsExpansionEnabled) {
            FlingSettings(0 /* vel */, TRUE /* expand */);
        }
    }
    return NOERROR;
}

ECode CNotificationPanelView::OnAnimationToSideStarted(
    /* [in] */ Boolean rightPage)
{
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    Boolean start = dir == LAYOUT_DIRECTION_RTL ? rightPage : !rightPage;
    mIsLaunchTransitionRunning = TRUE;
    mLaunchAnimationEndRunnable = NULL;
    if (start) {
        mKeyguardBottomArea->LaunchPhone();
    }
    else {
        mSecureCameraLaunchManager->StartSecureCameraLaunch();
    }
    mBlockTouches = TRUE;
    return NOERROR;
}

ECode CNotificationPanelView::OnAnimationToSideEnded()
{
    mIsLaunchTransitionRunning = FALSE;
    mIsLaunchTransitionFinished = TRUE;
    if (mLaunchAnimationEndRunnable != NULL) {
        mLaunchAnimationEndRunnable->Run();
        mLaunchAnimationEndRunnable = NULL;
    }
    return NOERROR;
}

void CNotificationPanelView::OnEdgeClicked(
    /* [in] */ Boolean right)
{
    if (right) {
        AutoPtr<IKeyguardAffordanceView> view;
        GetRightIcon((IKeyguardAffordanceView**)&view);
        Int32 v = 0;
        IView::Probe(view)->GetVisibility(&v);
        if (v != IView::VISIBLE) return;
    }
    else if (!right) {
        AutoPtr<IKeyguardAffordanceView> view;
        GetLeftIcon((IKeyguardAffordanceView**)&view);
        Int32 v = 0;
        IView::Probe(view)->GetVisibility(&v);
        if (v != IView::VISIBLE) return;
    }
    else if (IsDozing()) {
        return;
    }
    mHintAnimationRunning = TRUE;
    AutoPtr<Runnable7> run  = new Runnable7(this);
    mAfforanceHelper->StartHintAnimation(right, run);

    Int32 dir = 0;
    GetLayoutDirection(&dir);
    Boolean start = dir == LAYOUT_DIRECTION_RTL ? right : !right;
    if (start) {
        mStatusBar->OnPhoneHintStarted();
    }
    else {
        mStatusBar->OnCameraHintStarted();
    }
}

void CNotificationPanelView::StartUnlockHintAnimation()
{
    PanelView::StartUnlockHintAnimation();

    AutoPtr<IKeyguardAffordanceView> view;
    GetCenterIcon((IKeyguardAffordanceView**)&view);
    StartHighlightIconAnimation(view);
}

void CNotificationPanelView::StartHighlightIconAnimation(
    /* [in] */ IKeyguardAffordanceView* icon)
{
    AutoPtr<Runnable8> run = new Runnable8(this, icon);
    icon->SetImageAlpha(1.0f, TRUE, KeyguardAffordanceHelper::HINT_PHASE1_DURATION,
            mFastOutSlowInInterpolator, run);
}

ECode CNotificationPanelView::GetPageWidth(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 w = 0;
    GetWidth(&w);
    *result = w;
    return NOERROR;
}

ECode CNotificationPanelView::OnSwipingStarted()
{
    mSecureCameraLaunchManager->OnSwipingStarted();
    RequestDisallowInterceptTouchEvent(TRUE);
    mOnlyAffordanceInThisMotion = TRUE;
    return NOERROR;
}

ECode CNotificationPanelView::GetLeftIcon(
    /* [out] */ IKeyguardAffordanceView** view)
{
    VALIDATE_NOT_NULL(view);
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    if (dir == LAYOUT_DIRECTION_RTL) {
        return mKeyguardBottomArea->GetCameraView(view);
    }
    return mKeyguardBottomArea->GetPhoneView(view);
}

ECode CNotificationPanelView::GetCenterIcon(
    /* [out] */ IKeyguardAffordanceView** view)
{
    VALIDATE_NOT_NULL(view);
    return mKeyguardBottomArea->GetLockIcon(view);
}

ECode CNotificationPanelView::GetRightIcon(
    /* [out] */ IKeyguardAffordanceView** view)
{
    VALIDATE_NOT_NULL(view);
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    if (dir == LAYOUT_DIRECTION_RTL) {
        return mKeyguardBottomArea->GetPhoneView(view);
    }
    return mKeyguardBottomArea->GetCameraView(view);
}

ECode CNotificationPanelView::GetLeftPreview(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    if (dir == LAYOUT_DIRECTION_RTL) {
        return mKeyguardBottomArea->GetCameraPreview(view);
    }
    return mKeyguardBottomArea->GetPhonePreview(view);
}

ECode CNotificationPanelView::GetRightPreview(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    if (dir == LAYOUT_DIRECTION_RTL) {
        return mKeyguardBottomArea->GetPhonePreview(view);
    }
    return mKeyguardBottomArea->GetCameraPreview(view);
}

ECode CNotificationPanelView::GetAffordanceFalsingFactor(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = (mStatusBar->IsScreenOnComingFromTouch(&tmp), tmp) ? 1.5f : 1.0f;
    return NOERROR;
}

Float CNotificationPanelView::GetPeekHeight()
{
    Int32 value = 0;
    if ((mNotificationStackScroller->GetNotGoneChildCount(&value), value) > 0) {
        mNotificationStackScroller->GetPeekHeight(&value);
        return value;
    }
    return mQsMinExpansionHeight * HEADER_RUBBERBAND_FACTOR;
}

Float CNotificationPanelView::GetCannedFlingDurationFactor()
{
    if (mQsExpanded) {
        return 0.7f;
    }
    return 0.6f;
}

Boolean CNotificationPanelView::FullyExpandedClearAllVisible()
{
    Boolean tmp = FALSE;
    return (mNotificationStackScroller->IsDismissViewNotGone(&tmp), tmp)
            && (IScrollAdapter::Probe(mNotificationStackScroller)->IsScrolledToBottom(&tmp), tmp)
            && !mTwoFingerQsExpand;
}

Boolean CNotificationPanelView::IsClearAllVisible()
{
    Boolean tmp = FALSE;
    mNotificationStackScroller->IsDismissViewVisible(&tmp);
    return tmp;
}

Int32 CNotificationPanelView::GetClearAllHeight()
{
    Int32 h = 0;
    mNotificationStackScroller->GetDismissViewHeight(&h);
    return h;
}

Boolean CNotificationPanelView::IsTrackingBlocked()
{
    return mConflictingQsExpansionGesture && mQsExpanded;
}

ECode CNotificationPanelView::NotifyVisibleChildrenChanged()
{
    Int32 c = 0;
    mNotificationStackScroller->GetNotGoneChildCount(&c);
    if (c != 0) {
        mReserveNotificationSpace->SetVisibility(IView::VISIBLE);
    }
    else {
        mReserveNotificationSpace->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode CNotificationPanelView::IsQsExpanded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mQsExpanded;
    return NOERROR;
}

ECode CNotificationPanelView::IsQsDetailShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mQsPanel->IsShowingDetail(result);
}

ECode CNotificationPanelView::CloseQsDetail()
{
    mQsPanel->CloseDetail();
    return NOERROR;
}

ECode CNotificationPanelView::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CNotificationPanelView::IsLaunchTransitionFinished(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsLaunchTransitionFinished;
    return NOERROR;
}

ECode CNotificationPanelView::IsLaunchTransitionRunning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsLaunchTransitionRunning;
    return NOERROR;
}

ECode CNotificationPanelView::SetLaunchTransitionEndRunnable(
    /* [in] */ IRunnable* r)
{
    mLaunchAnimationEndRunnable = r;
    return NOERROR;
}

ECode CNotificationPanelView::SetEmptyDragAmount(
    /* [in] */ Float amount)
{
    Float factor = 0.8f;
    Int32 c = 0;
    Boolean tmp = FALSE;
    mNotificationStackScroller->GetNotGoneChildCount(&c);
    if (c > 0) {
        factor = 0.4f;
    }
    else if (mStatusBar->HasActiveNotifications(&tmp), !tmp) {
        factor = 0.4f;
    }
    mEmptyDragAmount = amount * factor;
    PositionClockAndNotifications();
    return NOERROR;
}

Float CNotificationPanelView::Interpolate(
    /* [in] */ Float t,
    /* [in] */ Float start,
    /* [in] */ Float end)
{
    return (1 - t) * start + t * end;
}

void CNotificationPanelView::UpdateKeyguardStatusBarVisibility()
{
    IView::Probe(mKeyguardStatusBar)->SetVisibility(mKeyguardShowing && !mDozing ? IView::VISIBLE : IView::INVISIBLE);
}

ECode CNotificationPanelView::SetDozing(
    /* [in] */ Boolean dozing)
{
    if (dozing == mDozing) return NOERROR;
    mDozing = dozing;
    if (mDozing) {
        SetBackgroundColorAlpha(this, DOZE_BACKGROUND_COLOR, 0xff, FALSE /*animate*/);
    }
    else {
        SetBackgroundColorAlpha(this, DOZE_BACKGROUND_COLOR, 0, TRUE /*animate*/);
    }
    UpdateKeyguardStatusBarVisibility();
    return NOERROR;
}

Boolean CNotificationPanelView::IsDozing()
{
    return mDozing;
}

void CNotificationPanelView::SetBackgroundColorAlpha(
    /* [in] */ IView* target,
    /* [in] */ Int32 rgb,
    /* [in] */ Int32 targetAlpha,
    /* [in] */ Boolean animate)
{
    Int32 currentAlpha = GetBackgroundAlpha(target);
    if (currentAlpha == targetAlpha) {
        return;
    }

    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 r, g, b;
    color->Red(rgb, &r);
    color->Green(rgb, &g);
    color->Blue(rgb, &b);
    AutoPtr<IInterface> runningAnim;
    target->GetTag(TAG_KEY_ANIM, (IInterface**)&runningAnim);
    if (IValueAnimator::Probe(runningAnim)) {
        IAnimator::Probe(runningAnim)->Cancel();
    }
    if (!animate) {
        Int32 argb = 0;
        color->Argb(targetAlpha, r, g, b, &argb);
        target->SetBackgroundColor(argb);
        return;
    }

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> anim;
    AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
    (*ivs)[0] = currentAlpha;
    (*ivs)[1] = targetAlpha;
    helper->OfInt32(ivs, (IValueAnimator**)&anim);

    AutoPtr<SetBgColorAlphaAnimatorUpdateListener> ul = new SetBgColorAlphaAnimatorUpdateListener(target, r, g, b);
    anim->AddUpdateListener(ul);
    anim->SetDuration(DOZE_BACKGROUND_ANIM_DURATION);

    AutoPtr<SetBgColorAlphaAnimatorListenerAdapter> al = new SetBgColorAlphaAnimatorListenerAdapter(target);
    IAnimator::Probe(anim)->AddListener(al);
    IAnimator::Probe(anim)->Start();
    target->SetTag(TAG_KEY_ANIM, anim);
}

Int32 CNotificationPanelView::GetBackgroundAlpha(
    /* [in] */ IView* view)
{
    AutoPtr<IDrawable> d;
    view->GetBackground((IDrawable**)&d);
    AutoPtr<IColorDrawable> drawable = IColorDrawable::Probe(d);
    if (drawable) {
        AutoPtr<IColor> color;
        CColor::AcquireSingleton((IColor**)&color);
        Int32 alpha = 0, c = 0;
        drawable->GetColor(&c);
        color->Alpha(c, &alpha);
        return alpha;
    }

    return 0;
}

ECode CNotificationPanelView::SetShadeEmpty(
    /* [in] */ Boolean shadeEmpty)
{
    mShadeEmpty = shadeEmpty;
    UpdateEmptyShadeView();
    return NOERROR;
}

void CNotificationPanelView::UpdateEmptyShadeView()
{
    // Hide "No notifications" in QS.
    mNotificationStackScroller->UpdateEmptyShadeView(mShadeEmpty && !mQsExpanded);
}

ECode CNotificationPanelView::SetQsScrimEnabled(
    /* [in] */ Boolean qsScrimEnabled)
{
    Boolean changed = mQsScrimEnabled != qsScrimEnabled;
    mQsScrimEnabled = qsScrimEnabled;
    if (changed) {
        UpdateQsState();
    }
    return NOERROR;
}

ECode CNotificationPanelView::SetKeyguardUserSwitcher(
    /* [in] */ IKeyguardUserSwitcher* keyguardUserSwitcher)
{
    mKeyguardUserSwitcher = (KeyguardUserSwitcher*)keyguardUserSwitcher;
    return NOERROR;
}

ECode CNotificationPanelView::OnScreenTurnedOn()
{
    ((CKeyguardStatusView*)mKeyguardStatusView.Get())->RefreshTime();
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
