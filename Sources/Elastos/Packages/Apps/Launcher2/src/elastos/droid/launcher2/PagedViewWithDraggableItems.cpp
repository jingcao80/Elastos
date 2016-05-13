
#include "elastos/droid/launcher2/PagedViewWithDraggableItems.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Math.h>

using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL_2(PagedViewWithDraggableItems, PagedView, IViewOnLongClickListener, IViewOnTouchListener);

PagedViewWithDraggableItems::PagedViewWithDraggableItems()
    : mIsDragging(FALSE)
    , mIsDragEnabled(FALSE)
    , mDragSlopeThreshold(0.0f)
{
}

ECode PagedViewWithDraggableItems::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PagedViewWithDraggableItems::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode PagedViewWithDraggableItems::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    PagedView::constructor(context, attrs, defStyle);
    mLauncher = ILauncher::Probe(context);
    return NOERROR;
}

Boolean PagedViewWithDraggableItems::BeginDragging(
    /* [in] */ IView* v)
{
    Boolean wasDragging = mIsDragging;
    mIsDragging = TRUE;
    return !wasDragging;
}

void PagedViewWithDraggableItems::CancelDragging()
{
    mIsDragging = FALSE;
    mLastTouchedItem = NULL;
    mIsDragEnabled = FALSE;
}

void PagedViewWithDraggableItems::HandleTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    ev->GetAction(&action);
    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN:
            CancelDragging();
            mIsDragEnabled = TRUE;
            break;
        case IMotionEvent::ACTION_MOVE:
            if (mTouchState != TOUCH_STATE_SCROLLING && !mIsDragging && mIsDragEnabled) {
                DetermineDraggingStart(ev);
            }
            break;
    }
}

ECode PagedViewWithDraggableItems::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    HandleTouchEvent(ev);
    return PagedView::OnInterceptTouchEvent(ev, result);
}

ECode PagedViewWithDraggableItems::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    HandleTouchEvent(ev);
    return PagedView::OnTouchEvent(ev, result);
}

ECode PagedViewWithDraggableItems::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    mLastTouchedItem = v;
    mIsDragEnabled = TRUE;
    *result = FALSE;
    return NOERROR;
}

ECode PagedViewWithDraggableItems::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Return early if this is not initiated from a touch
    Boolean res;
    v->IsInTouchMode(&res);
    if (!res) {
        *result = FALSE;
        return NOERROR;
    }
    // Return early if we are still animating the pages
    if (mNextPage != INVALID_PAGE) {
        *result = FALSE;
        return NOERROR;
    }
    // When we have exited all apps or are in transition, disregard long clicks
    mLauncher->IsAllAppsVisible(&res);
    if (!res) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IWorkspace> ws;
    mLauncher->GetWorkspace((IWorkspace**)&ws);
    ws->IsSwitchingState(&res);
    if (res) {
        *result = FALSE;
        return NOERROR;
    }
    // Return if global dragging is not enabled
    mLauncher->IsDraggingEnabled(&res);
    if (!res) {
        *result = FALSE;
        return NOERROR;
    }

    *result = BeginDragging(v);
    return NOERROR;
}

ECode PagedViewWithDraggableItems::DetermineScrollingStart(
    /* [in] */ IMotionEvent* ev)
{
    if (!mIsDragging) {
        return PagedView::DetermineScrollingStart(ev);
    }
    return NOERROR;
}

void PagedViewWithDraggableItems::DetermineDraggingStart(
    /* [in] */ IMotionEvent* ev)
{
    /*
     * Locally do absolute value. mLastMotionX is set to the y value
     * of the down event.
     */
    Int32 pointerIndex;
    ev->FindPointerIndex(mActivePointerId, &pointerIndex);
    Float x;
    ev->GetX(pointerIndex, &x);
    Float y;
    ev->GetY(pointerIndex, &y);
    Int32 xDiff = (Int32)(Elastos::Core::Math::Abs(x - mLastMotionX));
    Int32 yDiff = (Int32)(Elastos::Core::Math::Abs(y - mLastMotionY));

    const Int32 touchSlop = mTouchSlop;
    Boolean yMoved = yDiff > touchSlop;
    Boolean isUpwardMotion = (yDiff / (Float) xDiff) > mDragSlopeThreshold;

    if (isUpwardMotion && yMoved && mLastTouchedItem != NULL) {
        // Drag if the user moved far enough along the Y axis
        BeginDragging(mLastTouchedItem);

        // Cancel any pending long press
        if (mAllowLongPress) {
            mAllowLongPress = FALSE;
            // Try canceling the long press. It could also have been scheduled
            // by a distant descendant, so use the mAllowLongPress flag to block
            // everything
            AutoPtr<IView> currentPage = GetPageAt(mCurrentPage);
            if (currentPage != NULL) {
                currentPage->CancelLongPress();
            }
        }
    }
}

ECode PagedViewWithDraggableItems::SetDragSlopeThreshold(
    /* [in] */ Float dragSlopeThreshold)
{
    mDragSlopeThreshold = dragSlopeThreshold;
    return NOERROR;
}

ECode PagedViewWithDraggableItems::OnDetachedFromWindow()
{
    CancelDragging();
    PagedView::OnDetachedFromWindow();
    return NOERROR;
}

ECode PagedViewWithDraggableItems::OnPageBeginMoving()
{
    return ShowScrollingIndicator(FALSE);
}

ECode PagedViewWithDraggableItems::OnPageEndMoving()
{
    return HideScrollingIndicator(FALSE);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos