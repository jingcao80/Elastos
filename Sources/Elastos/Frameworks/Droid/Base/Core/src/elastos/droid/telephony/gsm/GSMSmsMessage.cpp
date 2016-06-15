#include "Elastos.Droid.Internal.h"
#include "elastos/droid/telephony/gsm/GSMSmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

//=====================================================================
//                        GSMSmsMessage::SubmitPdu
//=====================================================================
CAR_INTERFACE_IMPL(GSMSmsMessage::SubmitPdu, Object, ISubmitPdu);

GSMSmsMessage::SubmitPdu::SubmitPdu()
{
}

ECode GSMSmsMessage::SubmitPdu::constructor()
{
    return NOERROR;
}

ECode GSMSmsMessage::SubmitPdu::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;
    // ==================before translated======================
    // return "SubmitPdu: encodedScAddress = "
    //         + Arrays.toString(encodedScAddress)
    //         + ", encodedMessage = "
    //         + Arrays.toString(encodedMessage);
    assert(0);
    return NOERROR;
}

//GSMSmsMessage::SubmitPdu::SubmitPdu(
//    /* [in] */ ISmsMessageBaseSubmitPduBase* spb)
//{
//    // ==================before translated======================
//    // this.encodedMessage = spb.encodedMessage;
//    // this.encodedScAddress = spb.encodedScAddress;
//}

//=====================================================================
//                              GSMSmsMessage
//=====================================================================
CAR_INTERFACE_IMPL(GSMSmsMessage, Object, ISmsMessage);

GSMSmsMessage::GSMSmsMessage()
{
    // ==================before translated======================
    // this(getSmsFacility());
}

ECode GSMSmsMessage::constructor()
{
    return NOERROR;
}

AutoPtr<GSMSmsMessage> GSMSmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // ==================before translated======================
    // SmsMessageBase wrappedMessage;
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    //
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     wrappedMessage = com.android.internal.telephony.cdma.SmsMessage.createFromPdu(pdu);
    // } else {
    //     wrappedMessage = com.android.internal.telephony.gsm.GSMSmsMessage.createFromPdu(pdu);
    // }
    //
    // // Hangouts, and some other bad citizens, are using this deprecated API call.
    // // Make sure that WhisperPush and Voice+ integration properly work,
    // // as this may be a synthetic message.
    // if (wrappedMessage == null) {
    //     // returns null if it is not synthetic. (likely malformed)
    //     wrappedMessage = com.android.internal.telephony.SyntheticSmsMessage.createFromPdu(pdu);
    // }
    //
    // return new GSMSmsMessage(wrappedMessage);
    assert(0);
    AutoPtr<GSMSmsMessage> empty;
    return empty;
}

Int32 GSMSmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu)
{
    // ==================before translated======================
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    //
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     return com.android.internal.telephony.cdma.SmsMessage.getTPLayerLengthForPDU(pdu);
    // } else {
    //     return com.android.internal.telephony.gsm.GSMSmsMessage.getTPLayerLengthForPDU(pdu);
    // }
    assert(0);
    return 0;
}

AutoPtr< ArrayOf<Int32> > GSMSmsMessage::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    // ==================before translated======================
    // GsmAlphabet.TextEncodingDetails ted =
    //         com.android.internal.telephony.gsm.GSMSmsMessage
    //                 .calculateLength(messageBody, use7bitOnly);
    // int ret[] = new int[4];
    // ret[0] = ted.msgCount;
    // ret[1] = ted.codeUnitCount;
    // ret[2] = ted.codeUnitsRemaining;
    // ret[3] = ted.codeUnitSize;
    // return ret;
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr< ArrayOf<Int32> > GSMSmsMessage::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    // ==================before translated======================
    // return calculateLength((CharSequence)messageBody, use7bitOnly);
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr<GSMSmsMessage::SubmitPdu> GSMSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header)
{
    // ==================before translated======================
    // SubmitPduBase spb;
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    //
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     spb = com.android.internal.telephony.cdma.SmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, message, statusReportRequested,
    //             SmsHeader.fromByteArray(header));
    // } else {
    //     spb = com.android.internal.telephony.gsm.GSMSmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, message, statusReportRequested, header);
    // }
    //
    // return new SubmitPdu(spb);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<GSMSmsMessage::SubmitPdu> GSMSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested)
{
    // ==================before translated======================
    // SubmitPduBase spb;
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    //
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     spb = com.android.internal.telephony.cdma.SmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, message, statusReportRequested, null);
    // } else {
    //     spb = com.android.internal.telephony.gsm.GSMSmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, message, statusReportRequested);
    // }
    //
    // return new SubmitPdu(spb);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<GSMSmsMessage::SubmitPdu> GSMSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    // ==================before translated======================
    // SubmitPduBase spb;
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    //
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     spb = com.android.internal.telephony.cdma.SmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, destinationPort, data, statusReportRequested);
    // } else {
    //     spb = com.android.internal.telephony.gsm.GSMSmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, destinationPort, data, statusReportRequested);
    // }
    //
    // return new SubmitPdu(spb);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

ECode GSMSmsMessage::GetServiceCenterAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getServiceCenterAddress();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getOriginatingAddress();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetDisplayOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getDisplayOriginatingAddress();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getMessageBody();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetMessageClass(
    /* [out] */ Int32/*TODO MessageClass*/* result)
{
    VALIDATE_NOT_NULL(result);
    *result  = 0;//TODO MessageClass::MessageClass_UNKNOWN;
    // ==================before translated======================
    // int index = mWrappedSmsMessage.getMessageClass().ordinal();
    //
    // return MessageClass.values()[index];
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetDisplayMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getDisplayMessageBody();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetPseudoSubject(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getPseudoSubject();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetTimestampMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getTimestampMillis();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsEmail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isEmail();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetEmailBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getEmailBody();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetEmailFrom(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getEmailFrom();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getProtocolIdentifier();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isReplace();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isCphsMwiMessage();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isMWIClearMessage();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isMWISetMessage();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isMwiDontStore();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getUserData();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetPdu(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getPdu();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetStatusOnSim(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getStatusOnIcc();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetStatusOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    //
    // return mWrappedSmsMessage.getStatusOnIcc();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetIndexOnSim(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getIndexOnIcc();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetIndexOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    //
    // return mWrappedSmsMessage.getIndexOnIcc();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getStatus();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isStatusReportMessage();
    assert(0);
    return NOERROR;
}

ECode GSMSmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isReplyPathPresent();
    assert(0);
    return NOERROR;
}

GSMSmsMessage::GSMSmsMessage(
    /* [in] */ ISmsMessageBase* smb)
{
    // ==================before translated======================
    // mWrappedSmsMessage = smb;
}

AutoPtr<ISmsMessageBase> GSMSmsMessage::GetSmsFacility()
{
    // ==================before translated======================
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     return new com.android.internal.telephony.cdma.SmsMessage();
    // } else {
    //     return new com.android.internal.telephony.gsm.GSMSmsMessage();
    // }
    assert(0);
    AutoPtr<ISmsMessageBase> empty;
    return empty;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos


