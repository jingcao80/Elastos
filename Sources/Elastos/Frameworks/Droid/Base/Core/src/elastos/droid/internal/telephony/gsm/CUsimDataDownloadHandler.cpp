#include "elastos/droid/internal/telephony/gsm/CUsimDataDownloadHandler.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
// TODO:
// using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CUsimDataDownloadHandler::TAG("UsimDataDownloadHandler");
const Int32 CUsimDataDownloadHandler::BER_SMS_PP_DOWNLOAD_TAG;
const Int32 CUsimDataDownloadHandler::DEV_ID_UICC;
const Int32 CUsimDataDownloadHandler::DEV_ID_NETWORK;
const Int32 CUsimDataDownloadHandler::EVENT_START_DATA_DOWNLOAD;
const Int32 CUsimDataDownloadHandler::EVENT_SEND_ENVELOPE_RESPONSE;
const Int32 CUsimDataDownloadHandler::EVENT_WRITE_SMS_COMPLETE;

CAR_INTERFACE_IMPL(CUsimDataDownloadHandler, Handler, IUsimDataDownloadHandler)

CAR_OBJECT_IMPL(CUsimDataDownloadHandler)

CUsimDataDownloadHandler::CUsimDataDownloadHandler()
{
}

ECode CUsimDataDownloadHandler::constructor()
{
    return NOERROR;
}

ECode CUsimDataDownloadHandler::constructor(
    /* [in] */ ICommandsInterface* commandsInterface)
{
    mCi = commandsInterface;
    return NOERROR;
}

ECode CUsimDataDownloadHandler::HandleUsimDataDownload(
     /* [in] */ IUsimServiceTable* ust,
     /* [in] */ ISmsMessage* smsMessage,
     /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // // If we receive an SMS-PP message before the UsimServiceTable has been loaded,
    // // assume that the data download service is not present. This is very unlikely to
    // // happen because the IMS connection will not be established until after the ISIM
    // // records have been loaded, after the USIM service table has been loaded.
    // if (ust != null && ust.isAvailable(
    //         UsimServiceTable.UsimService.DATA_DL_VIA_SMS_PP)) {
    //     Rlog.d(TAG, "Received SMS-PP data download, sending to UICC.");
    //     return startDataDownload(smsMessage);
    // } else {
    //     Rlog.d(TAG, "DATA_DL_VIA_SMS_PP service not available, storing message to UICC.");
    //     String smsc = IccUtils.bytesToHexString(
    //             PhoneNumberUtils.networkPortionToCalledPartyBCDWithLength(
    //                     smsMessage.getServiceCenterAddress()));
    //     mCi.writeSmsToSim(SmsManager.STATUS_ON_ICC_UNREAD, smsc,
    //             IccUtils.bytesToHexString(smsMessage.getPdu()),
    //             obtainMessage(EVENT_WRITE_SMS_COMPLETE));
    //     return Activity.RESULT_OK;  // acknowledge after response from write to USIM
    // }
    return NOERROR;
}

ECode CUsimDataDownloadHandler::StartDataDownload(
     /* [in] */ ISmsMessage* smsMessage,
     /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_START_DATA_DOWNLOAD, smsMessage, (IMessage**)&msg);
    Boolean b;
    if (SendMessage(msg, &b), b) {
        *result = IActivity::RESULT_OK;  // we will send SMS ACK/ERROR based on UICC response
    }
    else {
        Logger::E(TAG, "startDataDownload failed to send message to start data download.");
        *result = 2; //IIntents::RESULT_SMS_GENERIC_ERROR;
    }
    return NOERROR;
}

ECode CUsimDataDownloadHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    assert(0);
    // AsyncResult ar;

    // switch (msg.what) {
    //     case EVENT_START_DATA_DOWNLOAD:
    //         handleDataDownload((SmsMessage) msg.obj);
    //         break;

    //     case EVENT_SEND_ENVELOPE_RESPONSE:
    //         ar = (AsyncResult) msg.obj;

    //         if (ar.exception != null) {
    //             Rlog.e(TAG, "UICC Send Envelope failure, exception: " + ar.exception);
    //             acknowledgeSmsWithError(
    //                     CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR);
    //             return;
    //         }

    //         int[] dcsPid = (int[]) ar.userObj;
    //         sendSmsAckForEnvelopeResponse((IccIoResult) ar.result, dcsPid[0], dcsPid[1]);
    //         break;

    //     case EVENT_WRITE_SMS_COMPLETE:
    //         ar = (AsyncResult) msg.obj;
    //         if (ar.exception == null) {
    //             Rlog.d(TAG, "Successfully wrote SMS-PP message to UICC");
    //             mCi.acknowledgeLastIncomingGsmSms(true, 0, null);
    //         } else {
    //             Rlog.d(TAG, "Failed to write SMS-PP message to UICC", ar.exception);
    //             mCi.acknowledgeLastIncomingGsmSms(false,
    //                     CommandsInterface.GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR, null);
    //         }
    //         break;

    //     default:
    //         Rlog.e(TAG, "Ignoring unexpected message, what=" + msg.what);
    // }
    return NOERROR;
}

void CUsimDataDownloadHandler::HandleDataDownload(
    /* [in] */ ISmsMessage* smsMessage)
{
    assert(0);
    // int dcs = smsMessage.getDataCodingScheme();
    // int pid = smsMessage.getProtocolIdentifier();
    // byte[] pdu = smsMessage.getPdu();           // includes SC address

    // int scAddressLength = pdu[0] & 0xff;
    // int tpduIndex = scAddressLength + 1;        // start of TPDU
    // int tpduLength = pdu.length - tpduIndex;

    // int bodyLength = getEnvelopeBodyLength(scAddressLength, tpduLength);

    // // Add 1 byte for SMS-PP download tag and 1-2 bytes for BER-TLV length.
    // // See ETSI TS 102 223 Annex C for encoding of length and tags.
    // int totalLength = bodyLength + 1 + (bodyLength > 127 ? 2 : 1);

    // byte[] envelope = new byte[totalLength];
    // int index = 0;

    // // SMS-PP download tag and length (assumed to be < 256 bytes).
    // envelope[index++] = (byte) BER_SMS_PP_DOWNLOAD_TAG;
    // if (bodyLength > 127) {
    //     envelope[index++] = (byte) 0x81;    // length 128-255 encoded as 0x81 + length
    // }
    // envelope[index++] = (byte) bodyLength;

    // // Device identities TLV
    // envelope[index++] = (byte) (0x80 | ComprehensionTlvTag.DEVICE_IDENTITIES.value());
    // envelope[index++] = (byte) 2;
    // envelope[index++] = (byte) DEV_ID_NETWORK;
    // envelope[index++] = (byte) DEV_ID_UICC;

    // // Address TLV (if present). Encoded length is assumed to be < 127 bytes.
    // if (scAddressLength != 0) {
    //     envelope[index++] = (byte) ComprehensionTlvTag.ADDRESS.value();
    //     envelope[index++] = (byte) scAddressLength;
    //     System.arraycopy(pdu, 1, envelope, index, scAddressLength);
    //     index += scAddressLength;
    // }

    // // SMS TPDU TLV. Length is assumed to be < 256 bytes.
    // envelope[index++] = (byte) (0x80 | ComprehensionTlvTag.SMS_TPDU.value());
    // if (tpduLength > 127) {
    //     envelope[index++] = (byte) 0x81;    // length 128-255 encoded as 0x81 + length
    // }
    // envelope[index++] = (byte) tpduLength;
    // System.arraycopy(pdu, tpduIndex, envelope, index, tpduLength);
    // index += tpduLength;

    // // Verify that we calculated the payload size correctly.
    // if (index != envelope.length) {
    //     Rlog.e(TAG, "startDataDownload() calculated incorrect envelope length, aborting.");
    //     acknowledgeSmsWithError(CommandsInterface.GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR);
    //     return;
    // }

    // String encodedEnvelope = IccUtils.bytesToHexString(envelope);
    // mCi.sendEnvelopeWithStatus(encodedEnvelope, obtainMessage(
    //         EVENT_SEND_ENVELOPE_RESPONSE, new int[]{ dcs, pid }));
}

Int32 CUsimDataDownloadHandler::GetEnvelopeBodyLength(
    /* [in] */ Int32 scAddressLength,
    /* [in] */ Int32 tpduLength)
{
    // Add 4 bytes for device identities TLV + 1 byte for SMS TPDU tag byte
    Int32 length = tpduLength + 5;
    // Add 1 byte for TPDU length, or 2 bytes if length > 127
    length += (tpduLength > 127 ? 2 : 1);
    // Add length of address tag, if present (+ 2 bytes for tag and length)
    if (scAddressLength != 0) {
        length = length + 2 + scAddressLength;
    }
    return length;
}

void CUsimDataDownloadHandler::SendSmsAckForEnvelopeResponse(
    /* [in] */ IIccIoResult* response,
    /* [in] */ Int32 dcs,
    /* [in] */ Int32 pid)
{
    assert(0);
    // int sw1 = response.sw1;
    // int sw2 = response.sw2;

    // boolean success;
    // if ((sw1 == 0x90 && sw2 == 0x00) || sw1 == 0x91) {
    //     Rlog.d(TAG, "USIM data download succeeded: " + response.toString());
    //     success = true;
    // } else if (sw1 == 0x93 && sw2 == 0x00) {
    //     Rlog.e(TAG, "USIM data download failed: Toolkit busy");
    //     acknowledgeSmsWithError(CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_APP_TOOLKIT_BUSY);
    //     return;
    // } else if (sw1 == 0x62 || sw1 == 0x63) {
    //     Rlog.e(TAG, "USIM data download failed: " + response.toString());
    //     success = false;
    // } else {
    //     Rlog.e(TAG, "Unexpected SW1/SW2 response from UICC: " + response.toString());
    //     success = false;
    // }

    // byte[] responseBytes = response.payload;
    // if (responseBytes == null || responseBytes.length == 0) {
    //     if (success) {
    //         mCi.acknowledgeLastIncomingGsmSms(true, 0, null);
    //     } else {
    //         acknowledgeSmsWithError(
    //                 CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR);
    //     }
    //     return;
    // }

    // byte[] smsAckPdu;
    // int index = 0;
    // if (success) {
    //     smsAckPdu = new byte[responseBytes.length + 5];
    //     smsAckPdu[index++] = 0x00;      // TP-MTI, TP-UDHI
    //     smsAckPdu[index++] = 0x07;      // TP-PI: TP-PID, TP-DCS, TP-UDL present
    // } else {
    //     smsAckPdu = new byte[responseBytes.length + 6];
    //     smsAckPdu[index++] = 0x00;      // TP-MTI, TP-UDHI
    //     smsAckPdu[index++] = (byte)
    //             CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR;  // TP-FCS
    //     smsAckPdu[index++] = 0x07;      // TP-PI: TP-PID, TP-DCS, TP-UDL present
    // }

    // smsAckPdu[index++] = (byte) pid;
    // smsAckPdu[index++] = (byte) dcs;

    // if (is7bitDcs(dcs)) {
    //     int septetCount = responseBytes.length * 8 / 7;
    //     smsAckPdu[index++] = (byte) septetCount;
    // } else {
    //     smsAckPdu[index++] = (byte) responseBytes.length;
    // }

    // System.arraycopy(responseBytes, 0, smsAckPdu, index, responseBytes.length);

    // mCi.acknowledgeIncomingGsmSmsWithPdu(success,
    //         IccUtils.bytesToHexString(smsAckPdu), null);
}

void CUsimDataDownloadHandler::AcknowledgeSmsWithError(
    /* [in] */ Int32 cause)
{
    mCi->AcknowledgeLastIncomingGsmSms(FALSE, cause, NULL);
}

Boolean CUsimDataDownloadHandler::Is7bitDcs(
    /* [in] */ Int32 dcs)
{
    // See 3GPP TS 23.038 section 4
    return ((dcs & 0x8C) == 0x00) || ((dcs & 0xF4) == 0xF0);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos