#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/telephony/CellBroadcastHandler.h"
#include "elastos/droid/internal/telephony/cdma/CdmaInboundSmsHandler.h"
#include "elastos/droid/internal/telephony/cdma/CdmaServiceCategoryProgramHandler.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSMSDispatcher.h"
#include "elastos/droid/internal/telephony/CWspTypeDecoder.h"
#include "elastos/droid/internal/telephony/InboundSmsTracker.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/cdma/SmsMessage.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Internal::Telephony::Cdma::SmsMessage;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ISmsEnvelope;
using Elastos::Droid::Internal::Telephony::InboundSmsTracker;
using Elastos::Droid::Internal::Telephony::PhoneBase;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::ITelephonySmsIntents;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                        CdmaInboundSmsHandler
//=====================================================================
CAR_INTERFACE_IMPL(CdmaInboundSmsHandler, InboundSmsHandler, ICdmaInboundSmsHandler);

AutoPtr<ICdmaInboundSmsHandler> CdmaInboundSmsHandler::MakeInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ICdmaSMSDispatcher* smsDispatcher)
{
    AutoPtr<CdmaInboundSmsHandler> handler = new CdmaInboundSmsHandler(
            context, storageMonitor, phone, smsDispatcher);
    handler->Start();
    return ICdmaInboundSmsHandler::Probe(handler);
}

void CdmaInboundSmsHandler::OnQuitting()
{
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    ((PhoneBase*)mPhone.Get())->mCi->UnSetOnNewCdmaSms(handler);
    mCellBroadcastHandler->Dispose();

    if (DBG) Log(String("unregistered for 3GPP2 SMS"));
    InboundSmsHandler::OnQuitting();
}

Boolean CdmaInboundSmsHandler::Is3gpp2()
{
    return TRUE;
}

Int32 CdmaInboundSmsHandler::DispatchMessageRadioSpecific(
    /* [in] */ ISmsMessageBase* smsb)
{
    if (IsInEmergencyCallMode()) {
        return IActivity::RESULT_OK;
    }

    AutoPtr<SmsMessage> sms = (SmsMessage*)smsb;
    Int32 type;
    sms->GetMessageType(&type);
    Boolean isBroadcastType = (ISmsEnvelope::MESSAGE_TYPE_BROADCAST == type);

    // Handle CMAS emergency broadcast messages.
    if (isBroadcastType) {
        Log(String("Broadcast type message"));
        AutoPtr<ISmsCbMessage> cbMessage;
        sms->ParseBroadcastSms((ISmsCbMessage**)&cbMessage);
        if (cbMessage != NULL) {
            IWakeLockStateMachine::Probe(mCellBroadcastHandler)->DispatchSmsMessage(cbMessage);
        }
        else {
            Loge(String("error trying to parse broadcast SMS"));
        }
        return ITelephonySmsIntents::RESULT_SMS_HANDLED;
    }

    // Initialize fingerprint field, and see if we have a network duplicate SMS.
    sms->GetIncomingSmsFingerprint((ArrayOf<Byte>**)&mLastDispatchedSmsFingerprint);
    if (mLastAcknowledgedSmsFingerprint != NULL &&
            Arrays::Equals(mLastDispatchedSmsFingerprint, mLastAcknowledgedSmsFingerprint)) {
        return ITelephonySmsIntents::RESULT_SMS_HANDLED;
    }

    // Decode BD stream and set sms variables.
    sms->ParseSms();
    Int32 teleService;
    sms->GetTeleService(&teleService);

    Boolean b;
    switch (teleService) {
        case ISmsEnvelope::TELESERVICE_VMN:
        case ISmsEnvelope::TELESERVICE_MWI:
            // handle voicemail indication
            HandleVoicemailTeleservice(ISmsMessage::Probe(sms));
            return ITelephonySmsIntents::RESULT_SMS_HANDLED;

        case ISmsEnvelope::TELESERVICE_WMT:
        case ISmsEnvelope::TELESERVICE_WEMT:
            if (sms->IsStatusReportMessage(&b), b) {
                ((CdmaSMSDispatcher*)mSmsDispatcher.Get())->SendStatusReportMessage(ISmsMessage::Probe(sms));
                return ITelephonySmsIntents::RESULT_SMS_HANDLED;
            }
            break;

        case ISmsEnvelope::TELESERVICE_SCPT:
            IWakeLockStateMachine::Probe(mServiceCategoryProgramHandler)->DispatchSmsMessage(ISmsMessage::Probe(sms));
            return ITelephonySmsIntents::RESULT_SMS_HANDLED;

        case ISmsEnvelope::TELESERVICE_WAP:
            // handled below, after storage check
            break;

        case ISmsEnvelope::TELESERVICE_CT_WAP:
            // handled below, after TELESERVICE_WAP
            break;

        default:
            Loge(String("unsupported teleservice 0x") + StringUtils::ToHexString(teleService));
            return ITelephonySmsIntents::RESULT_SMS_UNSUPPORTED;
    }

    MessageClass mc;
    sms->GetMessageClass(&mc);
    if ((mStorageMonitor->IsStorageAvailable(&b), !b) &&
            mc != MessageClass_CLASS_0) {
        // It's a storable message and there's no storage available.  Bail.
        // (See C.S0015-B v2.0 for a description of "Immediate Display"
        // messages, which we represent as CLASS_0.)
        return ITelephonySmsIntents::RESULT_SMS_OUT_OF_MEMORY;
    }

    AutoPtr<ArrayOf<Byte> > userData;
    sms->GetUserData((ArrayOf<Byte>**)&userData);
    String addr;
    sms->GetOriginatingAddress(&addr);
    Int64 ts;
    sms->GetTimestampMillis(&ts);
    if (ISmsEnvelope::TELESERVICE_WAP == teleService) {
        return ProcessCdmaWapPdu(userData, sms->mMessageRef, addr, ts);
    }
    else if (ISmsEnvelope::TELESERVICE_CT_WAP == teleService) {
        /* China Telecom WDP header contains Message identifier
           and User data subparametrs extract these fields */
        if (!sms->ProcessCdmaCTWdpHeader(ISmsMessage::Probe(sms))) {
            return ITelephonySmsIntents::RESULT_SMS_HANDLED;
        }
        return ProcessCdmaWapPdu(userData, sms->mMessageRef, addr, ts);
    }

    return DispatchNormalMessage(smsb);
}

void CdmaInboundSmsHandler::AcknowledgeLastIncomingSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 result,
    /* [in] */ IMessage* response)
{
    if (IsInEmergencyCallMode()) {
        return;
    }

    Int32 causeCode = ResultToCause(result);
    ((PhoneBase*)mPhone.Get())->mCi->AcknowledgeLastIncomingCdmaSms(success, causeCode, response);

    if (causeCode == 0) {
        mLastAcknowledgedSmsFingerprint = mLastDispatchedSmsFingerprint;
    }
    mLastDispatchedSmsFingerprint = NULL;
}

void CdmaInboundSmsHandler::OnUpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    InboundSmsHandler::OnUpdatePhoneObject(phone);
    mCellBroadcastHandler->UpdatePhoneObject(phone);
}

CdmaInboundSmsHandler::CdmaInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ICdmaSMSDispatcher* smsDispatcher)
    : InboundSmsHandler(String("CdmaInboundSmsHandler"), context, storageMonitor, phone, CellBroadcastHandler::MakeCellBroadcastHandler(context, phone))
    , mSmsDispatcher(smsDispatcher)
{
    mServiceCategoryProgramHandler = CdmaServiceCategoryProgramHandler::MakeScpHandler(context, ((PhoneBase*)phone)->mCi);
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    ((PhoneBase*)phone)->mCi->SetOnNewCdmaSms(handler, EVENT_NEW_SMS, NULL);
}

Boolean CdmaInboundSmsHandler::IsInEmergencyCallMode()
{
    String inEcm;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE,
            String("FALSE"), &inEcm);
    return inEcm.Equals("TRUE");
}

Int32 CdmaInboundSmsHandler::ResultToCause(
    /* [in] */ Int32 rc)
{
    switch (rc) {
    case IActivity::RESULT_OK:
    case ITelephonySmsIntents::RESULT_SMS_HANDLED:
        // Cause code is ignored on success.
        return 0;
    case ITelephonySmsIntents::RESULT_SMS_OUT_OF_MEMORY:
        return ICommandsInterface::CDMA_SMS_FAIL_CAUSE_RESOURCE_SHORTAGE;
    case ITelephonySmsIntents::RESULT_SMS_UNSUPPORTED:
        return ICommandsInterface::CDMA_SMS_FAIL_CAUSE_INVALID_TELESERVICE_ID;
    case ITelephonySmsIntents::RESULT_SMS_GENERIC_ERROR:
    default:
        return ICommandsInterface::CDMA_SMS_FAIL_CAUSE_ENCODING_PROBLEM;
    }
}

void CdmaInboundSmsHandler::HandleVoicemailTeleservice(
    /* [in] */ ISmsMessage* sms)
{
    Int32 voicemailCount;
    ((SmsMessage*)sms)->GetNumOfVoicemails(&voicemailCount);
    if (DBG) Log(String("Voicemail count=") + StringUtils::ToString(voicemailCount));

    IPhone::Probe(mPhone)->SetVoiceMessageWaiting(1, voicemailCount);
    // range check
    if (voicemailCount < 0) {
        voicemailCount = -1;
    }
    else if (voicemailCount > 99) {
        // C.S0015-B v2, 4.5.12
        // range: 0-99
        voicemailCount = 99;
    }
    // update voice mail count in phone
    mPhone->SetVoiceMessageCount(voicemailCount);
    // store voice mail count in preferences
    StoreVoiceMailCount();
}

Int32 CdmaInboundSmsHandler::ProcessCdmaWapPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 referenceNumber,
    /* [in] */ const String& address,
    /* [in] */ Int64 timestamp)
{
    Int32 index = 0;

    Int32 msgType = (0xFF & (*pdu)[index++]);
    if (msgType != 0) {
        Log(String("Received a WAP SMS which is not WDP. Discard."));
        return ITelephonySmsIntents::RESULT_SMS_HANDLED;
    }
    Int32 totalSegments = (0xFF & (*pdu)[index++]);   // >= 1
    Int32 segment = (0xFF & (*pdu)[index++]);         // >= 0

    if (segment >= totalSegments) {
        Loge(String("WDP bad segment #") + StringUtils::ToString(segment) + " expecting 0-" + StringUtils::ToString((totalSegments - 1)));
        return ITelephonySmsIntents::RESULT_SMS_HANDLED;
    }

    // Only the first segment contains sourcePort and destination Port
    Int32 sourcePort = 0;
    Int32 destinationPort = 0;
    if (segment == 0) {
        //process WDP segment
        sourcePort = (0xFF & (*pdu)[index++]) << 8;
        sourcePort |= 0xFF & (*pdu)[index++];
        destinationPort = (0xFF & (*pdu)[index++]) << 8;
        destinationPort |= 0xFF & (*pdu)[index++];
        // Some carriers incorrectly send duplicate port fields in omadm wap pushes.
        // If configured, check for that here
        if (mCheckForDuplicatePortsInOmadmWapPush) {
            if (CheckDuplicatePortOmadmWapPush(pdu, index)) {
                index = index + 4; // skip duplicate port fields
            }
        }
    }

    // Lookup all other related parts
    Log(String("Received WAP PDU. Type = ") + StringUtils::ToString(msgType)
            + ", originator = " + address
            + ", src-port = " + StringUtils::ToString(sourcePort)
            + ", dst-port = " + StringUtils::ToString(destinationPort)
            + ", ID = " + StringUtils::ToString(referenceNumber)
            + ", segment# = " + StringUtils::ToString(segment)
            + "/" + StringUtils::ToString(totalSegments));

    // pass the user data portion of the PDU to the shared handler in SMSDispatcher
    AutoPtr<ArrayOf<Byte> > userData = ArrayOf<Byte>::Alloc(pdu->GetLength() - index);
    userData->Copy(0, pdu, index, pdu->GetLength() - index);

    AutoPtr<InboundSmsTracker> tracker = new InboundSmsTracker(userData, timestamp, destinationPort,
            TRUE, address, referenceNumber, segment, totalSegments, TRUE);

    return AddTrackerToRawTableAndSendMessage(tracker);
}

Boolean CdmaInboundSmsHandler::CheckDuplicatePortOmadmWapPush(
    /* [in] */ ArrayOf<Byte>* origPdu,
    /* [in] */ Int32 index)
{
    index += 4;
    AutoPtr<ArrayOf<Byte> > omaPdu = ArrayOf<Byte>::Alloc(origPdu->GetLength() - index);
    omaPdu->Copy(0, origPdu, index, omaPdu->GetLength());

    AutoPtr<IWspTypeDecoder> pduDecoder;
    CWspTypeDecoder::New(omaPdu, (IWspTypeDecoder**)&pduDecoder);
    Int32 wspIndex = 2;

    // Process header length field
    Boolean b;
    if (pduDecoder->DecodeUintvarInteger(wspIndex, &b), !b) {
        return FALSE;
    }

    Int32 val;
    pduDecoder->GetDecodedDataLength(&val);
    wspIndex += val;  // advance to next field

    // Process content type field
    if (pduDecoder->DecodeContentType(wspIndex, &b), !b) {
        return FALSE;
    }

    String mimeType;
    pduDecoder->GetValueString(&mimeType);
    return mimeType.Equals(IWspTypeDecoder::CONTENT_TYPE_B_PUSH_SYNCML_NOTI);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
