
#include "elastos/droid/dialer/widget/OverlappingPaneLayout.h"
#include "elastos/droid/dialer/widget/ViewDragHelper.h"
#include <elastos/droid/view/View.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::View::View;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Widget {

//================================================================
// OverlappingPaneLayout::LayoutParams
//================================================================
const Int32 OverlappingPaneLayout::LayoutParams::ATTRS = {
    Elastos::R::attr::layout_weight
};

CAR_INTERFACE_IMPL(OverlappingPaneLayout::LayoutParams, ViewGroup::MarginLayoutParams,
        IOverlappingPaneLayoutLayoutParams);

OverlappingPaneLayout::LayoutParams::LayoutParams(
    /* [in] */ OverlappingPaneLayout* host)
    : mHost(host)
    , mWeight(0)
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


ECode OverlappingPaneLayout::SavedState::OverlappingPaneLayout::LayoutParams::constructor(
    /* [in] */ IOverlappingPaneLayoutLayoutParams* source)
{
    ViewGroup::MarginLayoutParams::constructor(source);
    mWeight = ((LayoutParams*)source)->mWeight;
    return NOERROR;
}

ECode OverlappingPaneLayout::LayoutParams::constructor(
    /* [in] */ IContext* c,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::MarginLayoutParams::constructor(c, attrs);

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(ATTRS, sizeof(ATTRS) / sizeof(Int32));
    c->ObtainStyledAttributes(attrs, array, (ITypedArray**)&a);
    Float weight;
    a->GetFloat(0, 0, &weight);
    mWeight = weight;
    a->Recycle();
    return NOERROR;
}

//================================================================
// OverlappingPaneLayout::SavedState
//================================================================
OverlappingPaneLayout::SavedState::SavedState(
    /* [in] */ OverlappingPaneLayout* host)
    : mHost(host)
{}

ECode OverlappingPaneLayout::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return BaseSavedState::constructor(superState);
}

ECode OverlappingPaneLayout::SavedState::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    BaseSavedState::WriteToParcel(out, flags);
    out->WriteInt32(mIsOpen ? 1 : 0);
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

//================================================================
// OverlappingPaneLayout::AccessibilityDelegate
//================================================================
// OverlappingPaneLayout::AccessibilityDelegate::AccessibilityDelegate(
//     /* [in] */ OverlappingPaneLayout* host)
//     : mHost(host)
// {}

// ECode OverlappingPaneLayout::AccessibilityDelegate::OnInitializeAccessibilityNodeInfo(
//     /* [in] */ IView* host,
//     /* [in] */ IAccessibilityNodeInfoCompat* info)
// {
//     final AccessibilityNodeInfoCompat superNode = AccessibilityNodeInfoCompat.obtain(info);
//     AccessibilityDelegateCompat::OnInitializeAccessibilityNodeInfo(host, superNode);
//     copyNodeInfoNoChildren(info, superNode);
//     superNode.recycle();

//     info.setClassName(OverlappingPaneLayout.class.getName());
//     info.setSource(host);

//     final ViewParent parent = ViewCompat.getParentForAccessibility(host);
//     if (parent instanceof View) {
//         info.setParent((View) parent);
//     }

//     // This is a best-approximation of addChildrenForAccessibility()
//     // that accounts for filtering.
//     final Int32 childCount = getChildCount();
//     for (Int32 i = 0; i < childCount; i++) {
//         final View child = getChildAt(i);
//         if (child.getVisibility() == View.VISIBLE) {
//             // Force importance to "yes" since we can't read the value.
//             ViewCompat.setImportantForAccessibility(
//                     child, ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_YES);
//             info.addChild(child);
//         }
//     }
// }

// ECode OverlappingPaneLayout::AccessibilityDelegate::OnInitializeAccessibilityEvent(
//     /* [in] */ IView* host,
//     /* [in] */ IAccessibilityEvent* event)
// {
//     super.onInitializeAccessibilityEvent(host, event);

//     event.setClassName(OverlappingPaneLayout.class.getName());
// }

//================================================================
// OverlappingPaneLayout::DragHelperCallback
//================================================================
CAR_INTERFACE_IMPL(OverlappingPaneLayout::DragHelperCallback, Object, IViewDragHelperCallback)

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
    *result = ((LayoutParams*)params)->mSlideable;
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::OnViewDragStateChanged(
    /* [in] */ Int32 state)
{
    if (OverlappingPaneLayout::DEBUG) {
        Logger::D(OverlappingPaneLayout::TAG, "onViewDragStateChanged: %d", state);
    }

    Int32 dragHelper;
    mHost->mDragHelper->GetViewDragState(&dragHelper);
    if (dragHelper == IViewDragHelper::STATE_IDLE) {
        if (mHost->mSlideOffset == 0) {
            mHost->UpdateObscuredViewsVisibility(mSlideableView);
            mHost->DispatchOnPanelClosed(mSlideableView);
            mHost->mPreservedOpenState = FALSE;
        }
        else {
            mHost->DispatchOnPanelOpened(mSlideableView);
            mHost->mPreservedOpenState = TRUE;
        }
    }

    Int32 magnitude;
    if (state == IViewDragHelper::STATE_IDLE
            && mHost->mDragHelper->GetVelocityMagnitude(&magnitude), magnitude > 0
            && mHost->mIsInNestedFling) {
        mHost->mIsInNestedFling = FALSE;

        Int32 flingVelocity = !mHost->mInUpwardsPreFling ?
                -magnitude : magnitude;
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

ECode OverlappingPaneLayout::DragHelperCallback::OnViewReleased(
    /* [in] */ IView* releasedChild,
    /* [in] */ Float xvel,
    /* [in] */ Float yvel)
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
    Int32 yOffsetPx;
    mHost->mDragHelper->PredictFlingYOffset((Int32)yVelocity, &yOffsetPx);
    if (yVelocity < 0) {
        // Only perform a fling if we know the fling will fully compress the header.
        if (-yOffsetPx > mSlideOffsetPx) {
            Int32 left;
            releasedChild->GetLeft(&left);
            mHost->mDragHelper->FlingCapturedView(left, /* minTop = */ 0,
                    mSlideRange, 0x7FFFFFFF, (Int32)yVelocity);
        }
        else {
            mIsInNestedFling = FALSE;
            OnViewReleased(releasedChild, xVelocity, yVelocity);
        }
    }
    else {
        // Only perform a fling if we know the fling will expand the header as far
        // as it can possible be expanded, given the isScrollableChildUnscrolled() value.
        Boolean result;
        if (yOffsetPx + mHost->mSlideOffsetPx >= mSlideRange
                && mHost->mPanelSlideCallbacks->IsScrollableChildUnscrolled(&result), result) {
            Int32 left;
            releasedChild->GetLeft(&left);
            mHost->mDragHelper->FlingCapturedView(left, /* minTop = */ 0,
                    0x7FFFFFFF, mHost->mSlideRange, (Int32)yVelocity);
        }
        else if (yOffsetPx + mHost->mSlideOffsetPx >= mIntermediateOffset
                && mHost->mSlideOffsetPx <= mHost->mIntermediateOffset
                && mPanelSlideCallbacks->IsScrollableChildUnscrolled(&result), !result) {
            Int32 left;
            releasedChild->GetLeft(&left);
            mHost->mDragHelper->FlingCapturedView(left, /* minTop = */ 0,
                    0x7FFFFFFF, mHost->mIntermediateOffset, (Int32)yVelocity);
        }
        else {
            (Int32)mIsInNestedFling = FALSE;
            OnViewReleased(releasedChild, xVelocity, yVelocity);
        }
    }

    mHost->mInNestedPreScrollDownwards = FALSE;
    mHost->mInNestedPreScrollUpwards = FALSE;

    // Without this invalidate, some calls to flingCapturedView can have no affect.
    mHost->Invalidate();
    return NOERROR;
}

ECode OverlappingPaneLayout::DragHelperCallback::GetViewVerticalDragRange(
    /* [in] */ IView* child,
    /* [out] */ Int32* range)
{
    VALIDATE_NOT_NULL(range);
    *range = mSlideRange;
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
    AutoPtr<IViewGroupLayoutParams> lp;
    mHost->mSlideableView->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 newTop;
    Int32 previousTop = top - dy;
    Int32 topMargin;
    lp->GetTopMargin(&topMargin);
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 topBound = paddingTop + topMargin;

    Boolean isScrollableChildUnscrolled;
    Int32 bottomBound = topBound +
            (mHost->mPanelSlideCallbacks->IsScrollableChildUnscrolled(&isScrollableChildUnscrolled),
            isScrollableChildUnscrolled
            || !mHost->mIsInNestedScroll ? mHost->mSlideRange : mHost->mIntermediateOffset);
    if (previousTop > bottomBound) {
        // We were previously below the bottomBound, so loosen the bottomBound so that this
        // makes sense. This can occur after the view was directly dragged by the tabs.
        bottomBound = Math::Max(bottomBound, mSlideRange);
    }
    newTop = Math::Min(Math::Max(top, topBound), bottomBound);

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
// OverlappingPaneLayouts
//================================================================
const String OverlappingPaneLayouts::TAG("SlidingPaneLayout");
const Boolean OverlappingPaneLayouts::DEBUG = FALSE;

const Int32 OverlappingPaneLayouts::DEFAULT_OVERHANG_SIZE = 32; // dp;

const Int32 OverlappingPaneLayouts::DEFAULT_FADE_COLOR = 0xcccccccc;

const Int32 OverlappingPaneLayouts::MIN_FLING_VELOCITY = 400; // dips per second

CAR_INTERFACE_IMPL(OverlappingPaneLayouts, ViewGroup, IOverlappingPaneLayout)

OverlappingPaneLayouts::OverlappingPaneLayouts()
    : mIntermediateOffset(0)
{
    CRect::New((IRect**)&mTmpRect);
}

ECode OverlappingPaneLayouts::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode OverlappingPaneLayouts::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode OverlappingPaneLayouts::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ViewGroup::constructor(context, attrs, defStyle);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);
    mOverhangSize = (Int32)(DEFAULT_OVERHANG_SIZE * density + 0.5f);

    SetWillNotDraw(FALSE);

    assert(0 && "TODO");
    // ViewCompat.setAccessibilityDelegate(this, new AccessibilityDelegate());
    // ViewCompat.setImportantForAccessibility(this, ViewCompat.IMPORTANT_FOR_ACCESSIBILITY_YES);

    mDragHelper = ViewDragHelper::Create(this, 0.5f, new DragHelperCallback());
    mDragHelper->SetMinVelocity(MIN_FLING_VELOCITY * density);

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton(&helper);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IViewConfiguration> vc;
    helper->Get((IViewConfiguration**)&vc);
    vc->GetScaledTouchSlop(&mReleaseScrollSlop);

    return NOERROR;
}

ECode OverlappingPaneLayouts::SetIntermediatePinnedOffset(
    /* [in] */ Int32 offset)
{
    mIntermediateOffset = offset;
    return NOERROR;
}

ECode OverlappingPaneLayouts::SetCapturableView(
    /* [in] */ IView* capturableView)
{
    mCapturableView = capturableView;
    return NOERROR;
}

ECode OverlappingPaneLayouts::SetPanelSlideCallbacks(
    /* [in] */ IOverlappingPaneLayoutPanelSlideCallbacks* listener)
{
    mPanelSlideCallbacks = listener;
    return NOERROR;
}

void OverlappingPaneLayouts::DispatchOnPanelSlide(
    /* [in] */ IView* panel)
{
    mPanelSlideCallbacks->OnPanelSlide(panel, mSlideOffset);
}

void OverlappingPaneLayouts::DispatchOnPanelOpened(
    /* [in] */ IView* panel)
{
    mPanelSlideCallbacks->OnPanelOpened(panel);
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
}

void OverlappingPaneLayouts::DispatchOnPanelClosed(
    /* [in] */ IView* panel)
{
    mPanelSlideCallbacks->OnPanelClosed(panel);
    SendAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED);
}

void OverlappingPaneLayouts::UpdateObscuredViewsVisibility(
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
    if (panel != null && ViewIsOpaque(panel)) {
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

        if (child == panel) {
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

void OverlappingPaneLayouts::SetAllChildrenVisible()
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

Boolean OverlappingPaneLayouts::ViewIsOpaque(
    /* [in] */ IView* v)
{
    assert(0 && "TODO");
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

ECode OverlappingPaneLayouts::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();
    mFirstLayout = TRUE;
    return NOERROR;
}

ECode OverlappingPaneLayouts::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();
    mFirstLayout = TRUE;
    return NOERROR;
}

void OverlappingPaneLayouts::OnMeasure(
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
            return;
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
            return;
        }
    }

    Int32 layoutWidth = 0;
    Int32 maxLayoutWidth = -1;
    switch (widthMode) {
        case View::MeasureSpec::EXACTLY:
            Int32 paddingLeft, paddingRight;
            GetPaddingLeft(&paddingLeft);
            GetPaddingRight(&paddingRight);
            layoutWidth = maxLayoutWidth = widthSize - paddingLeft - paddingRight;
            break;
        case View::MeasureSpec::AT_MOST:
            Int32 paddingLeft, paddingRight;
            GetPaddingLeft(&paddingLeft);
            GetPaddingRight(&paddingRight);
            maxLayoutWidth = widthSize - paddingLeft - paddingRight;
            break;
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
        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        Int32 visibility;
        if (child->GetVisibility(&visibility), visibility == IView::GONE) {
            continue;
        }

        if (((LayoutParams*)lp)->mWeight > 0) {
            weightSum += ((LayoutParams*)lp)->mWeight;

            // If we have no height, weight is the only contributor to the final size.
            // Measure this view on the weight pass only.
            Int32 height;
            if (lp->GetHeight(&height), height == 0) continue;
        }

        Int32 topMargin, bottomMargin;
        ((IViewGroupMarginLayoutParams*)lp)->GetTopMargin(&topMargin);
        ((IViewGroupMarginLayoutParams*)lp)->GetBottomMargin(&bottomMargin);
        Int32 height;
        lp->GetHeight(&height);

        Int32 childHeightSpec;
        Int32 verticalMargin = topMargin + bottomMargin;
        if (height == IViewGroupLayoutParams::WRAP_CONTENT) {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(heightAvailable - verticalMargin,
                    View::MeasureSpec::AT_MOST);
        }
        else if (height == IViewGroupLayoutParams::MATCH_PARENT) {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(heightAvailable - verticalMargin,
                    View::MeasureSpec::EXACTLY);
        }
        else {
            childHeightSpec = View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY);
        }

        Int32 width;
        lp->GetWidth(&width);
        Int32 childWidthSpec;
        if (width == IViewGroupLayoutParams::WRAP_CONTENT) {
            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                    maxLayoutWidth, View::MeasureSpec::AT_MOST);
        }
        else if (width == IViewGroupLayoutParams::MATCH_PARENT) {
            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                    maxLayoutWidth, View::MeasureSpec::EXACTLY);
        }
        else {
            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(
                    width, View::MeasureSpec::EXACTLY);
        }

        child->Measure(childWidthSpec, childHeightSpec);
        Int32 childWidth;
        child->GetMeasuredWidth(&childWidth);
        Int32 childHeight;
        child->GetMeasuredHeight(&childHeight);

        if (widthMode == View::MeasureSpec::AT_MOST && childWidth > layoutWidth) {
            layoutWidth = Math::Min(childWidth, maxLayoutWidth);
        }

        heightRemaining -= childHeight;
        canSlide |= ((LayoutParams*)lp)->mSlideable = heightRemaining < 0;
        if (((LayoutParams*)lp)->mSlideable) {
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

            AutoPtr<IViewGroupLayoutParams> lp;
            child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

            if (child->GetVisibility(&visibility), visibility == IView::GONE) {
                continue;
            }

            Int32 height;
            lp->GetHeight(&height);
            Int32 width;
            lp->GetWidth(&width);
            Boolean skippedFirstPass = height == 0 && ((LayoutParams*)lp)->mWeight > 0;
            Int32 childMeasuredHeight;
            Int32 measuredHeight = skippedFirstPass ? 0 :
                    child->GetMeasuredHeight(&childMeasuredHeight), childMeasuredHeight;
            if (canSlide && child != mSlideableView) {
                if (height < 0 && (measuredHeight > fixedPanelHeightLimit
                        || ((LayoutParams*)lp)->mWeight > 0)) {
                    // Fixed panels in a sliding configuration should
                    // be clamped to the fixed panel limit.
                    Int32 childWidthSpec;
                    if (skippedFirstPass) {
                        // Do initial width measurement if we skipped measuring this view
                        // the first time around.
                        if (width == IViewGroupLayoutParams::WRAP_CONTENT) {
                            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                    View::MeasureSpec::AT_MOST);
                        }
                        else if (height == IViewGroupLayoutParams::MATCH_PARENT) {
                            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                    View::MeasureSpec::EXACTLY);
                        }
                        else {
                            childWidthSpec = View::MeasureSpec::MakeMeasureSpec(width,
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
            else if (width > 0) {
                Int32 childWidthSpec;
                if (height == 0) {
                    // This was skipped the first time; figure out a real width spec.
                    if (width == IViewGroupLayoutParams::WRAP_CONTENT) {
                        childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                View::MeasureSpec::AT_MOST);
                    }
                    else if (width == IViewGroupLayoutParams::MATCH_PARENT) {
                        childWidthSpec = View::MeasureSpec::MakeMeasureSpec(maxLayoutWidth,
                                View::MeasureSpec::EXACTLY);
                    }
                    else {
                        childWidthSpec = View::MeasureSpec::MakeMeasureSpec(width,
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
                    Int32 topMargin, bottomMargin;
                    ((IViewGroupMarginLayoutParams*)lp)->GetTopMargin(&topMargin);
                    ((IViewGroupMarginLayoutParams*)lp)->GetBottomMargin(&bottomMargin);
                    Int32 verticalMargin = topMargin + bottomMargin;
                    Int32 newHeight = heightAvailable - verticalMargin;
                    Int32 childHeightSpec = View::MeasureSpec::MakeMeasureSpec(
                            newHeight, View::MeasureSpec::EXACTLY);
                    if (measuredHeight != newHeight) {
                        child->Measure(childWidthSpec, childHeightSpec);
                    }
                }
                else {
                    // Distribute the extra width proportionally similar to LinearLayout
                    Int32 heightToDistribute = Math::Max(0, heightRemaining);
                    Int32 addedHeight = (Int32)(weight * heightToDistribute / weightSum);
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

    Int32 dragHelper;
    mDragHelper->GetViewDragState(&dragHelper);
    if (dragHelper != IViewDragHelper::STATE_IDLE && !canSlide) {
        // Cancel scrolling in progress, it's no longer relevant.
        mDragHelper->Abort();
    }
}

ECode OverlappingPaneLayouts::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    mDragHelper->SetEdgeTrackingEnabled(IViewDragHelper::EDGE_TOP);

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

        AutoPtr<IViewGroupLayoutParams> lp;
        child->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        Int32 childHeight;
        child->GetMeasuredHeight(&childHeight);

        if (((LayoutParams*)lp)->mSlideable) {
            Int32 topMargin, bottomMargin;
            ((IViewGroupMarginLayoutParams*)lp)->GetTopMargin(&topMargin);
            ((IViewGroupMarginLayoutParams*)lp)->GetBottomMargin(&bottomMargin);
            Int32 margin = topMargin + bottomMargin;
            Int32 range = Elastos::Core::Math::Min(nextYStart,
                    height - paddingBottom - mOverhangSize) - yStart - margin;
            mSlideRange = range;
            Int32 lpMargin = topMargin;
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

void OverlappingPaneLayouts::OnSizeChanged(
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
}

ECode OverlappingPaneLayouts::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    ViewGroup::RequestChildFocus(child, focused);
    Boolean isInTouchMode;
    if (isInTouchMode(&isInTouchMode), !isInTouchMode && !mCanSlide) {
        mPreservedOpenState = child == mSlideableView;
    }

    return NOERROR;
}

ECode OverlappingPaneLayouts::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 action;
    assert(0 && "TODO");
    // MotionEventCompat.getActionMasked(ev);

    // Preserve the open state based on the last view that was touched.
    Int32 count;
    if (!mCanSlide && action == IMotionEvent::ACTION_DOWN
            && GetChildCount(&count), count > 1) {
        // After the first things will be slideable.
        AutoPtr<IView> secondChild;
        GetChildAt(1, (IView**)&secondChild);
        if (secondChild != NULL) {
            Float x, y;
            ev->GetX(&x);
            ev->GetY(&y);
            Int32 isViewUnder;
            mDragHelper->IsViewUnder(secondChild,
                    (Int32)x, (Int32)y, &isViewUnder);
            mPreservedOpenState = !isViewUnder;
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

        case MotionEvent.ACTION_MOVE: {
            Float x;
            ev->GetX(&x);
            Float y;
            ev->GetY(&y);
            Float adx = Math::Abs(x - mInitialMotionX);
            Float ady = Math::Abs(y - mInitialMotionY);
            Int32 slop;
            mDragHelper->GetTouchSlop(&slop);
            if (ady > slop && adx > ady || !IsCapturableViewUnder((Int32) x, (Int32) y)) {
                if (!mIsInNestedScroll) {
                    mDragHelper->Cancel();
                }
                mIsUnableToDrag = TRUE;
                *res = FALSE;
                return NOERROR;
            }
        }
    }

    Boolean interceptForDrag;
    mDragHelper->ShouldInterceptTouchEvent(ev, &interceptForDrag);

    *res = interceptForDrag;
    return NOERROR;
}

ECode OverlappingPaneLayouts::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (!mCanSlide) {
        return ViewGroup::OnTouchEvent(ev);
    }

    mDragHelper->ProcessTouchEvent(ev);

    Int32 action;
    ev->GetAction(&action);
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

Boolean OverlappingPaneLayouts::ClosePane(
    /* [in] */ IView* pane,
    /* [in] */ Int32 initialVelocity)
{
    if (mFirstLayout || SmoothSlideTo(0.f, initialVelocity)) {
        mPreservedOpenState = FALSE;
        return TRUE;
    }
    return FALSE;
}

Boolean OverlappingPaneLayouts::OpenPane(
    /* [in] */ IView* pane,
    /* [in] */ Int32 initialVelocity)
{
    if (mFirstLayout || SmoothSlideTo(1.f, initialVelocity)) {
        mPreservedOpenState = TRUE;
        return TRUE;
    }
    return FALSE;
}

void OverlappingPaneLayouts::UpdateSlideOffset(
    /* [in] */ Int32 offsetPx)
{
    mSlideOffsetPx = offsetPx;
    mSlideOffset = (Float) mSlideOffsetPx / mSlideRange;
}

ECode OverlappingPaneLayouts::OpenPane(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = OpenPane(mSlideableView, 0);
    return NOERROR;
}

ECode OverlappingPaneLayouts::ClosePane(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ClosePane(mSlideableView, 0);
    return NOERROR;
}

ECode OverlappingPaneLayouts::IsOpen(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !mCanSlide || mSlideOffset > 0;
    return NOERROR;
}

ECode OverlappingPaneLayouts::IsSlideable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCanSlide;
    return NOERROR;
}

void OverlappingPaneLayouts::OnPanelDragged(
    /* [in] */ Int32 newTop)
{
    if (mSlideableView == NULL) {
        // This can happen if we're aborting motion during layout because everything now fits.
        mSlideOffset = 0;
        return;
    }
    AutoPtr<IViewGroupLayoutParams> lp;
    mSlideableView->GetLayoutParams((IViewGroupLayoutParams**)&lp);

    Int32 lpMargin;
    ((IViewGroupMarginLayoutParams*)lp)->GetTopMargin(&lpMargin);
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 topBound = paddingTop + lpMargin;

    UpdateSlideOffset(newTop - topBound);

    DispatchOnPanelSlide(mSlideableView);
}

Boolean OverlappingPaneLayouts::DrawChild(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IView* child,
    /* [in] */ Int64 drawingTime)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    final LayoutParams lp = (LayoutParams) child.getLayoutParams();
    Boolean result;
    Int32 save;
    canvas->Save(ICanvas::CLIP_SAVE_FLAG, &save);

    if (mCanSlide && !((LayoutParams*)lp)->mSlideable && mSlideableView != NULL) {
        // Clip against the slider; no sense drawing what will immediately be covered.
        canvas->GetClipBounds(mTmpRect);

        Int32 bottom;
        mTmpRect->GetBottom(&bottom);
        Int32 top;
        mSlideableView->GetTop(&top);
        mTmpRect->SetBottom(Math::Min(bottom, top));
        canvas->ClipRect(mTmpRect);
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

Boolean OverlappingPaneLayouts::SmoothSlideTo(
    /* [in] */ Float slideOffset,
    /* [in] */ Int32 velocity)
{
    if (!mCanSlide) {
        // Nothing to do.
        return FALSE;
    }

    AutoPtr<IViewGroupLayoutParams> lp;
    mSlideableView->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    final LayoutParams lp = (LayoutParams) mSlideableView.getLayoutParams();

    Int32 y;
    Int32 lpMargin;
    ((IViewGroupMarginLayoutParams*)lp)->GetTopMargin(&lpMargin);
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 topBound = paddingTop + topMargin;
    y = (Int32) (topBound + slideOffset * mSlideRange);

    Int32 left;
    mSlideableView->GetLeft(&left);
    Boolean result;
    if (mDragHelper->SmoothSlideViewTo(mSlideableView, left, y, &result), result) {
        SetAllChildrenVisible();
        assert(0 && "TODO");
        // ViewCompat.postInvalidateOnAnimation(this);
        return TRUE;
    }
    return FALSE;
}

ECode OverlappingPaneLayouts::ComputeScroll()
{
    Boolean result;
    if (mDragHelper->ContinueSettling(
            /* deferCallbacks = */ FALSE, &result), result) {
        if (!mCanSlide) {
            mDragHelper->Abort();
            return NOERROR;
        }

        assert(0 && "TODO");
        // ViewCompat.postInvalidateOnAnimation(this);
    }
    return NOERROR;
}

Boolean OverlappingPaneLayouts::IsCapturableViewUnder(
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
    Int32 screenX = parentLocation[0] + x;
    Int32 screenY = parentLocation[1] + y;

    Int32 width, height;
    capturableView->GetWidth(&width);
    capturableView->GetHeight(&height);
    return screenX >= viewLocation[0]
            && screenX < viewLocation[0] + width
            && screenY >= viewLocation[1]
            && screenY < viewLocation[1] + height;
}

ECode OverlappingPaneLayouts::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<LayoutParams> lp = new LayoutParams(this);
    lp->constructor();
    *params = (IViewGroupLayoutParams*)lp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

AutoPtr<IViewGroupLayoutParams> OverlappingPaneLayouts::GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p)
{
    AutoPtr<LayoutParams> lp;
    if (IViewGroupMarginLayoutParams::Probe(p) != NULL){
        lp = new LayoutParams(this);
        lp->constructor(IViewGroupMarginLayoutParams::Probe(p));
    }
    else {
        lp = new LayoutParams(this);
        lp->constructor(p);
    }
    *params = (IViewGroupLayoutParams*)lp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

Boolean OverlappingPaneLayouts::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p)
{
    return IOverlappingPaneLayoutLayoutParams::Probe(p) != NULL
            && ViewGroup::CheckLayoutParams(p);
}

ECode OverlappingPaneLayouts::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** params)
{
    VALIDATE_NOT_NULL(params);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<LayoutParams> lp = new LayoutParams(this);
    lp->constructor(context, attrs);
    *params = (IViewGroupLayoutParams*)lp;
    REFCOUNT_ADD(*params);
    return NOERROR;
}

AutoPtr<IParcelable> OverlappingPaneLayouts::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = ViewGroup::OnSaveInstanceState();

    AutoPtr<SavedState> ss = new SavedState(this);
    ss->constructor(superState);
    Boolean isSlideable;
    IsSlideable(&isSlideable);
    Boolean isOpen;
    ss->mIsOpen = isSlideable ? IsOpen(&isOpen), isOpen : mPreservedOpenState;

    return (IParcelable*)ss;
}

void OverlappingPaneLayouts::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<SavedState> ss = (SavedState*)state;
    AutoPtr<IParcelable> p;
    ss->GetSuperState((IParcelable**)&p);
    ViewGroup::OnRestoreInstanceState(p);

    if (ss->mIsOpen) {
        OpenPane();
    }
    else {
        ClosePane();
    }
    mPreservedOpenState = ss->mIsOpen;
}

ECode OverlappingPaneLayouts::OnStartNestedScroll(
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

ECode OverlappingPaneLayouts::OnNestedPreScroll(
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

ECode OverlappingPaneLayouts::OnNestedPreFling(
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
            && mPanelSlideCallbacks->IsScrollableChildUnscrolled(&result), result)) {
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

ECode OverlappingPaneLayouts::OnNestedScroll(
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

ECode OverlappingPaneLayouts::OnStopNestedScroll(
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

