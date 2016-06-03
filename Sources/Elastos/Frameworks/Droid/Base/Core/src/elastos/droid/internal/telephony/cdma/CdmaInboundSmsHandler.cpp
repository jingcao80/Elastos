#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaInboundSmsHandler.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                        CdmaInboundSmsHandler
//=====================================================================
CAR_INTERFACE_IMPL(CdmaInboundSmsHandler, /*TODO InboundSmsHandler*/ Object, ICdmaInboundSmsHandler);

AutoPtr<ICdmaInboundSmsHandler> CdmaInboundSmsHandler::MakeInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ICdmaSMSDispatcher* smsDispatcher)
{
    // ==================before translated======================
    // CdmaInboundSmsHandler handler = new CdmaInboundSmsHandler(context, storageMonitor,
    //         phone, smsDispatcher);
    // handler.start();
    // return handler;
    assert(0);
    AutoPtr<ICdmaInboundSmsHandler> empty;
    return empty;
}

void CdmaInboundSmsHandler::OnQuitting()
{
    // ==================before translated======================
    // mPhone.mCi.unSetOnNewCdmaSms(getHandler());
    // mCellBroadcastHandler.dispose();
    //
    // if (DBG) log("unregistered for 3GPP2 SMS");
    // super.onQuitting();
    assert(0);
}

Boolean CdmaInboundSmsHandler::Is3gpp2()
{
    // ==================before translated======================
    // return true;
    assert(0);
    return FALSE;
}

Int32 CdmaInboundSmsHandler::DispatchMessageRadioSpecific(
    /* [in] */ ISmsMessageBase* smsb)
{
    // ==================before translated======================
    // if (isInEmergencyCallMode()) {
    //     return Activity.RESULT_OK;
    // }
    //
    // SmsMessage sms = (SmsMessage) smsb;
    // boolean isBroadcastType = (SmsEnvelope.MESSAGE_TYPE_BROADCAST == sms.getMessageType());
    //
    // // Handle CMAS emergency broadcast messages.
    // if (isBroadcastType) {
    //     log("Broadcast type message");
    //     SmsCbMessage cbMessage = sms.parseBroadcastSms();
    //     if (cbMessage != null) {
    //         mCellBroadcastHandler.dispatchSmsMessage(cbMessage);
    //     } else {
    //         loge("error trying to parse broadcast SMS");
    //     }
    //     return Intents.RESULT_SMS_HANDLED;
    // }
    //
    // // Initialize fingerprint field, and see if we have a network duplicate SMS.
    // mLastDispatchedSmsFingerprint = sms.getIncomingSmsFingerprint();
    // if (mLastAcknowledgedSmsFingerprint != null &&
    //         Arrays.equals(mLastDispatchedSmsFingerprint, mLastAcknowledgedSmsFingerprint)) {
    //     return Intents.RESULT_SMS_HANDLED;
    // }
    //
    // // Decode BD stream and set sms variables.
    // sms.parseSms();
    // int teleService = sms.getTeleService();
    //
    // switch (teleService) {
    //     case SmsEnvelope.TELESERVICE_VMN:
    //     case SmsEnvelope.TELESERVICE_MWI:
    //         // handle voicemail indication
    //         handleVoicemailTeleservice(sms);
    //         return Intents.RESULT_SMS_HANDLED;
    //
    //     case SmsEnvelope.TELESERVICE_WMT:
    //     case SmsEnvelope.TELESERVICE_WEMT:
    //         if (sms.isStatusReportMessage()) {
    //             mSmsDispatcher.sendStatusReportMessage(sms);
    //             return Intents.RESULT_SMS_HANDLED;
    //         }
    //         break;
    //
    //     case SmsEnvelope.TELESERVICE_SCPT:
    //         mServiceCategoryProgramHandler.dispatchSmsMessage(sms);
    //         return Intents.RESULT_SMS_HANDLED;
    //
    //     case SmsEnvelope.TELESERVICE_WAP:
    //         // handled below, after storage check
    //         break;
    //
    //     case SmsEnvelope.TELESERVICE_CT_WAP:
    //         // handled below, after TELESERVICE_WAP
    //         break;
    //
    //     default:
    //         loge("unsupported teleservice 0x" + Integer.toHexString(teleService));
    //         return Intents.RESULT_SMS_UNSUPPORTED;
    // }
    //
    // if (!mStorageMonitor.isStorageAvailable() &&
    //         sms.getMessageClass() != SmsConstants.MessageClass.CLASS_0) {
    //     // It's a storable message and there's no storage available.  Bail.
    //     // (See C.S0015-B v2.0 for a description of "Immediate Display"
    //     // messages, which we represent as CLASS_0.)
    //     return Intents.RESULT_SMS_OUT_OF_MEMORY;
    // }
    //
    // if (SmsEnvelope.TELESERVICE_WAP == teleService) {
    //     return processCdmaWapPdu(sms.getUserData(), sms.mMessageRef,
    //             sms.getOriginatingAddress(), sms.getTimestampMillis());
    // } else if (SmsEnvelope.TELESERVICE_CT_WAP == teleService) {
    //     /* China Telecom WDP header contains Message identifier
    //        and User data subparametrs extract these fields */
    //     if (!sms.processCdmaCTWdpHeader(sms)) {
    //         return Intents.RESULT_SMS_HANDLED;
    //     }
    //     return processCdmaWapPdu(sms.getUserData(), sms.mMessageRef,
    //             sms.getOriginatingAddress(), sms.getTimestampMillis());
    // }
    //
    // return dispatchNormalMessage(smsb);
    assert(0);
    return 0;
}

void CdmaInboundSmsHandler::AcknowledgeLastIncomingSms(
    /* [in] */ Boolean success,
    /* [in] */ Int32 result,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // if (isInEmergencyCallMode()) {
    //     return;
    // }
    //
    // int causeCode = resultToCause(result);
    // mPhone.mCi.acknowledgeLastIncomingCdmaSms(success, causeCode, response);
    //
    // if (causeCode == 0) {
    //     mLastAcknowledgedSmsFingerprint = mLastDispatchedSmsFingerprint;
    // }
    // mLastDispatchedSmsFingerprint = null;
    assert(0);
}

void CdmaInboundSmsHandler::OnUpdatePhoneObject(
    /* [in] */ IPhoneBase* phone)
{
    // ==================before translated======================
    // super.onUpdatePhoneObject(phone);
    // mCellBroadcastHandler.updatePhoneObject(phone);
    assert(0);
}

CdmaInboundSmsHandler::CdmaInboundSmsHandler(
    /* [in] */ IContext* context,
    /* [in] */ ISmsStorageMonitor* storageMonitor,
    /* [in] */ IPhoneBase* phone,
    /* [in] */ ICdmaSMSDispatcher* smsDispatcher)
{
    // ==================before translated======================
    // super("CdmaInboundSmsHandler", context, storageMonitor, phone,
    //         CellBroadcastHandler.makeCellBroadcastHandler(context, phone));
    // mSmsDispatcher = smsDispatcher;
    // mServiceCategoryProgramHandler = CdmaServiceCategoryProgramHandler.makeScpHandler(context,
    //         phone.mCi);
    // phone.mCi.setOnNewCdmaSms(getHandler(), EVENT_NEW_SMS, null);
}

Boolean CdmaInboundSmsHandler::IsInEmergencyCallMode()
{
    // ==================before translated======================
    // String inEcm = SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE, "false");
    // return "true".equals(inEcm);
    assert(0);
    return FALSE;
}

Int32 CdmaInboundSmsHandler::ResultToCause(
    /* [in] */ Int32 rc)
{
    // ==================before translated======================
    // switch (rc) {
    // case Activity.RESULT_OK:
    // case Intents.RESULT_SMS_HANDLED:
    //     // Cause code is ignored on success.
    //     return 0;
    // case Intents.RESULT_SMS_OUT_OF_MEMORY:
    //     return CommandsInterface.CDMA_SMS_FAIL_CAUSE_RESOURCE_SHORTAGE;
    // case Intents.RESULT_SMS_UNSUPPORTED:
    //     return CommandsInterface.CDMA_SMS_FAIL_CAUSE_INVALID_TELESERVICE_ID;
    // case Intents.RESULT_SMS_GENERIC_ERROR:
    // default:
    //     return CommandsInterface.CDMA_SMS_FAIL_CAUSE_ENCODING_PROBLEM;
    // }
    assert(0);
    return 0;
}

void CdmaInboundSmsHandler::HandleVoicemailTeleservice(
    /* [in] */ ISmsMessage* sms)
{
    // ==================before translated======================
    // int voicemailCount = sms.getNumOfVoicemails();
    // if (DBG) log("Voicemail count=" + voicemailCount);
    //
    // mPhone.setVoiceMessageWaiting(1, voicemailCount);
    // // range check
    // if (voicemailCount < 0) {
    //     voicemailCount = -1;
    // } else if (voicemailCount > 99) {
    //     // C.S0015-B v2, 4.5.12
    //     // range: 0-99
    //     voicemailCount = 99;
    // }
    // // update voice mail count in phone
    // mPhone.setVoiceMessageCount(voicemailCount);
    // // store voice mail count in preferences
    // storeVoiceMailCount();
    assert(0);
}

Int32 CdmaInboundSmsHandler::ProcessCdmaWapPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ Int32 referenceNumber,
    /* [in] */ const String& address,
    /* [in] */ Int64 timestamp)
{
    // ==================before translated======================
    // int index = 0;
    //
    // int msgType = (0xFF & pdu[index++]);
    // if (msgType != 0) {
    //     log("Received a WAP SMS which is not WDP. Discard.");
    //     return Intents.RESULT_SMS_HANDLED;
    // }
    // int totalSegments = (0xFF & pdu[index++]);   // >= 1
    // int segment = (0xFF & pdu[index++]);         // >= 0
    //
    // if (segment >= totalSegments) {
    //     loge("WDP bad segment #" + segment + " expecting 0-" + (totalSegments - 1));
    //     return Intents.RESULT_SMS_HANDLED;
    // }
    //
    // // Only the first segment contains sourcePort and destination Port
    // int sourcePort = 0;
    // int destinationPort = 0;
    // if (segment == 0) {
    //     //process WDP segment
    //     sourcePort = (0xFF & pdu[index++]) << 8;
    //     sourcePort |= 0xFF & pdu[index++];
    //     destinationPort = (0xFF & pdu[index++]) << 8;
    //     destinationPort |= 0xFF & pdu[index++];
    //     // Some carriers incorrectly send duplicate port fields in omadm wap pushes.
    //     // If configured, check for that here
    //     if (mCheckForDuplicatePortsInOmadmWapPush) {
    //         if (checkDuplicatePortOmadmWapPush(pdu, index)) {
    //             index = index + 4; // skip duplicate port fields
    //         }
    //     }
    // }
    //
    // // Lookup all other related parts
    // log("Received WAP PDU. Type = " + msgType + ", originator = " + address
    //         + ", src-port = " + sourcePort + ", dst-port = " + destinationPort
    //         + ", ID = " + referenceNumber + ", segment# = " + segment + '/' + totalSegments);
    //
    // // pass the user data portion of the PDU to the shared handler in SMSDispatcher
    // byte[] userData = new byte[pdu.length - index];
    // System.arraycopy(pdu, index, userData, 0, pdu.length - index);
    //
    // InboundSmsTracker tracker = new InboundSmsTracker(userData, timestamp, destinationPort,
    //         true, address, referenceNumber, segment, totalSegments, true);
    //
    // return addTrackerToRawTableAndSendMessage(tracker);
    assert(0);
    return 0;
}

Boolean CdmaInboundSmsHandler::CheckDuplicatePortOmadmWapPush(
    /* [in] */ ArrayOf<Byte>* origPdu,
    /* [in] */ Int32 index)
{
    // ==================before translated======================
    // index += 4;
    // byte[] omaPdu = new byte[origPdu.length - index];
    // System.arraycopy(origPdu, index, omaPdu, 0, omaPdu.length);
    //
    // WspTypeDecoder pduDecoder = new WspTypeDecoder(omaPdu);
    // int wspIndex = 2;
    //
    // // Process header length field
    // if (!pduDecoder.decodeUintvarInteger(wspIndex)) {
    //     return false;
    // }
    //
    // wspIndex += pduDecoder.getDecodedDataLength();  // advance to next field
    //
    // // Process content type field
    // if (!pduDecoder.decodeContentType(wspIndex)) {
    //     return false;
    // }
    //
    // String mimeType = pduDecoder.getValueString();
    // return (WspTypeDecoder.CONTENT_TYPE_B_PUSH_SYNCML_NOTI.equals(mimeType));
    assert(0);
    return FALSE;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
