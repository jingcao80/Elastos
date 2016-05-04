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

package android.telephony.gsm;

using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::SmsMessageBase::ISubmitPduBase;

using Elastos::Utility::IArrays;

using static android::Telephony::TelephonyManager::IPHONE_TYPE_CDMA;


/**
 * A Short Message Service message.
 * @deprecated Replaced by android.telephony.SmsMessage that supports both GSM and CDMA.
 */
@Deprecated
public class SmsMessage {
    /**
     * SMS Class enumeration.
     * See TS 23.038.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public enum MessageClass{
        UNKNOWN, CLASS_0, CLASS_1, CLASS_2, CLASS_3;
    }

    /** Unknown encoding Scheme (see TS 23.038)
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated public static const Int32 ENCODING_UNKNOWN = 0;

    /** 7-bit encoding Scheme (see TS 23.038)
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated public static const Int32 ENCODING_7BIT = 1;

    /** 8-bit encoding Scheme (see TS 23.038)
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated public static const Int32 ENCODING_8BIT = 2;

    /** 16-bit encoding Scheme (see TS 23.038)
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated public static const Int32 ENCODING_16BIT = 3;

    /** The maximum number of payload bytes per message
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated public static const Int32 MAX_USER_DATA_BYTES = 140;

    /**
     * The maximum number of payload bytes per message if a user data header
     * is present.  This assumes the header only contains the
     * CONCATENATED_8_BIT_REFERENCE element.
     *
     * @deprecated Use android.telephony.SmsMessage.
     * @hide pending API Council approval to extend the public API
     */
    //@Deprecated public static const Int32 MAX_USER_DATA_BYTES_WITH_HEADER = 134;

    /** The maximum number of payload septets per message
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated public static const Int32 MAX_USER_DATA_SEPTETS = 160;

    /**
     * The maximum number of payload septets per message if a user data header
     * is present.  This assumes the header only contains the
     * CONCATENATED_8_BIT_REFERENCE element.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated public static const Int32 MAX_USER_DATA_SEPTETS_WITH_HEADER = 153;

    /** Contains actual SmsMessage. Only public for debugging and for framework layer.
     * @deprecated Use android.telephony.SmsMessage.
     * {@hide}
     */
    //@Deprecated public SmsMessageBase mWrappedSmsMessage;

    /** @deprecated Use android.telephony.SmsMessage. */
    //@Deprecated
    public static class SubmitPdu {
        /** @deprecated Use android.telephony.SmsMessage. */
        //@Deprecated public Byte[] encodedScAddress; // Null if not applicable.
        /** @deprecated Use android.telephony.SmsMessage. */
        //@Deprecated public Byte[] encodedMessage;

        //Constructor
        /** @deprecated Use android.telephony.SmsMessage. */
        //@Deprecated
        public SubmitPdu() {
        }

        /** @deprecated Use android.telephony.SmsMessage.
         * {@hide}
         */
        //@Deprecated
        protected SubmitPdu(SubmitPduBase spb) {
            this.encodedMessage = spb.encodedMessage;
            this.encodedScAddress = spb.encodedScAddress;
        }

        /** @deprecated Use android.telephony.SmsMessage. */
        //@Override
        //@Deprecated
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "SubmitPdu: encodedScAddress = "
                    + Arrays->ToString(encodedScAddress)
                    + ", encodedMessage = "
                    + Arrays->ToString(encodedMessage);
        }
    }

    // Constructor
    /** @deprecated Use android.telephony.SmsMessage. */
    //@Deprecated
    public SmsMessage() {
        This(GetSmsFacility());
    }

    private SmsMessage(SmsMessageBase smb) {
        mWrappedSmsMessage = smb;
    }

    /**
     * Create an SmsMessage from a raw PDU.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public static SmsMessage CreateFromPdu(Byte[] pdu) {
        SmsMessageBase wrappedMessage;
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();

        If (PHONE_TYPE_CDMA == activePhone) {
            wrappedMessage = com.android.internal.telephony.cdma.SmsMessage->CreateFromPdu(pdu);
        } else {
            wrappedMessage = com.android.internal.telephony.gsm.SmsMessage->CreateFromPdu(pdu);
        }

        // Hangouts, and some other bad citizens, are using this deprecated API call.
        // Make sure that WhisperPush and Voice+ integration properly work,
        // as this may be a synthetic message.
        If (wrappedMessage == NULL) {
            // returns NULL if it is not synthetic. (likely malformed)
            wrappedMessage = com.android.internal.telephony.SyntheticSmsMessage->CreateFromPdu(pdu);
        }

        return new SmsMessage(wrappedMessage);
    }

    /**
     * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
     * length in Bytes (not hex chars) less the SMSC header
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public static Int32 GetTPLayerLengthForPDU(String pdu) {
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();

        If (PHONE_TYPE_CDMA == activePhone) {
            return com.android.internal.telephony.cdma.SmsMessage->GetTPLayerLengthForPDU(pdu);
        } else {
            return com.android.internal.telephony.gsm.SmsMessage->GetTPLayerLengthForPDU(pdu);
        }
    }

    /**
     * Calculates the number of SMS's required to encode the message body and
     * the number of characters remaining until the next message, given the
     * current encoding.
     *
     * @param messageBody the message to encode
     * @param use7bitOnly if TRUE, characters that are not part of the GSM
     *         alphabet are counted as a single space Char32.  If FALSE, a
     *         messageBody containing non-GSM alphabet characters is calculated
     *         for 16-bit encoding.
     * @return an Int32[4] with Int32[0] being the number of SMS's required, Int32[1]
     *         the number of code units used, and Int32[2] is the number of code
     *         units remaining until the next message. Int32[3] is the encoding
     *         type that should be used for the message.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public static Int32[] CalculateLength(CharSequence messageBody, Boolean use7bitOnly) {
        GsmAlphabet.TextEncodingDetails ted =
                com.android.internal.telephony.gsm.SmsMessage
                        .CalculateLength(messageBody, use7bitOnly);
        Int32 ret[] = new Int32[4];
        ret[0] = ted.msgCount;
        ret[1] = ted.codeUnitCount;
        ret[2] = ted.codeUnitsRemaining;
        ret[3] = ted.codeUnitSize;
        return ret;
    }

    /**
     * Calculates the number of SMS's required to encode the message body and
     * the number of characters remaining until the next message, given the
     * current encoding.
     *
     * @param messageBody the message to encode
     * @param use7bitOnly if TRUE, characters that are not part of the GSM
     *         alphabet are counted as a single space Char32.  If FALSE, a
     *         messageBody containing non-GSM alphabet characters is calculated
     *         for 16-bit encoding.
     * @return an Int32[4] with Int32[0] being the number of SMS's required, Int32[1]
     *         the number of code units used, and Int32[2] is the number of code
     *         units remaining until the next message. Int32[3] is the encoding
     *         type that should be used for the message.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public static Int32[] CalculateLength(String messageBody, Boolean use7bitOnly) {
        return CalculateLength((CharSequence)messageBody, use7bitOnly);
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @deprecated Use android.telephony.SmsMessage.
     * @hide
     */
    //@Deprecated
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message,
            Boolean statusReportRequested, Byte[] header) {
        SubmitPduBase spb;
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();

        If (PHONE_TYPE_CDMA == activePhone) {
            spb = com.android.internal.telephony.cdma.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested,
                    SmsHeader->FromByteArray(header));
        } else {
            spb = com.android.internal.telephony.gsm.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested, header);
        }

        return new SubmitPdu(spb);
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message, Boolean statusReportRequested) {
        SubmitPduBase spb;
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();

        If (PHONE_TYPE_CDMA == activePhone) {
            spb = com.android.internal.telephony.cdma.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested, NULL);
        } else {
            spb = com.android.internal.telephony.gsm.SmsMessage->GetSubmitPdu(scAddress,
                    destinationAddress, message, statusReportRequested);
        }

        return new SubmitPdu(spb);
    }

    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
     *
     * @param scAddress Service Centre address. NULL == use default
     * @param destinationAddress the address of the destination for the message
     * @param destinationPort the port to deliver the message to at the
     *        destination
     * @param data the dat for the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, short destinationPort, Byte[] data,
            Boolean statusReportRequested) {
        SubmitPduBase spb;
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();

        If (PHONE_TYPE_CDMA == activePhone) {
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
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetServiceCenterAddress() {
        return mWrappedSmsMessage->GetServiceCenterAddress();
    }

    /**
     * Returns the originating Address (sender) of this SMS message in String
     * form or NULL if unavailable
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetOriginatingAddress() {
        return mWrappedSmsMessage->GetOriginatingAddress();
    }

    /**
     * Returns the originating address, or email from address if this message
     * was from an email gateway. Returns NULL if originating address
     * unavailable.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetDisplayOriginatingAddress() {
        return mWrappedSmsMessage->GetDisplayOriginatingAddress();
    }

    /**
     * Returns the message body as a String, if it exists and is text based.
     * @return message body is there is one, otherwise NULL
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetMessageBody() {
        return mWrappedSmsMessage->GetMessageBody();
    }

    /**
     * Returns the class of this message.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public MessageClass GetMessageClass() {
        Int32 index = mWrappedSmsMessage->GetMessageClass()->Ordinal();

        return MessageClass->Values()[index];
    }

    /**
     * Returns the message body, or email message body if this message was from
     * an email gateway. Returns NULL if message body unavailable.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetDisplayMessageBody() {
        return mWrappedSmsMessage->GetDisplayMessageBody();
    }

    /**
     * Unofficial convention of a subject line enclosed in parens empty string
     * if not present
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetPseudoSubject() {
        return mWrappedSmsMessage->GetPseudoSubject();
    }

    /**
     * Returns the service centre timestamp in CurrentTimeMillis() format
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Int64 GetTimestampMillis() {
        return mWrappedSmsMessage->GetTimestampMillis();
    }

    /**
     * Returns TRUE if message is an email.
     *
     * @return TRUE if this message came through an email gateway and email
     *         sender / subject / parsed body are available
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsEmail() {
        return mWrappedSmsMessage->IsEmail();
    }

     /**
     * @return if IsEmail() is TRUE, body of the email sent through the gateway.
     *         NULL otherwise
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetEmailBody() {
        return mWrappedSmsMessage->GetEmailBody();
    }

    /**
     * @return if IsEmail() is TRUE, email from address of email sent through
     *         the gateway. NULL otherwise
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public String GetEmailFrom() {
        return mWrappedSmsMessage->GetEmailFrom();
    }

    /**
     * Get protocol identifier.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Int32 GetProtocolIdentifier() {
        return mWrappedSmsMessage->GetProtocolIdentifier();
    }

    /**
     * See TS 23.040 9.2.3.9 returns TRUE if this is a "replace short message" SMS
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsReplace() {
        return mWrappedSmsMessage->IsReplace();
    }

    /**
     * Returns TRUE for CPHS MWI toggle message.
     *
     * @return TRUE if this is a CPHS MWI toggle message See CPHS 4.2 section B.4.2
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsCphsMwiMessage() {
        return mWrappedSmsMessage->IsCphsMwiMessage();
    }

    /**
     * returns TRUE if this message is a CPHS voicemail / message waiting
     * Indicator (MWI) clear message
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsMWIClearMessage() {
        return mWrappedSmsMessage->IsMWIClearMessage();
    }

    /**
     * returns TRUE if this message is a CPHS voicemail / message waiting
     * Indicator (MWI) set message
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsMWISetMessage() {
        return mWrappedSmsMessage->IsMWISetMessage();
    }

    /**
     * returns TRUE if this message is a "Message Waiting Indication Group:
     * Discard Message" notification and should not be stored.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsMwiDontStore() {
        return mWrappedSmsMessage->IsMwiDontStore();
    }

    /**
     * returns the user data section minus the user data header if one was present.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Byte[] GetUserData() {
        return mWrappedSmsMessage->GetUserData();
    }

    /* Not part of the SDK interface and only needed by specific classes:
       protected SmsHeader GetUserDataHeader()
    */

    /**
     * Returns the raw PDU for the message.
     *
     * @return the raw PDU for the message.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
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
     * @deprecated Use android.telephony.SmsMessage and getStatusOnIcc instead.
     */
    //@Deprecated
    public Int32 GetStatusOnSim() {
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
     * @deprecated Use android.telephony.SmsMessage.
     * @hide
     */
    //@Deprecated
    public Int32 GetStatusOnIcc() {

        return mWrappedSmsMessage->GetStatusOnIcc();
    }

    /**
     * Returns the record index of the message on the SIM (1-based index).
     * @return the record index of the message on the SIM, or -1 if this
     *         SmsMessage was not created from a SIM SMS EF record.
     * @deprecated Use android.telephony.SmsMessage and getIndexOnIcc instead.
     */
    //@Deprecated
    public Int32 GetIndexOnSim() {
        return mWrappedSmsMessage->GetIndexOnIcc();
    }

    /**
     * Returns the record index of the message on the ICC (1-based index).
     * @return the record index of the message on the ICC, or -1 if this
     *         SmsMessage was not created from a ICC SMS EF record.
     * @deprecated Use android.telephony.SmsMessage.
     * @hide
     */
    //@Deprecated
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
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Int32 GetStatus() {
        return mWrappedSmsMessage->GetStatus();
    }

    /**
     * Return TRUE iff the message is a SMS-STATUS-REPORT message.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsStatusReportMessage() {
        return mWrappedSmsMessage->IsStatusReportMessage();
    }

    /**
     * Returns TRUE iff the <code>TP-Reply-Path</code> bit is set in
     * this message.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    public Boolean IsReplyPathPresent() {
        return mWrappedSmsMessage->IsReplyPathPresent();
    }

    /** This method returns the reference to a specific
     *  SmsMessage object, which is used for accessing its static methods.
     * @return Specific SmsMessage.
     * @deprecated Use android.telephony.SmsMessage.
     */
    //@Deprecated
    private static const SmsMessageBase GetSmsFacility(){
        Int32 activePhone = TelephonyManager->GetDefault()->GetCurrentPhoneType();
        If (PHONE_TYPE_CDMA == activePhone) {
            return new com.android.internal.telephony.cdma->SmsMessage();
        } else {
            return new com.android.internal.telephony.gsm->SmsMessage();
        }
    }
}
