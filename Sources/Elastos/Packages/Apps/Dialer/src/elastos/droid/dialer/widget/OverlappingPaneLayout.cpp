
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/dialer/widget/OverlappingPaneLayout.h"
#include "elastos/droid/dialer/widget/COverlappingPaneLayoutSavedState.h"
#include "elastos/droid/support/v4/view/MotionEventCompat.h"
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Support::V4::View::MotionEventCompat;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Widget {

//================================================================
// OverlappingPaneLayout::DragHelperCallback
//================================================================
OverlappingPaneLayout::DragHelperCallback::DragHelperCallback(
    /* [in] */ OverlappingPaneLayout* host)
    : mHost(host)
{}

ECode OverlappingPaneLayout::DragHelperCallback::TryCaptureView(
    /* [in] */ IView* child,
    /* [in] */ Int32 pointerId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mHost->mIsUnableToDrag) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    *result = ((LayoutParams*)params.Get())->mSlideable;
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::OnViewDragStateChanged(
    /* [in] */ Int32 state)
{
    if (DEBUG) {
        Logger::D(TAG, "onViewDragStateChanged: %d", state);
    }

    if (mHost->mDragHelper->GetViewDragState() == ViewDragHelper::STATE_IDLE) {
        if (mHost->mSlideOffset == 0) {
            mHost->UpdateObscuredViewsVisibility(mHost->mSlideableView);
            mHost->DispatchOnPanelClosed(mHost->mSlideableView);
            mHost->mPreservedOpenState = FALSE;
        }
        else {
            mHost->DispatchOnPanelOpened(mHost->mSlideableView);
            mHost->mPreservedOpenState = TRUE;
        }
    }

    if (state == ViewDragHelper::STATE_IDLE
            && (mHost->mDragHelper->GetVelocityMagnitude() > 0)
            && mHost->mIsInNestedFling) {
        mHost->mIsInNestedFling = FALSE;
        Int32 flingVelocity = !mHost->mInUpwardsPreFling ?
                -mHost->mDragHelper->GetVelocityMagnitude() : mHost->mDragHelper->GetVelocityMagnitude();
        mHost->mPanelSlideCallbacks->OnPanelFlingReachesEdge(flingVelocity);
    }
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::OnViewCaptured(
    /* [in] */ IView* capturedChild,
    /* [in] */ Int32 activePointerId)
{
    // Make all child views visible in preparation for sliding things around
    mHost->SetAllChildrenVisible();
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::OnViewPositionChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    mHost->OnPanelDragged(top);
    mHost->Invalidate();
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::OnViewFling(
    /* [in] */ IView* releasedChild,
    /* [in] */ Float xVelocity,
    /* [in] */ Float yVelocity)
{
    if (releasedChild == NULL) {
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onViewFling: %f", yVelocity);
    }

    // Flings won't always fully expand or collapse the header. Instead of performing the
    // fling and then waiting for the fling to end before snapping into place, we
    // immediately snap into place if we predict the fling won't fully expand or collapse
    // the header.
    Int32 yOffsetPx = mHost->mDragHelper->PredictFlingYOffset((Int32)yVelocity);
    if (yVelocity < 0) {
        // Only perform a fling if we know the fling will fully compress the header.
        if (-yOffsetPx > mHost->mSlideOffsetPx) {
            Int32 left;
            releasedChild->GetLeft(&left);
            mHost->mDragHelper->FlingCapturedView(left, /* minTop = */ 0,
                    mHost->mSlideRange, Elastos::Core::Math::INT32_MAX_VALUE, (Int32)yVelocity);
        }
        else {
            mHost->mIsInNestedFling = FALSE;
            OnViewReleased(releasedChild, xVelocity, yVelocity);
        }
    }
    else {
        // Only perform a fling if we know the fling will expand the header as far
        // as it can possible be expanded, given the isScrollableChildUnscrolled() value.
        Boolean result;
        if (yOffsetPx + mHost->mSlideOffsetPx >= mHost->mSlideRange
                && (mHost->mPanelSlideCallbacks->IsScrollableChildUnscrolled(&result), result)) {
            Int32 left;
            releasedChild->GetLeft(&left);
            mHost->mDragHelper->FlingCapturedView(left, /* minTop = */ 0,
                    Elastos::Core::Math::INT32_MAX_VALUE, mHost->mSlideRange, (Int32)yVelocity);
        }
        else if (yOffsetPx + mHost->mSlideOffsetPx >= mHost->mIntermediateOffset
                && mHost->mSlideOffsetPx <= mHost->mIntermediateOffset
                && (mHost->mPanelSlideCallbacks->IsScrollableChildUnscrolled(&result), !result)) {
            Int32 left;
            releasedChild->GetLeft(&left);
            mHost->mDragHelper->FlingCapturedView(left, /* minTop = */ 0,
                    Elastos::Core::Math::INT32_MAX_VALUE, mHost->mIntermediateOffset, (Int32)yVelocity);
        }
        else {
            mHost->mIsInNestedFling = FALSE;
            OnViewReleased(releasedChild, xVelocity, yVelocity);
        }
    }

    mHost->mInNestedPreScrollDownwards = FALSE;
    mHost->mInNestedPreScrollUpwards = FALSE;

    // Without this invalidate, some calls to flingCapturedView can have no affect.
    mHost->Invalidate();
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::OnViewReleased(
    /* [in] */ IView* releasedChild,
    /* [in] */ Float xvel,
    /* [in] */ Float yvel)
{
    if (DEBUG) {
        Boolean unscrolled;
        mHost->mPanelSlideCallbacks->IsScrollableChildUnscrolled(&unscrolled);
        Logger::D(TAG, "onViewReleased: mIsInNestedFling=%d unscrolled=%d"
                ", mInNestedPreScrollDownwards =%d, mInNestedPreScrollUpwards =%d, yvel=%f",
                mHost->mIsInNestedFling, unscrolled, mHost->mInNestedPreScrollDownwards,
                mHost->mInNestedPreScrollUpwards, yvel);
    }
    if (releasedChild == NULL) {
        return NOERROR;
    }

    AutoPtr<IViewGroupLayoutParams> params;
    releasedChild->GetLayoutParams((IViewGroupLayoutParams**)&params);
    LayoutParams* lp = (LayoutParams*)params.Get();
    Int32 pt;
    mHost->GetPaddingTop(&pt);
    Int32 top = pt + lp->mTopMargin;

    // Decide where to snap to according to the current direction of motion and the current
    // position. The velocity's magnitude has no bearing on this.
    if (mHost->mInNestedPreScrollDownwards || yvel > 0) {
        // Scrolling downwards
        if (mHost->mSlideOffsetPx > mHost->mIntermediateOffset + mHost->mReleaseScrollSlop) {
            top += mHost->mSlideRange;
        }
        else if (mHost->mSlideOffsetPx > mHost->mReleaseScrollSlop) {
            top += mHost->mIntermediateOffset;
        }
        else {
            // Offset is very close to 0
        }
    }
    else if (mHost->mInNestedPreScrollUpwards || yvel < 0) {
        // Scrolling upwards
        if (mHost->mSlideOffsetPx > mHost->mSlideRange - mHost->mReleaseScrollSlop) {
            // Offset is very close to mSlideRange
            top += mHost->mSlideRange;
        }
        else if (mHost->mSlideOffsetPx > mHost->mIntermediateOffset - mHost->mReleaseScrollSlop) {
            // Offset is between mIntermediateOffset and mSlideRange.
            top += mHost->mIntermediateOffset;
        }
        else {
            // Offset is between 0 and mIntermediateOffset.
        }
    }
    else {
        // Not moving upwards or downwards. This case can only be triggered when directly
        // dragging the tabs. We don't bother to remember previous scroll direction
        // when directly dragging the tabs.
        if (0 <= mHost->mSlideOffsetPx && mHost->mSlideOffsetPx <= mHost->mIntermediateOffset / 2) {
            // Offset is between 0 and mIntermediateOffset, but closer to 0
            // Leave top unchanged
        }
        else if (mHost->mIntermediateOffset / 2 <= mHost->mSlideOffsetPx
                && mHost->mSlideOffsetPx <= (mHost->mIntermediateOffset + mHost->mSlideRange) / 2) {
            // Offset is closest to mIntermediateOffset
            top += mHost->mIntermediateOffset;
        }
        else {
            // Offset is between mIntermediateOffset and mSlideRange, but closer to
            // mSlideRange
            top += mHost->mSlideRange;
        }
    }

    Int32 left;
    releasedChild->GetLeft(&left);
    Boolean result;
    mHost->mDragHelper->SettleCapturedViewAt(left, top, &result);
    mHost->Invalidate();
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::GetViewVerticalDragRange(
    /* [in] */ IView* child,
    /* [out] */ Int32* range)
{
    VALIDATE_NOT_NULL(range);
    *range = mHost->mSlideRange;
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::ClampViewPositionHorizontal(
    /* [in] */ IView* child,
    /* [in] */ Int32 left,
    /* [in] */ Int32 dx,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // Make sure we never move views horizontally.
    child->GetLeft(result);
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::ClampViewPositionVertical(
    /* [in] */ IView* child,
    /* [in] */ Int32 top,
    /* [in] */ Int32 dy,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IViewGroupLayoutParams> params;
    mHost->mSlideableView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    LayoutParams* lp = (LayoutParams*)params.Get();

    Int32 newTop;
    Int32 previousTop = top - dy;
    Int32 paddingTop;
    mHost->GetPaddingTop(&paddingTop);
    Int32 topBound = paddingTop + lp->mTopMargin;

    Boolean isUnscrolled;
    Int32 bottomBound = topBound +
            ((mHost->mPanelSlideCallbacks->IsScrollableChildUnscrolled(&isUnscrolled), isUnscrolled)
            || !mHost->mIsInNestedScroll ? mHost->mSlideRange : mHost->mIntermediateOffset);
    if (previousTop > bottomBound) {
        // We were previously below the bottomBound, so loosen the bottomBound so that this
        // makes sense. This can occur after the view was directly dragged by the tabs.
        bottomBound = Elastos::Core::Math::Max(bottomBound, mHost->mSlideRange);
    }
    newTop = Elastos::Core::Math::Min(Elastos::Core::Math::Max(top, topBound), bottomBound);

    *result = newTop;
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::OnEdgeDragStarted(
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 pointerId)
{
    return mHost->mDragHelper->CaptureChildView(
            mHost->mSlideableView, pointerId);
}


//================================================================
// OverlappingPaneLayout::LayoutParams
//================================================================
const AutoPtr< ArrayOf<Int32> > OverlappingPaneLayout::LayoutParams::ATTRS = OverlappingPaneLayout::LayoutParams::Init_ATTRS();

CAR_INTERFACE_IMPL(OverlappingPaneLayout::LayoutParams, ViewGroup::MarginLayoutParams,
        IOverlappingPaneLayoutLayoutParams);

OverlappingPaneLayout::LayoutParams::LayoutParams()
    : mWeight(0)
    , mSlideable(FALSE)
{}

ECode OverlappingPaneLayout::LayoutParams::constructor()
{
    return ViewGroup::MarginLayoutParams::constructor(FILL_PARENT, FILL_PARENT);
}

ECode OverlappingPaneLayout::LayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return ViewGroup::MarginLayoutParams::constructor(width, height);
}

ECode OverlappingPaneLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}

ECode OverlappingPaneLayout::LayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return ViewGroup::MarginLayoutParams::constructor(source);
}


ECode OverlappingPaneLayout::LayoutParams::constructor(
    /* [in] */ IOverlappingPaneLayoutLayoutParams* source)
{
    ViewGroup::MarginLayoutParams::constructor(IViewGroupMarginLayoutParams::Probe(source));
    mWeight = ((LayoutParams*)source)->mWeight;
    return NOERROR;
}

ECode OverlappingPaneLayout::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::MarginLayoutParams::constructor(c, attrs);

    AutoPtr<ITypedArray> a;
    c->ObtainStyledAttributes(attrs, ATTRS, (ITypedArray**)&a);
    Float weight;
    a->GetFloat(0, 0, &weight);
    mWeight = weight;
    a->Recycle();
    return NOERROR;
}

AutoPtr< ArrayOf<Int32> > OverlappingPaneLayout::LayoutParams::Init_ATTRS()
{
    AutoPtr< ArrayOf<Int32> > attrs = ArrayOf<Int32>::Alloc(1);
    (*attrs)[0] = Elastos::Droid::R::attr::layout_weight;
    return attrs;
}


//================================================================
// OverlappingPaneLayout::SavedState
//================================================================
OverlappingPaneLayout::SavedState::SavedState()
    : mIsOpen(FALSE)
{}

ECode OverlappingPaneLayout::SavedState::constructor()
{
    return BaseSavedState::constructor();
}

ECode OverlappingPaneLayout::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return BaseSavedState::constructor(superState);
}

ECode OverlappingPaneLayout::SavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    BaseSavedState::WriteToParcel(out);
    out->WriteInt32(mIsOpen? 1 : 0);
    return NOERROR;
}

ECode OverlappingPaneLayout::SavedState::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    BaseSavedState::ReadFromParcel(in);
    Int32 value;
    in->ReadInt32(&value);
    mIsOpen = value != 0;
    return NOERROR;
}


// //================================================================
// // OverlappingPaneLayout::AccessibilityDelegate
// //================================================================
// // OverlappingPaneLayout::AccessibilityDelegate::AccessibilityDelegate(
// //     /* [in] */ OverlappingPaneLayout* host)
// //     : mHost(host)
// // {}

// // ECode OverlappingPaneLayout::AccessibilityDelegate::OnInitializeAccessibilityNodeInfo(
// //     /* [in] */ IView* host,
// //     /* [in] */ IAccessibilityNodeInfoCompat* info)
// // {
// //     final AccessibilityNodeInfoCompat superNode = AccessibilityNodeInfoCompat.obtain(info);
// //     AccessibilityDelegateCompat::OnInitializeAccessibilityNodeInfo(host, superNode);
// //     copyNodeInfoNoChildren(info, superNode);
// //     superNode.recycle();

// //     info.setClassName(OverlappingPaneLayout.class.getName());
// //     info.setSource(host);

// //     final ViewParent parent = ViewCompat.getParentForAccessibility(host);
// //     if (parent instanceof View) {
// //         info.setParent((View) parent);
// //     }

// //     // This is a best-approximation of addChildrenForAccessibility()
// //     // that accounts for filtering.
// //     final Int32 childCount = getChildCount();
// //     for (Int32 i = 0; i < childCount; i++) {
// //         final View child = getChildAt(i);
// //         if (child.getVisibility() == View.VISIBLE) {
// //             // Force importance to "yes" since we can't read the value.
// //             ViewCompat.setImportantForAccessibility(
// //                     child, ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_YES);
// //             info.addChild(child);
// //         }
// //     }
// // }

// // ECode OverlappingPaneLayout::AccessibilityDelegate::OnInitializeAccessibilityEvent(
// //     /* [in] */ IView* host,
// //     /* [in] */ IAccessibilityEvent* event)
// // {
// //     super.onInitializeAccessibilityEvent(host, event);

// //     event.setClassName(OverlappingPaneLayout.class.getName());
// // }


//================================================================
// OverlappingPaneLayout
//================================================================
const String OverlappingPaneLayout::TAG("SlidingPaneLayout");
const Boolean OverlappingPaneLayout::DEBUG = FALSE;

const Int32 OverlappingPaneLayout::DEFAULT_OVERHANG_SIZE = 32; // dp;

const Int32 OverlappingPaneLayout::DEFAULT_FADE_COLOR = 0xcccccccc;

const Int32 OverlappingPaneLayout::MIN_FLING_VELOCITY = 400; // dips per second

CAR_INTERFACE_IMPL(OverlappingPaneLayout, ViewGroup, IOverlappingPaneLayout)

OverlappingPaneLayout::OverlappingPaneLayout()
    : mOverhangSize(0)
    , mCanSlide(FALSE)
    , mSlideOffset(0)
    , mSlideOffsetPx(0)
    , mSlideRange(0)
    , mIsUnableToDrag(FALSE)
    , mIsInNestedScroll(FALSE)
    , mInNestedPreScrollDownwards(FALSE)
    , mInNestedPreScrollUpwards(FALSE)
    , mIsInNestedFling(FALSE)
    , mInUpwardsPreFling(FALSE)
    , mIntermediateOffset(0)
    , mInitialMotionX(0)
    , mInitialMotionY(0)
    , mPreservedOpenState(FALSE)
    , mFirstLayout(TRUE)
    , mReleaseScrollSlop(0)
{
    CRect::New((IRect**)&mTmpRect);
}

ECode OverlappingPaneLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode OverlappingPaneLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode OverlappingPaneLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs, defStyle));

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);
    mOverhangSize = (Int32)(DEFAULT_OVERHANG_SIZE * density + 0.5f);

    SetWillNotDraw(FALSE);

    // TODO:
    // ViewCompat.setAccessibilityDelegate(this, new AccessibilityDelegate());
    // ViewCompat.setImportantForAccessibility(this, ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_YES);

    mDragHelper = ViewDragHelper::Create(this, 0.5f, new DragHelperCallback(this));
    mDragHelper->SetMinVelocity(MIN_FLING_VELOCITY * density);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(ctx, (IViewConfiguration**)&vc);
    vc->GetScaledTouchSlop(&mReleaseScrollSlop);
    return NOERROR;
}

