/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.cdma.sms;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Telephony::ISmsCbCmasInfo;
using Elastos::Droid::Telephony::Cdma::ICdmaSmsCbProgramData;
using Elastos::Droid::Telephony::Cdma::ICdmaSmsCbProgramResults;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::GsmAlphabet::ITextEncodingDetails;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Utility::IBitwiseInputStream;
using Elastos::Droid::Internal::Utility::IBitwiseOutputStream;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ITimeZone;

/**
 * An object to encode and decode CDMA SMS bearer data.
 */
public class BearerData {
    private static const String LOG_TAG = "BearerData";

    /**
     * Bearer Data Subparameter Identifiers
     * (See 3GPP2 C.S0015-B, v2.0, table 4.5-1)
     * NOTE: Commented subparameter types are not implemented.
     */
    private static const Byte SUBPARAM_MESSAGE_IDENTIFIER               = 0x00;
    private static const Byte SUBPARAM_USER_DATA                        = 0x01;
    private static const Byte SUBPARAM_USER_RESPONSE_CODE               = 0x02;
    private static const Byte SUBPARAM_MESSAGE_CENTER_TIME_STAMP        = 0x03;
    private static const Byte SUBPARAM_VALIDITY_PERIOD_ABSOLUTE         = 0x04;
    private static const Byte SUBPARAM_VALIDITY_PERIOD_RELATIVE         = 0x05;
    private static const Byte SUBPARAM_DEFERRED_DELIVERY_TIME_ABSOLUTE  = 0x06;
    private static const Byte SUBPARAM_DEFERRED_DELIVERY_TIME_RELATIVE  = 0x07;
    private static const Byte SUBPARAM_PRIORITY_INDICATOR               = 0x08;
    private static const Byte SUBPARAM_PRIVACY_INDICATOR                = 0x09;
    private static const Byte SUBPARAM_REPLY_OPTION                     = 0x0A;
    private static const Byte SUBPARAM_NUMBER_OF_MESSAGES               = 0x0B;
    private static const Byte SUBPARAM_ALERT_ON_MESSAGE_DELIVERY        = 0x0C;
    private static const Byte SUBPARAM_LANGUAGE_INDICATOR               = 0x0D;
    private static const Byte SUBPARAM_CALLBACK_NUMBER                  = 0x0E;
    private static const Byte SUBPARAM_MESSAGE_DISPLAY_MODE             = 0x0F;
    //private static const Byte SUBPARAM_MULTIPLE_ENCODING_USER_DATA      = 0x10;
    private static const Byte SUBPARAM_MESSAGE_DEPOSIT_INDEX            = 0x11;
    private static const Byte SUBPARAM_SERVICE_CATEGORY_PROGRAM_DATA    = 0x12;
    private static const Byte SUBPARAM_SERVICE_CATEGORY_PROGRAM_RESULTS = 0x13;
    private static const Byte SUBPARAM_MESSAGE_STATUS                   = 0x14;
    //private static const Byte SUBPARAM_TP_FAILURE_CAUSE                 = 0x15;
    //private static const Byte SUBPARAM_ENHANCED_VMN                     = 0x16;
    //private static const Byte SUBPARAM_ENHANCED_VMN_ACK                 = 0x17;

    // All other values after this are reserved.
    private static const Byte SUBPARAM_ID_LAST_DEFINED                    = 0x17;

    /**
     * Supported message types for CDMA SMS messages
     * (See 3GPP2 C.S0015-B, v2.0, table 4.5.1-1)
     */
    public static const Int32 MESSAGE_TYPE_DELIVER        = 0x01;
    public static const Int32 MESSAGE_TYPE_SUBMIT         = 0x02;
    public static const Int32 MESSAGE_TYPE_CANCELLATION   = 0x03;
    public static const Int32 MESSAGE_TYPE_DELIVERY_ACK   = 0x04;
    public static const Int32 MESSAGE_TYPE_USER_ACK       = 0x05;
    public static const Int32 MESSAGE_TYPE_READ_ACK       = 0x06;
    public static const Int32 MESSAGE_TYPE_DELIVER_REPORT = 0x07;
    public static const Int32 MESSAGE_TYPE_SUBMIT_REPORT  = 0x08;

    public Int32 messageType;

    /**
     * 16-bit value indicating the message ID, which increments modulo 65536.
     * (Special rules apply for WAP-messages.)
     * (See 3GPP2 C.S0015-B, v2, 4.5.1)
     */
    public Int32 messageId;

    /**
     * Supported priority modes for CDMA SMS messages
     * (See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1)
     */
    public static const Int32 PRIORITY_NORMAL        = 0x0;
    public static const Int32 PRIORITY_INTERACTIVE   = 0x1;
    public static const Int32 PRIORITY_URGENT        = 0x2;
    public static const Int32 PRIORITY_EMERGENCY     = 0x3;

    public Boolean priorityIndicatorSet = FALSE;
    public Int32 priority = PRIORITY_NORMAL;

    /**
     * Supported privacy modes for CDMA SMS messages
     * (See 3GPP2 C.S0015-B, v2.0, table 4.5.10-1)
     */
    public static const Int32 PRIVACY_NOT_RESTRICTED = 0x0;
    public static const Int32 PRIVACY_RESTRICTED     = 0x1;
    public static const Int32 PRIVACY_CONFIDENTIAL   = 0x2;
    public static const Int32 PRIVACY_SECRET         = 0x3;

    public Boolean privacyIndicatorSet = FALSE;
    public Int32 privacy = PRIVACY_NOT_RESTRICTED;

    /**
     * Supported alert priority modes for CDMA SMS messages
     * (See 3GPP2 C.S0015-B, v2.0, table 4.5.13-1)
     */
    public static const Int32 ALERT_DEFAULT          = 0x0;
    public static const Int32 ALERT_LOW_PRIO         = 0x1;
    public static const Int32 ALERT_MEDIUM_PRIO      = 0x2;
    public static const Int32 ALERT_HIGH_PRIO        = 0x3;

    public Boolean alertIndicatorSet = FALSE;
    public Int32 alert = ALERT_DEFAULT;

    /**
     * Supported display modes for CDMA SMS messages.  Display mode is
     * a 2-bit value used to indicate to the mobile station when to
     * display the received message.  (See 3GPP2 C.S0015-B, v2,
     * 4.5.16)
     */
    public static const Int32 DISPLAY_MODE_IMMEDIATE      = 0x0;
    public static const Int32 DISPLAY_MODE_DEFAULT        = 0x1;
    public static const Int32 DISPLAY_MODE_USER           = 0x2;

    public Boolean displayModeSet = FALSE;
    public Int32 displayMode = DISPLAY_MODE_DEFAULT;

    /**
     * Language Indicator values.  NOTE: the Spec (3GPP2 C.S0015-B,
     * v2, 4.5.14) is ambiguous as to the meaning of this field, as it
     * refers to C.R1001-D but that reference has been crossed out.
     * It would seem reasonable to assume the values from C.R1001-F
     * (table 9.2-1) are to be used instead.
     */
    public static const Int32 LANGUAGE_UNKNOWN  = 0x00;
    public static const Int32 LANGUAGE_ENGLISH  = 0x01;
    public static const Int32 LANGUAGE_FRENCH   = 0x02;
    public static const Int32 LANGUAGE_SPANISH  = 0x03;
    public static const Int32 LANGUAGE_JAPANESE = 0x04;
    public static const Int32 LANGUAGE_KOREAN   = 0x05;
    public static const Int32 LANGUAGE_CHINESE  = 0x06;
    public static const Int32 LANGUAGE_HEBREW   = 0x07;

    public Boolean languageIndicatorSet = FALSE;
    public Int32 language = LANGUAGE_UNKNOWN;

    /**
     * SMS Message Status Codes.  The first component of the Message
     * status indicates if an error has occurred and whether the error
     * is considered permanent or temporary.  The second component of
     * the Message status indicates the cause of the Error (if any).
     * (See 3GPP2 C.S0015-B, v2.0, 4.5.21)
     */
    /* no-error codes */
    public static const Int32 ERROR_NONE                   = 0x00;
    public static const Int32 STATUS_ACCEPTED              = 0x00;
    public static const Int32 STATUS_DEPOSITED_TO_INTERNET = 0x01;
    public static const Int32 STATUS_DELIVERED             = 0x02;
    public static const Int32 STATUS_CANCELLED             = 0x03;
    /* temporary-error and permanent-error codes */
    public static const Int32 ERROR_TEMPORARY              = 0x02;
    public static const Int32 STATUS_NETWORK_CONGESTION    = 0x04;
    public static const Int32 STATUS_NETWORK_ERROR         = 0x05;
    public static const Int32 STATUS_UNKNOWN_ERROR         = 0x1F;
    /* permanent-error codes */
    public static const Int32 ERROR_PERMANENT              = 0x03;
    public static const Int32 STATUS_CANCEL_FAILED         = 0x06;
    public static const Int32 STATUS_BLOCKED_DESTINATION   = 0x07;
    public static const Int32 STATUS_TEXT_TOO_LONG         = 0x08;
    public static const Int32 STATUS_DUPLICATE_MESSAGE     = 0x09;
    public static const Int32 STATUS_INVALID_DESTINATION   = 0x0A;
    public static const Int32 STATUS_MESSAGE_EXPIRED       = 0x0D;
    /* undefined-status codes */
    public static const Int32 ERROR_UNDEFINED              = 0xFF;
    public static const Int32 STATUS_UNDEFINED             = 0xFF;

    public Boolean messageStatusSet = FALSE;
    public Int32 errorClass = ERROR_UNDEFINED;
    public Int32 messageStatus = STATUS_UNDEFINED;

    /**
     * 1-bit value that indicates whether a User Data Header (UDH) is present.
     * (See 3GPP2 C.S0015-B, v2, 4.5.1)
     *
     * NOTE: during encoding, this value will be set based on the
     * presence of a UDH in the structured data, any existing setting
     * will be overwritten.
     */
    public Boolean hasUserDataHeader;

    /**
     * provides the information for the user data
     * (e.g. padding bits, user data, user data header, etc)
     * (See 3GPP2 C.S.0015-B, v2, 4.5.2)
     */
    public UserData userData;

    /**
     * The User Response Code subparameter is used in the SMS User
     * Acknowledgment Message to respond to previously received short
     * messages. This message center-specific element carries the
     * identifier of a predefined response. (See 3GPP2 C.S.0015-B, v2,
     * 4.5.3)
     */
    public Boolean userResponseCodeSet = FALSE;
    public Int32 userResponseCode;

    /**
     * 6-Byte-field, see 3GPP2 C.S0015-B, v2, 4.5.4
     */
    public static class TimeStamp extends Time {

        public TimeStamp() {
            Super(TimeZone->GetDefault()->GetID());   // 3GPP2 timestamps use the local timezone
        }

        public static TimeStamp FromByteArray(Byte[] data) {
            TimeStamp ts = new TimeStamp();
            // C.S0015-B v2.0, 4.5.4: range is 1996-2095
            Int32 year = IccUtils->CdmaBcdByteToInt(data[0]);
            If (year > 99 || year < 0) return NULL;
            ts.year = year >= 96 ? year + 1900 : year + 2000;
            Int32 month = IccUtils->CdmaBcdByteToInt(data[1]);
            If (month < 1 || month > 12) return NULL;
            ts.month = month - 1;
            Int32 day = IccUtils->CdmaBcdByteToInt(data[2]);
            If (day < 1 || day > 31) return NULL;
            ts.monthDay = day;
            Int32 hour = IccUtils->CdmaBcdByteToInt(data[3]);
            If (hour < 0 || hour > 23) return NULL;
            ts.hour = hour;
            Int32 minute = IccUtils->CdmaBcdByteToInt(data[4]);
            If (minute < 0 || minute > 59) return NULL;
            ts.minute = minute;
            Int32 second = IccUtils->CdmaBcdByteToInt(data[5]);
            If (second < 0 || second > 59) return NULL;
            ts.second = second;
            return ts;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            StringBuilder builder = new StringBuilder();
            builder->Append("TimeStamp ");
            builder->Append("{ year=" + year);
            builder->Append(", month=" + month);
            builder->Append(", day=" + monthDay);
            builder->Append(", hour=" + hour);
            builder->Append(", minute=" + minute);
            builder->Append(", second=" + second);
            builder->Append(" }");
            return builder->ToString();
        }
    }

    public TimeStamp msgCenterTimeStamp;
    public TimeStamp validityPeriodAbsolute;
    public TimeStamp deferredDeliveryTimeAbsolute;

    /**
     * Relative time is specified as one Byte, the value of which
     * falls into a series of ranges, as specified below.  The idea is
     * that shorter time intervals allow greater precision -- the
     * value means minutes from zero until the MINS_LIMIT (inclusive),
     * upon which it means hours until the HOURS_LIMIT, and so
     * forth. (See 3GPP2 C.S0015-B, v2, 4.5.6-1)
     */
    public static const Int32 RELATIVE_TIME_MINS_LIMIT      = 143;
    public static const Int32 RELATIVE_TIME_HOURS_LIMIT     = 167;
    public static const Int32 RELATIVE_TIME_DAYS_LIMIT      = 196;
    public static const Int32 RELATIVE_TIME_WEEKS_LIMIT     = 244;
    public static const Int32 RELATIVE_TIME_INDEFINITE      = 245;
    public static const Int32 RELATIVE_TIME_NOW             = 246;
    public static const Int32 RELATIVE_TIME_MOBILE_INACTIVE = 247;
    public static const Int32 RELATIVE_TIME_RESERVED        = 248;

    public Boolean validityPeriodRelativeSet;
    public Int32 validityPeriodRelative;
    public Boolean deferredDeliveryTimeRelativeSet;
    public Int32 deferredDeliveryTimeRelative;

    /**
     * The Reply Option subparameter contains 1-bit values which
     * indicate whether SMS acknowledgment is requested or not.  (See
     * 3GPP2 C.S0015-B, v2, 4.5.11)
     */
    public Boolean userAckReq;
    public Boolean deliveryAckReq;
    public Boolean readAckReq;
    public Boolean reportReq;

    /**
     * The Number of Messages Subparameter (8-bit value) is a decimal
     * number in the 0 to 99 range representing the number of messages
     * stored at the Voice Mail System. This element is used by the
     * Voice Mail Notification service.  (See 3GPP2 C.S0015-B, v2,
     * 4.5.12)
     */
    public Int32 numberOfMessages;

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
    public Int32 depositIndex;

    /**
     * 4-bit or 8-bit value that indicates the number to be dialed in reply to a
     * received SMS message.
     * (See 3GPP2 C.S0015-B, v2, 4.5.15)
     */
    public CdmaSmsAddress callbackNumber;

    /**
     * CMAS warning notification information.
     * @see #DecodeCmasUserData(BearerData, Int32)
     */
    public SmsCbCmasInfo cmasWarningInfo;

    /**
     * The Service Category Program Data subparameter is used to enable and disable
     * SMS broadcast service categories to display. If this subparameter is present,
     * this field will contain a list of one or more
     * {@link android.telephony.cdma.CdmaSmsCbProgramData} objects containing the
     * Operation(s) to perform.
     */
    public ArrayList<CdmaSmsCbProgramData> serviceCategoryProgramData;

    /**
     * The Service Category Program Results subparameter informs the message center
     * of the results of a Service Category Program Data request.
     */
    public ArrayList<CdmaSmsCbProgramResults> serviceCategoryProgramResults;


    private static class CodingException extends Exception {
        public CodingException(String s) {
            Super(s);
        }
    }

    /**
     * Returns the language indicator as a two-character ISO 639 string.
     * @return a two character ISO 639 language code
     */
    public String GetLanguage() {
        return GetLanguageCodeForValue(language);
    }

    /**
     * Converts a CDMA language indicator value to an ISO 639 two character language code.
     * @param languageValue the CDMA language value to convert
     * @return the two character ISO 639 language code for the specified value, or NULL if unknown
     */
    private static String GetLanguageCodeForValue(Int32 languageValue) {
        Switch (languageValue) {
            case LANGUAGE_ENGLISH:
                return "en";

            case LANGUAGE_FRENCH:
                return "fr";

            case LANGUAGE_SPANISH:
                return "es";

            case LANGUAGE_JAPANESE:
                return "ja";

            case LANGUAGE_KOREAN:
                return "ko";

            case LANGUAGE_CHINESE:
                return "zh";

            case LANGUAGE_HEBREW:
                return "he";

            default:
                return NULL;
        }
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder builder = new StringBuilder();
        builder->Append("BearerData ");
        builder->Append("{ messageType=" + messageType);
        builder->Append(", messageId=" + messageId);
        builder->Append(", priority=" + (priorityIndicatorSet ? priority : "unset"));
        builder->Append(", privacy=" + (privacyIndicatorSet ? privacy : "unset"));
        builder->Append(", alert=" + (alertIndicatorSet ? alert : "unset"));
        builder->Append(", displayMode=" + (displayModeSet ? displayMode : "unset"));
        builder->Append(", language=" + (languageIndicatorSet ? language : "unset"));
        builder->Append(", errorClass=" + (messageStatusSet ? errorClass : "unset"));
        builder->Append(", msgStatus=" + (messageStatusSet ? messageStatus : "unset"));
        builder->Append(", msgCenterTimeStamp=" +
                ((msgCenterTimeStamp != NULL) ? msgCenterTimeStamp : "unset"));
        builder->Append(", validityPeriodAbsolute=" +
                ((validityPeriodAbsolute != NULL) ? validityPeriodAbsolute : "unset"));
        builder->Append(", validityPeriodRelative=" +
                ((validityPeriodRelativeSet) ? validityPeriodRelative : "unset"));
        builder->Append(", deferredDeliveryTimeAbsolute=" +
                ((deferredDeliveryTimeAbsolute != NULL) ? deferredDeliveryTimeAbsolute : "unset"));
        builder->Append(", deferredDeliveryTimeRelative=" +
                ((deferredDeliveryTimeRelativeSet) ? deferredDeliveryTimeRelative : "unset"));
        builder->Append(", userAckReq=" + userAckReq);
        builder->Append(", deliveryAckReq=" + deliveryAckReq);
        builder->Append(", readAckReq=" + readAckReq);
        builder->Append(", reportReq=" + reportReq);
        builder->Append(", numberOfMessages=" + numberOfMessages);
        builder->Append(", callbackNumber=" + callbackNumber);
        builder->Append(", depositIndex=" + depositIndex);
        builder->Append(", hasUserDataHeader=" + hasUserDataHeader);
        builder->Append(", userData=" + userData);
        builder->Append(" }");
        return builder->ToString();
    }

    private static void EncodeMessageId(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 3);
        outStream->Write(4, bData.messageType);
        outStream->Write(8, bData.messageId >> 8);
        outStream->Write(8, bData.messageId);
        outStream->Write(1, bData.hasUserDataHeader ? 1 : 0);
        outStream->Skip(3);
    }

    private static Int32 CountAsciiSeptets(CharSequence msg, Boolean force) {
        Int32 msgLen = msg->Length();
        If (force) return msgLen;
        For (Int32 i = 0; i < msgLen; i++) {
            If (UserData.charToAscii->Get(msg->CharAt(i), -1) == -1) {
                return -1;
            }
        }
        return msgLen;
    }

    /**
     * Calculate the message text encoding length, fragmentation, and other details.
     *
     * @param msg message text
     * @param force7BitEncoding Ignore (but still count) illegal characters if TRUE
     * @return septet count, or -1 on failure
     */
    public static TextEncodingDetails CalcTextEncodingDetails(CharSequence msg,
            Boolean force7BitEncoding) {
        TextEncodingDetails ted;
        Int32 septets = CountAsciiSeptets(msg, force7BitEncoding);
        If (septets != -1 && septets <= SmsConstants.MAX_USER_DATA_SEPTETS) {
            ted = new TextEncodingDetails();
            ted.msgCount = 1;
            ted.codeUnitCount = septets;
            ted.codeUnitsRemaining = SmsConstants.MAX_USER_DATA_SEPTETS - septets;
            ted.codeUnitSize = SmsConstants.ENCODING_7BIT;
        } else {
            ted = com.android.internal.telephony.gsm.SmsMessage->CalculateLength(
                    msg, force7BitEncoding);
            If (ted.msgCount == 1 && ted.codeUnitSize == SmsConstants.ENCODING_7BIT) {
                // We don't support single-segment EMS, so calculate for 16-bit
                // TODO: Consider supporting single-segment EMS
                ted.codeUnitCount = msg->Length();
                Int32 octets = ted.codeUnitCount * 2;
                If (octets > SmsConstants.MAX_USER_DATA_BYTES) {
                    // If EMS is not supported, break down EMS into single segment SMS
                    // and add page info " x/y".
                    // In the case of UCS2 encoding type, we need 8 bytes for this
                    // but we only have 6 bytes from UDH, so truncate the limit for
                    // each segment by 2 Bytes (1 Char32).
                    Int32 max_user_data_bytes_with_header =
                            SmsConstants.MAX_USER_DATA_BYTES_WITH_HEADER;
                    If (!android.telephony.SmsMessage->HasEmsSupport()) {
                        // make sure total number of segments is less than 10
                        If (octets <= 9 * (max_user_data_bytes_with_header - 2))
                            max_user_data_bytes_with_header -= 2;
                    }

                    ted.msgCount = (octets + (max_user_data_bytes_with_header - 1)) /
                            max_user_data_bytes_with_header;
                    ted.codeUnitsRemaining = ((ted.msgCount *
                            max_user_data_bytes_with_header) - octets) / 2;
                } else {
                    ted.msgCount = 1;
                    ted.codeUnitsRemaining = (SmsConstants.MAX_USER_DATA_BYTES - octets)/2;
                }
                ted.codeUnitSize = SmsConstants.ENCODING_16BIT;
            }
        }
        return ted;
    }

    private static Byte[] Encode7bitAscii(String msg, Boolean force)
        throws CodingException
    {
        try {
            BitwiseOutputStream outStream = new BitwiseOutputStream(msg->Length());
            Int32 msgLen = msg->Length();
            For (Int32 i = 0; i < msgLen; i++) {
                Int32 charCode = UserData.charToAscii->Get(msg->CharAt(i), -1);
                If (charCode == -1) {
                    If (force) {
                        outStream->Write(7, UserData.UNENCODABLE_7_BIT_CHAR);
                    } else {
                        throw new CodingException("cannot ASCII Encode (" + msg->CharAt(i) + ")");
                    }
                } else {
                    outStream->Write(7, charCode);
                }
            }
            return outStream->ToByteArray();
        } Catch (BitwiseOutputStream.AccessException ex) {
            throw new CodingException("7bit ASCII encode failed: " + ex);
        }
    }

    private static Byte[] EncodeUtf16(String msg)
        throws CodingException
    {
        try {
            return msg->GetBytes("utf-16be");
        } Catch (java.io.UnsupportedEncodingException ex) {
            throw new CodingException("UTF-16 encode failed: " + ex);
        }
    }

    private static class Gsm7bitCodingResult {
        Int32 septets;
        Byte[] data;
    }

    private static Gsm7bitCodingResult Encode7bitGsm(String msg, Int32 septetOffset, Boolean force)
        throws CodingException
    {
        try {
            /*
             * TODO(cleanup): It would be nice if GsmAlphabet provided
             * an option to produce just the data without prepending
             * the septet count, as this function is really just a
             * wrapper to strip that off.  Not to mention that the
             * septet count is generally known prior to invocation of
             * the encoder.  Note that it cannot be derived from the
             * resulting array length, since that cannot distinguish
             * if the last contains either 1 or 8 valid bits.
             *
             * TODO(cleanup): The BitwiseXStreams could also be
             * extended with Byte-wise reversed endianness read/write
             * routines to allow a corresponding implementation of
             * stringToGsm7BitPacked, and potentially directly support
             * access to the main bitwise stream from encode/decode.
             */
            Byte[] fullData = GsmAlphabet->StringToGsm7BitPacked(msg, septetOffset, !force, 0, 0);
            Gsm7bitCodingResult result = new Gsm7bitCodingResult();
            result.data = new Byte[fullData.length - 1];
            System->Arraycopy(fullData, 1, result.data, 0, fullData.length - 1);
            result.septets = fullData[0] & 0x00FF;
            return result;
        } Catch (com.android.internal.telephony.EncodeException ex) {
            throw new CodingException("7bit GSM encode failed: " + ex);
        }
    }

    private static void Encode7bitEms(UserData uData, Byte[] udhData, Boolean force)
        throws CodingException
    {
        Int32 udhBytes = udhData.length + 1;  // Add length octet.
        Int32 udhSeptets = ((udhBytes * 8) + 6) / 7;
        Gsm7bitCodingResult gcr = Encode7bitGsm(uData.payloadStr, udhSeptets, force);
        uData.msgEncoding = UserData.ENCODING_GSM_7BIT_ALPHABET;
        uData.msgEncodingSet = TRUE;
        uData.numFields = gcr.septets;
        uData.payload = gcr.data;
        uData.payload[0] = (Byte)udhData.length;
        System->Arraycopy(udhData, 0, uData.payload, 1, udhData.length);
    }

    private static void Encode16bitEms(UserData uData, Byte[] udhData)
        throws CodingException
    {
        Byte[] payload = EncodeUtf16(uData.payloadStr);
        Int32 udhBytes = udhData.length + 1;  // Add length octet.
        Int32 udhCodeUnits = (udhBytes + 1) / 2;
        Int32 payloadCodeUnits = payload.length / 2;
        uData.msgEncoding = UserData.ENCODING_UNICODE_16;
        uData.msgEncodingSet = TRUE;
        uData.numFields = udhCodeUnits + payloadCodeUnits;
        uData.payload = new Byte[uData.numFields * 2];
        uData.payload[0] = (Byte)udhData.length;
        System->Arraycopy(udhData, 0, uData.payload, 1, udhData.length);
        System->Arraycopy(payload, 0, uData.payload, udhBytes, payload.length);
    }

    private static void Encode7bitAsciiEms(UserData uData, Byte[] udhData, Boolean force)
            throws CodingException
    {
        try {
            Rlog->D(LOG_TAG, "encode7bitAsciiEms");
            Int32 udhBytes = udhData.length + 1;  // Add length octet.
            Int32 udhSeptets = ((udhBytes * 8) + 6) / 7;
            Int32 paddingBits = (udhSeptets * 7) - (udhBytes * 8);
            String msg = uData.payloadStr;
            Byte[] payload ;
            Int32 msgLen = msg->Length();
            BitwiseOutputStream outStream = new BitwiseOutputStream(msgLen +
                    (paddingBits > 0 ? 1 : 0));
            outStream->Write(paddingBits, 0);
            For (Int32 i = 0; i < msgLen; i++) {
                Int32 charCode = UserData.charToAscii->Get(msg->CharAt(i), -1);
                If (charCode == -1) {
                    If (force) {
                        outStream->Write(7, UserData.UNENCODABLE_7_BIT_CHAR);
                    } else {
                        throw new CodingException("cannot ASCII Encode (" + msg->CharAt(i) + ")");
                    }
                } else {
                    outStream->Write(7, charCode);
                }
            }
            payload = outStream->ToByteArray();
            uData.msgEncoding = UserData.ENCODING_7BIT_ASCII;
            uData.msgEncodingSet = TRUE;
            uData.numFields = udhSeptets + uData.payloadStr->Length();
            uData.payload = new Byte[udhBytes + payload.length];
            uData.payload[0] = (Byte)udhData.length;
            System->Arraycopy(udhData, 0, uData.payload, 1, udhData.length);
            System->Arraycopy(payload, 0, uData.payload, udhBytes, payload.length);
        } Catch (BitwiseOutputStream.AccessException ex) {
            throw new CodingException("7bit ASCII encode failed: " + ex);
        }
    }

    private static void EncodeEmsUserDataPayload(UserData uData)
        throws CodingException
    {
        Byte[] headerData = SmsHeader->ToByteArray(uData.userDataHeader);
        If (uData.msgEncodingSet) {
            If (uData.msgEncoding == UserData.ENCODING_GSM_7BIT_ALPHABET) {
                Encode7bitEms(uData, headerData, TRUE);
            } else If (uData.msgEncoding == UserData.ENCODING_UNICODE_16) {
                Encode16bitEms(uData, headerData);
            } else If (uData.msgEncoding == UserData.ENCODING_7BIT_ASCII) {
                Encode7bitAsciiEms(uData, headerData, TRUE);
            } else {
                throw new CodingException("unsupported EMS user data Encoding (" +
                                          uData.msgEncoding + ")");
            }
        } else {
            try {
                Encode7bitEms(uData, headerData, FALSE);
            } Catch (CodingException ex) {
                Encode16bitEms(uData, headerData);
            }
        }
    }

    private static Byte[] EncodeShiftJis(String msg) throws CodingException {
        try {
            return msg->GetBytes("Shift_JIS");
        } Catch (java.io.UnsupportedEncodingException ex) {
            throw new CodingException("Shift-JIS encode failed: " + ex);
        }
    }

    private static void EncodeUserDataPayload(UserData uData)
        throws CodingException
    {
        If ((uData.payloadStr == NULL) && (uData.msgEncoding != UserData.ENCODING_OCTET)) {
            Rlog->E(LOG_TAG, "user data with NULL payloadStr");
            uData.payloadStr = "";
        }

        If (uData.userDataHeader != NULL) {
            EncodeEmsUserDataPayload(uData);
            return;
        }

        If (uData.msgEncodingSet) {
            If (uData.msgEncoding == UserData.ENCODING_OCTET) {
                If (uData.payload == NULL) {
                    Rlog->E(LOG_TAG, "user data with octet encoding but NULL payload");
                    uData.payload = new Byte[0];
                    uData.numFields = 0;
                } else {
                    uData.numFields = uData.payload.length;
                }
            } else {
                If (uData.payloadStr == NULL) {
                    Rlog->E(LOG_TAG, "non-octet user data with NULL payloadStr");
                    uData.payloadStr = "";
                }
                If (uData.msgEncoding == UserData.ENCODING_GSM_7BIT_ALPHABET) {
                    Gsm7bitCodingResult gcr = Encode7bitGsm(uData.payloadStr, 0, TRUE);
                    uData.payload = gcr.data;
                    uData.numFields = gcr.septets;
                } else If (uData.msgEncoding == UserData.ENCODING_7BIT_ASCII) {
                    uData.payload = Encode7bitAscii(uData.payloadStr, TRUE);
                    uData.numFields = uData.payloadStr->Length();
                } else If (uData.msgEncoding == UserData.ENCODING_UNICODE_16) {
                    uData.payload = EncodeUtf16(uData.payloadStr);
                    uData.numFields = uData.payloadStr->Length();
                } else If (uData.msgEncoding == UserData.ENCODING_SHIFT_JIS) {
                    uData.payload = EncodeShiftJis(uData.payloadStr);
                    uData.numFields = uData.payload.length;
                } else {
                    throw new CodingException("unsupported user data Encoding (" +
                                              uData.msgEncoding + ")");
                }
            }
        } else {
            try {
                uData.payload = Encode7bitAscii(uData.payloadStr, FALSE);
                uData.msgEncoding = UserData.ENCODING_7BIT_ASCII;
            } Catch (CodingException ex) {
                uData.payload = EncodeUtf16(uData.payloadStr);
                uData.msgEncoding = UserData.ENCODING_UNICODE_16;
            }
            uData.numFields = uData.payloadStr->Length();
            uData.msgEncodingSet = TRUE;
        }
    }

    private static void EncodeUserData(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException, CodingException
    {
        /*
         * TODO(cleanup): Do we really need to set userData.payload as
         * a side effect of encoding?  If not, we could avoid data
         * copies by passing outStream directly.
         */
        EncodeUserDataPayload(bData.userData);
        bData.hasUserDataHeader = bData.userData.userDataHeader != NULL;

        If (bData.userData.payload.length > SmsConstants.MAX_USER_DATA_BYTES) {
            throw new CodingException("encoded user data too Large (" +
                                      bData.userData.payload.length +
                                      " > " + SmsConstants.MAX_USER_DATA_BYTES + " bytes)");
        }

        /*
         * TODO(cleanup): figure out what the right answer is WRT paddingBits field
         *
         *   userData.paddingBits = (userData.payload.length * 8) - (userData.numFields * 7);
         *   userData.paddingBits = 0; // XXX this seems better, but why?
         *
         */
        Int32 dataBits = (bData.userData.payload.length * 8) - bData.userData.paddingBits;
        Int32 paramBits = dataBits + 13;
        If ((bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) ||
            (bData.userData.msgEncoding == UserData.ENCODING_GSM_DCS)) {
            paramBits += 8;
        }
        Int32 paramBytes = (paramBits / 8) + ((paramBits % 8) > 0 ? 1 : 0);
        Int32 paddingBits = (paramBytes * 8) - paramBits;
        outStream->Write(8, paramBytes);
        outStream->Write(5, bData.userData.msgEncoding);
        If ((bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) ||
            (bData.userData.msgEncoding == UserData.ENCODING_GSM_DCS)) {
            outStream->Write(8, bData.userData.msgType);
        }
        outStream->Write(8, bData.userData.numFields);
        outStream->WriteByteArray(dataBits, bData.userData.payload);
        If (paddingBits > 0) outStream->Write(paddingBits, 0);
    }

    private static void EncodeReplyOption(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(1, bData.userAckReq     ? 1 : 0);
        outStream->Write(1, bData.deliveryAckReq ? 1 : 0);
        outStream->Write(1, bData.readAckReq     ? 1 : 0);
        outStream->Write(1, bData.reportReq      ? 1 : 0);
        outStream->Write(4, 0);
    }

    private static Byte[] EncodeDtmfSmsAddress(String address) {
        Int32 digits = address->Length();
        Int32 dataBits = digits * 4;
        Int32 dataBytes = (dataBits / 8);
        dataBytes += (dataBits % 8) > 0 ? 1 : 0;
        Byte[] rawData = new Byte[dataBytes];
        For (Int32 i = 0; i < digits; i++) {
            Char32 c = address->CharAt(i);
            Int32 val = 0;
            If ((c >= '1') && (c <= '9')) val = c - '0';
            else If (c == '0') val = 10;
            else If (c == '*') val = 11;
            else If (c == '#') val = 12;
            else return NULL;
            rawData[i / 2] |= val << (4 - ((i % 2) * 4));
        }
        return rawData;
    }

    /*
     * TODO(cleanup): CdmaSmsAddress encoding should make use of
     * CdmaSmsAddress.parse provided that DTMF encoding is unified,
     * and the difference in 4-bit vs. 8-bit is resolved.
     */

    private static void EncodeCdmaSmsAddress(CdmaSmsAddress addr) throws CodingException {
        If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
            try {
                addr.origBytes = addr.address->GetBytes("US-ASCII");
            } Catch (java.io.UnsupportedEncodingException ex) {
                throw new CodingException("invalid SMS address, cannot convert to ASCII");
            }
        } else {
            addr.origBytes = EncodeDtmfSmsAddress(addr.address);
        }
    }

    private static void EncodeCallbackNumber(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException, CodingException
    {
        CdmaSmsAddress addr = bData.callbackNumber;
        EncodeCdmaSmsAddress(addr);
        Int32 paramBits = 9;
        Int32 dataBits = 0;
        If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
            paramBits += 7;
            dataBits = addr.numberOfDigits * 8;
        } else {
            dataBits = addr.numberOfDigits * 4;
        }
        paramBits += dataBits;
        Int32 paramBytes = (paramBits / 8) + ((paramBits % 8) > 0 ? 1 : 0);
        Int32 paddingBits = (paramBytes * 8) - paramBits;
        outStream->Write(8, paramBytes);
        outStream->Write(1, addr.digitMode);
        If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
            outStream->Write(3, addr.ton);
            outStream->Write(4, addr.numberPlan);
        }
        outStream->Write(8, addr.numberOfDigits);
        outStream->WriteByteArray(dataBits, addr.origBytes);
        If (paddingBits > 0) outStream->Write(paddingBits, 0);
    }

    private static void EncodeMsgStatus(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(2, bData.errorClass);
        outStream->Write(6, bData.messageStatus);
    }

    private static void EncodeMsgCount(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(8, bData.numberOfMessages);
    }

    private static void EncodeValidityPeriodRel(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(8, bData.validityPeriodRelative);
    }

    private static void EncodePrivacyIndicator(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(2, bData.privacy);
        outStream->Skip(6);
    }

    private static void EncodeLanguageIndicator(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(8, bData.language);
    }

    private static void EncodeDisplayMode(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(2, bData.displayMode);
        outStream->Skip(6);
    }

    private static void EncodePriorityIndicator(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(2, bData.priority);
        outStream->Skip(6);
    }

    private static void EncodeMsgDeliveryAlert(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        outStream->Write(8, 1);
        outStream->Write(2, bData.alert);
        outStream->Skip(6);
    }

    private static void EncodeScpResults(BearerData bData, BitwiseOutputStream outStream)
        throws BitwiseOutputStream.AccessException
    {
        ArrayList<CdmaSmsCbProgramResults> results = bData.serviceCategoryProgramResults;
        outStream->Write(8, (results->Size() * 4));   // 4 octets per program result
        For (CdmaSmsCbProgramResults result : results) {
            Int32 category = result->GetCategory();
            outStream->Write(8, category >> 8);
            outStream->Write(8, category);
            outStream->Write(8, result->GetLanguage());
            outStream->Write(4, result->GetCategoryResult());
            outStream->Skip(4);
        }
    }

    /**
     * Create serialized representation for BearerData object.
     * (See 3GPP2 C.R1001-F, v1.0, section 4.5 for layout details)
     *
     * @param bData an instance of BearerData.
     *
     * @return Byte array of raw encoded SMS bearer data.
     */
    public static Byte[] Encode(BearerData bData) {
        bData.hasUserDataHeader = ((bData.userData != NULL) &&
                (bData.userData.userDataHeader != NULL));
        try {
            BitwiseOutputStream outStream = new BitwiseOutputStream(200);
            outStream->Write(8, SUBPARAM_MESSAGE_IDENTIFIER);
            EncodeMessageId(bData, outStream);
            If (bData.userData != NULL) {
                outStream->Write(8, SUBPARAM_USER_DATA);
                EncodeUserData(bData, outStream);
            }
            If (bData.callbackNumber != NULL) {
                outStream->Write(8, SUBPARAM_CALLBACK_NUMBER);
                EncodeCallbackNumber(bData, outStream);
            }
            If (bData.userAckReq || bData.deliveryAckReq || bData.readAckReq || bData.reportReq) {
                outStream->Write(8, SUBPARAM_REPLY_OPTION);
                EncodeReplyOption(bData, outStream);
            }
            If (bData.numberOfMessages != 0) {
                outStream->Write(8, SUBPARAM_NUMBER_OF_MESSAGES);
                EncodeMsgCount(bData, outStream);
            }
            If (bData.validityPeriodRelativeSet) {
                outStream->Write(8, SUBPARAM_VALIDITY_PERIOD_RELATIVE);
                EncodeValidityPeriodRel(bData, outStream);
            }
            If (bData.privacyIndicatorSet) {
                outStream->Write(8, SUBPARAM_PRIVACY_INDICATOR);
                EncodePrivacyIndicator(bData, outStream);
            }
            If (bData.languageIndicatorSet) {
                outStream->Write(8, SUBPARAM_LANGUAGE_INDICATOR);
                EncodeLanguageIndicator(bData, outStream);
            }
            If (bData.displayModeSet) {
                outStream->Write(8, SUBPARAM_MESSAGE_DISPLAY_MODE);
                EncodeDisplayMode(bData, outStream);
            }
            If (bData.priorityIndicatorSet) {
                outStream->Write(8, SUBPARAM_PRIORITY_INDICATOR);
                EncodePriorityIndicator(bData, outStream);
            }
            If (bData.alertIndicatorSet) {
                outStream->Write(8, SUBPARAM_ALERT_ON_MESSAGE_DELIVERY);
                EncodeMsgDeliveryAlert(bData, outStream);
            }
            If (bData.messageStatusSet) {
                outStream->Write(8, SUBPARAM_MESSAGE_STATUS);
                EncodeMsgStatus(bData, outStream);
            }
            If (bData.serviceCategoryProgramResults != NULL) {
                outStream->Write(8, SUBPARAM_SERVICE_CATEGORY_PROGRAM_RESULTS);
                EncodeScpResults(bData, outStream);
            }
            return outStream->ToByteArray();
        } Catch (BitwiseOutputStream.AccessException ex) {
            Rlog->E(LOG_TAG, "BearerData encode failed: " + ex);
        } Catch (CodingException ex) {
            Rlog->E(LOG_TAG, "BearerData encode failed: " + ex);
        }
        return NULL;
   }

    private static Boolean DecodeMessageId(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 3 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.messageType = inStream->Read(4);
            bData.messageId = inStream->Read(8) << 8;
            bData.messageId |= inStream->Read(8);
            bData.hasUserDataHeader = (inStream->Read(1) == 1);
            inStream->Skip(3);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "MESSAGE_IDENTIFIER decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        return decodeSuccess;
    }

    private static Boolean DecodeReserved(
            BearerData bData, BitwiseInputStream inStream, Int32 subparamId)
        throws BitwiseInputStream.AccessException, CodingException
    {
        Boolean decodeSuccess = FALSE;
        Int32 subparamLen = inStream->Read(8); // SUBPARAM_LEN
        Int32 paramBits = subparamLen * 8;
        If (paramBits <= inStream->Available()) {
            decodeSuccess = TRUE;
            inStream->Skip(paramBits);
        }
        Rlog->D(LOG_TAG, "RESERVED bearer data subparameter " + subparamId + " decode "
                + (decodeSuccess ? "succeeded" : "failed") + " (param bits = " + paramBits + ")");
        If (!decodeSuccess) {
            throw new CodingException("RESERVED bearer data subparameter " + subparamId
                    + " had invalid SUBPARAM_LEN " + subparamLen);
        }

        return decodeSuccess;
    }

    private static Boolean DecodeUserData(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException
    {
        Int32 paramBits = inStream->Read(8) * 8;
        bData.userData = new UserData();
        bData.userData.msgEncoding = inStream->Read(5);
        bData.userData.msgEncodingSet = TRUE;
        bData.userData.msgType = 0;
        Int32 consumedBits = 5;
        If ((bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) ||
            (bData.userData.msgEncoding == UserData.ENCODING_GSM_DCS)) {
            bData.userData.msgType = inStream->Read(8);
            consumedBits += 8;
        }
        bData.userData.numFields = inStream->Read(8);
        consumedBits += 8;
        Int32 dataBits = paramBits - consumedBits;
        bData.userData.payload = inStream->ReadByteArray(dataBits);
        return TRUE;
    }

    private static String DecodeUtf8(Byte[] data, Int32 offset, Int32 numFields)
        throws CodingException
    {
        return DecodeCharset(data, offset, numFields, 1, "UTF-8");
    }

    private static String DecodeUtf16(Byte[] data, Int32 offset, Int32 numFields)
        throws CodingException
    {
        // Subtract header and possible padding Byte (at end) from num fields.
        Int32 padding = offset % 2;
        numFields -= (offset + padding) / 2;
        return DecodeCharset(data, offset, numFields, 2, "utf-16be");
    }

    private static String DecodeCharset(Byte[] data, Int32 offset, Int32 numFields, Int32 width,
            String charset) throws CodingException
    {
        If (numFields < 0 || (numFields * width + offset) > data.length) {
            // Try to decode the max number of characters in payload
            Int32 padding = offset % width;
            Int32 maxNumFields = (data.length - offset - padding) / width;
            If (maxNumFields < 0) {
                throw new CodingException(charset + " decode failed: offset out of range");
            }
            Rlog->E(LOG_TAG, charset + " decode error: offset = " + offset + " numFields = "
                    + numFields + " data.length = " + data.length + " maxNumFields = "
                    + maxNumFields);
            numFields = maxNumFields;
        }
        try {
            return new String(data, offset, numFields * width, charset);
        } Catch (java.io.UnsupportedEncodingException ex) {
            throw new CodingException(charset + " decode failed: " + ex);
        }
    }

    private static String Decode7bitAscii(Byte[] data, Int32 offset, Int32 numFields)
        throws CodingException
    {
        try {
            Int32 offsetBits = offset * 8;
            Int32 offsetSeptets = (offsetBits + 6) / 7;
            numFields -= offsetSeptets;
            Int32 paddingBits = (offsetSeptets * 7) - offsetBits;

            StringBuffer strBuf = new StringBuffer(numFields);
            BitwiseInputStream inStream = new BitwiseInputStream(data);
            Int32 wantedBits = (offsetSeptets * 7) + (numFields * 7);
            If (inStream->Available() < wantedBits) {
                throw new CodingException("insufficient Data (wanted " + wantedBits +
                                          " bits, but only have " + inStream->Available() + ")");
            }
            inStream->Skip(offsetBits + paddingBits);
            For (Int32 i = 0; i < numFields; i++) {
                Int32 charCode = inStream->Read(7);
                If ((charCode >= UserData.ASCII_MAP_BASE_INDEX) &&
                        (charCode <= UserData.ASCII_MAP_MAX_INDEX)) {
                    strBuf->Append(UserData.ASCII_MAP[charCode - UserData.ASCII_MAP_BASE_INDEX]);
                } else If (charCode == UserData.ASCII_NL_INDEX) {
                    strBuf->Append('\n');
                } else If (charCode == UserData.ASCII_CR_INDEX) {
                    strBuf->Append('\r');
                } else {
                    /* For other charCodes, they are unprintable, and so simply use SPACE. */
                    strBuf->Append(' ');
                }
            }
            return strBuf->ToString();
        } Catch (BitwiseInputStream.AccessException ex) {
            throw new CodingException("7bit ASCII decode failed: " + ex);
        }
    }

    private static String Decode7bitGsm(Byte[] data, Int32 offset, Int32 numFields)
        throws CodingException
    {
        // Start reading from the next 7-bit aligned boundary after offset.
        Int32 offsetBits = offset * 8;
        Int32 offsetSeptets = (offsetBits + 6) / 7;
        numFields -= offsetSeptets;
        Int32 paddingBits = (offsetSeptets * 7) - offsetBits;
        String result = GsmAlphabet->Gsm7BitPackedToString(data, offset, numFields, paddingBits,
                0, 0);
        If (result == NULL) {
            throw new CodingException("7bit GSM decoding failed");
        }
        return result;
    }

    private static String DecodeLatin(Byte[] data, Int32 offset, Int32 numFields)
        throws CodingException
    {
        return DecodeCharset(data, offset, numFields, 1, "ISO-8859-1");
    }

    private static String DecodeShiftJis(Byte[] data, Int32 offset, Int32 numFields)
        throws CodingException
    {
        return DecodeCharset(data, offset, numFields, 1, "Shift_JIS");
    }

    private static String DecodeGsmDcs(Byte[] data, Int32 offset, Int32 numFields, Int32 msgType)
            throws CodingException
    {
        Switch ((msgType >> 2) & 0x3) {
        case UserData.ENCODING_GSM_DCS_7BIT:
            return Decode7bitGsm(data, offset, numFields);
        case UserData.ENCODING_GSM_DCS_8BIT:
            return DecodeUtf8(data, offset, numFields);
        case UserData.ENCODING_GSM_DCS_16BIT:
            return DecodeUtf16(data, offset, numFields);
        default:
            throw new CodingException("unsupported user msgType Encoding ("
                    + msgType + ")");
        }
    }

    private static void DecodeUserDataPayload(UserData userData, Boolean hasUserDataHeader)
        throws CodingException
    {
        Int32 offset = 0;
        If (hasUserDataHeader) {
            Int32 udhLen = userData.payload[0] & 0x00FF;
            offset += udhLen + 1;
            Byte[] headerData = new Byte[udhLen];
            System->Arraycopy(userData.payload, 1, headerData, 0, udhLen);
            userData.userDataHeader = SmsHeader->FromByteArray(headerData);
        }
        Switch (userData.msgEncoding) {
        case UserData.ENCODING_OCTET:
            /*
            *  Octet decoding depends on the carrier service.
            */
            Boolean decodingtypeUTF8 = Resources->GetSystem()
                    .GetBoolean(R.bool.config_sms_utf8_support);

            // Strip off any padding bytes, meaning any differences between the length of the
            // array and the target length specified by numFields.  This is to avoid any
            // confusion by code elsewhere that only considers the payload array length.
            Byte[] payload = new Byte[userData.numFields];
            Int32 copyLen = userData.numFields < userData.payload.length
                    ? userData.numFields : userData.payload.length;

            System->Arraycopy(userData.payload, 0, payload, 0, copyLen);
            userData.payload = payload;

            If (!decodingtypeUTF8) {
                // There are many devices in the market that send 8bit text Sms (latin encoded) as
                // octet encoded.
                userData.payloadStr = DecodeLatin(userData.payload, offset, userData.numFields);
            } else {
                userData.payloadStr = DecodeUtf8(userData.payload, offset, userData.numFields);
            }
            break;

        case UserData.ENCODING_IA5:
        case UserData.ENCODING_7BIT_ASCII:
            userData.payloadStr = Decode7bitAscii(userData.payload, offset, userData.numFields);
            break;
        case UserData.ENCODING_UNICODE_16:
            userData.payloadStr = DecodeUtf16(userData.payload, offset, userData.numFields);
            break;
        case UserData.ENCODING_GSM_7BIT_ALPHABET:
            userData.payloadStr = Decode7bitGsm(userData.payload, offset, userData.numFields);
            break;
        case UserData.ENCODING_LATIN:
            userData.payloadStr = DecodeLatin(userData.payload, offset, userData.numFields);
            break;
        case UserData.ENCODING_SHIFT_JIS:
            userData.payloadStr = DecodeShiftJis(userData.payload, offset, userData.numFields);
            break;
        case UserData.ENCODING_GSM_DCS:
            userData.payloadStr = DecodeGsmDcs(userData.payload, offset,
                    userData.numFields, userData.msgType);
            break;
        default:
            throw new CodingException("unsupported user data Encoding ("
                                      + userData.msgEncoding + ")");
        }
    }

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
    private static void DecodeIs91VoicemailStatus(BearerData bData)
        throws BitwiseInputStream.AccessException, CodingException
    {
        BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
        Int32 dataLen = inStream->Available() / 6;  // 6-bit packed character encoding.
        Int32 numFields = bData.userData.numFields;
        If ((dataLen > 14) || (dataLen < 3) || (dataLen < numFields)) {
            throw new CodingException("IS-91 voicemail status decoding failed");
        }
        try {
            StringBuffer strbuf = new StringBuffer(dataLen);
            While (inStream->Available() >= 6) {
                strbuf->Append(UserData.ASCII_MAP[inStream->Read(6)]);
            }
            String data = strbuf->ToString();
            bData.numberOfMessages = Integer->ParseInt(data->Substring(0, 2));
            Char32 prioCode = data->CharAt(2);
            If (prioCode == ' ') {
                bData.priority = PRIORITY_NORMAL;
            } else If (prioCode == '!') {
                bData.priority = PRIORITY_URGENT;
            } else {
                throw new CodingException("IS-91 voicemail status decoding failed: " +
                        "illegal priority Setting (" + prioCode + ")");
            }
            bData.priorityIndicatorSet = TRUE;
            bData.userData.payloadStr = data->Substring(3, numFields - 3);
       } Catch (java.lang.NumberFormatException ex) {
            throw new CodingException("IS-91 voicemail status decoding failed: " + ex);
        } Catch (java.lang.IndexOutOfBoundsException ex) {
            throw new CodingException("IS-91 voicemail status decoding failed: " + ex);
        }
    }

    /**
     * IS-91 Short Message decoding
     * (See 3GPP2 C.S0015-A, Table 4.3.1.4.1-1)
     * (For character encodings, see TIA/EIA/IS-91, Annex B)
     *
     * Protocol Summary: The user data payload may contain 1-14
     * characters, which are treated as normal text user data payload.
     * Note that the characters encoding is 6-bit packed.
     */
    private static void DecodeIs91ShortMessage(BearerData bData)
        throws BitwiseInputStream.AccessException, CodingException
    {
        BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
        Int32 dataLen = inStream->Available() / 6;  // 6-bit packed character encoding.
        Int32 numFields = bData.userData.numFields;
        // dataLen may be > 14 characters due to octet padding
        If ((numFields > 14) || (dataLen < numFields)) {
            throw new CodingException("IS-91 short message decoding failed");
        }
        StringBuffer strbuf = new StringBuffer(dataLen);
        For (Int32 i = 0; i < numFields; i++) {
            strbuf->Append(UserData.ASCII_MAP[inStream->Read(6)]);
        }
        bData.userData.payloadStr = strbuf->ToString();
    }

    /**
     * IS-91 CLI Message (callback number) decoding
     * (See 3GPP2 C.S0015-A, Table 4.3.1.4.1-1)
     *
     * Protocol Summary: The data payload may contain 1-32 digits,
     * encoded using standard 4-bit DTMF, which are treated as a
     * callback number.
     */
    private static void DecodeIs91Cli(BearerData bData) throws CodingException {
        BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
        Int32 dataLen = inStream->Available() / 4;  // 4-bit packed DTMF digit encoding.
        Int32 numFields = bData.userData.numFields;
        If ((dataLen > 14) || (dataLen < 3) || (dataLen < numFields)) {
            throw new CodingException("IS-91 voicemail status decoding failed");
        }
        CdmaSmsAddress addr = new CdmaSmsAddress();
        addr.digitMode = CdmaSmsAddress.DIGIT_MODE_4BIT_DTMF;
        addr.origBytes = bData.userData.payload;
        addr.numberOfDigits = (Byte)numFields;
        DecodeSmsAddress(addr);
        bData.callbackNumber = addr;
    }

    private static void DecodeIs91(BearerData bData)
        throws BitwiseInputStream.AccessException, CodingException
    {
        Switch (bData.userData.msgType) {
        case UserData.IS91_MSG_TYPE_VOICEMAIL_STATUS:
            DecodeIs91VoicemailStatus(bData);
            break;
        case UserData.IS91_MSG_TYPE_CLI:
            DecodeIs91Cli(bData);
            break;
        case UserData.IS91_MSG_TYPE_SHORT_MESSAGE_FULL:
        case UserData.IS91_MSG_TYPE_SHORT_MESSAGE:
            DecodeIs91ShortMessage(bData);
            break;
        default:
            throw new CodingException("unsupported IS-91 message Type (" +
                    bData.userData.msgType + ")");
        }
    }

    private static Boolean DecodeReplyOption(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.userAckReq     = (inStream->Read(1) == 1);
            bData.deliveryAckReq = (inStream->Read(1) == 1);
            bData.readAckReq     = (inStream->Read(1) == 1);
            bData.reportReq      = (inStream->Read(1) == 1);
            inStream->Skip(4);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "REPLY_OPTION decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        return decodeSuccess;
    }

    private static Boolean DecodeMsgCount(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.numberOfMessages = IccUtils->CdmaBcdByteToInt((Byte)inStream->Read(8));
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "NUMBER_OF_MESSAGES decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        return decodeSuccess;
    }

    private static Boolean DecodeDepositIndex(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 2 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.depositIndex = (inStream->Read(8) << 8) | inStream->Read(8);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "MESSAGE_DEPOSIT_INDEX decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        return decodeSuccess;
    }

    private static String DecodeDtmfSmsAddress(Byte[] rawData, Int32 numFields)
        throws CodingException
    {
        /* DTMF 4-bit digit encoding, defined in at
         * 3GPP2 C.S005-D, v2.0, table 2.7.1.3.2.4-4 */
        StringBuffer strBuf = new StringBuffer(numFields);
        For (Int32 i = 0; i < numFields; i++) {
            Int32 val = 0x0F & (rawData[i / 2] >>> (4 - ((i % 2) * 4)));
            If ((val >= 1) && (val <= 9)) strBuf->Append(Integer->ToString(val, 10));
            else If (val == 10) strBuf->Append('0');
            else If (val == 11) strBuf->Append('*');
            else If (val == 12) strBuf->Append('#');
            else throw new CodingException("invalid SMS address DTMF Code (" + val + ")");
        }
        return strBuf->ToString();
    }

    private static void DecodeSmsAddress(CdmaSmsAddress addr) throws CodingException {
        If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
            try {
                /* As specified in 3GPP2 C.S0015-B, v2, 4.5.15 -- actually
                 * just 7-bit ASCII encoding, with the MSB being zero. */
                addr.address = new String(addr.origBytes, 0, addr.origBytes.length, "US-ASCII");
            } Catch (java.io.UnsupportedEncodingException ex) {
                throw new CodingException("invalid SMS address ASCII code");
            }
        } else {
            addr.address = DecodeDtmfSmsAddress(addr.origBytes, addr.numberOfDigits);
        }
    }

    private static Boolean DecodeCallbackNumber(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException, CodingException
    {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8; //at least
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits < EXPECTED_PARAM_SIZE) {
            inStream->Skip(paramBits);
            return FALSE;
        }
        CdmaSmsAddress addr = new CdmaSmsAddress();
        addr.digitMode = inStream->Read(1);
        Byte fieldBits = 4;
        Byte consumedBits = 1;
        If (addr.digitMode == CdmaSmsAddress.DIGIT_MODE_8BIT_CHAR) {
            addr.ton = inStream->Read(3);
            addr.numberPlan = inStream->Read(4);
            fieldBits = 8;
            consumedBits += 7;
        }
        addr.numberOfDigits = inStream->Read(8);
        consumedBits += 8;
        Int32 remainingBits = paramBits - consumedBits;
        Int32 dataBits = addr.numberOfDigits * fieldBits;
        Int32 paddingBits = remainingBits - dataBits;
        If (remainingBits < dataBits) {
            throw new CodingException("CALLBACK_NUMBER subparam encoding size Error (" +
                                      "remainingBits + " + remainingBits + ", dataBits + " +
                                      dataBits + ", paddingBits + " + paddingBits + ")");
        }
        addr.origBytes = inStream->ReadByteArray(dataBits);
        inStream->Skip(paddingBits);
        DecodeSmsAddress(addr);
        bData.callbackNumber = addr;
        return TRUE;
    }

    private static Boolean DecodeMsgStatus(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.errorClass = inStream->Read(2);
            bData.messageStatus = inStream->Read(6);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "MESSAGE_STATUS decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.messageStatusSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodeMsgCenterTimeStamp(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 6 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.msgCenterTimeStamp = TimeStamp->FromByteArray(inStream->ReadByteArray(6 * 8));
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "MESSAGE_CENTER_TIME_STAMP decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        return decodeSuccess;
    }

    private static Boolean DecodeValidityAbs(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 6 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.validityPeriodAbsolute = TimeStamp->FromByteArray(inStream->ReadByteArray(6 * 8));
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "VALIDITY_PERIOD_ABSOLUTE decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        return decodeSuccess;
    }

    private static Boolean DecodeDeferredDeliveryAbs(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 6 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.deferredDeliveryTimeAbsolute = TimeStamp->FromByteArray(
                    inStream->ReadByteArray(6 * 8));
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "DEFERRED_DELIVERY_TIME_ABSOLUTE decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        return decodeSuccess;
    }

    private static Boolean DecodeValidityRel(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.deferredDeliveryTimeRelative = inStream->Read(8);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "VALIDITY_PERIOD_RELATIVE decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.deferredDeliveryTimeRelativeSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodeDeferredDeliveryRel(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.validityPeriodRelative = inStream->Read(8);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "DEFERRED_DELIVERY_TIME_RELATIVE decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.validityPeriodRelativeSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodePrivacyIndicator(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.privacy = inStream->Read(2);
            inStream->Skip(6);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "PRIVACY_INDICATOR decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.privacyIndicatorSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodeLanguageIndicator(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.language = inStream->Read(8);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "LANGUAGE_INDICATOR decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.languageIndicatorSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodeDisplayMode(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.displayMode = inStream->Read(2);
            inStream->Skip(6);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "DISPLAY_MODE decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.displayModeSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodePriorityIndicator(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.priority = inStream->Read(2);
            inStream->Skip(6);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "PRIORITY_INDICATOR decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.priorityIndicatorSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodeMsgDeliveryAlert(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.alert = inStream->Read(2);
            inStream->Skip(6);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "ALERT_ON_MESSAGE_DELIVERY decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.alertIndicatorSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodeUserResponseCode(BearerData bData, BitwiseInputStream inStream)
        throws BitwiseInputStream.AccessException {
        final Int32 EXPECTED_PARAM_SIZE = 1 * 8;
        Boolean decodeSuccess = FALSE;
        Int32 paramBits = inStream->Read(8) * 8;
        If (paramBits >= EXPECTED_PARAM_SIZE) {
            paramBits -= EXPECTED_PARAM_SIZE;
            decodeSuccess = TRUE;
            bData.userResponseCode = inStream->Read(8);
        }
        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "USER_RESPONSE_CODE decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ")");
        }
        inStream->Skip(paramBits);
        bData.userResponseCodeSet = decodeSuccess;
        return decodeSuccess;
    }

    private static Boolean DecodeServiceCategoryProgramData(BearerData bData,
            BitwiseInputStream inStream) throws BitwiseInputStream.AccessException, CodingException
    {
        If (inStream->Available() < 13) {
            throw new CodingException("SERVICE_CATEGORY_PROGRAM_DATA decode failed: only "
                    + inStream->Available() + " bits available");
        }

        Int32 paramBits = inStream->Read(8) * 8;
        Int32 msgEncoding = inStream->Read(5);
        paramBits -= 5;

        If (inStream->Available() < paramBits) {
            throw new CodingException("SERVICE_CATEGORY_PROGRAM_DATA decode failed: only "
                    + inStream->Available() + " bits Available (" + paramBits + " bits expected)");
        }

        ArrayList<CdmaSmsCbProgramData> programDataList = new ArrayList<CdmaSmsCbProgramData>();

        final Int32 CATEGORY_FIELD_MIN_SIZE = 6 * 8;
        Boolean decodeSuccess = FALSE;
        While (paramBits >= CATEGORY_FIELD_MIN_SIZE) {
            Int32 operation = inStream->Read(4);
            Int32 category = (inStream->Read(8) << 8) | inStream->Read(8);
            Int32 language = inStream->Read(8);
            Int32 maxMessages = inStream->Read(8);
            Int32 alertOption = inStream->Read(4);
            Int32 numFields = inStream->Read(8);
            paramBits -= CATEGORY_FIELD_MIN_SIZE;

            Int32 textBits = GetBitsForNumFields(msgEncoding, numFields);
            If (paramBits < textBits) {
                throw new CodingException("category name is " + textBits + " bits in length,"
                        + " but there are only " + paramBits + " bits available");
            }

            UserData userData = new UserData();
            userData.msgEncoding = msgEncoding;
            userData.msgEncodingSet = TRUE;
            userData.numFields = numFields;
            userData.payload = inStream->ReadByteArray(textBits);
            paramBits -= textBits;

            DecodeUserDataPayload(userData, FALSE);
            String categoryName = userData.payloadStr;
            CdmaSmsCbProgramData programData = new CdmaSmsCbProgramData(operation, category,
                    language, maxMessages, alertOption, categoryName);
            programDataList->Add(programData);

            decodeSuccess = TRUE;
        }

        If ((! decodeSuccess) || (paramBits > 0)) {
            Rlog->D(LOG_TAG, "SERVICE_CATEGORY_PROGRAM_DATA decode " +
                      (decodeSuccess ? "succeeded" : "failed") +
                      " (extra bits = " + paramBits + ')');
        }

        inStream->Skip(paramBits);
        bData.serviceCategoryProgramData = programDataList;
        return decodeSuccess;
    }

    private static Int32 ServiceCategoryToCmasMessageClass(Int32 serviceCategory) {
        Switch (serviceCategory) {
            case SmsEnvelope.SERVICE_CATEGORY_CMAS_PRESIDENTIAL_LEVEL_ALERT:
                return SmsCbCmasInfo.CMAS_CLASS_PRESIDENTIAL_LEVEL_ALERT;

            case SmsEnvelope.SERVICE_CATEGORY_CMAS_EXTREME_THREAT:
                return SmsCbCmasInfo.CMAS_CLASS_EXTREME_THREAT;

            case SmsEnvelope.SERVICE_CATEGORY_CMAS_SEVERE_THREAT:
                return SmsCbCmasInfo.CMAS_CLASS_SEVERE_THREAT;

            case SmsEnvelope.SERVICE_CATEGORY_CMAS_CHILD_ABDUCTION_EMERGENCY:
                return SmsCbCmasInfo.CMAS_CLASS_CHILD_ABDUCTION_EMERGENCY;

            case SmsEnvelope.SERVICE_CATEGORY_CMAS_TEST_MESSAGE:
                return SmsCbCmasInfo.CMAS_CLASS_REQUIRED_MONTHLY_TEST;

            default:
                return SmsCbCmasInfo.CMAS_CLASS_UNKNOWN;
        }
    }

    /**
     * Calculates the number of bits to read for the specified number of encoded characters.
     * @param msgEncoding the message encoding to use
     * @param numFields the number of characters to read. For Shift-JIS and Korean encodings,
     *  this is the number of bytes to read.
     * @return the number of bits to read from the stream
     * @throws CodingException if the specified encoding is not supported
     */
    private static Int32 GetBitsForNumFields(Int32 msgEncoding, Int32 numFields)
            throws CodingException {
        Switch (msgEncoding) {
            case UserData.ENCODING_OCTET:
            case UserData.ENCODING_SHIFT_JIS:
            case UserData.ENCODING_KOREAN:
            case UserData.ENCODING_LATIN:
            case UserData.ENCODING_LATIN_HEBREW:
                return numFields * 8;

            case UserData.ENCODING_IA5:
            case UserData.ENCODING_7BIT_ASCII:
            case UserData.ENCODING_GSM_7BIT_ALPHABET:
                return numFields * 7;

            case UserData.ENCODING_UNICODE_16:
                return numFields * 16;

            default:
                throw new CodingException("unsupported message Encoding (" + msgEncoding + ')');
        }
    }

    /**
     * CMAS message decoding.
     * (See TIA-1149-0-1, CMAS over CDMA)
     *
     * @param serviceCategory is the service category from the SMS envelope
     */
    private static void DecodeCmasUserData(BearerData bData, Int32 serviceCategory)
            throws BitwiseInputStream.AccessException, CodingException {
        BitwiseInputStream inStream = new BitwiseInputStream(bData.userData.payload);
        If (inStream->Available() < 8) {
            throw new CodingException("emergency CB with no CMAE_protocol_version");
        }
        Int32 protocolVersion = inStream->Read(8);
        If (protocolVersion != 0) {
            throw new CodingException("unsupported CMAE_protocol_version " + protocolVersion);
        }

        Int32 messageClass = ServiceCategoryToCmasMessageClass(serviceCategory);
        Int32 category = SmsCbCmasInfo.CMAS_CATEGORY_UNKNOWN;
        Int32 responseType = SmsCbCmasInfo.CMAS_RESPONSE_TYPE_UNKNOWN;
        Int32 severity = SmsCbCmasInfo.CMAS_SEVERITY_UNKNOWN;
        Int32 urgency = SmsCbCmasInfo.CMAS_URGENCY_UNKNOWN;
        Int32 certainty = SmsCbCmasInfo.CMAS_CERTAINTY_UNKNOWN;

        While (inStream->Available() >= 16) {
            Int32 recordType = inStream->Read(8);
            Int32 recordLen = inStream->Read(8);
            Switch (recordType) {
                case 0:     // Type 0 Elements (Alert text)
                    UserData alertUserData = new UserData();
                    alertUserData.msgEncoding = inStream->Read(5);
                    alertUserData.msgEncodingSet = TRUE;
                    alertUserData.msgType = 0;

                    Int32 numFields;                          // number of chars to decode
                    Switch (alertUserData.msgEncoding) {
                        case UserData.ENCODING_OCTET:
                        case UserData.ENCODING_LATIN:
                            numFields = recordLen - 1;      // subtract 1 Byte for encoding
                            break;

                        case UserData.ENCODING_IA5:
                        case UserData.ENCODING_7BIT_ASCII:
                        case UserData.ENCODING_GSM_7BIT_ALPHABET:
                            numFields = ((recordLen * 8) - 5) / 7;  // subtract 5 bits for encoding
                            break;

                        case UserData.ENCODING_UNICODE_16:
                            numFields = (recordLen - 1) / 2;
                            break;

                        default:
                            numFields = 0;      // unsupported encoding
                    }

                    alertUserData.numFields = numFields;
                    alertUserData.payload = inStream->ReadByteArray(recordLen * 8 - 5);
                    DecodeUserDataPayload(alertUserData, FALSE);
                    bData.userData = alertUserData;
                    break;

                case 1:     // Type 1 elements
                    category = inStream->Read(8);
                    responseType = inStream->Read(8);
                    severity = inStream->Read(4);
                    urgency = inStream->Read(4);
                    certainty = inStream->Read(4);
                    inStream->Skip(recordLen * 8 - 28);
                    break;

                default:
                    Rlog->W(LOG_TAG, "skipping unsupported CMAS record type " + recordType);
                    inStream->Skip(recordLen * 8);
                    break;
            }
        }

        bData.cmasWarningInfo = new SmsCbCmasInfo(messageClass, category, responseType, severity,
                urgency, certainty);
    }

    /**
     * Create BearerData object from serialized representation.
     * (See 3GPP2 C.R1001-F, v1.0, section 4.5 for layout details)
     *
     * @param smsData Byte array of raw encoded SMS bearer data.
     * @return an instance of BearerData.
     */
    public static BearerData Decode(Byte[] smsData) {
        return Decode(smsData, 0);
    }

    private static Boolean IsCmasAlertCategory(Int32 category) {
        return category >= SmsEnvelope.SERVICE_CATEGORY_CMAS_PRESIDENTIAL_LEVEL_ALERT
                && category <= SmsEnvelope.SERVICE_CATEGORY_CMAS_LAST_RESERVED_VALUE;
    }

    /**
     * Create BearerData object from serialized representation.
     * (See 3GPP2 C.R1001-F, v1.0, section 4.5 for layout details)
     *
     * @param smsData Byte array of raw encoded SMS bearer data.
     * @param serviceCategory the envelope service Category (for CMAS alert handling)
     * @return an instance of BearerData.
     */
    public static BearerData Decode(Byte[] smsData, Int32 serviceCategory) {
        try {
            BitwiseInputStream inStream = new BitwiseInputStream(smsData);
            BearerData bData = new BearerData();
            Int32 foundSubparamMask = 0;
            While (inStream->Available() > 0) {
                Int32 subparamId = inStream->Read(8);
                Int32 subparamIdBit = 1 << subparamId;
                // Int32 is 4 bytes. This duplicate check has a limit to Id number up to 32 (4*8)
                // as 32th bit is the max bit in Int32.
                // Per 3GPP2 C.S0015-B Table 4.5-1 Bearer Data Subparameter Identifiers:
                // last defined subparam ID is 23 (00010111 = 0x17 = 23).
                // Only do duplicate subparam ID check if subparam is within defined value as
                // reserved subparams are just skipped.
                If ((foundSubparamMask & subparamIdBit) != 0 &&
                        (subparamId >= SUBPARAM_MESSAGE_IDENTIFIER &&
                        subparamId <= SUBPARAM_ID_LAST_DEFINED)) {
                    throw new CodingException("illegal duplicate Subparameter (" +
                                              subparamId + ")");
                }
                Boolean decodeSuccess;
                Switch (subparamId) {
                case SUBPARAM_MESSAGE_IDENTIFIER:
                    decodeSuccess = DecodeMessageId(bData, inStream);
                    break;
                case SUBPARAM_USER_DATA:
                    decodeSuccess = DecodeUserData(bData, inStream);
                    break;
                case SUBPARAM_USER_RESPONSE_CODE:
                    decodeSuccess = DecodeUserResponseCode(bData, inStream);
                    break;
                case SUBPARAM_REPLY_OPTION:
                    decodeSuccess = DecodeReplyOption(bData, inStream);
                    break;
                case SUBPARAM_NUMBER_OF_MESSAGES:
                    decodeSuccess = DecodeMsgCount(bData, inStream);
                    break;
                case SUBPARAM_CALLBACK_NUMBER:
                    decodeSuccess = DecodeCallbackNumber(bData, inStream);
                    break;
                case SUBPARAM_MESSAGE_STATUS:
                    decodeSuccess = DecodeMsgStatus(bData, inStream);
                    break;
                case SUBPARAM_MESSAGE_CENTER_TIME_STAMP:
                    decodeSuccess = DecodeMsgCenterTimeStamp(bData, inStream);
                    break;
                case SUBPARAM_VALIDITY_PERIOD_ABSOLUTE:
                    decodeSuccess = DecodeValidityAbs(bData, inStream);
                    break;
                case SUBPARAM_VALIDITY_PERIOD_RELATIVE:
                    decodeSuccess = DecodeValidityRel(bData, inStream);
                    break;
                case SUBPARAM_DEFERRED_DELIVERY_TIME_ABSOLUTE:
                    decodeSuccess = DecodeDeferredDeliveryAbs(bData, inStream);
                    break;
                case SUBPARAM_DEFERRED_DELIVERY_TIME_RELATIVE:
                    decodeSuccess = DecodeDeferredDeliveryRel(bData, inStream);
                    break;
                case SUBPARAM_PRIVACY_INDICATOR:
                    decodeSuccess = DecodePrivacyIndicator(bData, inStream);
                    break;
                case SUBPARAM_LANGUAGE_INDICATOR:
                    decodeSuccess = DecodeLanguageIndicator(bData, inStream);
                    break;
                case SUBPARAM_MESSAGE_DISPLAY_MODE:
                    decodeSuccess = DecodeDisplayMode(bData, inStream);
                    break;
                case SUBPARAM_PRIORITY_INDICATOR:
                    decodeSuccess = DecodePriorityIndicator(bData, inStream);
                    break;
                case SUBPARAM_ALERT_ON_MESSAGE_DELIVERY:
                    decodeSuccess = DecodeMsgDeliveryAlert(bData, inStream);
                    break;
                case SUBPARAM_MESSAGE_DEPOSIT_INDEX:
                    decodeSuccess = DecodeDepositIndex(bData, inStream);
                    break;
                case SUBPARAM_SERVICE_CATEGORY_PROGRAM_DATA:
                    decodeSuccess = DecodeServiceCategoryProgramData(bData, inStream);
                    break;
                default:
                    decodeSuccess = DecodeReserved(bData, inStream, subparamId);
                }
                If (decodeSuccess &&
                        (subparamId >= SUBPARAM_MESSAGE_IDENTIFIER &&
                        subparamId <= SUBPARAM_ID_LAST_DEFINED)) {
                    foundSubparamMask |= subparamIdBit;
                }
            }
            If ((foundSubparamMask & (1 << SUBPARAM_MESSAGE_IDENTIFIER)) == 0) {
                throw new CodingException("missing MESSAGE_IDENTIFIER subparam");
            }
            If (bData.userData != NULL) {
                If (IsCmasAlertCategory(serviceCategory)) {
                    DecodeCmasUserData(bData, serviceCategory);
                } else If (bData.userData.msgEncoding == UserData.ENCODING_IS91_EXTENDED_PROTOCOL) {
                    If ((foundSubparamMask ^
                             (1 << SUBPARAM_MESSAGE_IDENTIFIER) ^
                             (1 << SUBPARAM_USER_DATA))
                            != 0) {
                        Rlog->E(LOG_TAG, "IS-91 must occur without extra Subparams (" +
                              foundSubparamMask + ")");
                    }
                    DecodeIs91(bData);
                } else {
                    DecodeUserDataPayload(bData.userData, bData.hasUserDataHeader);
                }
            }
            return bData;
        } Catch (BitwiseInputStream.AccessException ex) {
            Rlog->E(LOG_TAG, "BearerData decode failed: " + ex);
        } Catch (CodingException ex) {
            Rlog->E(LOG_TAG, "BearerData decode failed: " + ex);
        }
        return NULL;
    }
}
