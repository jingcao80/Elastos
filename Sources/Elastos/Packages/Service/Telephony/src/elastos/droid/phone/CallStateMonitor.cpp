
#include "elastos/droid/phone/CallLogger.h"

namespace Elastos {
namespace Droid {
namespace Phone {

const Int32 CallStateMonitor::PHONE_STATE_CHANGED = 1;
const Int32 CallStateMonitor::PHONE_NEW_RINGING_CONNECTION = 2;
const Int32 CallStateMonitor::PHONE_DISCONNECT = 3;
const Int32 CallStateMonitor::PHONE_UNKNOWN_CONNECTION_APPEARED = 4;
const Int32 CallStateMonitor::PHONE_STATE_DISPLAYINFO = 6;
const Int32 CallStateMonitor::PHONE_STATE_SIGNALINFO = 7;
const Int32 CallStateMonitor::PHONE_CDMA_CALL_WAITING = 8;
const Int32 CallStateMonitor::PHONE_ENHANCED_VP_ON = 9;
const Int32 CallStateMonitor::PHONE_ENHANCED_VP_OFF = 10;
const Int32 CallStateMonitor::PHONE_RINGBACK_TONE = 11;
const Int32 CallStateMonitor::PHONE_RESEND_MUTE = 12;
const Int32 CallStateMonitor::PHONE_ON_DIAL_CHARS = 13;

// Other events from call manager
const Int32 CallStateMonitor::EVENT_OTA_PROVISION_CHANGE = 20;

const String CallStateMonitor::LOG_TAG("CallStateMonitor");// = CallStateMonitor.class.getSimpleName();

static Boolean initDBG()
{
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    Int32 value;
    helper->GetInt32(String("ro.debuggable"), 0, &value);
    Boolean result = (PhoneGlobals::DBG_LEVEL >= 1) && (value == 1);
    return result;
}

const Boolean DBG = initDBG();

CallStateMonitor::CallStateMonitor(
    /* [in] */ ICallManager* callManager)
    : mCallManager(callManager)
{
    Handler::constructor();

    CArrayList::New((IArrayList**)&mRegisteredHandlers);
    RegisterForNotifications();
}

void CallStateMonitor::RegisterForNotifications()
{
    //
    // TODO: The lines commented out here can be removed as their associated functionality in
    // other files is removed.
    //
    //callManager.registerForNewRingingConnection(this, PHONE_NEW_RINGING_CONNECTION, null);
    //callManager.registerForPreciseCallStateChanged(this, PHONE_STATE_CHANGED, null);
    //callManager.registerForDisconnect(this, PHONE_DISCONNECT, null);
    //callManager.registerForUnknownConnection(this, PHONE_UNKNOWN_CONNECTION_APPEARED, null);
    mCallManager->RegisterForCdmaOtaStatusChange(this, EVENT_OTA_PROVISION_CHANGE, NULL);
    //callManager.registerForCallWaiting(this, PHONE_CDMA_CALL_WAITING, null);
    mCallManager->RegisterForDisplayInfo(this, PHONE_STATE_DISPLAYINFO, NULL);
    mCallManager->RegisterForSignalInfo(this, PHONE_STATE_SIGNALINFO, NULL);
    mCallManager->RegisterForInCallVoicePrivacyOn(this, PHONE_ENHANCED_VP_ON, NULL);
    mCallManager->RegisterForInCallVoicePrivacyOff(this, PHONE_ENHANCED_VP_OFF, NULL);
    //callManager.registerForRingbackTone(this, PHONE_RINGBACK_TONE, null);
    //callManager.registerForResendIncallMute(this, PHONE_RESEND_MUTE, null);
    //callManager.registerForPostDialCharacter(this, PHONE_ON_DIAL_CHARS, null);
}

ECode CallStateMonitor::AddListener(
    /* [in] */ IHandler* handler)
{
    Boolean res;
    if (handler != NULL && (mRegisteredHandlers->Contains(TO_IINTERFACE(handler), &res), !res)) {
        if (DBG) {
            StringBuilder sb;
            sb += "Adding Handler: ";
            sb += TO_CSTR(handler);
            Logger::D(LOG_TAG, sb.ToString());
        }

        mRegisteredHandlers->Add(TO_IINTERFACE(handler));
    }
    return NOERROR;
}

ECode CallStateMonitor::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (DBG) {
        StringBuilder sb;
        sb += "handleMessage(";
        Int32 what;
        msg->GetWhat(&what);
        sb += what;
        sb += ")";
        Logger::D(LOG_TAG, sb.ToString());
    }

    Int32 size;
    mRegisteredHandlers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mRegisteredHandlers->Get(i, (IInterface**)&obj);
        AutoPtr<IHandler> handler = IHandler::Probe(obj);
        handler->HandleMessage(msg);
    }
    return NOERROR;
}

ECode CallStateMonitor::UpdateAfterRadioTechnologyChange()
{
    if (DBG) {
        Logger::D(LOG_TAG, String("updateCallNotifierRegistrationsAfterRadioTechnologyChange..."));
    }

    // Unregister all events from the old obsolete phone
    //callManager.unregisterForNewRingingConnection(this);
    //callManager.unregisterForPreciseCallStateChanged(this);
    //callManager.unregisterForDisconnect(this);
    //callManager.unregisterForUnknownConnection(this);
    //callManager.unregisterForCallWaiting(this);
    mCallManager->UnregisterForDisplayInfo(this);
    mCallManager->UnregisterForSignalInfo(this);
    mCallManager->UnregisterForCdmaOtaStatusChange(this);
    //callManager.unregisterForRingbackTone(this);
    //callManager.unregisterForResendIncallMute(this);
    mCallManager->UnregisterForInCallVoicePrivacyOn(this);
    mCallManager->UnregisterForInCallVoicePrivacyOff(this);
    //callManager.unregisterForPostDialCharacter(this);

    RegisterForNotifications();
    return NOERROR;
}


} // namespace Phone
} // namespace Droid
} // namespace Elastos