ECode OverlappingPaneLayout::SetIntermediatePinnedOffset(
    /* [in] */ Int32 offset)
{
    mIntermediateOffset = offset;
    return NOERROR;
}

ECode OverlappingPaneLayout::SetCapturableView(
    /* [in] */ IView* capturableView)
{
    mCapturableView = capturableView;
    return NOERROR;
}

ECode OverlappingPaneLayout::SetPanelSlideCallbacks(
    /* [in] */ IPanelSlideCallbacks* listener)
{
    mPanelSlideCallbacks = listener;
    return NOERROR;
}

void OverlappingPaneLayout::DispatchOnPanelSlide(
    /* [in] */ IView* panel)
{
    mPanelSlideCallbacks->OnPanelSlide(panel, mSlideOffset);
}

void OverlappingPaneLayout::DispatchOnPanelOpened(
    /* [in] */ IView* panel)
{
    mPanelSlideCallbacks->OnPanelOpened(panel);
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
}

void OverlappingPaneLayout::DispatchOnPanelClosed(
    /* [in] */ IView* panel)
{
    mPanelSlideCallbacks->OnPanelClosed(panel);
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
}

void OverlappingPaneLayout::UpdateObscuredViewsVisibility(
    /* [in] */ IView* panel)
{
    Int32 startBound;
    GetPaddingTop(&startBound);
    Int32 height, paddingBottom;
    GetHeight(&height);
    GetPaddingBottom(&paddingBottom);
    Int32 endBound = height - paddingBottom;

    Int32 leftBound;
    GetPaddingLeft(&leftBound);
    Int32 width, paddingRight;
    GetWidth(&width);
    GetPaddingRight(&paddingRight);
    Int32 rightBound = width - paddingRight;
    Int32 left;
    Int32 right;
    Int32 top;
    Int32 bottom;
    if (panel != NULL && ViewIsOpaque(panel)) {
        panel->GetLeft(&left);
        panel->GetRight(&right);
        panel->GetTop(&top);
        panel->GetBottom(&bottom);
    }
    else {
        left = right = top = bottom = 0;
    }

    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        if (child.Get() == panel) {
            // There are still more children above the panel but they won't be affected.
            break;
        }

        Int32 cLeft, cRight, cTop, cBottom;
        child->GetLeft(&cLeft);
        child->GetRight(&cRight);
        child->GetTop(&cTop);
        child->GetBottom(&cBottom);
        Int32 clampedChildLeft = Elastos::Core::Math::Max(leftBound, cLeft);
        Int32 clampedChildRight = Elastos::Core::Math::Min(rightBound, cRight);
        Int32 clampedChildTop = Elastos::Core::Math::Max(startBound, cTop);
        Int32 clampedChildBottom = Elastos::Core::Math::Min(endBound, cBottom);

        Int32 vis;
        if (clampedChildLeft >= left && clampedChildTop >= top &&
                clampedChildRight <= right && clampedChildBottom <= bottom) {
            vis = IView::INVISIBLE;
        }
        else {
            vis = IView::VISIBLE;
        }
        child->SetVisibility(vis);
    }
}

void OverlappingPaneLayout::SetAllChildrenVisible()
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        if (child->GetVisibility(&visibility), visibility == IView::INVISIBLE) {
            child->SetVisibility(IView::VISIBLE);
        }
    }
}

Boolean OverlappingPaneLayout::ViewIsOpaque(
    /* [in] */ IView* v)
{
    // TODO:
    // if (ViewCompat.isOpaque(v)) return TRUE;

    AutoPtr<IDrawable> bg;
    v->GetBackground((IDrawable**)&bg);
    if (bg != NULL) {
        Int32 opacity;
        bg->GetOpacity(&opacity);
        return opacity == IPixelFormat::OPAQUE;
    }
    return FALSE;
}

ECode OverlappingPaneLayout::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();
    mFirstLayout = TRUE;
    return NOERROR;
}

ECode OverlappingPaneLayout::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();
    mFirstLayout = TRUE;
    return NOERROR;
}

ECode OverlappingPaneLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 widthSize = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 heightSize = View::MeasureSpec::GetSize(heightMeasureSpec);

    if (widthMode != View::MeasureSpec::EXACTLY) {
        Boolean isInEditMode;
        if (IsInEditMode(&isInEditMode), isInEditMode) {
            // Don't crash the layout editor. Consume all of the space if specified
            // or pick a magic number from thin air otherwise.
            // TODO Better communication with tools of this bogus state.
            // It will crash on a real device.
            if (widthMode == View::MeasureSpec::AT_MOST) {
                widthMode = View::MeasureSpec::EXACTLY;
            }
            else if (widthMode == View::MeasureSpec::UNSPECIFIED) {
                widthMode = View::MeasureSpec::EXACTLY;
                widthSize = 300;
            }
        }
        else {
            // throw new IllegalStateException("Width must have an exact value or MATCH_PARENT");
            return NOERROR;
        }
    }
    else if (heightMode == View::MeasureSpec::UNSPECIFIED) {
        Boolean isInEditMode;
        if (IsInEditMode(&isInEditMode), isInEditMode) {
            // Don't crash the layout editor. Pick a magic number from thin air instead.
            // TODO Better communication with tools of this bogus state.
            // It will crash on a real device.
            if (heightMode == View::MeasureSpec::UNSPECIFIED) {
                heightMode = View::MeasureSpec::AT_MOST;
                heightSize = 300;
            }
        }
        else {
            // throw new IllegalStateException("Height must not be UNSPECIFIED");
            assert(0 && "TODO");
            return NOERROR;
        }
    }

    Int32 layoutWidth = 0;
    Int32 maxLayoutWidth = -1;
    switch (widthMode) {
        case View::MeasureSpec::EXACTLY: {
            Int32 paddingLeft, paddingRight;
            GetPaddingLeft(&paddingLeft);
            GetPaddingRight(&paddingRight);
            layoutWidth = maxLayoutWidth = widthSize - paddingLeft - paddingRight;
            break;
        }
        case View::MeasureSpec::AT_MOST: {
            Int32 paddingLeft, paddingRight;
            GetPaddingLeft(&paddingLeft);
            GetPaddingRight(&paddingRight);
            maxLayoutWidth = widthSize - paddingLeft - paddingRight;
            break;
        }
    }

    Float weightSum = 0;
    Boolean canSlide = FALSE;
    Int32 paddingTop, paddingBottom;
    GetPaddingTop(&paddingTop);
    GetPaddingBottom(&paddingBottom);
    Int32 heightAvailable = heightSize - paddingTop - paddingBottom;
    Int32 heightRemaining = heightAvailable;
    Int32 childCount;
    GetChildCount(&childCount);

    if (childCount > 2) {
        Logger::E(TAG, "onMeasure: More than two child views are not supported.");
    }

    // We'll find the current one below.
    mSlideableView = NULL;

    // First pass. Measure based on child LayoutParams width/height.
    // Weight will incur a second pass.
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        LayoutParams* lp = (LayoutParams*)params.Get();

        Int32 visibility;
        if (child->GetVisibility(&visibility), visibility == IView::GONE) {
            continue;
        }

        if (lp->mWeight > 0) {
            weightSum += lp->mWeight;

            // If we have no height, weight is the only contributor to the final size.
            // Measure this view on the weight pass only.
            if (lp->mHeight == 0) continue;
        }

        Int32 childHeightSpec;
        Int32 verticalMargin = lp->mTopMargin + lp->mBottomMargin;
        if (lp->mHeight == IViewGroupLayoutParams::WRAP_CONTENT) {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(heightAvailable - verticalMargin,
                    View::MeasureSpec::AT_MOST);
        }
        else if (lp->mHeight == IViewGroupLayoutParams::MATCH_PARENT) {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(heightAvailable - verticalMargin,
                    View::MeasureSpec::EXACTLY);
        }
        else {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(lp->mHeight, View::MeasureSpec::EXACTLY);
        }

        Int32 childWidthSpec;
        if (lp->mWidth == IViewGroupLayoutParams::WRAP_CONTENT) {
            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                    maxLayoutWidth, View::MeasureSpec::AT_MOST);
        }
        else if (lp->mWidth == IViewGroupLayoutParams::MATCH_PARENT) {
            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                    maxLayoutWidth, View::MeasureSpec::EXACTLY);
        }
        else {
            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                    lp->mWidth, View::MeasureSpec::EXACTLY);
        }

        child->Measure(childWidthSpec, childHeightSpec);
        Int32 childWidth;
        child->GetMeasuredWidth(&childWidth);
        Int32 childHeight;
        child->GetMeasuredHeight(&childHeight);

        if (widthMode == View::MeasureSpec::AT_MOST && childWidth > layoutWidth) {
            layoutWidth = Elastos::Core::Math::Min(childWidth, maxLayoutWidth);
        }

        heightRemaining -= childHeight;
        canSlide |= lp->mSlideable = heightRemaining < 0;
        if (lp->mSlideable) {
            mSlideableView = child;
        }
    }

    // Resolve weight and make sure non-sliding panels are smaller than the full screen.
    if (canSlide || weightSum > 0) {
        Int32 fixedPanelHeightLimit = heightAvailable - mOverhangSize;

        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);

            Int32 visibility;
            if (child->GetVisibility(&visibility), visibility == IView::GONE) {
                continue;
            }

            AutoPtr<IViewGroupLayoutParams> params;
            child->GetLayoutParams((IViewGroupLayoutParams**)&params);
            LayoutParams* lp = (LayoutParams*)params.Get();

            if (child->GetVisibility(&visibility), visibility == IView::GONE) {
                continue;
            }

            Boolean skippedFirstPass = lp->mHeight == 0 && lp->mWeight > 0;
            Int32 childMeasuredHeight;
            Int32 measuredHeight = skippedFirstPass ? 0 :
                    (child->GetMeasuredHeight(&childMeasuredHeight), childMeasuredHeight);
            if (canSlide && child != mSlideableView) {
                if (lp->mHeight < 0 && (measuredHeight > fixedPanelHeightLimit
                        || lp->mWeight > 0)) {
                    // Fixed panels in a sliding configuration should
                    // be clamped to the fixed panel limit.
                    Int32 childWidthSpec;
                    if (skippedFirstPass) {
                        // Do initial width measurement if we skipped measuring this view
                        // the first time around.
                        if (lp->mWidth == IViewGroupLayoutParams::WRAP_CONTENT) {
                            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                    View::MeasureSpec::AT_MOST);
                        }
                        else if (lp->mHeight == IViewGroupLayoutParams::MATCH_PARENT) {
                            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                    View::MeasureSpec::EXACTLY);
                        }
                        else {
                            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(lp->mWidth,
                                    View::MeasureSpec::EXACTLY);
                        }
                    }
                    else {
                        child->GetMeasuredHeight(&childMeasuredHeight);
                        childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                                childMeasuredHeight, View::MeasureSpec::EXACTLY);
                    }
                    Int32 childHeightSpec = View::MeasureSpec::MakeMeasureSpec(
                            fixedPanelHeightLimit, View::MeasureSpec::EXACTLY);
                    child->Measure(childWidthSpec, childHeightSpec);
                }
            }
            else if (lp->mWeight > 0) {
                Int32 childWidthSpec;
                if (lp->mHeight == 0) {
                    // This was skipped the first time; figure out a real width spec.
                    if (lp->mWidth == IViewGroupLayoutParams::WRAP_CONTENT) {
                        childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                View::MeasureSpec::AT_MOST);
                    }
                    else if (lp->mWidth == IViewGroupLayoutParams::MATCH_PARENT) {
                        childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                View::MeasureSpec::EXACTLY);
                    }
                    else {
                        childWidthSpec = View::MeasureSpec::MakeMeasureSpec(lp->mWidth,
                                View::MeasureSpec::EXACTLY);
                    }
                }
                else {
                    Int32 measuredWidth;
                    child->GetMeasuredWidth(&measuredWidth);
                    childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                            measuredWidth, View::MeasureSpec::EXACTLY);
                }

                if (canSlide) {
                    // Consume available space
                    Int32 verticalMargin = lp->mTopMargin + lp->mBottomMargin;
                    Int32 newHeight = heightAvailable - verticalMargin;
                    Int32 childHeightSpec = View::MeasureSpec::MakeMeasureSpec(
                            newHeight, View::MeasureSpec::EXACTLY);
                    if (measuredHeight != newHeight) {
                        child->Measure(childWidthSpec, childHeightSpec);
                    }
                }
                else {
                    // Distribute the extra width proportionally similar to LinearLayout
                    Int32 heightToDistribute = Elastos::Core::Math::Max(0, heightRemaining);
                    Int32 addedHeight = (Int32)(lp->mWeight * heightToDistribute / weightSum);
                    Int32 childHeightSpec = View::MeasureSpec::MakeMeasureSpec(
                            measuredHeight + addedHeight, View::MeasureSpec::EXACTLY);
                    child->Measure(childWidthSpec, childHeightSpec);
                }
            }
        }
    }

    Int32 measuredHeight = heightSize;
    Int32 paddingLeft, paddingRight;
    GetPaddingLeft(&paddingLeft);
    GetPaddingRight(&paddingRight);
    Int32 measuredWidth = layoutWidth + paddingLeft + paddingRight;

    SetMeasuredDimension(measuredWidth, measuredHeight);
    mCanSlide = canSlide;

    if (mDragHelper->GetViewDragState() != ViewDragHelper::STATE_IDLE && !canSlide) {
        // Cancel scrolling in progress, it's no longer relevant.
        mDragHelper->Abort();
    }
    return NOERROR;
}

ECode OverlappingPaneLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    mDragHelper->SetEdgeTrackingEnabled(ViewDragHelper::EDGE_TOP);

    Int32 height = b - t;
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 paddingBottom;
    GetPaddingBottom(&paddingBottom);
    Int32 paddingLeft;
    GetPaddingLeft(&paddingLeft);

    Int32 childCount;
    GetChildCount(&childCount);
    Int32 yStart = paddingTop;
    Int32 nextYStart = yStart;

    if (mFirstLayout) {
        mSlideOffset = mCanSlide && mPreservedOpenState ? 1.f : 0.f;
    }

    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);

        Int32 visibility;
        if (child->GetVisibility(&visibility), visibility == IView::GONE) {
            continue;
        }

        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        LayoutParams* lp = (LayoutParams*)params.Get();

        Int32 childHeight;
        child->GetMeasuredHeight(&childHeight);

        if (lp->mSlideable) {
            Int32 margin = lp->mTopMargin + lp->mBottomMargin;
            Int32 range = Elastos::Core::Math::Min(nextYStart,
                    height - paddingBottom - mOverhangSize) - yStart - margin;
            mSlideRange = range;
            Int32 lpMargin = lp->mTopMargin;
            Int32 pos = (Int32)(range * mSlideOffset);
            yStart += pos + lpMargin;
            UpdateSlideOffset(pos);
        }
        else {
            yStart = nextYStart;
        }

        Int32 childTop = yStart;
        Int32 childBottom = childTop + childHeight;
        Int32 childLeft = paddingLeft;
        Int32 measuredWidth;
        child->GetMeasuredWidth(&measuredWidth);
        Int32 childRight = childLeft + measuredWidth;

        child->Layout(childLeft, childTop, childRight, childBottom);

        Int32 height;
        child->GetHeight(&height);
        nextYStart += height;
    }

    if (mFirstLayout) {
        UpdateObscuredViewsVisibility(mSlideableView);
    }

    mFirstLayout = FALSE;
    return NOERROR;
}

ECode OverlappingPaneLayout::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    ViewGroup::OnSizeChanged(w, h, oldw, oldh);
    // Recalculate sliding panes and their details
    if (h != oldh) {
        mFirstLayout = TRUE;
    }
    return NOERROR;
}

ECode OverlappingPaneLayout::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    ViewGroup::RequestChildFocus(child, focused);
    Boolean isInTouchMode;
    if ((IsInTouchMode(&isInTouchMode), !isInTouchMode) && !mCanSlide) {
        mPreservedOpenState = child == mSlideableView;
    }
    return NOERROR;
}

ECode OverlappingPaneLayout::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 action = MotionEventCompat::GetActionMasked(ev);

    // Preserve the open state based on the last view that was touched.
    Int32 count;
    if (!mCanSlide && action == IMotionEvent::ACTION_DOWN
            && (GetChildCount(&count), count > 1)) {
        // After the first things will be slideable.
        AutoPtr<IView> secondChild;
        GetChildAt(1, (IView**)&secondChild);
        if (secondChild != NULL) {
            Float x, y;
            ev->GetX(&x);
            ev->GetY(&y);
            mPreservedOpenState = !mDragHelper->IsViewUnder(secondChild,
                    (Int32)x, (Int32)y);
        }
    }

    if (!mCanSlide || (mIsUnableToDrag && action != IMotionEvent::ACTION_DOWN)) {
        if (!mIsInNestedScroll) {
            mDragHelper->Cancel();
        }
        return ViewGroup::OnInterceptTouchEvent(ev, res);
    }

    if (action == IMotionEvent::ACTION_CANCEL || action == IMotionEvent::ACTION_UP) {
        if (!mIsInNestedScroll) {
            mDragHelper->Cancel();
        }
        *res = FALSE;
        return NOERROR;
    }

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            mIsUnableToDrag = FALSE;
            Float x;
            ev->GetX(&x);
            Float y;
            ev->GetY(&y);
            mInitialMotionX = x;
            mInitialMotionY = y;

            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Float x;
            ev->GetX(&x);
            Float y;
            ev->GetY(&y);
            Float adx = Elastos::Core::Math::Abs(x - mInitialMotionX);
            Float ady = Elastos::Core::Math::Abs(y - mInitialMotionY);
            Int32 slop = mDragHelper->GetTouchSlop();
            if ((ady > slop && adx > ady) || !IsCapturableViewUnder((Int32) x, (Int32) y)) {
                if (!mIsInNestedScroll) {
                    mDragHelper->Cancel();
                }
                mIsUnableToDrag = TRUE;
                *res = FALSE;
                return NOERROR;
            }
        }
    }

    Boolean interceptForDrag = mDragHelper->ShouldInterceptTouchEvent(ev);

    *res = interceptForDrag;
    return NOERROR;
}

ECode OverlappingPaneLayout::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (!mCanSlide) {
        return ViewGroup::OnTouchEvent(event, res);
    }

    mDragHelper->ProcessTouchEvent(event);

    Int32 action;
    event->GetAction(&action);
    Boolean wantTouchEvents = TRUE;

    assert(0 && "TODO");
    // switch (action & MotionEventCompat.ACTION_MASK) {
    //     case IMotionEvent::ACTION_DOWN: {
    //         Float x;
    //         ev->GetX(&x);
    //         Float y;
    //         ev->GetY(&y);
    //         mInitialMotionX = x;
    //         mInitialMotionY = y;
    //         break;
    //     }
    // }

    *res = wantTouchEvents;
    return NOERROR;
}

Boolean OverlappingPaneLayout::ClosePane(
    /* [in] */ IView* pane,
    /* [in] */ Int32 initialVelocity)
{
    if (mFirstLayout || SmoothSlideTo(0.f, initialVelocity)) {
        mPreservedOpenState = FALSE;
        return TRUE;
    }
    return FALSE;
}

Boolean OverlappingPaneLayout::OpenPane(
    /* [in] */ IView* pane,
    /* [in] */ Int32 initialVelocity)
{
    if (mFirstLayout || SmoothSlideTo(1.f, initialVelocity)) {
        mPreservedOpenState = TRUE;
        return TRUE;
    }
    return FALSE;
}

