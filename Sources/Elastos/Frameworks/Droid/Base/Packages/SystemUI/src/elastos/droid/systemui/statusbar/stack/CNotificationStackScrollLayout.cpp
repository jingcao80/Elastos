
#include "elastos/droid/systemui/statusbar/stack/CNotificationStackScrollLayout.h"
#include "elastos/droid/systemui/statusbar/stack/AmbientState.h"
#include "elastos/droid/systemui/statusbar/stack/AnimationFilter.h"
#include "elastos/droid/systemui/statusbar/stack/StackScrollState.h"
#include "elastos/droid/systemui/statusbar/stack/StackScrollAlgorithm.h"
#include "elastos/droid/systemui/statusbar/stack/StackStateAnimator.h"
#include "elastos/droid/systemui/CExpandHelper.h"
#include "elastos/droid/systemui/CSwipeHelper.h"
#include "../R.h"
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::SystemUI::IGefingerpoken;
using Elastos::Droid::SystemUI::StatusBar::IStackScrollerDecorView;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IScrollAdapter;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::Widget::COverScroller;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

#define View Elastos::Droid::View::View
#define ViewState StackScrollState::ViewState

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

static AutoPtr<ArrayOf<IAnimationFilter*> > InitFILTERS()
{
    AutoPtr<ArrayOf<IAnimationFilter*> > filters = ArrayOf<IAnimationFilter*>::Alloc(14);

    // ANIMATION_TYPE_ADD
    AutoPtr<IAnimationFilter> filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateZ();
    filter->HasDelays();
    filters->Set(0, filter);

    // ANIMATION_TYPE_REMOVE
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateZ();
    filter->HasDelays();
    filters->Set(1, filter);

    // ANIMATION_TYPE_REMOVE_SWIPED_OUT
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateZ();
    filter->HasDelays();
    filters->Set(2, filter);

    // ANIMATION_TYPE_TOP_PADDING_CHANGED
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateDimmed();
    filter->AnimateScale();
    filter->AnimateZ();
    filters->Set(3, filter);

    // ANIMATION_TYPE_START_DRAG
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filters->Set(4, filter);

    // ANIMATION_TYPE_SNAP_BACK
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filters->Set(5, filter);

    // ANIMATION_TYPE_ACTIVATED_CHILD
    filter = new AnimationFilter();
    filter->AnimateScale();
    filter->AnimateAlpha();
    filters->Set(6, filter);

    // ANIMATION_TYPE_DIMMED
    filter = new AnimationFilter();
    filter->AnimateY();
    filter->AnimateScale();
    filter->AnimateDimmed();
    filters->Set(7, filter);

    // ANIMATION_TYPE_CHANGE_POSITION
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateZ();
    filters->Set(8, filter);

    // ANIMATION_TYPE_DARK
    filter = new AnimationFilter();
    filter->AnimateDark();
    filters->Set(9, filter);

    // ANIMATION_TYPE_GO_TO_FULL_SHADE
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateDimmed();
    filter->AnimateScale();
    filter->AnimateZ();
    filter->HasDelays();
    filters->Set(10, filter);

    // ANIMATION_TYPE_HIDE_SENSITIVE
    filter = new AnimationFilter();
    filter->AnimateHideSensitive();
    filters->Set(11, filter);

    // ANIMATION_TYPE_VIEW_RESIZE
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateZ();
    filters->Set(12, filter);

    // ANIMATION_TYPE_EVERYTHING
    filter = new AnimationFilter();
    filter->AnimateAlpha();
    filter->AnimateDark();
    filter->AnimateScale();
    filter->AnimateDimmed();
    filter->AnimateHideSensitive();
    filter->AnimateHeight();
    filter->AnimateTopInset();
    filter->AnimateY();
    filter->AnimateZ();
    filters->Set(13, filter);

    return filters;
}

AutoPtr<ArrayOf<IAnimationFilter*> > CNotificationStackScrollLayout::AnimationEvent::FILTERS = InitFILTERS();

Int32 CNotificationStackScrollLayout::AnimationEvent::LENGTHS[] = {
    // ANIMATION_TYPE_ADD
    IStackStateAnimator::ANIMATION_DURATION_APPEAR_DISAPPEAR,

    // ANIMATION_TYPE_REMOVE
    IStackStateAnimator::ANIMATION_DURATION_APPEAR_DISAPPEAR,

    // ANIMATION_TYPE_REMOVE_SWIPED_OUT
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_TOP_PADDING_CHANGED
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_START_DRAG
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_SNAP_BACK
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_ACTIVATED_CHILD
    IStackStateAnimator::ANIMATION_DURATION_DIMMED_ACTIVATED,

    // ANIMATION_TYPE_DIMMED
    IStackStateAnimator::ANIMATION_DURATION_DIMMED_ACTIVATED,

    // ANIMATION_TYPE_CHANGE_POSITION
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_DARK
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_GO_TO_FULL_SHADE
    IStackStateAnimator::ANIMATION_DURATION_GO_TO_FULL_SHADE,

    // ANIMATION_TYPE_HIDE_SENSITIVE
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_VIEW_RESIZE
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,

    // ANIMATION_TYPE_EVERYTHING
    IStackStateAnimator::ANIMATION_DURATION_STANDARD,
};

const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_ADD;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_REMOVE;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_REMOVE_SWIPED_OUT;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_TOP_PADDING_CHANGED;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_START_DRAG;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_SNAP_BACK;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_ACTIVATED_CHILD;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_DIMMED;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_CHANGE_POSITION;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_DARK;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_GO_TO_FULL_SHADE;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_HIDE_SENSITIVE;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_VIEW_RESIZE;
const Int32 CNotificationStackScrollLayout::AnimationEvent::ANIMATION_TYPE_EVERYTHING;

//============================================================================
// CNotificationStackScrollLayout::AnimationEvent
//============================================================================
CNotificationStackScrollLayout::AnimationEvent::AnimationEvent(
    /* [in] */ IView* view,
    /* [in] */ Int32 type)
    : mEventStartTime(0)
    , mAnimationType(type)
    , mLength(0)
{
    Init(view, type, LENGTHS[type]);
}

CNotificationStackScrollLayout::AnimationEvent::AnimationEvent(
    /* [in] */ IView* view,
    /* [in] */ Int32 type,
    /* [in] */ Int64 length)
    : mEventStartTime(0)
    , mAnimationType(type)
    , mLength(length)
{
    Init(view, type, length);
}

void CNotificationStackScrollLayout::AnimationEvent::Init(
    /* [in] */ IView* view,
    /* [in] */ Int32 type,
    /* [in] */ Int64 length)
{
    AnimationUtils::CurrentAnimationTimeMillis(&mEventStartTime);
    mChangingView = view;
    mAnimationType = type;
    mFilter = (*FILTERS)[type];
    mLength = length;
}

Int64 CNotificationStackScrollLayout::AnimationEvent::CombineLength(
    /* [in] */ IArrayList/*<AnimationEvent>*/* events)
{
    Int64 length = 0;
    Int32 size = 0;
    events->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        events->Get(i, (IInterface**)&obj);
        AutoPtr<AnimationEvent> event = (AnimationEvent*)IObject::Probe(obj);
        length = Elastos::Core::Math::Max(length, event->mLength);
        if (event->mAnimationType == ANIMATION_TYPE_GO_TO_FULL_SHADE) {
            return event->mLength;
        }
    }
    return length;
}

//============================================================================
// CNotificationStackScrollLayout::EmptyShadeViewGoneRunnable
//============================================================================
CNotificationStackScrollLayout::EmptyShadeViewGoneRunnable::EmptyShadeViewGoneRunnable(
    /* [in] */ CNotificationStackScrollLayout* host)
    : mHost(host)
{}

ECode CNotificationStackScrollLayout::EmptyShadeViewGoneRunnable::Run()
{
    IView::Probe(mHost->mEmptyShadeView)->SetVisibility(IView::GONE);
    IStackScrollerDecorView::Probe(mHost->mEmptyShadeView)->SetWillBeGone(FALSE);
    mHost->UpdateContentHeight();
    mHost->NotifyHeightChangeListener(IExpandableView::Probe(mHost->mDismissView));
    return NOERROR;
}

//============================================================================
// CNotificationStackScrollLayout::DismissViewGoneRunnable
//============================================================================
CNotificationStackScrollLayout::DismissViewGoneRunnable::DismissViewGoneRunnable(
    /* [in] */ CNotificationStackScrollLayout* host)
    : mHost(host)
{}

ECode CNotificationStackScrollLayout::DismissViewGoneRunnable::Run()
{
    IView::Probe(mHost->mDismissView)->SetVisibility(IView::GONE);
    IStackScrollerDecorView::Probe(mHost->mDismissView)->SetWillBeGone(FALSE);
    mHost->UpdateContentHeight();
    mHost->NotifyHeightChangeListener(IExpandableView::Probe(mHost->mDismissView));
    return NOERROR;
}

//============================================================================
// CNotificationStackScrollLayout::ChildrenUpdater
//============================================================================
CAR_INTERFACE_IMPL(CNotificationStackScrollLayout::ChildrenUpdater, Object, IOnPreDrawListener)

CNotificationStackScrollLayout::ChildrenUpdater::ChildrenUpdater(
    /* [in] */ CNotificationStackScrollLayout* host)
    : mHost(host)
{}

ECode CNotificationStackScrollLayout::ChildrenUpdater::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mHost->UpdateChildren();
    mHost->mChildrenUpdateRequested = FALSE;
    AutoPtr<IViewTreeObserver> vto;
    mHost->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(this);
    *result = TRUE;
    return NOERROR;
}

//============================================================================
// CNotificationStackScrollLayout
//============================================================================
const String CNotificationStackScrollLayout::TAG("NotificationStackScrollLayout");
const Boolean CNotificationStackScrollLayout::DEBUG = FALSE;
const Float CNotificationStackScrollLayout::RUBBER_BAND_FACTOR_NORMAL = 0.35f;
const Float CNotificationStackScrollLayout::RUBBER_BAND_FACTOR_AFTER_EXPAND = 0.15f;
const Float CNotificationStackScrollLayout::RUBBER_BAND_FACTOR_ON_PANEL_EXPAND = 0.21f;
const Int32 CNotificationStackScrollLayout::INVALID_POINTER = -1;

CAR_INTERFACE_IMPL_5(CNotificationStackScrollLayout, ViewGroup, INotificationStackScrollLayout \
    , ISwipeHelperCallback, IExpandHelperCallback, IScrollAdapter, IExpandableViewOnHeightChangedListener)

CAR_OBJECT_IMPL(CNotificationStackScrollLayout)

CNotificationStackScrollLayout::CNotificationStackScrollLayout()
    : mSwipingInProgress(FALSE)
    , mCurrentStackHeight(Elastos::Core::Math::INT32_MAX_VALUE)
    , mOwnScrollY(0)
    , mMaxLayoutHeight(0)
    , mTouchSlop(0)
    , mMinimumVelocity(0)
    , mMaximumVelocity(0)
    , mOverflingDistance(0)
    , mMaxOverScroll(0)
    , mIsBeingDragged(FALSE)
    , mLastMotionY(0)
    , mDownX(0)
    , mActivePointerId(0)
    , mSidePaddings(0)
    , mContentHeight(0)
    , mCollapsedSize(0)
    , mBottomStackSlowDownHeight(0)
    , mBottomStackPeekSize(0)
    , mPaddingBetweenElements(0)
    , mPaddingBetweenElementsDimmed(0)
    , mPaddingBetweenElementsNormal(0)
    , mTopPadding(0)
    , mCollapseSecondCardPadding(0)
    , mAnimationsEnabled(FALSE)
    , mChangePositionInProgress(FALSE)
    , mOverScrolledTopPixels(0)
    , mOverScrolledBottomPixels(0)
    , mNeedsAnimation(FALSE)
    , mTopPaddingNeedsAnimation(FALSE)
    , mDimmedNeedsAnimation(FALSE)
    , mHideSensitiveNeedsAnimation(FALSE)
    , mDarkNeedsAnimation(FALSE)
    , mActivateNeedsAnimation(FALSE)
    , mGoToFullShadeNeedsAnimation(FALSE)
    , mIsExpanded(TRUE)
    , mChildrenUpdateRequested(FALSE)
    , mIsExpansionChanging(FALSE)
    , mExpandingNotification(FALSE)
    , mExpandedInThisMotion(FALSE)
    , mScrollingEnabled(FALSE)
    , mDismissAllInProgress(FALSE)
    , mScrolledToTopOnFirstDown(FALSE)
    , mMinTopOverScrollToEscape(0)
    , mIntrinsicPadding(0)
    , mNotificationTopPadding(0)
    , mTopPaddingOverflow(0)
    , mDontReportNextOverScroll(FALSE)
    , mRequestViewResizeAnimationOnLayout(FALSE)
    , mNeedViewResizeAnimation(FALSE)
    , mEverythingNeedsAnimation(FALSE)
    , mMaxScrollAfterExpand(0)
    , mOnlyScrollingInThisMotion(FALSE)
    , mInterceptDelegateEnabled(FALSE)
    , mDelegateToScrollView(FALSE)
    , mDisallowScrollingInThisMotion(FALSE)
    , mGoToFullShadeDelay(0)
{
    mAmbientState = new AmbientState();
    CArrayList::New((IArrayList**)&mChildrenToAddAnimated);
    CArrayList::New((IArrayList**)&mChildrenToRemoveAnimated);
    CArrayList::New((IArrayList**)&mSnappedBackChildren);
    CArrayList::New((IArrayList**)&mDragAnimPendingChildren);
    CArrayList::New((IArrayList**)&mChildrenChangingPositions);
    CHashSet::New((IHashSet**)&mFromMoreCardAdditions);
    CArrayList::New((IArrayList**)&mAnimationEvents);
    CArrayList::New((IArrayList**)&mSwipedOutViews);
}

ECode CNotificationStackScrollLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CNotificationStackScrollLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CNotificationStackScrollLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CNotificationStackScrollLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes);
    mCurrentStackScrollState = new StackScrollState(this);
    mStateAnimator = new StackStateAnimator(this);
    mChildrenUpdater = new ChildrenUpdater(this);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 minHeight = 0;
    res->GetDimensionPixelSize(R::dimen::notification_min_height, &minHeight);
    Int32 maxHeight = 0;
    res->GetDimensionPixelSize(R::dimen::notification_max_height, &maxHeight);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CExpandHelper::New(ctx, this, minHeight, maxHeight, (IExpandHelper**)&mExpandHelper);
    mExpandHelper->SetEventSource(this);
    mExpandHelper->SetScrollAdapter(this);

    CSwipeHelper::New(CSwipeHelper::X, this, ctx, (ISwipeHelper**)&mSwipeHelper);
    mSwipeHelper->SetLongPressListener(mLongPressListener);
    InitView(context);
    if (DEBUG) {
        SetWillNotDraw(FALSE);
        CPaint::New((IPaint**)&mDebugPaint);
        mDebugPaint->SetColor(0xffff0000);
        mDebugPaint->SetStrokeWidth(2);
        mDebugPaint->SetStyle(PaintStyle_STROKE);
    }
    return NOERROR;
}

void CNotificationStackScrollLayout::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (DEBUG) {
        Int32 y = mCollapsedSize;
        Int32 w = 0;
        GetWidth(&w);
        canvas->DrawLine(0, y, w, y, mDebugPaint);

        y = (Int32) (GetLayoutHeight() - mBottomStackPeekSize - mBottomStackSlowDownHeight);
        canvas->DrawLine(0, y, w, y, mDebugPaint);

        y = (Int32) (GetLayoutHeight() - mBottomStackPeekSize);
        canvas->DrawLine(0, y, w, y, mDebugPaint);

        y = (Int32) GetLayoutHeight();
        canvas->DrawLine(0, y, w, y, mDebugPaint);

        Int32 h = 0, bm = 0;
        y = (GetHeight(&h), h) - (GetEmptyBottomMargin(&bm), bm);
        canvas->DrawLine(0, y, w, y, mDebugPaint);
    }
}

void CNotificationStackScrollLayout::InitView(
    /* [in] */ IContext* context)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    COverScroller::New(ctx, (IOverScroller**)&mScroller);
    SetFocusable(TRUE);
    SetDescendantFocusability(FOCUS_AFTER_DESCENDANTS);
    SetClipChildren(FALSE);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(context, (IViewConfiguration**)&configuration);
    configuration->GetScaledTouchSlop(&mTouchSlop);
    configuration->GetScaledMinimumFlingVelocity(&mMinimumVelocity);
    configuration->GetScaledMaximumFlingVelocity(&mMaximumVelocity);
    configuration->GetScaledOverflingDistance(&mOverflingDistance);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::notification_side_padding, &mSidePaddings);
    res->GetDimensionPixelSize(R::dimen::notification_min_height, &mCollapsedSize);
    res->GetDimensionPixelSize(R::dimen::bottom_stack_peek_amount, &mBottomStackPeekSize);
    mStackScrollAlgorithm = new StackScrollAlgorithm(context);

    Boolean isDimmed = FALSE;
    mAmbientState->IsDimmed(&isDimmed);
    mStackScrollAlgorithm->SetDimmed(isDimmed);
    res->GetDimensionPixelSize(R::dimen::notification_padding_dimmed, &mPaddingBetweenElementsDimmed);
    res->GetDimensionPixelSize(R::dimen::notification_padding, &mPaddingBetweenElementsNormal);
    UpdatePadding(isDimmed);
    res = NULL;
    GetResources((IResources**)&res);
    Int32 tmp = 0;
    res->GetDimensionPixelSize(R::dimen::min_top_overscroll_to_qs, &tmp);
    mMinTopOverScrollToEscape = tmp;
    res->GetDimensionPixelSize(R::dimen::notifications_top_padding, &mNotificationTopPadding);
    res->GetDimensionPixelSize(R::dimen::notification_collapse_second_card_padding, &mCollapseSecondCardPadding);
}

void CNotificationStackScrollLayout::UpdatePadding(
    /* [in] */ Boolean dimmed)
{
    Boolean tmp = FALSE;
    mStackScrollAlgorithm->ShouldScaleDimmed(&tmp);
    mPaddingBetweenElements = dimmed && tmp
            ? mPaddingBetweenElementsDimmed
            : mPaddingBetweenElementsNormal;
    mStackScrollAlgorithm->GetBottomStackSlowDownLength(&mBottomStackSlowDownHeight);
    UpdateContentHeight();
    NotifyHeightChangeListener(NULL);
}

void CNotificationStackScrollLayout::NotifyHeightChangeListener(
    /* [in] */ IExpandableView* view)
{
    if (mOnHeightChangedListener != NULL) {
        mOnHeightChangedListener->OnHeightChanged(view);
    }
}

ECode CNotificationStackScrollLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    Int32 mode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 size = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 childMeasureSpec = View::MeasureSpec::MakeMeasureSpec(size - 2 * mSidePaddings, mode);
    MeasureChildren(childMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // we layout all our children centered on the top
    Int32 value = 0;
    GetWidth(&value);
    Float centerX = value / 2.0f;
    GetChildCount(&value);
    Int32 width, height;
    for (Int32 i = 0; i < value; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        child->GetMeasuredWidth(&width);
        child->GetMeasuredHeight(&height);
        child->Layout((Int32) (centerX - width / 2.0f),
                0,
                (Int32) (centerX + width / 2.0f),
                (Int32) height);
    }

    GetHeight(&value);
    SetMaxLayoutHeight(value);
    UpdateContentHeight();
    ClampScrollPosition();
    RequestAnimationOnViewResize();
    RequestChildrenUpdate();
    return NOERROR;
}

void CNotificationStackScrollLayout::RequestAnimationOnViewResize()
{
    if (mRequestViewResizeAnimationOnLayout && mIsExpanded && mAnimationsEnabled) {
        mNeedViewResizeAnimation = TRUE;
        mNeedsAnimation = TRUE;
    }
    mRequestViewResizeAnimationOnLayout = FALSE;
}

ECode CNotificationStackScrollLayout::UpdateSpeedBumpIndex(
    /* [in] */ Int32 newIndex)
{
    Int32 currentIndex = 0;
    IndexOfChild(IView::Probe(mSpeedBumpView), &currentIndex);

    // If we are currently layouted before the new speed bump index, we have to decrease it.
    Boolean validIndex = newIndex > 0;
    Int32 value = 0;
    GetChildCount(&value);
    if (newIndex > value - 1) {
        validIndex = FALSE;
        newIndex = -1;
    }
    if (validIndex && currentIndex != newIndex) {
        ChangeViewPosition(IView::Probe(mSpeedBumpView), newIndex);
    }
    UpdateSpeedBump(validIndex);
    mAmbientState->SetSpeedBumpIndex(newIndex);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetChildLocationsChangedListener(
    /* [in] */ IOnChildLocationsChangedListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetChildLocation(
    /* [in] */ IView* child,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mCurrentStackScrollState->GetViewStateForView(child, (IInterface**)&obj);
    AutoPtr<ViewState> childViewState = (ViewState*)IObject::Probe(obj);
    if (childViewState == NULL) {
        *result = ViewState::LOCATION_UNKNOWN;
        return NOERROR;
    }
    *result = childViewState->mLocation;
    return NOERROR;
}

void CNotificationStackScrollLayout::SetMaxLayoutHeight(
    /* [in] */ Int32 maxLayoutHeight)
{
    mMaxLayoutHeight = maxLayoutHeight;
    UpdateAlgorithmHeightAndPadding();
}

void CNotificationStackScrollLayout::UpdateAlgorithmHeightAndPadding()
{
    mStackScrollAlgorithm->SetLayoutHeight(GetLayoutHeight());
    mStackScrollAlgorithm->SetTopPadding(mTopPadding);
}

Boolean CNotificationStackScrollLayout::NeedsHeightAdaption()
{
    Int32 count = 0;
    return (GetNotGoneChildCount(&count), count) > 1;
}

void CNotificationStackScrollLayout::UpdateChildren()
{
    mAmbientState->SetScrollY(mOwnScrollY);
    mStackScrollAlgorithm->GetStackScrollState(mAmbientState, mCurrentStackScrollState);
    if (!IsCurrentlyAnimating() && !mNeedsAnimation) {
        ApplyCurrentState();
    }
    else {
        StartAnimationToState();
    }
}

void CNotificationStackScrollLayout::RequestChildrenUpdate()
{
    if (!mChildrenUpdateRequested) {
        AutoPtr<IViewTreeObserver> vto;
        GetViewTreeObserver((IViewTreeObserver**)&vto);
        vto->AddOnPreDrawListener(mChildrenUpdater);
        mChildrenUpdateRequested = TRUE;
        Invalidate();
    }
}

Boolean CNotificationStackScrollLayout::IsCurrentlyAnimating()
{
    Boolean tmp = FALSE;
    return (mStateAnimator->IsRunning(&tmp), tmp);
}

void CNotificationStackScrollLayout::ClampScrollPosition()
{
    Int32 scrollRange = GetScrollRange();
    if (scrollRange < mOwnScrollY) {
        mOwnScrollY = scrollRange;
    }
}

ECode CNotificationStackScrollLayout::GetTopPadding(
    /* [out] */ Int32* padding)
{
    VALIDATE_NOT_NULL(padding);
    *padding = mTopPadding;
    return NOERROR;
}

void CNotificationStackScrollLayout::SetTopPadding(
    /* [in] */ Int32 topPadding,
    /* [in] */ Boolean animate)
{
    if (mTopPadding != topPadding) {
        mTopPadding = topPadding;
        UpdateAlgorithmHeightAndPadding();
        UpdateContentHeight();
        if (animate && mAnimationsEnabled && mIsExpanded) {
            mTopPaddingNeedsAnimation = TRUE;
            mNeedsAnimation =  TRUE;
        }
        RequestChildrenUpdate();
        NotifyHeightChangeListener(NULL);
    }
}

ECode CNotificationStackScrollLayout::SetStackHeight(
    /* [in] */ Float height)
{
    SetIsExpanded(height > 0.0f);
    Int32 newStackHeight = (Int32) height;
    Int32 minStackHeight = 0;
    GetMinStackHeight(&minStackHeight);
    Int32 stackHeight = 0, value = 0;
    if (newStackHeight - mTopPadding >= minStackHeight || (GetNotGoneChildCount(&value), value) == 0) {
        SetTranslationY(mTopPaddingOverflow);
        stackHeight = newStackHeight;
    }
    else {
        // We did not reach the position yet where we actually start growing,
        // so we translate the stack upwards.
        Int32 translationY = (newStackHeight - minStackHeight);
        // A slight parallax effect is introduced in order for the stack to catch up with
        // the top card.
        Float partiallyThere = (newStackHeight - mTopPadding) * 1.0f / minStackHeight;
        partiallyThere = Elastos::Core::Math::Max((Float)0, partiallyThere);
        translationY += (1 - partiallyThere) * (mBottomStackPeekSize +
                mCollapseSecondCardPadding);
        SetTranslationY(translationY - mTopPadding);
        stackHeight = (Int32) (height - (translationY - mTopPadding));
    }

    if (stackHeight != mCurrentStackHeight) {
        mCurrentStackHeight = stackHeight;
        UpdateAlgorithmHeightAndPadding();
        RequestChildrenUpdate();
    }
    return NOERROR;
}

Int32 CNotificationStackScrollLayout::GetLayoutHeight()
{
    return Elastos::Core::Math::Min(mMaxLayoutHeight, mCurrentStackHeight);
}

ECode CNotificationStackScrollLayout::GetItemHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCollapsedSize;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetBottomStackPeekSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBottomStackPeekSize;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetCollapseSecondCardPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCollapseSecondCardPadding;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetLongPressListener(
    /* [in] */ ISwipeHelperLongPressListener* listener)
{
    mSwipeHelper->SetLongPressListener(listener);
    mLongPressListener = listener;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetScrollView(
    /* [in] */ IViewGroup* scrollView)
{
    mScrollView = scrollView;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetInterceptDelegateEnabled(
    /* [in] */ Boolean interceptDelegateEnabled)
{
    mInterceptDelegateEnabled = interceptDelegateEnabled;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnChildDismissed(
    /* [in] */ IView* v)
{
    if (mDismissAllInProgress) {
        return NOERROR;
    }
    if (DEBUG) Logger::V(TAG, "onChildDismissed: %s", TO_CSTR(v));
    AutoPtr<IView> veto;
    v->FindViewById(R::id::veto, (IView**)&veto);
    Int32 value = 0;
    if (veto != NULL && (veto->GetVisibility(&value), value) != IView::GONE) {
        Boolean tmp = FALSE;
        veto->PerformClick(&tmp);
    }
    SetSwipingInProgress(FALSE);
    Boolean contains = FALSE;
    if (mDragAnimPendingChildren->Contains(v, &contains), contains) {
        // We start the swipe and finish it in the same frame, we don't want any animation
        // for the drag
        mDragAnimPendingChildren->Remove(v);
    }
    mSwipedOutViews->Add(v);
    mAmbientState->OnDragFinished(v);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnChildSnappedBack(
    /* [in] */ IView* animView)
{
    mAmbientState->OnDragFinished(animView);
    Boolean contains = FALSE;
    if (mDragAnimPendingChildren->Contains(animView, &contains), !contains) {
        if (mAnimationsEnabled) {
            mSnappedBackChildren->Add(animView);
            mNeedsAnimation = TRUE;
        }
        RequestChildrenUpdate();
    }
    else {
        // We start the swipe and snap back in the same frame, we don't want any animation
        mDragAnimPendingChildren->Remove(animView);
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::UpdateSwipeProgress(
    /* [in] */ IView* animView,
    /* [in] */ Boolean dismissable,
    /* [in] */ Float swipeProgress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetFalsingThresholdFactor(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    mPhoneStatusBar->IsScreenOnComingFromTouch(&tmp);
    *result = tmp ? 1.5f : 1.0f;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnBeginDrag(
    /* [in] */ IView* v)
{
    SetSwipingInProgress(TRUE);
    mAmbientState->OnBeginDrag(v);
    if (mAnimationsEnabled) {
        mDragAnimPendingChildren->Add(v);
        mNeedsAnimation = TRUE;
    }
    RequestChildrenUpdate();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnDragCancelled(
    /* [in] */ IView* v)
{
    SetSwipingInProgress(FALSE);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    Float x = 0, y = 0;
    ev->GetX(&x);
    ev->GetY(&y);
    AutoPtr<IExpandableView> v;
    GetChildAtPosition(x, y, (IExpandableView**)&v);
    *view = IView::Probe(v);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetChildAtRawPosition(
    /* [in] */ Float touchX,
    /* [in] */ Float touchY,
    /* [out] */ IExpandableView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<ArrayOf<Int32> > location = ArrayOf<Int32>::Alloc(2);
    GetLocationOnScreen(location);
    return GetChildAtPosition(touchX - (*location)[0], touchY - (*location)[1], view);
}

ECode CNotificationStackScrollLayout::GetChildAtPosition(
    /* [in] */ Float touchX,
    /* [in] */ Float touchY,
    /* [out] */ IExpandableView** view)
{
    VALIDATE_NOT_NULL(view);
    // find the view under the pointer, accounting for GONE views
    Int32 count = 0;
    GetChildCount(&count);
    for (Int32 childIdx = 0; childIdx < count; childIdx++) {
        AutoPtr<IView> obj;
        GetChildAt(childIdx, (IView**)&obj);
        AutoPtr<IExpandableView> slidingChild = IExpandableView::Probe(obj);
        Int32 v = 0;
        if ((IView::Probe(slidingChild)->GetVisibility(&v), v) == IView::GONE) {
            continue;
        }
        Float childTop = 0;
        Int32 iv = 0;
        IView::Probe(slidingChild)->GetTranslationY(&childTop);
        Float top = childTop + (slidingChild->GetClipTopAmount(&iv), iv);
        Float bottom = childTop + (slidingChild->GetActualHeight(&iv), iv);

        // Allow the full width of this view to prevent gesture conflict on Keyguard (phone and
        // camera affordance).
        Int32 left = 0;
        Int32 right = 0;
        GetWidth(&right);

        if (touchY >= top && touchY <= bottom && touchX >= left && touchX <= right) {
            *view = slidingChild;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    *view = NULL;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::CanChildBeExpanded(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean tmp = FALSE;
    *result = IExpandableNotificationRow::Probe(v)
            && (IExpandableNotificationRow::Probe(v)->IsExpandable(&tmp), tmp);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetUserExpandedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userExpanded)
{
    if (IExpandableNotificationRow::Probe(v)) {
        IExpandableNotificationRow::Probe(v)->SetUserExpanded(userExpanded);
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetUserLockedChild(
    /* [in] */ IView* v,
    /* [in] */ Boolean userLocked)
{
    if (IExpandableNotificationRow::Probe(v)) {
        IExpandableNotificationRow::Probe(v)->SetUserLocked(userLocked);
    }
    RemoveLongPressCallback();
    RequestDisallowInterceptTouchEvent(TRUE);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::ExpansionStateChanged(
    /* [in] */ Boolean isExpanding)
{
    mExpandingNotification = isExpanding;
    if (!mExpandedInThisMotion) {
        mMaxScrollAfterExpand = mOwnScrollY;
        mExpandedInThisMotion = TRUE;
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetScrollingEnabled(
    /* [in] */ Boolean enable)
{
    mScrollingEnabled = enable;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetExpandingEnabled(
    /* [in] */ Boolean enable)
{
    mExpandHelper->SetEnabled(enable);
    return NOERROR;
}

Boolean CNotificationStackScrollLayout::IsScrollingEnabled()
{
    return mScrollingEnabled;
}

ECode CNotificationStackScrollLayout::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = v;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode CNotificationStackScrollLayout::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    AutoPtr<IView> veto;
    v->FindViewById(R::id::veto, (IView**)&veto);
    Int32 value = 0;
    *can = (veto != NULL && (veto->GetVisibility(&value), value) != IView::GONE);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::IsAntiFalsingNeeded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 s = 0;
    *result = (mPhoneStatusBar->GetBarState(&s), s) == IStatusBarState::KEYGUARD;
    return NOERROR;
}

void CNotificationStackScrollLayout::SetSwipingInProgress(
    /* [in] */ Boolean isSwiped)
{
    mSwipingInProgress = isSwiped;
    if(isSwiped) {
        RequestDisallowInterceptTouchEvent(TRUE);
    }
}

ECode CNotificationStackScrollLayout::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    ViewGroup::OnConfigurationChanged(newConfig);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float densityScale = 0;
    dm->GetDensity(&densityScale);
    mSwipeHelper->SetDensityScale(densityScale);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IViewConfiguration> cfg;
    helper->Get(ctx, (IViewConfiguration**)&cfg);
    Int32 pagingTouchSlop = 0;
    cfg->GetScaledPagingTouchSlop(&pagingTouchSlop);
    mSwipeHelper->SetPagingTouchSlop(pagingTouchSlop);
    InitView(ctx);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::DismissViewAnimated(
    /* [in] */ IView* child,
    /* [in] */ IRunnable* endRunnable,
    /* [in] */ Int32 delay,
    /* [in] */ Int64 duration)
{
    child->SetClipBounds(NULL);
    mSwipeHelper->DismissChild(child, 0, endRunnable, delay, TRUE, duration);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 masked = 0;
    ev->GetActionMasked(&masked);
    Boolean isCancelOrUp = masked == IMotionEvent::ACTION_CANCEL
            || masked == IMotionEvent::ACTION_UP;
    if (mDelegateToScrollView) {
        if (isCancelOrUp) {
            mDelegateToScrollView = FALSE;
        }
        TransformTouchEvent(ev, this, IView::Probe(mScrollView));
        return IView::Probe(mScrollView)->OnTouchEvent(ev, result);
    }
    Boolean expandWantsIt = FALSE;
    if (!mSwipingInProgress && !mOnlyScrollingInThisMotion && IsScrollingEnabled()) {
        if (isCancelOrUp) {
            mExpandHelper->OnlyObserveMovements(FALSE);
        }
        Boolean wasExpandingBefore = mExpandingNotification;
        IGefingerpoken::Probe(mExpandHelper)->OnTouchEvent(ev, &expandWantsIt);
        if (mExpandedInThisMotion && !mExpandingNotification && wasExpandingBefore
                && !mDisallowScrollingInThisMotion) {
            DispatchDownEventToScroller(ev);
        }
    }
    Boolean scrollerWantsIt = FALSE;
    if (!mSwipingInProgress && !mExpandingNotification && !mDisallowScrollingInThisMotion) {
        scrollerWantsIt = OnScrollTouch(ev);
    }
    Boolean horizontalSwipeWantsIt = FALSE;
    if (!mIsBeingDragged
            && !mExpandingNotification
            && !mExpandedInThisMotion
            && !mOnlyScrollingInThisMotion) {
        IGefingerpoken::Probe(mSwipeHelper)->OnTouchEvent(ev, &horizontalSwipeWantsIt);
    }

    Boolean tmp = FALSE;
    *result = horizontalSwipeWantsIt || scrollerWantsIt
                || expandWantsIt || (ViewGroup::OnTouchEvent(ev, &tmp), tmp);
    return NOERROR;
}

void CNotificationStackScrollLayout::DispatchDownEventToScroller(
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> downEvent;
    helper->Obtain(ev, (IMotionEvent**)&downEvent);
    downEvent->SetAction(IMotionEvent::ACTION_DOWN);
    OnScrollTouch(downEvent);
    IInputEvent::Probe(downEvent)->Recycle();
}

Boolean CNotificationStackScrollLayout::OnScrollTouch(
    /* [in] */ IMotionEvent* ev)
{
    if (!IsScrollingEnabled()) {
        return FALSE;
    }
    InitVelocityTrackerIfNotExists();
    mVelocityTracker->AddMovement(ev);

    Int32 action = 0;
    ev->GetAction(&action);

    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN: {
            Int32 count = 0;
            if ((GetChildCount(&count), count) == 0 || !IsInContentBounds(ev)) {
                return FALSE;
            }
            Boolean finished = FALSE;
            Boolean isBeingDragged = !(mScroller->IsFinished(&finished), finished);
            SetIsBeingDragged(isBeingDragged);

            /*
             * If being flinged and user touches, stop the fling. isFinished
             * will be FALSE if being flinged.
             */
            if (!finished) {
                mScroller->ForceFinished(TRUE);
            }

            // Remember where the motion event started
            Float fv = 0;
            mLastMotionY = (Int32) (ev->GetY(&fv), fv);
            mDownX = (Int32) (ev->GetX(&fv), fv);
            ev->GetPointerId(0, &mActivePointerId);
            break;
        }
        case IMotionEvent::ACTION_MOVE: {
            Int32 activePointerIndex = 0;
            ev->FindPointerIndex(mActivePointerId, &activePointerIndex);
            if (activePointerIndex == -1) {
                Logger::E(TAG, "Invalid pointerId=%d in onTouchEvent", mActivePointerId);
                break;
            }

            Float fv = 0;
            const Int32 y = (Int32) (ev->GetY(activePointerIndex, &fv), fv);
            const Int32 x = (Int32) (ev->GetX(activePointerIndex, &fv), fv);
            Int32 deltaY = mLastMotionY - y;
            const Int32 xDiff = Elastos::Core::Math::Abs(x - mDownX);
            const Int32 yDiff = Elastos::Core::Math::Abs(deltaY);
            if (!mIsBeingDragged && yDiff > mTouchSlop && yDiff > xDiff) {
                SetIsBeingDragged(TRUE);
                if (deltaY > 0) {
                    deltaY -= mTouchSlop;
                }
                else {
                    deltaY += mTouchSlop;
                }
            }
            if (mIsBeingDragged) {
                // Scroll to follow the motion event
                mLastMotionY = y;
                Int32 range = GetScrollRange();
                if (mExpandedInThisMotion) {
                    range = Elastos::Core::Math::Min(range, mMaxScrollAfterExpand);
                }

                Float scrollAmount;
                if (deltaY < 0) {
                    scrollAmount = OverScrollDown(deltaY);
                }
                else {
                    scrollAmount = OverScrollUp(deltaY, range);
                }

                // Calling OverScrollBy will call OnOverScrolled, which
                // calls OnScrollChanged if applicable.
                if (scrollAmount != 0.0f) {
                    // The scrolling motion could not be compensated with the
                    // existing overScroll, we have to scroll the view
                    Int32 h = 0;
                    GetHeight(&h);
                    OverScrollBy(0, (Int32) scrollAmount, 0, mOwnScrollY,
                            0, range, 0, h / 2, TRUE);
                }
            }
            break;
        }
        case IMotionEvent::ACTION_UP:
            if (mIsBeingDragged) {
                AutoPtr<IVelocityTracker> velocityTracker = mVelocityTracker;
                velocityTracker->ComputeCurrentVelocity(1000, mMaximumVelocity);
                Float fv = 0;
                velocityTracker->GetYVelocity(mActivePointerId, &fv);
                Int32 initialVelocity = (Int32) fv;

                if (ShouldOverScrollFling(initialVelocity)) {
                    OnOverScrollFling(TRUE, initialVelocity);
                }
                else {
                    Int32 count = 0;
                    if ((GetChildCount(&count), count) > 0) {
                        if ((Elastos::Core::Math::Abs(initialVelocity) > mMinimumVelocity)) {
                            Float currentOverScrollTop = 0;
                            GetCurrentOverScrollAmount(TRUE, &currentOverScrollTop);
                            if (currentOverScrollTop == 0.0f || initialVelocity > 0) {
                                Fling(-initialVelocity);
                            }
                            else {
                                OnOverScrollFling(FALSE, initialVelocity);
                            }
                        }
                        else {
                            Boolean tmp = FALSE;
                            mScroller->SpringBack(mScrollX, mOwnScrollY, 0, 0, 0, GetScrollRange(), &tmp);
                            if (tmp) {
                                PostInvalidateOnAnimation();
                            }
                        }
                    }
                }

                mActivePointerId = INVALID_POINTER;
                EndDrag();
            }

            break;
        case IMotionEvent::ACTION_CANCEL: {
            Int32 count = 0;
            if (mIsBeingDragged && (GetChildCount(&count), count) > 0) {
                Boolean tmp = FALSE;
                if (mScroller->SpringBack(mScrollX, mOwnScrollY, 0, 0, 0, GetScrollRange(), &tmp), tmp) {
                    PostInvalidateOnAnimation();
                }
                mActivePointerId = INVALID_POINTER;
                EndDrag();
            }
            break;
        }
        case IMotionEvent::ACTION_POINTER_DOWN: {
            Int32 index = 0;
            ev->GetActionIndex(&index);
            Float fv = 0;
            mLastMotionY = (Int32) (ev->GetY(index, &fv), fv);
            mDownX = (Int32) (ev->GetX(index, &fv), fv);
            ev->GetPointerId(index, &mActivePointerId);
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 iv = 0;
            ev->FindPointerIndex(mActivePointerId, &iv);
            OnSecondaryPointerUp(ev);
            Float fv = 0;
            mLastMotionY = (Int32) (ev->GetY(iv, &fv), fv);
            mDownX = (Int32) (ev->GetX(iv, &fv), fv);
            break;
        }
    }
    return TRUE;
}

void CNotificationStackScrollLayout::OnOverScrollFling(
    /* [in] */ Boolean open,
    /* [in] */ Int32 initialVelocity)
{
    if (mOverscrollTopChangedListener != NULL) {
        mOverscrollTopChangedListener->FlingTopOverscroll(initialVelocity, open);
    }
    mDontReportNextOverScroll = TRUE;
    SetOverScrollAmount(0.0f, TRUE, FALSE);
}

Float CNotificationStackScrollLayout::OverScrollUp(
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 range)
{
    deltaY = Elastos::Core::Math::Max(deltaY, 0);
    Float currentTopAmount = 0;
    GetCurrentOverScrollAmount(TRUE, &currentTopAmount);
    Float newTopAmount = currentTopAmount - deltaY;
    if (currentTopAmount > 0) {
        SetOverScrollAmount(newTopAmount, TRUE /* onTop */,
                FALSE /* animate */);
    }
    // Top overScroll might not grab all scrolling motion,
    // we have to scroll as well.
    Float scrollAmount = newTopAmount < 0 ? -newTopAmount : 0.0f;
    Float newScrollY = mOwnScrollY + scrollAmount;
    if (newScrollY > range) {
        if (!mExpandedInThisMotion) {
            Float currentBottomPixels = 0;
            GetCurrentOverScrolledPixels(FALSE, &currentBottomPixels);
            // We overScroll on the top
            SetOverScrolledPixels(currentBottomPixels + newScrollY - range,
                    FALSE /* onTop */,
                    FALSE /* animate */);
        }
        mOwnScrollY = range;
        scrollAmount = 0.0f;
    }
    return scrollAmount;
}

Float CNotificationStackScrollLayout::OverScrollDown(
    /* [in] */ Int32 deltaY)
{
    deltaY = Elastos::Core::Math::Min(deltaY, 0);
    Float currentBottomAmount = 0;
    GetCurrentOverScrollAmount(FALSE, &currentBottomAmount);
    Float newBottomAmount = currentBottomAmount + deltaY;
    if (currentBottomAmount > 0) {
        SetOverScrollAmount(newBottomAmount, FALSE /* onTop */,
                FALSE /* animate */);
    }
    // Bottom overScroll might not grab all scrolling motion,
    // we have to scroll as well.
    Float scrollAmount = newBottomAmount < 0 ? newBottomAmount : 0.0f;
    Float newScrollY = mOwnScrollY + scrollAmount;
    if (newScrollY < 0) {
        Float currentTopPixels = 0;
        GetCurrentOverScrolledPixels(TRUE, &currentTopPixels);
        // We overScroll on the top
        SetOverScrolledPixels(currentTopPixels - newScrollY,
                TRUE /* onTop */,
                FALSE /* animate */);
        mOwnScrollY = 0;
        scrollAmount = 0.0f;
    }
    return scrollAmount;
}

void CNotificationStackScrollLayout::OnSecondaryPointerUp(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action = 0;
    ev->GetAction(&action);
    const Int32 pointerIndex = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK) >>
            IMotionEvent::ACTION_POINTER_INDEX_SHIFT;
    Int32 pointerId = 0;
    ev->GetPointerId(pointerIndex, &pointerId);
    if (pointerId == mActivePointerId) {
        // This was our active pointer going up. Choose a new
        // active pointer and adjust accordingly.
        // TODO: Make this decision more intelligent.
        const Int32 newPointerIndex = pointerIndex == 0 ? 1 : 0;
        Float fv = 0;
        mLastMotionY = (Int32) (ev->GetY(newPointerIndex, &fv), fv);
        ev->GetPointerId(newPointerIndex, &mActivePointerId);
        if (mVelocityTracker != NULL) {
            mVelocityTracker->Clear();
        }
    }
}

void CNotificationStackScrollLayout::InitVelocityTrackerIfNotExists()
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
}

void CNotificationStackScrollLayout::RecycleVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

void CNotificationStackScrollLayout::InitOrResetVelocityTracker()
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    else {
        mVelocityTracker->Clear();
    }
}

ECode CNotificationStackScrollLayout::ComputeScroll()
{
    Boolean tmp = FALSE;
    if (mScroller->ComputeScrollOffset(&tmp), tmp) {
        // This is called at drawing time by ViewGroup.
        Int32 oldX = mScrollX;
        Int32 oldY = mOwnScrollY;
        Int32 x = 0;
        mScroller->GetCurrX(&x);
        Int32 y = 0;
        mScroller->GetCurrY(&y);

        if (oldX != x || oldY != y) {
            const Int32 range = GetScrollRange();
            if ((y < 0 && oldY >= 0) || (y > range && oldY <= range)) {
                Float currVelocity = 0;
                mScroller->GetCurrVelocity(&currVelocity);
                if (currVelocity >= mMinimumVelocity) {
                    mMaxOverScroll = Elastos::Core::Math::Abs(currVelocity) / 1000 * mOverflingDistance;
                }
            }

            OverScrollBy(x - oldX, y - oldY, oldX, oldY, 0, range,
                    0, (Int32) (mMaxOverScroll), FALSE);
            OnScrollChanged(mScrollX, mOwnScrollY, oldX, oldY);
        }

        // Keep on drawing until the animation has finished.
        PostInvalidateOnAnimation();
    }
    return NOERROR;
}

Boolean CNotificationStackScrollLayout::OverScrollBy(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY,
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Int32 scrollRangeX,
    /* [in] */ Int32 scrollRangeY,
    /* [in] */ Int32 maxOverScrollX,
    /* [in] */ Int32 maxOverScrollY,
    /* [in] */ Boolean isTouchEvent)
{
    Int32 newScrollY = scrollY + deltaY;

    const Int32 top = -maxOverScrollY;
    const Int32 bottom = maxOverScrollY + scrollRangeY;

    Boolean clampedY = FALSE;
    if (newScrollY > bottom) {
        newScrollY = bottom;
        clampedY = TRUE;
    }
    else if (newScrollY < top) {
        newScrollY = top;
        clampedY = TRUE;
    }

    OnOverScrolled(0, newScrollY, FALSE, clampedY);

    return clampedY;
}

ECode CNotificationStackScrollLayout::SetOverScrolledPixels(
    /* [in] */ Float numPixels,
    /* [in] */ Boolean onTop,
    /* [in] */ Boolean animate)
{
    SetOverScrollAmount(numPixels * GetRubberBandFactor(onTop), onTop, animate, TRUE);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetOverScrollAmount(
    /* [in] */ Float amount,
    /* [in] */ Boolean onTop,
    /* [in] */ Boolean animate)
{
    SetOverScrollAmount(amount, onTop, animate, TRUE);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetOverScrollAmount(
    /* [in] */ Float amount,
    /* [in] */ Boolean onTop,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean cancelAnimators)
{
    SetOverScrollAmount(amount, onTop, animate, cancelAnimators, IsRubberbanded(onTop));
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetOverScrollAmount(
    /* [in] */ Float amount,
    /* [in] */ Boolean onTop,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean cancelAnimators,
    /* [in] */ Boolean isRubberbanded)
{
    if (cancelAnimators) {
        mStateAnimator->CancelOverScrollAnimators(onTop);
    }
    SetOverScrollAmountInternal(amount, onTop, animate, isRubberbanded);
    return NOERROR;
}

void CNotificationStackScrollLayout::SetOverScrollAmountInternal(
    /* [in] */ Float amount,
    /* [in] */ Boolean onTop,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean isRubberbanded)
{
    amount = Elastos::Core::Math::Max((Float)0, amount);
    if (animate) {
        mStateAnimator->AnimateOverScrollToAmount(amount, onTop, isRubberbanded);
    }
    else {
        SetOverScrolledPixels(amount / GetRubberBandFactor(onTop), onTop);
        mAmbientState->SetOverScrollAmount(amount, onTop);
        if (onTop) {
            NotifyOverscrollTopListener(amount, isRubberbanded);
        }
        RequestChildrenUpdate();
    }
}

void CNotificationStackScrollLayout::NotifyOverscrollTopListener(
    /* [in] */ Float amount,
    /* [in] */ Boolean isRubberbanded)
{
    mExpandHelper->OnlyObserveMovements(amount > 1.0f);
    if (mDontReportNextOverScroll) {
        mDontReportNextOverScroll = FALSE;
        return;
    }
    if (mOverscrollTopChangedListener != NULL) {
        mOverscrollTopChangedListener->OnOverscrollTopChanged(amount, isRubberbanded);
    }
}

ECode CNotificationStackScrollLayout::SetOverscrollTopChangedListener(
    /* [in] */ IOnOverscrollTopChangedListener* overscrollTopChangedListener)
{
    mOverscrollTopChangedListener = overscrollTopChangedListener;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetCurrentOverScrollAmount(
    /* [in] */ Boolean top,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    return mAmbientState->GetOverScrollAmount(top, result);
}

ECode CNotificationStackScrollLayout::GetCurrentOverScrolledPixels(
    /* [in] */ Boolean top,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = top? mOverScrolledTopPixels : mOverScrolledBottomPixels;
    return NOERROR;
}

void CNotificationStackScrollLayout::SetOverScrolledPixels(
    /* [in] */ Float amount,
    /* [in] */ Boolean onTop)
{
    if (onTop) {
        mOverScrolledTopPixels = amount;
    }
    else {
        mOverScrolledBottomPixels = amount;
    }
}

void CNotificationStackScrollLayout::CustomScrollTo(
    /* [in] */ Int32 y)
{
    mOwnScrollY = y;
    UpdateChildren();
}

ECode CNotificationStackScrollLayout::OnOverScrolled(
    /* [in] */ Int32 scrollX,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean clampedX,
    /* [in] */ Boolean clampedY)
{
    // Treat animating scrolls differently; see #computeScroll() for why.
    Boolean finished = FALSE;
    mScroller->IsFinished(&finished);
    if (!finished) {
        const Int32 oldX = mScrollX;
        const Int32 oldY = mOwnScrollY;
        mScrollX = scrollX;
        mOwnScrollY = scrollY;
        if (clampedY) {
            SpringBack();
        }
        else {
            OnScrollChanged(mScrollX, mOwnScrollY, oldX, oldY);
            InvalidateParentIfNeeded();
            UpdateChildren();
            Float overScrollTop = 0;
            GetCurrentOverScrollAmount(TRUE, &overScrollTop);
            if (mOwnScrollY < 0) {
                NotifyOverscrollTopListener(-mOwnScrollY, IsRubberbanded(TRUE));
            }
            else {
                NotifyOverscrollTopListener(overScrollTop, IsRubberbanded(TRUE));
            }
        }
    }
    else {
        CustomScrollTo(scrollY);
        ScrollTo(scrollX, mScrollY);
    }
    return NOERROR;
}

void CNotificationStackScrollLayout::SpringBack()
{
    Int32 scrollRange = GetScrollRange();
    Boolean overScrolledTop = mOwnScrollY <= 0;
    Boolean overScrolledBottom = mOwnScrollY >= scrollRange;
    if (overScrolledTop || overScrolledBottom) {
        Boolean onTop;
        Float newAmount;
        if (overScrolledTop) {
            onTop = TRUE;
            newAmount = -mOwnScrollY;
            mOwnScrollY = 0;
            mDontReportNextOverScroll = TRUE;
        }
        else {
            onTop = FALSE;
            newAmount = mOwnScrollY - scrollRange;
            mOwnScrollY = scrollRange;
        }
        SetOverScrollAmount(newAmount, onTop, FALSE);
        SetOverScrollAmount(0.0f, onTop, TRUE);
        mScroller->ForceFinished(TRUE);
    }
}

Int32 CNotificationStackScrollLayout::GetScrollRange()
{
    Int32 scrollRange = 0;
    AutoPtr<IView> firstChild = GetFirstChildNotGone();
    if (firstChild != NULL) {
        Int32 contentHeight = 0;
        GetContentHeight(&contentHeight);
        Int32 firstChildMaxExpandHeight = GetMaxExpandHeight(firstChild);
        scrollRange = Elastos::Core::Math::Max(0, contentHeight - mMaxLayoutHeight + mBottomStackPeekSize
                + mBottomStackSlowDownHeight);
        if (scrollRange > 0) {
            // We want to at least be able collapse the first item and not ending in a weird
            // end state.
            scrollRange = Elastos::Core::Math::Max(scrollRange, firstChildMaxExpandHeight - mCollapsedSize);
        }
    }

    return scrollRange;
}

AutoPtr<IView> CNotificationStackScrollLayout::GetFirstChildNotGone()
{
    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 v = 0;
        if ((child->GetVisibility(&v), v) != IView::GONE) {
            return child;
        }
    }
    return NULL;
}

AutoPtr<IView> CNotificationStackScrollLayout::GetFirstChildBelowTranlsationY(
    /* [in] */ Float translationY)
{
    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 v = 0;
        Float y = 0;
        if ((child->GetVisibility(&v), v) != IView::GONE && (child->GetTranslationY(&y), y) >= translationY) {
            return child;
        }
    }
    return NULL;
}

ECode CNotificationStackScrollLayout::GetLastChildNotGone(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 v = 0;
        if ((child->GetVisibility(&v), v) != IView::GONE) {
            *view = child;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    *view = NULL;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetNotGoneChildCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 childCount = 0;
    GetChildCount(&childCount);
    Int32 count = 0;
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 v = 0;
        if ((child->GetVisibility(&v), v) != IView::GONE) {
            count++;
        }
    }
    Boolean gone = FALSE;
    if (IStackScrollerDecorView::Probe(mDismissView)->WillBeGone(&gone), gone) {
        count--;
    }
    if (IStackScrollerDecorView::Probe(mEmptyShadeView)->WillBeGone(&gone), gone) {
        count--;
    }
    *result = count;
    return NOERROR;
}

Int32 CNotificationStackScrollLayout::GetMaxExpandHeight(
    /* [in] */ IView* view)
{
    Int32 h = 0;
    if (IExpandableNotificationRow::Probe(view)) {
        AutoPtr<IExpandableNotificationRow> row = IExpandableNotificationRow::Probe(view);
        row->GetIntrinsicHeight(&h);
        return h;
    }
    view->GetHeight(&h);
    return h;
}

ECode CNotificationStackScrollLayout::GetContentHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mContentHeight;
    return NOERROR;
}

void CNotificationStackScrollLayout::UpdateContentHeight()
{
    Int32 height = 0, count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 v = 0;
        if ((child->GetVisibility(&v), v) != IView::GONE) {
            if (height != 0) {
                // add the padding before this element
                height += mPaddingBetweenElements;
            }
            if (IExpandableNotificationRow::Probe(child)) {
                AutoPtr<IExpandableNotificationRow> row = IExpandableNotificationRow::Probe(child);
                height += (row->GetIntrinsicHeight(&v), v);
            }
            else if (IExpandableView::Probe(child)) {
                AutoPtr<IExpandableView> expandableView = IExpandableView::Probe(child);
                height += (expandableView->GetActualHeight(&v), v);
            }
        }
    }
    mContentHeight = height + mTopPadding;
}

void CNotificationStackScrollLayout::Fling(
    /* [in] */ Int32 velocityY)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) > 0) {
        Int32 scrollRange = GetScrollRange();

        Float topAmount = 0;
        GetCurrentOverScrollAmount(TRUE, &topAmount);
        Float bottomAmount = 0;
        GetCurrentOverScrollAmount(FALSE, &bottomAmount);
        if (velocityY < 0 && topAmount > 0) {
            mOwnScrollY -= (Int32) topAmount;
            mDontReportNextOverScroll = TRUE;
            SetOverScrollAmount(0, TRUE, FALSE);
            mMaxOverScroll = Elastos::Core::Math::Abs(velocityY) / 1000.f * GetRubberBandFactor(TRUE /* onTop */)
                    * mOverflingDistance + topAmount;
        }
        else if (velocityY > 0 && bottomAmount > 0) {
            mOwnScrollY += bottomAmount;
            SetOverScrollAmount(0, FALSE, FALSE);
            mMaxOverScroll = Elastos::Core::Math::Abs(velocityY) / 1000.f
                    * GetRubberBandFactor(FALSE /* onTop */) * mOverflingDistance
                    +  bottomAmount;
        }
        else {
            // it will be set once we reach the boundary
            mMaxOverScroll = 0.0f;
        }
        mScroller->Fling(mScrollX, mOwnScrollY, 1, velocityY, 0, 0, 0,
                Elastos::Core::Math::Max(0, scrollRange), 0, Elastos::Core::Math::INT32_MAX_VALUE / 2);

        PostInvalidateOnAnimation();
    }
}

Boolean CNotificationStackScrollLayout::ShouldOverScrollFling(
    /* [in] */ Int32 initialVelocity)
{
    Float topOverScroll = 0;
    GetCurrentOverScrollAmount(TRUE, &topOverScroll);
    return mScrolledToTopOnFirstDown
            && !mExpandedInThisMotion
            && topOverScroll > mMinTopOverScrollToEscape
            && initialVelocity > 0;
}

ECode CNotificationStackScrollLayout::UpdateTopPadding(
    /* [in] */ Float qsHeight,
    /* [in] */ Int32 scrollY,
    /* [in] */ Boolean animate)
{
    Float start = qsHeight - scrollY + mNotificationTopPadding;
    Int32 value = 0;
    Float stackHeight = (GetHeight(&value), value) - start;
    Int32 minStackHeight = 0;
    GetMinStackHeight(&minStackHeight);
    if (stackHeight <= minStackHeight) {
        Float overflow = minStackHeight - stackHeight;
        stackHeight = minStackHeight;
        start = value - stackHeight;
        SetTranslationY(overflow);
        mTopPaddingOverflow = overflow;
    }
    else {
        SetTranslationY(0);
        mTopPaddingOverflow = 0;
    }
    SetTopPadding(ClampPadding((Int32) start), animate);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetNotificationTopPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNotificationTopPadding;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetMinStackHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCollapsedSize + mBottomStackPeekSize + mCollapseSecondCardPadding;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetTopPaddingOverflow(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTopPaddingOverflow;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetPeekHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIntrinsicPadding + mCollapsedSize + mBottomStackPeekSize
            + mCollapseSecondCardPadding;
    return NOERROR;
}

Int32 CNotificationStackScrollLayout::ClampPadding(
    /* [in] */ Int32 desiredPadding)
{
    return Elastos::Core::Math::Max(desiredPadding, mIntrinsicPadding);
}

Float CNotificationStackScrollLayout::GetRubberBandFactor(
    /* [in] */ Boolean onTop)
{
    if (!onTop) {
        return RUBBER_BAND_FACTOR_NORMAL;
    }
    if (mExpandedInThisMotion) {
        return RUBBER_BAND_FACTOR_AFTER_EXPAND;
    }
    else if (mIsExpansionChanging) {
        return RUBBER_BAND_FACTOR_ON_PANEL_EXPAND;
    }
    else if (mScrolledToTopOnFirstDown) {
        return 1.0f;
    }
    return RUBBER_BAND_FACTOR_NORMAL;
}

Boolean CNotificationStackScrollLayout::IsRubberbanded(
    /* [in] */ Boolean onTop)
{
    return !onTop || mExpandedInThisMotion || mIsExpansionChanging
            || !mScrolledToTopOnFirstDown;
}

void CNotificationStackScrollLayout::EndDrag()
{
    SetIsBeingDragged(FALSE);

    RecycleVelocityTracker();

    Float fv = 0;
    if ((GetCurrentOverScrollAmount(TRUE /* onTop */, &fv), fv) > 0) {
        SetOverScrollAmount(0, TRUE /* onTop */, TRUE /* animate */);
    }
    if ((GetCurrentOverScrollAmount(FALSE /* onTop */, &fv), fv) > 0) {
        SetOverScrollAmount(0, FALSE /* onTop */, TRUE /* animate */);
    }
}

void CNotificationStackScrollLayout::TransformTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [in] */ IView* sourceView,
    /* [in] */ IView* targetView)
{
    Float fx = 0, fy = 0;
    ev->OffsetLocation((sourceView->GetX(&fx), fx), (sourceView->GetY(&fy), fy));
    ev->OffsetLocation(-(targetView->GetX(&fx), fx), -(targetView->GetY(&fy), fy));
}

ECode CNotificationStackScrollLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mInterceptDelegateEnabled) {
        TransformTouchEvent(ev, this, IView::Probe(mScrollView));
        Boolean tmp = FALSE;
        if (mScrollView->OnInterceptTouchEvent(ev, &tmp), tmp) {
            mDelegateToScrollView = TRUE;
            RemoveLongPressCallback();
            *result = TRUE;
            return NOERROR;
        }
        TransformTouchEvent(ev, IView::Probe(mScrollView), this);
    }
    InitDownStates(ev);
    Boolean expandWantsIt = FALSE;
    if (!mSwipingInProgress && !mOnlyScrollingInThisMotion && IsScrollingEnabled()) {
        IGefingerpoken::Probe(mExpandHelper)->OnInterceptTouchEvent(ev, &expandWantsIt);
    }
    Boolean scrollWantsIt = FALSE;
    if (!mSwipingInProgress && !mExpandingNotification) {
        scrollWantsIt = OnInterceptTouchEventScroll(ev);
    }
    Boolean swipeWantsIt = FALSE;
    if (!mIsBeingDragged
            && !mExpandingNotification
            && !mExpandedInThisMotion
            && !mOnlyScrollingInThisMotion) {
        IGefingerpoken::Probe(mSwipeHelper)->OnInterceptTouchEvent(ev, &swipeWantsIt);
    }

    Boolean tmp = FALSE;
    *result = swipeWantsIt || scrollWantsIt || expandWantsIt || (ViewGroup::OnInterceptTouchEvent(ev, &tmp), tmp);
    return NOERROR;
}

void CNotificationStackScrollLayout::InitDownStates(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action = 0;
    ev->GetAction(&action);
    if (action == IMotionEvent::ACTION_DOWN) {
        mExpandedInThisMotion = FALSE;
        mScroller->IsFinished(&mOnlyScrollingInThisMotion);
        mOnlyScrollingInThisMotion = !mOnlyScrollingInThisMotion;
        mDisallowScrollingInThisMotion = FALSE;
    }
}

void CNotificationStackScrollLayout::OnViewRemoved(
    /* [in] */ IView* child)
{
    ViewGroup::OnViewRemoved(child);
    mStackScrollAlgorithm->NotifyChildrenChanged(this);
    if (mChangePositionInProgress) {
        // This is only a position change, don't do anything special
        return;
    }
    IExpandableView::Probe(child)->SetOnHeightChangedListener(NULL);
    mCurrentStackScrollState->RemoveViewStateForView(child);
    UpdateScrollStateForRemovedChild(child);
    Boolean animationGenerated = GenerateRemoveAnimation(child);
    Boolean contains = FALSE;
    if (animationGenerated && (mSwipedOutViews->Contains(child, &contains), !contains)) {
        // Add this view to an overlay in order to ensure that it will still be temporary
        // drawn when removed
        AutoPtr<IViewOverlay> vo;
        GetOverlay((IViewOverlay**)&vo);
        IViewGroupOverlay::Probe(vo)->Add(child);
    }
    UpdateAnimationState(FALSE, child);

    // Make sure the clipRect we might have set is removed
    child->SetClipBounds(NULL);
}

Boolean CNotificationStackScrollLayout::GenerateRemoveAnimation(
    /* [in] */ IView* child)
{
    if (mIsExpanded && mAnimationsEnabled) {
        Boolean contains = FALSE;
        if ((mChildrenToAddAnimated->Contains(child, &contains), !contains)) {
            // Generate Animations
            mChildrenToRemoveAnimated->Add(child);
            mNeedsAnimation = TRUE;
            return TRUE;
        }
        else {
            mChildrenToAddAnimated->Remove(child);
            mFromMoreCardAdditions->Remove(child);
            return FALSE;
        }
    }
    return FALSE;
}

void CNotificationStackScrollLayout::UpdateScrollStateForRemovedChild(
    /* [in] */ IView* removedChild)
{
    Int32 startingPosition = GetPositionInLinearLayout(removedChild);
    Int32 childHeight = GetIntrinsicHeight(removedChild) + mPaddingBetweenElements;
    Int32 endPosition = startingPosition + childHeight;
    if (endPosition <= mOwnScrollY) {
        // This child is fully scrolled of the top, so we have to deduct its height from the
        // scrollPosition
        mOwnScrollY -= childHeight;
    }
    else if (startingPosition < mOwnScrollY) {
        // This child is currently being scrolled into, set the scroll position to the start of
        // this child
        mOwnScrollY = startingPosition;
    }
}

Int32 CNotificationStackScrollLayout::GetIntrinsicHeight(
    /* [in] */ IView* view)
{
    Int32 h = 0;
    if (IExpandableView::Probe(view)) {
        AutoPtr<IExpandableView> expandableView = IExpandableView::Probe(view);
        expandableView->GetIntrinsicHeight(&h);
        return h;
    }
    view->GetHeight(&h);
    return h;
}

Int32 CNotificationStackScrollLayout::GetPositionInLinearLayout(
    /* [in] */ IView* requestedChild)
{
    Int32 position = 0, count = 0;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child.Get() == requestedChild) {
            return position;
        }
        Int32 v = 0;
        if ((child->GetVisibility(&v), v) != IView::GONE) {
            position += GetIntrinsicHeight(child);
            if (i < count - 1) {
                position += mPaddingBetweenElements;
            }
        }
    }
    return 0;
}

void CNotificationStackScrollLayout::OnViewAdded(
    /* [in] */ IView* child)
{
    ViewGroup::OnViewAdded(child);
    mStackScrollAlgorithm->NotifyChildrenChanged(this);
    IExpandableView::Probe(child)->SetOnHeightChangedListener(this);
    GenerateAddAnimation(child, FALSE /* fromMoreCard */);
    UpdateAnimationState(child);
}

ECode CNotificationStackScrollLayout::SetAnimationsEnabled(
    /* [in] */ Boolean animationsEnabled)
{
    mAnimationsEnabled = animationsEnabled;
    UpdateNotificationAnimationStates();
    return NOERROR;
}

void CNotificationStackScrollLayout::UpdateNotificationAnimationStates()
{
    Boolean running = mIsExpanded && mAnimationsEnabled;
    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        UpdateAnimationState(running, child);
    }
}

void CNotificationStackScrollLayout::UpdateAnimationState(
    /* [in] */ IView* child)
{
    UpdateAnimationState(mAnimationsEnabled && mIsExpanded, child);
}

void CNotificationStackScrollLayout::UpdateAnimationState(
    /* [in] */ Boolean running,
    /* [in] */ IView* child)
{
    if (IExpandableNotificationRow::Probe(child)) {
        AutoPtr<IExpandableNotificationRow> row = IExpandableNotificationRow::Probe(child);
        row->SetIconAnimationRunning(running);
    }
}

ECode CNotificationStackScrollLayout::IsAddOrRemoveAnimationPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean em1 = FALSE, em2 = FALSE;
    *result = mNeedsAnimation
            && ((mChildrenToAddAnimated->IsEmpty(&em1), !em1)
                || (mChildrenToRemoveAnimated->IsEmpty(&em2), !em2));
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GenerateAddAnimation(
    /* [in] */ IView* child,
    /* [in] */ Boolean fromMoreCard)
{
    if (mIsExpanded && mAnimationsEnabled && !mChangePositionInProgress) {
        // Generate Animations
        mChildrenToAddAnimated->Add(child);
        if (fromMoreCard) {
            mFromMoreCardAdditions->Add(child);
        }
        mNeedsAnimation = TRUE;
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::ChangeViewPosition(
    /* [in] */ IView* child,
    /* [in] */ Int32 newIndex)
{
    Int32 currentIndex = 0;
    IndexOfChild(child, &currentIndex);
    AutoPtr<IViewParent> vp;
    if (child != NULL && (child->GetParent((IViewParent**)&vp), vp.Get()) == (IViewParent*)this
            && currentIndex != newIndex) {
        mChangePositionInProgress = TRUE;
        RemoveView(child);
        AddView(child, newIndex);
        mChangePositionInProgress = FALSE;
        Int32 v = 0;
        if (mIsExpanded && mAnimationsEnabled && (child->GetVisibility(&v), v) != IView::GONE) {
            mChildrenChangingPositions->Add(child);
            mNeedsAnimation = TRUE;
        }
    }
    return NOERROR;
}

void CNotificationStackScrollLayout::StartAnimationToState()
{
    if (mNeedsAnimation) {
        GenerateChildHierarchyEvents();
        mNeedsAnimation = FALSE;
    }
    Boolean empty = FALSE;
    if ((mAnimationEvents->IsEmpty(&empty), !empty) || IsCurrentlyAnimating()) {
        mStateAnimator->StartAnimationForEvents(mAnimationEvents, mCurrentStackScrollState,
                mGoToFullShadeDelay);
        mAnimationEvents->Clear();
    }
    else {
        ApplyCurrentState();
    }
    mGoToFullShadeDelay = 0;
}

void CNotificationStackScrollLayout::GenerateChildHierarchyEvents()
{
    GenerateChildRemovalEvents();
    GenerateChildAdditionEvents();
    GeneratePositionChangeEvents();
    GenerateSnapBackEvents();
    GenerateDragEvents();
    GenerateTopPaddingEvent();
    GenerateActivateEvent();
    GenerateDimmedEvent();
    GenerateHideSensitiveEvent();
    GenerateDarkEvent();
    GenerateGoToFullShadeEvent();
    GenerateViewResizeEvent();
    GenerateAnimateEverythingEvent();
    mNeedsAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateViewResizeEvent()
{
    if (mNeedViewResizeAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_VIEW_RESIZE);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mNeedViewResizeAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateSnapBackEvents()
{
    AutoPtr<IIterator> it;
    mSnappedBackChildren->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        AutoPtr<AnimationEvent> ae = new AnimationEvent(child, AnimationEvent::ANIMATION_TYPE_SNAP_BACK);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mSnappedBackChildren->Clear();
}

void CNotificationStackScrollLayout::GenerateDragEvents()
{
    AutoPtr<IIterator> it;
    mDragAnimPendingChildren->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        AutoPtr<AnimationEvent> ae = new AnimationEvent(child, AnimationEvent::ANIMATION_TYPE_START_DRAG);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }

    mDragAnimPendingChildren->Clear();
}

void CNotificationStackScrollLayout::GenerateChildRemovalEvents()
{
    AutoPtr<IIterator> it;
    mChildrenToRemoveAnimated->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        Boolean childWasSwipedOut = FALSE;
        mSwipedOutViews->Contains(child, &childWasSwipedOut);
        Int32 animationType = childWasSwipedOut
                ? AnimationEvent::ANIMATION_TYPE_REMOVE_SWIPED_OUT
                : AnimationEvent::ANIMATION_TYPE_REMOVE;
        AutoPtr<AnimationEvent> event = new AnimationEvent(child, animationType);

        // we need to know the view after this one
        Float y = 0;
        child->GetTranslationY(&y);
        event->mViewAfterChangingView = GetFirstChildBelowTranlsationY(y);
        mAnimationEvents->Add(event->Probe(EIID_IInterface));
    }

    mSwipedOutViews->Clear();
    mChildrenToRemoveAnimated->Clear();
}

void CNotificationStackScrollLayout::GeneratePositionChangeEvents()
{
    AutoPtr<IIterator> it;
    mChildrenChangingPositions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        AutoPtr<AnimationEvent> ae = new AnimationEvent(child, AnimationEvent::ANIMATION_TYPE_CHANGE_POSITION);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }

    mChildrenChangingPositions->Clear();
}

void CNotificationStackScrollLayout::GenerateChildAdditionEvents()
{
    AutoPtr<IIterator> it;
    mChildrenToAddAnimated->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IView> child = IView::Probe(obj);

        Boolean contains = FALSE;
        if (mFromMoreCardAdditions->Contains(child, &contains), contains) {
            AutoPtr<AnimationEvent> ae = new AnimationEvent(child, AnimationEvent::ANIMATION_TYPE_ADD,
                    IStackStateAnimator::ANIMATION_DURATION_STANDARD);
            mAnimationEvents->Add(ae->Probe(EIID_IInterface));
        }
        else {
            AutoPtr<AnimationEvent> ae = new AnimationEvent(child, AnimationEvent::ANIMATION_TYPE_ADD);
            mAnimationEvents->Add(ae->Probe(EIID_IInterface));
        }
    }

    mChildrenToAddAnimated->Clear();
    mFromMoreCardAdditions->Clear();
}

void CNotificationStackScrollLayout::GenerateTopPaddingEvent()
{
    if (mTopPaddingNeedsAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_TOP_PADDING_CHANGED);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mTopPaddingNeedsAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateActivateEvent()
{
    if (mActivateNeedsAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_ACTIVATED_CHILD);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mActivateNeedsAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateAnimateEverythingEvent()
{
    if (mEverythingNeedsAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_EVERYTHING);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mEverythingNeedsAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateDimmedEvent()
{
    if (mDimmedNeedsAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_DIMMED);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mDimmedNeedsAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateHideSensitiveEvent()
{
    if (mHideSensitiveNeedsAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_HIDE_SENSITIVE);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mHideSensitiveNeedsAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateDarkEvent()
{
    if (mDarkNeedsAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_DARK);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mDarkNeedsAnimation = FALSE;
}

void CNotificationStackScrollLayout::GenerateGoToFullShadeEvent()
{
    if (mGoToFullShadeNeedsAnimation) {
        AutoPtr<AnimationEvent> ae = new AnimationEvent(NULL, AnimationEvent::ANIMATION_TYPE_GO_TO_FULL_SHADE);
        mAnimationEvents->Add(ae->Probe(EIID_IInterface));
    }
    mGoToFullShadeNeedsAnimation = FALSE;
}

Boolean CNotificationStackScrollLayout::OnInterceptTouchEventScroll(
    /* [in] */ IMotionEvent* ev)
{
    if (!IsScrollingEnabled()) {
        return FALSE;
    }
    /*
     * This method JUST determines whether we want to intercept the motion.
     * If we return TRUE, onMotionEvent will be called and we do the actual
     * scrolling there.
     */

    /*
    * Shortcut the most recurring case: the user is in the dragging
    * state and he is moving his finger.  We want to intercept this
    * motion.
    */
    Int32 action = 0;
    ev->GetAction(&action);
    if ((action == IMotionEvent::ACTION_MOVE) && (mIsBeingDragged)) {
        return TRUE;
    }

    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_MOVE: {
            /*
             * mIsBeingDragged == FALSE, otherwise the shortcut would have caught it. Check
             * whether the user has moved far enough from his original down touch.
             */

            /*
            * Locally do absolute value. mLastMotionY is set to the y value
            * of the down event.
            */
            const Int32 activePointerId = mActivePointerId;
            if (activePointerId == INVALID_POINTER) {
                // If we don't have a valid id, the touch down wasn't on content.
                break;
            }

            Int32 pointerIndex = 0;
            ev->FindPointerIndex(activePointerId, &pointerIndex);
            if (pointerIndex == -1) {
                Logger::E(TAG, "Invalid pointerId=%d in onInterceptTouchEvent", activePointerId);
                break;
            }

            Float fv = 0;
            const Int32 y = (Int32) (ev->GetY(pointerIndex, &fv), fv);
            const Int32 x = (Int32) (ev->GetX(pointerIndex, &fv), fv);
            const Int32 yDiff = Elastos::Core::Math::Abs(y - mLastMotionY);
            const Int32 xDiff = Elastos::Core::Math::Abs(x - mDownX);
            if (yDiff > mTouchSlop && yDiff > xDiff) {
                SetIsBeingDragged(TRUE);
                mLastMotionY = y;
                mDownX = x;
                InitVelocityTrackerIfNotExists();
                mVelocityTracker->AddMovement(ev);
            }
            break;
        }

        case IMotionEvent::ACTION_DOWN: {
            Float fv = 0;
            const Int32 y = (Int32) (ev->GetY(&fv), fv);
            ev->GetX(&fv);
            AutoPtr<IExpandableView> exV;
            GetChildAtPosition(fv, y, (IExpandableView**)&exV);
            if (exV == NULL) {
                SetIsBeingDragged(FALSE);
                RecycleVelocityTracker();
                break;
            }

            /*
             * Remember location of down touch.
             * ACTION_DOWN always refers to pointer index 0.
             */
            mLastMotionY = y;
            mDownX = (Int32) fv;
            ev->GetPointerId(0, &mActivePointerId);
            IsScrolledToTop(&mScrolledToTopOnFirstDown);

            InitOrResetVelocityTracker();
            mVelocityTracker->AddMovement(ev);
            /*
            * If being flinged and user touches the screen, initiate drag;
            * otherwise don't.  mScroller.isFinished should be FALSE when
            * being flinged.
            */
            Boolean isBeingDragged = FALSE;
            mScroller->IsFinished(&isBeingDragged);
            isBeingDragged = !isBeingDragged;
            SetIsBeingDragged(isBeingDragged);
            break;
        }

        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP: {
            /* Release the drag */
            SetIsBeingDragged(FALSE);
            mActivePointerId = INVALID_POINTER;
            RecycleVelocityTracker();
            Boolean tmp = FALSE;
            if (mScroller->SpringBack(mScrollX, mOwnScrollY, 0, 0, 0, GetScrollRange(), &tmp), tmp) {
                PostInvalidateOnAnimation();
            }
            break;
        }
        case IMotionEvent::ACTION_POINTER_UP:
            OnSecondaryPointerUp(ev);
            break;
    }

    /*
    * The only time we want to intercept motion events is if we are in the
    * drag mode.
    */
    return mIsBeingDragged;
}

Boolean CNotificationStackScrollLayout::IsInContentBounds(
    /* [in] */ IMotionEvent* event)
{
    Float y = 0;
    event->GetY(&y);
    Int32 h = 0, b = 0;
    GetHeight(&h);
    return y < h - (GetEmptyBottomMargin(&b), b);
}

void CNotificationStackScrollLayout::SetIsBeingDragged(
    /* [in] */ Boolean isDragged)
{
    mIsBeingDragged = isDragged;
    if (isDragged) {
        RequestDisallowInterceptTouchEvent(TRUE);
        RemoveLongPressCallback();
    }
}

ECode CNotificationStackScrollLayout::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    ViewGroup::OnWindowFocusChanged(hasWindowFocus);
    if (!hasWindowFocus) {
        RemoveLongPressCallback();
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::RemoveLongPressCallback()
{
    mSwipeHelper->RemoveLongPressCallback();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::IsScrolledToTop(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwnScrollY == 0;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::IsScrolledToBottom(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOwnScrollY >= GetScrollRange();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetHostView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = this;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetEmptyBottomMargin(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 emptyMargin = mMaxLayoutHeight - mContentHeight - mBottomStackPeekSize;
    if (NeedsHeightAdaption()) {
        emptyMargin -= mBottomStackSlowDownHeight;
    }
    else {
        emptyMargin -= mCollapseSecondCardPadding;
    }
    *result = Elastos::Core::Math::Max(emptyMargin, 0);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnExpansionStarted()
{
    mIsExpansionChanging = TRUE;
    mStackScrollAlgorithm->OnExpansionStarted(mCurrentStackScrollState);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnExpansionStopped()
{
    mIsExpansionChanging = FALSE;
    mStackScrollAlgorithm->OnExpansionStopped();
    if (!mIsExpanded) {
        mOwnScrollY = 0;
    }
    return NOERROR;
}

void CNotificationStackScrollLayout::SetIsExpanded(
    /* [in] */ Boolean isExpanded)
{
    Boolean changed = isExpanded != mIsExpanded;
    mIsExpanded = isExpanded;
    mStackScrollAlgorithm->SetIsExpanded(isExpanded);
    if (changed) {
        UpdateNotificationAnimationStates();
    }
}

ECode CNotificationStackScrollLayout::OnHeightChanged(
    /* [in] */ IExpandableView* view)
{
    UpdateContentHeight();
    UpdateScrollPositionOnExpandInBottom(view);
    ClampScrollPosition();
    NotifyHeightChangeListener(view);
    RequestChildrenUpdate();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnReset(
    /* [in] */ IExpandableView* view)
{
    if (mIsExpanded && mAnimationsEnabled) {
        mRequestViewResizeAnimationOnLayout = TRUE;
    }
    mStackScrollAlgorithm->OnReset(view);
    UpdateAnimationState(IView::Probe(view));
    return NOERROR;
}

void CNotificationStackScrollLayout::UpdateScrollPositionOnExpandInBottom(
    /* [in] */ IExpandableView* view)
{
    if (IExpandableNotificationRow::Probe(view)) {
        AutoPtr<IExpandableNotificationRow> row = IExpandableNotificationRow::Probe(view);
        Boolean tmp = FALSE;
        if (row->IsUserLocked(&tmp), tmp) {
            // We are actually expanding this view
            Float y = 0;
            Int32 h = 0;
            IView::Probe(row)->GetTranslationY(&y);
            IExpandableView::Probe(row)->GetActualHeight(&h);
            Float endPosition = y + h;
            Int32 stackEnd = mMaxLayoutHeight - mBottomStackPeekSize -
                    mBottomStackSlowDownHeight;
            if (endPosition > stackEnd) {
                mOwnScrollY += endPosition - stackEnd;
                mDisallowScrollingInThisMotion = TRUE;
            }
        }
    }
}

ECode CNotificationStackScrollLayout::SetOnHeightChangedListener(
    /* [in] */ IExpandableViewOnHeightChangedListener* listener)
{
    mOnHeightChangedListener = listener;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnChildAnimationFinished()
{
    RequestChildrenUpdate();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetDimmed(
    /* [in] */ Boolean dimmed,
    /* [in] */ Boolean animate)
{
    mStackScrollAlgorithm->SetDimmed(dimmed);
    mAmbientState->SetDimmed(dimmed);
    UpdatePadding(dimmed);
    if (animate && mAnimationsEnabled) {
        mDimmedNeedsAnimation = TRUE;
        mNeedsAnimation =  TRUE;
    }
    RequestChildrenUpdate();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetHideSensitive(
    /* [in] */ Boolean hideSensitive,
    /* [in] */ Boolean animate)
{
    Boolean tmp = FALSE;
    if (hideSensitive != (mAmbientState->IsHideSensitive(&tmp), tmp)) {
        Int32 childCount = 0;
        GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> view;
            GetChildAt(i, (IView**)&view);
            AutoPtr<IExpandableView> v = IExpandableView::Probe(view);
            v->SetHideSensitiveForIntrinsicHeight(hideSensitive);
        }
        mAmbientState->SetHideSensitive(hideSensitive);
        if (animate && mAnimationsEnabled) {
            mHideSensitiveNeedsAnimation = TRUE;
            mNeedsAnimation =  TRUE;
        }
        RequestChildrenUpdate();
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetActivatedChild(
    /* [in] */ IActivatableNotificationView* activatedChild)
{
    mAmbientState->SetActivatedChild(activatedChild);
    if (mAnimationsEnabled) {
        mActivateNeedsAnimation = TRUE;
        mNeedsAnimation =  TRUE;
    }
    RequestChildrenUpdate();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetActivatedChild(
    /* [out] */ IActivatableNotificationView** view)
{
    VALIDATE_NOT_NULL(view);
    return mAmbientState->GetActivatedChild(view);
}

void CNotificationStackScrollLayout::ApplyCurrentState()
{
    mCurrentStackScrollState->Apply();
    if (mListener != NULL) {
        mListener->OnChildLocationsChanged(this);
    }
}

ECode CNotificationStackScrollLayout::SetSpeedBumpView(
    /* [in] */ ISpeedBumpView* speedBumpView)
{
    mSpeedBumpView = speedBumpView;
    AddView(IView::Probe(speedBumpView));
    return NOERROR;
}

void CNotificationStackScrollLayout::UpdateSpeedBump(
    /* [in] */ Boolean visible)
{
    Int32 v = 0;
    IView::Probe(mSpeedBumpView)->GetVisibility(&v);
    Boolean notGoneBefore = v != IView::GONE;
    if (visible != notGoneBefore) {
        Int32 newVisibility = visible ? IView::VISIBLE : IView::GONE;
        IView::Probe(mSpeedBumpView)->SetVisibility(newVisibility);
        if (visible) {
            // Make invisible to ensure that the appear animation is played.
            mSpeedBumpView->SetInvisible();
        }
        else {
            // TODO: This doesn't really work, because the view is already set to GONE above.
            GenerateRemoveAnimation(IView::Probe(mSpeedBumpView));
        }
    }
}

ECode CNotificationStackScrollLayout::GoToFullShade(
    /* [in] */ Int64 delay)
{
    UpdateSpeedBump(TRUE /* visibility */);
    IStackScrollerDecorView::Probe(mDismissView)->SetInvisible();
    IStackScrollerDecorView::Probe(mEmptyShadeView)->SetInvisible();
    mGoToFullShadeNeedsAnimation = TRUE;
    mGoToFullShadeDelay = delay;
    mNeedsAnimation =  TRUE;
    RequestChildrenUpdate();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::CancelExpandHelper()
{
    mExpandHelper->Cancel();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetIntrinsicPadding(
    /* [in] */ Int32 intrinsicPadding)
{
    mIntrinsicPadding = intrinsicPadding;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetIntrinsicPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIntrinsicPadding;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetNotificationsTopY(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Float fv = 0;
    GetTranslationY(&fv);
    *result = mTopPadding + fv;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetDark(
    /* [in] */ Boolean dark,
    /* [in] */ Boolean animate)
{
    mAmbientState->SetDark(dark);
    if (animate && mAnimationsEnabled) {
        mDarkNeedsAnimation = TRUE;
        mNeedsAnimation =  TRUE;
    }
    RequestChildrenUpdate();
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetDismissView(
    /* [in] */ IDismissView* dismissView)
{
    mDismissView = dismissView;
    AddView(IView::Probe(mDismissView));
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetEmptyShadeView(
    /* [in] */ IEmptyShadeView* emptyShadeView)
{
    mEmptyShadeView = emptyShadeView;
    AddView(IView::Probe(mEmptyShadeView));
    return NOERROR;
}

ECode CNotificationStackScrollLayout::UpdateEmptyShadeView(
    /* [in] */ Boolean visible)
{
    IStackScrollerDecorView* decorView = IStackScrollerDecorView::Probe(mEmptyShadeView);
    Boolean willBeGone = FALSE;
    decorView->WillBeGone(&willBeGone);
    Int32 v = 0;
    Int32 oldVisibility = willBeGone ? IView::GONE : (IView::Probe(mEmptyShadeView)->GetVisibility(&v), v);
    Int32 newVisibility = visible ? IView::VISIBLE : IView::GONE;
    if (oldVisibility != newVisibility) {
        if (newVisibility != IView::GONE) {
            if (willBeGone) {
                decorView->CancelAnimation();
            }
            else {
                decorView->SetInvisible();
            }
            IView::Probe(mEmptyShadeView)->SetVisibility(newVisibility);
            decorView->SetWillBeGone(FALSE);
            UpdateContentHeight();
            NotifyHeightChangeListener(IExpandableView::Probe(mDismissView));
        }
        else {
            decorView->SetWillBeGone(TRUE);
            AutoPtr<EmptyShadeViewGoneRunnable> run = new EmptyShadeViewGoneRunnable(this);
            decorView->PerformVisibilityAnimation(FALSE, run);
        }
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::UpdateDismissView(
    /* [in] */ Boolean visible)
{
    Boolean willBeGone = FALSE;
    IStackScrollerDecorView::Probe(mDismissView)->WillBeGone(&willBeGone);
    Int32 v = 0;
    Int32 oldVisibility = willBeGone ? IView::GONE : (IView::Probe(mDismissView)->GetVisibility(&v), v);
    Int32 newVisibility = visible ? VISIBLE : IView::GONE;
    if (oldVisibility != newVisibility) {
        if (newVisibility != IView::GONE) {
            if (willBeGone) {
                IStackScrollerDecorView::Probe(mDismissView)->CancelAnimation();
            }
            else {
                IStackScrollerDecorView::Probe(mDismissView)->SetInvisible();
            }
            IView::Probe(mDismissView)->SetVisibility(newVisibility);
            IStackScrollerDecorView::Probe(mDismissView)->SetWillBeGone(FALSE);
            UpdateContentHeight();
            NotifyHeightChangeListener(IExpandableView::Probe(mDismissView));
        }
        else {
            IStackScrollerDecorView::Probe(mDismissView)->SetWillBeGone(TRUE);
            AutoPtr<DismissViewGoneRunnable> run = new DismissViewGoneRunnable(this);
            IStackScrollerDecorView::Probe(mDismissView)->PerformVisibilityAnimation(FALSE, run);
        }
    }
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetDismissAllInProgress(
    /* [in] */ Boolean dismissAllInProgress)
{
    mDismissAllInProgress = dismissAllInProgress;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::IsDismissViewNotGone(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 v = 0;
    IView::Probe(mDismissView)->GetVisibility(&v);
    Boolean willBeGone = FALSE;
    *result = v != IView::GONE && (IStackScrollerDecorView::Probe(mDismissView)->WillBeGone(&willBeGone), !willBeGone);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::IsDismissViewVisible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return IStackScrollerDecorView::Probe(mDismissView)->IsVisible(result);
}

ECode CNotificationStackScrollLayout::GetDismissViewHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 height = 0;
    IView::Probe(mDismissView)->GetHeight(&height);
    height += mPaddingBetweenElementsNormal;

    // Hack: Accommodate for additional distance when we only have one notification and the
    // dismiss all button.
    AutoPtr<IView> view;
    GetLastChildNotGone((IView**)&view);
    Int32 count = 0;
    GetNotGoneChildCount(&count);
    AutoPtr<IView> v = GetFirstChildNotGone();
    if (count == 2 && view.Get() == IView::Probe(mDismissView)
            && IActivatableNotificationView::Probe(v)) {
        height += mCollapseSecondCardPadding;
    }
    *result = height;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::GetBottomMostNotificationBottom(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count = 0;
    GetChildCount(&count);
    Float max = 0;
    for (Int32 childIdx = 0; childIdx < count; childIdx++) {
        AutoPtr<IView> view;
        GetChildAt(childIdx, (IView**)&view);
        AutoPtr<IExpandableView> child = IExpandableView::Probe(view);
        Int32 v = 0;
        IView::Probe(child)->GetVisibility(&v);
        if (v == IView::GONE) {
            continue;
        }
        Float y = 0;
        IView::Probe(child)->GetTranslationY(&y);
        Float bottom = y + (child->GetActualHeight(&v), v);
        if (bottom > max) {
            max = bottom;
        }
    }
    Float tmp = 0;
    *result = max + (GetTranslationY(&tmp), tmp);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::UpdateIsSmallScreen(
    /* [in] */ Int32 qsMinHeight)
{
    mStackScrollAlgorithm->UpdateIsSmallScreen(mMaxLayoutHeight - qsMinHeight);
    return NOERROR;
}

ECode CNotificationStackScrollLayout::SetPhoneStatusBar(
    /* [in] */ IPhoneStatusBar* phoneStatusBar)
{
    mPhoneStatusBar = phoneStatusBar;
    return NOERROR;
}

ECode CNotificationStackScrollLayout::OnGoToKeyguard()
{
    if (mIsExpanded && mAnimationsEnabled) {
        mEverythingNeedsAnimation = TRUE;
        RequestChildrenUpdate();
    }
    return NOERROR;
}

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
