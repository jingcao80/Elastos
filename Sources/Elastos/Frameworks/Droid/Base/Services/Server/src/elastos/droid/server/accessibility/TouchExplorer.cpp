
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Gesture.h>
#include "elastos/droid/server/accessibility/AccessibilityManagerService.h"
#include "elastos/droid/server/accessibility/TouchExplorer.h"
#include "elastos/droid/server/accessibility/GestureUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Gesture::IGestureLibraries;
using Elastos::Droid::Gesture::CGestureLibraries;
using Elastos::Droid::Gesture::IGestureStore;
using Elastos::Droid::Gesture::CGesturePoint;
using Elastos::Droid::Gesture::IGesturePoint;
using Elastos::Droid::Gesture::CGesture;
using Elastos::Droid::Gesture::IGesture;
using Elastos::Droid::Gesture::IGestureStroke;
using Elastos::Droid::Gesture::CGestureStroke;
using Elastos::Droid::Gesture::IPrediction;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Server::Accessibility::EIID_IEventStreamTransformation;
using Elastos::Droid::Utility::IDisplayMetrics;
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
using Elastos::Droid::View::CPointerCoordsHelper;
using Elastos::Droid::View::IPointerCoordsHelper;
using Elastos::Droid::View::IPointerPropertiesHelper;
using Elastos::Droid::View::CPointerPropertiesHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

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

const String TouchExplorer::SendHoverEnterAndMoveDelayed::LOG_TAG_SEND_HOVER_DELAYED =
        String("SendHoverEnterAndMoveDelayed");

const String TouchExplorer::SendHoverExitDelayed::LOG_TAG_SEND_HOVER_DELAYED =
        String("SendHoverExitDelayed");

const String TouchExplorer::InjectedPointerTracker::LOG_TAG_INJECTED_POINTER_TRACKER =
        String("InjectedPointerTracker");

const String TouchExplorer::ReceivedPointerTracker::LOG_TAG_RECEIVED_POINTER_TRACKER =
        String("ReceivedPointerTracker");

//===============================================================================
//                  TouchExplorer::SendHoverEnterAndMoveDelayed
//===============================================================================

TouchExplorer::SendHoverEnterAndMoveDelayed::SendHoverEnterAndMoveDelayed(
    /* [in] */ TouchExplorer* host)
    : mPointerIdBits(0)
    , mPolicyFlags(0)
    , mHost(host)
{}

TouchExplorer::SendHoverEnterAndMoveDelayed::~SendHoverEnterAndMoveDelayed()
{}

void TouchExplorer::SendHoverEnterAndMoveDelayed::Post(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Boolean touchExplorationInProgress,
    /* [in] */ Int32 pointerIdBits,
    /* [in] */ Int32 policyFlags)
{
    Cancel();
    AddEvent(event);
    mPointerIdBits = pointerIdBits;
    mPolicyFlags = policyFlags;

    Boolean result;
    mHost->mHandler->PostDelayed(this, mHost->mDetermineUserIntentTimeout, &result);
}

void TouchExplorer::SendHoverEnterAndMoveDelayed::AddEvent(
    /* [in] */ IMotionEvent* event)
{
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> motionEvent;
    helper->Obtain(event, (IMotionEvent**)&motionEvent);
    mEvents->Add(motionEvent);
}

void TouchExplorer::SendHoverEnterAndMoveDelayed::Cancel()
{
    if (IsPending()) {
        mHost->mHandler->RemoveCallbacks(this);
        Clear();
    }
}

Boolean TouchExplorer::SendHoverEnterAndMoveDelayed::IsPending()
{
    Boolean res;
    mHost->mHandler->HasCallbacks(this, &res);
    return res;
}

void TouchExplorer::SendHoverEnterAndMoveDelayed::Clear()
{
    mPointerIdBits = -1;
    mPolicyFlags = 0;
    Int32 eventCount;
    mEvents->GetSize(&eventCount);
    for (Int32 i = eventCount -1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mEvents->Remove(i, (IInterface**)&obj);
        IInputEvent::Probe(obj)->Recycle();
    }
}

void TouchExplorer::SendHoverEnterAndMoveDelayed::ForceSendAndRemove()
{
    if (IsPending()) {
        Run();
        Cancel();
    }
}

ECode TouchExplorer::SendHoverEnterAndMoveDelayed::Run()
{
    // Send an accessibility event to announce the touch exploration start.
    mHost->SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_START);

    Boolean res;
    if (mEvents->IsEmpty(&res), !res) {
        // Deliver a down event.
        AutoPtr<IInterface> obj;
        mEvents->Get(0, (IInterface**)&obj);

        mHost->SendMotionEvent(IMotionEvent::Probe(obj), IMotionEvent::ACTION_HOVER_ENTER,
                mPointerIdBits, mPolicyFlags);
        if (DEBUG) {
            Slogger::D(LOG_TAG_SEND_HOVER_DELAYED,
                    "Injecting motion event: ACTION_HOVER_ENTER");
        }

        // Deliver move events.
        Int32 eventCount;
        mEvents->GetSize(&eventCount);
        for (Int32 i = 1; i < eventCount; i++) {
            AutoPtr<IInterface> obj;
            mEvents->Get(i, (IInterface**)&obj);
            mHost->SendMotionEvent(IMotionEvent::Probe(obj), IMotionEvent::ACTION_HOVER_MOVE,
                    mPointerIdBits, mPolicyFlags);
            if (DEBUG) {
                Slogger::D(LOG_TAG_SEND_HOVER_DELAYED,
                        "Injecting motion event: ACTION_HOVER_MOVE");
            }
        }
    }
    Clear();
    return NOERROR;
}

//===============================================================================
//                  TouchExplorer::SendHoverExitDelayed
//===============================================================================

TouchExplorer::SendHoverExitDelayed::SendHoverExitDelayed(
    /* [in] */ TouchExplorer* host)
    : mPointerIdBits(0)
    , mPolicyFlags(0)
    , mHost(host)
{}

TouchExplorer::SendHoverExitDelayed::~SendHoverExitDelayed()
{}

void TouchExplorer::SendHoverExitDelayed::Post(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIdBits,
    /* [in] */ Int32 policyFlags)
{
    Cancel();
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    mPrototype = NULL;
    helper->Obtain(event, (IMotionEvent**)&mPrototype);
    mPointerIdBits = pointerIdBits;
    mPolicyFlags = policyFlags;

    Boolean result;
    mHost->mHandler->PostDelayed(this, mHost->mDetermineUserIntentTimeout, &result);
}

void TouchExplorer::SendHoverExitDelayed::Cancel()
{
    if (IsPending()) {
        mHost->mHandler->RemoveCallbacks(this);
        Clear();
    }
}

Boolean TouchExplorer::SendHoverExitDelayed::IsPending()
{
    Boolean res;
    mHost->mHandler->HasCallbacks(this, &res);
    return res;
}

void TouchExplorer::SendHoverExitDelayed::Clear()
{
    IInputEvent::Probe(mPrototype)->Recycle();
    mPrototype = NULL;
    mPointerIdBits = -1;
    mPolicyFlags = 0;
}

void TouchExplorer::SendHoverExitDelayed::ForceSendAndRemove()
{
    if (IsPending()) {
        Run();
        Cancel();
    }
}

ECode TouchExplorer::SendHoverExitDelayed::Run()
{
    if (DEBUG) {
        Slogger::D(LOG_TAG_SEND_HOVER_DELAYED, "Injecting motion event: ACTION_HOVER_EXIT");
    }
    mHost->SendMotionEvent(mPrototype, IMotionEvent::ACTION_HOVER_EXIT, mPointerIdBits, mPolicyFlags);

    if (!mHost->mSendTouchExplorationEndDelayed->IsPending()) {
        mHost->mSendTouchExplorationEndDelayed->Cancel();
        mHost->mSendTouchExplorationEndDelayed->Post();
    }
    if (mHost->mSendTouchInteractionEndDelayed->IsPending()) {
        mHost->mSendTouchInteractionEndDelayed->Cancel();
        mHost->mSendTouchInteractionEndDelayed->Post();
    }

    Clear();

    return NOERROR;
}

//===============================================================================
//                  TouchExplorer::InjectedPointerTracker
//===============================================================================

TouchExplorer::InjectedPointerTracker::InjectedPointerTracker()
    : mInjectedPointersDown(0)
    , mLastInjectedDownEventTime(0)
{}

TouchExplorer::InjectedPointerTracker::~InjectedPointerTracker()
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
                IInputEvent::Probe(mLastInjectedHoverEvent)->Recycle();
                mLastInjectedHoverEvent = NULL;
            }
            AutoPtr<IMotionEventHelper> helper;
            CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
            helper->Obtain(event, (IMotionEvent**)&mLastInjectedHoverEvent);
            if (mLastInjectedHoverEventForClick != NULL) {
                IInputEvent::Probe(mLastInjectedHoverEventForClick)->Recycle();
                mLastInjectedHoverEventForClick = NULL;
            }
            helper->Obtain(event, (IMotionEvent**)&mLastInjectedHoverEventForClick);
        } break;
    }
    if (DEBUG) {
        String str;
        ToString(&str);
        Slogger::I(LOG_TAG_INJECTED_POINTER_TRACKER, "Injected pointer:\n %s", str.string());
    }
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
    return Elastos::Core::Math::BitCount(mInjectedPointersDown);
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

ECode TouchExplorer::InjectedPointerTracker::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder.Append("=========================");
    builder.Append("\nDown pointers #");
    // builder.Append(Integer.bitCount(mInjectedPointersDown));
    builder.Append(" [ ");
    for (Int32 i = 0; i < TouchExplorer::MAX_POINTER_COUNT; i++) {
        if ((mInjectedPointersDown & i) != 0) {
            builder.Append(i);
            builder.Append(" ");
        }
    }
    builder.Append("]");
    builder.Append("\n=========================");
    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  TouchExplorer::ReceivedPointerTracker
//===============================================================================

TouchExplorer::ReceivedPointerTracker::ReceivedPointerTracker(
    /* [in] */ TouchExplorer* host)
    : mReceivedPointersDown(0)
    , mLastReceivedDownEdgeFlags(0)
    , mPrimaryPointerId(0)
    , mLastReceivedUpPointerDownTime(0)
    , mLastReceivedUpPointerDownX(0)
    , mLastReceivedUpPointerDownY(0)
    , mHost(host)
{
    mReceivedPointerDownX = ArrayOf<Float>::Alloc(TouchExplorer::MAX_POINTER_COUNT);
    mReceivedPointerDownY = ArrayOf<Float>::Alloc(TouchExplorer::MAX_POINTER_COUNT);
    mReceivedPointerDownTime = ArrayOf<Int64>::Alloc(TouchExplorer::MAX_POINTER_COUNT);
}

TouchExplorer::ReceivedPointerTracker::~ReceivedPointerTracker()
{}

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
    mPrimaryPointerId = 0;
    mLastReceivedUpPointerDownTime = 0;
    mLastReceivedUpPointerDownX = 0;
    mLastReceivedUpPointerDownY = 0;
}

void TouchExplorer::ReceivedPointerTracker::OnMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    if (mLastReceivedEvent != NULL) {
        IInputEvent::Probe(mLastReceivedEvent)->Recycle();
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
        case IMotionEvent::ACTION_UP:
            HandleReceivedPointerUp(actionIndex, event);
            break;
        case IMotionEvent::ACTION_POINTER_UP:
            HandleReceivedPointerUp(actionIndex, event);
            break;
    }
    if (DEBUG) {
        String str;
        ToString(&str);
        Slogger::I(LOG_TAG_RECEIVED_POINTER_TRACKER, "Received pointer: \n %s", str.string());
    }
}

AutoPtr<IMotionEvent> TouchExplorer::ReceivedPointerTracker::GetLastReceivedEvent()
{
    return mLastReceivedEvent;
}

Int32 TouchExplorer::ReceivedPointerTracker::GetReceivedPointerDownCount()
{
    return Elastos::Core::Math::BitCount(mReceivedPointersDown);
}

Boolean TouchExplorer::ReceivedPointerTracker::IsReceivedPointerDown(
    /* [in] */ Int32 pointerId)
{
    Int32 pointerFlag = (1 << pointerId);
    return (mReceivedPointersDown & pointerFlag) != 0;
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

Int32 TouchExplorer::ReceivedPointerTracker::GetPrimaryPointerId()
{
    if (mPrimaryPointerId == TouchExplorer::INVALID_POINTER_ID) {
        mPrimaryPointerId = FindPrimaryPointerId();
    }
    return mPrimaryPointerId;
}

Int64 TouchExplorer::ReceivedPointerTracker::GetLastReceivedUpPointerDownTime()
{
    return mLastReceivedUpPointerDownTime;
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

void TouchExplorer::ReceivedPointerTracker::HandleReceivedPointerDown(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ IMotionEvent* event)
{
    Int32 pointerId;
    event->GetPointerId(pointerIndex, &pointerId);
    Int32 pointerFlag = (1 << pointerId);

    mLastReceivedUpPointerDownTime = 0;
    mLastReceivedUpPointerDownX = 0;
    mLastReceivedUpPointerDownX = 0;

    event->GetEdgeFlags(&mLastReceivedDownEdgeFlags);

    mReceivedPointersDown |= pointerFlag;
    event->GetX(pointerIndex, &(*mReceivedPointerDownX)[pointerId]);
    event->GetY(pointerIndex, &(*mReceivedPointerDownY)[pointerId]);
    IInputEvent::Probe(event)->GetEventTime(&(*mReceivedPointerDownTime)[pointerId]);

    mPrimaryPointerId = pointerId;
}

void TouchExplorer::ReceivedPointerTracker::HandleReceivedPointerUp(
    /* [in] */ Int32 pointerIndex,
    /* [in] */ IMotionEvent* event)
{
    Int32 pointerId;
    event->GetPointerId(pointerIndex, &pointerId);
    Int32 pointerFlag = (1 << pointerId);

    mLastReceivedUpPointerDownTime = GetReceivedPointerDownTime(pointerId);
    mLastReceivedUpPointerDownX = (*mReceivedPointerDownX)[pointerId];
    mLastReceivedUpPointerDownY = (*mReceivedPointerDownY)[pointerId];

    mReceivedPointersDown &= ~pointerFlag;
    (*mReceivedPointerDownX)[pointerId] = 0;
    (*mReceivedPointerDownY)[pointerId] = 0;
    (*mReceivedPointerDownTime)[pointerId] = 0;

    if (mPrimaryPointerId == pointerId) {
        mPrimaryPointerId = TouchExplorer::INVALID_POINTER_ID;
    }
}

Int32 TouchExplorer::ReceivedPointerTracker::FindPrimaryPointerId()
{
    Int32 primaryPointerId = TouchExplorer::INVALID_POINTER_ID;
    Int64 minDownTime = Elastos::Core::Math::INT64_MAX_VALUE;

    // Find the pointer that went down first.
    Int32 pointerIdBits = mReceivedPointersDown;
    while (pointerIdBits > 0) {
        const Int32 pointerId = Elastos::Core::Math::NumberOfTrailingZeros(pointerIdBits);
        pointerIdBits &= ~(1 << pointerId);
        const Int64 downPointerTime = (*mReceivedPointerDownTime)[pointerId];
        if (downPointerTime < minDownTime) {
            minDownTime = downPointerTime;
            primaryPointerId = pointerId;
        }
    }
    return primaryPointerId;
}

ECode TouchExplorer::ReceivedPointerTracker::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder builder;
    builder.Append("=========================");
    builder.Append("\nDown pointers #");
    builder.Append(GetReceivedPointerDownCount());
    builder.Append(" [ ");
    for (Int32 i = 0; i < TouchExplorer::MAX_POINTER_COUNT; i++) {
        if (IsReceivedPointerDown(i)) {
            builder.Append(i);
            builder.Append(" ");
        }
    }
    builder.Append("]");
    builder.Append("\nPrimary pointer id [ ");
    builder.Append(GetPrimaryPointerId());
    builder.Append(" ]");
    builder.Append("\n=========================");
    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  TouchExplorer::DoubleTapDetector
//===============================================================================

TouchExplorer::DoubleTapDetector::DoubleTapDetector(
    /* [in] */ TouchExplorer* host)
    : mHost(host)
{}

TouchExplorer::DoubleTapDetector::~DoubleTapDetector()
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
                    IInputEvent::Probe(mDownEvent)->Recycle();
                    mDownEvent = NULL;
                    return;
                }
                if (GestureUtils::IsMultiTap(mFirstTapEvent, event, mHost->mDoubleTapTimeout,
                        mHost->mDoubleTapSlop, actionIndex)) {
                    OnDoubleTap(event, policyFlags);
                    IInputEvent::Probe(mFirstTapEvent)->Recycle();
                    mFirstTapEvent = NULL;
                    IInputEvent::Probe(mDownEvent)->Recycle();
                    mDownEvent = NULL;
                    return;
                }
                IInputEvent::Probe(mFirstTapEvent)->Recycle();
                mFirstTapEvent = NULL;
            }
            else {
                if (mFirstTapEvent != NULL) {
                    IInputEvent::Probe(mFirstTapEvent)->Recycle();
                    mFirstTapEvent = NULL;
                }
            }
            IInputEvent::Probe(mDownEvent)->Recycle();
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
    mHost->mSendHoverEnterAndMoveDelayed->Cancel();
    mHost->mSendHoverExitDelayed->Cancel();
    mHost->mPerformLongPressDelayed->Cancel();

    if (mHost->mSendTouchExplorationEndDelayed->IsPending()) {
        mHost->mSendTouchExplorationEndDelayed->ForceSendAndRemove();
    }
    if (mHost->mSendTouchInteractionEndDelayed->IsPending()) {
        mHost->mSendTouchInteractionEndDelayed->ForceSendAndRemove();
    }

    Int32 actionIndex, pointerId, pointerIndex;
    secondTapUp->GetActionIndex(&actionIndex);
    secondTapUp->GetPointerId(actionIndex, &pointerId);
    secondTapUp->FindPointerIndex(pointerId, &pointerIndex);

    AutoPtr<IPoint> clickLocation = mHost->mTempPoint;
    if (!mHost->ComputeClickLocation(clickLocation)) {
        return;
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

    Int32 x, y;
    clickLocation->Get(&x, &y);
    (*coords)[0]->SetX(x);
    (*coords)[0]->SetY(y);
    AutoPtr<IMotionEventHelper> helper;
    ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper));
    Int64 downTime, eventTime;
    secondTapUp->GetDownTime(&downTime);
    IInputEvent::Probe(secondTapUp)->GetEventTime(&eventTime);
    Int32 deviceId, source, flags;
    IInputEvent::Probe(secondTapUp)->GetDeviceId(&deviceId);
    IInputEvent::Probe(secondTapUp)->GetSource(&source);
    secondTapUp->GetFlags(&flags);
    AutoPtr<IMotionEvent> event;
    helper->Obtain(downTime, eventTime, IMotionEvent::ACTION_DOWN, 1, properties,
            coords, 0, 0, 1.0f, 1.0f, deviceId, 0, source, flags, (IMotionEvent**)&event);
    mHost->SendActionDownAndUp(event, policyFlags);
    IInputEvent::Probe(event)->Recycle();
}

