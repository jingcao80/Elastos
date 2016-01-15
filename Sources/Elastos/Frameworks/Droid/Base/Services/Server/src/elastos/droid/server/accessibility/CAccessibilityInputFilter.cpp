
#include "accessibility/CAccessibilityInputFilter.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::EIID_IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const String CAccessibilityInputFilter::TAG("CAccessibilityInputFilter");
const Boolean CAccessibilityInputFilter::DEBUG = TRUE;
const Int32 CAccessibilityInputFilter::UNDEFINED_DEVICE_ID = -1;
const Int32 CAccessibilityInputFilter::FLAG_FEATURE_SCREEN_MAGNIFIER = 0x00000001;
const Int32 CAccessibilityInputFilter::FLAG_FEATURE_TOUCH_EXPLORATION = 0x00000002;

CAccessibilityInputFilter::CAccessibilityInputFilter()
    : mCurrentDeviceId(0)
    , mInstalled(FALSE)
    , mEnabledFeatures(0)
{}

ECode CAccessibilityInputFilter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAccessibilityManager* service)
{
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);
    InputFilter::Init(looper);

    mContext = context;
    mAms = (CAccessibilityManagerService*)service;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPm = IPowerManager::Probe(obj);

    return NOERROR;
}

ECode CAccessibilityInputFilter::OnInstalled()
{
    if (DEBUG) {
        Slogger::D(TAG, "Accessibility input filter installed.");
    }

    mInstalled = TRUE;
    DisableFeatures();
    EnableFeatures();
    return InputFilter::OnInstalled();
}

ECode CAccessibilityInputFilter::OnUninstalled()
{
    if (DEBUG) {
        Slogger::D(TAG, "Accessibility input filter uninstalled.");
    }
    mInstalled = FALSE;
    DisableFeatures();
    return InputFilter::OnUninstalled();
}

ECode CAccessibilityInputFilter::OnInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (DEBUG) {
        Slogger::D(TAG, "Received event: %p, policyFlags=0x%08x", policyFlags);
    }

    if (mEventHandler == NULL) {
        return InputFilter::OnInputEvent(event, policyFlags);
    }

    Int32 source;
    event->GetSource(&source);
    if (source != IInputDevice::SOURCE_TOUCHSCREEN) {
        return InputFilter::OnInputEvent(event, policyFlags);
    }

    if ((policyFlags & IWindowManagerPolicy::FLAG_PASS_TO_USER) == 0) {
        mEventHandler->Clear();
        return InputFilter::OnInputEvent(event, policyFlags);
    }

    Int32 deviceId;
    event->GetDeviceId(&deviceId);
    if (mCurrentDeviceId != deviceId) {
        if (mCurrentDeviceId != UNDEFINED_DEVICE_ID) {
            mEventHandler->Clear();
        }
        mCurrentDeviceId = deviceId;
    }

    if (mPm) {
        Int64 eventTime;
        event->GetEventTime(&eventTime);
        mPm->UserActivity(eventTime, FALSE);
    }

    AutoPtr<IMotionEvent> rawEvent = IMotionEvent::Probe(event);
    AutoPtr<IMotionEventHelper> helper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&helper);
    AutoPtr<IMotionEvent> transformedEvent;
    helper->Obtain(rawEvent, (IMotionEvent**)&transformedEvent);
    mEventHandler->OnMotionEvent(transformedEvent, rawEvent, policyFlags);
    transformedEvent->Recycle();
    return NOERROR;
}

ECode CAccessibilityInputFilter::OnMotionEvent(
    /* [in] */ IMotionEvent* transformedEvent,
    /* [in] */ IMotionEvent* rawEvent,
    /* [in] */ Int32 policyFlags)
{
    return InputFilter::SendInputEvent(transformedEvent, policyFlags);
}

ECode CAccessibilityInputFilter::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    // TODO Implement this to inject the accessibility event
    //      into the accessibility manager service similarly
    //      to how this is done for input events.
    return NOERROR;
}

ECode CAccessibilityInputFilter::SetNext(
    /* [in] */ IEventStreamTransformation* next)
{
    /* do nothing */
    return NOERROR;
}

ECode CAccessibilityInputFilter::Clear()
{
    /* do nothing */
    return NOERROR;
}

ECode CAccessibilityInputFilter::SetEnabledFeatures(
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

ECode CAccessibilityInputFilter::NotifyAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    if (mEventHandler != NULL) {
        mEventHandler->OnAccessibilityEvent(event);
    }
    return NOERROR;
}

void CAccessibilityInputFilter::EnableFeatures()
{
    if ((mEnabledFeatures & FLAG_FEATURE_SCREEN_MAGNIFIER) != 0) {
        mScreenMagnifier = new ScreenMagnifier(mContext);
        mEventHandler = (IEventStreamTransformation*)mScreenMagnifier.Get();
        mEventHandler->SetNext(THIS_PROBE(IEventStreamTransformation));
    }

    if ((mEnabledFeatures & FLAG_FEATURE_TOUCH_EXPLORATION) != 0) {
        mTouchExplorer = new TouchExplorer(mContext, mAms);
        mTouchExplorer->SetNext(THIS_PROBE(IEventStreamTransformation));
        if (mEventHandler != NULL) {
            mEventHandler->SetNext((IEventStreamTransformation*)mTouchExplorer);
        }
        else {
            mEventHandler = (IEventStreamTransformation*)mTouchExplorer;
        }
    }
}

void CAccessibilityInputFilter::DisableFeatures()
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
}

ECode CAccessibilityInputFilter::OnDestroy()
{
    /* ignore */
    return NOERROR;
}

ECode CAccessibilityInputFilter::Install(
    /* [in] */ IInputFilterHost* host)
{
    return InputFilter::Install(host);
}

ECode CAccessibilityInputFilter::Uninstall()
{
    return InputFilter::Uninstall();
}

ECode CAccessibilityInputFilter::FilterInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    return InputFilter::FilterInputEvent(event, policyFlags);
}

ECode CAccessibilityInputFilter::SendInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    return InputFilter::SendInputEvent(event, policyFlags);
}


} // Accessibility
} // Server
} // Droid
} // Elastos
