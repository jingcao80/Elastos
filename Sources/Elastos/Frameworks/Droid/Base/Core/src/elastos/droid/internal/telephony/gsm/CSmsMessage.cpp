#include "elastos/droid/internal/telephony/gsm/CSmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/CGsmSmsAddress.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"

using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::GsmAlphabet;
using Elastos::Droid::Telephony::PhoneNumberUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CSmsMessage::TAG("SmsMessage");
const Boolean CSmsMessage::VDBG;
const Int32 CSmsMessage::VALIDITY_PERIOD_FORMAT_NONE;
const Int32 CSmsMessage::VALIDITY_PERIOD_FORMAT_ENHANCED;
const Int32 CSmsMessage::VALIDITY_PERIOD_FORMAT_RELATIVE;
const Int32 CSmsMessage::VALIDITY_PERIOD_FORMAT_ABSOLUTE;
const Int32 CSmsMessage::VALIDITY_PERIOD_MIN;
const Int32 CSmsMessage::VALIDITY_PERIOD_MAX;
const Int32 CSmsMessage::INVALID_VALIDITY_PERIOD;

//==============================================================================
//  CSmsMessage::PduParser
//==============================================================================

CSmsMessage::PduParser::PduParser(
    /* [in] */ ArrayOf<Byte>* pdu)
    : mPdu(pdu)
    , mCur(0)
    , mUserDataSeptetPadding(0)
{
}

ECode CSmsMessage::PduParser::GetSCAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 len;
    String ret;

    // length of SC Address
    GetByte(&len);

    if (len == 0) {
        // no SC address
        ret = NULL;
    }
    else {
        // SC address
        // try {
        PhoneNumberUtils::CalledPartyBCDToString(mPdu, mCur, len, &ret);
        // } catch (RuntimeException tr) {
        //     Rlog.d(TAG, "invalid SC address: ", tr);
        //     ret = NULL;
        // }
    }

    mCur += len;

    *result = ret;
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetByte(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (*mPdu)[mCur++] & 0xff;
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetAddress(
    /* [out] */ IGsmSmsAddress** result)
{
    VALIDATE_NOT_NULL(result)

    // "The Address-Length field is an integer representation of
    // the number field, i.e. excludes any semi-octet containing only
    // fill bits."
    // The TOA field is not included as part of this
    Int32 addressLength = (*mPdu)[mCur] & 0xff;
    Int32 lengthBytes = 2 + (addressLength + 1) / 2;

    // try {
    CGsmSmsAddress::New(mPdu, mCur, lengthBytes, result);
    // } catch (ParseException e) {
    //     ret = NULL;
    //     //This is caught by createFromPdu(byte[] pdu)
    //     throw new RuntimeException(e.getMessage());
    // }

    mCur += lengthBytes;

    return NOERROR;
}

ECode CSmsMessage::PduParser::GetSCTimestampMillis(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // // TP-Service-Centre-Time-Stamp
    // int year = IccUtils.gsmBcdByteToInt(mPdu[mCur++]);
    // int month = IccUtils.gsmBcdByteToInt(mPdu[mCur++]);
    // int day = IccUtils.gsmBcdByteToInt(mPdu[mCur++]);
    // int hour = IccUtils.gsmBcdByteToInt(mPdu[mCur++]);
    // int minute = IccUtils.gsmBcdByteToInt(mPdu[mCur++]);
    // int second = IccUtils.gsmBcdByteToInt(mPdu[mCur++]);

    // // For the timezone, the most significant bit of the
    // // least significant nibble is the sign byte
    // // (meaning the max range of this field is 79 quarter-hours,
    // // which is more than enough)

    // byte tzByte = mPdu[mCur++];

    // // Mask out sign bit.
    // int timezoneOffset = IccUtils.gsmBcdByteToInt((byte) (tzByte & (~0x08)));

    // timezoneOffset = ((tzByte & 0x08) == 0) ? timezoneOffset : -timezoneOffset;

    // Time time = new Time(Time.TIMEZONE_UTC);

    // // It's 2006.  Should I really support years < 2000?
    // time.year = year >= 90 ? year + 1900 : year + 2000;
    // time.month = month - 1;
    // time.monthDay = day;
    // time.hour = hour;
    // time.minute = minute;
    // time.second = second;

    // // Timezone offset is in quarter hours.
    // return time.toMillis(true) - (timezoneOffset * 15 * 60 * 1000);
    return NOERROR;
}

ECode CSmsMessage::PduParser::ConstructUserData(
    /* [in] */ Boolean hasUserDataHeader,
    /* [in] */ Boolean dataInSeptets,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // int offset = mCur;
    // int userDataLength = mPdu[offset++] & 0xff;
    // int headerSeptets = 0;
    // int userDataHeaderLength = 0;

    // if (hasUserDataHeader) {
    //     userDataHeaderLength = mPdu[offset++] & 0xff;

    //     byte[] udh = new byte[userDataHeaderLength];
    //     System.arraycopy(mPdu, offset, udh, 0, userDataHeaderLength);
    //     mUserDataHeader = SmsHeader.fromByteArray(udh);
    //     offset += userDataHeaderLength;

    //     int headerBits = (userDataHeaderLength + 1) * 8;
    //     headerSeptets = headerBits / 7;
    //     headerSeptets += (headerBits % 7) > 0 ? 1 : 0;
    //     mUserDataSeptetPadding = (headerSeptets * 7) - headerBits;
    // }

    // int bufferLen;
    // if (dataInSeptets) {

    //      * Here we just create the user data length to be the remainder of
    //      * the pdu minus the user data header, since userDataLength means
    //      * the number of uncompressed septets.

    //     bufferLen = mPdu.length - offset;
    // } else {
    //     /*
    //      * userDataLength is the count of octets, so just subtract the
    //      * user data header.
    //      */
    //     bufferLen = userDataLength - (hasUserDataHeader ? (userDataHeaderLength + 1) : 0);
    //     if (bufferLen < 0) {
    //         bufferLen = 0;
    //     }
    // }

    // mUserData = new byte[bufferLen];
    // System.arraycopy(mPdu, offset, mUserData, 0, mUserData.length);
    // mCur = offset;

    // if (dataInSeptets) {
    //     // Return the number of septets
    //     int count = userDataLength - headerSeptets;
    //     // If count < 0, return 0 (means UDL was probably incorrect)
    //     return count < 0 ? 0 : count;
    // } else {
    //     // Return the number of octets
    //     return mUserData.length;
    // }
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUserData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetUserDataHeader(
    /* [out] */ ISmsHeader** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUserDataHeader;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetUserDataGSM7Bit(
    /* [in] */ Int32 septetCount,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    GsmAlphabet::Gsm7BitPackedToString(mPdu, mCur, septetCount,
            mUserDataSeptetPadding, languageTable, languageShiftTable, result);

    mCur += (septetCount * 7) / 8;
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetUserDataGSM8bit(
    /* [in] */ Int32 byteCount,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    GsmAlphabet::Gsm8BitUnpackedToString(mPdu, mCur, byteCount, result);

    mCur += byteCount;
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetUserDataUCS2(
    /* [in] */ Int32 byteCount,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String ret;

    // try {
    *result = String(*mPdu, mCur, byteCount); // new String(mPdu, mCur, byteCount, "utf-16");
    // } catch (UnsupportedEncodingException ex) {
    //     ret = "";
    //     Rlog.e(TAG, "implausible UnsupportedEncodingException", ex);
    // }

    mCur += byteCount;
    return NOERROR;
}

ECode CSmsMessage::PduParser::GetUserDataKSC5601(
    /* [in] */ Int32 byteCount,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    *result = String(*mPdu, mCur, byteCount); // new String(mPdu, mCur, byteCount, "KSC5601");
    // } catch (UnsupportedEncodingException ex) {
    //     ret = "";
    //     Rlog.e(TAG, "implausible UnsupportedEncodingException", ex);
    // }

    mCur += byteCount;
    return NOERROR;
}

ECode CSmsMessage::PduParser::MoreDataPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mPdu->GetLength() > mCur);
    return NOERROR;
}

//==============================================================================
//  CSmsMessage
//==============================================================================

CAR_INTERFACE_IMPL(CSmsMessage, SmsMessageBase, ISmsMessage)

CAR_OBJECT_IMPL(CSmsMessage)

CSmsMessage::CSmsMessage()
    : mMessageClass(MessageClass_UNKNOWN)
    , mMti(0)
    , mProtocolIdentifier(0)
    , mDataCodingScheme(0)
    , mReplyPathPresent(FALSE)
    , mStatus(0)
    , mIsStatusReportMessage(FALSE)
    , mVoiceMailCount(0)
{
}

ECode CSmsMessage::constructor()
{
    return NOERROR;
}

ECode CSmsMessage::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<ISmsMessage> msg;
    CSmsMessage::New((ISmsMessage**)&msg);
    ((CSmsMessage*)msg.Get())->ParsePdu(pdu);
    *result = msg;
    REFCOUNT_ADD(*result)
    // } catch (RuntimeException ex) {
    //     Rlog.e(TAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // } catch (OutOfMemoryError e) {
    //     Rlog.e(TAG, "SMS PDU parsing failed with out of memory: ", e);
    //     return NULL;
    // }
    return NOERROR;
}

ECode CSmsMessage::IsTypeZero(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mProtocolIdentifier == 0x40);
    return NOERROR;
}

ECode CSmsMessage::NewFromCMT(
    /* [in] */ ArrayOf<String>* lines,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // try {
    //     SmsMessage msg = new SmsMessage();
    //     msg.parsePdu(IccUtils.hexStringToBytes(lines[1]));
    //     return msg;
    // } catch (RuntimeException ex) {
    //     Rlog.e(TAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // }
    return NOERROR;
}

ECode CSmsMessage::NewFromCDS(
    /* [in] */ const String& line,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // try {
    //     SmsMessage msg = new SmsMessage();
    //     msg.parsePdu(IccUtils.hexStringToBytes(line));
    //     return msg;
    // } catch (RuntimeException ex) {
    //     Rlog.e(TAG, "CDS SMS PDU parsing failed: ", ex);
    //     return NULL;
    // }
    return NOERROR;
}

ECode CSmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // try {
    //     SmsMessage msg = new SmsMessage();

    //     msg.mIndexOnIcc = index;

    //     // First byte is status: RECEIVED_READ, RECEIVED_UNREAD, STORED_SENT,
    //     // or STORED_UNSENT
    //     // See TS 51.011 10.5.3
    //     if ((data[0] & 1) == 0) {
    //         Rlog.w(TAG,
    //                 "SMS parsing failed: Trying to parse a free record");
    //         return NULL;
    //     } else {
    //         msg.mStatusOnIcc = data[0] & 0x07;
    //     }

    //     int size = data.length - 1;

    //     // Note: Data may include trailing FF's.  That's OK; message
    //     // should still parse correctly.
    //     byte[] pdu = new byte[size];
    //     System.arraycopy(data, 1, pdu, 0, size);
    //     msg.parsePdu(pdu);
    //     return msg;
    // } catch (RuntimeException ex) {
    //     Rlog.e(TAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // }
    return NOERROR;
}

ECode CSmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // int len = pdu.length() / 2;
    // int smscLen = Integer.parseInt(pdu.substring(0, 2), 16);

    // return len - smscLen - 1;
    return NOERROR;
}

ECode CSmsMessage::GetRelativeValidityPeriod(
    /* [in] */ Int32 validityPeriod,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // int relValidityPeriod = INVALID_VALIDITY_PERIOD;

    // if (validityPeriod < VALIDITY_PERIOD_MIN  || validityPeriod > VALIDITY_PERIOD_MAX) {
    //     Rlog.e(TAG,"Invalid Validity Period" + validityPeriod);
    //     return relValidityPeriod;
    // }

    // if (validityPeriod <= 720) {
    //     relValidityPeriod = (validityPeriod  / 5) - 1;
    // } else if (validityPeriod <= 1440) {
    //     relValidityPeriod = ((validityPeriod - 720) / 30) + 143;
    // } else if (validityPeriod <= 43200) {
    //     relValidityPeriod = (validityPeriod  / 1440) + 166;
    // } else if (validityPeriod <= 635040) {
    //     relValidityPeriod = (validityPeriod  / 10080) + 192;
    // }
    // return relValidityPeriod;
    return NOERROR;
}

ECode CSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested, header,
            ISmsConstants::ENCODING_UNKNOWN, 0, 0, result);
}

ECode CSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [in] */ Int32 encoding,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested,
        header, encoding, languageTable, languageShiftTable, -1, result);
}

ECode CSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [in] */ Int32 encoding,
    /* [in] */ Int32 languageTable,
    /* [in] */ Int32 languageShiftTable,
    /* [in] */ Int32 validityPeriod,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // // Perform NULL parameter checks.
    // if (message == NULL || destinationAddress == NULL) {
    //     return NULL;
    // }

    // if (encoding == ENCODING_UNKNOWN) {
    //     // Find the best encoding to use
    //     TextEncodingDetails ted = calculateLength(message, false);
    //     encoding = ted.codeUnitSize;
    //     languageTable = ted.languageTable;
    //     languageShiftTable = ted.languageShiftTable;

    //     if (encoding == ENCODING_7BIT &&
    //             (languageTable != 0 || languageShiftTable != 0)) {
    //         if (header != NULL) {
    //             SmsHeader smsHeader = SmsHeader.fromByteArray(header);
    //             if (smsHeader.languageTable != languageTable
    //                     || smsHeader.languageShiftTable != languageShiftTable) {
    //                 Rlog.w(TAG, "Updating language table in SMS header: "
    //                         + smsHeader.languageTable + " -> " + languageTable + ", "
    //                         + smsHeader.languageShiftTable + " -> " + languageShiftTable);
    //                 smsHeader.languageTable = languageTable;
    //                 smsHeader.languageShiftTable = languageShiftTable;
    //                 header = SmsHeader.toByteArray(smsHeader);
    //             }
    //         } else {
    //             SmsHeader smsHeader = new SmsHeader();
    //             smsHeader.languageTable = languageTable;
    //             smsHeader.languageShiftTable = languageShiftTable;
    //             header = SmsHeader.toByteArray(smsHeader);
    //         }
    //     }
    // }

    // SubmitPdu ret = new SubmitPdu();

    // int validityPeriodFormat = VALIDITY_PERIOD_FORMAT_NONE;
    // int relativeValidityPeriod = INVALID_VALIDITY_PERIOD;

    // // TP-Validity-Period-Format (TP-VPF) in 3GPP TS 23.040 V6.8.1 section 9.2.3.3
    // //bit 4:3 = 10 - TP-VP field present - relative format
    // if((relativeValidityPeriod = getRelativeValidityPeriod(validityPeriod)) >= 0) {
    //     validityPeriodFormat = VALIDITY_PERIOD_FORMAT_RELATIVE;
    // }

    // byte mtiByte = (byte)(0x01 | (validityPeriodFormat << 0x03) |
    //         (header != NULL ? 0x40 : 0x00));

    // ByteArrayOutputStream bo = getSubmitPduHead(
    //         scAddress, destinationAddress, mtiByte,
    //         statusReportRequested, ret);

    // // User Data (and length)
    // byte[] userData;
    // try {
    //     if (encoding == ENCODING_7BIT) {
    //         userData = GsmAlphabet.stringToGsm7BitPackedWithHeader(message, header,
    //                 languageTable, languageShiftTable);
    //     } else { //assume UCS-2
    //         try {
    //             userData = encodeUCS2(message, header);
    //         } catch(UnsupportedEncodingException uex) {
    //             Rlog.e(TAG,
    //                     "Implausible UnsupportedEncodingException ",
    //                     uex);
    //             return NULL;
    //         }
    //     }
    // } catch (EncodeException ex) {
    //     // Encoding to the 7-bit alphabet failed. Let's see if we can
    //     // send it as a UCS-2 encoded message
    //     try {
    //         userData = encodeUCS2(message, header);
    //         encoding = ENCODING_16BIT;
    //     } catch(UnsupportedEncodingException uex) {
    //         Rlog.e(TAG,
    //                 "Implausible UnsupportedEncodingException ",
    //                 uex);
    //         return NULL;
    //     }
    // }

    // if (encoding == ENCODING_7BIT) {
    //     if ((0xff & userData[0]) > MAX_USER_DATA_SEPTETS) {
    //         // Message too long
    //         Rlog.e(TAG, "Message too long (" + (0xff & userData[0]) + " septets)");
    //         return NULL;
    //     }
    //     // TP-Data-Coding-Scheme
    //     // Default encoding, uncompressed
    //     // To test writing messages to the SIM card, change this value 0x00
    //     // to 0x12, which means "bits 1 and 0 contain message class, and the
    //     // class is 2". Note that this takes effect for the sender. In other
    //     // words, messages sent by the phone with this change will end up on
    //     // the receiver's SIM card. You can then send messages to yourself
    //     // (on a phone with this change) and they'll end up on the SIM card.
    //     bo.write(0x00);
    // } else { // assume UCS-2
    //     if ((0xff & userData[0]) > MAX_USER_DATA_BYTES) {
    //         // Message too long
    //         Rlog.e(TAG, "Message too long (" + (0xff & userData[0]) + " bytes)");
    //         return NULL;
    //     }
    //     // TP-Data-Coding-Scheme
    //     // UCS-2 encoding, uncompressed
    //     bo.write(0x08);
    // }

    // if (validityPeriodFormat == VALIDITY_PERIOD_FORMAT_RELATIVE) {
    //     // ( TP-Validity-Period - relative format)
    //     bo.write(relativeValidityPeriod);
    // }

    // bo.write(userData, 0, userData.length);
    // ret.encodedMessage = bo.toByteArray();
    // return ret;
    return NOERROR;
}