void OverlappingPaneLayout::UpdateSlideOffset(
    /* [in] */ Int32 offsetPx)
{
    mSlideOffsetPx = offsetPx;
    mSlideOffset = (Float) mSlideOffsetPx / mSlideRange;
}

ECode OverlappingPaneLayout::OpenPane(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = OpenPane(mSlideableView, 0);
    return NOERROR;
}

ECode OverlappingPaneLayout::ClosePane(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ClosePane(mSlideableView, 0);
    return NOERROR;
}

ECode OverlappingPaneLayout::IsOpen(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !mCanSlide || mSlideOffset > 0;
    return NOERROR;
}

ECode OverlappingPaneLayout::IsSlideable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCanSlide;
    return NOERROR;
}

void OverlappingPaneLayout::OnPanelDragged(
    /* [in] */ Int32 newTop)
{
    if (mSlideableView == NULL) {
        // This can happen if we're aborting motion during layout because everything now fits.
        mSlideOffset = 0;
        return;
    }
    AutoPtr<IViewGroupLayoutParams> params;
    mSlideableView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    LayoutParams* lp = (LayoutParams*)params.Get();

    Int32 lpMargin = lp->mTopMargin;
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 topBound = paddingTop + lpMargin;

    UpdateSlideOffset(newTop - topBound);

    DispatchOnPanelSlide(mSlideableView);
}

Boolean OverlappingPaneLayout::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    LayoutParams* lp = (LayoutParams*)params.Get();
    Boolean result;
    Int32 save;
    canvas->Save(ICanvas::CLIP_SAVE_FLAG, &save);

    if (mCanSlide && !lp->mSlideable && mSlideableView != NULL) {
        // Clip against the slider; no sense drawing what will immediately be covered.
        canvas->GetClipBounds(mTmpRect, &result);

        Int32 bottom;
        mTmpRect->GetBottom(&bottom);
        Int32 top;
        mSlideableView->GetTop(&top);
        mTmpRect->SetBottom(Elastos::Core::Math::Min(bottom, top));
        canvas->ClipRect(mTmpRect, &result);
    }

    if (Build::VERSION::SDK_INT >= 11) { // HC
        result = ViewGroup::DrawChild(canvas, child, drawingTime);
    }
    else {
        Boolean isEnabled;
        if (child->IsDrawingCacheEnabled(&isEnabled), isEnabled) {
            child->SetDrawingCacheEnabled(FALSE);
        }
        result = ViewGroup::DrawChild(canvas, child, drawingTime);
    }

    canvas->RestoreToCount(save);

    return result;
}

Boolean OverlappingPaneLayout::SmoothSlideTo(
    /* [in] */ Float slideOffset,
    /* [in] */ Int32 velocity)
{
    if (!mCanSlide) {
        // Nothing to do.
        return FALSE;
    }

    AutoPtr<IViewGroupLayoutParams> params;
    mSlideableView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    LayoutParams* lp = (LayoutParams*)params.Get();

    Int32 y;
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 topBound = paddingTop + lp->mTopMargin;
    y = (Int32) (topBound + slideOffset * mSlideRange);

    Int32 left;
    mSlideableView->GetLeft(&left);
    if (mDragHelper->SmoothSlideViewTo(mSlideableView, left, y)) {
        SetAllChildrenVisible();
        assert(0 && "TODO");
        // ViewCompat.postInvalidateOnAnimation(this);
        return TRUE;
    }
    return FALSE;
}

ECode OverlappingPaneLayout::ComputeScroll()
{
    if (mDragHelper->ContinueSettling(
            /* deferCallbacks = */ FALSE)) {
        if (!mCanSlide) {
            mDragHelper->Abort();
            return NOERROR;
        }

        assert(0 && "TODO");
        // ViewCompat.postInvalidateOnAnimation(this);
    }
    return NOERROR;
}

Boolean OverlappingPaneLayout::IsCapturableViewUnder(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<IView> capturableView = mCapturableView != NULL ? mCapturableView : mSlideableView;
    if (capturableView == NULL) {
        return FALSE;
    }
    AutoPtr<ArrayOf<Int32> > viewLocation = ArrayOf<Int32>::Alloc(2);
    capturableView->GetLocationOnScreen(viewLocation);
    AutoPtr<ArrayOf<Int32> > parentLocation = ArrayOf<Int32>::Alloc(2);
    GetLocationOnScreen(parentLocation);
    Int32 screenX = (*parentLocation)[0] + x;
    Int32 screenY = (*parentLocation)[1] + y;

    Int32 width, height;
    capturableView->GetWidth(&width);
    capturableView->GetHeight(&height);
    return screenX >= (*viewLocation)[0]
            && screenX < (*viewLocation)[0] + width
            && screenY >= (*viewLocation)[1]
            && screenY < (*viewLocation)[1] + height;
}

ECode OverlappingPaneLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<LayoutParams> lp = new LayoutParams();
    lp->constructor();
    *params = (IViewGroupLayoutParams*)lp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> OverlappingPaneLayout::GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<LayoutParams> lp;
    if (IViewGroupMarginLayoutParams::Probe(p) != NULL){
        lp = new LayoutParams();
        lp->constructor(IViewGroupMarginLayoutParams::Probe(p));
    }
    else {
        lp = new LayoutParams();
        lp->constructor(p);
    }
    return (IViewGroupLayoutParams*)lp;
}

Boolean OverlappingPaneLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return IOverlappingPaneLayoutLayoutParams::Probe(p) != NULL
            && ViewGroup::CheckLayoutParams(p);
}

ECode OverlappingPaneLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<LayoutParams> lp = new LayoutParams();
    lp->constructor(context, attrs);
    *params = (IViewGroupLayoutParams*)lp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

AutoPtr<IParcelable> OverlappingPaneLayout::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = ViewGroup::OnSaveInstanceState();

    AutoPtr<COverlappingPaneLayoutSavedState> ss;
    COverlappingPaneLayoutSavedState::NewByFriend(superState, (COverlappingPaneLayoutSavedState**)&ss);
    Boolean isSlideable;
    IsSlideable(&isSlideable);
    Boolean isOpen;
    ss->mIsOpen = isSlideable ? (IsOpen(&isOpen), isOpen) : mPreservedOpenState;

    return (IParcelable*)ss;
}

ECode OverlappingPaneLayout::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<SavedState> ss = (SavedState*)state;
    AutoPtr<IParcelable> p;
    ss->GetSuperState((IParcelable**)&p);
    ViewGroup::OnRestoreInstanceState(p);

    if (ss->mIsOpen) {
        Boolean result;
        OpenPane(&result);
    }
    else {
        Boolean result;
        ClosePane(&result);
    }
    mPreservedOpenState = ss->mIsOpen;
    return NOERROR;
}

ECode OverlappingPaneLayout::OnStartNestedScroll(
    /* [in] */ IView* child,
    /* [in] */ IView* target,
    /* [in] */ Int32 nestedScrollAxes,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean startNestedScroll = (nestedScrollAxes & SCROLL_AXIS_VERTICAL) != 0;
    if (startNestedScroll) {
        mIsInNestedScroll = TRUE;
        mDragHelper->StartNestedScroll(mSlideableView);
    }
    if (DEBUG) {
        Logger::D(TAG, "onStartNestedScroll: %d", startNestedScroll);
    }
    *res = startNestedScroll;
    return NOERROR;
}

ECode OverlappingPaneLayout::OnNestedPreScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ ArrayOf<Int32>* consumed)
{
    if (dy == 0) {
        // Nothing to do
        return NOERROR;
    }
    if (DEBUG) {
        Logger::D(TAG, "onNestedPreScroll: %d", dy);
    }

    mInNestedPreScrollDownwards = dy < 0;
    mInNestedPreScrollUpwards = dy > 0;
    mIsInNestedFling = FALSE;
    mDragHelper->ProcessNestedScroll(mSlideableView, 0, -dy, consumed);
    return NOERROR;
}

ECode OverlappingPaneLayout::OnNestedPreFling(
    /* [in] */ IView* target,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean result;
    if (!(velocityY > 0 && mSlideOffsetPx != 0
            || velocityY < 0 && mSlideOffsetPx < mIntermediateOffset
            || velocityY < 0 && mSlideOffsetPx < mSlideRange
            && (mPanelSlideCallbacks->IsScrollableChildUnscrolled(&result), result))) {
        // No need to consume the fling if the fling won't collapse or expand the header.
        // How far we are willing to expand the header depends on isScrollableChildUnscrolled().
        *res = FALSE;
        return NOERROR;
    }

    if (DEBUG) {
        Logger::D(TAG, "onNestedPreFling: %d", velocityY);
    }
    mInUpwardsPreFling = velocityY > 0;
    mIsInNestedFling = TRUE;
    mIsInNestedScroll = FALSE;
    mDragHelper->ProcessNestedFling(mSlideableView, (Int32) -velocityY);
    *res = TRUE;
    return NOERROR;
}

ECode OverlappingPaneLayout::OnNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dxConsumed,
    /* [in] */ Int32 dyConsumed,
    /* [in] */ Int32 dxUnconsumed,
    /* [in] */ Int32 dyUnconsumed)
{
    if (DEBUG) {
        Logger::D(TAG, "onNestedScroll: %d", dyUnconsumed);
    }
    mIsInNestedFling = FALSE;
    mDragHelper->ProcessNestedScroll(mSlideableView, 0, -dyUnconsumed, NULL);
    return NOERROR;
}

ECode OverlappingPaneLayout::OnStopNestedScroll(
    /* [in] */ IView* child)
{
    if (DEBUG) {
        Logger::D(TAG, "onStopNestedScroll");
    }
    if (mIsInNestedScroll && !mIsInNestedFling) {
        mDragHelper->StopNestedScroll(mSlideableView);
        mInNestedPreScrollDownwards = FALSE;
        mInNestedPreScrollUpwards = FALSE;
        mIsInNestedScroll = FALSE;
    }
    return NOERROR;
}

} // Widget
} // Dialer
} // Droid
} // Elastos
