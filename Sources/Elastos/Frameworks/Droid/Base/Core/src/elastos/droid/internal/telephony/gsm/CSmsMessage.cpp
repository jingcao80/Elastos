#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/internal/telephony/gsm/CSmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/CGsmSmsAddress.h"
// TODO: Need CSms7BitEncodingTranslator
// #include "elastos/droid/internal/telephony/CSms7BitEncodingTranslator.h"
#include "elastos/droid/internal/telephony/CSmsHeader.h"
#include "elastos/droid/internal/telephony/CGsmAlphabetTextEncodingDetails.h"
#include "elastos/droid/internal/telephony/GsmAlphabet.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/SmsHeader.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include "elastos/droid/telephony/SmsMessage.h"
#include "elastos/droid/text/format/CTime.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::CGsmAlphabetTextEncodingDetails;
// TODO: Need CSms7BitEncodingTranslator
// using Elastos::Droid::Internal::Telephony::CSms7BitEncodingTranslator;
using Elastos::Droid::Internal::Telephony::CSmsHeader;
using Elastos::Droid::Internal::Telephony::GsmAlphabet;
using Elastos::Droid::Internal::Telephony::IccUtils;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::SmsHeader;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Logging::Logger;

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

CAR_INTERFACE_IMPL(CSmsMessage::SubmitPdu, Object, ISmsMessageSubmitPdu)

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
        //     Logger::D(TAG, "invalid SC address: ", tr);
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
    // TP-Service-Centre-Time-Stamp
    Int32 year = IccUtils::GsmBcdByteToInt((*mPdu)[mCur++]);
    Int32 month = IccUtils::GsmBcdByteToInt((*mPdu)[mCur++]);
    Int32 day = IccUtils::GsmBcdByteToInt((*mPdu)[mCur++]);
    Int32 hour = IccUtils::GsmBcdByteToInt((*mPdu)[mCur++]);
    Int32 minute = IccUtils::GsmBcdByteToInt((*mPdu)[mCur++]);
    Int32 second = IccUtils::GsmBcdByteToInt((*mPdu)[mCur++]);

    // For the timezone, the most significant bit of the
    // least significant nibble is the sign byte
    // (meaning the max range of this field is 79 quarter-hours,
    // which is more than enough)

    Byte tzByte = (*mPdu)[mCur++];

    // Mask out sign bit.
    Int32 timezoneOffset = IccUtils::GsmBcdByteToInt((Byte) (tzByte & (~0x08)));

    timezoneOffset = ((tzByte & 0x08) == 0) ? timezoneOffset : -timezoneOffset;

    AutoPtr<ITime> time;
    CTime::New(ITime::TIMEZONE_UTC, (ITime**)&time);

    // It's 2006.  Should I really support years < 2000?
    time->SetYear(year >= 90 ? year + 1900 : year + 2000);
    time->SetMonth(month - 1);
    time->SetMonthDay(day);
    time->SetHour(hour);
    time->SetMinute(minute);
    time->SetSecond(second);

    // Timezone offset is in quarter hours.
    Int64 val;
    time->ToMillis(TRUE, &val);
    *result = val - (timezoneOffset * 15 * 60 * 1000);
    return NOERROR;
}

ECode CSmsMessage::PduParser::ConstructUserData(
    /* [in] */ Boolean hasUserDataHeader,
    /* [in] */ Boolean dataInSeptets,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 offset = mCur;
    Int32 userDataLength = (*mPdu)[offset++] & 0xff;
    Int32 headerSeptets = 0;
    Int32 userDataHeaderLength = 0;

    if (hasUserDataHeader) {
        userDataHeaderLength = (*mPdu)[offset++] & 0xff;

        AutoPtr<ArrayOf<Byte> > udh = ArrayOf<Byte>::Alloc(userDataHeaderLength);
        udh->Copy(0, mPdu, offset, userDataHeaderLength);
        SmsHeader::FromByteArray(udh, (ISmsHeader**)&mUserDataHeader);
        offset += userDataHeaderLength;

        Int32 headerBits = (userDataHeaderLength + 1) * 8;
        headerSeptets = headerBits / 7;
        headerSeptets += (headerBits % 7) > 0 ? 1 : 0;
        mUserDataSeptetPadding = (headerSeptets * 7) - headerBits;
    }

    int bufferLen;
    if (dataInSeptets) {
        /*
         * Here we just create the user data length to be the remainder of
         * the pdu minus the user data header, since userDataLength means
         * the number of uncompressed septets.
         */
        bufferLen = mPdu->GetLength() - offset;
    }
    else {
        /*
         * userDataLength is the count of octets, so just subtract the
         * user data header.
         */
        bufferLen = userDataLength - (hasUserDataHeader ? (userDataHeaderLength + 1) : 0);
        if (bufferLen < 0) {
            bufferLen = 0;
        }
    }

    mUserData = ArrayOf<Byte>::Alloc(bufferLen);
    mUserData->Copy(0, mPdu, offset, mUserData->GetLength());
    mCur = offset;

    if (dataInSeptets) {
        // Return the number of septets
        Int32 count = userDataLength - headerSeptets;
        // If count < 0, return 0 (means UDL was probably incorrect)
        *result = count < 0 ? 0 : count;
    }
    else {
        // Return the number of octets
        *result = mUserData->GetLength();
    }
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
    //     Logger::E(TAG, "implausible UnsupportedEncodingException", ex);
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
    //     Logger::E(TAG, "implausible UnsupportedEncodingException", ex);
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
    //     Logger::E(TAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // } catch (OutOfMemoryError e) {
    //     Logger::E(TAG, "SMS PDU parsing failed with out of memory: ", e);
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
    // try {
    AutoPtr<ISmsMessage> msg;
    CSmsMessage::New((ISmsMessage**)&msg);
    ((CSmsMessage*)msg.Get())->ParsePdu(IccUtils::HexStringToBytes((*lines)[1]));
    *result = msg;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (RuntimeException ex) {
    //     Logger::E(TAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // }
}

ECode CSmsMessage::NewFromCDS(
    /* [in] */ const String& line,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<ISmsMessage> msg;
    CSmsMessage::New((ISmsMessage**)&msg);
    ((CSmsMessage*)msg.Get())->ParsePdu(IccUtils::HexStringToBytes(line));
    *result = msg;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (RuntimeException ex) {
    //     Logger::E(TAG, "CDS SMS PDU parsing failed: ", ex);
    //     return NULL;
    // }
}

ECode CSmsMessage::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result)

    // try {
    AutoPtr<ISmsMessage> msg;
    CSmsMessage::New((ISmsMessage**)&msg);

    ((CSmsMessage*)msg.Get())->mIndexOnIcc = index;

    // First byte is status: RECEIVED_READ, RECEIVED_UNREAD, STORED_SENT,
    // or STORED_UNSENT
    // See TS 51.011 10.5.3
    if (((*data)[0] & 1) == 0) {
        Logger::W(TAG,
                "SMS parsing failed: Trying to parse a free record");
        *result = NULL;
    }
    else {
        ((CSmsMessage*)msg.Get())->mStatusOnIcc = (*data)[0] & 0x07;
    }

    Int32 size = data->GetLength() - 1;

    // Note: Data may include trailing FF's.  That's OK; message
    // should still parse correctly.
    AutoPtr<ArrayOf<Byte> > pdu = ArrayOf<Byte>::Alloc(size);
    pdu->Copy(0, data, 1, size);
    ((CSmsMessage*)msg.Get())->ParsePdu(pdu);
    *result = msg;
    REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (RuntimeException ex) {
    //     Logger::E(TAG, "SMS PDU parsing failed: ", ex);
    //     return NULL;
    // }
}

ECode CSmsMessage::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 len = pdu.GetLength() / 2;
    Int32 smscLen = StringUtils::ParseInt32(pdu.Substring(0, 2), 16);

    *result = len - smscLen - 1;
    return NOERROR;
}

ECode CSmsMessage::GetRelativeValidityPeriod(
    /* [in] */ Int32 validityPeriod,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 relValidityPeriod = INVALID_VALIDITY_PERIOD;

    if (validityPeriod < VALIDITY_PERIOD_MIN  || validityPeriod > VALIDITY_PERIOD_MAX) {
        Logger::E(TAG,"Invalid Validity Period %d", validityPeriod);
        *result = relValidityPeriod;
        return NOERROR;
    }

    if (validityPeriod <= 720) {
        relValidityPeriod = (validityPeriod  / 5) - 1;
    }
    else if (validityPeriod <= 1440) {
        relValidityPeriod = ((validityPeriod - 720) / 30) + 143;
    }
    else if (validityPeriod <= 43200) {
        relValidityPeriod = (validityPeriod  / 1440) + 166;
    }
    else if (validityPeriod <= 635040) {
        relValidityPeriod = (validityPeriod  / 10080) + 192;
    }
    *result = relValidityPeriod;
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
    // Perform NULL parameter checks.
    if (message == NULL || destinationAddress == NULL) {
        *result = NULL;
        return NOERROR;
    }

    if (encoding == ISmsConstants::ENCODING_UNKNOWN) {
        // Find the best encoding to use
        AutoPtr<IGsmAlphabetTextEncodingDetails> ted;
        CalculateLength(CoreUtils::Convert(message), FALSE, (IGsmAlphabetTextEncodingDetails**)&ted);
        encoding = ((CGsmAlphabetTextEncodingDetails*)ted.Get())->mCodeUnitSize;
        languageTable = ((CGsmAlphabetTextEncodingDetails*)ted.Get())->mLanguageTable;
        languageShiftTable = ((CGsmAlphabetTextEncodingDetails*)ted.Get())->mLanguageShiftTable;

        if (encoding == ISmsConstants::ENCODING_7BIT &&
                (languageTable != 0 || languageShiftTable != 0)) {
            if (header != NULL) {
                AutoPtr<ISmsHeader> smsHeader;
                SmsHeader::FromByteArray(header, (ISmsHeader**)&smsHeader);
// TODO: Need SmsHeader
                // if (smsHeader->mLanguageTable != languageTable
                //         || smsHeader->mLanguageShiftTable != languageShiftTable) {
                //     Logger::W(TAG, "Updating language table in SMS header: "
                //             + smsHeader->mLanguageTable + " -> " + languageTable + ", "
                //             + smsHeader->mLanguageShiftTable + " -> " + languageShiftTable);
                //     smsHeader->mLanguageTable = languageTable;
                //     smsHeader->mLanguageShiftTable = languageShiftTable;
                //     SmsHeader::ToByteArray(smsHeader, (ArrayOf<Byte>**)&header);
                // }
            }
            else {
                AutoPtr<ISmsHeader> smsHeader;
                CSmsHeader::New((ISmsHeader**)&smsHeader);
// TODO: Need SmsHeader
                // smsHeader->mLanguageTable = languageTable;
                // smsHeader->mLanguageShiftTable = languageShiftTable;
                SmsHeader::ToByteArray(smsHeader, (ArrayOf<Byte>**)&header);
            }
        }
    }

    AutoPtr<SubmitPdu> ret = new SubmitPdu();

    Int32 validityPeriodFormat = VALIDITY_PERIOD_FORMAT_NONE;
    Int32 relativeValidityPeriod = INVALID_VALIDITY_PERIOD;

    // TP-Validity-Period-Format (TP-VPF) in 3GPP TS 23.040 V6.8.1 section 9.2.3.3
    //bit 4:3 = 10 - TP-VP field present - relative format
    Int32 val;
    GetRelativeValidityPeriod(validityPeriod, &val);
    if((relativeValidityPeriod = val) >= 0) {
        validityPeriodFormat = VALIDITY_PERIOD_FORMAT_RELATIVE;
    }

    Byte mtiByte = (Byte)(0x01 | (validityPeriodFormat << 0x03) |
            (header != NULL ? 0x40 : 0x00));

    AutoPtr<IByteArrayOutputStream> bo = GetSubmitPduHead(
            scAddress, destinationAddress, mtiByte,
            statusReportRequested, ret);

    // User Data (and length)
    AutoPtr<ArrayOf<Byte> > userData;
    // try {
    if (encoding == ISmsConstants::ENCODING_7BIT) {
        GsmAlphabet::StringToGsm7BitPackedWithHeader(message, header,
                languageTable, languageShiftTable, (ArrayOf<Byte>**)&userData);
    } else { //assume UCS-2
        // try {
        userData = EncodeUCS2(message, header);
        // } catch(UnsupportedEncodingException uex) {
        //     Logger::E(TAG,
        //             "Implausible UnsupportedEncodingException ",
        //             uex);
        //     return NULL;
        // }
    }
    // } catch (EncodeException ex) {
    //     // Encoding to the 7-bit alphabet failed. Let's see if we can
    //     // send it as a UCS-2 encoded message
    //     try {
    //         userData = encodeUCS2(message, header);
    //         encoding = ISmsConstants::ENCODING_16BIT;
    //     } catch(UnsupportedEncodingException uex) {
    //         Logger::E(TAG,
    //                 "Implausible UnsupportedEncodingException ",
    //                 uex);
    //         return NULL;
    //     }
    // }

    if (encoding == ISmsConstants::ENCODING_7BIT) {
        if ((0xff & (*userData)[0]) > ISmsConstants::MAX_USER_DATA_SEPTETS) {
            // Message too long
            Logger::E(TAG, "Message too long (%d septets)", 0xff & (*userData)[0]);
            *result = NULL;
            return NOERROR;
        }
        // TP-Data-Coding-Scheme
        // Default encoding, uncompressed
        // To test writing messages to the SIM card, change this value 0x00
        // to 0x12, which means "bits 1 and 0 contain message class, and the
        // class is 2". Note that this takes effect for the sender. In other
        // words, messages sent by the phone with this change will end up on
        // the receiver's SIM card. You can then send messages to yourself
        // (on a phone with this change) and they'll end up on the SIM card.
        IOutputStream::Probe(bo)->Write(0x00);
    }
    else { // assume UCS-2
        if ((0xff & (*userData)[0]) > ISmsConstants::MAX_USER_DATA_BYTES) {
            // Message too long
            Logger::E(TAG, "Message too long (%d bytes)", 0xff & (*userData)[0]);
            *result = NULL;
            return NOERROR;
        }
        // TP-Data-Coding-Scheme
        // UCS-2 encoding, uncompressed
        IOutputStream::Probe(bo)->Write(0x08);
    }

    if (validityPeriodFormat == VALIDITY_PERIOD_FORMAT_RELATIVE) {
        // ( TP-Validity-Period - relative format)
        IOutputStream::Probe(bo)->Write(relativeValidityPeriod);
    }

    IOutputStream::Probe(bo)->Write(userData, 0, userData->GetLength());
// TODO: Need SmsMessageBase::SubmitPduBase
    // bo->ToByteArray((ArrayOf<Byte>**)&ret->mEncodedMessage);
    *result = ret;
    REFCOUNT_ADD(*result)
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
// TODO: Need SmsHeader::PortAddrs
    // AutoPtr<SmsHeaderPortAddrs> portAddrs = new SmsHeaderPortAddrs();
    // portAddrs->mDestPort = destinationPort;
    // portAddrs->mOrigPort = originatorPort;
    // portAddrs->mAreEightBits = FALSE;

    AutoPtr<ISmsHeader> smsHeader;
    CSmsHeader::New((ISmsHeader**)&smsHeader);
// TODO: Need SmsHeader::PortAddrs
    // smsHeader->mPortAddrs = portAddrs;

    AutoPtr<ArrayOf<Byte> > smsHeaderData;
    SmsHeader::ToByteArray(smsHeader, (ArrayOf<Byte>**)&smsHeaderData);

    if ((data->GetLength() + smsHeaderData->GetLength() + 1) > ISmsConstants::MAX_USER_DATA_BYTES) {
        Logger::E(TAG, "SMS data message may only contain %d bytes",
                ISmsConstants::MAX_USER_DATA_BYTES - smsHeaderData->GetLength() - 1);
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<SubmitPdu> ret = new SubmitPdu();
    AutoPtr<IByteArrayOutputStream> bo = GetSubmitPduHead(
            scAddress, destinationAddress, (Byte) 0x41, // MTI = SMS-SUBMIT,
                                                        // TP-UDHI = true
            statusReportRequested, ret);

    // TP-Data-Coding-Scheme
    // No class, 8 bit data
    IOutputStream::Probe(bo)->Write(0x04);

    // (no TP-Validity-Period)

    // Total size
    IOutputStream::Probe(bo)->Write(data->GetLength() + smsHeaderData->GetLength() + 1);

    // User data header
    IOutputStream::Probe(bo)->Write(smsHeaderData->GetLength());
    IOutputStream::Probe(bo)->Write(smsHeaderData, 0, smsHeaderData->GetLength());

    // User data
    IOutputStream::Probe(bo)->Write(data, 0, data->GetLength());

// TODO: Need SmsMessageBase::SubmitPduBase
    // bo->ToByteArray((ArrayOf<Byte>**)&ret->mEncodedMessage);
    *result = ret;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSmsMessage::CalculateLength(
    /* [in] */ ICharSequence* msgBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> newMsgBody;
    AutoPtr<IResources> r = CResources::GetSystem();
    String str;
    Boolean b;
    if (r->GetBoolean(R::bool_::config_sms_force_7bit_encoding, &b), b) {
        AutoPtr<ISms7BitEncodingTranslator> helper;
// TODO: Need CSms7BitEncodingTranslator
        // CSms7BitEncodingTranslator::AcquirSinglenton((ISms7BitEncodingTranslator**)&helper);
        helper->Translate(msgBody, &str);
    }
    newMsgBody = CoreUtils::Convert(str);
    if (str.IsEmpty()) {
        newMsgBody = msgBody;
    }
    AutoPtr<IGsmAlphabetTextEncodingDetails> ted;
    GsmAlphabet::CountGsmSeptets(newMsgBody, use7bitOnly, (IGsmAlphabetTextEncodingDetails**)&ted);
    if (ted == NULL) {
        CGsmAlphabetTextEncodingDetails::New((IGsmAlphabetTextEncodingDetails**)&ted);
        Int32 len;
        newMsgBody->GetLength(&len);
        Int32 octets = len * 2;
        ted->SetCodeUnitCount(len);
        if (octets > ISmsConstants::MAX_USER_DATA_BYTES) {
            // If EMS is not supported, break down EMS into single segment SMS
            // and add page info " x/y".
            // In the case of UCS2 encoding type, we need 8 bytes for this
            // but we only have 6 bytes from UDH, so truncate the limit for
            // each segment by 2 bytes (1 char).
            Int32 max_user_data_bytes_with_header = ISmsConstants::MAX_USER_DATA_BYTES_WITH_HEADER;
            if (!Elastos::Droid::Telephony::SmsMessage::HasEmsSupport()) {
                // make sure total number of segments is less than 10
                if (octets <= 9 * (max_user_data_bytes_with_header - 2))
                    max_user_data_bytes_with_header -= 2;
            }

            ted->SetMsgCount((octets + (max_user_data_bytes_with_header - 1)) /
                    max_user_data_bytes_with_header);
            Int32 c;
            ted->GetMsgCount(&c);
            ted->SetCodeUnitsRemaining(((c *
                    max_user_data_bytes_with_header) - octets) / 2);
        }
        else {
            ted->SetMsgCount(1);
            ted->SetCodeUnitsRemaining((ISmsConstants::MAX_USER_DATA_BYTES - octets)/2);
        }
        ted->SetCodeUnitSize(ISmsConstants::ENCODING_16BIT);
    }
    *result = ted;
    REFCOUNT_ADD(*result)
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
    Boolean b1, b2;
    IGsmSmsAddress::Probe(mOriginatingAddress)->IsCphsVoiceMessageClear(&b1);
    IGsmSmsAddress::Probe(mOriginatingAddress)->IsCphsVoiceMessageSet(&b2);
    *result = b1 || b2;
    return NOERROR;
}

ECode CSmsMessage::IsMWIClearMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mIsMwi && !mMwiSense) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean b;
    *result = mOriginatingAddress != NULL
            && (IGsmSmsAddress::Probe(mOriginatingAddress)->IsCphsVoiceMessageClear(&b), b);
    return NOERROR;
}

ECode CSmsMessage::IsMWISetMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mIsMwi && mMwiSense) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean b;
    *result = mOriginatingAddress != NULL
            && (IGsmSmsAddress::Probe(mOriginatingAddress)->IsCphsVoiceMessageSet(&b), b);
    return NOERROR;
}

ECode CSmsMessage::IsMwiDontStore(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mIsMwi && mMwiDontStore) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean b;
    if (IsCphsMwiMessage(&b), b) {
        // See CPHS 4.2 Section B.4.2.1
        // If the user data is a single space char, do not store
        // the message. Otherwise, store and display as usual
        String str;
        GetMessageBody(&str);
        if (str.Equals("")) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
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

    Boolean b;
    if ((!mIsMwi) && (IsCphsMwiMessage(&b), b)) {
        if (mOriginatingAddress != NULL
                && (IGsmSmsAddress::Probe(mOriginatingAddress)->IsCphsVoiceMessageSet(&b), b)) {
            mVoiceMailCount = 0xff;
        }
        else {
            mVoiceMailCount = 0;
        }
        Logger::V(TAG, "CPHS voice mail message");
    }
    *result = mVoiceMailCount;
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > CSmsMessage::EncodeUCS2(
    /* [in] */ const String& message,
    /* [in] */ ArrayOf<Byte>* header)
{
    AutoPtr<ArrayOf<Byte> > userData;
    AutoPtr<ArrayOf<Byte> > textPart;
    textPart = message.GetBytes();   // message->GetBytes("utf-16be");

    if (header != NULL) {
        // Need 1 byte for UDHL
        userData = ArrayOf<Byte>::Alloc(header->GetLength() + textPart->GetLength() + 1);

        (*userData)[0] = (Byte)header->GetLength();
        userData->Copy(1, header, 0, header->GetLength());
        userData->Copy(header->GetLength() + 1, textPart, 0, textPart->GetLength());
    }
    else {
        userData = textPart;
    }
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(userData->GetLength() + 1);
    (*ret)[0] = (Byte) (userData->GetLength() & 0xff );
    ret->Copy(1, userData, 0, userData->GetLength());
    return ret;
}

AutoPtr<IByteArrayOutputStream> CSmsMessage::GetSubmitPduHead(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Byte mtiByte,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ SubmitPdu* ret)
{
    AutoPtr<IByteArrayOutputStream> bo;
    CByteArrayOutputStream::New(ISmsConstants::MAX_USER_DATA_BYTES + 40, (IByteArrayOutputStream**)&bo);

    // SMSC address with length octet, or 0
    if (scAddress == NULL) {
// TODO: Need SmsMessageBase::SubmitPduBase
        // ret->mEncodedScAddress = NULL;
    }
    else {
// TODO: Need SmsMessageBase::SubmitPduBase
        // PhoneNumberUtils::NetworkPortionToCalledPartyBCDWithLength(
        //         scAddress, &ret->mEncodedScAddress);
    }

    // TP-Message-Type-Indicator (and friends)
    if (statusReportRequested) {
        // Set TP-Status-Report-Request bit.
        mtiByte |= 0x20;
        if (VDBG) Logger::D(TAG, "SMS status report requested");
    }
    IOutputStream::Probe(bo)->Write(mtiByte);

    // space for TP-Message-Reference
    IOutputStream::Probe(bo)->Write(0);

    AutoPtr<ArrayOf<Byte> > daBytes;

    PhoneNumberUtils::NetworkPortionToCalledPartyBCD(destinationAddress, (ArrayOf<Byte>**)&daBytes);

    // destination address length in BCD digits, ignoring TON byte and pad
    // TODO Should be better.
    IOutputStream::Probe(bo)->Write((daBytes->GetLength() - 1) * 2
            - (((*daBytes)[daBytes->GetLength() - 1] & 0xf0) == 0xf0 ? 1 : 0));

    // destination address
    IOutputStream::Probe(bo)->Write(daBytes, 0, daBytes->GetLength());

    // TP-Protocol-Identifier
    IOutputStream::Probe(bo)->Write(0);
    return bo;
}

ECode CSmsMessage::ParsePdu(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    mPdu = pdu;
    // Logger::D(TAG, "raw sms message:");
    // Logger::D(TAG, s);

    AutoPtr<PduParser> p = new PduParser(pdu);

    p->GetSCAddress(&mScAddress);

    if (mScAddress != NULL) {
        if (VDBG) Logger::D(TAG, "SMS SC address: %s", mScAddress.string());
    }

    // TODO(mkf) support reply path, user data header indicator

    // TP-Message-Type-Indicator
    // 9.2.3
    Int32 firstByte;
    p->GetByte(&firstByte);

    mMti = firstByte & 0x3;
    switch (mMti) {
    // TP-Message-Type-Indicator
    // 9.2.3
    case 0:
    case 3: //GSM 03.40 9.2.3.1: MTI == 3 is Reserved.
            //This should be processed in the same way as MTI == 0 (Deliver)
        ParseSmsDeliver(p, firstByte);
        break;
    case 1:
        ParseSmsSubmit(p, firstByte);
        break;
    case 2:
        ParseSmsStatusReport(p, firstByte);
        break;
    default:
        // TODO(mkf) the rest of these
        // throw new RuntimeException("Unsupported message type");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

void CSmsMessage::ParseSmsStatusReport(
    /* [in] */ PduParser* p,
    /* [in] */ Int32 firstByte)
{
    mIsStatusReportMessage = TRUE;

    // TP-Message-Reference
    p->GetByte(&mMessageRef);
    // TP-Recipient-Address
    AutoPtr<IGsmSmsAddress> addr;
    p->GetAddress((IGsmSmsAddress**)&addr);
    mRecipientAddress = ISmsAddress::Probe(addr);
    // TP-Service-Centre-Time-Stamp
    p->GetSCTimestampMillis(&mScTimeMillis);
    // p->GetSCTimestampMillis();
    // TP-Status
    p->GetByte(&mStatus);

    // The following are optional fields that may or may not be present.
    Boolean b;
    if (p->MoreDataPresent(&b), b) {
        // TP-Parameter-Indicator
        Int32 extraParams;
        p->GetByte(&extraParams);
        Int32 moreExtraParams = extraParams;
        while ((moreExtraParams & 0x80) != 0) {
            // We only know how to parse a few extra parameters, all
            // indicated in the first TP-PI octet, so skip over any
            // additional TP-PI octets.
            p->GetByte(&moreExtraParams);
        }
        // As per 3GPP 23.040 section 9.2.3.27 TP-Parameter-Indicator,
        // only process the byte if the reserved bits (bits3 to 6) are zero.
        if ((extraParams & 0x78) == 0) {
            // TP-Protocol-Identifier
            if ((extraParams & 0x01) != 0) {
                p->GetByte(&mProtocolIdentifier);
            }
            // TP-Data-Coding-Scheme
            if ((extraParams & 0x02) != 0) {
                p->GetByte(&mDataCodingScheme);
            }
            // TP-User-Data-Length (implies existence of TP-User-Data)
            if ((extraParams & 0x04) != 0) {
                Boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;
                ParseUserData(p, hasUserDataHeader);
            }
        }
    }
}

void CSmsMessage::ParseSmsDeliver(
    /* [in] */ PduParser* p,
    /* [in] */ Int32 firstByte)
{
    mReplyPathPresent = (firstByte & 0x80) == 0x80;

    AutoPtr<IGsmSmsAddress> addr;
    p->GetAddress((IGsmSmsAddress**)&addr);
    mOriginatingAddress = ISmsAddress::Probe(addr);

    if (mOriginatingAddress != NULL) {
        // if (VDBG) Logger::V(TAG, "SMS originating address: "
        //         + mOriginatingAddress.address);
    }

    // TP-Protocol-Identifier (TP-PID)
    // TS 23.040 9.2.3.9
    p->GetByte(&mProtocolIdentifier);

    // TP-Data-Coding-Scheme
    // see TS 23.038
    p->GetByte(&mDataCodingScheme);

    if (VDBG) {
        // Logger::V(TAG, "SMS TP-PID:" + mProtocolIdentifier
        //         + " data coding scheme: " + mDataCodingScheme);
    }

    p->GetSCTimestampMillis(&mScTimeMillis);

    if (VDBG) Logger::D(TAG, "SMS SC timestamp: %ld", mScTimeMillis);

    Boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;

    ParseUserData(p, hasUserDataHeader);
}

void CSmsMessage::ParseSmsSubmit(
    /* [in] */ PduParser* p,
    /* [in] */ Int32 firstByte)
{
    mReplyPathPresent = (firstByte & 0x80) == 0x80;

    // TP-MR (TP-Message Reference)
    p->GetByte(&mMessageRef);

    AutoPtr<IGsmSmsAddress> addr;
    p->GetAddress((IGsmSmsAddress**)&addr);
    mRecipientAddress = ISmsAddress::Probe(addr);

    if (mRecipientAddress != NULL) {
        // if (VDBG) Logger::V(TAG, "SMS recipient address: " + mRecipientAddress.address);
    }

    // TP-Protocol-Identifier (TP-PID)
    // TS 23.040 9.2.3.9
    p->GetByte(&mProtocolIdentifier);

    // TP-Data-Coding-Scheme
    // see TS 23.038
    p->GetByte(&mDataCodingScheme);

    if (VDBG) {
        Logger::V(TAG, "SMS TP-PID: %d data coding scheme: %d", mProtocolIdentifier, mDataCodingScheme);
    }

    // TP-Validity-Period-Format
    Int32 validityPeriodLength = 0;
    Int32 validityPeriodFormat = ((firstByte>>3) & 0x3);
    if (0x0 == validityPeriodFormat) /* 00, TP-VP field not present*/
    {
        validityPeriodLength = 0;
    }
    else if (0x2 == validityPeriodFormat) /* 10, TP-VP: relative format*/
    {
        validityPeriodLength = 1;
    }
    else /* other case, 11 or 01, TP-VP: absolute or enhanced format*/
    {
        validityPeriodLength = 7;
    }

    // TP-Validity-Period is not used on phone, so just ignore it for now.
    while (validityPeriodLength-- > 0)
    {
        Int32 tmp;
        p->GetByte(&tmp);
    }

    Boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;

    ParseUserData(p, hasUserDataHeader);
}

void CSmsMessage::ParseUserData(
    /* [in] */ PduParser* p,
    /* [in] */ Boolean hasUserDataHeader)
{
    Boolean hasMessageClass = FALSE;
    Boolean userDataCompressed = FALSE;

    Int32 encodingType = ISmsConstants::ENCODING_UNKNOWN;

    // Look up the data encoding scheme
    if ((mDataCodingScheme & 0x80) == 0) {
        userDataCompressed = (0 != (mDataCodingScheme & 0x20));
        hasMessageClass = (0 != (mDataCodingScheme & 0x10));

        if (userDataCompressed) {
            Logger::W(TAG, "4 - Unsupported SMS data coding scheme (compression) %d",
                    (mDataCodingScheme & 0xff));
        }
        else {
            switch ((mDataCodingScheme >> 2) & 0x3) {
            case 0: // GSM 7 bit default alphabet
                encodingType = ISmsConstants::ENCODING_7BIT;
                break;

            case 2: // UCS 2 (16bit)
                encodingType = ISmsConstants::ENCODING_16BIT;
                break;

            case 1: {// 8 bit data
                //Support decoding the user data payload as pack GSM 8-bit (a GSM alphabet string
                //that's stored in 8-bit unpacked format) characters.
                AutoPtr<IResources> r = CResources::GetSystem();
                Boolean b;
                if (r->GetBoolean(R::bool_::config_sms_decode_gsm_8bit_data, &b), b) {
                    encodingType = ISmsConstants::ENCODING_8BIT;
                    break;
                }
            }
            case 3: // reserved
                Logger::W(TAG, "1 - Unsupported SMS data coding scheme %d"
                        , (mDataCodingScheme & 0xff));
                encodingType = ISmsConstants::ENCODING_8BIT;
                break;
            }
        }
    }
    else if ((mDataCodingScheme & 0xf0) == 0xf0) {
        hasMessageClass = TRUE;
        userDataCompressed = FALSE;

        if (0 == (mDataCodingScheme & 0x04)) {
            // GSM 7 bit default alphabet
            encodingType = ISmsConstants::ENCODING_7BIT;
        }
        else {
            // 8 bit data
            encodingType = ISmsConstants::ENCODING_8BIT;
        }
    }
    else if ((mDataCodingScheme & 0xF0) == 0xC0
            || (mDataCodingScheme & 0xF0) == 0xD0
            || (mDataCodingScheme & 0xF0) == 0xE0) {
        // 3GPP TS 23.038 V7.0.0 (2006-03) section 4

        // 0xC0 == 7 bit, don't store
        // 0xD0 == 7 bit, store
        // 0xE0 == UCS-2, store

        if ((mDataCodingScheme & 0xF0) == 0xE0) {
            encodingType = ISmsConstants::ENCODING_16BIT;
        }
        else {
            encodingType = ISmsConstants::ENCODING_7BIT;
        }

        userDataCompressed = FALSE;
        Boolean active = ((mDataCodingScheme & 0x08) == 0x08);
        // bit 0x04 reserved

        // VM - If TP-UDH is present, these values will be overwritten
        if ((mDataCodingScheme & 0x03) == 0x00) {
            mIsMwi = TRUE; /* Indicates vmail */
            mMwiSense = active;/* Indicates vmail notification set/clear */
            mMwiDontStore = ((mDataCodingScheme & 0xF0) == 0xC0);

            /* Set voice mail count based on notification bit */
            if (active == TRUE) {
                mVoiceMailCount = -1; // unknown number of messages waiting
            }
            else {
                mVoiceMailCount = 0; // no unread messages
            }

            Logger::W(TAG, "MWI in DCS for Vmail. DCS = %d Dont store = %d vmail count = %d",
                    (mDataCodingScheme & 0xff), mMwiDontStore, mVoiceMailCount);

        }
        else {
            mIsMwi = FALSE;
            Logger::W(TAG, "MWI in DCS for fax/email/other: %d"
                    , (mDataCodingScheme & 0xff));
        }
    }
    else if ((mDataCodingScheme & 0xC0) == 0x80) {
        // 3GPP TS 23.038 V7.0.0 (2006-03) section 4
        // 0x80..0xBF == Reserved coding groups
        if (mDataCodingScheme == 0x84) {
            // This value used for KSC5601 by carriers in Korea.
            encodingType = ISmsConstants::ENCODING_KSC5601;
        }
        else {
            Logger::W(TAG, "5 - Unsupported SMS data coding scheme %d"
                    , (mDataCodingScheme & 0xff));
        }
    }
    else {
        Logger::W(TAG, "3 - Unsupported SMS data coding scheme %d"
                , (mDataCodingScheme & 0xff));
    }

    // set both the user data and the user data header.
    Int32 count;
    p->ConstructUserData(hasUserDataHeader,
            encodingType == ISmsConstants::ENCODING_7BIT, &count);
    p->GetUserData((ArrayOf<Byte>**)&mUserData);
    p->GetUserDataHeader((ISmsHeader**)&mUserDataHeader);

    /*
     * Look for voice mail indication in TP_UDH TS23.040 9.2.3.24
     * ieid = 1 (0x1) (SPECIAL_SMS_MSG_IND)
     * ieidl =2 octets
     * ieda msg_ind_type = 0x00 (voice mail; discard sms )or
     *                   = 0x80 (voice mail; store sms)
     * msg_count = 0x00 ..0xFF
     */
// TODO: Need SmsHeader
    // Int32 size;
    // mUserDataHeader->mSpecialSmsMsgList->GetSize(&size);
    // if (hasUserDataHeader && (size != 0)) {
    //    for (SmsHeader->SpecialSmsMsg msg : mUserDataHeader->mSpecialSmsMsgList) {
    //        Int32 msgInd = msg->msgIndType & 0xff;
    //        /*
    //         * TS 23.040 V6.8.1 Sec 9.2.3.24.2
    //         * bits 1 0 : basic message indication type
    //         * bits 4 3 2 : extended message indication type
    //         * bits 6 5 : Profile id bit 7 storage type
    //         */
    //        if ((msgInd == 0) || (msgInd == 0x80)) {
    //            mIsMwi = TRUE;
    //            if (msgInd == 0x80) {
    //                /* Store message because TP_UDH indicates so*/
    //                mMwiDontStore = FALSE;
    //            }
    //            else if (mMwiDontStore == FALSE) {
    //                /* Storage bit is not set by TP_UDH
    //                 * Check for conflict
    //                 * between message storage bit in TP_UDH
    //                 * & DCS. The message shall be stored if either of
    //                 * the one indicates so.
    //                 * TS 23.040 V6.8.1 Sec 9.2.3.24.2
    //                 */
    //                if (!((((mDataCodingScheme & 0xF0) == 0xD0)
    //                       || ((mDataCodingScheme & 0xF0) == 0xE0))
    //                       && ((mDataCodingScheme & 0x03) == 0x00))) {
    //                    /* Even DCS did not have voice mail with Storage bit
    //                     * 3GPP TS 23.038 V7.0.0 section 4
    //                     * So clear this flag*/
    //                    mMwiDontStore = TRUE;
    //                }
    //            }

    //            mVoiceMailCount = msg->msgCount & 0xff;

    //            /*
    //             * In the event of a conflict between message count setting
    //             * and DCS then the Message Count in the TP-UDH shall
    //             * override the indication in the TP-DCS. Set voice mail
    //             * notification based on count in TP-UDH
    //             */
    //            if (mVoiceMailCount > 0)
    //                mMwiSense = TRUE;
    //            else
    //                mMwiSense = FALSE;

    //            Logger::W(TAG, "MWI in TP-UDH for Vmail. Msg Ind = %d Dont store = %d Vmail count = %d"
    //                    , msgInd, mMwiDontStore, mVoiceMailCount);

    //            /*
    //             * There can be only one IE for each type of message
    //             * indication in TP_UDH. In the event they are duplicated
    //             * last occurence will be used. Hence the for loop
    //             */
    //        }
    //        else {
    //            Logger::W(TAG, "TP_UDH fax/email/extended msg/multisubscriber profile. Msg Ind = %d", msgInd);
    //        }
    //    } // end of for
    //} // end of if UDH

    switch (encodingType) {
    case ISmsConstants::ENCODING_UNKNOWN:
        mMessageBody = String(NULL);
        break;

    case ISmsConstants::ENCODING_8BIT: {
        //Support decoding the user data payload as pack GSM 8-bit (a GSM alphabet string
        //that's stored in 8-bit unpacked format) characters.
        AutoPtr<IResources> r = CResources::GetSystem();
        Boolean b;
        if (r->GetBoolean(R::bool_::config_sms_decode_gsm_8bit_data, &b), b) {
            p->GetUserDataGSM8bit(count, &mMessageBody);
        }
        else {
            mMessageBody = String(NULL);
        }
        break;
    }
    case ISmsConstants::ENCODING_7BIT:
// TODO: Need SmsHeader
        // p->GetUserDataGSM7Bit(count,
        //         hasUserDataHeader ? mUserDataHeader->mLanguageTable : 0,
        //         hasUserDataHeader ? mUserDataHeader->mLanguageShiftTable : 0, &mMessageBody);
        break;

    case ISmsConstants::ENCODING_16BIT:
        p->GetUserDataUCS2(count, &mMessageBody);
        break;

    case ISmsConstants::ENCODING_KSC5601:
        p->GetUserDataKSC5601(count, &mMessageBody);
        break;
    }

    if (VDBG) Logger::V(TAG, "SMS message body (raw): '%s'", mMessageBody.string());

    if (!mMessageBody.IsNull()) {
        ParseMessageBody();
    }

    if (!hasMessageClass) {
        mMessageClass = MessageClass_UNKNOWN;
    }
    else {
        switch (mDataCodingScheme & 0x3) {
        case 0:
            mMessageClass = MessageClass_CLASS_0;
            break;
        case 1:
            mMessageClass = MessageClass_CLASS_1;
            break;
        case 2:
            mMessageClass = MessageClass_CLASS_2;
            break;
        case 3:
            mMessageClass = MessageClass_CLASS_3;
            break;
        }
    }
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos