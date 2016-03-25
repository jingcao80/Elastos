
#include "elastos/droid/server/accessibility/AccessibilityManagerService.h"
#include "elastos/droid/server/accessibility/AccessibilityInputFilter.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::EIID_IMotionEvent;
using Elastos::Droid::View::CChoreographerHelper;
using Elastos::Droid::View::IChoreographerHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const Int32 MotionEventHolder::MAX_POOL_SIZE = 32;
AutoPtr< Pools::SimplePool<MotionEventHolder> > MotionEventHolder::sPool =
        new Pools::SimplePool<MotionEventHolder>(MAX_POOL_SIZE);

//===============================================================================
//                  MotionEventHolder
//===============================================================================

MotionEventHolder::MotionEventHolder()
    : mPolicyFlags(0)
{}

MotionEventHolder::~MotionEventHolder()
{}

AutoPtr<MotionEventHolder> MotionEventHolder::Obtain(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<MotionEventHolder> holder = sPool->AcquireItem();
    if (holder == NULL) {
        holder = new MotionEventHolder();
    }

    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    helper->Obtain(event, (IMotionEvent**)&holder->mEvent);
    holder->mPolicyFlags = policyFlags;
    return holder;
}

ECode MotionEventHolder::Recycle()
{
    IInputEvent::Probe(mEvent)->Recycle();
    mEvent = NULL;
    mPolicyFlags = 0;
    mNext = NULL;
    mPrevious = NULL;
    sPool->ReleaseItem(this);
    return NOERROR;
}

const Int32 AccessibilityInputFilter::FLAG_FEATURE_SCREEN_MAGNIFIER = 0x00000001;
const Int32 AccessibilityInputFilter::FLAG_FEATURE_TOUCH_EXPLORATION = 0x00000002;
const Int32 AccessibilityInputFilter::FLAG_FEATURE_FILTER_KEY_EVENTS = 0x00000004;
const String AccessibilityInputFilter::TAG("AccessibilityInputFilter");
const Boolean AccessibilityInputFilter::DEBUG = TRUE;

//===============================================================================
//                  AccessibilityInputFilter::MyRunnable
//===============================================================================

AccessibilityInputFilter::MyRunnable::MyRunnable(
    /* [in] */ AccessibilityInputFilter* host)
    : mHost(host)
{}

AccessibilityInputFilter::MyRunnable::~MyRunnable()
{}

ECode AccessibilityInputFilter::MyRunnable::Run()
{
    Int64 frameTimeNanos;
    mHost->mChoreographer->GetFrameTimeNanos(&frameTimeNanos);
    if (DEBUG) {
        Slogger::I(TAG, "Begin batch processing for frame: %lld", frameTimeNanos);
    }
    mHost->ProcessBatchedEvents(frameTimeNanos);
    if (DEBUG) {
        Slogger::I(TAG, "End batch processing.");
    }
    if (mHost->mEventQueue != NULL) {
        mHost->ScheduleProcessBatchedEvents();
    }
    return NOERROR;
}

//===============================================================================
//                  AccessibilityInputFilter
//===============================================================================

CAR_INTERFACE_IMPL(AccessibilityInputFilter, InputFilter, IEventStreamTransformation);

AccessibilityInputFilter::AccessibilityInputFilter()
    : InputFilter::InputFilter()
    , mCurrentTouchDeviceId(0)
    , mInstalled(FALSE)
    , mEnabledFeatures(0)
    , mMotionEventSequenceStarted(FALSE)
    , mHoverEventSequenceStarted(FALSE)
    , mKeyEventSequenceStarted(FALSE)
    , mFilterKeyEvents(FALSE)
{}

AccessibilityInputFilter::~AccessibilityInputFilter()
{
    mAms = NULL;
}
ECode AccessibilityInputFilter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAccessibilityManager* service)
{
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    InputFilter::constructor(looper);

    mContext = context;
    mAms = (AccessibilityManagerService*)service;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPm = IPowerManager::Probe(obj);

    AutoPtr<IChoreographerHelper> helper;
    CChoreographerHelper::AcquireSingleton((IChoreographerHelper**)&helper);
    helper->GetInstance((IChoreographer**)&mChoreographer);

    return NOERROR;
}

ECode AccessibilityInputFilter::OnInstalled()
{
    if (DEBUG) {
        Slogger::D(TAG, "Accessibility input filter installed.");
    }

    mInstalled = TRUE;
    DisableFeatures();
    EnableFeatures();
    return InputFilter::OnInstalled();
}

ECode AccessibilityInputFilter::OnUninstalled()
{
    if (DEBUG) {
        Slogger::D(TAG, "Accessibility input filter uninstalled.");
    }
    mInstalled = FALSE;
    DisableFeatures();
    return InputFilter::OnUninstalled();
}

ECode AccessibilityInputFilter::OnInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (DEBUG) {
        Slogger::D(TAG, "Received event: %p, policyFlags=0x%08x", policyFlags);
    }

    Boolean res;
    if (IMotionEvent::Probe(event) != NULL
            && (event->IsFromSource(IInputDevice::SOURCE_TOUCHSCREEN, &res), res)) {
        AutoPtr<IMotionEvent> motionEvent = IMotionEvent::Probe(event);
        OnMotionEvent(motionEvent, policyFlags);
    }
    else if (IKeyEvent::Probe(event) != NULL
            && (event->IsFromSource(IInputDevice::SOURCE_KEYBOARD, &res), res)) {
        AutoPtr<IKeyEvent> keyEvent = IKeyEvent::Probe(event);
        OnKeyEvent(keyEvent, policyFlags);
    }
    else {
        InputFilter::OnInputEvent(event, policyFlags);
    }

    return NOERROR;
}

void AccessibilityInputFilter::OnMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (mEventHandler == NULL) {
        InputFilter::OnInputEvent(IInputEvent::Probe(event), policyFlags);
        return;
    }

    if ((policyFlags & IWindowManagerPolicy::FLAG_PASS_TO_USER) == 0) {
        mMotionEventSequenceStarted = FALSE;
        mHoverEventSequenceStarted = FALSE;
        mEventHandler->Clear();
        InputFilter::OnInputEvent(IInputEvent::Probe(event), policyFlags);
        return;
    }

    Int32 deviceId;
    IInputEvent::Probe(event)->GetDeviceId(&deviceId);
    if (mCurrentTouchDeviceId != deviceId) {
        mCurrentTouchDeviceId = deviceId;
        mMotionEventSequenceStarted = FALSE;
        mHoverEventSequenceStarted = FALSE;
        mEventHandler->Clear();
    }
    if (mCurrentTouchDeviceId < 0) {
        InputFilter::OnInputEvent(IInputEvent::Probe(event), policyFlags);
        return;
    }
    // We do not handle scroll events.
    Int32 actionMasked;
    if ((event->GetActionMasked(&actionMasked), actionMasked) == IMotionEvent::ACTION_SCROLL) {
        InputFilter::OnInputEvent(IInputEvent::Probe(event), policyFlags);
        return;
    }
    // Wait for a down touch event to start processing.
    Boolean res;
    if (event->IsTouchEvent(&res), res) {
        if (!mMotionEventSequenceStarted) {
            if ((event->GetActionMasked(&actionMasked), actionMasked) != IMotionEvent::ACTION_DOWN) {
                return;
            }
            mMotionEventSequenceStarted = TRUE;
        }
    }
    else {
        // Wait for an enter hover event to start processing.
        if (!mHoverEventSequenceStarted) {
            if ((event->GetActionMasked(&actionMasked), actionMasked) != IMotionEvent::ACTION_HOVER_ENTER) {
                return;
            }
            mHoverEventSequenceStarted = TRUE;
        }
    }
    BatchMotionEvent(event, policyFlags);
}

void AccessibilityInputFilter::OnKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (!mFilterKeyEvents) {
        InputFilter::OnInputEvent(IInputEvent::Probe(event), policyFlags);
        return;
    }
    if ((policyFlags & IWindowManagerPolicy::FLAG_PASS_TO_USER) == 0) {
        mKeyEventSequenceStarted = FALSE;
        InputFilter::OnInputEvent(IInputEvent::Probe(event), policyFlags);
        return;
    }
    // Wait for a down key event to start processing.
    if (!mKeyEventSequenceStarted) {
        Int32 action;
        if ((event->GetAction(&action), action) != IKeyEvent::ACTION_DOWN) {
            InputFilter::OnInputEvent(IInputEvent::Probe(event), policyFlags);
            return;
        }
        mKeyEventSequenceStarted = TRUE;
    }
    mAms->NotifyKeyEvent(event, policyFlags);
}

void AccessibilityInputFilter::ScheduleProcessBatchedEvents()
{
    mChoreographer->PostCallback(IChoreographer::CALLBACK_INPUT,
            mProcessBatchedEventsRunnable, NULL);
}

void AccessibilityInputFilter::BatchMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (DEBUG) {
        Slogger::I(TAG, "Batching event: %p, policyFlags: %d", event, policyFlags);
    }
    if (mEventQueue == NULL) {
        mEventQueue = MotionEventHolder::Obtain(event, policyFlags);
        ScheduleProcessBatchedEvents();
        return;
    }
    Boolean res;
    if (mEventQueue->mEvent->AddBatch(event, &res), res) {
        return;
    }

    AutoPtr<MotionEventHolder> holder = MotionEventHolder::Obtain(event, policyFlags);
    holder->mNext = mEventQueue;
    mEventQueue->mPrevious = holder;
    mEventQueue = holder;
}

void AccessibilityInputFilter::ProcessBatchedEvents(
    /* [in] */ Int64 frameNanos)
{
    AutoPtr<MotionEventHolder> current = mEventQueue;
    while (current->mNext != NULL) {
        current = current->mNext;
    }
    while (TRUE) {
        if (current == NULL) {
            mEventQueue = NULL;
            break;
        }
        Int64 nano;
        if ((IInputEvent::Probe(current->mEvent)->GetEventTimeNano(&nano), nano) >= frameNanos) {
            // Finished with this choreographer frame. Do the rest on the next one.
            current->mNext = NULL;
            break;
        }
        HandleMotionEvent(current->mEvent, current->mPolicyFlags);
        AutoPtr<MotionEventHolder> prior = current;
        current = current->mPrevious;
        prior->Recycle();
    }
}

void AccessibilityInputFilter::HandleMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (DEBUG) {
        Slogger::I(TAG, "Handling batched event: %p, policyFlags: %d", event, policyFlags);
    }
    // Since we do batch processing it is possible that by the time the
    // next batch is processed the event handle had been set to null.
    if (mEventHandler != NULL) {
        if (mPm) {
            Int64 eventTime;
            IInputEvent::Probe(event)->GetEventTime(&eventTime);
            mPm->UserActivity(eventTime, FALSE);
        }

        AutoPtr<IMotionEventHelper> helper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
        AutoPtr<IMotionEvent> transformedEvent;
        helper->Obtain(event, (IMotionEvent**)&transformedEvent);
        mEventHandler->OnMotionEvent(transformedEvent, event, policyFlags);
        IInputEvent::Probe(transformedEvent)->Recycle();
    }
}

ECode AccessibilityInputFilter::OnMotionEvent(
    /* [in] */ IMotionEvent* transformedEvent,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    return InputFilter::SendInputEvent(IInputEvent::Probe(transformedEvent), policyFlags);
}

ECode AccessibilityInputFilter::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    // TODO Implement this to inject the accessibility event
    //      into the accessibility manager service similarly
    //      to how this is done for input events.
    return NOERROR;
}

ECode AccessibilityInputFilter::SetNext(
    /* [in] */ IEventStreamTransformation* next)
{
    /* do nothing */
    return NOERROR;
}

ECode AccessibilityInputFilter::Clear()
{
    /* do nothing */
    return NOERROR;
}

ECode AccessibilityInputFilter::SetEnabledFeatures(
    /* [in] */ Int32 enabledFeatures)
{
    if (mEnabledFeatures == enabledFeatures) {
        return NOERROR;
    }
    if (mInstalled) {
        DisableFeatures();
    }
    mEnabledFeatures = enabledFeatures;
    if (mInstalled) {
        EnableFeatures();
    }

    return NOERROR;
}

ECode AccessibilityInputFilter::NotifyAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    if (mEventHandler != NULL) {
        mEventHandler->OnAccessibilityEvent(event);
    }
    return NOERROR;
}

void AccessibilityInputFilter::EnableFeatures()
{
    mMotionEventSequenceStarted = FALSE;
    mHoverEventSequenceStarted = FALSE;
    if ((mEnabledFeatures & FLAG_FEATURE_SCREEN_MAGNIFIER) != 0) {
        mScreenMagnifier = new ScreenMagnifier(mContext, IDisplay::DEFAULT_DISPLAY, mAms);
        mEventHandler = (IEventStreamTransformation*)mScreenMagnifier.Get();
        mEventHandler->SetNext(this);
    }

    if ((mEnabledFeatures & FLAG_FEATURE_TOUCH_EXPLORATION) != 0) {
        mTouchExplorer = new TouchExplorer(mContext, mAms);
        mTouchExplorer->SetNext(this);
        if (mEventHandler != NULL) {
            mEventHandler->SetNext((IEventStreamTransformation*)mTouchExplorer);
        }
        else {
            mEventHandler = (IEventStreamTransformation*)mTouchExplorer;
        }
    }
    if ((mEnabledFeatures & FLAG_FEATURE_FILTER_KEY_EVENTS) != 0) {
        mFilterKeyEvents = TRUE;
    }
}

ECode AccessibilityInputFilter::DisableFeatures()
{
    if (mTouchExplorer != NULL) {
        mTouchExplorer->Clear();
        mTouchExplorer->OnDestroy();
        mTouchExplorer = NULL;
    }

    if (mScreenMagnifier != NULL) {
        mScreenMagnifier->Clear();
        mScreenMagnifier->OnDestroy();
        mScreenMagnifier = NULL;
    }
    mEventHandler = NULL;
    mKeyEventSequenceStarted = FALSE;
    mMotionEventSequenceStarted = FALSE;
    mHoverEventSequenceStarted = FALSE;
    mFilterKeyEvents = FALSE;

    return NOERROR;
}

ECode AccessibilityInputFilter::OnDestroy()
{
    /* ignore */
    return NOERROR;
}

} // Accessibility
} // Server
} // Droid
} // Elastos