void TouchExplorer::DoubleTapDetector::Clear()
{
    if (mDownEvent != NULL) {
        IInputEvent::Probe(mDownEvent)->Recycle();
        mDownEvent = NULL;
    }
    if (mFirstTapEvent != NULL) {
        IInputEvent::Probe(mFirstTapEvent)->Recycle();
        mFirstTapEvent = NULL;
    }
}

Boolean TouchExplorer::DoubleTapDetector::FirstTapDetected()
{
    Int64 eventTime;
    IInputEvent::Probe(mFirstTapEvent)->GetEventTime(&eventTime);
    return mFirstTapEvent != NULL
            && SystemClock::GetUptimeMillis() - eventTime < mHost->mDoubleTapTimeout;
}

//===============================================================================
//                  TouchExplorer::ExitGestureDetectionModeDelayed
//===============================================================================

TouchExplorer::ExitGestureDetectionModeDelayed::ExitGestureDetectionModeDelayed(
    /* [in] */ TouchExplorer* host)
    : mHost(host)
{}

TouchExplorer::ExitGestureDetectionModeDelayed::~ExitGestureDetectionModeDelayed()
{}

void TouchExplorer::ExitGestureDetectionModeDelayed::Post()
{
    Boolean result;
    mHost->mHandler->PostDelayed(this, EXIT_GESTURE_DETECTION_TIMEOUT, &result);
}

void TouchExplorer::ExitGestureDetectionModeDelayed::Cancel()
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

//===============================================================================
//                  TouchExplorer::PerformLongPressDelayed
//===============================================================================

TouchExplorer::PerformLongPressDelayed::PerformLongPressDelayed(
    /* [in] */ TouchExplorer* host)
    : mPolicyFlags(0)
    , mHost(host)
{}

TouchExplorer::PerformLongPressDelayed::~PerformLongPressDelayed()
{}

void TouchExplorer::PerformLongPressDelayed::Post(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    mEvent = NULL;
    helper->Obtain(prototype, (IMotionEvent**)&mEvent);
    mPolicyFlags = policyFlags;
    AutoPtr<IViewConfigurationHelper> configHelper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&configHelper);
    Int32 timeOut;
    configHelper->GetLongPressTimeout(&timeOut);

    Boolean result;
    mHost->mHandler->PostDelayed(this, timeOut, &result);
}

void TouchExplorer::PerformLongPressDelayed::Cancel()
{
    if (mEvent != NULL) {
        mHost->mHandler->RemoveCallbacks(this);
        mHost->Clear();
    }
}

Boolean TouchExplorer::PerformLongPressDelayed::IsPending()
{
    Boolean res;
    mHost->mHandler->HasCallbacks(this, &res);
    return res;
}

ECode TouchExplorer::PerformLongPressDelayed::Run()
{
    // Pointers should not be zero when running this command.
    Int32 count;
    if ((mHost->mReceivedPointerTracker->GetLastReceivedEvent()->GetPointerCount(&count), count) == 0) {
        return NOERROR;
    }

    Int32 pointerId, pointerIndex;
    Int32 eventIndex;
    mEvent->GetActionIndex(&eventIndex);
    mEvent->GetPointerId(eventIndex, &pointerId);
    mEvent->FindPointerIndex(pointerId, &pointerIndex);

    AutoPtr<IPoint> clickLocation = mHost->mTempPoint;
    if (!mHost->ComputeClickLocation(clickLocation)) {
        return NOERROR;
    }

    mHost->mLongPressingPointerId = pointerId;
    Float eventX, eventY;
    mEvent->GetX(pointerIndex, &eventX);
    mEvent->GetY(pointerIndex, &eventY);
    Int32 x, y;
    clickLocation->Get(&x, &y);
    mHost->mLongPressingPointerDeltaX = (Int32)(eventX - x);
    mHost->mLongPressingPointerDeltaY = (Int32)(eventY - y);

    mHost->SendHoverExitAndTouchExplorationGestureEndIfNeeded(mPolicyFlags);

    mHost->mCurrentState = TouchExplorer::STATE_DELEGATING;
    mHost->SendDownForAllNotInjectedPointers(mEvent, mPolicyFlags);
    Clear();
    return NOERROR;
}

void TouchExplorer::PerformLongPressDelayed::Clear()
{
    IInputEvent::Probe(mEvent)->Recycle();
    mEvent = NULL;
    mPolicyFlags = 0;
}

//===============================================================================
//                  TouchExplorer::SendAccessibilityEventDelayed
//===============================================================================

TouchExplorer::SendAccessibilityEventDelayed::SendAccessibilityEventDelayed(
    /* [in] */ Int32 eventType,
    /* [in] */ Int32 delay,
    /* [in] */ TouchExplorer* host)
    : mEventType(eventType)
    , mDelay(delay)
    , mHost(host)
{}

TouchExplorer::SendAccessibilityEventDelayed::~SendAccessibilityEventDelayed()
{}

void TouchExplorer::SendAccessibilityEventDelayed::Cancel()
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
        Cancel();
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

CAR_INTERFACE_IMPL(TouchExplorer, Object, IEventStreamTransformation);

TouchExplorer::TouchExplorer(
    /* [in] */ IContext* context,
    /* [in] */ AccessibilityManagerService* service)
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
    AutoPtr<IVelocityTrackerHelper> velhelper;
    ASSERT_SUCCEEDED(CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&velhelper));
    ASSERT_SUCCEEDED(velhelper->Obtain((IVelocityTracker**)&mVelocityTracker));

    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));

    ASSERT_SUCCEEDED(CPoint::New((IPoint**)&mTempPoint));

    CArrayList::New(100, (IArrayList**)&mStrokeBuffer);

    mReceivedPointerTracker = new ReceivedPointerTracker(this);
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
    mSendHoverEnterAndMoveDelayed = new SendHoverEnterAndMoveDelayed(this);
    mSendHoverExitDelayed = new SendHoverExitDelayed(this);
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
    mAms = NULL;
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
    mSendHoverEnterAndMoveDelayed->Cancel();
    mSendHoverExitDelayed->Cancel();
    mPerformLongPressDelayed->Cancel();
    mExitGestureDetectionModeDelayed->Cancel();
    mSendTouchExplorationEndDelayed->Cancel();
    mSendTouchInteractionEndDelayed->Cancel();
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
        mSendTouchExplorationEndDelayed->Cancel();
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_END);
    }

    // The event for touch interaction end should be strictly after the
    // last hover exit and the touch exploration gesture end events.
    if (mSendTouchInteractionEndDelayed->IsPending()
                && eventType == IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT) {
        mSendTouchInteractionEndDelayed->Cancel();
        SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END);
    }

    // If a new window opens or the accessibility focus moves we no longer
    // want to click/long press on the last touch explored location.
    switch (eventType) {
        case IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED:
        case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED:
            if (mInjectedPointerTracker->mLastInjectedHoverEventForClick != NULL) {
                IInputEvent::Probe(mInjectedPointerTracker->mLastInjectedHoverEventForClick)->Recycle();
                mInjectedPointerTracker->mLastInjectedHoverEventForClick = NULL;
            }
            mLastTouchedWindowId = -1;
            break;
        case IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER:
        case IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT:
            IAccessibilityRecord::Probe(event)->GetWindowId(&mLastTouchedWindowId);
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

    mVelocityTracker->AddMovement(rawEvent);

    mDoubleTapDetector->OnMotionEvent(event, policyFlags);

    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            mAms->OnTouchInteractionStart();
            // Pre-feed the motion events to the gesture detector since we
            // have a distance slop before getting into gesture detection
            // mode and not using the points within this slop significantly
            // decreases the quality of gesture recognition.
            HandleMotionEventGestureDetecting(rawEvent, policyFlags);
            SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_START);

            // If we still have not notified the user for the last
            // touch, we figure out what to do. If were waiting
            // we resent the delayed callback and wait again.
            mSendHoverEnterAndMoveDelayed->Cancel();
            mSendHoverExitDelayed->Cancel();
            mPerformLongPressDelayed->Cancel();

            if (mSendTouchExplorationEndDelayed->IsPending()) {
                mSendTouchExplorationEndDelayed->ForceSendAndRemove();
            }

            if (mSendTouchInteractionEndDelayed->IsPending()) {
                mSendTouchInteractionEndDelayed->ForceSendAndRemove();
            }

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
                if (!mSendHoverEnterAndMoveDelayed->IsPending()) {
                    // Deliver hover enter with a delay to have a chance
                    // to detect what the user is trying to do.
                    Int32 pointerId = receivedTracker->GetPrimaryPointerId();
                    Int32 pointerIdBits = (1 << pointerId);
                    mSendHoverEnterAndMoveDelayed->Post(event, TRUE, pointerIdBits, policyFlags);
                }
                else {
                    // Cache the event until we discern exploration from gesturing.
                    mSendHoverEnterAndMoveDelayed->AddEvent(event);
                }
            }
            break;
        }

        case IMotionEvent::ACTION_POINTER_DOWN: {
            // Another finger down means that if we have not started to deliver
            // hover events, we will not have to. The code for ACTION_MOVE will
            // decide what we will actually do next.
            mSendHoverEnterAndMoveDelayed->Cancel();
            mSendHoverExitDelayed->Cancel();
            mPerformLongPressDelayed->Cancel();
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Int32 pointerId = receivedTracker->GetPrimaryPointerId();
            Int32 pointerIndex;
            event->FindPointerIndex(pointerId, &pointerIndex);
            Int32 pointerIdBits = (1 << pointerId);
            Int32 count;
            event->GetPointerCount(&count);
            switch (count) {
                case 1: {
                    // We have not started sending events since we try to
                    // figure out what the user is doing.
                    if (mSendHoverEnterAndMoveDelayed->IsPending()) {
                        // Pre-feed the motion events to the gesture detector since we
                        // have a distance slop before getting into gesture detection
                        // mode and not using the points within this slop significantly
                        // decreases the quality of gesture recognition.
                        HandleMotionEventGestureDetecting(rawEvent, policyFlags);

                        // Cache the event until we discern exploration from gesturing.
                        mSendHoverEnterAndMoveDelayed->AddEvent(event);

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
                            using Elastos::Core::Math;
                            Float maxAbsVelocity = Math::Max(
                                    Math::Abs(xVelocity), Math::Abs(yVelocity));
                            if (maxAbsVelocity > mScaledGestureDetectionVelocity) {
                                // We have to perform gesture detection, so
                                // clear the current state and try to detect.
                                mCurrentState = STATE_GESTURE_DETECTING;
                                mVelocityTracker->Clear();
                                mSendHoverEnterAndMoveDelayed->Cancel();
                                mSendHoverExitDelayed->Cancel();
                                mPerformLongPressDelayed->Cancel();
                                mExitGestureDetectionModeDelayed->Post();
                                // Send accessibility event to announce the start
                                // of gesture recognition.
                                SendAccessibilityEvent(
                                        IAccessibilityEvent::TYPE_GESTURE_DETECTION_START);
                            }
                            else {
                                // We have just decided that the user is touch,
                                // exploring so start sending events.
                                mSendHoverEnterAndMoveDelayed->ForceSendAndRemove();
                                mSendHoverExitDelayed->Cancel();
                                mPerformLongPressDelayed->Cancel();
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
                                mPerformLongPressDelayed->Cancel();
                            }
                        }
                        if (mTouchExplorationInProgress) {
                            SendTouchExplorationGestureStartAndHoverEnterIfNeeded(policyFlags);
                            SendMotionEvent(event, IMotionEvent::ACTION_HOVER_MOVE, pointerIdBits,
                                    policyFlags);
                        }
                    }
                    break;
                }

                case 2: {
                    // More than one pointer so the user is not touch exploring
                    // and now we have to decide whether to delegate or drag.
                    if (mSendHoverEnterAndMoveDelayed->IsPending()) {
                        // We have not started sending events so cancel
                        // scheduled sending events.
                        mSendHoverEnterAndMoveDelayed->Cancel();
                        mSendHoverExitDelayed->Cancel();
                        mPerformLongPressDelayed->Cancel();
                    }
                    else {
                        mPerformLongPressDelayed->Cancel();
                        if (mTouchExplorationInProgress) {
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
                        SendDownForAllNotInjectedPointers(event, policyFlags);
                    }
                    mVelocityTracker->Clear();
                    break;
                }

                default: {
                    // More than one pointer so the user is not touch exploring
                    // and now we have to decide whether to delegate or drag.
                    if (mSendHoverEnterAndMoveDelayed->IsPending()) {
                        // We have not started sending events so cancel
                        // scheduled sending events.
                        mSendHoverEnterAndMoveDelayed->Cancel();
                        mSendHoverExitDelayed->Cancel();
                        mPerformLongPressDelayed->Cancel();
                    }
                    else {
                        mPerformLongPressDelayed->Cancel();
                        // We are sending events so send exit and gesture
                        // end since we transition to another state.
                        SendHoverExitAndTouchExplorationGestureEndIfNeeded(policyFlags);
                    }

                    // More than two pointers are delegated to the view hierarchy.
                    mCurrentState = STATE_DELEGATING;
                    SendDownForAllNotInjectedPointers(event, policyFlags);
                    mVelocityTracker->Clear();
                }
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            mAms->OnTouchInteractionEnd();
            // We know that we do not need the pre-fed gesture points are not
            // needed anymore since the last pointer just went up.
            mStrokeBuffer->Clear();
            Int32 index;
            event->GetActionIndex(&index);
            Int32 pointerId;
            event->GetPointerId(index, &pointerId);
            Int32 pointerIdBits = (1 << pointerId);

            mPerformLongPressDelayed->Cancel();
            mVelocityTracker->Clear();

            // If we have not delivered the enter schedule exit.
            if (mSendHoverEnterAndMoveDelayed->IsPending()) {
                mSendHoverExitDelayed->Post(event, pointerIdBits, policyFlags);
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
            SendDownForAllNotInjectedPointers(event, policyFlags);
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Int32 pointerCount;
            event->GetPointerCount(&pointerCount);
            switch (pointerCount) {
                case 1: {
                    // do nothing
                    break;
                }

                case 2: {
                    if (IsDraggingGesture(event)) {
                        Float firstPtrX, firstPtrY, secondPtrX, secondPtrY;
                        event->GetX(0, &firstPtrX);
                        event->GetY(0, &firstPtrY);
                        event->GetX(1, &secondPtrX);
                        event->GetY(1, &secondPtrY);

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
                        // Deliver all pointers to the view hierarchy.
                        SendDownForAllNotInjectedPointers(event, policyFlags);
                    }
                    break;
                }

                default: {
                    mCurrentState = STATE_DELEGATING;
                    // Send an event to the end of the drag gesture.
                    SendMotionEvent(event, IMotionEvent::ACTION_UP, pointerIdBits,
                            policyFlags);
                    // Deliver all pointers to the view hierarchy.
                    SendDownForAllNotInjectedPointers(event, policyFlags);
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

        case IMotionEvent::ACTION_UP: {
            // Offset the event if we are doing a long press as the
            // target is not necessarily under the user's finger.
            if (mLongPressingPointerId >= 0) {
                event = OffsetEvent(event, - mLongPressingPointerDeltaX,
                        - mLongPressingPointerDeltaY);
                // Clear the long press state.
                mLongPressingPointerId = -1;
                mLongPressingPointerDeltaX = 0;
                mLongPressingPointerDeltaY = 0;
            }

            // Deliver the event.
            Int32 action;
            event->GetAction(&action);
            SendMotionEvent(event, action, ALL_POINTER_ID_BITS, policyFlags);

            // Announce the end of a the touch interaction.
            mAms->OnTouchInteractionEnd();
            SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END);

            mCurrentState = STATE_TOUCH_EXPLORING;
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            Clear(event, policyFlags);
            break;
        }

        default: {
            // Deliver the event.
            Int32 action;
            event->GetAction(&action);
            SendMotionEvent(event, action, ALL_POINTER_ID_BITS, policyFlags);
        }
    }

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
    IInputEvent::Probe(event)->GetEventTime(&eventTime);

    switch (actionMasked) {
        case IMotionEvent::ACTION_DOWN: {
            mPreviousX = x;
            mPreviousY = y;
            AutoPtr<IGesturePoint> point;
            CGesturePoint::New(x, y, eventTime, (IGesturePoint**)&point);
            mStrokeBuffer->Add(point);
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
                mStrokeBuffer->Add(point);
            }
            break;
        }

        case IMotionEvent::ACTION_UP: {
            mAms->OnTouchInteractionEnd();
            // Announce the end of the gesture recognition.
            SendAccessibilityEvent(IAccessibilityEvent::TYPE_GESTURE_DETECTION_END);
            // Announce the end of a the touch interaction.
            SendAccessibilityEvent(IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END);

            AutoPtr<IGesturePoint> point;
            CGesturePoint::New(x, y, eventTime, (IGesturePoint**)&point);
            mStrokeBuffer->Add(point);

            AutoPtr<IGesture> gesture;
            ASSERT_SUCCEEDED(CGesture::New((IGesture**)&gesture));
            AutoPtr<IGestureStroke> stroke;
            CGestureStroke::New(mStrokeBuffer, (IGestureStroke**)&stroke);
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
            mExitGestureDetectionModeDelayed->Cancel();
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
        IAccessibilityRecord::Probe(event)->SetWindowId(mAms->GetActiveWindowId());
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

void TouchExplorer::SendDownForAllNotInjectedPointers(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<InjectedPointerTracker> injectedPointers = mInjectedPointerTracker;
    Int32 pointerIdBits = 0;
    Int32 pointerCount;
    prototype->GetPointerCount(&pointerCount);

    for (Int32 i = 0; i < pointerCount; i++) {
        Int32 pointerId;
        prototype->GetPointerId(i, &pointerId);

        // Do not send event for already delivered pointers.
        if (!injectedPointers->IsInjectedPointerDown(pointerId)) {
            pointerIdBits |= (1 << pointerId);
            Int32 action = ComputeInjectionAction(IMotionEvent::ACTION_DOWN, i);
            SendMotionEvent(prototype, action, pointerIdBits, policyFlags);
        }
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

void TouchExplorer::SendActionDownAndUp(
    /* [in] */ IMotionEvent* prototype,
    /* [in] */ Int32 policyFlags)
{
    // Tap with the pointer that last explored.
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

    AutoPtr<IMotionEvent> event;
    if (pointerIdBits == ALL_POINTER_ID_BITS) {
        event = prototype;
    }
    else {
        prototype->Split(pointerIdBits, (IMotionEvent**)&event);
    }
    if (action == IMotionEvent::ACTION_DOWN) {
        Int64 eventTime;
        IInputEvent::Probe(event)->GetEventTime(&eventTime);
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
        event = OffsetEvent(event, - mLongPressingPointerDeltaX,
                - mLongPressingPointerDeltaY);
    }

    if (DEBUG) {
        Slogger::D(LOG_TAG, "Injecting event: %p, policyFlags=0x",
                event.Get()/*, StringUtils::ToHexString(policyFlags)*/);
    }

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
        IInputEvent::Probe(event)->Recycle();
    }
}

AutoPtr<IMotionEvent> TouchExplorer::OffsetEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 offsetX,
    /* [in] */ Int32 offsetY)
{
    if (offsetX == 0 && offsetY == 0) {
        return event;
    }

    Int32 remappedIndex;
    event->FindPointerIndex(mLongPressingPointerId, &remappedIndex);
    Int32 pointerCount;
    event->GetPointerCount(&pointerCount);

    AutoPtr<IPointerPropertiesHelper> ppHelper;
    CPointerPropertiesHelper::AcquireSingleton((IPointerPropertiesHelper**)&ppHelper);
    AutoPtr< ArrayOf<IPointerProperties*> > props;
    ppHelper->CreateArray(pointerCount, (ArrayOf<IPointerProperties*>**)&props);

    AutoPtr<IPointerCoordsHelper> pcHelper;
    CPointerCoordsHelper::AcquireSingleton((IPointerCoordsHelper**)&pcHelper);
    AutoPtr< ArrayOf<IPointerCoords*> > coords;
    pcHelper->CreateArray(pointerCount, (ArrayOf<IPointerCoords*>**)&coords);

    for (Int32 i = 0; i < pointerCount; i++) {
        event->GetPointerProperties(i, (*props)[i]);
        event->GetPointerCoords(i, (*coords)[i]);
        if (i == remappedIndex) {
            Float x, y;
            (*coords)[i]->GetX(&x);
            (*coords)[i]->GetY(&y);
            x += offsetX;
            y += offsetY;
            (*coords)[i]->SetX(x);
            (*coords)[i]->SetY(y);
        }
    }

    Int64 downTime, eventTime;
    event->GetDownTime(&downTime);
    IInputEvent::Probe(event)->GetEventTime(&eventTime);
    Int32 action, pCount, metaState, buttonState, deviceId, edgeFlags, source, flags;
    event->GetAction(&action);
    event->GetPointerCount(&pCount);
    event->GetMetaState(&metaState);
    event->GetButtonState(&buttonState);
    IInputEvent::Probe(event)->GetDeviceId(&deviceId);
    event->GetEdgeFlags(&edgeFlags);
    IInputEvent::Probe(event)->GetSource(&source);
    event->GetFlags(&flags);
    AutoPtr<IMotionEventHelper> eventHelper;
    ASSERT_SUCCEEDED(CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&eventHelper));
    AutoPtr<IMotionEvent> result;
    eventHelper->Obtain(downTime, eventTime, action, pCount,
        props, coords, metaState, buttonState, 1.0f, 1.0f, deviceId, edgeFlags,
        source, flags, (IMotionEvent**)&result);

    return result;
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

    Float firstPtrX, firstPtrY, secondPtrX, secondPtrY;
    event->GetX(0, &firstPtrX);
    event->GetY(0, &firstPtrY);
    event->GetX(1, &secondPtrX);
    event->GetY(1, &secondPtrY);

    Float firstPtrDownX = receivedTracker->GetReceivedPointerDownX(0);
    Float firstPtrDownY = receivedTracker->GetReceivedPointerDownY(0);
    Float secondPtrDownX = receivedTracker->GetReceivedPointerDownX(1);
    Float secondPtrDownY = receivedTracker->GetReceivedPointerDownY(1);

    return GestureUtils::IsDraggingGesture(firstPtrDownX, firstPtrDownY, secondPtrDownX,
        secondPtrDownY, firstPtrX, firstPtrY, secondPtrX, secondPtrY,
        MAX_DRAGGING_ANGLE_COS);
}

Boolean TouchExplorer::ComputeClickLocation(
    /* [in] */ IPoint* outLocation)
{
    AutoPtr<IMotionEvent> lastExploreEvent = mInjectedPointerTracker->GetLastInjectedHoverEventForClick();
    if (lastExploreEvent != NULL) {
        Int32 lastExplorePointerIndex;
        lastExploreEvent->GetActionIndex(&lastExplorePointerIndex);
        Float x, y;
        lastExploreEvent->GetX(lastExplorePointerIndex, &x);
        lastExploreEvent->GetY(lastExplorePointerIndex, &y);

        outLocation->Set((Int32)x, (Int32)y);
        if (!mAms->AccessibilityFocusOnlyInActiveWindow()
                || mLastTouchedWindowId == mAms->GetActiveWindowId()) {
            mAms->GetAccessibilityFocusClickPointInScreen(outLocation);
        }
        return TRUE;
    }
    if (mAms->GetAccessibilityFocusClickPointInScreen(outLocation)) {
        return TRUE;
    }
    return FALSE;
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
            return String("Unknown state: ") + StringUtils::ToString(state);
            // throw new IllegalArgumentException("Unknown state: " + state);
    }
}

ECode TouchExplorer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = LOCAL_LOG_TAG;
    return NOERROR;
}

} // Accessibility
} // Server
} // Droid
} // Elastos
