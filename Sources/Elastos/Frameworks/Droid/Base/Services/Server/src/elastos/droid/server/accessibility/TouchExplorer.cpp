
#include "accessibility/CAccessibilityManagerService.h"
#include "accessibility/TouchExplorer.h"
#include "accessibility/GestureUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CObjectContainer;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Server::Accessibility::EIID_IEventStreamTransformation;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IPointerProperties;
using Elastos::Droid::View::CPointerProperties;
using Elastos::Droid::View::IPointerCoords;
using Elastos::Droid::View::CPointerCoords;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Gesture::IGestureLibraries;
using Elastos::Droid::Gesture::CGestureLibraries;
using Elastos::Droid::Gesture::IGestureStore;
using Elastos::Droid::Gesture::CGesturePoint;
using Elastos::Droid::Gesture::CGesture;
using Elastos::Droid::Gesture::IGesture;
using Elastos::Droid::Gesture::IGestureStroke;
using Elastos::Droid::Gesture::CGestureStroke;
using Elastos::Droid::Gesture::IPrediction;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const Boolean TouchExplorer::DEBUG = TRUE;
const String TouchExplorer::LOCAL_LOG_TAG("TouchExplorer");
const Int32 TouchExplorer::STATE_TOUCH_EXPLORING = 0x00000001;
const Int32 TouchExplorer::STATE_DRAGGING = 0x00000002;
const Int32 TouchExplorer::STATE_DELEGATING = 0x00000004;
const Int32 TouchExplorer::STATE_GESTURE_DETECTING = 0x00000005;
const Float TouchExplorer::MAX_DRAGGING_ANGLE_COS = 0.525321989f; // cos(pi/4)
const Int32 TouchExplorer::ALL_POINTER_ID_BITS = 0xFFFFFFFF;
const Int32 TouchExplorer::MAX_POINTER_COUNT = 32;
const Int32 TouchExplorer::INVALID_POINTER_ID = -1;
const Int32 TouchExplorer::GESTURE_DETECTION_VELOCITY_DIP = 1000;
const Int32 TouchExplorer::MIN_POINTER_DISTANCE_TO_USE_MIDDLE_LOCATION_DIP = 200;
const Int32 TouchExplorer::EXIT_GESTURE_DETECTION_TIMEOUT = 2000;
const Int32 TouchExplorer::TOUCH_TOLERANCE = 3;
const Float TouchExplorer::MIN_PREDICTION_SCORE = 2.0f;

const String TouchExplorer::SendHoverDelayed::LOG_TAG_SEND_HOVER_DELAYED = String("SendHoverDelayed");

//===============================================================
// TouchExplorer::SendHoverDelayed
//===============================================================
TouchExplorer::SendHoverDelayed::SendHoverDelayed(
    /* [in] */ Int32 hoverAction,
    /* [in] */ Boolean gestureStarted,
    /* [in] */ TouchExplorer* host)
    : mHoverAction(hoverAction)
    , mGestureStarted(gestureStarted)
    , mPointerIdBits(0)
    , mPolicyFlags(0)
    , mHost(host)
{}

void TouchExplorer::SendHoverDelayed::Post(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Boolean touchExplorationInProgress,
    /* [in] */ Int32 pointerIdBits,
    /* [in] */ Int32 policyFlags)
{
    Remove();
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    mPrototype = NULL;
    helper->Obtain(prototype, (IMotionEvent**)&mPrototype);
    mPointerIdBits = pointerIdBits;
    mPolicyFlags = policyFlags;

    Boolean result;
    mHost->mHandler->PostDelayed(this, mHost->mDetermineUserIntentTimeout, &result);
}

Float TouchExplorer::SendHoverDelayed::GetX()
{
    if (IsPending()) {
        Float x;
        mPrototype->GetX(&x);
        return x;
    }
    return 0;
}

Float TouchExplorer::SendHoverDelayed::GetY()
{
    if (IsPending()) {
        Float y;
        mPrototype->GetY(&y);
        return y;
    }
    return 0;
}

void TouchExplorer::SendHoverDelayed::Remove()
{
    mHost->mHandler->RemoveCallbacks(this);

    Clear();
}

Boolean TouchExplorer::SendHoverDelayed::IsPending()
{
    return (mPrototype != NULL);
}

void TouchExplorer::SendHoverDelayed::Clear()
{
    if (!IsPending()) {
        return;
    }
    mPrototype->Recycle();
    mPrototype = NULL;
    mPointerIdBits = -1;
    mPolicyFlags = 0;
}

void TouchExplorer::SendHoverDelayed::ForceSendAndRemove()
{
    if (IsPending()) {
        Run();
        Remove();
}
}

ECode TouchExplorer::SendHoverDelayed::Run()
{
    // if (DEBUG) {
    //     Slog.d(LOG_TAG_SEND_HOVER_DELAYED, "Injecting motion event: "
    //             + IMotionEvent::actionToString(mHoverAction));
    //     Slog.d(LOG_TAG_SEND_HOVER_DELAYED, mGestureStarted ?
    //             "touchExplorationGestureStarted" : "touchExplorationGestureEnded");
    // }
    if (mGestureStarted) {
        mHost->SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_START);
    }
    else {
        if (!mHost->mSendTouchExplorationEndDelayed->IsPending()) {
            mHost->mSendTouchExplorationEndDelayed->Post();
        }
        if (mHost->mSendTouchInteractionEndDelayed->IsPending()) {
            mHost->mSendTouchInteractionEndDelayed->Remove();
            mHost->mSendTouchInteractionEndDelayed->Post();
        }
    }
    mHost->SendMotionEvent(mPrototype, mHoverAction, mPointerIdBits, mPolicyFlags);
    Clear();

    return NOERROR;
}

//===============================================================
// TouchExplorer::InjectedPointerTracker
//===============================================================
const String TouchExplorer::InjectedPointerTracker::LOG_TAG_INJECTED_POINTER_TRACKER =
        String("InjectedPointerTracker");

TouchExplorer::InjectedPointerTracker::InjectedPointerTracker()
    : mInjectedPointersDown(0)
    , mLastInjectedDownEventTime(0)
{}

void TouchExplorer::InjectedPointerTracker::OnMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 action;
    event->GetActionMasked(&action);
    Int32 pointerId;
    Int32 actionIndex;
    Int32 pointerFlag;
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
        case IMotionEvent::ACTION_POINTER_DOWN: {
            event->GetActionIndex(&actionIndex);
            event->GetPointerId(actionIndex, &pointerId);
            pointerFlag = (1 << pointerId);
            mInjectedPointersDown |= pointerFlag;
            event->GetDownTime(&mLastInjectedDownEventTime);
        } break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_POINTER_UP: {
            event->GetActionIndex(&actionIndex);
            event->GetPointerId(actionIndex, &pointerId);
            pointerFlag = (1 << pointerId);
            mInjectedPointersDown &= ~pointerFlag;
            if (mInjectedPointersDown == 0) {
                mLastInjectedDownEventTime = 0;
            }
        } break;
        case IMotionEvent::ACTION_HOVER_ENTER:
        case IMotionEvent::ACTION_HOVER_MOVE:
        case IMotionEvent::ACTION_HOVER_EXIT: {
            if (mLastInjectedHoverEvent != NULL) {
                mLastInjectedHoverEvent->Recycle();
                mLastInjectedHoverEvent = NULL;
            }
            AutoPtr<IMotionEventHelper> helper;
            CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
            helper->Obtain(event, (IMotionEvent**)&mLastInjectedHoverEvent);
            if (mLastInjectedHoverEventForClick != NULL) {
                mLastInjectedHoverEventForClick->Recycle();
                mLastInjectedHoverEventForClick = NULL;
            }
            helper->Obtain(event, (IMotionEvent**)&mLastInjectedHoverEventForClick);
        } break;
    }
    // if (DEBUG) {
    //     Slog.i(LOG_TAG_INJECTED_POINTER_TRACKER, "Injected pointer:\n" + toString());
    // }
}

void TouchExplorer::InjectedPointerTracker::Clear()
{
    mInjectedPointersDown = 0;
}

Int64 TouchExplorer::InjectedPointerTracker::GetLastInjectedDownEventTime()
{
    return mLastInjectedDownEventTime;
}

Int32 TouchExplorer::InjectedPointerTracker::GetInjectedPointerDownCount()
{
    // return Integer.bitCount(mInjectedPointersDown);
    assert(0);
    return 0;
}

Int32 TouchExplorer::InjectedPointerTracker::GetInjectedPointersDown()
{
    return mInjectedPointersDown;
}

Boolean TouchExplorer::InjectedPointerTracker::IsInjectedPointerDown(
    /* [in] */ Int32 pointerId)
{
    Int32 pointerFlag = (1 << pointerId);
    return (mInjectedPointersDown & pointerFlag) != 0;
}

AutoPtr<IMotionEvent> TouchExplorer::InjectedPointerTracker::GetLastInjectedHoverEvent()
{
    return mLastInjectedHoverEvent;
}

AutoPtr<IMotionEvent> TouchExplorer::InjectedPointerTracker::GetLastInjectedHoverEventForClick()
{
    return mLastInjectedHoverEventForClick;
}

String TouchExplorer::InjectedPointerTracker::ToString()
{
    StringBuilder builder;
    builder.Append("=========================");
    builder.Append("\nDown pointers #");
    // builder.Append(Integer.bitCount(mInjectedPointersDown));
    builder.Append(" [ ");
    for (Int32 i = 0; i < TouchExplorer::MAX_POINTER_COUNT; i++) {
        if ((mInjectedPointersDown & i) != 0) {
            builder.AppendInt32(i);
            builder.Append(" ");
        }
    }
    builder.Append("]");
    builder.Append("\n=========================");
    return builder.ToString();
}

//===============================================================
// TouchExplorer::ReceivedPointerTracker
//===============================================================
const String TouchExplorer::ReceivedPointerTracker::LOG_TAG_RECEIVED_POINTER_TRACKER =
        String("ReceivedPointerTracker");
const Int32 TouchExplorer::ReceivedPointerTracker::COEFFICIENT_ACTIVE_POINTER = 2;

TouchExplorer::ReceivedPointerTracker::ReceivedPointerTracker(
    /* [in] */ IContext* context,
    /* [in] */ TouchExplorer* host)
    : mReceivedPointersDown(0)
    , mLastReceivedDownEdgeFlags(0)
    , mActivePointers(0)
    , mPrimaryActivePointerId(0)
    , mHasMovingActivePointer(FALSE)
    , mLastReceivedUpPointerDownTime(0)
    , mLastReceivedUpPointerId(0)
    , mLastReceivedUpPointerActive(FALSE)
    , mLastReceivedUpPointerDownX(0)
    , mLastReceivedUpPointerDownY(0)
    , mHost(host)
{
    mReceivedPointerDownX = ArrayOf<Float>::Alloc(TouchExplorer::MAX_POINTER_COUNT);
    mReceivedPointerDownY = ArrayOf<Float>::Alloc(TouchExplorer::MAX_POINTER_COUNT);
    mReceivedPointerDownTime = ArrayOf<Int64>::Alloc(TouchExplorer::MAX_POINTER_COUNT);

    AutoPtr<IViewConfigurationHelper> helper;
    ASSERT_SUCCEEDED(CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper));
    AutoPtr<IViewConfiguration> config;
    ASSERT_SUCCEEDED(helper->Get(context, (IViewConfiguration**)&config));
    Int32 slop;
    config->GetScaledTouchSlop(&slop);
    mThresholdActivePointer = slop * COEFFICIENT_ACTIVE_POINTER;
}

void TouchExplorer::ReceivedPointerTracker::Clear()
{
    Int32 i = 0;
    for (; i < mReceivedPointerDownX->GetLength(); ++i) {
        (*mReceivedPointerDownX)[i] = 0;
    }
    for (i = 0; i < mReceivedPointerDownY->GetLength(); ++i) {
        (*mReceivedPointerDownY)[i] = 0;
    }
    for (i = 0; i < mReceivedPointerDownTime->GetLength(); ++i) {
        (*mReceivedPointerDownTime)[i] = 0;
    }
    mReceivedPointersDown = 0;
    mActivePointers = 0;
    mPrimaryActivePointerId = 0;
    mHasMovingActivePointer = FALSE;
    mLastReceivedUpPointerDownTime = 0;
    mLastReceivedUpPointerId = 0;
    mLastReceivedUpPointerActive = FALSE;
    mLastReceivedUpPointerDownX = 0;
    mLastReceivedUpPointerDownY = 0;
}

void TouchExplorer::ReceivedPointerTracker::OnMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    if (mLastReceivedEvent != NULL) {
        mLastReceivedEvent->Recycle();
        mLastReceivedEvent = NULL;
    }
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    helper->Obtain(event, (IMotionEvent**)&mLastReceivedEvent);

    Int32 action;
    event->GetActionMasked(&action);
    Int32 actionIndex;
    event->GetActionIndex(&actionIndex);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            HandleReceivedPointerDown(actionIndex, event);
            break;
        case IMotionEvent::ACTION_POINTER_DOWN:
            HandleReceivedPointerDown(actionIndex, event);
            break;
        case IMotionEvent::ACTION_MOVE:
            HandleReceivedPointerMove(event);
            break;
        case IMotionEvent::ACTION_UP:
            HandleReceivedPointerUp(actionIndex, event);
            break;
        case IMotionEvent::ACTION_POINTER_UP:
            HandleReceivedPointerUp(actionIndex, event);
            break;
    }
    // if (DEBUG) {
    //     Slog.i(LOG_TAG_RECEIVED_POINTER_TRACKER, "Received pointer: " + toString());
    // }
}

AutoPtr<IMotionEvent> TouchExplorer::ReceivedPointerTracker::GetLastReceivedEvent()
{
    return mLastReceivedEvent;
}

Int32 TouchExplorer::ReceivedPointerTracker::GetReceivedPointerDownCount()
{
    // return Integer.bitCount(mReceivedPointersDown);
    assert(0);
    return 0;
}

Int32 TouchExplorer::ReceivedPointerTracker::GetActivePointers()
{
    return mActivePointers;
}

Int32 TouchExplorer::ReceivedPointerTracker::GetActivePointerCount()
{
    // return Integer.bitCount(mActivePointers);
    assert(0);
    return 0;
}

Boolean TouchExplorer::ReceivedPointerTracker::IsReceivedPointerDown(
    /* [in] */ Int32 pointerId)
{
    Int32 pointerFlag = (1 << pointerId);
    return (mReceivedPointersDown & pointerFlag) != 0;
}

Boolean TouchExplorer::ReceivedPointerTracker::IsActivePointer(
    /* [in] */ Int32 pointerId)
{
    Int32 pointerFlag = (1 << pointerId);
    return (mActivePointers & pointerFlag) != 0;
}

Float TouchExplorer::ReceivedPointerTracker::GetReceivedPointerDownX(
    /* [in] */ Int32 pointerId)
{
    return (*mReceivedPointerDownX)[pointerId];
}

Float TouchExplorer::ReceivedPointerTracker::GetReceivedPointerDownY(
    /* [in] */ Int32 pointerId)
{
    return (*mReceivedPointerDownY)[pointerId];
}

Int64 TouchExplorer::ReceivedPointerTracker::GetReceivedPointerDownTime(
    /* [in] */ Int32 pointerId)
{
    return (*mReceivedPointerDownTime)[pointerId];
}

Int32 TouchExplorer::ReceivedPointerTracker::GetPrimaryActivePointerId()
{
    if (mPrimaryActivePointerId == TouchExplorer::INVALID_POINTER_ID) {
        mPrimaryActivePointerId = FindPrimaryActivePointer();
    }
    return mPrimaryActivePointerId;
}

Int64 TouchExplorer::ReceivedPointerTracker::GetLastReceivedUpPointerDownTime()
{
    return mLastReceivedUpPointerDownTime;
}

Int32 TouchExplorer::ReceivedPointerTracker::GetLastReceivedUpPointerId()
{
    return mLastReceivedUpPointerId;
}

Float TouchExplorer::ReceivedPointerTracker::GetLastReceivedUpPointerDownX()
{
    return mLastReceivedUpPointerDownX;
}

Float TouchExplorer::ReceivedPointerTracker::GetLastReceivedUpPointerDownY()
{
    return mLastReceivedUpPointerDownY;
}

Int32 TouchExplorer::ReceivedPointerTracker::GetLastReceivedDownEdgeFlags()
{
    return mLastReceivedDownEdgeFlags;
}

Boolean TouchExplorer::ReceivedPointerTracker::WasLastReceivedUpPointerActive()
{
    return mLastReceivedUpPointerActive;
}

void TouchExplorer::ReceivedPointerTracker::PopulateActivePointerIds(
    /* [in] */ ArrayOf<Int32>* outPointerIds)
{
    // Int32 index = 0;
    // for (Int32 idBits = mActivePointers; idBits != 0; ) {
    //     Int32 id = Integer.numberOfTrailingZeros(idBits);
    //     idBits &= ~(1 << id);
    //     outPointerIds[index] = id;
    //     index++;
    // }
    assert(0);
}

Boolean TouchExplorer::ReceivedPointerTracker::IsActiveOrWasLastActiveUpPointer(
    /* [in] */ Int32 pointerId)
{
    return (IsActivePointer(pointerId)
            || (mLastReceivedUpPointerId == pointerId
                    && mLastReceivedUpPointerActive));
}

void TouchExplorer::ReceivedPointerTracker::HandleReceivedPointerDown(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ IMotionEvent* event)
{
    Int32 pointerId;
    event->GetPointerId(pointerIndex, &pointerId);
    Int32 pointerFlag = (1 << pointerId);

    mLastReceivedUpPointerId = 0;
    mLastReceivedUpPointerDownTime = 0;
    mLastReceivedUpPointerActive = FALSE;
    mLastReceivedUpPointerDownX = 0;
    mLastReceivedUpPointerDownX = 0;

    event->GetEdgeFlags(&mLastReceivedDownEdgeFlags);

    mReceivedPointersDown |= pointerFlag;
    event->GetX(pointerIndex, &(*mReceivedPointerDownX)[pointerId]);
    event->GetY(pointerIndex, &(*mReceivedPointerDownY)[pointerId]);
    event->GetEventTime(&(*mReceivedPointerDownTime)[pointerId]);

    if (!mHasMovingActivePointer) {
        // If still no moving active pointers every
        // down pointer is the only active one.
        mActivePointers = pointerFlag;
        mPrimaryActivePointerId = pointerId;
    }
    else {
        // If at least one moving active pointer every
        // subsequent down pointer is active.
        mActivePointers |= pointerFlag;
    }
}

void TouchExplorer::ReceivedPointerTracker::HandleReceivedPointerMove(
    /* [in] */ IMotionEvent* event)
{
    DetectActivePointers(event);
}

void TouchExplorer::ReceivedPointerTracker::HandleReceivedPointerUp(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ IMotionEvent* event)
{
    Int32 pointerId;
    event->GetPointerId(pointerIndex, &pointerId);
    Int32 pointerFlag = (1 << pointerId);

    mLastReceivedUpPointerId = pointerId;
    mLastReceivedUpPointerDownTime = GetReceivedPointerDownTime(pointerId);
    mLastReceivedUpPointerActive = IsActivePointer(pointerId);
    mLastReceivedUpPointerDownX = (*mReceivedPointerDownX)[pointerId];
    mLastReceivedUpPointerDownY = (*mReceivedPointerDownY)[pointerId];

    mReceivedPointersDown &= ~pointerFlag;
    mActivePointers &= ~pointerFlag;
    (*mReceivedPointerDownX)[pointerId] = 0;
    (*mReceivedPointerDownY)[pointerId] = 0;
    (*mReceivedPointerDownTime)[pointerId] = 0;

    if (mActivePointers == 0) {
        mHasMovingActivePointer = FALSE;
    }
    if (mPrimaryActivePointerId == pointerId) {
        mPrimaryActivePointerId = TouchExplorer::INVALID_POINTER_ID;
    }
}

void TouchExplorer::ReceivedPointerTracker::DetectActivePointers(
    /* [in] */ IMotionEvent* event)
{
    Int32 count;
    event->GetPointerCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        Int32 pointerId;
        event->GetPointerId(i, &pointerId);
        if (mHasMovingActivePointer) {
            // If already active => nothing to do.
            if (IsActivePointer(pointerId)) {
                continue;
            }
        }
        // Active pointers are ones that moved more than a given threshold.
        Float pointerDeltaMove = ComputePointerDeltaMove(i, event);
        if (pointerDeltaMove > mThresholdActivePointer) {
            Int32 pointerFlag = (1 << pointerId);
            mActivePointers |= pointerFlag;
            mHasMovingActivePointer = TRUE;
        }
    }
}

Int32 TouchExplorer::ReceivedPointerTracker::FindPrimaryActivePointer()
{
    Int32 primaryActivePointerId = TouchExplorer::INVALID_POINTER_ID;
    Int64 minDownTime = Elastos::Core::Math::INT64_MAX_VALUE;
    // Find the active pointer that went down first.
    for (Int32 i = 0, count = mReceivedPointerDownTime->GetLength(); i < count; ++i) {
        if (IsActivePointer(i)) {
            Int64 downPointerTime = (*mReceivedPointerDownTime)[i];
            if (downPointerTime < minDownTime) {
                minDownTime = downPointerTime;
                primaryActivePointerId = i;
            }
        }
    }
    return primaryActivePointerId;
}

Float TouchExplorer::ReceivedPointerTracker::ComputePointerDeltaMove(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ IMotionEvent* event)
{
    Int32 pointerId;
    event->GetPointerId(pointerIndex, &pointerId);
    Float x, y;
    event->GetX(pointerIndex, &x);
    event->GetY(pointerIndex, &y);
    Double deltaX = (Double)(x - (*mReceivedPointerDownX)[pointerId]);
    Double deltaY = (Double)(y - (*mReceivedPointerDownY)[pointerId]);
    return (Float)Elastos::Core::Math::Hypot(deltaX, deltaY);
}

String TouchExplorer::ReceivedPointerTracker::ToString()
{
    StringBuilder builder;
    builder.Append("=========================");
    builder.Append("\nDown pointers #");
    // builder.Append(getReceivedPointerDownCount());
    builder.Append(" [ ");
    for (Int32 i = 0; i < TouchExplorer::MAX_POINTER_COUNT; i++) {
        if (IsReceivedPointerDown(i)) {
            builder.AppendInt32(i);
            builder.Append(" ");
        }
    }
    builder.Append("]");
    builder.Append("\nActive pointers #");
    // builder.Append(getActivePointerCount());
    builder.Append(" [ ");
    for (Int32 i = 0; i < TouchExplorer::MAX_POINTER_COUNT; i++) {
        if (IsActivePointer(i)) {
            builder.AppendInt32(i);
            builder.Append(" ");
        }
    }
    builder.Append("]");
    builder.Append("\nPrimary active pointer id [ ");
    builder.AppendInt32(GetPrimaryActivePointerId());
    builder.Append(" ]");
    builder.Append("\n=========================");
    return builder.ToString();
}

//===============================================================
// TouchExplorer::DoubleTapDetector
//===============================================================
TouchExplorer::DoubleTapDetector::DoubleTapDetector(
    /* [in] */ TouchExplorer* host)
    : mHost(host)
{}

void TouchExplorer::DoubleTapDetector::OnMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Int32 actionIndex, action;
    event->GetActionIndex(&actionIndex);
    event->GetActionMasked(&action);
    AutoPtr<IMotionEventHelper> helper;

    switch (action) {
        case IMotionEvent::ACTION_DOWN:
        case IMotionEvent::ACTION_POINTER_DOWN:
            if (mFirstTapEvent != NULL
                    && !GestureUtils::IsSamePointerContext(mFirstTapEvent, event)) {
                Clear();
            }
            ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper));
            mDownEvent = NULL;
            helper->Obtain(event, (IMotionEvent**)&mDownEvent);
            break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_POINTER_UP:
            if (mDownEvent == NULL) {
                return;
            }
            if (GestureUtils::IsSamePointerContext(mFirstTapEvent, event)) {
                Clear();
                return;
            }
            if (GestureUtils::IsTap(mDownEvent, event, mHost->mTapTimeout, mHost->mTouchSlop, actionIndex)) {
                if (mFirstTapEvent == NULL || GestureUtils::IsTimedOut(mFirstTapEvent,
                        event, mHost->mDoubleTapTimeout)) {
                    ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton(
                            (IMotionEventHelper**)&helper));
                    mFirstTapEvent = NULL;
                    helper->Obtain(event, (IMotionEvent**)&mFirstTapEvent);
                    mDownEvent->Recycle();
                    mDownEvent = NULL;
                    return;
                }
                if (GestureUtils::IsMultiTap(mFirstTapEvent, event, mHost->mDoubleTapTimeout,
                        mHost->mDoubleTapSlop, actionIndex)) {
                    OnDoubleTap(event, policyFlags);
                    mFirstTapEvent->Recycle();
                    mFirstTapEvent = NULL;
                    mDownEvent->Recycle();
                    mDownEvent = NULL;
                    return;
                }
                mFirstTapEvent->Recycle();
                mFirstTapEvent = NULL;
            }
            else {
                if (mFirstTapEvent != NULL) {
                    mFirstTapEvent->Recycle();
                    mFirstTapEvent = NULL;
                }
            }
            mDownEvent->Recycle();
            mDownEvent = NULL;
            break;
    }
}

void TouchExplorer::DoubleTapDetector::OnDoubleTap(
    /* [in] */ IMotionEvent* secondTapUp,
    /* [in] */ Int32 policyFlags)
{
    // This should never be called when more than two pointers are down.
    Int32 count;
    secondTapUp->GetPointerCount(&count);
    if (count > 2) {
        return;
    }

    // Remove pending event deliveries.
    mHost->mSendHoverEnterDelayed->Remove();
    mHost->mSendHoverExitDelayed->Remove();
    mHost->mPerformLongPressDelayed->Remove();

    if (mHost->mSendTouchExplorationEndDelayed->IsPending()) {
        mHost->mSendTouchExplorationEndDelayed->ForceSendAndRemove();
    }
    if (mHost->mSendTouchInteractionEndDelayed->IsPending()) {
        mHost->mSendTouchInteractionEndDelayed->ForceSendAndRemove();
    }

    Int32 clickLocationX;
    Int32 clickLocationY;

    Int32 actionIndex, pointerId, pointerIndex;
    secondTapUp->GetActionIndex(&actionIndex);
    secondTapUp->GetPointerId(actionIndex, &pointerId);
    secondTapUp->FindPointerIndex(pointerId, &pointerIndex);

    AutoPtr<IMotionEvent> lastExploreEvent =
        mHost->mInjectedPointerTracker->GetLastInjectedHoverEventForClick();
    if (lastExploreEvent == NULL) {
        // No last touch explored event but there is accessibility focus in
        // the active window. We click in the middle of the focus bounds.
        AutoPtr<IRect> focusBounds = mHost->mTempRect;
        if (mHost->mAms->GetAccessibilityFocusBoundsInActiveWindow(focusBounds)) {
            focusBounds->GetCenterX(&clickLocationX);
            focusBounds->GetCenterY(&clickLocationY);
        }
        else {
            // Out of luck - do nothing.
            return;
        }
    }
    else {
        // If the click is within the active window but not within the
        // accessibility focus bounds we click in the focus center.
        Int32 lastExplorePointerIndex;
        lastExploreEvent->GetActionIndex(&lastExplorePointerIndex);
        lastExploreEvent->GetX(lastExplorePointerIndex, (Float*)&clickLocationX);
        lastExploreEvent->GetY(lastExplorePointerIndex, (Float*)&clickLocationY);
        AutoPtr<IRect> activeWindowBounds = mHost->mTempRect;
        if (mHost->mLastTouchedWindowId == mHost->mAms->GetActiveWindowId()) {
            mHost->mAms->GetActiveWindowBounds(activeWindowBounds);
            Boolean isContain;
            if (activeWindowBounds->Contains(clickLocationX, clickLocationY, &isContain), isContain) {
                AutoPtr<IRect> focusBounds = mHost->mTempRect;
                if (mHost->mAms->GetAccessibilityFocusBoundsInActiveWindow(focusBounds)) {
                    if (focusBounds->Contains(clickLocationX, clickLocationY, &isContain), !isContain) {
                        focusBounds->GetCenterX(&clickLocationX);
                        focusBounds->GetCenterY(&clickLocationY);
                    }
                }
            }
        }
    }

    // Do the click.
    AutoPtr< ArrayOf<IPointerProperties*> > properties = ArrayOf<IPointerProperties*>::Alloc(1);
    AutoPtr<IPointerProperties> prop;
    CPointerProperties::New((IPointerProperties**)&prop);
    properties->Set(0, prop);
    secondTapUp->GetPointerProperties(pointerIndex, (*properties)[0]);
    AutoPtr< ArrayOf<IPointerCoords*> > coords = ArrayOf<IPointerCoords*>::Alloc(1);
    AutoPtr<IPointerCoords> coord;
    CPointerCoords::New((IPointerCoords**)&coord);
    coords->Set(0, coord);
    (*coords)[0]->SetX(clickLocationX);
    (*coords)[0]->SetY(clickLocationY);
    AutoPtr<IMotionEventHelper> helper;
    ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper));
    Int64 downTime, eventTime;
    secondTapUp->GetDownTime(&downTime);
    secondTapUp->GetEventTime(&eventTime);
    Int32 deviceId, source, flags;
    secondTapUp->GetDeviceId(&deviceId);
    secondTapUp->GetSource(&source);
    secondTapUp->GetFlags(&flags);
    AutoPtr<IMotionEvent> event;
    helper->Obtain(downTime, eventTime, IMotionEvent::ACTION_DOWN, 1, properties,
            coords, 0, 0, 1.0f, 1.0f, deviceId, 0, source, flags, (IMotionEvent**)&event);
    mHost->SendActionDownAndUp(event, policyFlags);
    event->Recycle();
}

void TouchExplorer::DoubleTapDetector::Clear()
{
    if (mDownEvent != NULL) {
        mDownEvent->Recycle();
        mDownEvent = NULL;
    }
    if (mFirstTapEvent != NULL) {
        mFirstTapEvent->Recycle();
        mFirstTapEvent = NULL;
    }
}

Boolean TouchExplorer::DoubleTapDetector::FirstTapDetected()
{
    Int64 eventTime;
    mFirstTapEvent->GetEventTime(&eventTime);
    return mFirstTapEvent != NULL
            && SystemClock::GetUptimeMillis() - eventTime < mHost->mDoubleTapTimeout;
}

//===============================================================
// TouchExplorer::ExitGestureDetectionModeDelayed
//===============================================================
TouchExplorer::ExitGestureDetectionModeDelayed::ExitGestureDetectionModeDelayed(
    /* [in] */ TouchExplorer* host)
    : mHost(host)
{}

void TouchExplorer::ExitGestureDetectionModeDelayed::Post()
{
    Boolean result;
    mHost->mHandler->PostDelayed(this, EXIT_GESTURE_DETECTION_TIMEOUT, &result);
}

void TouchExplorer::ExitGestureDetectionModeDelayed::Remove()
{
    mHost->mHandler->RemoveCallbacks(this);
}

ECode TouchExplorer::ExitGestureDetectionModeDelayed::Run()
{
    // Announce the end of gesture recognition.
    mHost->SendAccessibilityEvent(IAccessibilityEvent::TYPE_GESTURE_DETECTION_END);
    // Clearing puts is in touch exploration state with a finger already
    // down, so announce the transition to exploration state.
    mHost->SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_START);
    mHost->Clear();
    return NOERROR;
}

//===============================================================
// TouchExplorer::PerformLongPressDelayed
//===============================================================
TouchExplorer::PerformLongPressDelayed::PerformLongPressDelayed(
    /* [in] */ TouchExplorer* host)
    : mPolicyFlags(0)
    , mHost(host)
{}

void TouchExplorer::PerformLongPressDelayed::Post(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMotionEventHelper> helper;
    ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper));
    mEvent = NULL;
    helper->Obtain(prototype, (IMotionEvent**)&mEvent);
    mPolicyFlags = policyFlags;
    AutoPtr<IViewConfigurationHelper> configHelper;
    ASSERT_SUCCEEDED(CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper));
    Int32 timeOut;
    configHelper->GetLongPressTimeout(&timeOut);

    Boolean result;
    mHost->mHandler->PostDelayed(this, timeOut, &result);
}

void TouchExplorer::PerformLongPressDelayed::Remove()
{
    if (IsPending()) {
        mHost->mHandler->RemoveCallbacks(this);

        mHost->Clear();
    }
}

Boolean TouchExplorer::PerformLongPressDelayed::IsPending()
{
    return (mEvent != NULL);
}

ECode TouchExplorer::PerformLongPressDelayed::Run()
{
    // Active pointers should not be zero when running this command.
    if (mHost->mReceivedPointerTracker->GetActivePointerCount() == 0) {
        return NOERROR;
    }

    Int32 clickLocationX;
    Int32 clickLocationY;

    Int32 pointerId, pointerIndex;
    Int32 eventIndex;
    mEvent->GetActionIndex(&eventIndex);
    mEvent->GetPointerId(eventIndex, &pointerId);
    mEvent->FindPointerIndex(pointerId, &pointerIndex);

    AutoPtr<IMotionEvent> lastExploreEvent =
            mHost->mInjectedPointerTracker->GetLastInjectedHoverEventForClick();
    if (lastExploreEvent == NULL) {
        // No last touch explored event but there is accessibility focus in
        // the active window. We click in the middle of the focus bounds.
        AutoPtr<IRect> focusBounds = mHost->mTempRect;
        if (mHost->mAms->GetAccessibilityFocusBoundsInActiveWindow(focusBounds)) {
            focusBounds->GetCenterX(&clickLocationX);
            focusBounds->GetCenterY(&clickLocationY);
        }
        else {
            // Out of luck - do nothing.
            return NOERROR;
        }
    }
    else {
        // If the click is within the active window but not within the
        // accessibility focus bounds we click in the focus center.
        Int32 lastExplorePointerIndex;
        lastExploreEvent->GetActionIndex(&lastExplorePointerIndex);
        lastExploreEvent->GetX(lastExplorePointerIndex, (Float*)&clickLocationX);
        lastExploreEvent->GetY(lastExplorePointerIndex, (Float*)&clickLocationY);
        AutoPtr<IRect> activeWindowBounds = mHost->mTempRect;
        if (mHost->mLastTouchedWindowId == mHost->mAms->GetActiveWindowId()) {
            mHost->mAms->GetActiveWindowBounds(activeWindowBounds);
            Boolean isContain;
            if (activeWindowBounds->Contains(clickLocationX, clickLocationY, &isContain), isContain) {
                AutoPtr<IRect> focusBounds = mHost->mTempRect;
                if (mHost->mAms->GetAccessibilityFocusBoundsInActiveWindow(focusBounds)) {
                    if (focusBounds->Contains(clickLocationX, clickLocationY, &isContain), !isContain) {
                        focusBounds->GetCenterX(&clickLocationX);
                        focusBounds->GetCenterY(&clickLocationY);
                    }
                }
            }
        }
    }

    mHost->mLongPressingPointerId = pointerId;
    Float x, y;
    mEvent->GetX(pointerIndex, &x);
    mEvent->GetY(pointerIndex, &y);
    mHost->mLongPressingPointerDeltaX = (Int32)(x - clickLocationX);
    mHost->mLongPressingPointerDeltaY = (Int32)(y - clickLocationY);

    mHost->SendHoverExitAndTouchExplorationGestureEndIfNeeded(mPolicyFlags);

    mHost->mCurrentState = TouchExplorer::STATE_DELEGATING;
    mHost->SendDownForAllActiveNotInjectedPointers(mEvent, mPolicyFlags);
    Clear();
    return NOERROR;
}

void TouchExplorer::PerformLongPressDelayed::Clear()
{
    if (!IsPending()) {
        return;
    }
    mEvent->Recycle();
    mEvent = NULL;
    mPolicyFlags = 0;
}

//===============================================================
// TouchExplorer::SendAccessibilityEventDelayed
//===============================================================
TouchExplorer::SendAccessibilityEventDelayed::SendAccessibilityEventDelayed(
    /* [in] */ Int32 eventType,
    /* [in] */ Int32 delay,
    /* [in] */ TouchExplorer* host)
    : mEventType(eventType)
    , mDelay(delay)
    , mHost(host)
{}

void TouchExplorer::SendAccessibilityEventDelayed::Remove()
{
    mHost->mHandler->RemoveCallbacks(this);
}

void TouchExplorer::SendAccessibilityEventDelayed::Post()
{
    Boolean result;
    mHost->mHandler->PostDelayed(this, mDelay, &result);
}

Boolean TouchExplorer::SendAccessibilityEventDelayed::IsPending()
{
    Boolean result;
    return mHost->mHandler->HasCallbacks(this, &result);
}

void TouchExplorer::SendAccessibilityEventDelayed::ForceSendAndRemove()
{
    if (IsPending()) {
        Run();
        Remove();
    }
}

ECode TouchExplorer::SendAccessibilityEventDelayed::Run()
{
    mHost->SendAccessibilityEvent(mEventType);
    return NOERROR;
}

//===============================================================
// TouchExplorer
//===============================================================

CAR_INTERFACE_IMPL(TouchExplorer, IEventStreamTransformation);

TouchExplorer::TouchExplorer(
    /* [in] */ IContext* context,
    /* [in] */ CAccessibilityManagerService* service)
    : mDetermineUserIntentTimeout(0)
    , mTapTimeout(0)
    , mDoubleTapTimeout(0)
    , mTouchSlop(0)
    , mDoubleTapSlop(0)
    , mCurrentState(STATE_TOUCH_EXPLORING)
    , mDraggingPointerId(0)
    , mScaledMinPointerDistanceToUseMiddleLocation(0)
    , mScaledGestureDetectionVelocity(0)
    , mAms(service)
    , mContext(context)
    , mPreviousX(0)
    , mPreviousY(0)
    , mLongPressingPointerId(-1)
    , mLongPressingPointerDeltaX(0)
    , mLongPressingPointerDeltaY(0)
    , mLastTouchedWindowId(0)
    , mTouchExplorationInProgress(FALSE)
{
    mTempPointerIds = ArrayOf<Int32>::Alloc(MAX_POINTER_COUNT);

    AutoPtr<IVelocityTrackerHelper> velhelper;
    ASSERT_SUCCEEDED(CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&velhelper));
    ASSERT_SUCCEEDED(velhelper->Obtain((IVelocityTracker**)&mVelocityTracker));

    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));

    mStrokeBuffer = new List<AutoPtr<IGesturePoint> >(100);

    mReceivedPointerTracker = new ReceivedPointerTracker(context, this);
    mInjectedPointerTracker = new InjectedPointerTracker();
    AutoPtr<IViewConfigurationHelper> helper;
    ASSERT_SUCCEEDED(CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper));
    helper->GetTapTimeout(&mTapTimeout);
    helper->GetDoubleTapTimeout(&mDetermineUserIntentTimeout);
    helper->GetDoubleTapTimeout(&mDoubleTapTimeout);
    AutoPtr<IViewConfiguration> config;
    ASSERT_SUCCEEDED(helper->Get(context, (IViewConfiguration**)&config));
    config->GetScaledTouchSlop(&mTouchSlop);
    config->GetScaledDoubleTapSlop(&mDoubleTapSlop);
    AutoPtr<ILooper> looper;
    mContext->GetMainLooper((ILooper**)&looper);
    ASSERT_SUCCEEDED(CHandler::New(looper, (IHandler**)&mHandler));
    mPerformLongPressDelayed = new PerformLongPressDelayed(this);
    mExitGestureDetectionModeDelayed = new ExitGestureDetectionModeDelayed(this);
    AutoPtr<IGestureLibraries> libraries;
    ASSERT_SUCCEEDED(CGestureLibraries::AcquireSingleton((IGestureLibraries**)&libraries));
    ASSERT_SUCCEEDED(libraries->FromRawResource(context, R::raw::accessibility_gestures,
        (IGestureLibrary**)&mGestureLibrary));
    mGestureLibrary->SetOrientationStyle(8);
    mGestureLibrary->SetSequenceType(IGestureStore::SEQUENCE_SENSITIVE);
    Boolean loaded;
    mGestureLibrary->Load(&loaded);
    mSendHoverEnterDelayed = new SendHoverDelayed(IMotionEvent::ACTION_HOVER_ENTER, TRUE, this);
    mSendHoverExitDelayed = new SendHoverDelayed(IMotionEvent::ACTION_HOVER_EXIT, FALSE, this);
    mSendTouchExplorationEndDelayed = new SendAccessibilityEventDelayed(
        IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_END,
        mDetermineUserIntentTimeout, this);
    mSendTouchInteractionEndDelayed = new SendAccessibilityEventDelayed(
        IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END,
        mDetermineUserIntentTimeout, this);
    mDoubleTapDetector = new DoubleTapDetector(this);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    AutoPtr<IDisplayMetrics> metrics;
    r->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    mScaledMinPointerDistanceToUseMiddleLocation =
        (Int32)(MIN_POINTER_DISTANCE_TO_USE_MIDDLE_LOCATION_DIP * density);
    mScaledGestureDetectionVelocity = (Int32)(GESTURE_DETECTION_VELOCITY_DIP * density);
}

TouchExplorer::~TouchExplorer()
{
    mStrokeBuffer = NULL;
}

ECode TouchExplorer::Clear()
{
    // If we have not received an event then we are in initial
    // state. Therefore, there is not need to clean anything.
    AutoPtr<IMotionEvent> event = mReceivedPointerTracker->GetLastReceivedEvent();
    if (event != NULL) {
        Clear(event, IWindowManagerPolicy::FLAG_TRUSTED);
    }
    return NOERROR;
}

ECode TouchExplorer::OnDestroy()
{
    // TODO: Implement
    return NOERROR;
}

void TouchExplorer::Clear(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    switch (mCurrentState) {
        case STATE_TOUCH_EXPLORING:
            // If a touch exploration gesture is in progress send events for its end.
            SendHoverExitAndTouchExplorationGestureEndIfNeeded(policyFlags);
            break;
        case STATE_DRAGGING:
            mDraggingPointerId = INVALID_POINTER_ID;
            // Send exit to all pointers that we have delivered.
            SendUpForInjectedDownPointers(event, policyFlags);
            break;
        case STATE_DELEGATING:
            // Send exit to all pointers that we have delivered.
            SendUpForInjectedDownPointers(event, policyFlags);
            break;
        case STATE_GESTURE_DETECTING:
            // Clear the current stroke.
            mStrokeBuffer->Clear();
            break;
    }

    // Remove all pending callbacks.
    mSendHoverEnterDelayed->Remove();
    mSendHoverExitDelayed->Remove();
    mPerformLongPressDelayed->Remove();
    mExitGestureDetectionModeDelayed->Remove();
    mSendTouchExplorationEndDelayed->Remove();
    mSendTouchInteractionEndDelayed->Remove();
    // Reset the pointer trackers.
    mReceivedPointerTracker->Clear();
    mInjectedPointerTracker->Clear();
    // Clear the double tap detector
    mDoubleTapDetector->Clear();
    // Go to initial state.
    // Clear the long pressing pointer remap data.
    mLongPressingPointerId = -1;
    mLongPressingPointerDeltaX = 0;
    mLongPressingPointerDeltaY = 0;
    mCurrentState = STATE_TOUCH_EXPLORING;
    if (mNext != NULL) {
        mNext->Clear();
    }
    mTouchExplorationInProgress = FALSE;
    mAms->OnTouchInteractionEnd();
}

ECode TouchExplorer::SetNext(
    /* [in] */ IEventStreamTransformation* next)
{
    mNext = next;
    return NOERROR;
}

ECode TouchExplorer::OnMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    if (DEBUG) {
        Slogger::D(LOCAL_LOG_TAG, " >>> Received event: %p, policyFlags=%08x", event, policyFlags);
        Slogger::D(LOCAL_LOG_TAG, " >>> StateSymbolicName:%s", GetStateSymbolicName(mCurrentState).string());
    }

    mReceivedPointerTracker->OnMotionEvent(rawEvent);

    switch(mCurrentState) {
        case STATE_TOUCH_EXPLORING:
            HandleMotionEventStateTouchExploring(event, rawEvent, policyFlags);
            break;
        case STATE_DRAGGING:
            HandleMotionEventStateDragging(event, policyFlags);
            break;
        case STATE_DELEGATING:
            HandleMotionEventStateDelegating(event, policyFlags);
            break;
        case STATE_GESTURE_DETECTING:
            HandleMotionEventGestureDetecting(rawEvent, policyFlags);
            break;
        default:
            Slogger::E(LOCAL_LOG_TAG, "Illegal state: %d", mCurrentState);
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Illegal state: " + mCurrentState);
    }
    return NOERROR;
}

ECode TouchExplorer::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    Int32 eventType;
    event->GetEventType(&eventType);

    // The event for gesture end should be strictly after the
    // last hover exit event.
    if (mSendTouchExplorationEndDelayed->IsPending()
            && eventType == IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT) {
        mSendTouchExplorationEndDelayed->Remove();
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_END);
    }

    // The event for touch interaction end should be strictly after the
    // last hover exit and the touch exploration gesture end events.
    if (mSendTouchInteractionEndDelayed->IsPending()
            && eventType == IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT) {
        mSendTouchInteractionEndDelayed->Remove();
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END);
    }

    // If a new window opens or the accessibility focus moves we no longer
    // want to click/long press on the last touch explored location.
    switch (eventType) {
        case IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED:
        case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED:
            if (mInjectedPointerTracker->mLastInjectedHoverEventForClick != NULL) {
                mInjectedPointerTracker->mLastInjectedHoverEventForClick->Recycle();
                mInjectedPointerTracker->mLastInjectedHoverEventForClick = NULL;
            }
            mLastTouchedWindowId = -1;
            break;
        case IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER:
        case IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT:
            event->GetWindowId(&mLastTouchedWindowId);
            break;
    }
    if (mNext != NULL) {
        mNext->OnAccessibilityEvent(event);
    }
    return NOERROR;
}

ECode TouchExplorer::HandleMotionEventStateTouchExploring(
    /* [in] */ IMotionEvent* event,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<ReceivedPointerTracker> receivedTracker = mReceivedPointerTracker;
    Int32 activePointerCount = receivedTracker->GetActivePointerCount();

    mVelocityTracker->AddMovement(rawEvent);

    mDoubleTapDetector->OnMotionEvent(event, policyFlags);

    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN:
            mAms->OnTouchInteractionStart();
            // Pre-feed the motion events to the gesture detector since we
            // have a distance slop before getting into gesture detection
            // mode and not using the points within this slop significantly
            // decreases the quality of gesture recognition.
            HandleMotionEventGestureDetecting(rawEvent, policyFlags);
            //$FALL-THROUGH$
        case IMotionEvent::ACTION_POINTER_DOWN: {
            switch (activePointerCount) {
                case 0:
                    Slogger::E(LOCAL_LOG_TAG, "The must always be one active pointer in touch exploring state!");
                    return E_ILLEGAL_STATE_EXCEPTION;
                    // throw new IllegalStateException("The must always be one active pointer in"
                    //         + "touch exploring state!");
                case 1: {
                    // If we still have not notified the user for the last
                    // touch, we figure out what to do. If were waiting
                    // we resent the delayed callback and wait again.
                    if (mSendHoverEnterDelayed->IsPending()) {
                        mSendHoverEnterDelayed->Remove();
                        mSendHoverExitDelayed->Remove();
                    }

                    if (mSendTouchExplorationEndDelayed->IsPending()) {
                        mSendTouchExplorationEndDelayed->ForceSendAndRemove();
                    }

                    if (mSendTouchInteractionEndDelayed->IsPending()) {
                        mSendTouchInteractionEndDelayed->ForceSendAndRemove();
                    }

                    // Every pointer that goes down is active until it moves or
                    // another one goes down. Hence, having more than one pointer
                    // down we have already send the interaction start event.
                    Int32 pointerCount;
                    event->GetPointerCount(&pointerCount);
                    if (pointerCount == 1) {
                        SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_START);
                    }

                    mPerformLongPressDelayed->Remove();

                    // If we have the first tap schedule a long press and break
                    // since we do not want to schedule hover enter because
                    // the delayed callback will kick in before the long click.
                    // This would lead to a state transition resulting in long
                    // pressing the item below the double taped area which is
                    // not necessary where accessibility focus is.
                    if (mDoubleTapDetector->FirstTapDetected()) {
                        // We got a tap now post a long press action.
                        mPerformLongPressDelayed->Post(event, policyFlags);
                        break;
                    }
                    if (!mTouchExplorationInProgress) {
                        // Deliver hover enter with a delay to have a chance
                        // to detect what the user is trying to do.
                        Int32 pointerId = receivedTracker->GetPrimaryActivePointerId();
                        Int32 pointerIdBits = (1 << pointerId);
                        mSendHoverEnterDelayed->Post(event, TRUE, pointerIdBits, policyFlags);
                    }
                    break;
                }

                default:
                    /* do nothing - let the code for ACTION_MOVE decide what to do */
                    break;
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Int32 pointerId = receivedTracker->GetPrimaryActivePointerId();
            Int32 pointerIndex;
            event->FindPointerIndex(pointerId, &pointerIndex);
            Int32 pointerIdBits = (1 << pointerId);
            switch (activePointerCount) {
                case 0: {
                    /* do nothing - no active pointers so we swallow the event */
                    break;
                }

                case 1: {
                    // We have not started sending events since we try to
                    // figure out what the user is doing.
                    if (mSendHoverEnterDelayed->IsPending()) {
                        // Pre-feed the motion events to the gesture detector since we
                        // have a distance slop before getting into gesture detection
                        // mode and not using the points within this slop significantly
                        // decreases the quality of gesture recognition.
                        HandleMotionEventGestureDetecting(rawEvent, policyFlags);
                        // It is *important* to use the distance traveled by the pointers
                        // on the screen which may or may not be magnified.
                        Float x, y;
                        rawEvent->GetX(pointerIndex, &x);
                        rawEvent->GetY(pointerIndex, &y);
                        Float deltaX = receivedTracker->GetReceivedPointerDownX(pointerId) - x;
                        Float deltaY = receivedTracker->GetReceivedPointerDownY(pointerId) - y;
                        Double moveDelta = Elastos::Core::Math::Hypot((Double)deltaX, (Double)deltaY);
                        // The user has moved enough for us to decide.
                        if (moveDelta > mDoubleTapSlop) {
                            // Check whether the user is performing a gesture. We
                            // detect gestures if the pointer is moving above a
                            // given velocity.
                            mVelocityTracker->ComputeCurrentVelocity(1000);
                            Float xVelocity;
                            mVelocityTracker->GetXVelocity(pointerId, &xVelocity);
                            Float yVelocity;
                            mVelocityTracker->GetYVelocity(pointerId, &yVelocity);
                            Float maxAbsVelocity = Elastos::Core::Math::Max(
                                    Elastos::Core::Math::Abs(xVelocity), Elastos::Core::Math::Abs(yVelocity));
                            if (maxAbsVelocity > mScaledGestureDetectionVelocity) {
                                // We have to perform gesture detection, so
                                // clear the current state and try to detect.
                                mCurrentState = STATE_GESTURE_DETECTING;
                                mVelocityTracker->Clear();
                                mSendHoverEnterDelayed->Remove();
                                mSendHoverExitDelayed->Remove();
                                mPerformLongPressDelayed->Remove();
                                mExitGestureDetectionModeDelayed->Post();
                                // Send accessibility event to announce the start
                                // of gesture recognition.
                                SendAccessibilityEvent(
                                        IAccessibilityEvent::TYPE_GESTURE_DETECTION_START);
                            }
                            else {
                                // We have just decided that the user is touch,
                                // exploring so start sending events.
                                mSendHoverEnterDelayed->ForceSendAndRemove();
                                mSendHoverExitDelayed->Remove();
                                mPerformLongPressDelayed->Remove();
                                SendMotionEvent(event, IMotionEvent::ACTION_HOVER_MOVE,
                                        pointerIdBits, policyFlags);
                            }
                            break;
                        }
                    }
                    else {
                        // Cancel the long press if pending and the user
                        // moved more than the slop.
                        if (mPerformLongPressDelayed->IsPending()) {
                            Float x, y;
                            rawEvent->GetX(pointerIndex, &x);
                            rawEvent->GetY(pointerIndex, &y);
                            Float deltaX =
                                    receivedTracker->GetReceivedPointerDownX(pointerId) - x;
                            Float deltaY =
                                    receivedTracker->GetReceivedPointerDownY(pointerId) - y;
                            Double moveDelta = Elastos::Core::Math::Hypot((Double)deltaX, (Double)deltaY);
                            // The user has moved enough for us to decide.
                            if (moveDelta > mTouchSlop) {
                                mPerformLongPressDelayed->Remove();
                            }
                        }
                        // The user is wither double tapping or performing long
                        // press so do not send move events yet.
                        if (mDoubleTapDetector->FirstTapDetected()) {
                            break;
                        }
                        SendTouchExplorationGestureStartAndHoverEnterIfNeeded(policyFlags);
                        SendMotionEvent(event, IMotionEvent::ACTION_HOVER_MOVE, pointerIdBits,
                            policyFlags);
                    }
                    break;
                }
                case 2: {
                    // More than one pointer so the user is not touch exploring
                    // and now we have to decide whether to delegate or drag.
                    if (mSendHoverEnterDelayed->IsPending()) {
                        // We have not started sending events so cancel
                        // scheduled sending events.
                        mSendHoverEnterDelayed->Remove();
                        mSendHoverExitDelayed->Remove();
                        mPerformLongPressDelayed->Remove();
                    }
                    else {
                        mPerformLongPressDelayed->Remove();
                        // If the user is touch exploring the second pointer may be
                        // performing a double tap to activate an item without need
                        // for the user to lift his exploring finger.
                        // It is *important* to use the distance traveled by the pointers
                        // on the screen which may or may not be magnified.
                        Float x, y;
                        rawEvent->GetX(pointerIndex, &x);
                        rawEvent->GetY(pointerIndex, &y);
                        Float deltaX = receivedTracker->GetReceivedPointerDownX(pointerId) - x;
                        Float deltaY = receivedTracker->GetReceivedPointerDownY(pointerId) - y;
                        Double moveDelta = Elastos::Core::Math::Hypot(deltaX, deltaY);
                        if (moveDelta < mDoubleTapSlop) {
                            break;
                        }
                        // We are sending events so send exit and gesture
                        // end since we transition to another state.
                        SendHoverExitAndTouchExplorationGestureEndIfNeeded(policyFlags);
                    }

                    // We know that a new state transition is to happen and the
                    // new state will not be gesture recognition, so clear the
                    // stashed gesture strokes.
                    mStrokeBuffer->Clear();

                    if (IsDraggingGesture(event)) {
                        // Two pointers moving in the same direction within
                        // a given distance perform a drag.
                        mCurrentState = STATE_DRAGGING;
                        mDraggingPointerId = pointerId;
                        event->SetEdgeFlags(receivedTracker->GetLastReceivedDownEdgeFlags());
                        SendMotionEvent(event, IMotionEvent::ACTION_DOWN, pointerIdBits,
                                policyFlags);
                    }
                    else {
                        // Two pointers moving arbitrary are delegated to the view hierarchy.
                        mCurrentState = STATE_DELEGATING;
                        SendDownForAllActiveNotInjectedPointers(event, policyFlags);
                    }
                    mVelocityTracker->Clear();
                    break;
                }

                default: {
                    // More than one pointer so the user is not touch exploring
                    // and now we have to decide whether to delegate or drag.
                    if (mSendHoverEnterDelayed->IsPending()) {
                        // We have not started sending events so cancel
                        // scheduled sending events.
                        mSendHoverEnterDelayed->Remove();
                        mSendHoverExitDelayed->Remove();
                        mPerformLongPressDelayed->Remove();
                    }
                    else {
                        mPerformLongPressDelayed->Remove();
                        // We are sending events so send exit and gesture
                        // end since we transition to another state.
                        SendHoverExitAndTouchExplorationGestureEndIfNeeded(policyFlags);
                    }

                    // More than two pointers are delegated to the view hierarchy.
                    mCurrentState = STATE_DELEGATING;
                    SendDownForAllActiveNotInjectedPointers(event, policyFlags);
                    mVelocityTracker->Clear();
                }
            }
            break;
        }

        case IMotionEvent::ACTION_UP:
            mAms->OnTouchInteractionEnd();
            // We know that we do not need the pre-fed gesture points are not
            // needed anymore since the last pointer just went up.
            mStrokeBuffer->Clear();
            //$FALL-THROUGH$
        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 pointerId = receivedTracker->GetLastReceivedUpPointerId();
            Int32 pointerIdBits = (1 << pointerId);
            switch (activePointerCount) {
                case 0: {
                    // If the pointer that went up was not active we have nothing to do.
                    if (!receivedTracker->WasLastReceivedUpPointerActive()) {
                        break;
                    }

                    mPerformLongPressDelayed->Remove();

                    // If we have not delivered the enter schedule exit.
                    if (mSendHoverEnterDelayed->IsPending()) {
                        mSendHoverExitDelayed->Post(event, FALSE, pointerIdBits, policyFlags);
                    }
                    else {
                        // The user is touch exploring so we send events for end.
                        SendHoverExitAndTouchExplorationGestureEndIfNeeded(policyFlags);
                    }

                    if (!mSendTouchInteractionEndDelayed->IsPending()) {
                        mSendTouchInteractionEndDelayed->Post();
                    }
                    break;
                }
            }
            mVelocityTracker->Clear();
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            Clear(event, policyFlags);
            break;
        }
    }
    return NOERROR;
}

ECode TouchExplorer::HandleMotionEventStateDragging(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Int32 pointerIdBits = (1 << mDraggingPointerId);
    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            Slogger::E(LOCAL_LOG_TAG, "Dragging state can be reached only if two pointers are already down");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Dragging state can be reached only if two "
            //         + "pointers are already down");
        }

        case IMotionEvent::ACTION_POINTER_DOWN: {
            // We are in dragging state so we have two pointers and another one
            // goes down => delegate the three pointers to the view hierarchy
            mCurrentState = STATE_DELEGATING;
            if (mDraggingPointerId != INVALID_POINTER_ID) {
                SendMotionEvent(event, IMotionEvent::ACTION_UP, pointerIdBits, policyFlags);
            }
            SendDownForAllActiveNotInjectedPointers(event, policyFlags);
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Int32 activePointerCount = mReceivedPointerTracker->GetActivePointerCount();
            switch (activePointerCount) {
                case 1: {
                    // do nothing
                    break;
                }

                case 2: {
                    if (IsDraggingGesture(event)) {
                        // If the dragging pointer are closer that a given distance we
                        // use the location of the primary one. Otherwise, we take the
                        // middle between the pointers.
                        AutoPtr< ArrayOf<Int32> > pointerIds = mTempPointerIds;
                        mReceivedPointerTracker->PopulateActivePointerIds(pointerIds);

                        Int32 firstPtrIndex, secondPtrIndex;
                        event->FindPointerIndex((*pointerIds)[0], &firstPtrIndex);
                        event->FindPointerIndex((*pointerIds)[1], &secondPtrIndex);

                        Float firstPtrX, firstPtrY, secondPtrX, secondPtrY;
                        event->GetX(firstPtrIndex, &firstPtrX);
                        event->GetY(firstPtrIndex, &firstPtrY);
                        event->GetX(secondPtrIndex, &secondPtrX);
                        event->GetY(secondPtrIndex, &secondPtrY);

                        Float deltaX = firstPtrX - secondPtrX;
                        Float deltaY = firstPtrY - secondPtrY;
                        Double distance = Elastos::Core::Math::Hypot((Double)deltaX, (Double)deltaY);

                        if (distance > mScaledMinPointerDistanceToUseMiddleLocation) {
                            event->SetLocation(deltaX / 2, deltaY / 2);
                        }

                        // If still dragging send a drag event.
                        SendMotionEvent(event, IMotionEvent::ACTION_MOVE, pointerIdBits,
                                policyFlags);
                    }
                    else {
                        // The two pointers are moving either in different directions or
                        // no close enough => delegate the gesture to the view hierarchy.
                        mCurrentState = STATE_DELEGATING;
                        // Send an event to the end of the drag gesture.
                        SendMotionEvent(event, IMotionEvent::ACTION_UP, pointerIdBits,
                                policyFlags);
                        // Deliver all active pointers to the view hierarchy.
                        SendDownForAllActiveNotInjectedPointers(event, policyFlags);
                    }
                    break;
                }

                default: {
                    mCurrentState = STATE_DELEGATING;
                    // Send an event to the end of the drag gesture.
                    SendMotionEvent(event, IMotionEvent::ACTION_UP, pointerIdBits,
                            policyFlags);
                    // Deliver all active pointers to the view hierarchy.
                    SendDownForAllActiveNotInjectedPointers(event, policyFlags);
                }
            }
            break;
        }

        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 pointerId;
            Int32 actionIndex;
            event->GetActionIndex(&actionIndex);
            event->GetPointerId(actionIndex, &pointerId);
            if (pointerId == mDraggingPointerId) {
                mDraggingPointerId = INVALID_POINTER_ID;
                // Send an event to the end of the drag gesture.
                SendMotionEvent(event, IMotionEvent::ACTION_UP, pointerIdBits, policyFlags);
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            mAms->OnTouchInteractionEnd();
            // Announce the end of a new touch interaction.
            SendAccessibilityEvent(
                    IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END);
            Int32 pointerId;
            Int32 actionIndex;
            event->GetActionIndex(&actionIndex);
            event->GetPointerId(actionIndex, &pointerId);
            if (pointerId == mDraggingPointerId) {
                mDraggingPointerId = INVALID_POINTER_ID;
                // Send an event to the end of the drag gesture.
                SendMotionEvent(event, IMotionEvent::ACTION_UP, pointerIdBits, policyFlags);
            }
            mCurrentState = STATE_TOUCH_EXPLORING;
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            Clear(event, policyFlags);
            break;
        }
    }
    return NOERROR;
}

ECode TouchExplorer::HandleMotionEventStateDelegating(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            Slogger::E(LOCAL_LOG_TAG, "Delegating state can only be reached if there is at least one pointer down!");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Delegating state can only be reached if "
            //         + "there is at least one pointer down!");
        }

        case IMotionEvent::ACTION_MOVE: {
            // Check  whether some other pointer became active because they have moved
            // a given distance and if such exist send them to the view hierarchy
            Int32 notInjectedCount = GetNotInjectedActivePointerCount(
                    mReceivedPointerTracker, mInjectedPointerTracker);
            if (notInjectedCount > 0) {
                AutoPtr<IMotionEventHelper> helper;
                ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper));
                AutoPtr<IMotionEvent> prototype;
                helper->Obtain(event, (IMotionEvent**)&prototype);
                SendDownForAllActiveNotInjectedPointers(prototype, policyFlags);
            }
            break;
        }

        case IMotionEvent::ACTION_UP:
            // Announce the end of a new touch interaction.
            SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END);
            //$FALL-THROUGH$
        case IMotionEvent::ACTION_POINTER_UP: {
            mAms->OnTouchInteractionEnd();
            mLongPressingPointerId = -1;
            mLongPressingPointerDeltaX = 0;
            mLongPressingPointerDeltaY = 0;
            // No active pointers => go to initial state.
            if (mReceivedPointerTracker->GetActivePointerCount() == 0) {
                mCurrentState = STATE_TOUCH_EXPLORING;
            }
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            Clear(event, policyFlags);
            break;
        }
    }

    // Deliver the event striping out inactive pointers.
    SendMotionEventStripInactivePointers(event, policyFlags);
    return NOERROR;
}

void TouchExplorer::HandleMotionEventGestureDetecting(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    Float x, y;
    event->GetX(&x);
    event->GetY(&y);
    Int64 eventTime;
    event->GetEventTime(&eventTime);

    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            mPreviousX = x;
            mPreviousY = y;
            AutoPtr<IGesturePoint> point;
            CGesturePoint::New(x, y, eventTime, (IGesturePoint**)&point);
            mStrokeBuffer->PushBack(point);
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Float dX = Elastos::Core::Math::Abs(x - mPreviousX);
            Float dY = Elastos::Core::Math::Abs(y - mPreviousY);
            if (dX >= TOUCH_TOLERANCE || dY >= TOUCH_TOLERANCE) {
                mPreviousX = x;
                mPreviousY = y;
                AutoPtr<IGesturePoint> point;
                CGesturePoint::New(x, y, eventTime, (IGesturePoint**)&point);
                mStrokeBuffer->PushBack(point);
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            mAms->OnTouchInteractionEnd();
            // Announce the end of gesture recognition.
            SendAccessibilityEvent(IAccessibilityEvent::TYPE_GESTURE_DETECTION_END);
            // Announce the end of a new touch interaction.
            SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END);
            AutoPtr<IGesturePoint> point;
            CGesturePoint::New(x, y, eventTime, (IGesturePoint**)&point);
            mStrokeBuffer->PushBack(point);

            AutoPtr<IObjectContainer> container;
            CObjectContainer::New((IObjectContainer**)&container);
            List<AutoPtr<IGesturePoint> >::Iterator it = mStrokeBuffer->Begin();
            for (; it != mStrokeBuffer->End(); ++it) {
                container->Add((IInterface*)*it);
            }
            AutoPtr<IGestureStroke> stroke;
            CGestureStroke::New(container, (IGestureStroke**)&stroke);
            AutoPtr<IGesture> gesture;
            ASSERT_SUCCEEDED(CGesture::New((IGesture**)&gesture));
            gesture->AddStroke(stroke);

            AutoPtr<IArrayList> predictions;
            mGestureLibrary->Recognize(gesture, (IArrayList**)&predictions);
            Boolean empty = FALSE;
            predictions->IsEmpty(&empty);
            if (!empty) {
                AutoPtr<IInterface> item;
                predictions->Get(0, (IInterface**)&item);
                AutoPtr<IPrediction> bestPrediction = IPrediction::Probe(item);
                Double score;
                bestPrediction->GetScore(&score);
                if (score >= MIN_PREDICTION_SCORE) {
                    String name;
                    bestPrediction->GetName(&name);
                    if (DEBUG) {
                        Slogger::I(LOCAL_LOG_TAG, "gesture: %s score: ", name.string(), score);
                    }
                    // try {
                    const Int32 gestureId = StringUtils::ParseInt32(name);
                    mAms->OnGesture(gestureId);
                    // } catch (NumberFormatException nfe) {
                    // Slog.w(LOG_TAG, "Non numeric gesture id:" + bestPrediction.name);
                    // }
                }
            }

            mStrokeBuffer->Clear();
            mExitGestureDetectionModeDelayed->Remove();
            mCurrentState = STATE_TOUCH_EXPLORING;
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            Clear(event, policyFlags);
            break;
        }
    }
}

void TouchExplorer::SendAccessibilityEvent(
    /* [in] */ Int32 type)
{
    AutoPtr<IAccessibilityManager> accessibilityManager;
    AutoPtr<IAccessibilityManagerHelper> helper;
    ASSERT_SUCCEEDED(CAccessibilityManagerHelper::AcquireSingleton(
        (IAccessibilityManagerHelper**)&helper));
    ASSERT_SUCCEEDED(helper->GetInstance(mContext, (IAccessibilityManager**)&accessibilityManager));
    Boolean isEnabled;
    if (accessibilityManager->IsEnabled(&isEnabled), isEnabled) {
        AutoPtr<IAccessibilityEvent> event;
        AutoPtr<IAccessibilityEventHelper> helper;
        ASSERT_SUCCEEDED(CAccessibilityEventHelper::AcquireSingleton(
                (IAccessibilityEventHelper**)&helper));
        helper->Obtain(type, (IAccessibilityEvent**)&event);
        accessibilityManager->SendAccessibilityEvent(event);
        switch (type) {
            case IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_START: {
                mTouchExplorationInProgress = TRUE;
            }
            break;
            case IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_END: {
                mTouchExplorationInProgress = FALSE;
            }
            break;
        }
    }
}

void TouchExplorer::SendDownForAllActiveNotInjectedPointers(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<ReceivedPointerTracker> receivedPointers = mReceivedPointerTracker;
    AutoPtr<InjectedPointerTracker> injectedPointers = mInjectedPointerTracker;
    Int32 pointerIdBits = 0;
    Int32 pointerCount;
    prototype->GetPointerCount(&pointerCount);

    // Find which pointers are already injected.
    for (Int32 i = 0; i < pointerCount; i++) {
        Int32 pointerId;
        prototype->GetPointerId(i, &pointerId);
        if (injectedPointers->IsInjectedPointerDown(pointerId)) {
            pointerIdBits |= (1 << pointerId);
        }
    }

    // Inject the active and not injected pointers.
    for (Int32 i = 0; i < pointerCount; i++) {
        Int32 pointerId;
        prototype->GetPointerId(i, &pointerId);
        // Skip inactive pointers.
        if (!receivedPointers->IsActivePointer(pointerId)) {
            continue;
        }
        // Do not send event for already delivered pointers.
        if (injectedPointers->IsInjectedPointerDown(pointerId)) {
            continue;
        }
        pointerIdBits |= (1 << pointerId);
        Int32 action = ComputeInjectionAction(IMotionEvent::ACTION_DOWN, i);
        SendMotionEvent(prototype, action, pointerIdBits, policyFlags);
    }
}

void TouchExplorer::SendHoverExitAndTouchExplorationGestureEndIfNeeded(
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMotionEvent> event = mInjectedPointerTracker->GetLastInjectedHoverEvent();
    Int32 actionMasked;
    if (event != NULL &&
            (event->GetActionMasked(&actionMasked), actionMasked != IMotionEvent::ACTION_HOVER_EXIT)) {
        Int32 pointerIdBits;
        event->GetPointerIdBits(&pointerIdBits);
        if (!mSendTouchExplorationEndDelayed->IsPending()) {
            mSendTouchExplorationEndDelayed->Post();
        }
        SendMotionEvent(event, IMotionEvent::ACTION_HOVER_EXIT, pointerIdBits, policyFlags);
    }
}

void TouchExplorer::SendTouchExplorationGestureStartAndHoverEnterIfNeeded(
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMotionEvent> event = mInjectedPointerTracker->GetLastInjectedHoverEvent();
    Int32 actionMasked;
    if (event != NULL &&
            (event->GetActionMasked(&actionMasked), actionMasked != IMotionEvent::ACTION_HOVER_EXIT)) {
        Int32 pointerIdBits;
        event->GetPointerIdBits(&pointerIdBits);
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_START);
        SendMotionEvent(event, IMotionEvent::ACTION_HOVER_ENTER, pointerIdBits, policyFlags);
    }
}

void TouchExplorer::SendUpForInjectedDownPointers(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<InjectedPointerTracker> injectedTracked = mInjectedPointerTracker;
    Int32 pointerIdBits = 0;
    Int32 pointerCount;
    prototype->GetPointerCount(&pointerCount);
    for (Int32 i = 0; i < pointerCount; i++) {
        Int32 pointerId;
        prototype->GetPointerId(i, &pointerId);
        // Skip non injected down pointers.
        if (!injectedTracked->IsInjectedPointerDown(pointerId)) {
            continue;
        }
        pointerIdBits |= (1 << pointerId);
        Int32 action = ComputeInjectionAction(IMotionEvent::ACTION_UP, i);
        SendMotionEvent(prototype, action, pointerIdBits, policyFlags);
    }
}

void TouchExplorer::SendMotionEventStripInactivePointers(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<ReceivedPointerTracker> receivedTracker = mReceivedPointerTracker;

    // All pointers active therefore we just inject the event as is.
    Int32 pointerCount = 0;
    prototype->GetPointerCount(&pointerCount);
    if (pointerCount == receivedTracker->GetActivePointerCount()) {
        Int32 action;
        prototype->GetAction(&action);
        SendMotionEvent(prototype, action, ALL_POINTER_ID_BITS, policyFlags);
        return;
    }

    // No active pointers and the one that just went up was not
    // active, therefore we have nothing to do.
    if (receivedTracker->GetActivePointerCount() == 0
            && !receivedTracker->WasLastReceivedUpPointerActive()) {
        return;
    }

    // If the action pointer going up/down is not active we have nothing to do.
    // However, for moves we keep going to report moves of active pointers.
    Int32 actionMasked, actionPointerId;
    prototype->GetActionMasked(&actionMasked);
    Int32 actionIndex;
    prototype->GetActionIndex(&actionIndex);
    prototype->GetPointerId(actionIndex, &actionPointerId);
    if (actionMasked != IMotionEvent::ACTION_MOVE) {
        if (!receivedTracker->IsActiveOrWasLastActiveUpPointer(actionPointerId)) {
            return;
        }
    }

    // If the pointer is active or the pointer that just went up
    // was active we keep the pointer data in the event.
    Int32 pointerIdBits = 0;
    pointerCount = 0;
    prototype->GetPointerCount(&pointerCount);
    for (Int32 pointerIndex = 0; pointerIndex < pointerCount; pointerIndex++) {
        Int32 pointerId;
        prototype->GetPointerId(pointerIndex, &pointerId);
        if (receivedTracker->IsActiveOrWasLastActiveUpPointer(pointerId)) {
            pointerIdBits |= (1 << pointerId);
        }
    }
    Int32 action;
    prototype->GetAction(&action);
    SendMotionEvent(prototype, action, pointerIdBits, policyFlags);
}

void TouchExplorer::SendActionDownAndUp(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    // Tap with the pointer that last explored - we may have inactive pointers.
    Int32 pointerId, actionIndex;
    prototype->GetActionIndex(&actionIndex);
    prototype->GetPointerId(actionIndex, &pointerId);
    Int32 pointerIdBits = (1 << pointerId);
    SendMotionEvent(prototype, IMotionEvent::ACTION_DOWN, pointerIdBits, policyFlags);
    SendMotionEvent(prototype, IMotionEvent::ACTION_UP, pointerIdBits, policyFlags);
}

void TouchExplorer::SendMotionEvent(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 action,
    /* [in] */ Int32 pointerIdBits,
    /* [in] */ Int32 policyFlags)
{
    prototype->SetAction(action);

    AutoPtr<IMotionEvent> event = NULL;
    if (pointerIdBits == ALL_POINTER_ID_BITS) {
        event = prototype;
    }
    else {
        prototype->Split(pointerIdBits, (IMotionEvent**)&event);
    }
    if (action == IMotionEvent::ACTION_DOWN) {
        Int64 eventTime;
        event->GetEventTime(&eventTime);
        event->SetDownTime(eventTime);
    }
    else {
        event->SetDownTime(mInjectedPointerTracker->GetLastInjectedDownEventTime());
    }

    // If the user is long pressing but the long pressing pointer
    // was not exactly over the accessibility focused item we need
    // to remap the location of that pointer so the user does not
    // have to explicitly touch explore something to be able to
    // long press it, or even worse to avoid the user long pressing
    // on the wrong item since click and long press behave differently.
    if (mLongPressingPointerId >= 0) {
        Int32 remappedIndex, pointerCount;
        event->FindPointerIndex(mLongPressingPointerId, &remappedIndex);
        event->GetPointerCount(&pointerCount);

        AutoPtr< ArrayOf<IPointerProperties*> > props = ArrayOf<IPointerProperties*>::Alloc(pointerCount);
        AutoPtr< ArrayOf<IPointerCoords*> > coords = ArrayOf<IPointerCoords*>::Alloc(pointerCount);
        Float x, y;
        for (Int32 i = 0; i < pointerCount; i++) {
            AutoPtr<IPointerProperties> prop;
            CPointerProperties::New((IPointerProperties**)&prop);
            props->Set(i, prop);
            AutoPtr<IPointerCoords> coord;
            CPointerCoords::New((IPointerCoords**)&coord);
            coords->Set(i, coord);

            event->GetPointerProperties(i, (*props)[i]);
            event->GetPointerCoords(i, coord);
            if (i == remappedIndex) {
                coord->GetX(&x);
                coord->GetY(&y);
                x -= mLongPressingPointerDeltaX;
                y -= mLongPressingPointerDeltaY;
                coord->SetX(x);
                coord->SetY(y);
            }
        }

        Int64 downTime, eventTime;
        event->GetDownTime(&downTime);
        event->GetEventTime(&eventTime);
        Int32 action, pCount, metaState, buttonState, deviceId, edgeFlags, source, flags;
        event->GetAction(&action);
        event->GetPointerCount(&pCount);
        event->GetMetaState(&metaState);
        event->GetButtonState(&buttonState);
        event->GetDeviceId(&deviceId);
        event->GetEdgeFlags(&edgeFlags);
        event->GetSource(&source);
        event->GetFlags(&flags);
        AutoPtr<IMotionEventHelper> eventHelper;
        ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&eventHelper));
        AutoPtr<IMotionEvent> remapped;
        eventHelper->Obtain(downTime, eventTime, action, pCount,
            props, coords, metaState, buttonState, 1.0f, 1.0f, deviceId, edgeFlags,
            source, flags, (IMotionEvent**)&remapped);
        if (event.Get() != prototype) {
            event->Recycle();
        }
        event = remapped;
    }

    // if (DEBUG) {
    //     Slog.d(LOG_TAG, "Injecting event: " + event + ", policyFlags=0x"
    //             + Integer.toHexString(policyFlags));
    // }

    // Make sure that the user will see the event.
    policyFlags |= IWindowManagerPolicy::FLAG_PASS_TO_USER;
    if (mNext != NULL) {
        // TODO: For now pass null for the raw event since the touch
        //       explorer is the last event transformation and it does
        //       not care about the raw event.
        mNext->OnMotionEvent(event, NULL, policyFlags);
    }

    mInjectedPointerTracker->OnMotionEvent(event);

    if (event.Get() != prototype) {
        event->Recycle();
    }
}

Int32 TouchExplorer::ComputeInjectionAction(
    /* [in] */ Int32 actionMasked,
    /* [in] */ Int32 pointerIndex)
{
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN:
        case IMotionEvent::ACTION_POINTER_DOWN: {
            AutoPtr<InjectedPointerTracker> injectedTracker = mInjectedPointerTracker;
            // Compute the action based on how many down pointers are injected.
            if (injectedTracker->GetInjectedPointerDownCount() == 0) {
                return IMotionEvent::ACTION_DOWN;
            }
            else {
                return (pointerIndex << IMotionEvent::ACTION_POINTER_INDEX_SHIFT)
                    | IMotionEvent::ACTION_POINTER_DOWN;
            }
        }
        case IMotionEvent::ACTION_POINTER_UP: {
            AutoPtr<InjectedPointerTracker> injectedTracker = mInjectedPointerTracker;
            // Compute the action based on how many down pointers are injected.
            if (injectedTracker->GetInjectedPointerDownCount() == 1) {
                return IMotionEvent::ACTION_UP;
            }
            else {
                return (pointerIndex << IMotionEvent::ACTION_POINTER_INDEX_SHIFT)
                    | IMotionEvent::ACTION_POINTER_UP;
            }
        }
        default:
            return actionMasked;
    }
}

Boolean TouchExplorer::IsDraggingGesture(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<ReceivedPointerTracker> receivedTracker = mReceivedPointerTracker;
    AutoPtr< ArrayOf<Int32> > pointerIds = mTempPointerIds;
    receivedTracker->PopulateActivePointerIds(pointerIds);

    Int32 firstPtrIndex, secondPtrIndex;
    event->FindPointerIndex((*pointerIds)[0], &firstPtrIndex);
    event->FindPointerIndex((*pointerIds)[1], &secondPtrIndex);

    Float firstPtrX, firstPtrY, secondPtrX, secondPtrY;
    event->GetX(firstPtrIndex, &firstPtrX);
    event->GetY(firstPtrIndex, &firstPtrY);
    event->GetX(secondPtrIndex, &secondPtrX);
    event->GetY(secondPtrIndex, &secondPtrY);

    Float firstPtrDownX = receivedTracker->GetReceivedPointerDownX(firstPtrIndex);
    Float firstPtrDownY = receivedTracker->GetReceivedPointerDownY(firstPtrIndex);
    Float secondPtrDownX = receivedTracker->GetReceivedPointerDownX(secondPtrIndex);
    Float secondPtrDownY = receivedTracker->GetReceivedPointerDownY(secondPtrIndex);

    return GestureUtils::IsDraggingGesture(firstPtrDownX, firstPtrDownY, secondPtrDownX,
        secondPtrDownY, firstPtrX, firstPtrY, secondPtrX, secondPtrY,
        MAX_DRAGGING_ANGLE_COS);
}

String TouchExplorer::GetStateSymbolicName(
    /* [in] */ Int32 state)
{
    switch (state) {
        case STATE_TOUCH_EXPLORING:
            return String("STATE_TOUCH_EXPLORING");
        case STATE_DRAGGING:
            return String("STATE_DRAGGING");
        case STATE_DELEGATING:
            return String("STATE_DELEGATING");
        case STATE_GESTURE_DETECTING:
            return String("STATE_GESTURE_DETECTING");
        default:
            return String("Unknown state: ") + StringUtils::Int32ToString(state);
            // throw new IllegalArgumentException("Unknown state: " + state);
    }
}

Int32 TouchExplorer::GetNotInjectedActivePointerCount(
    /* [in] */ ReceivedPointerTracker* receivedTracker,
    /* [in] */ InjectedPointerTracker* injectedTracker)
{
    Int32 pointerState = receivedTracker->GetActivePointers()
            & ~injectedTracker->GetInjectedPointersDown();

    return Elastos::Core::Math::BitCount(pointerState);
}

String TouchExplorer::ToString()
{
    return LOCAL_LOG_TAG;
}


} // Accessibility
} // Server
} // Droid
} // Elastos
