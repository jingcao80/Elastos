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

package android.telephony;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::GsmAlphabet::ITextEncodingDetails;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::SmsMessageBase::ISubmitPduBase;
using Elastos::Droid::Internal::Telephony::ISms7BitEncodingTranslator;

using Elastos::Lang::IMath;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;

using static android::Telephony::TelephonyManager::IPHONE_TYPE_CDMA;


/**
 * A Short Message Service message.
 * @see android.provider.Telephony.Sms.Intents#getMessagesFromIntent
 */
public class SmsMessage {
    private static const String LOG_TAG = "SmsMessage";

    /**
     * SMS Class enumeration.
     * See TS 23.038.
     *
     */
    public enum MessageClass{
        UNKNOWN, CLASS_0, CLASS_1, CLASS_2, CLASS_3;
    }

    /** User data text encoding code unit size */
    public static const Int32 ENCODING_UNKNOWN = 0;
    public static const Int32 ENCODING_7BIT = 1;
    public static const Int32 ENCODING_8BIT = 2;
    public static const Int32 ENCODING_16BIT = 3;
    /**
     * @hide This value is not defined in global standard. Only in Korea, this is used.
     */
    public static const Int32 ENCODING_KSC5601 = 4;

    /** The maximum number of payload bytes per message */
    public static const Int32 MAX_USER_DATA_BYTES = 140;

    /**
     * The maximum number of payload bytes per message if a user data header
     * is present.  This assumes the header only contains the
     * CONCATENATED_8_BIT_REFERENCE element.
     */
    public static const Int32 MAX_USER_DATA_BYTES_WITH_HEADER = 134;

    /** The maximum number of payload septets per message */
    public static const Int32 MAX_USER_DATA_SEPTETS = 160;

    /**
     * The maximum number of payload septets per message if a user data header
     * is present.  This assumes the header only contains the
     * CONCATENATED_8_BIT_REFERENCE element.
     */
    public static const Int32 MAX_USER_DATA_SEPTETS_WITH_HEADER = 153;

    /**
     * Indicates a 3GPP format SMS message.
     * @hide pending API council approval
     */
    public static const String FORMAT_3GPP = "3gpp";

    /**
     * Indicates a 3GPP2 format SMS message.
     * @hide pending API council approval
     */
    public static const String FORMAT_3GPP2 = "3gpp2";

    /**
     * Indicates a synthetic SMS message.
     * @hide
     */
    public static const String FORMAT_SYNTHETIC = "synthetic";

    /** Contains actual SmsMessage. Only public for debugging and for framework layer.
     *
     * @hide
     */
    public SmsMessageBase mWrappedSmsMessage;

    /** Indicates the subId
     *
     * @hide
     */
    private Int64 mSubId = 0;

    /** set Subscription information
     *
     * @hide
     */
    CARAPI SetSubId(Int64 subId) {
        mSubId = subId;
    }

    /** get Subscription information
     *
     * @hide
     */
    public Int64 GetSubId() {
        return mSubId;
    }

    public static class SubmitPdu {

        public Byte[] encodedScAddress; // Null if not applicable.
        public Byte[] encodedMessage;

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "SubmitPdu: encodedScAddress = "
                    + Arrays->ToString(encodedScAddress)
                    + ", encodedMessage = "
                    + Arrays->ToString(encodedMessage);
        }

        /**
         * @hide
         */
        protected SubmitPdu(SubmitPduBase spb) {
            this.encodedMessage = spb.encodedMessage;
            this.encodedScAddress = spb.encodedScAddress;
        }

    }

    private SmsMessage(SmsMessageBase smb) {
        mWrappedSmsMessage = smb;
    }

    /**
     * Create an SmsMessage from a raw PDU.
     *
     * <p><b>This method will soon be deprecated</b> and all applications which handle
     * incoming SMS messages by processing the {@code SMS_RECEIVED_ACTION} broadcast
     * intent <b>must</b> now pass the new {@code format} String extra from the intent
     * into the new method {@code CreateFromPdu(Byte[], String)} which takes an
     * extra format parameter. This is required in order to correctly decode the PDU on
     * devices that require support for both 3GPP and 3GPP2 formats at the same time,
     * such as dual-mode GSM/CDMA and CDMA/LTE phones.  Guess format based on Voice
     * technology first, if it fails use other format.
     */
    public static SmsMessage CreateFromPdu(Byte[] pdu) {
         SmsMessage message = NULL;

        // Cdma(3gpp2) vs Gsm(3gpp) format info was not given,
        // guess from active voice phone type
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();
        String format = (PHONE_TYPE_CDMA == activePhone) ?
                SmsConstants.FORMAT_3GPP2 : SmsConstants.FORMAT_3GPP;
        If (com.android.internal.telephony.SyntheticSmsMessage->IsSyntheticPdu(pdu)) {
            format = FORMAT_SYNTHETIC;
        }
        message = CreateFromPdu(pdu, format);

        If (NULL == message || NULL == message.mWrappedSmsMessage) {
            // decoding pdu failed based on activePhone type, must be other format
            format = (PHONE_TYPE_CDMA == activePhone) ?
                    SmsConstants.FORMAT_3GPP : SmsConstants.FORMAT_3GPP2;
            message = CreateFromPdu(pdu, format);
        }
        return message;
    }

    /**
     * Create an SmsMessage from a raw PDU with the specified message format. The
     * message format is passed in the {@code SMS_RECEIVED_ACTION} as the {@code format}
     * String extra, and will be either "3gpp" for GSM/UMTS/LTE messages in 3GPP format
     * or "3gpp2" for CDMA/LTE messages in 3GPP2 format.
     *
     * @param pdu the message PDU from the SMS_RECEIVED_ACTION intent
     * @param format the format extra from the SMS_RECEIVED_ACTION intent
     * @hide pending API council approval
     */
    public static SmsMessage CreateFromPdu(Byte[] pdu, String format) {
        SmsMessageBase wrappedMessage;

        If (SmsConstants.FORMAT_3GPP2->Equals(format)) {
            wrappedMessage = com.android.internal.telephony.cdma.SmsMessage->CreateFromPdu(pdu);
        } else If (SmsConstants.FORMAT_3GPP->Equals(format)) {
            wrappedMessage = com.android.internal.telephony.gsm.SmsMessage->CreateFromPdu(pdu);
        } else If (FORMAT_SYNTHETIC->Equals(format)) {
            wrappedMessage = com.android.internal.telephony.SyntheticSmsMessage->CreateFromPdu(pdu);
        } else {
            Rlog->E(LOG_TAG, "CreateFromPdu(): unsupported message format " + format);
            return NULL;
        }

        return new SmsMessage(wrappedMessage);
    }

    /**
     * TS 27.005 3.4.1 lines[0] and lines[1] are the two lines read from the
     * +CMT unsolicited Response (PDU mode, of course)
     *  +CMT: [&lt;alpha>],<length><CR><LF><pdu>
     *
     * Only public for debugging and for RIL
     *
     * {@hide}
     */
    public static SmsMessage NewFromCMT(String[] lines) {
        // received SMS in 3GPP format
        SmsMessageBase wrappedMessage =
                com.android.internal.telephony.gsm.SmsMessage->NewFromCMT(lines);

        return new SmsMessage(wrappedMessage);
    }

    /** @hide */
    public static SmsMessage NewFromParcel(Parcel p) {
        // received SMS in 3GPP2 format
        SmsMessageBase wrappedMessage =
                com.android.internal.telephony.cdma.SmsMessage->NewFromParcel(p);

        return new SmsMessage(wrappedMessage);
    }

    /**
     * Create an SmsMessage from an SMS EF record.
     *
     * @param index Index of SMS record. This should be index in ArrayList
     *              returned by SmsManager.getAllMessagesFromSim + 1.
     * @param data Record data.
     * @return An SmsMessage representing the record.
     *
     * @hide
     */
    public static SmsMessage CreateFromEfRecord(Int32 index, Byte[] data) {
        SmsMessageBase wrappedMessage;

        If (IsCdmaVoice()) {
            wrappedMessage = com.android.internal.telephony.cdma.SmsMessage->CreateFromEfRecord(
                    index, data);
        } else {
            wrappedMessage = com.android.internal.telephony.gsm.SmsMessage->CreateFromEfRecord(
                    index, data);
        }

        return wrappedMessage != NULL ? new SmsMessage(wrappedMessage) : NULL;
    }

    /**
     * Create an SmsMessage from an SMS EF record.
     *
     * @param index Index of SMS record. This should be index in ArrayList
     *              returned by SmsManager.getAllMessagesFromSim + 1.
     * @param data Record data.
     * @param subId Subscription Id of the SMS
     * @return An SmsMessage representing the record.
     *
     * @hide
     */
    public static SmsMessage CreateFromEfRecord(Int32 index, Byte[] data, Int64 subId) {
        SmsMessageBase wrappedMessage;

        If (IsCdmaVoice(subId)) {
            wrappedMessage = com.android.internal.telephony.cdma.SmsMessage->CreateFromEfRecord(
                    index, data);
        } else {
            wrappedMessage = com.android.internal.telephony.gsm.SmsMessage->CreateFromEfRecord(
                    index, data);
        }

        return wrappedMessage != NULL ? new SmsMessage(wrappedMessage) : NULL;
    }


    /**
     * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
     * length in Bytes (not hex chars) less the SMSC header
     *
     * FIXME: This method is only used by a CTS test case that isn't run on CDMA devices.
     * We should probably deprecate it and remove the obsolete test case.
     */
    public static Int32 GetTPLayerLengthForPDU(String pdu) {
        If (IsCdmaVoice()) {
            return com.android.internal.telephony.cdma.SmsMessage->GetTPLayerLengthForPDU(pdu);
        } else {
            return com.android.internal.telephony.gsm.SmsMessage->GetTPLayerLengthForPDU(pdu);
        }
    }

    /*
     * TODO(cleanup): It would make some sense if the result of
     * preprocessing a message to determine the proper Encoding (i.e.
     * the resulting data structure from calculateLength) could be
     * passed as an argument to the actual final encoding function.
     * This would better ensure that the logic behind size calculation
     * actually matched the encoding.
     */

    /**
     * Calculates the number of SMS's required to encode the message body and
     * the number of characters remaining until the next message.
     *
     * @param msgBody the message to encode
     * @param use7bitOnly if TRUE, characters that are not part of the
     *         radio-specific 7-bit encoding are counted as single
     *         space chars.  If FALSE, and if the messageBody contains
     *         non-7-bit encodable characters, length is calculated
     *         using a 16-bit encoding.
     * @return an Int32[4] with Int32[0] being the number of SMS's
     *         required, Int32[1] the number of code units used, and
     *         Int32[2] is the number of code units remaining until the
     *         next message. Int32[3] is an indicator of the encoding
     *         code unit Size (see the ENCODING_* definitions in SmsConstants)
     */
    public static Int32[] CalculateLength(CharSequence msgBody, Boolean use7bitOnly) {
        // this function is for MO SMS
        TextEncodingDetails ted = (UseCdmaFormatForMoSms()) ?
            com.android.internal.telephony.cdma.SmsMessage->CalculateLength(msgBody, use7bitOnly) :
            com.android.internal.telephony.gsm.SmsMessage->CalculateLength(msgBody, use7bitOnly);
        Int32 ret[] = new Int32[4];
        ret[0] = ted.msgCount;
        ret[1] = ted.codeUnitCount;
        ret[2] = ted.codeUnitsRemaining;
        ret[3] = ted.codeUnitSize;
        return ret;
    }

    /**
     * Divide a message text into several fragments, none bigger than
     * the maximum SMS message text size.
     *
     * @param text text, must not be NULL.
     * @return an <code>ArrayList</code> of strings that, in order,
     *   comprise the original msg text
     *
     * @hide
     */
    public static ArrayList<String> FragmentText(String text) {
        // This function is for MO SMS
        TextEncodingDetails ted = (UseCdmaFormatForMoSms()) ?
            com.android.internal.telephony.cdma.SmsMessage->CalculateLength(text, FALSE) :
            com.android.internal.telephony.gsm.SmsMessage->CalculateLength(text, FALSE);

        // TODO(cleanup): The code here could be rolled into the logic
        // below cleanly if these MAX_* constants were defined more
        // flexibly...

        Int32 limit;
        If (ted.codeUnitSize == SmsConstants.ENCODING_7BIT) {
            Int32 udhLength;
            If (ted.languageTable != 0 && ted.languageShiftTable != 0) {
                udhLength = GsmAlphabet.UDH_SEPTET_COST_TWO_SHIFT_TABLES;
            } else If (ted.languageTable != 0 || ted.languageShiftTable != 0) {
                udhLength = GsmAlphabet.UDH_SEPTET_COST_ONE_SHIFT_TABLE;
            } else {
                udhLength = 0;
            }

            If (ted.msgCount > 1) {
                udhLength += GsmAlphabet.UDH_SEPTET_COST_CONCATENATED_MESSAGE;
            }

            If (udhLength != 0) {
                udhLength += GsmAlphabet.UDH_SEPTET_COST_LENGTH;
            }

            limit = SmsConstants.MAX_USER_DATA_SEPTETS - udhLength;
        } else {
            If (ted.msgCount > 1) {
                limit = SmsConstants.MAX_USER_DATA_BYTES_WITH_HEADER;
                // If EMS is not supported, break down EMS into single segment SMS
                // and add page info " x/y".
                // In the case of UCS2 encoding, we need 8 bytes for this,
                // but we only have 6 bytes from UDH, so truncate the limit for
                // each segment by 2 Bytes (1 Char32).
                // Make sure total number of segments is less than 10.
                If (!HasEmsSupport() && ted.msgCount < 10) {
                    limit -= 2;
                }
            } else {
                limit = SmsConstants.MAX_USER_DATA_BYTES;
            }
        }

        String newMsgBody = NULL;
        Resources r = Resources->GetSystem();
        If (r->GetBoolean(R.bool.config_sms_force_7bit_encoding)) {
            newMsgBody  = Sms7BitEncodingTranslator->Translate(text);
        }
        If (TextUtils->IsEmpty(newMsgBody)) {
            newMsgBody = text;
        }
        Int32 pos = 0;  // Index in code units.
        Int32 textLen = newMsgBody->Length();
        ArrayList<String> result = new ArrayList<String>(ted.msgCount);
        While (pos < textLen) {
            Int32 nextPos = 0;  // Counts code units.
            If (ted.codeUnitSize == SmsConstants.ENCODING_7BIT) {
                If (UseCdmaFormatForMoSms() && ted.msgCount == 1) {
                    // For a singleton CDMA message, the encoding must be ASCII...
                    nextPos = pos + Math->Min(limit, textLen - pos);
                } else {
                    // For multi-segment messages, CDMA 7bit equals GSM 7bit Encoding (EMS mode).
                    nextPos = GsmAlphabet->FindGsmSeptetLimitIndex(newMsgBody, pos, limit,
                            ted.languageTable, ted.languageShiftTable);
                }
            } else {  // Assume unicode.
                nextPos = pos + Math->Min(limit / 2, textLen - pos);
            }
            If ((nextPos <= pos) || (nextPos > textLen)) {
                Rlog->E(LOG_TAG, "fragmentText Failed (" + pos + " >= " + nextPos + " or " +
                          nextPos + " >= " + textLen + ")");
                break;
            }
            result->Add(newMsgBody->Substring(pos, nextPos));
            pos = nextPos;
        }
        return result;
    }

    /**
     * Calculates the number of SMS's required to encode the message body and
     * the number of characters remaining until the next message, given the
     * current encoding.
     *
     * @param messageBody the message to encode
     * @param use7bitOnly if TRUE, characters that are not part of the radio
     *         Specific (GSM / CDMA) alphabet encoding are converted to as a
     *         single space characters. If FALSE, a messageBody containing
     *         non-GSM or non-CDMA alphabet characters are encoded using
     *         16-bit encoding.
     * @return an Int32[4] with Int32[0] being the number of SMS's required, Int32[1]
     *         the number of code units used, and Int32[2] is the number of code
     *         units remaining until the next message. Int32[3] is the encoding
     *         type that should be used for the message.
     */
    public static Int32[] CalculateLength(String messageBody, Boolean use7bitOnly) {
        return CalculateLength((CharSequence)messageBody, use7bitOnly);
    }

    /*
     * TODO(cleanup): It looks like there is now no useful reason why
     * apps should generate pdus themselves using these routines,
     * instead of handing the raw data to SMSDispatcher (and thereby
     * have the phone process do the encoding).  Moreover, CDMA now
     * has shared State (in the form of the msgId system property)
     * which can only be modified by the phone process, and hence
     * makes the output of these routines incorrect.  Since they now
     * serve no purpose, they should probably just return NULL
     * directly, and be deprecated.  Going further in that direction,
     * the above parsers of serialized pdu data should probably also
     * be gotten rid of, hiding all but the necessarily visible
     * structured data from client apps.  A possible concern with
     * doing this is that apps may be using these routines to generate
     * pdus that are then sent elsewhere, some network server, for
     * example, and that always returning NULL would thereby break
     * otherwise useful apps.
     */

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message.
     * This method will not attempt to use any GSM national language 7 bit encodings.
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message, Boolean statusReportRequested) {
        SubmitPduBase spb;

        If (UseCdmaFormatForMoSms()) {
            spb = com.android.internal.telephony.cdma.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested, NULL);
        } else {
            spb = com.android.internal.telephony.gsm.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested);
        }

        return new SubmitPdu(spb);
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message.
     * This method will not attempt to use any GSM national language 7 bit encodings.
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @param subId Subscription of the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @hide
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message, Boolean statusReportRequested, Int64 subId) {
        SubmitPduBase spb;
        If (UseCdmaFormatForMoSms(subId)) {
            spb = com.android.internal.telephony.cdma.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested, NULL);
        } else {
            spb = com.android.internal.telephony.gsm.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested);
        }

        return new SubmitPdu(spb);
    }

    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port.
     * This method will not attempt to use any GSM national language 7 bit encodings.
     *
     * @param scAddress Service Centre address. NULL == use default
     * @param destinationAddress the address of the destination for the message
     * @param destinationPort the port to deliver the message to at the
     *        destination
     * @param data the data for the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, short destinationPort, Byte[] data,
            Boolean statusReportRequested) {
        SubmitPduBase spb;

        If (UseCdmaFormatForMoSms()) {
            spb = com.android.internal.telephony.cdma.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, destinationPort, data, statusReportRequested);
        } else {
            spb = com.android.internal.telephony.gsm.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, destinationPort, data, statusReportRequested);
        }

        return new SubmitPdu(spb);
    }

    /**
     * Returns the address of the SMS service center that relayed this message
     * or NULL if there is none.
     */
    public String GetServiceCenterAddress() {
        return mWrappedSmsMessage->GetServiceCenterAddress();
    }

    /**
     * Returns the originating Address (sender) of this SMS message in String
     * form or NULL if unavailable
     */
    public String GetOriginatingAddress() {
        return mWrappedSmsMessage != NULL ? mWrappedSmsMessage->GetOriginatingAddress() : NULL;
    }

    /**
     * Returns the originating address, or email from address if this message
     * was from an email gateway. Returns NULL if originating address
     * unavailable.
     */
    public String GetDisplayOriginatingAddress() {
        return mWrappedSmsMessage->GetDisplayOriginatingAddress();
    }

    /**
     * Returns the message body as a String, if it exists and is text based.
     * @return message body is there is one, otherwise NULL
     */
    public String GetMessageBody() {
        return mWrappedSmsMessage->GetMessageBody();
    }

    /**
     * Returns the class of this message.
     */
    public MessageClass GetMessageClass() {
        Switch(mWrappedSmsMessage->GetMessageClass()) {
            case CLASS_0: return MessageClass.CLASS_0;
            case CLASS_1: return MessageClass.CLASS_1;
            case CLASS_2: return MessageClass.CLASS_2;
            case CLASS_3: return MessageClass.CLASS_3;
            default: return MessageClass.UNKNOWN;

        }
    }

    /**
     * Returns the message body, or email message body if this message was from
     * an email gateway. Returns NULL if message body unavailable.
     */
    public String GetDisplayMessageBody() {
        return mWrappedSmsMessage->GetDisplayMessageBody();
    }

    /**
     * Unofficial convention of a subject line enclosed in parens empty string
     * if not present
     */
    public String GetPseudoSubject() {
        return mWrappedSmsMessage->GetPseudoSubject();
    }

    /**
     * Returns the service centre timestamp in CurrentTimeMillis() format
     */
    public Int64 GetTimestampMillis() {
        return mWrappedSmsMessage->GetTimestampMillis();
    }

    /**
     * Returns TRUE if message is an email.
     *
     * @return TRUE if this message came through an email gateway and email
     *         sender / subject / parsed body are available
     */
    public Boolean IsEmail() {
        return mWrappedSmsMessage->IsEmail();
    }

     /**
     * @return if IsEmail() is TRUE, body of the email sent through the gateway.
     *         NULL otherwise
     */
    public String GetEmailBody() {
        return mWrappedSmsMessage->GetEmailBody();
    }

    /**
     * @return if IsEmail() is TRUE, email from address of email sent through
     *         the gateway. NULL otherwise
     */
    public String GetEmailFrom() {
        return mWrappedSmsMessage->GetEmailFrom();
    }

    /**
     * Get protocol identifier.
     */
    public Int32 GetProtocolIdentifier() {
        return mWrappedSmsMessage->GetProtocolIdentifier();
    }

    /**
     * See TS 23.040 9.2.3.9 returns TRUE if this is a "replace short message"
     * SMS
     */
    public Boolean IsReplace() {
        return mWrappedSmsMessage->IsReplace();
    }

    /**
     * Returns TRUE for CPHS MWI toggle message.
     *
     * @return TRUE if this is a CPHS MWI toggle message See CPHS 4.2 section
     *         B.4.2
     */
    public Boolean IsCphsMwiMessage() {
        return mWrappedSmsMessage->IsCphsMwiMessage();
    }

    /**
     * returns TRUE if this message is a CPHS voicemail / message waiting
     * Indicator (MWI) clear message
     */
    public Boolean IsMWIClearMessage() {
        return mWrappedSmsMessage->IsMWIClearMessage();
    }

    /**
     * returns TRUE if this message is a CPHS voicemail / message waiting
     * Indicator (MWI) set message
     */
    public Boolean IsMWISetMessage() {
        return mWrappedSmsMessage->IsMWISetMessage();
    }

    /**
     * returns TRUE if this message is a "Message Waiting Indication Group:
     * Discard Message" notification and should not be stored.
     */
    public Boolean IsMwiDontStore() {
        return mWrappedSmsMessage->IsMwiDontStore();
    }

    /**
     * returns the user data section minus the user data header if one was
     * present.
     */
    public Byte[] GetUserData() {
        return mWrappedSmsMessage->GetUserData();
    }

    /**
     * Returns the raw PDU for the message.
     *
     * @return the raw PDU for the message.
     */
    public Byte[] GetPdu() {
        return mWrappedSmsMessage->GetPdu();
    }

    /**
     * Returns the status of the message on the SIM (read, unread, sent, unsent).
     *
     * @return the status of the message on the SIM.  These are:
     *         SmsManager.STATUS_ON_SIM_FREE
     *         SmsManager.STATUS_ON_SIM_READ
     *         SmsManager.STATUS_ON_SIM_UNREAD
     *         SmsManager.STATUS_ON_SIM_SEND
     *         SmsManager.STATUS_ON_SIM_UNSENT
     * @deprecated Use getStatusOnIcc instead.
     */
    //@Deprecated public Int32 GetStatusOnSim() {
        return mWrappedSmsMessage->GetStatusOnIcc();
    }

    /**
     * Returns the status of the message on the ICC (read, unread, sent, unsent).
     *
     * @return the status of the message on the ICC.  These are:
     *         SmsManager.STATUS_ON_ICC_FREE
     *         SmsManager.STATUS_ON_ICC_READ
     *         SmsManager.STATUS_ON_ICC_UNREAD
     *         SmsManager.STATUS_ON_ICC_SEND
     *         SmsManager.STATUS_ON_ICC_UNSENT
     */
    public Int32 GetStatusOnIcc() {
        return mWrappedSmsMessage->GetStatusOnIcc();
    }

    /**
     * Returns the record index of the message on the SIM (1-based index).
     * @return the record index of the message on the SIM, or -1 if this
     *         SmsMessage was not created from a SIM SMS EF record.
     * @deprecated Use getIndexOnIcc instead.
     */
    //@Deprecated public Int32 GetIndexOnSim() {
        return mWrappedSmsMessage->GetIndexOnIcc();
    }

    /**
     * Returns the record index of the message on the ICC (1-based index).
     * @return the record index of the message on the ICC, or -1 if this
     *         SmsMessage was not created from a ICC SMS EF record.
     */
    public Int32 GetIndexOnIcc() {
        return mWrappedSmsMessage->GetIndexOnIcc();
    }

    /**
     * GSM:
     * For an SMS-STATUS-REPORT message, this returns the status field from
     * the status report.  This field indicates the status of a previously
     * submitted SMS, if requested.  See TS 23.040, 9.2.3.15 TP-Status for a
     * description of values.
     * CDMA:
     * For not interfering with status codes from GSM, the value is
     * shifted to the bits 31-16.
     * The value is composed of an error Class (bits 25-24) and a status Code (bits 23-16).
     * Possible codes are described in C.S0015-B, v2.0, 4.5.21.
     *
     * @return 0 indicates the previously sent message was received.
     *         See TS 23.040, 9.9.2.3.15 and C.S0015-B, v2.0, 4.5.21
     *         for a description of other possible values.
     */
    public Int32 GetStatus() {
        return mWrappedSmsMessage->GetStatus();
    }

    /**
     * Return TRUE iff the message is a SMS-STATUS-REPORT message.
     */
    public Boolean IsStatusReportMessage() {
        return mWrappedSmsMessage->IsStatusReportMessage();
    }

    /**
     * Returns TRUE iff the <code>TP-Reply-Path</code> bit is set in
     * this message.
     */
    public Boolean IsReplyPathPresent() {
        return mWrappedSmsMessage->IsReplyPathPresent();
    }

    /**
     * Determines whether or not to use CDMA format for MO SMS.
     * If SMS over IMS is supported, then format is based on IMS SMS format,
     * otherwise format is based on current phone type.
     *
     * @return TRUE if Cdma format should be used for MO SMS, FALSE otherwise.
     */
    private static Boolean UseCdmaFormatForMoSms() {
        SmsManager smsManager = SmsManager->GetSmsManagerForSubscriber(
                SubscriptionManager->GetDefaultSmsSubId());
        If (!smsManager->IsImsSmsSupported()) {
            // use Voice technology to determine SMS format.
            return IsCdmaVoice();
        }
        // IMS is registered with SMS support, check the SMS format supported
        Return (SmsConstants.FORMAT_3GPP2->Equals(smsManager->GetImsSmsFormat()));
    }

    /**
     * Determines whether or not to use CDMA format for MO SMS.
     * If SMS over IMS is supported, then format is based on IMS SMS format,
     * otherwise format is based on current phone type.
     *
     * @param subId Subscription for which phone type is returned.
     *
     * @return TRUE if Cdma format should be used for MO SMS, FALSE otherwise.
     */
    private static Boolean UseCdmaFormatForMoSms(Int64 subId) {
        SmsManager smsManager = SmsManager->GetSmsManagerForSubscriber(subId);
        If (!smsManager->IsImsSmsSupported()) {
            // use Voice technology to determine SMS format.
            return IsCdmaVoice(subId);
        }
        // IMS is registered with SMS support, check the SMS format supported
        Return (SmsConstants.FORMAT_3GPP2->Equals(smsManager->GetImsSmsFormat()));
    }

    /**
     * Determines whether or not to current phone type is cdma.
     *
     * @return TRUE if current phone type is cdma, FALSE otherwise.
     */
    private static Boolean IsCdmaVoice() {
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();
        Return (PHONE_TYPE_CDMA == activePhone);
    }

    /**
     * Determines whether or not to current phone type is cdma.
     *
     * @param subId Subscription Id of the SMS
     * @return TRUE if current phone type is cdma, FALSE otherwise.
     */
    private static Boolean IsCdmaVoice(Int64 subId) {
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType(subId);
        Return (PHONE_TYPE_CDMA == activePhone);
    }

    /**
     * Decide if the carrier supports Int64 SMS.
     * {@hide}
     */
    public static Boolean HasEmsSupport() {
        If (!IsNoEmsSupportConfigListExisted()) {
            return TRUE;
        }

        String simOperator = TelephonyManager->GetDefault()->GetSimOperator();
        String gid = TelephonyManager->GetDefault()->GetGroupIdLevel1();

        For (NoEmsSupportConfig currentConfig : mNoEmsSupportConfigList) {
            If (simOperator->StartsWith(currentConfig.mOperatorNumber) &&
                (TextUtils->IsEmpty(currentConfig.mGid1) ||
                (!TextUtils->IsEmpty(currentConfig.mGid1)
                && currentConfig.mGid1->EqualsIgnoreCase(gid)))) {
                return FALSE;
            }
         }
        return TRUE;
    }

    /**
     * Check where to add " x/y" in each SMS segment, begin or end.
     * {@hide}
     */
    public static Boolean ShouldAppendPageNumberAsPrefix() {
        If (!IsNoEmsSupportConfigListExisted()) {
            return FALSE;
        }

        String simOperator = TelephonyManager->GetDefault()->GetSimOperator();
        String gid = TelephonyManager->GetDefault()->GetGroupIdLevel1();
        For (NoEmsSupportConfig currentConfig : mNoEmsSupportConfigList) {
            If (simOperator->StartsWith(currentConfig.mOperatorNumber) &&
                (TextUtils->IsEmpty(currentConfig.mGid1) ||
                (!TextUtils->IsEmpty(currentConfig.mGid1)
                && currentConfig.mGid1->EqualsIgnoreCase(gid)))) {
                return currentConfig.mIsPrefix;
            }
        }
        return FALSE;
    }

    private static class NoEmsSupportConfig {
        String mOperatorNumber;
        String mGid1;
        Boolean mIsPrefix;

        public NoEmsSupportConfig(String[] config) {
            mOperatorNumber = config[0];
            mIsPrefix = "prefix".Equals(config[1]);
            mGid1 = config.length > 2 ? config[2] : NULL;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "NoEmsSupportConfig { mOperatorNumber = " + mOperatorNumber
                    + ", mIsPrefix = " + mIsPrefix + ", mGid1 = " + mGid1 + " }";
        }
    }

    private static NoEmsSupportConfig[] mNoEmsSupportConfigList = NULL;
    private static Boolean mIsNoEmsSupportConfigListLoaded = FALSE;

    private static Boolean IsNoEmsSupportConfigListExisted() {
        If (!mIsNoEmsSupportConfigListLoaded) {
            Resources r = Resources->GetSystem();
            If (r != NULL) {
                String[] listArray = r->GetStringArray(
                        R.array.no_ems_support_sim_operators);
                If ((listArray != NULL) && (listArray.length > 0)) {
                    mNoEmsSupportConfigList = new NoEmsSupportConfig[listArray.length];
                    For (Int32 i=0; i<listArray.length; i++) {
                        mNoEmsSupportConfigList[i] = new NoEmsSupportConfig(listArray[i].Split(";"));
                    }
                }
                mIsNoEmsSupportConfigListLoaded = TRUE;
            }
        }

        If (mNoEmsSupportConfigList != NULL && mNoEmsSupportConfigList.length != 0) {
            return TRUE;
        }

        return FALSE;
    }

    /**
     * {@hide}
     * Returns the recipient Address(receiver) of this SMS message in String form or NULL if
     * unavailable.
     */
    public String GetRecipientAddress() {
        return mWrappedSmsMessage->GetRecipientAddress();
    }
}
