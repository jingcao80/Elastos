#include "Elastos.Droid.Internal.h"
#include "elastos/droid/telephony/gsm/SmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

//=====================================================================
//                        SmsMessage::SubmitPdu
//=====================================================================
CAR_INTERFACE_IMPL(SmsMessage::SubmitPdu, Object, ISubmitPdu);

SmsMessage::SubmitPdu::SubmitPdu()
{
}

ECode SmsMessage::SubmitPdu::ToString(
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

//SmsMessage::SubmitPdu::SubmitPdu(
//    /* [in] */ ISmsMessageBaseSubmitPduBase* spb)
//{
//    // ==================before translated======================
//    // this.encodedMessage = spb.encodedMessage;
//    // this.encodedScAddress = spb.encodedScAddress;
//}

//=====================================================================
//                              SmsMessage
//=====================================================================
CAR_INTERFACE_IMPL(SmsMessage, Object, ISmsMessage);

SmsMessage::SmsMessage()
{
    // ==================before translated======================
    // this(getSmsFacility());
}

AutoPtr<SmsMessage> SmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // ==================before translated======================
    // SmsMessageBase wrappedMessage;
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    //
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     wrappedMessage = com.android.internal.telephony.cdma.SmsMessage.createFromPdu(pdu);
    // } else {
    //     wrappedMessage = com.android.internal.telephony.gsm.SmsMessage.createFromPdu(pdu);
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
    // return new SmsMessage(wrappedMessage);
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

Int32 SmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu)
{
    // ==================before translated======================
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    //
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     return com.android.internal.telephony.cdma.SmsMessage.getTPLayerLengthForPDU(pdu);
    // } else {
    //     return com.android.internal.telephony.gsm.SmsMessage.getTPLayerLengthForPDU(pdu);
    // }
    assert(0);
    return 0;
}

AutoPtr< ArrayOf<Int32> > SmsMessage::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    // ==================before translated======================
    // GsmAlphabet.TextEncodingDetails ted =
    //         com.android.internal.telephony.gsm.SmsMessage
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

AutoPtr< ArrayOf<Int32> > SmsMessage::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    // ==================before translated======================
    // return calculateLength((CharSequence)messageBody, use7bitOnly);
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
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
    //     spb = com.android.internal.telephony.gsm.SmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, message, statusReportRequested, header);
    // }
    //
    // return new SubmitPdu(spb);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
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
    //     spb = com.android.internal.telephony.gsm.SmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, message, statusReportRequested);
    // }
    //
    // return new SubmitPdu(spb);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

AutoPtr<SmsMessage::SubmitPdu> SmsMessage::GetSubmitPdu(
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
    //     spb = com.android.internal.telephony.gsm.SmsMessage.getSubmitPdu(scAddress,
    //             destinationAddress, destinationPort, data, statusReportRequested);
    // }
    //
    // return new SubmitPdu(spb);
    assert(0);
    AutoPtr<SubmitPdu> empty;
    return empty;
}

ECode SmsMessage::GetServiceCenterAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getServiceCenterAddress();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getOriginatingAddress();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetDisplayOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getDisplayOriginatingAddress();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getMessageBody();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetMessageClass(
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

ECode SmsMessage::GetDisplayMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getDisplayMessageBody();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetPseudoSubject(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getPseudoSubject();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetTimestampMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getTimestampMillis();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsEmail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isEmail();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetEmailBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getEmailBody();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetEmailFrom(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getEmailFrom();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getProtocolIdentifier();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isReplace();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isCphsMwiMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isMWIClearMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isMWISetMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isMwiDontStore();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getUserData();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetPdu(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mWrappedSmsMessage.getPdu();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetStatusOnSim(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getStatusOnIcc();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetStatusOnIcc(
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

ECode SmsMessage::GetIndexOnSim(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getIndexOnIcc();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetIndexOnIcc(
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

ECode SmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    // ==================before translated======================
    // return mWrappedSmsMessage.getStatus();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isStatusReportMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // return mWrappedSmsMessage.isReplyPathPresent();
    assert(0);
    return NOERROR;
}

SmsMessage::SmsMessage(
    /* [in] */ ISmsMessageBase* smb)
{
    // ==================before translated======================
    // mWrappedSmsMessage = smb;
}

AutoPtr<ISmsMessageBase> SmsMessage::GetSmsFacility()
{
    // ==================before translated======================
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    // if (PHONE_TYPE_CDMA == activePhone) {
    //     return new com.android.internal.telephony.cdma.SmsMessage();
    // } else {
    //     return new com.android.internal.telephony.gsm.SmsMessage();
    // }
    assert(0);
    AutoPtr<ISmsMessageBase> empty;
    return empty;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos


