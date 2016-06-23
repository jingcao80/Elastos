#include "elastos/droid/internal/telephony/gsm/CGsmCellBroadcastHandler.h"
#include "elastos/droid/internal/telephony/gsm/GsmInboundSmsHandler.h"
#include "elastos/droid/internal/telephony/gsm/CSmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/CUsimDataDownloadHandler.h"
#include "elastos/droid/internal/telephony/CellBroadcastHandler.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::PhoneBase;
// TODO:
// using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(GsmInboundSmsHandler, InboundSmsHandler, IGsmInboundSmsHandler)

GsmInboundSmsHandler::GsmInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone)
    : InboundSmsHandler(String("GsmInboundSmsHandler"), context, storageMonitor, phone, CellBroadcastHandler::MakeCellBroadcastHandler(context, phone))
{
    // AutoPtr<IGsmCellBroadcastHandler> gcbh;
    // CGsmCellBroadcastHandler::MakeGsmCellBroadcastHandler(context, phone, (IGsmCellBroadcastHandler**)&gcbh);
    // InboundSmsHandler(String("GsmInboundSmsHandler"), context, storageMonitor, phone, gcbh);
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    ((PhoneBase*)phone)->mCi->SetOnNewGsmSms(handler, EVENT_NEW_SMS, NULL);
    CUsimDataDownloadHandler::New(((PhoneBase*)phone)->mCi, (IUsimDataDownloadHandler**)&mDataDownloadHandler);
}

ECode GsmInboundSmsHandler::MakeInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [out] */ IGsmInboundSmsHandler** result)
{
    AutoPtr<GsmInboundSmsHandler> handler = new GsmInboundSmsHandler(context, storageMonitor, phone);
    IStateMachine::Probe(handler)->Start();
    *result = IGsmInboundSmsHandler::Probe(handler);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode GsmInboundSmsHandler::UpdateMessageWaitingIndicator(
    /* [in] */ Int32 voicemailCount)
{
    // range check
    if (voicemailCount < 0) {
        voicemailCount = -1;
    }
    else if (voicemailCount > 0xff) {
        // TS 23.040 9.2.3.24.2
        // "The value 255 shall be taken to mean 255 or greater"
        voicemailCount = 0xff;
    }
    // update voice mail count in GsmPhone
    mPhone->SetVoiceMessageCount(voicemailCount);
    // store voice mail count in SIM & shared preferences
    AutoPtr<IUiccController> uicc = UiccController::GetInstance();
    Int32 id;
    mPhone->GetPhoneId(&id);
    AutoPtr<IIccRecords> records;
    uicc->GetIccRecords(id, IUiccController::APP_FAM_3GPP, (IIccRecords**)&records);
    if (records != NULL) {
        Log(String("updateMessageWaitingIndicator: updating SIM Records"));
        records->SetVoiceMessageWaiting(1, voicemailCount);
    }
    else {
        Log(String("updateMessageWaitingIndicator: SIM Records not found"));
    }
    StoreVoiceMailCount();
    return NOERROR;
}

void GsmInboundSmsHandler::OnQuitting()
{
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    ((PhoneBase*)mPhone.Get())->mCi->UnSetOnNewGsmSms(handler);
    mCellBroadcastHandler->Dispose();

    if (DBG) Log(String("unregistered for 3GPP SMS"));
    InboundSmsHandler::OnQuitting();     // release wakelock
}

Boolean GsmInboundSmsHandler::Is3gpp2()
{
    return FALSE;
}

Int32 GsmInboundSmsHandler::DispatchMessageRadioSpecific(
    /* [in] */ ISmsMessageBase* smsb)
{
    AutoPtr<ISmsMessage> sms = ISmsMessage::Probe(smsb);

    Boolean b;
    if (sms->IsTypeZero(&b), b) {
        // As per 3GPP TS 23.040 9.2.3.9, Type Zero messages should not be
        // Displayed/Stored/Notified. They should only be acknowledged.
        Log(String("Received short message type 0, Don't display or store it. Send Ack"));
        return 1; //IIntents::RESULT_SMS_HANDLED;
    }

    // Send SMS-PP data download messages to UICC. See 3GPP TS 31.111 section 7.1.1.
    if (((CSmsMessage*)sms.Get())->IsUsimDataDownload(&b), b) {
        AutoPtr<IUsimServiceTable> ust;
        IPhone::Probe(mPhone)->GetUsimServiceTable((IUsimServiceTable**)&ust);
        Int32 result;
        mDataDownloadHandler->HandleUsimDataDownload(ust, sms, &result);
        return result;
    }

    Boolean handled = FALSE;
    if (ISmsMessageBase::Probe(sms)->IsMWISetMessage(&b), b) {
        Int32 val;
        sms->GetNumOfVoicemails(&val);
        UpdateMessageWaitingIndicator(val);
        ISmsMessageBase::Probe(sms)->IsMwiDontStore(&handled);
        if (DBG) Log(String("Received voice mail indicator set SMS shouldStore=") + !handled);
    }
    else if (ISmsMessageBase::Probe(sms)->IsMWIClearMessage(&b), b) {
        UpdateMessageWaitingIndicator(0);
        ISmsMessageBase::Probe(sms)->IsMwiDontStore(&handled);
        if (DBG) Log(String("Received voice mail indicator clear SMS shouldStore=") + !handled);
    }
    if (handled) {
        return 1; //IIntents::RESULT_SMS_HANDLED;
    }

    MessageClass cls;
// TODO: Need ISmsMessage::GetMessageClass
    // sms->GetMessageClass(&cls);
    if ((mStorageMonitor->IsStorageAvailable(&b), b) &&
            cls != MessageClass_CLASS_0) {
        // It's a storable message and there's no storage available.  Bail.
        // (See TS 23.038 for a description of class 0 messages.)
        return 3; //IIntents::RESULT_SMS_OUT_OF_MEMORY;
    }

    return DispatchNormalMessage(smsb);
}

void GsmInboundSmsHandler::AcknowledgeLastIncomingSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 result,
    /* [in] */ IMessage* response)
{
    ((PhoneBase*)mPhone.Get())->mCi->AcknowledgeLastIncomingGsmSms(success, ResultToCause(result), response);
}

void GsmInboundSmsHandler::OnUpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    InboundSmsHandler::OnUpdatePhoneObject(phone);
    Log(String("onUpdatePhoneObject: dispose of old CellBroadcastHandler and make a new one"));
    mCellBroadcastHandler->Dispose();
    mCellBroadcastHandler = (CellBroadcastHandler*)(CGsmCellBroadcastHandler::MakeGsmCellBroadcastHandler(mContext, phone)).Get();
}

Int32 GsmInboundSmsHandler::ResultToCause(
    /* [in] */ Int32 rc)
{
    switch (rc) {
        case IActivity::RESULT_OK:
        case 1: //IIntents::RESULT_SMS_HANDLED:
            // Cause code is ignored on success.
            return 0;
        case 3: //IIntents::RESULT_SMS_OUT_OF_MEMORY:
            return ICommandsInterface::GSM_SMS_FAIL_CAUSE_MEMORY_CAPACITY_EXCEEDED;
        case 2: //IIntents::RESULT_SMS_GENERIC_ERROR:
        default:
            return ICommandsInterface::GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR;
    }
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos