#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_BEARERDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_BEARERDATA_H__

#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "elastos/droid/text/format/Time.h"
#include "elastos/droid/internal/telephony/cdma/sms/UserData.h"
#include "elastos/droid/internal/telephony/cdma/sms/CdmaSmsAddress.h"

// package com.android.internal.telephony.cdma.sms;
// import android.content.res.Resources;
// import android.telephony.SmsCbCmasInfo;
// import android.telephony.cdma.CdmaSmsCbProgramData;
// import android.telephony.cdma.CdmaSmsCbProgramResults;
// import android.telephony.Rlog;
// import com.android.internal.telephony.GsmAlphabet;
// import com.android.internal.telephony.SmsConstants;
// import com.android.internal.telephony.SmsHeader;
// import com.android.internal.telephony.GsmAlphabet.TextEncodingDetails;
// import com.android.internal.telephony.uicc.IccUtils;
// import com.android.internal.util.BitwiseInputStream;
// import com.android.internal.util.BitwiseOutputStream;
// import java.util.ArrayList;
// import java.util.TimeZone;

using Elastos::Droid::Internal::Telephony::IGsmAlphabetTextEncodingDetails;
//using Elastos::Droid::Internal::Telephony::Cdma::Sms::UserData;
//using Elastos::Droid::Internal::Telephony::Cdma::Sms::CdmaSmsAddress;
using Elastos::Droid::Internal::Utility::IBitwiseOutputStream;
using Elastos::Droid::Internal::Utility::IBitwiseInputStream;
using Elastos::Droid::Telephony::ISmsCbCmasInfo;
using Elastos::Droid::Text::Format::Time;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

/**
  * An object to encode and decode CDMA SMS bearer data.
  */
class BearerData
    : public Object
    , public IBearerData
{
public:
    /**
      * 6-byte-field, see 3GPP2 C.S0015-B, v2, 4.5.4
      */
    class TimeStamp
        : public Time
        , public IBearerDataTimeStamp
    {
    public:
        TimeStamp();

        CAR_INTERFACE_DECL();

        static CARAPI_(AutoPtr<TimeStamp>) FromByteArray(
            /* [in] */ ArrayOf<Byte>* data);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);
    };

private:
    //class CodingException
    //    : public Object
    //    , public Exception
    //{
    //public:
    //    CodingException(
    //        /* [in] */ const String& s);
    //};

    class Gsm7bitCodingResult
        : public Object
    {
    public:
        Int32 septets;
        AutoPtr< ArrayOf<Byte> > data;
    };

public:
    CAR_INTERFACE_DECL();

    /**
      * Returns the language indicator as a two-character ISO 639 string.
      * @return a two character ISO 639 language code
      */
    virtual CARAPI GetLanguage(
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
      * Calculate the message text encoding length, fragmentation, and other details.
      *
      * @param msg message text
      * @param force7BitEncoding ignore (but still count) illegal characters if true
      * @return septet count, or -1 on failure
      */
    static CARAPI_(AutoPtr<IGsmAlphabetTextEncodingDetails>) CalcTextEncodingDetails(
        /* [in] */ ICharSequence* msg,
        /* [in] */ Boolean force7BitEncoding);

    /**
      * Create serialized representation for BearerData object.
      * (See 3GPP2 C.R1001-F, v1.0, section 4.5 for layout details)
      *
      * @param bData an instance of BearerData.
      *
      * @return byte array of raw encoded SMS bearer data.
      */
    static CARAPI_(AutoPtr<ArrayOf<Byte> >) Encode(
        /* [in] */ IBearerData* bData);

    /**
      * Create BearerData object from serialized representation.
      * (See 3GPP2 C.R1001-F, v1.0, section 4.5 for layout details)
      *
      * @param smsData byte array of raw encoded SMS bearer data.
      * @return an instance of BearerData.
      */
    static CARAPI_(AutoPtr<BearerData>) Decode(
        /* [in] */ ArrayOf<Byte>* smsData);

    /**
      * Create BearerData object from serialized representation.
      * (See 3GPP2 C.R1001-F, v1.0, section 4.5 for layout details)
      *
      * @param smsData byte array of raw encoded SMS bearer data.
      * @param serviceCategory the envelope service category (for CMAS alert handling)
      * @return an instance of BearerData.
      */
    static CARAPI_(AutoPtr<BearerData>) Decode(
        /* [in] */ ArrayOf<Byte>* smsData,
        /* [in] */ Int32 serviceCategory);

private:
    /**
      * Converts a CDMA language indicator value to an ISO 639 two character language code.
      * @param languageValue the CDMA language value to convert
      * @return the two character ISO 639 language code for the specified value, or null if unknown
      */
    static CARAPI_(String) GetLanguageCodeForValue(
        /* [in] */ Int32 languageValue);

    static CARAPI_(void) EncodeMessageId(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(Int32) CountAsciiSeptets(
        /* [in] */ ICharSequence* msg,
        /* [in] */ Boolean force);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) Encode7bitAscii(
        /* [in] */ const String& msg,
        /* [in] */ Boolean force);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) EncodeUtf16(
        /* [in] */ const String& msg);

    static CARAPI_(AutoPtr<Gsm7bitCodingResult>) Encode7bitGsm(
        /* [in] */ const String& msg,
        /* [in] */ Int32 septetOffset,
        /* [in] */ Boolean force);

    static CARAPI_(void) Encode7bitEms(
        /* [in] */ UserData* uData,
        /* [in] */ ArrayOf<Byte>* udhData,
        /* [in] */ Boolean force);

    static CARAPI_(void) Encode16bitEms(
        /* [in] */ UserData* uData,
        /* [in] */ ArrayOf<Byte>* udhData);

    static CARAPI_(void) Encode7bitAsciiEms(
        /* [in] */ UserData* uData,
        /* [in] */ ArrayOf<Byte>* udhData,
        /* [in] */ Boolean force);

    static CARAPI_(void) EncodeEmsUserDataPayload(
        /* [in] */ UserData* uData);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) EncodeShiftJis(
        /* [in] */ const String& msg);

    static CARAPI_(void) EncodeUserDataPayload(
        /* [in] */ UserData* uData);

    static CARAPI_(void) EncodeUserData(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeReplyOption(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(AutoPtr<ArrayOf<Byte> >) EncodeDtmfSmsAddress(
        /* [in] */ const String& address);

    /*
      * TODO(cleanup): CdmaSmsAddress encoding should make use of
      * CdmaSmsAddress.parse provided that DTMF encoding is unified,
      * and the difference in 4-bit vs. 8-bit is resolved.
      */
    static CARAPI_(void) EncodeCdmaSmsAddress(
        /* [in] */ CdmaSmsAddress* addr);

    static CARAPI_(void) EncodeCallbackNumber(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeMsgStatus(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeMsgCount(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeValidityPeriodRel(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodePrivacyIndicator(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeLanguageIndicator(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeDisplayMode(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodePriorityIndicator(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeMsgDeliveryAlert(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(void) EncodeScpResults(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseOutputStream* outStream);

    static CARAPI_(Boolean) DecodeMessageId(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeReserved(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream,
        /* [in] */ Int32 subparamId);

    static CARAPI_(Boolean) DecodeUserData(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(String) DecodeUtf8(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields);

    static CARAPI_(String) DecodeUtf16(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields);

    static CARAPI_(String) DecodeCharset(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields,
        /* [in] */ Int32 width,
        /* [in] */ const String& charset);

    static CARAPI_(String) Decode7bitAscii(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields);

    static CARAPI_(String) Decode7bitGsm(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields);

    static CARAPI_(String) DecodeLatin(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields);

    static CARAPI_(String) DecodeShiftJis(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields);

    static CARAPI_(String) DecodeGsmDcs(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 numFields,
        /* [in] */ Int32 msgType);

    static CARAPI_(void) DecodeUserDataPayload(
        /* [in] */ UserData* userData,
        /* [in] */ Boolean hasUserDataHeader);

    /**
      * IS-91 Voice Mail message decoding
      * (See 3GPP2 C.S0015-A, Table 4.3.1.4.1-1)
      * (For character encodings, see TIA/EIA/IS-91, Annex B)
      *
      * Protocol Summary: The user data payload may contain 3-14
      * characters.  The first two characters are parsed as a number
      * and indicate the number of voicemails.  The third character is
      * either a SPACE or '!' to indicate normal or urgent priority,
      * respectively.  Any following characters are treated as normal
      * text user data payload.
      *
      * Note that the characters encoding is 6-bit packed.
      */
    static CARAPI_(void) DecodeIs91VoicemailStatus(
        /* [in] */ BearerData* bData);

    /**
      * IS-91 Short Message decoding
      * (See 3GPP2 C.S0015-A, Table 4.3.1.4.1-1)
      * (For character encodings, see TIA/EIA/IS-91, Annex B)
      *
      * Protocol Summary: The user data payload may contain 1-14
      * characters, which are treated as normal text user data payload.
      * Note that the characters encoding is 6-bit packed.
      */
    static CARAPI_(void) DecodeIs91ShortMessage(
        /* [in] */ BearerData* bData);

    /**
      * IS-91 CLI message (callback number) decoding
      * (See 3GPP2 C.S0015-A, Table 4.3.1.4.1-1)
      *
      * Protocol Summary: The data payload may contain 1-32 digits,
      * encoded using standard 4-bit DTMF, which are treated as a
      * callback number.
      */
    static CARAPI_(void) DecodeIs91Cli(
        /* [in] */ BearerData* bData);

    static CARAPI_(void) DecodeIs91(
        /* [in] */ BearerData* bData);

    static CARAPI_(Boolean) DecodeReplyOption(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeMsgCount(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeDepositIndex(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(String) DecodeDtmfSmsAddress(
        /* [in] */ ArrayOf<Byte>* rawData,
        /* [in] */ Int32 numFields);

    static CARAPI_(void) DecodeSmsAddress(
        /* [in] */ CdmaSmsAddress* addr);

    static CARAPI_(Boolean) DecodeCallbackNumber(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeMsgStatus(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeMsgCenterTimeStamp(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeValidityAbs(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeDeferredDeliveryAbs(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeValidityRel(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeDeferredDeliveryRel(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodePrivacyIndicator(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeLanguageIndicator(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeDisplayMode(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodePriorityIndicator(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeMsgDeliveryAlert(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeUserResponseCode(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Boolean) DecodeServiceCategoryProgramData(
        /* [in] */ BearerData* bData,
        /* [in] */ IBitwiseInputStream* inStream);

    static CARAPI_(Int32) ServiceCategoryToCmasMessageClass(
        /* [in] */ Int32 serviceCategory);

    /**
      * Calculates the number of bits to read for the specified number of encoded characters.
      * @param msgEncoding the message encoding to use
      * @param numFields the number of characters to read. For Shift-JIS and Korean encodings,
      *  this is the number of bytes to read.
      * @return the number of bits to read from the stream
      * @throws CodingException if the specified encoding is not supported
      */
    static CARAPI_(Int32) GetBitsForNumFields(
        /* [in] */ Int32 msgEncoding,
        /* [in] */ Int32 numFields);

    /**
      * CMAS message decoding.
      * (See TIA-1149-0-1, CMAS over CDMA)
      *
      * @param serviceCategory is the service category from the SMS envelope
      */
    static CARAPI_(void) DecodeCmasUserData(
        /* [in] */ BearerData* bData,
        /* [in] */ Int32 serviceCategory);

    static CARAPI_(Boolean) IsCmasAlertCategory(
        /* [in] */ Int32 category);

public:

    Int32 messageType;
    /**
      * 16-bit value indicating the message ID, which increments modulo 65536.
      * (Special rules apply for WAP-messages.)
      * (See 3GPP2 C.S0015-B, v2, 4.5.1)
      */
    Int32 messageId;
    Boolean priorityIndicatorSet;
    Int32 priority;
    Boolean privacyIndicatorSet;
    Int32 privacy;
    Boolean alertIndicatorSet;
    Int32 alert;
    Boolean displayModeSet;
    Int32 displayMode;
    Boolean languageIndicatorSet;
    Int32 language;
    Boolean messageStatusSet;
    Int32 errorClass;
    Int32 messageStatus;
    /**
      * 1-bit value that indicates whether a User Data Header (UDH) is present.
      * (See 3GPP2 C.S0015-B, v2, 4.5.1)
      *
      * NOTE: during encoding, this value will be set based on the
      * presence of a UDH in the structured data, any existing setting
      * will be overwritten.
      */
    Boolean hasUserDataHeader;
    /**
      * provides the information for the user data
      * (e.g. padding bits, user data, user data header, etc)
      * (See 3GPP2 C.S.0015-B, v2, 4.5.2)
      */
    AutoPtr<UserData> userData;
    /**
      * The User Response Code subparameter is used in the SMS User
      * Acknowledgment Message to respond to previously received short
      * messages. This message center-specific element carries the
      * identifier of a predefined response. (See 3GPP2 C.S.0015-B, v2,
      * 4.5.3)
      */
    Boolean userResponseCodeSet;
    Int32 userResponseCode;
    AutoPtr<TimeStamp> msgCenterTimeStamp;
    AutoPtr<TimeStamp> validityPeriodAbsolute;
    AutoPtr<TimeStamp> deferredDeliveryTimeAbsolute;
    Boolean validityPeriodRelativeSet;
    Int32 validityPeriodRelative;
    Boolean deferredDeliveryTimeRelativeSet;
    Int32 deferredDeliveryTimeRelative;
    /**
      * The Reply Option subparameter contains 1-bit values which
      * indicate whether SMS acknowledgment is requested or not.  (See
      * 3GPP2 C.S0015-B, v2, 4.5.11)
      */
    Boolean userAckReq;
    Boolean deliveryAckReq;
    Boolean readAckReq;
    Boolean reportReq;
    /**
      * The Number of Messages subparameter (8-bit value) is a decimal
      * number in the 0 to 99 range representing the number of messages
      * stored at the Voice Mail System. This element is used by the
      * Voice Mail Notification service.  (See 3GPP2 C.S0015-B, v2,
      * 4.5.12)
      */
    Int32 numberOfMessages;
    /**
      * The Message Deposit Index subparameter is assigned by the
      * message center as a unique index to the contents of the User
      * Data subparameter in each message sent to a particular mobile
      * station. The mobile station, when replying to a previously
      * received short message which included a Message Deposit Index
      * subparameter, may include the Message Deposit Index of the
      * received message to indicate to the message center that the
      * original contents of the message are to be included in the
      * reply.  (See 3GPP2 C.S0015-B, v2, 4.5.18)
      */
    Int32 depositIndex;
    /**
      * 4-bit or 8-bit value that indicates the number to be dialed in reply to a
      * received SMS message.
      * (See 3GPP2 C.S0015-B, v2, 4.5.15)
      */
    AutoPtr<CdmaSmsAddress> callbackNumber;
    /**
      * CMAS warning notification information.
      * @see #decodeCmasUserData(BearerData, int)
      */
    AutoPtr<ISmsCbCmasInfo> cmasWarningInfo;
    /**
      * The Service Category Program Data subparameter is used to enable and disable
      * SMS broadcast service categories to display. If this subparameter is present,
      * this field will contain a list of one or more
      * {@link android.telephony.cdma.CdmaSmsCbProgramData} objects containing the
      * operation(s) to perform.
      */
    AutoPtr<IArrayList> serviceCategoryProgramData; // ICdmaSmsCbProgramData
    /**
      * The Service Category Program Results subparameter informs the message center
      * of the results of a Service Category Program Data request.
      */
    AutoPtr<IArrayList> serviceCategoryProgramResults; // ICdmaSmsCbProgramResults

private:
    static const String LOGTAG;
    /**
      * Bearer Data Subparameter Identifiers
      * (See 3GPP2 C.S0015-B, v2.0, table 4.5-1)
      * NOTE: Commented subparameter types are not implemented.
      */
    static const Byte SUBPARAM_MESSAGE_IDENTIFIER;
    static const Byte SUBPARAM_USER_DATA;
    static const Byte SUBPARAM_USER_RESPONSE_CODE;
    static const Byte SUBPARAM_MESSAGE_CENTER_TIME_STAMP;
    static const Byte SUBPARAM_VALIDITY_PERIOD_ABSOLUTE;
    static const Byte SUBPARAM_VALIDITY_PERIOD_RELATIVE;
    static const Byte SUBPARAM_DEFERRED_DELIVERY_TIME_ABSOLUTE;
    static const Byte SUBPARAM_DEFERRED_DELIVERY_TIME_RELATIVE;
    static const Byte SUBPARAM_PRIORITY_INDICATOR;
    static const Byte SUBPARAM_PRIVACY_INDICATOR;
    static const Byte SUBPARAM_REPLY_OPTION;
    static const Byte SUBPARAM_NUMBER_OF_MESSAGES;
    static const Byte SUBPARAM_ALERT_ON_MESSAGE_DELIVERY;
    static const Byte SUBPARAM_LANGUAGE_INDICATOR;
    static const Byte SUBPARAM_CALLBACK_NUMBER;
    static const Byte SUBPARAM_MESSAGE_DISPLAY_MODE;
    //private final static byte SUBPARAM_MULTIPLE_ENCODING_USER_DATA      = 0x10;
    static const Byte SUBPARAM_MESSAGE_DEPOSIT_INDEX;
    static const Byte SUBPARAM_SERVICE_CATEGORY_PROGRAM_DATA;
    static const Byte SUBPARAM_SERVICE_CATEGORY_PROGRAM_RESULTS;
    static const Byte SUBPARAM_MESSAGE_STATUS;
    //private final static byte SUBPARAM_TP_FAILURE_CAUSE                 = 0x15;
    //private final static byte SUBPARAM_ENHANCED_VMN                     = 0x16;
    //private final static byte SUBPARAM_ENHANCED_VMN_ACK                 = 0x17;
    // All other values after this are reserved.
    static const Byte SUBPARAM_ID_LAST_DEFINED;
};

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_SMS_BEARERDATA_H__
