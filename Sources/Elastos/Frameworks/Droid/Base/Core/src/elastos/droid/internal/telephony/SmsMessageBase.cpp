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

package com.android.internal.telephony;

using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Utility::IArrays;

using Elastos::Droid::Provider::ITelephony;

/**
 * Base class declaring the specific methods and members for SmsMessage.
 * {@hide}
 */
public abstract class SmsMessageBase {
    /** {@hide} The address of the SMSC. May be NULL */
    protected String mScAddress;

    /** {@hide} The address of the sender */
    protected SmsAddress mOriginatingAddress;

    /** {@hide} The address of the receiver */
    protected SmsAddress mRecipientAddress;

    /** {@hide} The message body as a string. May be NULL if the message isn't text */
    protected String mMessageBody;

    /** {@hide} */
    protected String mPseudoSubject;

    /** {@hide} Non-NULL if this is an email gateway message */
    protected String mEmailFrom;

    /** {@hide} Non-NULL if this is an email gateway message */
    protected String mEmailBody;

    /** {@hide} */
    protected Boolean mIsEmail;

    /** {@hide} */
    protected Int64 mScTimeMillis;

    /** {@hide} The raw PDU of the message */
    protected Byte[] mPdu;

    /** {@hide} The raw bytes for the user data section of the message */
    protected Byte[] mUserData;

    /** {@hide} */
    protected SmsHeader mUserDataHeader;

    // "Message Waiting Indication Group"
    // 23.038 Section 4
    /** {@hide} */
    protected Boolean mIsMwi;

    /** {@hide} */
    protected Boolean mMwiSense;

    /** {@hide} */
    protected Boolean mMwiDontStore;

    /**
     * Indicates status for messages stored on the ICC.
     */
    protected Int32 mStatusOnIcc = -1;

    /**
     * Record index of message in the EF.
     */
    protected Int32 mIndexOnIcc = -1;

    /** TP-Message-Reference - Message Reference of sent message. @hide */
    public Int32 mMessageRef;

    // TODO(): This class is duplicated in SmsMessage.java. Refactor accordingly.
    public static abstract class SubmitPduBase  {
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
    }

    /**
     * Returns the address of the SMS service center that relayed this message
     * or NULL if there is none.
     */
    public String GetServiceCenterAddress() {
        return mScAddress;
    }

    /**
     * Returns the originating Address (sender) of this SMS message in String
     * form or NULL if unavailable
     */
    public String GetOriginatingAddress() {
        If (mOriginatingAddress == NULL) {
            return NULL;
        }

        return mOriginatingAddress->GetAddressString();
    }

    /**
     * Returns the originating address, or email from address if this message
     * was from an email gateway. Returns NULL if originating address
     * unavailable.
     */
    public String GetDisplayOriginatingAddress() {
        If (mIsEmail) {
            return mEmailFrom;
        } else {
            return GetOriginatingAddress();
        }
    }

    /**
     * Returns the message body as a String, if it exists and is text based.
     * @return message body is there is one, otherwise NULL
     */
    public String GetMessageBody() {
        return mMessageBody;
    }

    /**
     * Returns the class of this message.
     */
    public abstract SmsConstants.MessageClass GetMessageClass();

    /**
     * Returns the message body, or email message body if this message was from
     * an email gateway. Returns NULL if message body unavailable.
     */
    public String GetDisplayMessageBody() {
        If (mIsEmail) {
            return mEmailBody;
        } else {
            return GetMessageBody();
        }
    }

    /**
     * Unofficial convention of a subject line enclosed in parens empty string
     * if not present
     */
    public String GetPseudoSubject() {
        return mPseudoSubject == NULL ? "" : mPseudoSubject;
    }

    /**
     * Returns the service centre timestamp in CurrentTimeMillis() format
     */
    public Int64 GetTimestampMillis() {
        return mScTimeMillis;
    }

    /**
     * Returns TRUE if message is an email.
     *
     * @return TRUE if this message came through an email gateway and email
     *         sender / subject / parsed body are available
     */
    public Boolean IsEmail() {
        return mIsEmail;
    }

    /**
     * @return if IsEmail() is TRUE, body of the email sent through the gateway.
     *         NULL otherwise
     */
    public String GetEmailBody() {
        return mEmailBody;
    }

    /**
     * @return if IsEmail() is TRUE, email from address of email sent through
     *         the gateway. NULL otherwise
     */
    public String GetEmailFrom() {
        return mEmailFrom;
    }

    /**
     * Get protocol identifier.
     */
    public abstract Int32 GetProtocolIdentifier();

    /**
     * See TS 23.040 9.2.3.9 returns TRUE if this is a "replace short message"
     * SMS
     */
    public abstract Boolean IsReplace();

    /**
     * Returns TRUE for CPHS MWI toggle message.
     *
     * @return TRUE if this is a CPHS MWI toggle message See CPHS 4.2 section
     *         B.4.2
     */
    public abstract Boolean IsCphsMwiMessage();

    /**
     * returns TRUE if this message is a CPHS voicemail / message waiting
     * Indicator (MWI) clear message
     */
    public abstract Boolean IsMWIClearMessage();

    /**
     * returns TRUE if this message is a CPHS voicemail / message waiting
     * Indicator (MWI) set message
     */
    public abstract Boolean IsMWISetMessage();

    /**
     * returns TRUE if this message is a "Message Waiting Indication Group:
     * Discard Message" notification and should not be stored.
     */
    public abstract Boolean IsMwiDontStore();

    /**
     * returns the user data section minus the user data header if one was
     * present.
     */
    public Byte[] GetUserData() {
        return mUserData;
    }

    /**
     * Returns an object representing the user data header
     *
     * {@hide}
     */
    public SmsHeader GetUserDataHeader() {
        return mUserDataHeader;
    }

    /**
     * TODO(cleanup): The term PDU is used in a seemingly non-unique
     * manner -- for example, what is the difference between this Byte
     * array and the contents of SubmitPdu objects.  Maybe a more
     * illustrative term would be appropriate.
     */

    /**
     * Returns the raw PDU for the message.
     */
    public Byte[] GetPdu() {
        return mPdu;
    }

    /**
     * For an SMS-STATUS-REPORT message, this returns the status field from
     * the status report.  This field indicates the status of a previously
     * submitted SMS, if requested.  See TS 23.040, 9.2.3.15 TP-Status for a
     * description of values.
     *
     * @return 0 indicates the previously sent message was received.
     *         See TS 23.040, 9.9.2.3.15 for a description of other possible
     *         values.
     */
    public abstract Int32 GetStatus();

    /**
     * Return TRUE iff the message is a SMS-STATUS-REPORT message.
     */
    public abstract Boolean IsStatusReportMessage();

    /**
     * Returns TRUE iff the <code>TP-Reply-Path</code> bit is set in
     * this message.
     */
    public abstract Boolean IsReplyPathPresent();

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
        return mStatusOnIcc;
    }

    /**
     * Returns the record index of the message on the ICC (1-based index).
     * @return the record index of the message on the ICC, or -1 if this
     *         SmsMessage was not created from a ICC SMS EF record.
     */
    public Int32 GetIndexOnIcc() {
        return mIndexOnIcc;
    }

    protected void ParseMessageBody() {
        // originatingAddress could be NULL if this message is from a status
        // report.
        If (mOriginatingAddress != NULL && mOriginatingAddress->CouldBeEmailGateway()) {
            ExtractEmailAddressFromMessageBody();
        }
    }

    /**
     * Try to parse this message as an email gateway message
     * There are two ways specified in TS 23.040 Section 3.8 :
     *  - SMS message "may have its TP-PID set for Internet electronic mail - MT
     * SMS format: [<from-address><space>]<message> - "Depending on the
     * nature of the gateway, the destination/origination address is either
     * derived from the content of the SMS TP-OA or TP-DA field, or the
     * TP-OA/TP-DA field contains a generic gateway address and the to/from
     * address is added at the beginning as shown above." (which is supported here)
     * - Multiple addresses separated by commas, no spaces, Subject field delimited
     * by '()' or '##' and '#' Section 9.2.3.24.11 (which are NOT supported here)
     */
    protected void ExtractEmailAddressFromMessageBody() {

        /* Some carriers may use " /" delimiter as below
         *
         * 1. [x@y][ ]/[subject][ ]/[body]
         * -or-
         * 2. [x@y][ ]/[body]
         */
         String[] parts = mMessageBody->Split("( /)|( )", 2);
         If (parts.length < 2) return;
         mEmailFrom = parts[0];
         mEmailBody = parts[1];
         mIsEmail = Telephony.Mms->IsEmailAddress(mEmailFrom);
    }

    /**
     * {@hide}
     * Returns the receiver address of this SMS message in String
     * form or NULL if unavailable
     */
    public String GetRecipientAddress() {
        If (mRecipientAddress == NULL) {
            return NULL;
        }

        return mRecipientAddress->GetAddressString();
    }
}
