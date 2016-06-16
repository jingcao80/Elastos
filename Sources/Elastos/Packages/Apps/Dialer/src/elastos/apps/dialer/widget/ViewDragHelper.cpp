
#include "widget/ViewDragHelper.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Widget {

//================================================================
// ViewDragHelper::Callback
//================================================================
CAR_INTERFACE_IMPL(ViewDragHelper::Callback, Object, IViewDragHelperCallback);

ECode ViewDragHelper::Callback::OnViewDragStateChanged(
    /* [in] */ Int32 state)
{
    return NOERROR;
}

ECode ViewDragHelper::Callback::OnViewPositionChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    return NOERROR;
}

ECode ViewDragHelper::Callback::OnViewCaptured(
    /* [in] */ IView* capturedChild,
    /* [in] */ Int32 activePointerId)
{
    return NOERROR;
}

ECode ViewDragHelper::Callback::OnViewReleased(
    /* [in] */ IView* releasedChild,
    /* [in] */ Float xvel,
    /* [in] */ Float yvel)
{
    return NOERROR;
}

ECode ViewDragHelper::Callback::OnViewFling(
    /* [in] */ IView* releasedChild,
    /* [in] */ Float xvel,
    /* [in] */ Float yvel)
{
    return NOERROR;
}

ECode ViewDragHelper::Callback::OnEdgeTouched(
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 pointerId)
{
    return NOERROR;
}

ECode ViewDragHelper::Callback::OnEdgeLock(
    /* [in] */ Int32 edgeFlags,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode ViewDragHelper::Callback::OnEdgeDragStarted(
    /* [in] */ Int32 edgeFlags,
    /* [in] */ Int32 pointerId)
{
    return NOERROR;
}

ECode ViewDragHelper::Callback::GetOrderedChildIndex(
    /* [in] */ Int32 index,
    /* [out] */ Int32* childIndex)
{
    VALUE_NOT_NULL(childIndex);
    *childIndex = index;
    return NOERROR;
}

ECode ViewDragHelper::Callback::GetViewHorizontalDragRange(
    /* [in] */ IView* child,
    /* [out] */ Int32* result)
{
    VALUE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode ViewDragHelper::Callback::GetViewVerticalDragRange(
    /* [in] */ IView* child,
    /* [out] */ Int32* result)
{
    VALUE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode ViewDragHelper::Callback::ClampViewPositionHorizontal(
    /* [in] */ IView* child,
    /* [in] */ Int32 left,
    /* [in] */ Int32 dx,
    /* [out] */ Int32* position)
{
    VALUE_NOT_NULL(position);
    *position = 0;
    return NOERROR;
}

ECode ViewDragHelper::Callback::ClampViewPositionVertical(
    /* [in] */ IView* child,
    /* [in] */ Int32 top,
    /* [in] */ Int32 dy,
    /* [out] */ Int32* position)
{
    VALUE_NOT_NULL(position);
    *position = 0;
    return NOERROR;
}

//================================================================
// ViewDragHelper::SetIdleRunnable
//================================================================
ViewDragHelper::SetIdleRunnable::SetIdleRunnable(
    /* [in] */ ViewDragHelper* host)
    : mHost(host)
{}

ECode ViewDragHelper::SetIdleRunnable::Run()
{
    mHost->SetDragState(STATE_IDLE);
    return NOERROR;
}

//================================================================
// ViewDragHelper
//================================================================
const String TAG("ViewDragHelper");

const Int32 EDGE_SIZE = 20; // dp

const Int32 BASE_SETTLE_DURATION = 256; // ms
const Int32 MAX_SETTLE_DURATION = 600; // ms

CAR_INTERFACE_IMPL(ViewDragHelper, Object, IViewDragHelper);

AutoPtr<IViewDragHelper> ViewDragHelper::Create(
    /* [in] */ IViewGroup* forParent,
    /* [in] */ IViewDragHelperCallback* cb)
{
    AutoPtr<IContext> context;
    forParent->GetContext((IContext**)&context);
    AutoPtr<ViewDragHelper> helper = new ViewDragHelper(context, forParent, cb);
    return (IViewDragHelper*)helper;
}

AutoPtr<IViewDragHelper> ViewDragHelper::Create(
    /* [in] */ IViewGroup* forParent,
    /* [in] */ Float sensitivity,
    /* [in] */ IViewDragHelperCallback* cb)
{
    AutoPtr<IViewDragHelper> helper = Create(forParent, cb);
    ((ViewDragHelper*)helper)->mTouchSlop =
            (Int32) (((ViewDragHelper*)helper)->mTouchSlop * (1 / sensitivity));
    return helper;
}

ViewDragHelper::ViewDragHelper(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* forParent,
    /* [in] */ IViewDragHelperCallback* cb)
    : mActivePointerId(IViewDragHelper::INVALID_POINTER)
{
    // if (forParent == NULL) {
    //     throw new IllegalArgumentException("Parent view may not be null");
    // }
    // if (cb == NULL) {
    //     throw new IllegalArgumentException("Callback may not be null");
    // }

    mSetIdleRunnable = new SetIdleRunnable(this);

    mParentView = forParent;
    mCallback = cb;

    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton(&helper);
    AutoPtr<IViewConfiguration> vc;
    helper->Get(context, (IViewConfigurationHelper**)&vc);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density;
    dm->GetDensity(&density);
    mEdgeSize = (Int32) (EDGE_SIZE * density + 0.5f);

    vc->GetScaledTouchSlop(&mTouchSlop);
    vc->GetScaledMaximumFlingVelocity(&mMaxVelocity);
    vc->GetScaledMinimumFlingVelocity(&mMinVelocity);
    CScroller::New(context, (IScroller**)&mScroller);
}

ECode ViewDragHelper::SetMinVelocity(
        /* [in] */ Float minVel)
{
    mMinVelocity = minVel;
    return NOERROR;
}

ECode ViewDragHelper::GetMinVelocity(
    /* [out] */ Float* minVel)
{
    VALUE_NOT_NULL(minVel);
    *minVel = mMinVelocity;
    return NOERROR;
}

ECode ViewDragHelper::GetViewDragState(
    /* [out] */ Int32* state)
{
    VALUE_NOT_NULL(state);
    *state = mDragState;
    return NOERROR;
}

ECode ViewDragHelper::SetEdgeTrackingEnabled(
    /* [in] */ Int32 edgeFlags)
{
    mTrackingEdges = edgeFlags;
    return NOERROR;
}

ECode ViewDragHelper::GetEdgeSize(
    /* [out] */ Int32* size)
{
    VALUE_NOT_NULL(size);
    *size = mEdgeSize;
    return NOERROR;
}

ECode ViewDragHelper::CaptureChildView(
    /* [in] */ IView* childView,
    /* [in] */ Int32 activePointerId)
{
    AutoPtr<IView> parent;
    childView->GetParent((IView**)&parent);
    if (parent != mParentView) {
        String str;
        mParentView->ToString(&str);
        Logger::E(TAG, "captureChildView: parameter must be a descendant "
            "of the ViewDragHelper's tracked parent view (%s)", str.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("captureChildView: parameter must be a descendant " +
        //         "of the ViewDragHelper's tracked parent view (" + mParentView + ")");
    }

    mCapturedView = childView;
    mActivePointerId = activePointerId;
    mCallback->OnViewCaptured(childView, activePointerId);
    SetDragState(STATE_DRAGGING);
    return NOERROR;
}

ECode ViewDragHelper::GetCapturedView(
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);
    *view = mCapturedView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode ViewDragHelper::GetActivePointerId(
    /* [out] */ Int32* id)
{
    VALUE_NOT_NULL(id);
    *id = mActivePointerId;
    return NOERROR;
}

ECode ViewDragHelper::GetTouchSlop(
    /* [out] */ Int32* touchSlop)
{
    VALUE_NOT_NULL(touchSlop);
    *touchSlop = mTouchSlop;
    return NOERROR;
}

ECode ViewDragHelper::Cancel()
{
    mActivePointerId = IViewDragHelper::INVALID_POINTER;
    ClearMotionHistory();

    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
    return NOERROR;
}

ECode ViewDragHelper::Abort()
{
    Cancel();
    if (mDragState == IViewDragHelper::STATE_SETTLING) {
        Int32 oldX;
        mScroller->GetCurrX(&oldX);
        Int32 oldY;
        mScroller->GetCurrY(&oldY);
        mScroller->AbortAnimation();
        Int32 newX;
        mScroller->GetCurrX(&newX);
        Int32 newY;
        mScroller->GetCurrY(&newY);
        mCallback->OnViewPositionChanged(mCapturedView, newX, newY, newX - oldX, newY - oldY);
    }
    SetDragState(STATE_IDLE);
    return NOERROR;
}

ECode ViewDragHelper::SmoothSlideViewTo(
    /* [in] */ IView* child,
    /* [in] */ Int32 constLeft,
    /* [in] */ Int32 constTop,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    mCapturedView = child;
    mActivePointerId = IViewDragHelper::INVALID_POINTER;

    *result = ForceSettleCapturedViewAt(finalLeft, finalTop, 0, 0);
    return NOERROR;
}

ECode ViewDragHelper::SettleCapturedViewAt(
    /* [in] */ Int32 constLeft,
    /* [in] */ Int32 constTop,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    if (!mReleaseInProgress) {
        Logger::E(TAG, "Cannot settleCapturedViewAt outside of a call to "
                 "Callback#onViewReleased");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot settleCapturedViewAt outside of a call to " +
        //         "Callback#onViewReleased");
    }

    assert(0 && "TODO");
    // *result = ForceSettleCapturedViewAt(finalLeft, finalTop,
    //         (int) VelocityTrackerCompat.getXVelocity(mVelocityTracker, mActivePointerId),
    //         (int) VelocityTrackerCompat.getYVelocity(mVelocityTracker, mActivePointerId));
    return NOERROR;
}

Boolean ViewDragHelper::ForceSettleCapturedViewAt(
    /* [in] */ Int32 constLeft,
    /* [in] */ Int32 constTop,
    /* [in] */ Int32 xvel,
    /* [in] */ Int32 yvel)
{
    Int32 startLeft;
    mCapturedView->GetLeft(&startLeft);
    Int32 startTop;
    mCapturedView->GetTop(&startTop);
    Int32 dx = finalLeft - startLeft;
    Int32 dy = finalTop - startTop;

    if (dx == 0 && dy == 0) {
        // Nothing to do. Send callbacks, be done.
        mScroller->AbortAnimation();
        SetDragState(STATE_IDLE);
        return FALSE;
    }

    Int32 duration = ComputeSettleDuration(mCapturedView, dx, dy, xvel, yvel);
    mScroller->StartScroll(startLeft, startTop, dx, dy, duration);
    mFinalScrollY = startTop + dy;

    SetDragState(IViewDragHelper::STATE_SETTLING);
    return TRUE;
}

Int32 ViewDragHelper::ComputeSettleDuration(
    /* [in] */ IView* child,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ Int32 xvel,
    /* [in] */ Int32 yvel)
{
    xvel = ClampMag(xvel, (int) mMinVelocity, (int) mMaxVelocity);
    yvel = ClampMag(yvel, (int) mMinVelocity, (int) mMaxVelocity);
    Int32 absDx = Math::Abs(dx);
    Int32 absDy = Math::Abs(dy);
    Int32 absXVel = Math::Abs(xvel);
    Int32 absYVel = Math::Abs(yvel);
    Int32 addedVel = absXVel + absYVel;
    Int32 addedDistance = absDx + absDy;

    Float xweight = xvel != 0 ? (Float) absXVel / addedVel :
            (Float) absDx / addedDistance;
    Float yweight = yvel != 0 ? (Float) absYVel / addedVel :
            (Float) absDy / addedDistance;

    Int32 hDragRange, vDragRange;
    mCallback->GetViewHorizontalDragRange(child, &hDragRange);
    mCallback->GetViewVerticalDragRange(child, &vDragRange);
    Int32 xduration = ComputeAxisDuration(dx, xvel, hDragRange);
    Int32 yduration = ComputeAxisDuration(dy, yvel, vDragRange);

    return (Int32) (xduration * xweight + yduration * yweight);
}

Int32 ViewDragHelper::ComputeAxisDuration(
    /* [in] */ Int32 delta,
    /* [in] */ Int32 velocity,
    /* [in] */ Int32 motionRange)
{
    if (delta == 0) {
        return 0;
    }

    Int32 width;
    mParentView->GetWidth(&width);
    Int32 halfWidth = width / 2;
    Float distanceRatio = Math::Min(1f, (Float) Math::Abs(delta) / width);
    Float distance = halfWidth + halfWidth *
            DistanceInfluenceForSnapDuration(distanceRatio);

    Int32 duration;
    velocity = Math::Abs(velocity);
    if (velocity > 0) {
        duration = 4 * Math::Round(1000 * Math::Abs(distance / velocity));
    }
    else {
        Float range = (Float) Math::Abs(delta) / motionRange;
        duration = (Int32) ((range + 1) * BASE_SETTLE_DURATION);
    }
    return Math::Min(duration, MAX_SETTLE_DURATION);
}

Int32 ViewDragHelper::ClampMag(
    /* [in] */ Int32 value,
    /* [in] */ Int32 absMin,
    /* [in] */ Int32 absMax)
{
    Int32 absValue = Math::Abs(value);
    if (absValue < absMin) return 0;
    if (absValue > absMax) return value > 0 ? absMax : -absMax;
    return value;
}

Float ViewDragHelper::ClampMag(
    /* [in] */ Float value,
    /* [in] */ Float absMin,
    /* [in] */ Float absMax)
{
    Float absValue = Math::Abs(value);
    if (absValue < absMin) return 0;
    if (absValue > absMax) return value > 0 ? absMax : -absMax;
    return value;
}

Float ViewDragHelper::DistanceInfluenceForSnapDuration(
    /* [in] */ Float f)
{
    f -= 0.5f; // center the values about 0.
    f *= 0.3f * Math::PI / 2.0f;
    return (Float) Math::Sin(f);
}

ECode ViewDragHelper::FlingCapturedView(
    /* [in] */ Int32 minLeft,
    /* [in] */ Int32 minTop,
    /* [in] */ Int32 maxLeft,
    /* [in] */ Int32 maxTop)
{
    if (!mReleaseInProgress) {
        Logger::E(TAG, "Cannot flingCapturedView outside of a call to "
                "Callback#onViewReleased");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot flingCapturedView outside of a call to " +
        //         "Callback#onViewReleased");
    }

    Int32 yVelocity;
    assert(0 && "TODO");
    // (Int32) VelocityTrackerCompat
    //         .getYVelocity(mVelocityTracker, mActivePointerId);
    Int32 left, top;
    mCapturedView->GetLeft(&left);
    mCapturedView->GetTop(&top);
    // mScroller.fling(left, top,
    //         (int) VelocityTrackerCompat.getXVelocity(mVelocityTracker, mActivePointerId),
    //         yVelocity, minLeft, maxLeft, minTop, maxTop);
    mFinalScrollY = yVelocity < 0 ? minTop : maxTop;

    SetDragState(IViewDragHelper::STATE_SETTLING);
    return NOERROR;
}

ECode ViewDragHelper::FlingCapturedView(
    /* [in] */ Int32 minLeft,
    /* [in] */ Int32 minTop,
    /* [in] */ Int32 maxLeft,
    /* [in] */ Int32 maxTop,
    /* [in] */ Int32 yvel)
{
    if (!mReleaseInProgress) {
        Logger::E(TAG, "Cannot flingCapturedView outside of a call to "
                "Callback#onViewReleased");
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException("Cannot flingCapturedView outside of a call to " +
        //         "Callback#onViewReleased");
    }
    mScroller->AbortAnimation();
    Int32 left, top;
    mCapturedView->GetLeft(&left);
    mCapturedView->GetTop(&top);
    mScroller->Fling(left, top, 0, yvel,
            0x80000000, 0x7FFFFFFF, 0x80000000, 0x7FFFFFFF);
    mFinalScrollY = yvel < 0 ? minTop : maxTop;

    SetDragState(IViewDragHelper::STATE_SETTLING);
    return NOERROR;
}

ECode ViewDragHelper::PredictFlingYOffset(
    /* [in] */ Int32 yvel,
    /* [out] */ Int32* yOffset)
{
    VALUE_NOT_NULL(yOffset);
    mScroller->AbortAnimation();
    mScroller->Fling(0, 0, 0, yvel, 0x80000000, 0x7FFFFFFF, 0x80000000,
            0x7FFFFFFF);
    Int32 finalY;
    mScroller->GetFinalY(&finalY);
    mScroller->AbortAnimation();
    *yOffset = finalY;
    return NOERROR;
}

ECode ViewDragHelper::ContinueSettling(
    /* [in] */ Boolean deferCallbacks,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    if (mDragState == IViewDragHelper::STATE_SETTLING) {
        Boolean keepGoing;
        mScroller->ComputeScrollOffset(&keepGoing);
        Int32 y;
        mScroller->GetCurrY(&y);

        // Since Scroller's getFinalY() can't be properly set (b/17704016), we need to
        // perform clamping of mScroller.getCurrY() here.
        Int32 top;
        mCapturedView->GetTop(&top);
        if (y - top > 0) {
            y = Math::Min(y, mFinalScrollY);
        }
        else {
            y = Math::Max(y, mFinalScrollY);
        }
        Int32 dy = y - top;

        if (dy != 0) {
            mCapturedView->OffsetTopAndBottom(dy);
            mCallback->OnViewPositionChanged(mCapturedView, 0, y, 0, dy);
        }

        if (keepGoing && y == mFinalScrollY) {
            // Close enough. The interpolator/scroller might think we're still moving
            // but the user sure doesn't.
            mScroller->AbortAnimation();
            mScroller->IsFinished(&keepGoing);
        }

        if (!keepGoing) {
            if (deferCallbacks) {
                mParentView->Post(mSetIdleRunnable);
            }
            else {
                SetDragState(IViewDragHelper::STATE_IDLE);
            }
        }
    }

    *result = mDragState == IViewDragHelper::STATE_SETTLING;
    return NOERROR;
}

ECode ViewDragHelper::ProcessNestedFling(
    /* [in] */ IView* target,
    /* [in] */ Int32 yvel)
{
    mCapturedView = target;
    DispatchViewFling(0, yvel);
    return NOERROR;
}

ECode ViewDragHelper::GetVelocityMagnitude(
    /* [out] */ Int32* magnitude)
{
    VALUE_NOT_NULL(magnitude);
    // Use Math.abs() to ensure this always returns an absolute value, even if the
    // ScrollerCompat implementation changes.
    Float velocity;
    mScroller->GetCurrVelocity(&velocity);
    *magnitude =  (Int32) Math::Abs(velocity);
    return NOERROR;
}

void ViewDragHelper::DispatchViewFling(
    /* [in] */ Float xvel,
    /* [in] */ Float yvel)
{
    mReleaseInProgress = TRUE;
    mCallback->OnViewFling(mCapturedView, xvel, yvel);
    mReleaseInProgress = FALSE;

    if (mDragState == IViewDragHelper::STATE_DRAGGING) {
        // onViewReleased didn't call a method that would have changed this. Go idle.
        SetDragState(IViewDragHelper::STATE_IDLE);
    }
}

void ViewDragHelper::DispatchViewReleased(
    /* [in] */ Float xvel,
    /* [in] */ Float yvel)
{
    mReleaseInProgress = TRUE;
    mCallback->OnViewReleased(mCapturedView, xvel, yvel);
    mReleaseInProgress = FALSE;

    if (mDragState == IViewDragHelper::STATE_DRAGGING) {
        // onViewReleased didn't call a method that would have changed this. Go idle.
        SetDragState(IViewDragHelper::STATE_IDLE);
    }
}

void ViewDragHelper::ClearMotionHistory()
{
    if (mInitialMotionX == NULL) {
        return;
    }
    Arrays::Fill(mInitialMotionX, 0);
    Arrays::Fill(mInitialMotionY, 0);
    Arrays::Fill(mLastMotionX, 0);
    Arrays::Fill(mLastMotionY, 0);
    Arrays::Fill(mInitialEdgesTouched, 0);
    Arrays::Fill(mEdgeDragsInProgress, 0);
    Arrays::Fill(mEdgeDragsLocked, 0);
    mPointersDown = 0;
}

void ViewDragHelper::ClearMotionHistory(
    /* [in] */ Int32 pointerId)
{
    if (mInitialMotionX == NULL) {
        return;
    }
    mInitialMotionX[pointerId] = 0;
    mInitialMotionY[pointerId] = 0;
    mLastMotionX[pointerId] = 0;
    mLastMotionY[pointerId] = 0;
    mInitialEdgesTouched[pointerId] = 0;
    mEdgeDragsInProgress[pointerId] = 0;
    mEdgeDragsLocked[pointerId] = 0;
    mPointersDown &= ~(1 << pointerId);
}

void ViewDragHelper::EnsureMotionHistorySizeForId(
    /* [in] */ Int32 pointerId)
{
    if (mInitialMotionX == NULL || mInitialMotionX->GetLength() <= pointerId) {
        AutoPtr<ArrayOf<Float> > imx = ArrayOf<Float>::Alloc(pointerId + 1);
        AutoPtr<ArrayOf<Float> > imy = ArrayOf<Float>::Alloc(pointerId + 1);
        AutoPtr<ArrayOf<Float> > lmx = ArrayOf<Float>::Alloc(pointerId + 1);
        AutoPtr<ArrayOf<Float> > lmy = ArrayOf<Float>::Alloc(pointerId + 1);
        AutoPtr<ArrayOf<Int32> > iit = ArrayOf<Int32::Alloc(pointerId + 1);
        AutoPtr<ArrayOf<Int32> > edip = ArrayOf<Int32::Alloc(pointerId + 1);
        AutoPtr<ArrayOf<Int32> > edl = ArrayOf<Int32::Alloc(pointerId + 1);

        if (mInitialMotionX != NULL) {
            imx->Copy(mInitialMotionX, 0, mInitialMotionX->GetLength());
            imy->Copy(mInitialMotionY, 0, mInitialMotionY->GetLength());
            lmx->Copy(mLastMotionX, 0, mLastMotionX->GetLength());
            lmy->Copy(mLastMotionY, 0, mLastMotionY->GetLength());
            iit->Copy(mInitialEdgesTouched, 0, mInitialEdgesTouched->GetLength());
            edip->Copy(mEdgeDragsInProgress, 0, mEdgeDragsInProgress->GetLength());
            edl->Copy(mEdgeDragsLocked, 0, mEdgeDragsLocked->GetLength());
        }

        mInitialMotionX = imx;
        mInitialMotionY = imy;
        mLastMotionX = lmx;
        mLastMotionY = lmy;
        mInitialEdgesTouched = iit;
        mEdgeDragsInProgress = edip;
        mEdgeDragsLocked = edl;
    }
}

void ViewDragHelper::SaveInitialMotion(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int32 pointerId)
{
    InsureMotionHistorySizeForId(pointerId);
    mInitialMotionX[pointerId] = mLastMotionX[pointerId] = x;
    mInitialMotionY[pointerId] = mLastMotionY[pointerId] = y;
    mInitialEdgesTouched[pointerId] = GetEdgesTouched((Int32) x, (Int32) y);
    mPointersDown |= 1 << pointerId;
}

void ViewDragHelper::SaveLastMotion(
    /* [in] */ IMotionEvent* ev)
{
    assert(0 && "TODO");
    // Int32 pointerCount = MotionEventCompat.getPointerCount(ev);
    // for (Int32 i = 0; i < pointerCount; i++) {
    //     Int32 pointerId = MotionEventCompat.getPointerId(ev, i);
    //     Float x = MotionEventCompat.getX(ev, i);
    //     Float y = MotionEventCompat.getY(ev, i);
    //     mLastMotionX[pointerId] = x;
    //     mLastMotionY[pointerId] = y;
    // }
}

ECode ViewDragHelper::IsPointerDown(
    /* [in] */ Int32 pointerId,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = (mPointersDown & 1 << pointerId) != 0;
    return NOERROR;
}

void ViewDragHelper::SetDragState(
    /* [in] */ Int32 state)
{
    if (mDragState != state) {
        mDragState = state;
        mCallback->OnViewDragStateChanged(state);
        if (state == IViewDragHelper::STATE_IDLE) {
            mCapturedView = NULL;
        }
    }
}

Boolean ViewDragHelper::TryCaptureViewForDrag(
    /* [in] */ IView* toCapture,
    /* [in] */ Int32 pointerId)
{
    if (toCapture == mCapturedView && mActivePointerId == pointerId) {
        // Already done!
        return TRUE;
    }
    Boolean result;
    if (toCapture != NULL &&
            mCallback->TryCaptureView(toCapture, pointerId, &result), result) {
        mActivePointerId = pointerId;
        CaptureChildView(toCapture, pointerId);
        return TRUE;
    }
    return FALSE;
}

Boolean ViewDragHelper::CanScroll(
    /* [in] */ IView* v,
    /* [in] */ Boolean checkV,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (IViewGroup::Probe(v) != NULL) {
        IViewGroup* group = IViewGroup::Probe(v);
        Int32 scrollX;
        v->GetScrollX(&scrollX);
        Int32 scrollY;
        v->GetScrollY(&scrollY);
        Int32 count;
        group->GetChildCount(&count);
        // Count backwards - let topmost views consume scroll distance first.
        for (Int32 i = count - 1; i >= 0; i--) {
            // TODO: Add versioned support here for transformed views.
            // This will not work for transformed views in Honeycomb+
            AutoPtr<IView> child;
            group->GetChildAt(i, (IView**)&child);
            Int32 left, right, top, bottom;
            child->GetLeft(&left);
            child->GetRight(&right);
            child->GetTop(&top);
            child->GetBottom(&bottom);
            if (x + scrollX >= left && x + scrollX < right &&
                    y + scrollY >= top && y + scrollY < bottom &&
                    CanScroll(child, TRUE, dx, dy, x + scrollX - left,
                            y + scrollY - top)) {
                return TRUE;
            }
        }
    }

    assert(0 && "TODO");
    // return checkV && (ViewCompat.canScrollHorizontally(v, -dx) ||
    //         ViewCompat.canScrollVertically(v, -dy));
    return FALSE;
}

ECode ViewDragHelper::ShouldInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    Int32 action;
    Int32 actionIndex;
    assert(0 && "TODO");
    // action = MotionEventCompat.getActionMasked(ev);
    // actionIndex = MotionEventCompat.getActionIndex(ev);

    if (action == IMotionEvent::ACTION_DOWN) {
        // Reset things for a new event stream, just in case we didn't get
        // the whole previous stream.
        Cancel();
    }

    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    mVelocityTracker->AddMovement(ev);

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            Float x;
            ev->GetX(&x);
            Float y;
            ev->GetY(&y);
            Int32 pointerId;
            assert(0 && "TODO");
            // pointerId = MotionEventCompat.getPointerId(ev, 0);
            SaveInitialMotion(x, y, pointerId);

            AutoPtr<IView> toCapture;
            FindTopChildUnder((Int32) x, (Int32) y, (IView**)&toCapture);

            // Catch a settling view if possible.
            if (toCapture == mCapturedView && mDragState == IViewDragHelper::STATE_SETTLING) {
                TryCaptureViewForDrag(toCapture, pointerId);
            }

            Int32 edgesTouched = mInitialEdgesTouched[pointerId];
            if ((edgesTouched & mTrackingEdges) != 0) {
                mCallback->OnEdgeTouched(edgesTouched & mTrackingEdges, pointerId);
            }
            break;
        }

        // TODO:
        // case MotionEventCompat.ACTION_POINTER_DOWN: {
        //     Int32 pointerId = MotionEventCompat.getPointerId(ev, actionIndex);
        //     Float x = MotionEventCompat.getX(ev, actionIndex);
        //     Float y = MotionEventCompat.getY(ev, actionIndex);

        //     saveInitialMotion(x, y, pointerId);

        //     // A ViewDragHelper can only manipulate one view at a time.
        //     if (mDragState == IViewDragHelper::STATE_IDLE) {
        //         Int32 edgesTouched = mInitialEdgesTouched[pointerId];
        //         if ((edgesTouched & mTrackingEdges) != 0) {
        //             mCallback.onEdgeTouched(edgesTouched & mTrackingEdges, pointerId);
        //         }
        //     }
        //     else if (mDragState == IViewDragHelper::STATE_SETTLING) {
        //         // Catch a settling view if possible.
        //         final View toCapture = findTopChildUnder((int) x, (int) y);
        //         if (toCapture == mCapturedView) {
        //             tryCaptureViewForDrag(toCapture, pointerId);
        //         }
        //     }
        //     break;
        // }

        case IMotionEvent::ACTION_MOVE: {
            // First to cross a touch slop over a draggable view wins. Also report edge drags.
            Int32 pointerCount;
            assert(0 && "TODO");
            // pointerCount = MotionEventCompat.getPointerCount(ev);
            for (Int32 i = 0; i < pointerCount; i++) {
                Int32 pointerId;
                assert(0 && "TODO");
                // pointerId = MotionEventCompat.getPointerId(ev, i);
                Float x;
                // x = MotionEventCompat.getX(ev, i);
                Float y;
                // y = MotionEventCompat.getY(ev, i);
                Float dx = x - mInitialMotionX[pointerId];
                Float dy = y - mInitialMotionY[pointerId];

                ReportNewEdgeDrags(dx, dy, pointerId);
                if (mDragState == IViewDragHelper::STATE_DRAGGING) {
                    // Callback might have started an edge drag
                    break;
                }

                AutoPtr<IView> toCapture;
                FindTopChildUnder((Int32) x, (Int32) y, (IView**)&toCapture);
                Boolean checkRes;

                if (toCapture != NULL && CheckTouchSlop(toCapture, dx, dy, &checkRes), checkRes &&
                        TryCaptureViewForDrag(toCapture, pointerId)) {
                    break;
                }
            }
            SaveLastMotion(ev);
            break;
        }

        // TODO:
        // case MotionEventCompat.ACTION_POINTER_UP: {
        //     Int32 pointerId = MotionEventCompat.getPointerId(ev, actionIndex);
        //     clearMotionHistory(pointerId);
        //     break;
        // }

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL: {
            Cancel();
            break;
        }
    }

    *result = mDragState == IViewDragHelper::STATE_DRAGGING;
    return NOERROR;
}

ECode ViewDragHelper::ProcessTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    Int32 action;
    assert(0 && "TODO");
    // action = MotionEventCompat.getActionMasked(ev);
    Int32 actionIndex;
    // actionIndex = MotionEventCompat.getActionIndex(ev);

    if (action == IMotionEvent::ACTION_DOWN) {
        // Reset things for a new event stream, just in case we didn't get
        // the whole previous stream.
        Cancel();
    }

    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    mVelocityTracker->AddMovement(ev);

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            Float x;
            ev->GetX(&x);
            Float y;
            ev->GetY(&y);
            Int32 pointerId;
            assert(0 && "TODO");
            // pointerId = MotionEventCompat.getPointerId(ev, 0);
            AutoPtr<IView> toCapture;
            FindTopChildUnder((Int32) x, (Int32) y, (IView**)&toCapture);

            SaveInitialMotion(x, y, pointerId);

            // Since the parent is already directly processing this touch event,
            // there is no reason to delay for a slop before dragging.
            // Start immediately if possible.
            TryCaptureViewForDrag(toCapture, pointerId);

            Int32 edgesTouched = mInitialEdgesTouched[pointerId];
            if ((edgesTouched & mTrackingEdges) != 0) {
                mCallback->OnEdgeTouched(edgesTouched & mTrackingEdges, pointerId);
            }
            break;
        }

        // TODO:
        // case MotionEventCompat.ACTION_POINTER_DOWN: {
        //     Int32 pointerId = MotionEventCompat.getPointerId(ev, actionIndex);
        //     Float x = MotionEventCompat.getX(ev, actionIndex);
        //     Float y = MotionEventCompat.getY(ev, actionIndex);

        //     saveInitialMotion(x, y, pointerId);

        //     // A ViewDragHelper can only manipulate one view at a time.
        //     if (mDragState == STATE_IDLE) {
        //         // If we're idle we can do anything! Treat it like a normal down event.

        //         final View toCapture = findTopChildUnder((int) x, (int) y);
        //         tryCaptureViewForDrag(toCapture, pointerId);

        //         Int32 edgesTouched = mInitialEdgesTouched[pointerId];
        //         if ((edgesTouched & mTrackingEdges) != 0) {
        //             mCallback.onEdgeTouched(edgesTouched & mTrackingEdges, pointerId);
        //         }
        //     } else if (isCapturedViewUnder((int) x, (int) y)) {
        //         // We're still tracking a captured view. If the same view is under this
        //         // point, we'll swap to controlling it with this pointer instead.
        //         // (This will still work if we're "catching" a settling view.)

        //         tryCaptureViewForDrag(mCapturedView, pointerId);
        //     }
        //     break;
        // }

        case IMotionEvent::ACTION_MOVE: {
            if (mDragState == STATE_DRAGGING) {
                Int32 index;
                assert(0 && "TODO");
                // index = MotionEventCompat.findPointerIndex(ev, mActivePointerId);
                Float x;
                // x = MotionEventCompat.getX(ev, index);
                Float y;
                // y = MotionEventCompat.getY(ev, index);
                Int32 idx = (Int32) (x - mLastMotionX[mActivePointerId]);
                Int32 idy = (Int32) (y - mLastMotionY[mActivePointerId]);

                Int32 left, top;
                mCapturedView->GetLeft(&left);
                mCapturedView->GetTop(&top);
                DragTo(left + idx, top + idy, idx, idy);

                SaveLastMotion(ev);
            }
            else {
                // Check to see if any pointer is now over a draggable view.
                Int32 pointerCount;
                assert(0 && "TODO");
                // pointerCount = MotionEventCompat.getPointerCount(ev);
                for (Int32 i = 0; i < pointerCount; i++) {
                    Int32 pointerId;
                    assert(0 && "TODO");
                    // pointerId = MotionEventCompat.getPointerId(ev, i);
                    Float x;
                    // x = MotionEventCompat.getX(ev, i);
                    Float y;
                    // y = MotionEventCompat.getY(ev, i);
                    Float dx = x - mInitialMotionX[pointerId];
                    Float dy = y - mInitialMotionY[pointerId];

                    ReportNewEdgeDrags(dx, dy, pointerId);
                    if (mDragState == IViewDragHelper::STATE_DRAGGING) {
                        // Callback might have started an edge drag.
                        break;
                    }

                    AutoPtr<IView> toCapture;
                    FindTopChildUnder((Int32) x, (Int32) y, (IView**)&toCapture);
                    Boolean checkRes;
                    if (CheckTouchSlop(toCapture, dx, dy, &checkRes), checkRes &&
                            TryCaptureViewForDrag(toCapture, pointerId)) {
                        break;
                    }
                }
                SaveLastMotion(ev);
            }
            break;
        }

        // TODO:
        // case MotionEventCompat.ACTION_POINTER_UP: {
        //     Int32 pointerId = MotionEventCompat.getPointerId(ev, actionIndex);
        //     if (mDragState == STATE_DRAGGING && pointerId == mActivePointerId) {
        //         // Try to find another pointer that's still holding on to the captured view.
        //         int newActivePointer = INVALID_POINTER;
        //         Int32 pointerCount = MotionEventCompat.getPointerCount(ev);
        //         for (int i = 0; i < pointerCount; i++) {
        //             Int32 id = MotionEventCompat.getPointerId(ev, i);
        //             if (id == mActivePointerId) {
        //                 // This one's going away, skip.
        //                 continue;
        //             }

        //             Float x = MotionEventCompat.getX(ev, i);
        //             Float y = MotionEventCompat.getY(ev, i);
        //             if (findTopChildUnder((int) x, (int) y) == mCapturedView &&
        //                     tryCaptureViewForDrag(mCapturedView, id)) {
        //                 newActivePointer = mActivePointerId;
        //                 break;
        //             }
        //         }

        //         if (newActivePointer == INVALID_POINTER) {
        //             // We didn't find another pointer still touching the view, release it.
        //             releaseViewForPointerUp();
        //         }
        //     }
        //     clearMotionHistory(pointerId);
        //     break;
        // }

        case IMotionEvent::ACTION_UP: {
            if (mDragState == IViewDragHelper::STATE_DRAGGING) {
                ReleaseViewForPointerUp();
            }
            Cancel();
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            if (mDragState == IViewDragHelper::STATE_DRAGGING) {
                DispatchViewReleased(0, 0);
            }
            Cancel();
            break;
        }
    }
    return NOERROR;
}

void ViewDragHelper::ReportNewEdgeDrags(
    /* [in] */ Float dx,
    /* [in] */ Float dy,
    /* [in] */ Int32 pointerId)
{
    Int32 dragsStarted = 0;
    if (CheckNewEdgeDrag(dx, dy, pointerId, IViewDragHelper::EDGE_LEFT)) {
        dragsStarted |= IViewDragHelper::EDGE_LEFT;
    }
    if (CheckNewEdgeDrag(dy, dx, pointerId, IViewDragHelper::EDGE_TOP)) {
        dragsStarted |= IViewDragHelper::EDGE_TOP;
    }
    if (CheckNewEdgeDrag(dx, dy, pointerId, IViewDragHelper::EDGE_RIGHT)) {
        dragsStarted |= IViewDragHelper::EDGE_RIGHT;
    }
    if (CheckNewEdgeDrag(dy, dx, pointerId, IViewDragHelper::EDGE_BOTTOM)) {
        dragsStarted |= IViewDragHelper::EDGE_BOTTOM;
    }

    if (dragsStarted != 0) {
        mEdgeDragsInProgress[pointerId] |= dragsStarted;
        mCallback->OnEdgeDragStarted(dragsStarted, pointerId);
    }
}

Boolean ViewDragHelper::CheckNewEdgeDrag(
    /* [in] */ Float delta,
    /* [in] */ Float odelta,
    /* [in] */ Int32 pointerId,
    /* [in] */ Int32 edge)
{
    Float absDelta = Math::Abs(delta);
    Float absODelta = Math::Abs(odelta);

    if ((mInitialEdgesTouched[pointerId] & edge) != edge  || (mTrackingEdges & edge) == 0 ||
            (mEdgeDragsLocked[pointerId] & edge) == edge ||
            (mEdgeDragsInProgress[pointerId] & edge) == edge ||
            (absDelta <= mTouchSlop && absODelta <= mTouchSlop)) {
        return FALSE;
    }

    Boolean res;
    if (absDelta < absODelta * 0.5f && mCallback->OnEdgeLock(edge, &res), res) {
        mEdgeDragsLocked[pointerId] |= edge;
        return FALSE;
    }
    return (mEdgeDragsInProgress[pointerId] & edge) == 0 && absDelta > mTouchSlop;
}

Boolean ViewDragHelper::CheckTouchSlop(
    /* [in] */ IView* child,
    /* [in] */ Float dx,
    /* [in] */ Float dy)
{
    if (child == NULL) {
        return FALSE;
    }
    Int32 range;
    mCallback->GetViewHorizontalDragRange(child, &range);
    Boolean checkHorizontal = range > 0;
    mCallback->GetViewVerticalDragRange(child, &range);
    Boolean checkVertical = range > 0;

    if (checkHorizontal && checkVertical) {
        return dx * dx + dy * dy > mTouchSlop * mTouchSlop;
    }
    else if (checkHorizontal) {
        return Math::Abs(dx) > mTouchSlop;
    }
    else if (checkVertical) {
        return Math::Abs(dy) > mTouchSlop;
    }
    return FALSE;
}

ECode ViewDragHelper::CheckTouchSlop(
    /* [in] */ Int32 directions,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    Int32 count = mInitialMotionX->GetLength();
    for (Int32 i = 0; i < count; i++) {
        Boolean checkRes;
        if (checkTouchSlop(directions, i, &checkRes), checkRes) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ViewDragHelper::CheckTouchSlop(
    /* [in] */ Int32 directions,
    /* [in] */ Int32 pointerId,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    Boolean isPointerDown;
    if (IsPointerDown(pointerId, &isPointerDown), !isPointerDown) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean checkHorizontal = (directions & IViewDragHelper::DIRECTION_HORIZONTAL)
            == IViewDragHelper::DIRECTION_HORIZONTAL;
    Boolean checkVertical = (directions & IViewDragHelper::DIRECTION_VERTICAL)
            == IViewDragHelper::DIRECTION_VERTICAL;

    Float dx = mLastMotionX[pointerId] - mInitialMotionX[pointerId];
    Float dy = mLastMotionY[pointerId] - mInitialMotionY[pointerId];

    if (checkHorizontal && checkVertical) {
        *result = dx * dx + dy * dy > mTouchSlop * mTouchSlop;
        return NOERROR;
    }
    else if (checkHorizontal) {
        *result = Math::Abs(dx) > mTouchSlop;
        return NOERROR;
    }
    else if (checkVertical) {
        *result = Math::Abs(dy) > mTouchSlop;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode ViewDragHelper::IsEdgeTouched(
    /* [in] */ Int32 edges,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    Int32 count = mInitialEdgesTouched->GetLength();
    for (Int32 i = 0; i < count; i++) {
        Boolean isEdgeTouched;
        if (IsEdgeTouched(edges, i, &isEdgeTouched), isEdgeTouched) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode ViewDragHelper::IsEdgeTouched(
    /* [in] */ Int32 edges,
    /* [in] */ Int32 pointerId,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    Boolean isPointerDown;
    IsPointerDown(pointerId, &isPointerDown);
    *result = isPointerDown && (mInitialEdgesTouched[pointerId] & edges) != 0;
    return NOERROR;
}

void ViewDragHelper::ReleaseViewForPointerUp()
{
    mVelocityTracker->ComputeCurrentVelocity(1000, mMaxVelocity);
    assert(0 && "TODO");
    // Float xvel = ClampMag(
    //         VelocityTrackerCompat.getXVelocity(mVelocityTracker, mActivePointerId),
    //         mMinVelocity, mMaxVelocity);
    // Float yvel = ClampMag(
    //         VelocityTrackerCompat.getYVelocity(mVelocityTracker, mActivePointerId),
    //         mMinVelocity, mMaxVelocity);
    // DispatchViewReleased(xvel, yvel);
}

void ViewDragHelper::DragTo(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    int clampedX = left;
    int clampedY = top;
    Int32 oldLeft;
    mCapturedView0>GetLeft(&oldLeft);
    Int32 oldTop;
    mCapturedView0>GetTop(&oldTop);
    if (dx != 0) {
        mCallback->ClampViewPositionHorizontal(mCapturedView, left, dx, &clampedX);
        mCapturedView->OffsetLeftAndRight(clampedX - oldLeft);
    }
    if (dy != 0) {
        mCallback->ClampViewPositionVertical(mCapturedView, top, dy, &clampedY);
        mCapturedView->OffsetTopAndBottom(clampedY - oldTop);
    }

    if (dx != 0 || dy != 0) {
        Int32 clampedDx = clampedX - oldLeft;
        Int32 clampedDy = clampedY - oldTop;
        mCallback->OnViewPositionChanged(mCapturedView, clampedX, clampedY,
                clampedDx, clampedDy);
    }
}

ECode ViewDragHelper::IsCapturedViewUnder(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    return IsViewUnder(mCapturedView, x, y, result);
}

ECode ViewDragHelper::IsViewUnder(
    /* [in] */ IView* view,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    if (view == NULL) {
        *result = FALSE;
    }
    Int32 left, right, top, bottom;
    view->GetLeft(&left);
    view->GetRight(&right);
    view->GetTop(&top);
    view->GetBottom(&bottom);
    *result = x >= left &&  x < right &&
            y >= top && y < bottom;
    return NOERROR;
}

ECode ViewDragHelper::FindTopChildUnder(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);
    Int32 childCount;
    mParentView->GetChildCount(&childCount);
    for (Int32 i = childCount - 1; i >= 0; i--) {
        Int32 index;
        mCallback->GetOrderedChildIndex(i, &index);
        AutoPtr<IView> child;
        mParentView->GetChildAt(index, (IView**)&child);
        Int32 left, right, top, bottom;
        child->GetLeft(&left);
        child->GetRight(&right);
        child->GetTop(&top);
        child->GetBottom(&bottom);
        if (x >= left && x < right &&
                y >= top && y < bottom) {
            *view = child;
            REFCOUNT_ADD(*view);
            return NOERROR;
        }
    }
    *view = NULL;
    return NOERROR;
}

Int32 ViewDragHelper::GetEdgesTouched(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 result = 0;

    Int32 left, right, top, bottom;
    mParentView->GetLeft(&left);
    mParentView->GetRight(&right);
    mParentView->GetTop(&top);
    mParentView->GetBottom(&bottom);
    if (x < left + mEdgeSize) result |= IViewDragHelper::EDGE_LEFT;
    if (y < top + mEdgeSize) result |= IViewDragHelper::EDGE_TOP;
    if (x > right - mEdgeSize) result |= IViewDragHelper::EDGE_RIGHT;
    if (y > bottom - mEdgeSize) result |= IViewDragHelper::EDGE_BOTTOM;

    return result;
}

ECode ViewDragHelper::StartNestedScroll(
    /* [in] */ IView* target)
{
    if (mVelocityTracker == NULL) {
        AutoPtr<IVelocityTrackerHelper> helper;
        CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
        helper->Obtain((IVelocityTracker**)&mVelocityTracker);
    }
    SetDragState(IViewDragHelper::STATE_DRAGGING);
    mCapturedView = target;
    return NOERROR;
}

ECode ViewDragHelper::StopNestedScroll(
    /* [in] */ IView* target)
{
    mCapturedView = target;
    DispatchViewReleased(0, 0);
    return NOERROR;
}

ECode ViewDragHelper::ProcessNestedScroll(
    /* [in] */ IView* target,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ ArrayOf<Int32>* consumed)
{
    Int32 left, top;
    mCapturedView->GetLeft(&left);
    mCapturedView->GetTop(&top);
    Int32 targetX = left + dx;
    Int32 targetY = top + dy;
    DragTo(targetX, targetY, dx, dy);
    if (consumed != null) {
        mCapturedView->GetLeft(&left);
        mCapturedView->GetTop(&top);
        Int32 unconsumedX = targetX - left;
        Int32 unconsumedY = targetY - top;
        consumed[0] = unconsumedX - dx;
        consumed[1] = unconsumedY - dy;
    }
    return NOERROR;
}

} // Widget
} // Dialer
} // Apps
} // Elastos
