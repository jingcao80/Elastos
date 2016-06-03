
#include "elastos/droid/internal/telephony/cdma/sms/BearerData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

//=====================================================================
//                        BearerData::TimeStamp
//=====================================================================
BearerData::TimeStamp::TimeStamp()
{
    // ==================before translated======================
    // super(TimeZone.getDefault().getID());   // 3GPP2 timestamps use the local timezone
}

AutoPtr<BearerData::TimeStamp> BearerData::TimeStamp::FromByteArray(
    /* [in] */ ArrayOf<Byte>* data)
{
    // ==================before translated======================
    // TimeStamp ts = new TimeStamp();
    // // C.S0015-B v2.0, 4.5.4: range is 1996-2095
    // int year = IccUtils.cdmaBcdByteToInt(data[0]);
    // if (year > 99 || year < 0) return null;
    // ts.year = year >= 96 ? year + 1900 : year + 2000;
    // int month = IccUtils.cdmaBcdByteToInt(data[1]);
    // if (month < 1 || month > 12) return null;
    // ts.month = month - 1;
    // int day = IccUtils.cdmaBcdByteToInt(data[2]);
    // if (day < 1 || day > 31) return null;
    // ts.monthDay = day;
    // int hour = IccUtils.cdmaBcdByteToInt(data[3]);
    // if (hour < 0 || hour > 23) return null;
    // ts.hour = hour;
    // int minute = IccUtils.cdmaBcdByteToInt(data[4]);
    // if (minute < 0 || minute > 59) return null;
    // ts.minute = minute;
    // int second = IccUtils.cdmaBcdByteToInt(data[5]);
    // if (second < 0 || second > 59) return null;
    // ts.second = second;
    // return ts;
    assert(0);
    AutoPtr<TimeStamp> empty;
    return empty;
}

ECode BearerData::TimeStamp::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;
    // ==================before translated======================
    // StringBuilder builder = new StringBuilder();
    // builder.append("TimeStamp ");
    // builder.append("{ year=" + year);
    // builder.append(", month=" + month);
    // builder.append(", day=" + monthDay);
    // builder.append(", hour=" + hour);
    // builder.append(", minute=" + minute);
    // builder.append(", second=" + second);
    // builder.append(" }");
    // return builder.toString();
    assert(0);
    return NOERROR;
}

////=====================================================================
////                     BearerData::CodingException
////=====================================================================
//BearerData::CodingException::CodingException(
//    /* [in] */ const String& s)
//{
//    // ==================before translated======================
//    // super(s);
//}

//=====================================================================
//                              BearerData
//=====================================================================
CAR_INTERFACE_IMPL(BearerData, Object, IBearerData);

const String BearerData::LOGTAG("BearerData");
const Byte BearerData::SUBPARAM_MESSAGE_IDENTIFIER = 0x00;
const Byte BearerData::SUBPARAM_USER_DATA = 0x01;
const Byte BearerData::SUBPARAM_USER_RESPONSE_CODE = 0x02;
const Byte BearerData::SUBPARAM_MESSAGE_CENTER_TIME_STAMP = 0x03;
const Byte BearerData::SUBPARAM_VALIDITY_PERIOD_ABSOLUTE = 0x04;
const Byte BearerData::SUBPARAM_VALIDITY_PERIOD_RELATIVE = 0x05;
const Byte BearerData::SUBPARAM_DEFERRED_DELIVERY_TIME_ABSOLUTE = 0x06;
const Byte BearerData::SUBPARAM_DEFERRED_DELIVERY_TIME_RELATIVE = 0x07;
const Byte BearerData::SUBPARAM_PRIORITY_INDICATOR = 0x08;
const Byte BearerData::SUBPARAM_PRIVACY_INDICATOR = 0x09;
const Byte BearerData::SUBPARAM_REPLY_OPTION = 0x0A;
const Byte BearerData::SUBPARAM_NUMBER_OF_MESSAGES = 0x0B;
const Byte BearerData::SUBPARAM_ALERT_ON_MESSAGE_DELIVERY = 0x0C;
const Byte BearerData::SUBPARAM_LANGUAGE_INDICATOR = 0x0D;
const Byte BearerData::SUBPARAM_CALLBACK_NUMBER = 0x0E;
const Byte BearerData::SUBPARAM_MESSAGE_DISPLAY_MODE = 0x0F;
const Byte BearerData::SUBPARAM_MESSAGE_DEPOSIT_INDEX = 0x11;
const Byte BearerData::SUBPARAM_SERVICE_CATEGORY_PROGRAM_DATA = 0x12;
const Byte BearerData::SUBPARAM_SERVICE_CATEGORY_PROGRAM_RESULTS = 0x13;
const Byte BearerData::SUBPARAM_MESSAGE_STATUS = 0x14;
const Byte BearerData::SUBPARAM_ID_LAST_DEFINED = 0x17;

ECode BearerData::GetLanguage(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return getLanguageCodeForValue(language);
    assert(0);
    return NOERROR;
}

ECode BearerData::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;
    // ==================before translated======================
    // StringBuilder builder = new StringBuilder();
    // builder.append("BearerData ");
    // builder.append("{ messageType=" + messageType);
    // builder.append(", messageId=" + messageId);
    // builder.append(", priority=" + (priorityIndicatorSet ? priority : "unset"));
    // builder.append(", privacy=" + (privacyIndicatorSet ? privacy : "unset"));
    // builder.append(", alert=" + (alertIndicatorSet ? alert : "unset"));
    // builder.append(", displayMode=" + (displayModeSet ? displayMode : "unset"));
    // builder.append(", language=" + (languageIndicatorSet ? language : "unset"));
    // builder.append(", errorClass=" + (messageStatusSet ? errorClass : "unset"));
    // builder.append(", msgStatus=" + (messageStatusSet ? messageStatus : "unset"));
    // builder.append(", msgCenterTimeStamp=" +
    //         ((msgCenterTimeStamp != null) ? msgCenterTimeStamp : "unset"));
    // builder.append(", validityPeriodAbsolute=" +
    //         ((validityPeriodAbsolute != null) ? validityPeriodAbsolute : "unset"));
    // builder.append(", validityPeriodRelative=" +
    //         ((validityPeriodRelativeSet) ? validityPeriodRelative : "unset"));
    // builder.append(", deferredDeliveryTimeAbsolute=" +
    //         ((deferredDeliveryTimeAbsolute != null) ? deferredDeliveryTimeAbsolute : "unset"));
    // builder.append(", deferredDeliveryTimeRelative=" +
    //         ((deferredDeliveryTimeRelativeSet) ? deferredDeliveryTimeRelative : "unset"));
    // builder.append(", userAckReq=" + userAckReq);
    // builder.append(", deliveryAckReq=" + deliveryAckReq);
    // builder.append(", readAckReq=" + readAckReq);
    // builder.append(", reportReq=" + reportReq);
    // builder.append(", numberOfMessages=" + numberOfMessages);
    // builder.append(", callbackNumber=" + callbackNumber);
    // builder.append(", depositIndex=" + depositIndex);
    // builder.append(", hasUserDataHeader=" + hasUserDataHeader);
    // builder.append(", userData=" + userData);
    // builder.append(" }");
    // return builder.toString();
    assert(0);
    return NOERROR;
}

AutoPtr<IGsmAlphabetTextEncodingDetails> BearerData::CalcTextEncodingDetails(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean force7BitEncoding)
{
    // ==================before translated======================
    // TextEncodingDetails ted;
    // int septets = countAsciiSeptets(msg, force7BitEncoding);
    // if (septets != -1 && septets <= SmsConstants.MAX_USER_DATA_SEPTETS) {
    //     ted = new TextEncodingDetails();
    //     ted.msgCount = 1;
    //     ted.codeUnitCount = septets;
    //     ted.codeUnitsRemaining = SmsConstants.MAX_USER_DATA_SEPTETS - septets;
    //     ted.codeUnitSize = SmsConstants.ENCODING_7BIT;
    // } else {
    //     ted = com.android.internal.telephony.gsm.SmsMessage.calculateLength(
    //             msg, force7BitEncoding);
    //     if (ted.msgCount == 1 && ted.codeUnitSize == SmsConstants.ENCODING_7BIT) {
    //         // We don't support single-segment EMS, so calculate for 16-bit
    //         // TODO: Consider supporting single-segment EMS
    //         ted.codeUnitCount = msg.length();
    //         int octets = ted.codeUnitCount * 2;
    //         if (octets > SmsConstants.MAX_USER_DATA_BYTES) {
    //             // If EMS is not supported, break down EMS into single segment SMS
    //             // and add page info " x/y".
    //             // In the case of UCS2 encoding type, we need 8 bytes for this
    //             // but we only have 6 bytes from UDH, so truncate the limit for
    //             // each segment by 2 bytes (1 char).
    //             int max_user_data_bytes_with_header =
    //                     SmsConstants.MAX_USER_DATA_BYTES_WITH_HEADER;
    //             if (!android.telephony.SmsMessage.hasEmsSupport()) {
    //                 // make sure total number of segments is less than 10
    //                 if (octets <= 9 * (max_user_data_bytes_with_header - 2))
    //                     max_user_data_bytes_with_header -= 2;
    //             }
    //
    //             ted.msgCount = (octets + (max_user_data_bytes_with_header - 1)) /
    //                     max_user_data_bytes_with_header;
    //             ted.codeUnitsRemaining = ((ted.msgCount *
    //                     max_user_data_bytes_with_header) - octets) / 2;
    //         } else {
    //             ted.msgCount = 1;
    //             ted.codeUnitsRemaining = (SmsConstants.MAX_USER_DATA_BYTES - octets)/2;
    //         }
    //         ted.codeUnitSize = SmsConstants.ENCODING_16BIT;
    //     }
    // }
    // return ted;
    assert(0);
    AutoPtr<IGsmAlphabetTextEncodingDetails> empty;
    return empty;
}

AutoPtr<ArrayOf<Byte> > BearerData::Encode(
    /* [in] */ IBearerData* bData)
{
    // ==================before translated======================
    // bData.hasUserDataHeader = ((bData.userData != null) &&
    //         (bData.userData.userDataHeader != null));
    // try {
    //     BitwiseOutputStream outStream = new BitwiseOutputStream(200);
    //     outStream.write(8, SUBPARAM_MESSAGE_IDENTIFIER);
    //     encodeMessageId(bData, outStream);
    //     if (bData.userData != null) {
    //         outStream.write(8, SUBPARAM_USER_DATA);
    //         encodeUserData(bData, outStream);
    //     }
    //     if (bData.callbackNumber != null) {
    //         outStream.write(8, SUBPARAM_CALLBACK_NUMBER);
    //         encodeCallbackNumber(bData, outStream);
    //     }
    //     if (bData.userAckReq || bData.deliveryAckReq || bData.readAckReq || bData.reportReq) {
    //         outStream.write(8, SUBPARAM_REPLY_OPTION);
    //         encodeReplyOption(bData, outStream);
    //     }
    //     if (bData.numberOfMessages != 0) {
    //         outStream.write(8, SUBPARAM_NUMBER_OF_MESSAGES);
    //         encodeMsgCount(bData, outStream);
    //     }
    //     if (bData.validityPeriodRelativeSet) {
    //         outStream.write(8, SUBPARAM_VALIDITY_PERIOD_RELATIVE);
    //         encodeValidityPeriodRel(bData, outStream);
    //     }
    //     if (bData.privacyIndicatorSet) {
    //         outStream.write(8, SUBPARAM_PRIVACY_INDICATOR);
    //         encodePrivacyIndicator(bData, outStream);
    //     }
    //     if (bData.languageIndicatorSet) {
    //         outStream.write(8, SUBPARAM_LANGUAGE_INDICATOR);
    //         encodeLanguageIndicator(bData, outStream);
    //     }
    //     if (bData.displayModeSet) {
    //         outStream.write(8, SUBPARAM_MESSAGE_DISPLAY_MODE);
    //         encodeDisplayMode(bData, outStream);
    //     }
    //     if (bData.priorityIndicatorSet) {
    //         outStream.write(8, SUBPARAM_PRIORITY_INDICATOR);
    //         encodePriorityIndicator(bData, outStream);
    //     }
    //     if (bData.alertIndicatorSet) {
    //         outStream.write(8, SUBPARAM_ALERT_ON_MESSAGE_DELIVERY);
    //         encodeMsgDeliveryAlert(bData, outStream);
    //     }
    //     if (bData.messageStatusSet) {
    //         outStream.write(8, SUBPARAM_MESSAGE_STATUS);
    //         encodeMsgStatus(bData, outStream);
    //     }
    //     if (bData.serviceCategoryProgramResults != null) {
    //         outStream.write(8, SUBPARAM_SERVICE_CATEGORY_PROGRAM_RESULTS);
    //         encodeScpResults(bData, outStream);
    //     }
    //     return outStream.toByteArray();
    // } catch (BitwiseOutputStream.AccessException ex) {
    //     Rlog.e(LOGTAG, "BearerData encode failed: " + ex);
    // } catch (CodingException ex) {
    //     Rlog.e(LOGTAG, "BearerData encode failed: " + ex);
    // }
    // return null;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr<BearerData> BearerData::Decode(
    /* [in] */ ArrayOf<Byte>* smsData)
{
    // ==================before translated======================
    // return decode(smsData, 0);
    assert(0);
    AutoPtr<BearerData> empty;
    return empty;
}

AutoPtr<BearerData> BearerData::Decode(
    /* [in] */ ArrayOf<Byte>* smsData,
    /* [in] */ Int32 serviceCategory)
{
    // ==================before translated======================
    // try {
    //     BitwiseInputStream inStream = new BitwiseInputStream(smsData);
    //     BearerData bData = new BearerData();
    //     int foundSubparamMask = 0;
    //     while (inStream.available() > 0) {
    //         int subparamId = inStream.read(8);
    //         int subparamIdBit = 1 << subparamId;
    //         // int is 4 bytes. This duplicate check has a limit to Id number up to 32 (4*8)
    //         // as 32th bit is the max bit in int.
    //         // Per 3GPP2 C.S0015-B Table 4.5-1 Bearer Data Subparameter Identifiers:
    //         // last defined subparam ID is 23 (00010111 = 0x17 = 23).
    //         // Only do duplicate subparam ID check if subparam is within defined value as
    //         // reserved subparams are just skipped.
    //         if ((foundSubparamMask & subparamIdBit) != 0 &&
    //                 (subparamId >= SUBPARAM_MESSAGE_IDENTIFIER &&
    //                 subparamId <= SUBPARAM_ID_LAST_DEFINED)) {
    //             throw new CodingException("illegal duplicate subparameter (" +
    //                                       subparamId + ")");
    //         }
    //         boolean decodeSuccess;
    //         switch (subparamId) {
    //         case SUBPARAM_MESSAGE_IDENTIFIER:
    //             decodeSuccess = decodeMessageId(bData, inStream);
    //             break;
    //         case SUBPARAM_USER_DATA:
    //             decodeSuccess = decodeUserData(bData, inStream);
    //             break;
    //         case SUBPARAM_USER_RESPONSE_CODE:
    //             decodeSuccess = decodeUserResponseCode(bData, inStream);
    //             break;
    //         case SUBPARAM_REPLY_OPTION:
    //             decodeSuccess = decodeReplyOption(bData, inStream);
    //             break;
    //         case SUBPARAM_NUMBER_OF_MESSAGES:
    //             decodeSuccess = decodeMsgCount(bData, inStream);
    //             break;
    //         case SUBPARAM_CALLBACK_NUMBER:
    //             decodeSuccess = decodeCallbackNumber(bData, inStream);
    //             break;
    //         case SUBPARAM_MESSAGE_STATUS:
    //             decodeSuccess = decodeMsgStatus(bData, inStream);
    //             break;
    //         case SUBPARAM_MESSAGE_CENTER_TIME_STAMP:
    //             decodeSuccess = decodeMsgCenterTimeStamp(bData, inStream);
    //             break;
    //         case SUBPARAM_VALIDITY_PERIOD_ABSOLUTE:
    //             decodeSuccess = decodeValidityAbs(bData, inStream);
    //             break;
    //         case SUBPARAM_VALIDITY_PERIOD_RELATIVE:
    //             decodeSuccess = decodeValidityRel(bData, inStream);
    //             break;
    //         case SUBPARAM_DEFERRED_DELIVERY_TIME_ABSOLUTE:
    //             decodeSuccess = decodeDeferredDeliveryAbs(bData, inStream);
    //             break;
    //         case SUBPARAM_DEFERRED_DELIVERY_TIME_RELATIVE:
    //             decodeSuccess = decodeDeferredDeliveryRel(bData, inStream);
    //             break;
    //         case SUBPARAM_PRIVACY_INDICATOR:
    //             decodeSuccess = decodePrivacyIndicator(bData, inStream);
    //             break;
    //         case SUBPARAM_LANGUAGE_INDICATOR:
    //             decodeSuccess = decodeLanguageIndicator(bData, inStream);
    //             break;
    //         case SUBPARAM_MESSAGE_DISPLAY_MODE:
    //             decodeSuccess = decodeDisplayMode(bData, inStream);
    //             break;
    //         case SUBPARAM_PRIORITY_INDICATOR:
    //             decodeSuccess = decodePriorityIndicator(bData, inStream);
    //             break;
    //         case SUBPARAM_ALERT_ON_MESSAGE_DELIVERY:
    //             decodeSuccess = decodeMsgDeliveryAlert(bData, inStream);
    //             break;
    //         case SUBPARAM_MESSAGE_DEPOSIT_INDEX:
    //             decodeSuccess = decodeDepositIndex(bData, inStream);
    //             break;
    //         case SUBPARAM_SERVICE_CATEGORY_PROGRAM_DATA:
    //             decodeSuccess = decodeServiceCategoryProgramData(bData, inStream);
    //             break;
    //         default:
    //             decodeSuccess = decodeReserved(bData, inStream, subparamId);
    //         }
    //         if (decodeSuccess &&
    //                 (subparamId >= SUBPARAM_MESSAGE_IDENTIFIER &&
    //                 subparamId <= SUBPARAM_ID_LAST_DEFINED)) {
    //             foundSubparamMask |= subparamIdBit;
    //         }
    //     }
    //     if ((foundSubparamMask & (1 << SUBPARAM_MESSAGE_IDENTIFIER)) == 0) {
    //         throw new CodingException("missing MESSAGE_IDENTIFIER subparam");
    //     }
    //     if (bData.userData != null) {
    //         if (isCmasAlertCategory(serviceCategory)) {
    //             decodeCmasUserData(bData, serviceCategory);
    //         } else if (bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) {
    //             if ((foundSubparamMask ^
    //                      (1 << SUBPARAM_MESSAGE_IDENTIFIER) ^
    //                      (1 << SUBPARAM_USER_DATA))
    //                     != 0) {
    //                 Rlog.e(LOGTAG, "IS-91 must occur without extra subparams (" +
    //                       foundSubparamMask + ")");
    //             }
    //             decodeIs91(bData);
    //         } else {
    //             decodeUserDataPayload(bData.userData, bData.hasUserDataHeader);
    //         }
    //     }
    //     return bData;
    // } catch (BitwiseInputStream.AccessException ex) {
    //     Rlog.e(LOGTAG, "BearerData decode failed: " + ex);
    // } catch (CodingException ex) {
    //     Rlog.e(LOGTAG, "BearerData decode failed: " + ex);
    // }
    // return null;
    assert(0);
    AutoPtr<BearerData> empty;
    return empty;
}

String BearerData::GetLanguageCodeForValue(
    /* [in] */ Int32 languageValue)
{
    // ==================before translated======================
    // switch (languageValue) {
    //     case LANGUAGE_ENGLISH:
    //         return "en";
    //
    //     case LANGUAGE_FRENCH:
    //         return "fr";
    //
    //     case LANGUAGE_SPANISH:
    //         return "es";
    //
    //     case LANGUAGE_JAPANESE:
    //         return "ja";
    //
    //     case LANGUAGE_KOREAN:
    //         return "ko";
    //
    //     case LANGUAGE_CHINESE:
    //         return "zh";
    //
    //     case LANGUAGE_HEBREW:
    //         return "he";
    //
    //     default:
    //         return null;
    // }
    assert(0);
    return String("");
}

void BearerData::EncodeMessageId(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 3);
    // outStream.write(4, bData.messageType);
    // outStream.write(8, bData.messageId >> 8);
    // outStream.write(8, bData.messageId);
    // outStream.write(1, bData.hasUserDataHeader ? 1 : 0);
    // outStream.skip(3);
    assert(0);
}

Int32 BearerData::CountAsciiSeptets(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean force)
{
    // ==================before translated======================
    // int msgLen = msg.length();
    // if (force) return msgLen;
    // for (int i = 0; i < msgLen; i++) {
    //     if (UserData.charToAscii.get(msg.charAt(i), -1) == -1) {
    //         return -1;
    //     }
    // }
    // return msgLen;
    assert(0);
    return 0;
}

AutoPtr< ArrayOf<Byte> > BearerData::Encode7bitAscii(
    /* [in] */ const String& msg,
    /* [in] */ Boolean force)
{
    // ==================before translated======================
    // try {
    //     BitwiseOutputStream outStream = new BitwiseOutputStream(msg.length());
    //     int msgLen = msg.length();
    //     for (int i = 0; i < msgLen; i++) {
    //         int charCode = UserData.charToAscii.get(msg.charAt(i), -1);
    //         if (charCode == -1) {
    //             if (force) {
    //                 outStream.write(7, UserData.UNENCODABLE_7_BIT_CHAR);
    //             } else {
    //                 throw new CodingException("cannot ASCII encode (" + msg.charAt(i) + ")");
    //             }
    //         } else {
    //             outStream.write(7, charCode);
    //         }
    //     }
    //     return outStream.toByteArray();
    // } catch (BitwiseOutputStream.AccessException ex) {
    //     throw new CodingException("7bit ASCII encode failed: " + ex);
    // }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr< ArrayOf<Byte> > BearerData::EncodeUtf16(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // try {
    //     return msg.getBytes("utf-16be");
    // } catch (java.io.UnsupportedEncodingException ex) {
    //     throw new CodingException("UTF-16 encode failed: " + ex);
    // }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

AutoPtr<BearerData::Gsm7bitCodingResult> BearerData::Encode7bitGsm(
    /* [in] */ const String& msg,
    /* [in] */ Int32 septetOffset,
    /* [in] */ Boolean force)
{
    // ==================before translated======================
    // try {
    //     /*
    //      * TODO(cleanup): It would be nice if GsmAlphabet provided
    //      * an option to produce just the data without prepending
    //      * the septet count, as this function is really just a
    //      * wrapper to strip that off.  Not to mention that the
    //      * septet count is generally known prior to invocation of
    //      * the encoder.  Note that it cannot be derived from the
    //      * resulting array length, since that cannot distinguish
    //      * if the last contains either 1 or 8 valid bits.
    //      *
    //      * TODO(cleanup): The BitwiseXStreams could also be
    //      * extended with byte-wise reversed endianness read/write
    //      * routines to allow a corresponding implementation of
    //      * stringToGsm7BitPacked, and potentially directly support
    //      * access to the main bitwise stream from encode/decode.
    //      */
    //     byte[] fullData = GsmAlphabet.stringToGsm7BitPacked(msg, septetOffset, !force, 0, 0);
    //     Gsm7bitCodingResult result = new Gsm7bitCodingResult();
    //     result.data = new byte[fullData.length - 1];
    //     System.arraycopy(fullData, 1, result.data, 0, fullData.length - 1);
    //     result.septets = fullData[0] & 0x00FF;
    //     return result;
    // } catch (com.android.internal.telephony.EncodeException ex) {
    //     throw new CodingException("7bit GSM encode failed: " + ex);
    // }
    assert(0);
    AutoPtr<Gsm7bitCodingResult> empty;
    return empty;
}

void BearerData::Encode7bitEms(
    /* [in] */ UserData* uData,
    /* [in] */ ArrayOf<Byte>* udhData,
    /* [in] */ Boolean force)
{
    // ==================before translated======================
    // int udhBytes = udhData.length + 1;  // Add length octet.
    // int udhSeptets = ((udhBytes * 8) + 6) / 7;
    // Gsm7bitCodingResult gcr = encode7bitGsm(uData.payloadStr, udhSeptets, force);
    // uData.msgEncoding = UserData.ENCODING_GSM_7BIT_ALPHABET;
    // uData.msgEncodingSet = true;
    // uData.numFields = gcr.septets;
    // uData.payload = gcr.data;
    // uData.payload[0] = (byte)udhData.length;
    // System.arraycopy(udhData, 0, uData.payload, 1, udhData.length);
    assert(0);
}

void BearerData::Encode16bitEms(
    /* [in] */ UserData* uData,
    /* [in] */ ArrayOf<Byte>* udhData)
{
    // ==================before translated======================
    // byte[] payload = encodeUtf16(uData.payloadStr);
    // int udhBytes = udhData.length + 1;  // Add length octet.
    // int udhCodeUnits = (udhBytes + 1) / 2;
    // int payloadCodeUnits = payload.length / 2;
    // uData.msgEncoding = UserData.ENCODING_UNICODE_16;
    // uData.msgEncodingSet = true;
    // uData.numFields = udhCodeUnits + payloadCodeUnits;
    // uData.payload = new byte[uData.numFields * 2];
    // uData.payload[0] = (byte)udhData.length;
    // System.arraycopy(udhData, 0, uData.payload, 1, udhData.length);
    // System.arraycopy(payload, 0, uData.payload, udhBytes, payload.length);
    assert(0);
}

void BearerData::Encode7bitAsciiEms(
    /* [in] */ UserData* uData,
    /* [in] */ ArrayOf<Byte>* udhData,
    /* [in] */ Boolean force)
{
    // ==================before translated======================
    // try {
    //     Rlog.d(LOGTAG, "encode7bitAsciiEms");
    //     int udhBytes = udhData.length + 1;  // Add length octet.
    //     int udhSeptets = ((udhBytes * 8) + 6) / 7;
    //     int paddingBits = (udhSeptets * 7) - (udhBytes * 8);
    //     String msg = uData.payloadStr;
    //     byte[] payload ;
    //     int msgLen = msg.length();
    //     BitwiseOutputStream outStream = new BitwiseOutputStream(msgLen +
    //             (paddingBits > 0 ? 1 : 0));
    //     outStream.write(paddingBits, 0);
    //     for (int i = 0; i < msgLen; i++) {
    //         int charCode = UserData.charToAscii.get(msg.charAt(i), -1);
    //         if (charCode == -1) {
    //             if (force) {
    //                 outStream.write(7, UserData.UNENCODABLE_7_BIT_CHAR);
    //             } else {
    //                 throw new CodingException("cannot ASCII encode (" + msg.charAt(i) + ")");
    //             }
    //         } else {
    //             outStream.write(7, charCode);
    //         }
    //     }
    //     payload = outStream.toByteArray();
    //     uData.msgEncoding = UserData.ENCODING_7BIT_ASCII;
    //     uData.msgEncodingSet = true;
    //     uData.numFields = udhSeptets + uData.payloadStr.length();
    //     uData.payload = new byte[udhBytes + payload.length];
    //     uData.payload[0] = (byte)udhData.length;
    //     System.arraycopy(udhData, 0, uData.payload, 1, udhData.length);
    //     System.arraycopy(payload, 0, uData.payload, udhBytes, payload.length);
    // } catch (BitwiseOutputStream.AccessException ex) {
    //     throw new CodingException("7bit ASCII encode failed: " + ex);
    // }
    assert(0);
}

void BearerData::EncodeEmsUserDataPayload(
    /* [in] */ UserData* uData)
{
    // ==================before translated======================
    // byte[] headerData = SmsHeader.toByteArray(uData.userDataHeader);
    // if (uData.msgEncodingSet) {
    //     if (uData.msgEncoding == UserData.ENCODING_GSM_7BIT_ALPHABET) {
    //         encode7bitEms(uData, headerData, true);
    //     } else if (uData.msgEncoding == UserData.ENCODING_UNICODE_16) {
    //         encode16bitEms(uData, headerData);
    //     } else if (uData.msgEncoding == UserData.ENCODING_7BIT_ASCII) {
    //         encode7bitAsciiEms(uData, headerData, true);
    //     } else {
    //         throw new CodingException("unsupported EMS user data encoding (" +
    //                                   uData.msgEncoding + ")");
    //     }
    // } else {
    //     try {
    //         encode7bitEms(uData, headerData, false);
    //     } catch (CodingException ex) {
    //         encode16bitEms(uData, headerData);
    //     }
    // }
    assert(0);
}

AutoPtr< ArrayOf<Byte> > BearerData::EncodeShiftJis(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // try {
    //     return msg.getBytes("Shift_JIS");
    // } catch (java.io.UnsupportedEncodingException ex) {
    //     throw new CodingException("Shift-JIS encode failed: " + ex);
    // }
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

void BearerData::EncodeUserDataPayload(
    /* [in] */ UserData* uData)
{
    // ==================before translated======================
    // if ((uData.payloadStr == null) && (uData.msgEncoding != UserData.ENCODING_OCTET)) {
    //     Rlog.e(LOGTAG, "user data with null payloadStr");
    //     uData.payloadStr = "";
    // }
    //
    // if (uData.userDataHeader != null) {
    //     encodeEmsUserDataPayload(uData);
    //     return;
    // }
    //
    // if (uData.msgEncodingSet) {
    //     if (uData.msgEncoding == UserData.ENCODING_OCTET) {
    //         if (uData.payload == null) {
    //             Rlog.e(LOGTAG, "user data with octet encoding but null payload");
    //             uData.payload = new byte[0];
    //             uData.numFields = 0;
    //         } else {
    //             uData.numFields = uData.payload.length;
    //         }
    //     } else {
    //         if (uData.payloadStr == null) {
    //             Rlog.e(LOGTAG, "non-octet user data with null payloadStr");
    //             uData.payloadStr = "";
    //         }
    //         if (uData.msgEncoding == UserData.ENCODING_GSM_7BIT_ALPHABET) {
    //             Gsm7bitCodingResult gcr = encode7bitGsm(uData.payloadStr, 0, true);
    //             uData.payload = gcr.data;
    //             uData.numFields = gcr.septets;
    //         } else if (uData.msgEncoding == UserData.ENCODING_7BIT_ASCII) {
    //             uData.payload = encode7bitAscii(uData.payloadStr, true);
    //             uData.numFields = uData.payloadStr.length();
    //         } else if (uData.msgEncoding == UserData.ENCODING_UNICODE_16) {
    //             uData.payload = encodeUtf16(uData.payloadStr);
    //             uData.numFields = uData.payloadStr.length();
    //         } else if (uData.msgEncoding == UserData.ENCODING_SHIFT_JIS) {
    //             uData.payload = encodeShiftJis(uData.payloadStr);
    //             uData.numFields = uData.payload.length;
    //         } else {
    //             throw new CodingException("unsupported user data encoding (" +
    //                                       uData.msgEncoding + ")");
    //         }
    //     }
    // } else {
    //     try {
    //         uData.payload = encode7bitAscii(uData.payloadStr, false);
    //         uData.msgEncoding = UserData.ENCODING_7BIT_ASCII;
    //     } catch (CodingException ex) {
    //         uData.payload = encodeUtf16(uData.payloadStr);
    //         uData.msgEncoding = UserData.ENCODING_UNICODE_16;
    //     }
    //     uData.numFields = uData.payloadStr.length();
    //     uData.msgEncodingSet = true;
    // }
    assert(0);
}

void BearerData::EncodeUserData(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // /*
    //  * TODO(cleanup): Do we really need to set userData.payload as
    //  * a side effect of encoding?  If not, we could avoid data
    //  * copies by passing outStream directly.
    //  */
    // encodeUserDataPayload(bData.userData);
    // bData.hasUserDataHeader = bData.userData.userDataHeader != null;
    //
    // if (bData.userData.payload.length > SmsConstants.MAX_USER_DATA_BYTES) {
    //     throw new CodingException("encoded user data too large (" +
    //                               bData.userData.payload.length +
    //                               " > " + SmsConstants.MAX_USER_DATA_BYTES + " bytes)");
    // }
    //
    // /*
    //  * TODO(cleanup): figure out what the right answer is WRT paddingBits field
    //  *
    //  *   userData.paddingBits = (userData.payload.length * 8) - (userData.numFields * 7);
    //  *   userData.paddingBits = 0; // XXX this seems better, but why?
    //  *
    //  */
    // int dataBits = (bData.userData.payload.length * 8) - bData.userData.paddingBits;
    // int paramBits = dataBits + 13;
    // if ((bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) ||
    //     (bData.userData.msgEncoding == UserData.ENCODING_GSM_DCS)) {
    //     paramBits += 8;
    // }
    // int paramBytes = (paramBits / 8) + ((paramBits % 8) > 0 ? 1 : 0);
    // int paddingBits = (paramBytes * 8) - paramBits;
    // outStream.write(8, paramBytes);
    // outStream.write(5, bData.userData.msgEncoding);
    // if ((bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) ||
    //     (bData.userData.msgEncoding == UserData.ENCODING_GSM_DCS)) {
    //     outStream.write(8, bData.userData.msgType);
    // }
    // outStream.write(8, bData.userData.numFields);
    // outStream.writeByteArray(dataBits, bData.userData.payload);
    // if (paddingBits > 0) outStream.write(paddingBits, 0);
    assert(0);
}

void BearerData::EncodeReplyOption(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(1, bData.userAckReq     ? 1 : 0);
    // outStream.write(1, bData.deliveryAckReq ? 1 : 0);
    // outStream.write(1, bData.readAckReq     ? 1 : 0);
    // outStream.write(1, bData.reportReq      ? 1 : 0);
    // outStream.write(4, 0);
    assert(0);
}

AutoPtr< ArrayOf<Byte> > BearerData::EncodeDtmfSmsAddress(
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // int digits = address.length();
    // int dataBits = digits * 4;
    // int dataBytes = (dataBits / 8);
    // dataBytes += (dataBits % 8) > 0 ? 1 : 0;
    // byte[] rawData = new byte[dataBytes];
    // for (int i = 0; i < digits; i++) {
    //     char c = address.charAt(i);
    //     int val = 0;
    //     if ((c >= '1') && (c <= '9')) val = c - '0';
    //     else if (c == '0') val = 10;
    //     else if (c == '*') val = 11;
    //     else if (c == '#') val = 12;
    //     else return null;
    //     rawData[i / 2] |= val << (4 - ((i % 2) * 4));
    // }
    // return rawData;
    assert(0);
    AutoPtr< ArrayOf<Byte> > empty;
    return empty;
}

void BearerData::EncodeCdmaSmsAddress(
    /* [in] */ CdmaSmsAddress* addr)
{
    // ==================before translated======================
    // if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
    //     try {
    //         addr.origBytes = addr.address.getBytes("US-ASCII");
    //     } catch (java.io.UnsupportedEncodingException ex) {
    //         throw new CodingException("invalid SMS address, cannot convert to ASCII");
    //     }
    // } else {
    //     addr.origBytes = encodeDtmfSmsAddress(addr.address);
    // }
    assert(0);
}

void BearerData::EncodeCallbackNumber(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // CdmaSmsAddress addr = bData.callbackNumber;
    // encodeCdmaSmsAddress(addr);
    // int paramBits = 9;
    // int dataBits = 0;
    // if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
    //     paramBits += 7;
    //     dataBits = addr.numberOfDigits * 8;
    // } else {
    //     dataBits = addr.numberOfDigits * 4;
    // }
    // paramBits += dataBits;
    // int paramBytes = (paramBits / 8) + ((paramBits % 8) > 0 ? 1 : 0);
    // int paddingBits = (paramBytes * 8) - paramBits;
    // outStream.write(8, paramBytes);
    // outStream.write(1, addr.digitMode);
    // if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
    //     outStream.write(3, addr.ton);
    //     outStream.write(4, addr.numberPlan);
    // }
    // outStream.write(8, addr.numberOfDigits);
    // outStream.writeByteArray(dataBits, addr.origBytes);
    // if (paddingBits > 0) outStream.write(paddingBits, 0);
    assert(0);
}

void BearerData::EncodeMsgStatus(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(2, bData.errorClass);
    // outStream.write(6, bData.messageStatus);
    assert(0);
}

void BearerData::EncodeMsgCount(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(8, bData.numberOfMessages);
    assert(0);
}

void BearerData::EncodeValidityPeriodRel(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(8, bData.validityPeriodRelative);
    assert(0);
}

void BearerData::EncodePrivacyIndicator(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(2, bData.privacy);
    // outStream.skip(6);
    assert(0);
}

void BearerData::EncodeLanguageIndicator(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(8, bData.language);
    assert(0);
}

void BearerData::EncodeDisplayMode(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(2, bData.displayMode);
    // outStream.skip(6);
    assert(0);
}

void BearerData::EncodePriorityIndicator(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(2, bData.priority);
    // outStream.skip(6);
    assert(0);
}

void BearerData::EncodeMsgDeliveryAlert(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // outStream.write(8, 1);
    // outStream.write(2, bData.alert);
    // outStream.skip(6);
    assert(0);
}

void BearerData::EncodeScpResults(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseOutputStream* outStream)
{
    // ==================before translated======================
    // ArrayList<CdmaSmsCbProgramResults> results = bData.serviceCategoryProgramResults;
    // outStream.write(8, (results.size() * 4));   // 4 octets per program result
    // for (CdmaSmsCbProgramResults result : results) {
    //     int category = result.getCategory();
    //     outStream.write(8, category >> 8);
    //     outStream.write(8, category);
    //     outStream.write(8, result.getLanguage());
    //     outStream.write(4, result.getCategoryResult());
    //     outStream.skip(4);
    // }
    assert(0);
}

Boolean BearerData::DecodeMessageId(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 3 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.messageType = inStream.read(4);
    //     bData.messageId = inStream.read(8) << 8;
    //     bData.messageId |= inStream.read(8);
    //     bData.hasUserDataHeader = (inStream.read(1) == 1);
    //     inStream.skip(3);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "MESSAGE_IDENTIFIER decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeReserved(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream,
    /* [in] */ Int32 subparamId)
{
    // ==================before translated======================
    // boolean decodeSuccess = false;
    // int subparamLen = inStream.read(8); // SUBPARAM_LEN
    // int paramBits = subparamLen * 8;
    // if (paramBits <= inStream.available()) {
    //     decodeSuccess = true;
    //     inStream.skip(paramBits);
    // }
    // Rlog.d(LOGTAG, "RESERVED bearer data subparameter " + subparamId + " decode "
    //         + (decodeSuccess ? "succeeded" : "failed") + " (param bits = " + paramBits + ")");
    // if (!decodeSuccess) {
    //     throw new CodingException("RESERVED bearer data subparameter " + subparamId
    //             + " had invalid SUBPARAM_LEN " + subparamLen);
    // }
    //
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeUserData(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // int paramBits = inStream.read(8) * 8;
    // bData.userData = new UserData();
    // bData.userData.msgEncoding = inStream.read(5);
    // bData.userData.msgEncodingSet = true;
    // bData.userData.msgType = 0;
    // int consumedBits = 5;
    // if ((bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) ||
    //     (bData.userData.msgEncoding == UserData.ENCODING_GSM_DCS)) {
    //     bData.userData.msgType = inStream.read(8);
    //     consumedBits += 8;
    // }
    // bData.userData.numFields = inStream.read(8);
    // consumedBits += 8;
    // int dataBits = paramBits - consumedBits;
    // bData.userData.payload = inStream.readByteArray(dataBits);
    // return true;
    assert(0);
    return FALSE;
}

String BearerData::DecodeUtf8(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // return decodeCharset(data, offset, numFields, 1, "UTF-8");
    assert(0);
    return String("");
}

String BearerData::DecodeUtf16(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // // Subtract header and possible padding byte (at end) from num fields.
    // int padding = offset % 2;
    // numFields -= (offset + padding) / 2;
    // return decodeCharset(data, offset, numFields, 2, "utf-16be");
    assert(0);
    return String("");
}

String BearerData::DecodeCharset(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields,
    /* [in] */ Int32 width,
    /* [in] */ const String& charset)
{
    // ==================before translated======================
    // if (numFields < 0 || (numFields * width + offset) > data.length) {
    //     // Try to decode the max number of characters in payload
    //     int padding = offset % width;
    //     int maxNumFields = (data.length - offset - padding) / width;
    //     if (maxNumFields < 0) {
    //         throw new CodingException(charset + " decode failed: offset out of range");
    //     }
    //     Rlog.e(LOGTAG, charset + " decode error: offset = " + offset + " numFields = "
    //             + numFields + " data.length = " + data.length + " maxNumFields = "
    //             + maxNumFields);
    //     numFields = maxNumFields;
    // }
    // try {
    //     return new String(data, offset, numFields * width, charset);
    // } catch (java.io.UnsupportedEncodingException ex) {
    //     throw new CodingException(charset + " decode failed: " + ex);
    // }
    assert(0);
    return String("");
}

String BearerData::Decode7bitAscii(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // try {
    //     int offsetBits = offset * 8;
    //     int offsetSeptets = (offsetBits + 6) / 7;
    //     numFields -= offsetSeptets;
    //     int paddingBits = (offsetSeptets * 7) - offsetBits;
    //
    //     StringBuffer strBuf = new StringBuffer(numFields);
    //     BitwiseInputStream inStream = new BitwiseInputStream(data);
    //     int wantedBits = (offsetSeptets * 7) + (numFields * 7);
    //     if (inStream.available() < wantedBits) {
    //         throw new CodingException("insufficient data (wanted " + wantedBits +
    //                                   " bits, but only have " + inStream.available() + ")");
    //     }
    //     inStream.skip(offsetBits + paddingBits);
    //     for (int i = 0; i < numFields; i++) {
    //         int charCode = inStream.read(7);
    //         if ((charCode >= UserData.ASCII_MAP_BASE_INDEX) &&
    //                 (charCode <= UserData.ASCII_MAP_MAX_INDEX)) {
    //             strBuf.append(UserData.ASCII_MAP[charCode - UserData.ASCII_MAP_BASE_INDEX]);
    //         } else if (charCode == UserData.ASCII_NL_INDEX) {
    //             strBuf.append('\n');
    //         } else if (charCode == UserData.ASCII_CR_INDEX) {
    //             strBuf.append('\r');
    //         } else {
    //             /* For other charCodes, they are unprintable, and so simply use SPACE. */
    //             strBuf.append(' ');
    //         }
    //     }
    //     return strBuf.toString();
    // } catch (BitwiseInputStream.AccessException ex) {
    //     throw new CodingException("7bit ASCII decode failed: " + ex);
    // }
    assert(0);
    return String("");
}

String BearerData::Decode7bitGsm(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // // Start reading from the next 7-bit aligned boundary after offset.
    // int offsetBits = offset * 8;
    // int offsetSeptets = (offsetBits + 6) / 7;
    // numFields -= offsetSeptets;
    // int paddingBits = (offsetSeptets * 7) - offsetBits;
    // String result = GsmAlphabet.gsm7BitPackedToString(data, offset, numFields, paddingBits,
    //         0, 0);
    // if (result == null) {
    //     throw new CodingException("7bit GSM decoding failed");
    // }
    // return result;
    assert(0);
    return String("");
}

String BearerData::DecodeLatin(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // return decodeCharset(data, offset, numFields, 1, "ISO-8859-1");
    assert(0);
    return String("");
}

String BearerData::DecodeShiftJis(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // return decodeCharset(data, offset, numFields, 1, "Shift_JIS");
    assert(0);
    return String("");
}

String BearerData::DecodeGsmDcs(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 numFields,
    /* [in] */ Int32 msgType)
{
    // ==================before translated======================
    // switch ((msgType >> 2) & 0x3) {
    // case UserData.ENCODING_GSM_DCS_7BIT:
    //     return decode7bitGsm(data, offset, numFields);
    // case UserData.ENCODING_GSM_DCS_8BIT:
    //     return decodeUtf8(data, offset, numFields);
    // case UserData.ENCODING_GSM_DCS_16BIT:
    //     return decodeUtf16(data, offset, numFields);
    // default:
    //     throw new CodingException("unsupported user msgType encoding ("
    //             + msgType + ")");
    // }
    assert(0);
    return String("");
}

void BearerData::DecodeUserDataPayload(
    /* [in] */ UserData* userData,
    /* [in] */ Boolean hasUserDataHeader)
{
    // ==================before translated======================
    // int offset = 0;
    // if (hasUserDataHeader) {
    //     int udhLen = userData.payload[0] & 0x00FF;
    //     offset += udhLen + 1;
    //     byte[] headerData = new byte[udhLen];
    //     System.arraycopy(userData.payload, 1, headerData, 0, udhLen);
    //     userData.userDataHeader = SmsHeader.fromByteArray(headerData);
    // }
    // switch (userData.msgEncoding) {
    // case UserData.ENCODING_OCTET:
    //     /*
    //     *  Octet decoding depends on the carrier service.
    //     */
    //     boolean decodingtypeUTF8 = Resources.getSystem()
    //             .getBoolean(com.android.internal.R.bool.config_sms_utf8_support);
    //
    //     // Strip off any padding bytes, meaning any differences between the length of the
    //     // array and the target length specified by numFields.  This is to avoid any
    //     // confusion by code elsewhere that only considers the payload array length.
    //     byte[] payload = new byte[userData.numFields];
    //     int copyLen = userData.numFields < userData.payload.length
    //             ? userData.numFields : userData.payload.length;
    //
    //     System.arraycopy(userData.payload, 0, payload, 0, copyLen);
    //     userData.payload = payload;
    //
    //     if (!decodingtypeUTF8) {
    //         // There are many devices in the market that send 8bit text sms (latin encoded) as
    //         // octet encoded.
    //         userData.payloadStr = decodeLatin(userData.payload, offset, userData.numFields);
    //     } else {
    //         userData.payloadStr = decodeUtf8(userData.payload, offset, userData.numFields);
    //     }
    //     break;
    //
    // case UserData.ENCODING_IA5:
    // case UserData.ENCODING_7BIT_ASCII:
    //     userData.payloadStr = decode7bitAscii(userData.payload, offset, userData.numFields);
    //     break;
    // case UserData.ENCODING_UNICODE_16:
    //     userData.payloadStr = decodeUtf16(userData.payload, offset, userData.numFields);
    //     break;
    // case UserData.ENCODING_GSM_7BIT_ALPHABET:
    //     userData.payloadStr = decode7bitGsm(userData.payload, offset, userData.numFields);
    //     break;
    // case UserData.ENCODING_LATIN:
    //     userData.payloadStr = decodeLatin(userData.payload, offset, userData.numFields);
    //     break;
    // case UserData.ENCODING_SHIFT_JIS:
    //     userData.payloadStr = decodeShiftJis(userData.payload, offset, userData.numFields);
    //     break;
    // case UserData.ENCODING_GSM_DCS:
    //     userData.payloadStr = decodeGsmDcs(userData.payload, offset,
    //             userData.numFields, userData.msgType);
    //     break;
    // default:
    //     throw new CodingException("unsupported user data encoding ("
    //                               + userData.msgEncoding + ")");
    // }
    assert(0);
}

void BearerData::DecodeIs91VoicemailStatus(
    /* [in] */ BearerData* bData)
{
    // ==================before translated======================
    //  BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
    //  int dataLen = inStream.available() / 6;  // 6-bit packed character encoding.
    //  int numFields = bData.userData.numFields;
    //  if ((dataLen > 14) || (dataLen < 3) || (dataLen < numFields)) {
    //      throw new CodingException("IS-91 voicemail status decoding failed");
    //  }
    //  try {
    //      StringBuffer strbuf = new StringBuffer(dataLen);
    //      while (inStream.available() >= 6) {
    //          strbuf.append(UserData.ASCII_MAP[inStream.read(6)]);
    //      }
    //      String data = strbuf.toString();
    //      bData.numberOfMessages = Integer.parseInt(data.substring(0, 2));
    //      char prioCode = data.charAt(2);
    //      if (prioCode == ' ') {
    //          bData.priority = PRIORITY_NORMAL;
    //      } else if (prioCode == '!') {
    //          bData.priority = PRIORITY_URGENT;
    //      } else {
    //          throw new CodingException("IS-91 voicemail status decoding failed: " +
    //                  "illegal priority setting (" + prioCode + ")");
    //      }
    //      bData.priorityIndicatorSet = true;
    //      bData.userData.payloadStr = data.substring(3, numFields - 3);
    // } catch (java.lang.NumberFormatException ex) {
    //      throw new CodingException("IS-91 voicemail status decoding failed: " + ex);
    //  } catch (java.lang.IndexOutOfBoundsException ex) {
    //      throw new CodingException("IS-91 voicemail status decoding failed: " + ex);
    //  }
    assert(0);
}

void BearerData::DecodeIs91ShortMessage(
    /* [in] */ BearerData* bData)
{
    // ==================before translated======================
    // BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
    // int dataLen = inStream.available() / 6;  // 6-bit packed character encoding.
    // int numFields = bData.userData.numFields;
    // // dataLen may be > 14 characters due to octet padding
    // if ((numFields > 14) || (dataLen < numFields)) {
    //     throw new CodingException("IS-91 short message decoding failed");
    // }
    // StringBuffer strbuf = new StringBuffer(dataLen);
    // for (int i = 0; i < numFields; i++) {
    //     strbuf.append(UserData.ASCII_MAP[inStream.read(6)]);
    // }
    // bData.userData.payloadStr = strbuf.toString();
    assert(0);
}

void BearerData::DecodeIs91Cli(
    /* [in] */ BearerData* bData)
{
    // ==================before translated======================
    // BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
    // int dataLen = inStream.available() / 4;  // 4-bit packed DTMF digit encoding.
    // int numFields = bData.userData.numFields;
    // if ((dataLen > 14) || (dataLen < 3) || (dataLen < numFields)) {
    //     throw new CodingException("IS-91 voicemail status decoding failed");
    // }
    // CdmaSmsAddress addr = new CdmaSmsAddress();
    // addr.digitMode = CdmaSmsAddress.DIGIT_MODE_4BIT_DTMF;
    // addr.origBytes = bData.userData.payload;
    // addr.numberOfDigits = (byte)numFields;
    // decodeSmsAddress(addr);
    // bData.callbackNumber = addr;
    assert(0);
}

void BearerData::DecodeIs91(
    /* [in] */ BearerData* bData)
{
    // ==================before translated======================
    // switch (bData.userData.msgType) {
    // case UserData.IS91_MSG_TYPE_VOICEMAIL_STATUS:
    //     decodeIs91VoicemailStatus(bData);
    //     break;
    // case UserData.IS91_MSG_TYPE_CLI:
    //     decodeIs91Cli(bData);
    //     break;
    // case UserData.IS91_MSG_TYPE_SHORT_MESSAGE_FULL:
    // case UserData.IS91_MSG_TYPE_SHORT_MESSAGE:
    //     decodeIs91ShortMessage(bData);
    //     break;
    // default:
    //     throw new CodingException("unsupported IS-91 message type (" +
    //             bData.userData.msgType + ")");
    // }
    assert(0);
}

Boolean BearerData::DecodeReplyOption(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.userAckReq     = (inStream.read(1) == 1);
    //     bData.deliveryAckReq = (inStream.read(1) == 1);
    //     bData.readAckReq     = (inStream.read(1) == 1);
    //     bData.reportReq      = (inStream.read(1) == 1);
    //     inStream.skip(4);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "REPLY_OPTION decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeMsgCount(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.numberOfMessages = IccUtils.cdmaBcdByteToInt((byte)inStream.read(8));
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "NUMBER_OF_MESSAGES decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeDepositIndex(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 2 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.depositIndex = (inStream.read(8) << 8) | inStream.read(8);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "MESSAGE_DEPOSIT_INDEX decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

String BearerData::DecodeDtmfSmsAddress(
    /* [in] */ ArrayOf<Byte>* rawData,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // /* DTMF 4-bit digit encoding, defined in at
    //  * 3GPP2 C.S005-D, v2.0, table 2.7.1.3.2.4-4 */
    // StringBuffer strBuf = new StringBuffer(numFields);
    // for (int i = 0; i < numFields; i++) {
    //     int val = 0x0F & (rawData[i / 2] >>> (4 - ((i % 2) * 4)));
    //     if ((val >= 1) && (val <= 9)) strBuf.append(Integer.toString(val, 10));
    //     else if (val == 10) strBuf.append('0');
    //     else if (val == 11) strBuf.append('*');
    //     else if (val == 12) strBuf.append('#');
    //     else throw new CodingException("invalid SMS address DTMF code (" + val + ")");
    // }
    // return strBuf.toString();
    assert(0);
    return String("");
}

void BearerData::DecodeSmsAddress(
    /* [in] */ CdmaSmsAddress* addr)
{
    // ==================before translated======================
    // if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
    //     try {
    //         /* As specified in 3GPP2 C.S0015-B, v2, 4.5.15 -- actually
    //          * just 7-bit ASCII encoding, with the MSB being zero. */
    //         addr.address = new String(addr.origBytes, 0, addr.origBytes.length, "US-ASCII");
    //     } catch (java.io.UnsupportedEncodingException ex) {
    //         throw new CodingException("invalid SMS address ASCII code");
    //     }
    // } else {
    //     addr.address = decodeDtmfSmsAddress(addr.origBytes, addr.numberOfDigits);
    // }
    assert(0);
}

Boolean BearerData::DecodeCallbackNumber(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8; //at least
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits < EXPECTED_PARAM_SIZE) {
    //     inStream.skip(paramBits);
    //     return false;
    // }
    // CdmaSmsAddress addr = new CdmaSmsAddress();
    // addr.digitMode = inStream.read(1);
    // byte fieldBits = 4;
    // byte consumedBits = 1;
    // if (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
    //     addr.ton = inStream.read(3);
    //     addr.numberPlan = inStream.read(4);
    //     fieldBits = 8;
    //     consumedBits += 7;
    // }
    // addr.numberOfDigits = inStream.read(8);
    // consumedBits += 8;
    // int remainingBits = paramBits - consumedBits;
    // int dataBits = addr.numberOfDigits * fieldBits;
    // int paddingBits = remainingBits - dataBits;
    // if (remainingBits < dataBits) {
    //     throw new CodingException("CALLBACK_NUMBER subparam encoding size error (" +
    //                               "remainingBits + " + remainingBits + ", dataBits + " +
    //                               dataBits + ", paddingBits + " + paddingBits + ")");
    // }
    // addr.origBytes = inStream.readByteArray(dataBits);
    // inStream.skip(paddingBits);
    // decodeSmsAddress(addr);
    // bData.callbackNumber = addr;
    // return true;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeMsgStatus(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.errorClass = inStream.read(2);
    //     bData.messageStatus = inStream.read(6);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "MESSAGE_STATUS decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.messageStatusSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeMsgCenterTimeStamp(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 6 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.msgCenterTimeStamp = TimeStamp.fromByteArray(inStream.readByteArray(6 * 8));
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "MESSAGE_CENTER_TIME_STAMP decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeValidityAbs(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 6 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.validityPeriodAbsolute = TimeStamp.fromByteArray(inStream.readByteArray(6 * 8));
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "VALIDITY_PERIOD_ABSOLUTE decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeDeferredDeliveryAbs(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 6 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.deferredDeliveryTimeAbsolute = TimeStamp.fromByteArray(
    //             inStream.readByteArray(6 * 8));
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "DEFERRED_DELIVERY_TIME_ABSOLUTE decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeValidityRel(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.deferredDeliveryTimeRelative = inStream.read(8);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "VALIDITY_PERIOD_RELATIVE decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.deferredDeliveryTimeRelativeSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeDeferredDeliveryRel(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.validityPeriodRelative = inStream.read(8);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "DEFERRED_DELIVERY_TIME_RELATIVE decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.validityPeriodRelativeSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodePrivacyIndicator(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.privacy = inStream.read(2);
    //     inStream.skip(6);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "PRIVACY_INDICATOR decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.privacyIndicatorSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeLanguageIndicator(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.language = inStream.read(8);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "LANGUAGE_INDICATOR decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.languageIndicatorSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeDisplayMode(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.displayMode = inStream.read(2);
    //     inStream.skip(6);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "DISPLAY_MODE decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.displayModeSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodePriorityIndicator(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.priority = inStream.read(2);
    //     inStream.skip(6);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "PRIORITY_INDICATOR decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.priorityIndicatorSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeMsgDeliveryAlert(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.alert = inStream.read(2);
    //     inStream.skip(6);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "ALERT_ON_MESSAGE_DELIVERY decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.alertIndicatorSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeUserResponseCode(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // final int EXPECTED_PARAM_SIZE = 1 * 8;
    // boolean decodeSuccess = false;
    // int paramBits = inStream.read(8) * 8;
    // if (paramBits >= EXPECTED_PARAM_SIZE) {
    //     paramBits -= EXPECTED_PARAM_SIZE;
    //     decodeSuccess = true;
    //     bData.userResponseCode = inStream.read(8);
    // }
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "USER_RESPONSE_CODE decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ")");
    // }
    // inStream.skip(paramBits);
    // bData.userResponseCodeSet = decodeSuccess;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Boolean BearerData::DecodeServiceCategoryProgramData(
    /* [in] */ BearerData* bData,
    /* [in] */ IBitwiseInputStream* inStream)
{
    // ==================before translated======================
    // if (inStream.available() < 13) {
    //     throw new CodingException("SERVICE_CATEGORY_PROGRAM_DATA decode failed: only "
    //             + inStream.available() + " bits available");
    // }
    //
    // int paramBits = inStream.read(8) * 8;
    // int msgEncoding = inStream.read(5);
    // paramBits -= 5;
    //
    // if (inStream.available() < paramBits) {
    //     throw new CodingException("SERVICE_CATEGORY_PROGRAM_DATA decode failed: only "
    //             + inStream.available() + " bits available (" + paramBits + " bits expected)");
    // }
    //
    // ArrayList<CdmaSmsCbProgramData> programDataList = new ArrayList<CdmaSmsCbProgramData>();
    //
    // final int CATEGORY_FIELD_MIN_SIZE = 6 * 8;
    // boolean decodeSuccess = false;
    // while (paramBits >= CATEGORY_FIELD_MIN_SIZE) {
    //     int operation = inStream.read(4);
    //     int category = (inStream.read(8) << 8) | inStream.read(8);
    //     int language = inStream.read(8);
    //     int maxMessages = inStream.read(8);
    //     int alertOption = inStream.read(4);
    //     int numFields = inStream.read(8);
    //     paramBits -= CATEGORY_FIELD_MIN_SIZE;
    //
    //     int textBits = getBitsForNumFields(msgEncoding, numFields);
    //     if (paramBits < textBits) {
    //         throw new CodingException("category name is " + textBits + " bits in length,"
    //                 + " but there are only " + paramBits + " bits available");
    //     }
    //
    //     UserData userData = new UserData();
    //     userData.msgEncoding = msgEncoding;
    //     userData.msgEncodingSet = true;
    //     userData.numFields = numFields;
    //     userData.payload = inStream.readByteArray(textBits);
    //     paramBits -= textBits;
    //
    //     decodeUserDataPayload(userData, false);
    //     String categoryName = userData.payloadStr;
    //     CdmaSmsCbProgramData programData = new CdmaSmsCbProgramData(operation, category,
    //             language, maxMessages, alertOption, categoryName);
    //     programDataList.add(programData);
    //
    //     decodeSuccess = true;
    // }
    //
    // if ((! decodeSuccess) || (paramBits > 0)) {
    //     Rlog.d(LOGTAG, "SERVICE_CATEGORY_PROGRAM_DATA decode " +
    //               (decodeSuccess ? "succeeded" : "failed") +
    //               " (extra bits = " + paramBits + ')');
    // }
    //
    // inStream.skip(paramBits);
    // bData.serviceCategoryProgramData = programDataList;
    // return decodeSuccess;
    assert(0);
    return FALSE;
}

Int32 BearerData::ServiceCategoryToCmasMessageClass(
    /* [in] */ Int32 serviceCategory)
{
    // ==================before translated======================
    // switch (serviceCategory) {
    //     case SmsEnvelope.SERVICE_CATEGORY_CMAS_PRESIDENTIAL_LEVEL_ALERT:
    //         return SmsCbCmasInfo.CMAS_CLASS_PRESIDENTIAL_LEVEL_ALERT;
    //
    //     case SmsEnvelope.SERVICE_CATEGORY_CMAS_EXTREME_THREAT:
    //         return SmsCbCmasInfo.CMAS_CLASS_EXTREME_THREAT;
    //
    //     case SmsEnvelope.SERVICE_CATEGORY_CMAS_SEVERE_THREAT:
    //         return SmsCbCmasInfo.CMAS_CLASS_SEVERE_THREAT;
    //
    //     case SmsEnvelope.SERVICE_CATEGORY_CMAS_CHILD_ABDUCTION_EMERGENCY:
    //         return SmsCbCmasInfo.CMAS_CLASS_CHILD_ABDUCTION_EMERGENCY;
    //
    //     case SmsEnvelope.SERVICE_CATEGORY_CMAS_TEST_MESSAGE:
    //         return SmsCbCmasInfo.CMAS_CLASS_REQUIRED_MONTHLY_TEST;
    //
    //     default:
    //         return SmsCbCmasInfo.CMAS_CLASS_UNKNOWN;
    // }
    assert(0);
    return 0;
}

Int32 BearerData::GetBitsForNumFields(
    /* [in] */ Int32 msgEncoding,
    /* [in] */ Int32 numFields)
{
    // ==================before translated======================
    // switch (msgEncoding) {
    //     case UserData.ENCODING_OCTET:
    //     case UserData.ENCODING_SHIFT_JIS:
    //     case UserData.ENCODING_KOREAN:
    //     case UserData.ENCODING_LATIN:
    //     case UserData.ENCODING_LATIN_HEBREW:
    //         return numFields * 8;
    //
    //     case UserData.ENCODING_IA5:
    //     case UserData.ENCODING_7BIT_ASCII:
    //     case UserData.ENCODING_GSM_7BIT_ALPHABET:
    //         return numFields * 7;
    //
    //     case UserData.ENCODING_UNICODE_16:
    //         return numFields * 16;
    //
    //     default:
    //         throw new CodingException("unsupported message encoding (" + msgEncoding + ')');
    // }
    assert(0);
    return 0;
}

void BearerData::DecodeCmasUserData(
    /* [in] */ BearerData* bData,
    /* [in] */ Int32 serviceCategory)
{
    // ==================before translated======================
    // BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
    // if (inStream.available() < 8) {
    //     throw new CodingException("emergency CB with no CMAE_protocol_version");
    // }
    // int protocolVersion = inStream.read(8);
    // if (protocolVersion != 0) {
    //     throw new CodingException("unsupported CMAE_protocol_version " + protocolVersion);
    // }
    //
    // int messageClass = serviceCategoryToCmasMessageClass(serviceCategory);
    // int category = SmsCbCmasInfo.CMAS_CATEGORY_UNKNOWN;
    // int responseType = SmsCbCmasInfo.CMAS_RESPONSE_TYPE_UNKNOWN;
    // int severity = SmsCbCmasInfo.CMAS_SEVERITY_UNKNOWN;
    // int urgency = SmsCbCmasInfo.CMAS_URGENCY_UNKNOWN;
    // int certainty = SmsCbCmasInfo.CMAS_CERTAINTY_UNKNOWN;
    //
    // while (inStream.available() >= 16) {
    //     int recordType = inStream.read(8);
    //     int recordLen = inStream.read(8);
    //     switch (recordType) {
    //         case 0:     // Type 0 elements (Alert text)
    //             UserData alertUserData = new UserData();
    //             alertUserData.msgEncoding = inStream.read(5);
    //             alertUserData.msgEncodingSet = true;
    //             alertUserData.msgType = 0;
    //
    //             int numFields;                          // number of chars to decode
    //             switch (alertUserData.msgEncoding) {
    //                 case UserData.ENCODING_OCTET:
    //                 case UserData.ENCODING_LATIN:
    //                     numFields = recordLen - 1;      // subtract 1 byte for encoding
    //                     break;
    //
    //                 case UserData.ENCODING_IA5:
    //                 case UserData.ENCODING_7BIT_ASCII:
    //                 case UserData.ENCODING_GSM_7BIT_ALPHABET:
    //                     numFields = ((recordLen * 8) - 5) / 7;  // subtract 5 bits for encoding
    //                     break;
    //
    //                 case UserData.ENCODING_UNICODE_16:
    //                     numFields = (recordLen - 1) / 2;
    //                     break;
    //
    //                 default:
    //                     numFields = 0;      // unsupported encoding
    //             }
    //
    //             alertUserData.numFields = numFields;
    //             alertUserData.payload = inStream.readByteArray(recordLen * 8 - 5);
    //             decodeUserDataPayload(alertUserData, false);
    //             bData.userData = alertUserData;
    //             break;
    //
    //         case 1:     // Type 1 elements
    //             category = inStream.read(8);
    //             responseType = inStream.read(8);
    //             severity = inStream.read(4);
    //             urgency = inStream.read(4);
    //             certainty = inStream.read(4);
    //             inStream.skip(recordLen * 8 - 28);
    //             break;
    //
    //         default:
    //             Rlog.w(LOGTAG, "skipping unsupported CMAS record type " + recordType);
    //             inStream.skip(recordLen * 8);
    //             break;
    //     }
    // }
    //
    // bData.cmasWarningInfo = new SmsCbCmasInfo(messageClass, category, responseType, severity,
    //         urgency, certainty);
    assert(0);
}

Boolean BearerData::IsCmasAlertCategory(
    /* [in] */ Int32 category)
{
    // ==================before translated======================
    // return category >= SmsEnvelope.SERVICE_CATEGORY_CMAS_PRESIDENTIAL_LEVEL_ALERT
    //         && category <= SmsEnvelope.SERVICE_CATEGORY_CMAS_LAST_RESERVED_VALUE;
    assert(0);
    return FALSE;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