ECode CSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return GetSubmitPdu(scAddress, destinationAddress, message,
            statusReportRequested, NULL, result);
}

ECode CSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int32 validityPeriod,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return GetSubmitPdu(scAddress, destinationAddress, message,
            statusReportRequested, NULL, ISmsConstants::ENCODING_UNKNOWN,
            0, 0, validityPeriod, result);
}

ECode CSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int32 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    return GetSubmitPdu(scAddress, destinationAddress, destinationPort, 0, data,
            statusReportRequested, result);
}

ECode CSmsMessage::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int32 destinationPort,
    /* [in] */ Int32 originatorPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // SmsHeader.PortAddrs portAddrs = new SmsHeader.PortAddrs();
    // portAddrs.destPort = destinationPort;
    // portAddrs.origPort = originatorPort;
    // portAddrs.areEightBits = false;

    // SmsHeader smsHeader = new SmsHeader();
    // smsHeader.portAddrs = portAddrs;

    // byte[] smsHeaderData = SmsHeader.toByteArray(smsHeader);

    // if ((data.length + smsHeaderData.length + 1) > MAX_USER_DATA_BYTES) {
    //     Rlog.e(TAG, "SMS data message may only contain "
    //             + (MAX_USER_DATA_BYTES - smsHeaderData.length - 1) + " bytes");
    //     return NULL;
    // }

    // SubmitPdu ret = new SubmitPdu();
    // ByteArrayOutputStream bo = getSubmitPduHead(
    //         scAddress, destinationAddress, (byte) 0x41, // MTI = SMS-SUBMIT,
    //                                                     // TP-UDHI = true
    //         statusReportRequested, ret);

    // // TP-Data-Coding-Scheme
    // // No class, 8 bit data
    // bo.write(0x04);

    // // (no TP-Validity-Period)

    // // Total size
    // bo.write(data.length + smsHeaderData.length + 1);

    // // User data header
    // bo.write(smsHeaderData.length);
    // bo.write(smsHeaderData, 0, smsHeaderData.length);

    // // User data
    // bo.write(data, 0, data.length);

    // ret.encodedMessage = bo.toByteArray();
    // return ret;
    return NOERROR;
}

ECode CSmsMessage::CalculateLength(
    /* [in] */ ICharSequence* msgBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // CharSequence newMsgBody = NULL;
    // Resources r = Resources.getSystem();
    // if (r.getBoolean(com.android.internal.R.bool.config_sms_force_7bit_encoding)) {
    //     newMsgBody  = Sms7BitEncodingTranslator.translate(msgBody);
    // }
    // if (TextUtils.isEmpty(newMsgBody)) {
    //     newMsgBody = msgBody;
    // }
    // TextEncodingDetails ted = GsmAlphabet.countGsmSeptets(newMsgBody, use7bitOnly);
    // if (ted == NULL) {
    //     ted = new TextEncodingDetails();
    //     int octets = newMsgBody.length() * 2;
    //     ted.codeUnitCount = newMsgBody.length();
    //     if (octets > MAX_USER_DATA_BYTES) {
    //         // If EMS is not supported, break down EMS into single segment SMS
    //         // and add page info " x/y".
    //         // In the case of UCS2 encoding type, we need 8 bytes for this
    //         // but we only have 6 bytes from UDH, so truncate the limit for
    //         // each segment by 2 bytes (1 char).
    //         int max_user_data_bytes_with_header = MAX_USER_DATA_BYTES_WITH_HEADER;
    //         if (!android.telephony.SmsMessage.hasEmsSupport()) {
    //             // make sure total number of segments is less than 10
    //             if (octets <= 9 * (max_user_data_bytes_with_header - 2))
    //                 max_user_data_bytes_with_header -= 2;
    //         }

    //         ted.msgCount = (octets + (max_user_data_bytes_with_header - 1)) /
    //                 max_user_data_bytes_with_header;
    //         ted.codeUnitsRemaining = ((ted.msgCount *
    //                 max_user_data_bytes_with_header) - octets) / 2;
    //     } else {
    //         ted.msgCount = 1;
    //         ted.codeUnitsRemaining = (MAX_USER_DATA_BYTES - octets)/2;
    //     }
    //     ted.codeUnitSize = ENCODING_16BIT;
    // }
    // return ted;
    return NOERROR;
}

ECode CSmsMessage::GetProtocolIdentifier(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mProtocolIdentifier;
    return NOERROR;
}

ECode CSmsMessage::GetDataCodingScheme(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDataCodingScheme;
    return NOERROR;
}

ECode CSmsMessage::IsReplace(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mProtocolIdentifier & 0xc0) == 0x40
            && (mProtocolIdentifier & 0x3f) > 0
            && (mProtocolIdentifier & 0x3f) < 8;
    return NOERROR;
}

ECode CSmsMessage::IsCphsMwiMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // return ((GsmSmsAddress) mOriginatingAddress).isCphsVoiceMessageClear()
    //         || ((GsmSmsAddress) mOriginatingAddress).isCphsVoiceMessageSet();
    return NOERROR;
}

ECode CSmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // if (mIsMwi && !mMwiSense) {
    //     return true;
    // }

    // return mOriginatingAddress != NULL
    //         && ((GsmSmsAddress) mOriginatingAddress).isCphsVoiceMessageClear();
    return NOERROR;
}

ECode CSmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // if (mIsMwi && mMwiSense) {
    //     return true;
    // }

    // return mOriginatingAddress != NULL
    //         && ((GsmSmsAddress) mOriginatingAddress).isCphsVoiceMessageSet();
    return NOERROR;
}

ECode CSmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // if (mIsMwi && mMwiDontStore) {
    //     return true;
    // }

    // if (isCphsMwiMessage()) {
    //     // See CPHS 4.2 Section B.4.2.1
    //     // If the user data is a single space char, do not store
    //     // the message. Otherwise, store and display as usual
    //     if (" ".equals(getMessageBody())) {
    //         return true;
    //     }
    // }

    // return false;
    return NOERROR;
}

ECode CSmsMessage::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mStatus;
    return NOERROR;
}

ECode CSmsMessage::IsStatusReportMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsStatusReportMessage;
    return NOERROR;
}

ECode CSmsMessage::IsReplyPathPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReplyPathPresent;
    return NOERROR;
}

ECode CSmsMessage::GetMessageClass(
    /* [out] */ Elastos::Droid::Internal::Telephony::MessageClass* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMessageClass;
    return NOERROR;
}

ECode CSmsMessage::IsUsimDataDownload(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMessageClass == Elastos::Droid::Internal::Telephony::MessageClass_CLASS_2 &&
            (mProtocolIdentifier == 0x7f || mProtocolIdentifier == 0x7c);
    return NOERROR;
}

ECode CSmsMessage::GetNumOfVoicemails(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * Order of priority if multiple indications are present is 1.UDH,
     *      2.DCS, 3.CPHS.
     * Voice mail count if voice mail present indication is
     * received
     *  1. UDH (or both UDH & DCS): mVoiceMailCount = 0 to 0xff. Ref[TS 23. 040]
     *  2. DCS only: count is unknown mVoiceMailCount= -1
     *  3. CPHS only: count is unknown mVoiceMailCount = 0xff. Ref[GSM-BTR-1-4700]
     * Voice mail clear, mVoiceMailCount = 0.
     */

    assert(0);
    // if ((!mIsMwi) && isCphsMwiMessage()) {
    //     if (mOriginatingAddress != NULL
    //             && ((GsmSmsAddress) mOriginatingAddress).isCphsVoiceMessageSet()) {
    //         mVoiceMailCount = 0xff;
    //     } else {
    //         mVoiceMailCount = 0;
    //     }
    //     Rlog.v(TAG, "CPHS voice mail message");
    // }
    // return mVoiceMailCount;
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > CSmsMessage::EncodeUCS2(
    /* [in] */ const String& message,
    /* [in] */ ArrayOf<Byte>* header)
{
    assert(0);
    // byte[] userData, textPart;
    // textPart = message.getBytes("utf-16be");

    // if (header != NULL) {
    //     // Need 1 byte for UDHL
    //     userData = new byte[header.length + textPart.length + 1];

    //     userData[0] = (byte)header.length;
    //     System.arraycopy(header, 0, userData, 1, header.length);
    //     System.arraycopy(textPart, 0, userData, header.length + 1, textPart.length);
    // }
    // else {
    //     userData = textPart;
    // }
    // byte[] ret = new byte[userData.length+1];
    // ret[0] = (byte) (userData.length & 0xff );
    // System.arraycopy(userData, 0, ret, 1, userData.length);
    // return ret;
    return NULL;
}

AutoPtr<IByteArrayOutputStream> CSmsMessage::GetSubmitPduHead(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Byte mtiByte,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ISmsMessageSubmitPdu* ret)
{
    assert(0);
    // ByteArrayOutputStream bo = new ByteArrayOutputStream(
    //         MAX_USER_DATA_BYTES + 40);

    // // SMSC address with length octet, or 0
    // if (scAddress == NULL) {
    //     ret.encodedScAddress = NULL;
    // } else {
    //     ret.encodedScAddress = PhoneNumberUtils.networkPortionToCalledPartyBCDWithLength(
    //             scAddress);
    // }

    // // TP-Message-Type-Indicator (and friends)
    // if (statusReportRequested) {
    //     // Set TP-Status-Report-Request bit.
    //     mtiByte |= 0x20;
    //     if (VDBG) Rlog.d(TAG, "SMS status report requested");
    // }
    // bo.write(mtiByte);

    // // space for TP-Message-Reference
    // bo.write(0);

    // byte[] daBytes;

    // daBytes = PhoneNumberUtils.networkPortionToCalledPartyBCD(destinationAddress);

    // // destination address length in BCD digits, ignoring TON byte and pad
    // // TODO Should be better.
    // bo.write((daBytes.length - 1) * 2
    //         - ((daBytes[daBytes.length - 1] & 0xf0) == 0xf0 ? 1 : 0));

    // // destination address
    // bo.write(daBytes, 0, daBytes.length);

    // // TP-Protocol-Identifier
    // bo.write(0);
    // return bo;
    return NULL;
}

void CSmsMessage::ParsePdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    assert(0);
    // mPdu = pdu;
    // // Rlog.d(TAG, "raw sms message:");
    // // Rlog.d(TAG, s);

    // PduParser p = new PduParser(pdu);

    // mScAddress = p.getSCAddress();

    // if (mScAddress != NULL) {
    //     if (VDBG) Rlog.d(TAG, "SMS SC address: " + mScAddress);
    // }

    // // TODO(mkf) support reply path, user data header indicator

    // // TP-Message-Type-Indicator
    // // 9.2.3
    // int firstByte = p.getByte();

    // mMti = firstByte & 0x3;
    // switch (mMti) {
    // // TP-Message-Type-Indicator
    // // 9.2.3
    // case 0:
    // case 3: //GSM 03.40 9.2.3.1: MTI == 3 is Reserved.
    //         //This should be processed in the same way as MTI == 0 (Deliver)
    //     parseSmsDeliver(p, firstByte);
    //     break;
    // case 1:
    //     parseSmsSubmit(p, firstByte);
    //     break;
    // case 2:
    //     parseSmsStatusReport(p, firstByte);
    //     break;
    // default:
    //     // TODO(mkf) the rest of these
    //     throw new RuntimeException("Unsupported message type");
    // }
}

void CSmsMessage::ParseSmsStatusReport(
    /* [in] */ PduParser* p,
    /* [in] */ Int32 firstByte)
{
    assert(0);
    // mIsStatusReportMessage = true;

    // // TP-Message-Reference
    // mMessageRef = p.getByte();
    // // TP-Recipient-Address
    // mRecipientAddress = p.getAddress();
    // // TP-Service-Centre-Time-Stamp
    // mScTimeMillis = p.getSCTimestampMillis();
    // p.getSCTimestampMillis();
    // // TP-Status
    // mStatus = p.getByte();

    // // The following are optional fields that may or may not be present.
    // if (p.moreDataPresent()) {
    //     // TP-Parameter-Indicator
    //     int extraParams = p.getByte();
    //     int moreExtraParams = extraParams;
    //     while ((moreExtraParams & 0x80) != 0) {
    //         // We only know how to parse a few extra parameters, all
    //         // indicated in the first TP-PI octet, so skip over any
    //         // additional TP-PI octets.
    //         moreExtraParams = p.getByte();
    //     }
    //     // As per 3GPP 23.040 section 9.2.3.27 TP-Parameter-Indicator,
    //     // only process the byte if the reserved bits (bits3 to 6) are zero.
    //     if ((extraParams & 0x78) == 0) {
    //         // TP-Protocol-Identifier
    //         if ((extraParams & 0x01) != 0) {
    //             mProtocolIdentifier = p.getByte();
    //         }
    //         // TP-Data-Coding-Scheme
    //         if ((extraParams & 0x02) != 0) {
    //             mDataCodingScheme = p.getByte();
    //         }
    //         // TP-User-Data-Length (implies existence of TP-User-Data)
    //         if ((extraParams & 0x04) != 0) {
    //             boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;
    //             parseUserData(p, hasUserDataHeader);
    //         }
    //     }
    // }
}

void CSmsMessage::ParseSmsDeliver(
    /* [in] */ PduParser* p,
    /* [in] */ Int32 firstByte)
{
    assert(0);
    // mReplyPathPresent = (firstByte & 0x80) == 0x80;

    // mOriginatingAddress = p.getAddress();

    // if (mOriginatingAddress != NULL) {
    //     if (VDBG) Rlog.v(TAG, "SMS originating address: "
    //             + mOriginatingAddress.address);
    // }

    // // TP-Protocol-Identifier (TP-PID)
    // // TS 23.040 9.2.3.9
    // mProtocolIdentifier = p.getByte();

    // // TP-Data-Coding-Scheme
    // // see TS 23.038
    // mDataCodingScheme = p.getByte();

    // if (VDBG) {
    //     Rlog.v(TAG, "SMS TP-PID:" + mProtocolIdentifier
    //             + " data coding scheme: " + mDataCodingScheme);
    // }

    // mScTimeMillis = p.getSCTimestampMillis();

    // if (VDBG) Rlog.d(TAG, "SMS SC timestamp: " + mScTimeMillis);

    // boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;

    // parseUserData(p, hasUserDataHeader);
}

void CSmsMessage::ParseSmsSubmit(
    /* [in] */ PduParser* p,
    /* [in] */ Int32 firstByte)
{
    assert(0);
    // mReplyPathPresent = (firstByte & 0x80) == 0x80;

    // // TP-MR (TP-Message Reference)
    // mMessageRef = p.getByte();

    // mRecipientAddress = p.getAddress();

    // if (mRecipientAddress != NULL) {
    //     if (VDBG) Rlog.v(TAG, "SMS recipient address: " + mRecipientAddress.address);
    // }

    // // TP-Protocol-Identifier (TP-PID)
    // // TS 23.040 9.2.3.9
    // mProtocolIdentifier = p.getByte();

    // // TP-Data-Coding-Scheme
    // // see TS 23.038
    // mDataCodingScheme = p.getByte();

    // if (VDBG) {
    //     Rlog.v(TAG, "SMS TP-PID:" + mProtocolIdentifier
    //             + " data coding scheme: " + mDataCodingScheme);
    // }

    // // TP-Validity-Period-Format
    // int validityPeriodLength = 0;
    // int validityPeriodFormat = ((firstByte>>3) & 0x3);
    // if (0x0 == validityPeriodFormat) /* 00, TP-VP field not present*/
    // {
    //     validityPeriodLength = 0;
    // }
    // else if (0x2 == validityPeriodFormat) /* 10, TP-VP: relative format*/
    // {
    //     validityPeriodLength = 1;
    // }
    // else /* other case, 11 or 01, TP-VP: absolute or enhanced format*/
    // {
    //     validityPeriodLength = 7;
    // }

    // // TP-Validity-Period is not used on phone, so just ignore it for now.
    // while (validityPeriodLength-- > 0)
    // {
    //     p.getByte();
    // }

    // boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;

    // parseUserData(p, hasUserDataHeader);
}

void CSmsMessage::ParseUserData(
    /* [in] */ PduParser* p,
    /* [in] */ Boolean hasUserDataHeader)
{
    assert(0);
//    boolean hasMessageClass = false;
//    boolean userDataCompressed = false;
//
//    int encodingType = ENCODING_UNKNOWN;
//
//    // Look up the data encoding scheme
//    if ((mDataCodingScheme & 0x80) == 0) {
//        userDataCompressed = (0 != (mDataCodingScheme & 0x20));
//        hasMessageClass = (0 != (mDataCodingScheme & 0x10));
//
//        if (userDataCompressed) {
//            Rlog.w(TAG, "4 - Unsupported SMS data coding scheme "
//                    + "(compression) " + (mDataCodingScheme & 0xff));
//        } else {
//            switch ((mDataCodingScheme >> 2) & 0x3) {
//            case 0: // GSM 7 bit default alphabet
//                encodingType = ENCODING_7BIT;
//                break;
//
//            case 2: // UCS 2 (16bit)
//                encodingType = ENCODING_16BIT;
//                break;
//
//            case 1: // 8 bit data
//                //Support decoding the user data payload as pack GSM 8-bit (a GSM alphabet string
//                //that's stored in 8-bit unpacked format) characters.
//                Resources r = Resources.getSystem();
//                if (r.getBoolean(com.android.internal.
//                        R.bool.config_sms_decode_gsm_8bit_data)) {
//                    encodingType = ENCODING_8BIT;
//                    break;
//                }
//
//            case 3: // reserved
//                Rlog.w(TAG, "1 - Unsupported SMS data coding scheme "
//                        + (mDataCodingScheme & 0xff));
//                encodingType = ENCODING_8BIT;
//                break;
//            }
//        }
//    } else if ((mDataCodingScheme & 0xf0) == 0xf0) {
//        hasMessageClass = true;
//        userDataCompressed = false;
//
//        if (0 == (mDataCodingScheme & 0x04)) {
//            // GSM 7 bit default alphabet
//            encodingType = ENCODING_7BIT;
//        } else {
//            // 8 bit data
//            encodingType = ENCODING_8BIT;
//        }
//    } else if ((mDataCodingScheme & 0xF0) == 0xC0
//            || (mDataCodingScheme & 0xF0) == 0xD0
//            || (mDataCodingScheme & 0xF0) == 0xE0) {
//        // 3GPP TS 23.038 V7.0.0 (2006-03) section 4
//
//        // 0xC0 == 7 bit, don't store
//        // 0xD0 == 7 bit, store
//        // 0xE0 == UCS-2, store
//
//        if ((mDataCodingScheme & 0xF0) == 0xE0) {
//            encodingType = ENCODING_16BIT;
//        } else {
//            encodingType = ENCODING_7BIT;
//        }
//
//        userDataCompressed = false;
//        boolean active = ((mDataCodingScheme & 0x08) == 0x08);
//        // bit 0x04 reserved
//
//        // VM - If TP-UDH is present, these values will be overwritten
//        if ((mDataCodingScheme & 0x03) == 0x00) {
//            mIsMwi = true; /* Indicates vmail */
//            mMwiSense = active;/* Indicates vmail notification set/clear */
//            mMwiDontStore = ((mDataCodingScheme & 0xF0) == 0xC0);
//
//            /* Set voice mail count based on notification bit */
//            if (active == true) {
//                mVoiceMailCount = -1; // unknown number of messages waiting
//            } else {
//                mVoiceMailCount = 0; // no unread messages
//            }
//
//            Rlog.w(TAG, "MWI in DCS for Vmail. DCS = "
//                    + (mDataCodingScheme & 0xff) + " Dont store = "
//                    + mMwiDontStore + " vmail count = " + mVoiceMailCount);
//
//        } else {
//            mIsMwi = false;
//            Rlog.w(TAG, "MWI in DCS for fax/email/other: "
//                    + (mDataCodingScheme & 0xff));
//        }
//    } else if ((mDataCodingScheme & 0xC0) == 0x80) {
//        // 3GPP TS 23.038 V7.0.0 (2006-03) section 4
//        // 0x80..0xBF == Reserved coding groups
//        if (mDataCodingScheme == 0x84) {
//            // This value used for KSC5601 by carriers in Korea.
//            encodingType = ENCODING_KSC5601;
//        } else {
//            Rlog.w(TAG, "5 - Unsupported SMS data coding scheme "
//                    + (mDataCodingScheme & 0xff));
//        }
//    } else {
//        Rlog.w(TAG, "3 - Unsupported SMS data coding scheme "
//                + (mDataCodingScheme & 0xff));
//    }
//
//    // set both the user data and the user data header.
//    int count = p.constructUserData(hasUserDataHeader,
//            encodingType == ENCODING_7BIT);
//    this.mUserData = p.getUserData();
//    this.mUserDataHeader = p.getUserDataHeader();
//
//    /*
//     * Look for voice mail indication in TP_UDH TS23.040 9.2.3.24
//     * ieid = 1 (0x1) (SPECIAL_SMS_MSG_IND)
//     * ieidl =2 octets
//     * ieda msg_ind_type = 0x00 (voice mail; discard sms )or
//     *                   = 0x80 (voice mail; store sms)
//     * msg_count = 0x00 ..0xFF
//     */
//    if (hasUserDataHeader && (mUserDataHeader.specialSmsMsgList.size() != 0)) {
//        for (SmsHeader.SpecialSmsMsg msg : mUserDataHeader.specialSmsMsgList) {
//            int msgInd = msg.msgIndType & 0xff;
//            /*
//             * TS 23.040 V6.8.1 Sec 9.2.3.24.2
//             * bits 1 0 : basic message indication type
//             * bits 4 3 2 : extended message indication type
//             * bits 6 5 : Profile id bit 7 storage type
//             */
//            if ((msgInd == 0) || (msgInd == 0x80)) {
//                mIsMwi = true;
//                if (msgInd == 0x80) {
//                    /* Store message because TP_UDH indicates so*/
//                    mMwiDontStore = false;
//                } else if (mMwiDontStore == false) {
//                    /* Storage bit is not set by TP_UDH
//                     * Check for conflict
//                     * between message storage bit in TP_UDH
//                     * & DCS. The message shall be stored if either of
//                     * the one indicates so.
//                     * TS 23.040 V6.8.1 Sec 9.2.3.24.2
//                     */
//                    if (!((((mDataCodingScheme & 0xF0) == 0xD0)
//                           || ((mDataCodingScheme & 0xF0) == 0xE0))
//                           && ((mDataCodingScheme & 0x03) == 0x00))) {
//                        /* Even DCS did not have voice mail with Storage bit
//                         * 3GPP TS 23.038 V7.0.0 section 4
//                         * So clear this flag*/
//                        mMwiDontStore = true;
//                    }
//                }
//
//                mVoiceMailCount = msg.msgCount & 0xff;
//
//                /*
//                 * In the event of a conflict between message count setting
//                 * and DCS then the Message Count in the TP-UDH shall
//                 * override the indication in the TP-DCS. Set voice mail
//                 * notification based on count in TP-UDH
//                 */
//                if (mVoiceMailCount > 0)
//                    mMwiSense = true;
//                else
//                    mMwiSense = false;
//
//                Rlog.w(TAG, "MWI in TP-UDH for Vmail. Msg Ind = " + msgInd
//                        + " Dont store = " + mMwiDontStore + " Vmail count = "
//                        + mVoiceMailCount);
//
//                /*
//                 * There can be only one IE for each type of message
//                 * indication in TP_UDH. In the event they are duplicated
//                 * last occurence will be used. Hence the for loop
//                 */
//            } else {
//                Rlog.w(TAG, "TP_UDH fax/email/"
//                        + "extended msg/multisubscriber profile. Msg Ind = " + msgInd);
//            }
//        } // end of for
//    } // end of if UDH
//
//    switch (encodingType) {
//    case ENCODING_UNKNOWN:
//        mMessageBody = NULL;
//        break;
//
//    case ENCODING_8BIT:
//        //Support decoding the user data payload as pack GSM 8-bit (a GSM alphabet string
//        //that's stored in 8-bit unpacked format) characters.
//        Resources r = Resources.getSystem();
//        if (r.getBoolean(com.android.internal.
//                R.bool.config_sms_decode_gsm_8bit_data)) {
//            mMessageBody = p.getUserDataGSM8bit(count);
//        } else {
//            mMessageBody = NULL;
//        }
//        break;
//
//    case ENCODING_7BIT:
//        mMessageBody = p.getUserDataGSM7Bit(count,
//                hasUserDataHeader ? mUserDataHeader.languageTable : 0,
//                hasUserDataHeader ? mUserDataHeader.languageShiftTable : 0);
//        break;
//
//    case ENCODING_16BIT:
//        mMessageBody = p.getUserDataUCS2(count);
//        break;
//
//    case ENCODING_KSC5601:
//        mMessageBody = p.getUserDataKSC5601(count);
//        break;
//    }
//
//    if (VDBG) Rlog.v(TAG, "SMS message body (raw): '" + mMessageBody + "'");
//
//    if (mMessageBody != NULL) {
//        parseMessageBody();
//    }
//
//    if (!hasMessageClass) {
//        messageClass = MessageClass.UNKNOWN;
//    } else {
//        switch (mDataCodingScheme & 0x3) {
//        case 0:
//            messageClass = MessageClass.CLASS_0;
//            break;
//        case 1:
//            messageClass = MessageClass.CLASS_1;
//            break;
//        case 2:
//            messageClass = MessageClass.CLASS_2;
//            break;
//        case 3:
//            messageClass = MessageClass.CLASS_3;
//            break;
//        }
//    }
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos