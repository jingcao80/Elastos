#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/telephony/SmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                        SmsMessage::SubmitPdu
//=====================================================================
CAR_INTERFACE_IMPL( SmsMessage::SubmitPdu, Object, ISmsMessageSubmitPdu);
ECode SmsMessage::SubmitPdu::ToString(
    /* [out] */ String* str)
{
    // ==================before translated======================
    // return "SubmitPdu: encodedScAddress = "
    //         + Arrays.toString(encodedScAddress)
    //         + ", encodedMessage = "
    //         + Arrays.toString(encodedMessage);
    assert(0);
    return NOERROR;
}

SmsMessage::SubmitPdu::SubmitPdu(
    /* [in] */ IInterface/*TODO ISmsMessageBaseSubmitPduBase*/* spb)
{
    // ==================before translated======================
    // this.encodedMessage = spb.encodedMessage;
    // this.encodedScAddress = spb.encodedScAddress;
}

//=====================================================================
//                    SmsMessage::NoEmsSupportConfig
//=====================================================================
SmsMessage::NoEmsSupportConfig::NoEmsSupportConfig(
    /* [in] */ ArrayOf<String>* config)
{
    // ==================before translated======================
    // mOperatorNumber = config[0];
    // mIsPrefix = "prefix".equals(config[1]);
    // mGid1 = config.length > 2 ? config[2] : null;
}

ECode SmsMessage::NoEmsSupportConfig::ToString(
    /* [out] */ String* str)
{
    // ==================before translated======================
    // return "NoEmsSupportConfig { mOperatorNumber = " + mOperatorNumber
    //         + ", mIsPrefix = " + mIsPrefix + ", mGid1 = " + mGid1 + " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//                              SmsMessage
//=====================================================================
CAR_INTERFACE_IMPL(SmsMessage, Object, ISmsMessage);

const String SmsMessage::LOGTAG("SmsMessage");
AutoPtr< ArrayOf<SmsMessage::NoEmsSupportConfig*> > SmsMessage::mNoEmsSupportConfigList = NULL;
Boolean SmsMessage::mIsNoEmsSupportConfigListLoaded = FALSE;

ECode SmsMessage::SetSubId(
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // mSubId = subId;
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSubId;
    assert(0);
    return NOERROR;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    // ==================before translated======================
    //  SmsMessage message = null;
    //
    // // cdma(3gpp2) vs gsm(3gpp) format info was not given,
    // // guess from active voice phone type
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    // String format = (PHONE_TYPE_CDMA == activePhone) ?
    //         SmsConstants.FORMAT_3GPP2 : SmsConstants.FORMAT_3GPP;
    // if (com.android.internal.telephony.SyntheticSmsMessage.isSyntheticPdu(pdu)) {
    //     format = FORMAT_SYNTHETIC;
    // }
    // message = createFromPdu(pdu, format);
    //
    // if (null == message || null == message.mWrappedSmsMessage) {
    //     // decoding pdu failed based on activePhone type, must be other format
    //     format = (PHONE_TYPE_CDMA == activePhone) ?
    //             SmsConstants.FORMAT_3GPP : SmsConstants.FORMAT_3GPP2;
    //     message = createFromPdu(pdu, format);
    // }
    // return message;
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format)
{
    // ==================before translated======================
    // SmsMessageBase wrappedMessage;
    //
    // if (SmsConstants.FORMAT_3GPP2.equals(format)) {
    //     wrappedMessage = com.android.internal.telephony.cdma.SmsMessage.createFromPdu(pdu);
    // } else if (SmsConstants.FORMAT_3GPP.equals(format)) {
    //     wrappedMessage = com.android.internal.telephony.gsm.SmsMessage.createFromPdu(pdu);
    // } else if (FORMAT_SYNTHETIC.equals(format)) {
    //     wrappedMessage = com.android.internal.telephony.SyntheticSmsMessage.createFromPdu(pdu);
    // } else {
    //     Rlog.e(LOGTAG, "createFromPdu(): unsupported message format " + format);
    //     return null;
    // }
    //
    // return new SmsMessage(wrappedMessage);
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

AutoPtr<ISmsMessage> SmsMessage::NewFromCMT(
    /* [in] */ ArrayOf<String>* lines)
{
    // ==================before translated======================
    // // received SMS in 3GPP format
    // SmsMessageBase wrappedMessage =
    //         com.android.internal.telephony.gsm.SmsMessage.newFromCMT(lines);
    //
    // return new SmsMessage(wrappedMessage);
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

AutoPtr<ISmsMessage> SmsMessage::NewFromParcel(
    /* [in] */ IParcel* p)
{
    // ==================before translated======================
    // // received SMS in 3GPP2 format
    // SmsMessageBase wrappedMessage =
    //         com.android.internal.telephony.cdma.SmsMessage.newFromParcel(p);
    //
    // return new SmsMessage(wrappedMessage);
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // SmsMessageBase wrappedMessage;
    //
    // if (isCdmaVoice()) {
    //     wrappedMessage = com.android.internal.telephony.cdma.SmsMessage.createFromEfRecord(
    //             index, data);
    // } else {
    //     wrappedMessage = com.android.internal.telephony.gsm.SmsMessage.createFromEfRecord(
    //             index, data);
    // }
    //
    // return wrappedMessage != null ? new SmsMessage(wrappedMessage) : null;
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

AutoPtr<ISmsMessage> SmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // SmsMessageBase wrappedMessage;
    //
    // if (isCdmaVoice(subId)) {
    //     wrappedMessage = com.android.internal.telephony.cdma.SmsMessage.createFromEfRecord(
    //             index, data);
    // } else {
    //     wrappedMessage = com.android.internal.telephony.gsm.SmsMessage.createFromEfRecord(
    //             index, data);
    // }
    //
    // return wrappedMessage != null ? new SmsMessage(wrappedMessage) : null;
    assert(0);
    AutoPtr<SmsMessage> empty;
    return empty;
}

Int32 SmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu)
{
    // ==================before translated======================
    // if (isCdmaVoice()) {
    //     return com.android.internal.telephony.cdma.SmsMessage.getTPLayerLengthForPDU(pdu);
    // } else {
    //     return com.android.internal.telephony.gsm.SmsMessage.getTPLayerLengthForPDU(pdu);
    // }
    assert(0);
    return 0;
}

AutoPtr<ArrayOf<Int32> > SmsMessage::CalculateLength(
    /* [in] */ ICharSequence* msgBody,
    /* [in] */ Boolean use7bitOnly)
{
    // ==================before translated======================
    // // this function is for MO SMS
    // TextEncodingDetails ted = (useCdmaFormatForMoSms()) ?
    //     com.android.internal.telephony.cdma.SmsMessage.calculateLength(msgBody, use7bitOnly) :
    //     com.android.internal.telephony.gsm.SmsMessage.calculateLength(msgBody, use7bitOnly);
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

AutoPtr<IArrayList> SmsMessage::FragmentText(
    /* [in] */ const String& text)
{
    // ==================before translated======================
    // // This function is for MO SMS
    // TextEncodingDetails ted = (useCdmaFormatForMoSms()) ?
    //     com.android.internal.telephony.cdma.SmsMessage.calculateLength(text, false) :
    //     com.android.internal.telephony.gsm.SmsMessage.calculateLength(text, false);
    //
    // // TODO(cleanup): The code here could be rolled into the logic
    // // below cleanly if these MAX_* constants were defined more
    // // flexibly...
    //
    // int limit;
    // if (ted.codeUnitSize == SmsConstants.ENCODING_7BIT) {
    //     int udhLength;
    //     if (ted.languageTable != 0 && ted.languageShiftTable != 0) {
    //         udhLength = GsmAlphabet.UDH_SEPTET_COST_TWO_SHIFT_TABLES;
    //     } else if (ted.languageTable != 0 || ted.languageShiftTable != 0) {
    //         udhLength = GsmAlphabet.UDH_SEPTET_COST_ONE_SHIFT_TABLE;
    //     } else {
    //         udhLength = 0;
    //     }
    //
    //     if (ted.msgCount > 1) {
    //         udhLength += GsmAlphabet.UDH_SEPTET_COST_CONCATENATED_MESSAGE;
    //     }
    //
    //     if (udhLength != 0) {
    //         udhLength += GsmAlphabet.UDH_SEPTET_COST_LENGTH;
    //     }
    //
    //     limit = SmsConstants.MAX_USER_DATA_SEPTETS - udhLength;
    // } else {
    //     if (ted.msgCount > 1) {
    //         limit = SmsConstants.MAX_USER_DATA_BYTES_WITH_HEADER;
    //         // If EMS is not supported, break down EMS into single segment SMS
    //         // and add page info " x/y".
    //         // In the case of UCS2 encoding, we need 8 bytes for this,
    //         // but we only have 6 bytes from UDH, so truncate the limit for
    //         // each segment by 2 bytes (1 char).
    //         // Make sure total number of segments is less than 10.
    //         if (!hasEmsSupport() && ted.msgCount < 10) {
    //             limit -= 2;
    //         }
    //     } else {
    //         limit = SmsConstants.MAX_USER_DATA_BYTES;
    //     }
    // }
    //
    // String newMsgBody = null;
    // Resources r = Resources.getSystem();
    // if (r.getBoolean(com.android.internal.R.bool.config_sms_force_7bit_encoding)) {
    //     newMsgBody  = Sms7BitEncodingTranslator.translate(text);
    // }
    // if (TextUtils.isEmpty(newMsgBody)) {
    //     newMsgBody = text;
    // }
    // int pos = 0;  // Index in code units.
    // int textLen = newMsgBody.length();
    // ArrayList<String> result = new ArrayList<String>(ted.msgCount);
    // while (pos < textLen) {
    //     int nextPos = 0;  // Counts code units.
    //     if (ted.codeUnitSize == SmsConstants.ENCODING_7BIT) {
    //         if (useCdmaFormatForMoSms() && ted.msgCount == 1) {
    //             // For a singleton CDMA message, the encoding must be ASCII...
    //             nextPos = pos + Math.min(limit, textLen - pos);
    //         } else {
    //             // For multi-segment messages, CDMA 7bit equals GSM 7bit encoding (EMS mode).
    //             nextPos = GsmAlphabet.findGsmSeptetLimitIndex(newMsgBody, pos, limit,
    //                     ted.languageTable, ted.languageShiftTable);
    //         }
    //     } else {  // Assume unicode.
    //         nextPos = pos + Math.min(limit / 2, textLen - pos);
    //     }
    //     if ((nextPos <= pos) || (nextPos > textLen)) {
    //         Rlog.e(LOGTAG, "fragmentText failed (" + pos + " >= " + nextPos + " or " +
    //                   nextPos + " >= " + textLen + ")");
    //         break;
    //     }
    //     result.add(newMsgBody.substring(pos, nextPos));
    //     pos = nextPos;
    // }
    // return result;
    assert(0);
    AutoPtr<IArrayList> empty;
    return empty;
}

AutoPtr<ArrayOf<Int32> > SmsMessage::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly)
{
    // ==================before translated======================
    // return calculateLength((CharSequence)messageBody, use7bitOnly);
    assert(0);
    AutoPtr< ArrayOf<Int32> > empty;
    return empty;
}

AutoPtr<ISmsMessageSubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested)
{
    // ==================before translated======================
    // SubmitPduBase spb;
    //
    // if (useCdmaFormatForMoSms()) {
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

AutoPtr<ISmsMessageSubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // SubmitPduBase spb;
    // if (useCdmaFormatForMoSms(subId)) {
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

AutoPtr<ISmsMessageSubmitPdu> SmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested)
{
    // ==================before translated======================
    // SubmitPduBase spb;
    //
    // if (useCdmaFormatForMoSms()) {
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
    // ==================before translated======================
    // return mWrappedSmsMessage.getServiceCenterAddress();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage != null ? mWrappedSmsMessage.getOriginatingAddress() : null;
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetDisplayOriginatingAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getDisplayOriginatingAddress();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getMessageBody();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetMessageClass(
    /* [out] */ Int32/*TODO MessageClass*/* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // switch(mWrappedSmsMessage.getMessageClass()) {
    //     case CLASS_0: return MessageClass.CLASS_0;
    //     case CLASS_1: return MessageClass.CLASS_1;
    //     case CLASS_2: return MessageClass.CLASS_2;
    //     case CLASS_3: return MessageClass.CLASS_3;
    //     default: return MessageClass.UNKNOWN;
    //
    // }
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetDisplayMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getDisplayMessageBody();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetPseudoSubject(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getPseudoSubject();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetTimestampMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getTimestampMillis();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsEmail(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isEmail();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetEmailBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getEmailBody();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetEmailFrom(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getEmailFrom();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getProtocolIdentifier();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isReplace();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isCphsMwiMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isMWIClearMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isMWISetMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isMwiDontStore();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getUserData();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetPdu(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getPdu();
    assert(0);
    return NOERROR;
}

Int32 SmsMessage::GetStatusOnSim()
{
    // ==================before translated======================
    // return mWrappedSmsMessage.getStatusOnIcc();
    assert(0);
    return 0;
}

ECode SmsMessage::GetStatusOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getStatusOnIcc();
    assert(0);
    return NOERROR;
}

Int32 SmsMessage::GetIndexOnSim()
{
    // ==================before translated======================
    // return mWrappedSmsMessage.getIndexOnIcc();
    assert(0);
    return 0;
}

ECode SmsMessage::GetIndexOnIcc(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getIndexOnIcc();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getStatus();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isStatusReportMessage();
    assert(0);
    return NOERROR;
}

ECode SmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.isReplyPathPresent();
    assert(0);
    return NOERROR;
}

Boolean SmsMessage::HasEmsSupport()
{
    // ==================before translated======================
    // if (!isNoEmsSupportConfigListExisted()) {
    //     return true;
    // }
    //
    // String simOperator = TelephonyManager.getDefault().getSimOperator();
    // String gid = TelephonyManager.getDefault().getGroupIdLevel1();
    //
    // for (NoEmsSupportConfig currentConfig : mNoEmsSupportConfigList) {
    //     if (simOperator.startsWith(currentConfig.mOperatorNumber) &&
    //         (TextUtils.isEmpty(currentConfig.mGid1) ||
    //         (!TextUtils.isEmpty(currentConfig.mGid1)
    //         && currentConfig.mGid1.equalsIgnoreCase(gid)))) {
    //         return false;
    //     }
    //  }
    // return true;
    assert(0);
    return FALSE;
}

Boolean SmsMessage::ShouldAppendPageNumberAsPrefix()
{
    // ==================before translated======================
    // if (!isNoEmsSupportConfigListExisted()) {
    //     return false;
    // }
    //
    // String simOperator = TelephonyManager.getDefault().getSimOperator();
    // String gid = TelephonyManager.getDefault().getGroupIdLevel1();
    // for (NoEmsSupportConfig currentConfig : mNoEmsSupportConfigList) {
    //     if (simOperator.startsWith(currentConfig.mOperatorNumber) &&
    //         (TextUtils.isEmpty(currentConfig.mGid1) ||
    //         (!TextUtils.isEmpty(currentConfig.mGid1)
    //         && currentConfig.mGid1.equalsIgnoreCase(gid)))) {
    //         return currentConfig.mIsPrefix;
    //     }
    // }
    // return false;
    assert(0);
    return FALSE;
}

ECode SmsMessage::GetRecipientAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mWrappedSmsMessage.getRecipientAddress();
    assert(0);
    return NOERROR;
}

SmsMessage::SmsMessage(
    /* [in] */ ISmsMessageBase* smb)
{
    // ==================before translated======================
    // mWrappedSmsMessage = smb;
}

Boolean SmsMessage::UseCdmaFormatForMoSms()
{
    // ==================before translated======================
    // SmsManager smsManager = SmsManager.getSmsManagerForSubscriber(
    //         SubscriptionManager.getDefaultSmsSubId());
    // if (!smsManager.isImsSmsSupported()) {
    //     // use Voice technology to determine SMS format.
    //     return isCdmaVoice();
    // }
    // // IMS is registered with SMS support, check the SMS format supported
    // return (SmsConstants.FORMAT_3GPP2.equals(smsManager.getImsSmsFormat()));
    assert(0);
    return FALSE;
}

Boolean SmsMessage::UseCdmaFormatForMoSms(
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // SmsManager smsManager = SmsManager.getSmsManagerForSubscriber(subId);
    // if (!smsManager.isImsSmsSupported()) {
    //     // use Voice technology to determine SMS format.
    //     return isCdmaVoice(subId);
    // }
    // // IMS is registered with SMS support, check the SMS format supported
    // return (SmsConstants.FORMAT_3GPP2.equals(smsManager.getImsSmsFormat()));
    assert(0);
    return FALSE;
}

Boolean SmsMessage::IsCdmaVoice()
{
    // ==================before translated======================
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType();
    // return (PHONE_TYPE_CDMA == activePhone);
    assert(0);
    return FALSE;
}

Boolean SmsMessage::IsCdmaVoice(
    /* [in] */ Int64 subId)
{
    // ==================before translated======================
    // int activePhone = TelephonyManager.getDefault().getCurrentPhoneType(subId);
    // return (PHONE_TYPE_CDMA == activePhone);
    assert(0);
    return FALSE;
}

Boolean SmsMessage::IsNoEmsSupportConfigListExisted()
{
    // ==================before translated======================
    // if (!mIsNoEmsSupportConfigListLoaded) {
    //     Resources r = Resources.getSystem();
    //     if (r != null) {
    //         String[] listArray = r.getStringArray(
    //                 com.android.internal.R.array.no_ems_support_sim_operators);
    //         if ((listArray != null) && (listArray.length > 0)) {
    //             mNoEmsSupportConfigList = new NoEmsSupportConfig[listArray.length];
    //             for (int i=0; i<listArray.length; i++) {
    //                 mNoEmsSupportConfigList[i] = new NoEmsSupportConfig(listArray[i].split(";"));
    //             }
    //         }
    //         mIsNoEmsSupportConfigListLoaded = true;
    //     }
    // }
    //
    // if (mNoEmsSupportConfigList != null && mNoEmsSupportConfigList.length != 0) {
    //     return true;
    // }
    //
    // return false;
    assert(0);
    return FALSE;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
