/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony.gsm;

using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::GsmAlphabet::ITextEncodingDetails;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::ISmsHeader;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::ISms7BitEncodingTranslator;

using Elastos::IO::IByteArrayOutputStream;

using static com::Android::Internal::Telephony::SmsConstants::IMessageClass;
using static com::Android::Internal::Telephony::SmsConstants::IENCODING_UNKNOWN;
using static com::Android::Internal::Telephony::SmsConstants::IENCODING_7BIT;
using static com::Android::Internal::Telephony::SmsConstants::IENCODING_8BIT;
using static com::Android::Internal::Telephony::SmsConstants::IENCODING_16BIT;
using static com::Android::Internal::Telephony::SmsConstants::IENCODING_KSC5601;
using static com::Android::Internal::Telephony::SmsConstants::IMAX_USER_DATA_SEPTETS;
using static com::Android::Internal::Telephony::SmsConstants::IMAX_USER_DATA_BYTES;
using static com::Android::Internal::Telephony::SmsConstants::IMAX_USER_DATA_BYTES_WITH_HEADER;

/**
 * A Short Message Service message.
 *
 */
public class SmsMessage extends SmsMessageBase {
    static const String LOG_TAG = "SmsMessage";
    private static const Boolean VDBG = FALSE;

    private MessageClass messageClass;

    /**
     * TP-Message-Type-Indicator
     * 9.2.3
     */
    private Int32 mMti;

    /** TP-Protocol-Identifier (TP-PID) */
    private Int32 mProtocolIdentifier;

    // TP-Data-Coding-Scheme
    // see TS 23.038
    private Int32 mDataCodingScheme;

    // TP-Reply-Path
    // e.g. 23.040 9.2.2.1
    private Boolean mReplyPathPresent = FALSE;

    /**
     *  TP-Status - status of a previously submitted SMS.
     *  This field applies to SMS-STATUS-REPORT messages.  0 indicates success;
     *  see TS 23.040, 9.2.3.15 for description of other possible values.
     */
    private Int32 mStatus;

    /**
     *  TP-Status - status of a previously submitted SMS.
     *  This field is TRUE iff the message is a SMS-STATUS-REPORT message.
     */
    private Boolean mIsStatusReportMessage = FALSE;

    private Int32 mVoiceMailCount = 0;

    private static const Int32 VALIDITY_PERIOD_FORMAT_NONE = 0x00;
    private static const Int32 VALIDITY_PERIOD_FORMAT_ENHANCED = 0x01;
    private static const Int32 VALIDITY_PERIOD_FORMAT_RELATIVE = 0x02;
    private static const Int32 VALIDITY_PERIOD_FORMAT_ABSOLUTE = 0x03;

    //Validity Period min - 5 mins
    private static const Int32 VALIDITY_PERIOD_MIN = 5;
    //Validity Period max - 63 weeks
    private static const Int32 VALIDITY_PERIOD_MAX = 635040;

    private static const Int32 INVALID_VALIDITY_PERIOD = -1;

    public static class SubmitPdu extends SubmitPduBase {
    }

    /**
     * Create an SmsMessage from a raw PDU.
     */
    public static SmsMessage CreateFromPdu(Byte[] pdu) {
        try {
            SmsMessage msg = new SmsMessage();
            msg->ParsePdu(pdu);
            return msg;
        } Catch (RuntimeException ex) {
            Rlog->E(LOG_TAG, "SMS PDU parsing failed: ", ex);
            return NULL;
        } Catch (OutOfMemoryError e) {
            Rlog->E(LOG_TAG, "SMS PDU parsing failed with out of memory: ", e);
            return NULL;
        }
    }

    /**
     * 3GPP TS 23.040 9.2.3.9 specifies that Type Zero messages are indicated
     * by TP_PID field set to value 0x40
     */
    public Boolean IsTypeZero() {
        Return (mProtocolIdentifier == 0x40);
    }

    /**
     * TS 27.005 3.4.1 lines[0] and lines[1] are the two lines read from the
     * +CMT unsolicited Response (PDU mode, of course)
     *  +CMT: [&lt;alpha>],<length><CR><LF><pdu>
     *
     * Only public for debugging
     *
     * {@hide}
     */
    public static SmsMessage NewFromCMT(String[] lines) {
        try {
            SmsMessage msg = new SmsMessage();
            msg->ParsePdu(IccUtils->HexStringToBytes(lines[1]));
            return msg;
        } Catch (RuntimeException ex) {
            Rlog->E(LOG_TAG, "SMS PDU parsing failed: ", ex);
            return NULL;
        }
    }

    /** @hide */
    public static SmsMessage NewFromCDS(String line) {
        try {
            SmsMessage msg = new SmsMessage();
            msg->ParsePdu(IccUtils->HexStringToBytes(line));
            return msg;
        } Catch (RuntimeException ex) {
            Rlog->E(LOG_TAG, "CDS SMS PDU parsing failed: ", ex);
            return NULL;
        }
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
        try {
            SmsMessage msg = new SmsMessage();

            msg.mIndexOnIcc = index;

            // First Byte is status: RECEIVED_READ, RECEIVED_UNREAD, STORED_SENT,
            // or STORED_UNSENT
            // See TS 51.011 10.5.3
            If ((data[0] & 1) == 0) {
                Rlog->W(LOG_TAG,
                        "SMS parsing failed: Trying to parse a free record");
                return NULL;
            } else {
                msg.mStatusOnIcc = data[0] & 0x07;
            }

            Int32 size = data.length - 1;

            // Note: Data may include trailing FF's.  That's OK; message
            // should still parse correctly.
            Byte[] pdu = new Byte[size];
            System->Arraycopy(data, 1, pdu, 0, size);
            msg->ParsePdu(pdu);
            return msg;
        } Catch (RuntimeException ex) {
            Rlog->E(LOG_TAG, "SMS PDU parsing failed: ", ex);
            return NULL;
        }
    }

    /**
     * Get the TP-Layer-Length for the given SMS-SUBMIT PDU Basically, the
     * length in Bytes (not hex chars) less the SMSC header
     */
    public static Int32 GetTPLayerLengthForPDU(String pdu) {
        Int32 len = pdu->Length() / 2;
        Int32 smscLen = Integer->ParseInt(pdu->Substring(0, 2), 16);

        return len - smscLen - 1;
    }

    /**
     * Get Encoded Relative Validty Period Value from Validity period in mins.
     *
     * @param validityPeriod Validity period in mins.
     *
     * Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     * ||RelValidityPeriod (TP-VP)  ||                 ||  validityPeriod   ||
     *
     *      0 to 143                            --->       (TP-VP + 1) x 5 minutes
     *
     *      144 to 167                         --->        12 hours + ((TP-VP -143) x 30 minutes)
     *
     *      168 to 196                         --->        (TP-VP - 166) x 1 day
     *
     *      197 to 255                         --->        (TP-VP - 192) x 1 week
     *
     * @return relValidityPeriod Encoded Relative Validity Period Value.
     * @hide
     */
    public static Int32 GetRelativeValidityPeriod(Int32 validityPeriod) {
        Int32 relValidityPeriod = INVALID_VALIDITY_PERIOD;

        If (validityPeriod < VALIDITY_PERIOD_MIN  || validityPeriod > VALIDITY_PERIOD_MAX) {
            Rlog->E(LOG_TAG,"Invalid Validity Period" + validityPeriod);
            return relValidityPeriod;
        }

        If (validityPeriod <= 720) {
            relValidityPeriod = (validityPeriod  / 5) - 1;
        } else If (validityPeriod <= 1440) {
            relValidityPeriod = ((validityPeriod - 720) / 30) + 143;
        } else If (validityPeriod <= 43200) {
            relValidityPeriod = (validityPeriod  / 1440) + 166;
        } else If (validityPeriod <= 635040) {
            relValidityPeriod = (validityPeriod  / 10080) + 192;
        }
        return relValidityPeriod;
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @hide
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message,
            Boolean statusReportRequested, Byte[] header) {
        return GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested, header,
                ENCODING_UNKNOWN, 0, 0);
    }


    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message using the
     * specified encoding.
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @param encoding Encoding defined by constants in
     *        com.android.internal.telephony.SmsConstants.ENCODING_*
     * @param languageTable
     * @param languageShiftTable
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @hide
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message,
            Boolean statusReportRequested, Byte[] header, Int32 encoding,
            Int32 languageTable, Int32 languageShiftTable) {
        return GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested,
            header, encoding, languageTable, languageShiftTable, -1);
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message using the
     * specified encoding.
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @param encoding Encoding defined by constants in
     *        com.android.internal.telephony.SmsConstants.ENCODING_*
     * @param languageTable
     * @param languageShiftTable
     * @param validityPeriod Validity Period of the message in Minutes.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     * @hide
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message,
            Boolean statusReportRequested, Byte[] header, Int32 encoding,
            Int32 languageTable, Int32 languageShiftTable, Int32 validityPeriod) {

        // Perform NULL parameter checks.
        If (message == NULL || destinationAddress == NULL) {
            return NULL;
        }

        If (encoding == ENCODING_UNKNOWN) {
            // Find the best encoding to use
            TextEncodingDetails ted = CalculateLength(message, FALSE);
            encoding = ted.codeUnitSize;
            languageTable = ted.languageTable;
            languageShiftTable = ted.languageShiftTable;

            If (encoding == ENCODING_7BIT &&
                    (languageTable != 0 || languageShiftTable != 0)) {
                If (header != NULL) {
                    SmsHeader smsHeader = SmsHeader->FromByteArray(header);
                    If (smsHeader.languageTable != languageTable
                            || smsHeader.languageShiftTable != languageShiftTable) {
                        Rlog->W(LOG_TAG, "Updating language table in SMS header: "
                                + smsHeader.languageTable + " -> " + languageTable + ", "
                                + smsHeader.languageShiftTable + " -> " + languageShiftTable);
                        smsHeader.languageTable = languageTable;
                        smsHeader.languageShiftTable = languageShiftTable;
                        header = SmsHeader->ToByteArray(smsHeader);
                    }
                } else {
                    SmsHeader smsHeader = new SmsHeader();
                    smsHeader.languageTable = languageTable;
                    smsHeader.languageShiftTable = languageShiftTable;
                    header = SmsHeader->ToByteArray(smsHeader);
                }
            }
        }

        SubmitPdu ret = new SubmitPdu();

        Int32 validityPeriodFormat = VALIDITY_PERIOD_FORMAT_NONE;
        Int32 relativeValidityPeriod = INVALID_VALIDITY_PERIOD;

        // TP-Validity-Period-Format (TP-VPF) in 3GPP TS 23.040 V6.8.1 section 9.2.3.3
        //bit 4:3 = 10 - TP-VP field present - relative format
        If((relativeValidityPeriod = GetRelativeValidityPeriod(validityPeriod)) >= 0) {
            validityPeriodFormat = VALIDITY_PERIOD_FORMAT_RELATIVE;
        }

        Byte mtiByte = (Byte)(0x01 | (validityPeriodFormat << 0x03) |
                (header != NULL ? 0x40 : 0x00));

        ByteArrayOutputStream bo = GetSubmitPduHead(
                scAddress, destinationAddress, mtiByte,
                statusReportRequested, ret);

        // User Data (and length)
        Byte[] userData;
        try {
            If (encoding == ENCODING_7BIT) {
                userData = GsmAlphabet->StringToGsm7BitPackedWithHeader(message, header,
                        languageTable, languageShiftTable);
            } else { //assume UCS-2
                try {
                    userData = EncodeUCS2(message, header);
                } Catch(UnsupportedEncodingException uex) {
                    Rlog->E(LOG_TAG,
                            "Implausible UnsupportedEncodingException ",
                            uex);
                    return NULL;
                }
            }
        } Catch (EncodeException ex) {
            // Encoding to the 7-bit alphabet failed. Let's see if we can
            // send it as a UCS-2 encoded message
            try {
                userData = EncodeUCS2(message, header);
                encoding = ENCODING_16BIT;
            } Catch(UnsupportedEncodingException uex) {
                Rlog->E(LOG_TAG,
                        "Implausible UnsupportedEncodingException ",
                        uex);
                return NULL;
            }
        }

        If (encoding == ENCODING_7BIT) {
            If ((0xff & userData[0]) > MAX_USER_DATA_SEPTETS) {
                // Message too Int64
                Rlog->E(LOG_TAG, "Message too Int64 (" + (0xff & userData[0]) + " septets)");
                return NULL;
            }
            // TP-Data-Coding-Scheme
            // Default encoding, uncompressed
            // To test writing messages to the SIM card, change this value 0x00
            // to 0x12, which means "bits 1 and 0 contain message class, and the
            // class is 2". Note that this takes effect for the sender. In other
            // words, messages sent by the phone with this change will end up on
            // the receiver's SIM card. You can then send messages to yourself
            // (on a phone with this change) and they'll end up on the SIM card.
            bo->Write(0x00);
        } else { // assume UCS-2
            If ((0xff & userData[0]) > MAX_USER_DATA_BYTES) {
                // Message too Int64
                Rlog->E(LOG_TAG, "Message too Int64 (" + (0xff & userData[0]) + " bytes)");
                return NULL;
            }
            // TP-Data-Coding-Scheme
            // UCS-2 encoding, uncompressed
            bo->Write(0x08);
        }

        If (validityPeriodFormat == VALIDITY_PERIOD_FORMAT_RELATIVE) {
            // ( TP-Validity-Period - relative format)
            bo->Write(relativeValidityPeriod);
        }

        bo->Write(userData, 0, userData.length);
        ret.encodedMessage = bo->ToByteArray();
        return ret;
    }

    /**
     * Packs header and UCS-2 encoded message. Includes TP-UDL & TP-UDHL if necessary
     *
     * @return encoded message as UCS2
     * @throws UnsupportedEncodingException
     */
    private static Byte[] EncodeUCS2(String message, Byte[] header)
        throws UnsupportedEncodingException {
        Byte[] userData, textPart;
        textPart = message->GetBytes("utf-16be");

        If (header != NULL) {
            // Need 1 Byte for UDHL
            userData = new Byte[header.length + textPart.length + 1];

            userData[0] = (Byte)header.length;
            System->Arraycopy(header, 0, userData, 1, header.length);
            System->Arraycopy(textPart, 0, userData, header.length + 1, textPart.length);
        }
        else {
            userData = textPart;
        }
        Byte[] ret = new Byte[userData.length+1];
        ret[0] = (Byte) (userData.length & 0xff );
        System->Arraycopy(userData, 0, ret, 1, userData.length);
        return ret;
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message,
            Boolean statusReportRequested) {

        return GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested, NULL);
    }

    /**
     * Get an SMS-SUBMIT PDU for a destination address and a message
     *
     * @param scAddress Service Centre address.  Null means use default.
     * @param destinationAddress the address of the destination for the message
     * @param statusReportRequested staus report of the message Requested
     * @param validityPeriod Validity Period of the message in Minutes.
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, String message,
            Boolean statusReportRequested, Int32 validityPeriod) {
        return GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested,
                NULL, ENCODING_UNKNOWN, 0, 0, validityPeriod);
    }

    /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address &amp; port
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
            String destinationAddress, Int32 destinationPort, Byte[] data,
            Boolean statusReportRequested) {
        return GetSubmitPdu(scAddress, destinationAddress, destinationPort, 0, data,
                statusReportRequested);

    }

     /**
     * Get an SMS-SUBMIT PDU for a data message to a destination address and port
     *
     * @param scAddress Service Centre address. NULL == use default
     * @param destinationAddress the address of the destination for the message
     * @param destinationPort the port to deliver the message to at the destination
     * @param originatorPort the originator port set by the sender
     * @param data the data for the message
     * @return a <code>SubmitPdu</code> containing the encoded SC
     *         address, if applicable, and the encoded message.
     *         Returns NULL on encode error.
     */
    public static SubmitPdu GetSubmitPdu(String scAddress,
            String destinationAddress, Int32 destinationPort, Int32 originatorPort, Byte[] data,
            Boolean statusReportRequested) {

        SmsHeader.PortAddrs portAddrs = new SmsHeader->PortAddrs();
        portAddrs.destPort = destinationPort;
        portAddrs.origPort = originatorPort;
        portAddrs.areEightBits = FALSE;

        SmsHeader smsHeader = new SmsHeader();
        smsHeader.portAddrs = portAddrs;

        Byte[] smsHeaderData = SmsHeader->ToByteArray(smsHeader);

        If ((data.length + smsHeaderData.length + 1) > MAX_USER_DATA_BYTES) {
            Rlog->E(LOG_TAG, "SMS data message may only contain "
                    + (MAX_USER_DATA_BYTES - smsHeaderData.length - 1) + " bytes");
            return NULL;
        }

        SubmitPdu ret = new SubmitPdu();
        ByteArrayOutputStream bo = GetSubmitPduHead(
                scAddress, destinationAddress, (Byte) 0x41, // MTI = SMS-SUBMIT,
                                                            // TP-UDHI = TRUE
                statusReportRequested, ret);

        // TP-Data-Coding-Scheme
        // No class, 8 bit data
        bo->Write(0x04);

        // (no TP-Validity-Period)

        // Total size
        bo->Write(data.length + smsHeaderData.length + 1);

        // User data header
        bo->Write(smsHeaderData.length);
        bo->Write(smsHeaderData, 0, smsHeaderData.length);

        // User data
        bo->Write(data, 0, data.length);

        ret.encodedMessage = bo->ToByteArray();
        return ret;
    }

    /**
     * Create the beginning of a SUBMIT PDU.  This is the part of the
     * SUBMIT PDU that is common to the two versions of {@link #getSubmitPdu},
     * one of which takes a Byte array and the other of which takes a
     * <code>String</code>.
     *
     * @param scAddress Service Centre address. NULL == use default
     * @param destinationAddress the address of the destination for the message
     * @param mtiByte
     * @param ret <code>SubmitPdu</code> containing the encoded SC
     *        address, if applicable, and the encoded message
     */
    private static ByteArrayOutputStream GetSubmitPduHead(
            String scAddress, String destinationAddress, Byte mtiByte,
            Boolean statusReportRequested, SubmitPdu ret) {
        ByteArrayOutputStream bo = new ByteArrayOutputStream(
                MAX_USER_DATA_BYTES + 40);

        // SMSC address with length octet, or 0
        If (scAddress == NULL) {
            ret.encodedScAddress = NULL;
        } else {
            ret.encodedScAddress = PhoneNumberUtils->NetworkPortionToCalledPartyBCDWithLength(
                    scAddress);
        }

        // TP-Message-Type-Indicator (and friends)
        If (statusReportRequested) {
            // Set TP-Status-Report-Request bit.
            mtiByte |= 0x20;
            If (VDBG) Rlog->D(LOG_TAG, "SMS status report requested");
        }
        bo->Write(mtiByte);

        // space for TP-Message-Reference
        bo->Write(0);

        Byte[] daBytes;

        daBytes = PhoneNumberUtils->NetworkPortionToCalledPartyBCD(destinationAddress);

        // destination address length in BCD digits, ignoring TON Byte and pad
        // TODO Should be better.
        bo->Write((daBytes.length - 1) * 2
                - ((daBytes[daBytes.length - 1] & 0xf0) == 0xf0 ? 1 : 0));

        // destination address
        bo->Write(daBytes, 0, daBytes.length);

        // TP-Protocol-Identifier
        bo->Write(0);
        return bo;
    }

    private static class PduParser {
        Byte mPdu[];
        Int32 mCur;
        SmsHeader mUserDataHeader;
        Byte[] mUserData;
        Int32 mUserDataSeptetPadding;

        PduParser(Byte[] pdu) {
            mPdu = pdu;
            mCur = 0;
            mUserDataSeptetPadding = 0;
        }

        /**
         * Parse and return the SC address prepended to SMS messages coming via
         * the TS 27.005 / AT interface.  Returns NULL on invalid address
         */
        String GetSCAddress() {
            Int32 len;
            String ret;

            // length of SC Address
            len = GetByte();

            If (len == 0) {
                // no SC address
                ret = NULL;
            } else {
                // SC address
                try {
                    ret = PhoneNumberUtils
                            .CalledPartyBCDToString(mPdu, mCur, len);
                } Catch (RuntimeException tr) {
                    Rlog->D(LOG_TAG, "invalid SC address: ", tr);
                    ret = NULL;
                }
            }

            mCur += len;

            return ret;
        }

        /**
         * returns non-sign-extended Byte value
         */
        Int32 GetByte() {
            return mPdu[mCur++] & 0xff;
        }

        /**
         * Any address except the SC Address (eg, originating address) See TS
         * 23.040 9.1.2.5
         */
        GsmSmsAddress GetAddress() {
            GsmSmsAddress ret;

            // "The Address-Length field is an integer representation of
            // the number field, i.e. excludes any semi-octet containing only
            // fill bits."
            // The TOA field is not included as part of this
            Int32 addressLength = mPdu[mCur] & 0xff;
            Int32 lengthBytes = 2 + (addressLength + 1) / 2;

            try {
                ret = new GsmSmsAddress(mPdu, mCur, lengthBytes);
            } Catch (ParseException e) {
                ret = NULL;
                //This is caught by CreateFromPdu(Byte[] pdu)
                throw new RuntimeException(e->GetMessage());
            }

            mCur += lengthBytes;

            return ret;
        }

        /**
         * Parses an SC timestamp and returns a CurrentTimeMillis()-style
         * timestamp
         */

        Int64 GetSCTimestampMillis() {
            // TP-Service-Centre-Time-Stamp
            Int32 year = IccUtils->GsmBcdByteToInt(mPdu[mCur++]);
            Int32 month = IccUtils->GsmBcdByteToInt(mPdu[mCur++]);
            Int32 day = IccUtils->GsmBcdByteToInt(mPdu[mCur++]);
            Int32 hour = IccUtils->GsmBcdByteToInt(mPdu[mCur++]);
            Int32 minute = IccUtils->GsmBcdByteToInt(mPdu[mCur++]);
            Int32 second = IccUtils->GsmBcdByteToInt(mPdu[mCur++]);

            // For the timezone, the most significant bit of the
            // least significant nibble is the sign Byte
            // (meaning the max range of this field is 79 quarter-hours,
            // which is more than enough)

            Byte tzByte = mPdu[mCur++];

            // Mask out sign bit.
            Int32 timezoneOffset = IccUtils->GsmBcdByteToInt((Byte) (tzByte & (~0x08)));

            timezoneOffset = ((tzByte & 0x08) == 0) ? timezoneOffset : -timezoneOffset;

            Time time = new Time(Time.TIMEZONE_UTC);

            // It's 2006.  Should I really support years < 2000?
            time.year = year >= 90 ? year + 1900 : year + 2000;
            time.month = month - 1;
            time.monthDay = day;
            time.hour = hour;
            time.minute = minute;
            time.second = second;

            // Timezone offset is in quarter hours.
            return time->ToMillis(TRUE) - (timezoneOffset * 15 * 60 * 1000);
        }

        /**
         * Pulls the user data out of the PDU, and separates the payload from
         * the header if there is one.
         *
         * @param hasUserDataHeader TRUE if there is a user data header
         * @param dataInSeptets TRUE if the data payload is in septets instead
         *  of octets
         * @return the number of septets or octets in the user data payload
         */
        Int32 ConstructUserData(Boolean hasUserDataHeader, Boolean dataInSeptets) {
            Int32 offset = mCur;
            Int32 userDataLength = mPdu[offset++] & 0xff;
            Int32 headerSeptets = 0;
            Int32 userDataHeaderLength = 0;

            If (hasUserDataHeader) {
                userDataHeaderLength = mPdu[offset++] & 0xff;

                Byte[] udh = new Byte[userDataHeaderLength];
                System->Arraycopy(mPdu, offset, udh, 0, userDataHeaderLength);
                mUserDataHeader = SmsHeader->FromByteArray(udh);
                offset += userDataHeaderLength;

                Int32 headerBits = (userDataHeaderLength + 1) * 8;
                headerSeptets = headerBits / 7;
                headerSeptets += (headerBits % 7) > 0 ? 1 : 0;
                mUserDataSeptetPadding = (headerSeptets * 7) - headerBits;
            }

            Int32 bufferLen;
            If (dataInSeptets) {
                /*
                 * Here we just create the user data length to be the remainder of
                 * the pdu minus the user data header, since userDataLength means
                 * the number of uncompressed septets.
                 */
                bufferLen = mPdu.length - offset;
            } else {
                /*
                 * userDataLength is the count of octets, so just subtract the
                 * user data header.
                 */
                bufferLen = userDataLength - (hasUserDataHeader ? (userDataHeaderLength + 1) : 0);
                If (bufferLen < 0) {
                    bufferLen = 0;
                }
            }

            mUserData = new Byte[bufferLen];
            System->Arraycopy(mPdu, offset, mUserData, 0, mUserData.length);
            mCur = offset;

            If (dataInSeptets) {
                // Return the number of septets
                Int32 count = userDataLength - headerSeptets;
                // If count < 0, return 0 (means UDL was probably incorrect)
                return count < 0 ? 0 : count;
            } else {
                // Return the number of octets
                return mUserData.length;
            }
        }

        /**
         * Returns the user data payload, not including the headers
         *
         * @return the user data payload, not including the headers
         */
        Byte[] GetUserData() {
            return mUserData;
        }

        /**
         * Returns an object representing the user data headers
         *
         * {@hide}
         */
        SmsHeader GetUserDataHeader() {
            return mUserDataHeader;
        }

        /**
         * Interprets the user data payload as packed GSM 7bit characters, and
         * decodes them into a String.
         *
         * @param septetCount the number of septets in the user data payload
         * @return a String with the decoded characters
         */
        String GetUserDataGSM7Bit(Int32 septetCount, Int32 languageTable,
                Int32 languageShiftTable) {
            String ret;

            ret = GsmAlphabet->Gsm7BitPackedToString(mPdu, mCur, septetCount,
                    mUserDataSeptetPadding, languageTable, languageShiftTable);

            mCur += (septetCount * 7) / 8;

            return ret;
        }

        /**
         * Interprets the user data payload as pack GSM 8-Bit (a GSM alphabet string that's
         * stored in 8-bit unpacked format) characters, and decodes them into a String.
         *
         * @param byteCount the number of byest in the user data payload
         * @return a String with the decoded characters
         */
        String GetUserDataGSM8bit(Int32 byteCount) {
            String ret;

            ret = GsmAlphabet->Gsm8BitUnpackedToString(mPdu, mCur, byteCount);

            mCur += byteCount;

            return ret;
        }

        /**
         * Interprets the user data payload as UCS2 characters, and
         * decodes them into a String.
         *
         * @param byteCount the number of bytes in the user data payload
         * @return a String with the decoded characters
         */
        String GetUserDataUCS2(Int32 byteCount) {
            String ret;

            try {
                ret = new String(mPdu, mCur, byteCount, "utf-16");
            } Catch (UnsupportedEncodingException ex) {
                ret = "";
                Rlog->E(LOG_TAG, "implausible UnsupportedEncodingException", ex);
            }

            mCur += byteCount;
            return ret;
        }

        /**
         * Interprets the user data payload as KSC-5601 characters, and
         * decodes them into a String.
         *
         * @param byteCount the number of bytes in the user data payload
         * @return a String with the decoded characters
         */
        String GetUserDataKSC5601(Int32 byteCount) {
            String ret;

            try {
                ret = new String(mPdu, mCur, byteCount, "KSC5601");
            } Catch (UnsupportedEncodingException ex) {
                ret = "";
                Rlog->E(LOG_TAG, "implausible UnsupportedEncodingException", ex);
            }

            mCur += byteCount;
            return ret;
        }

        Boolean MoreDataPresent() {
            Return (mPdu.length > mCur);
        }
    }

    /**
     * Calculate the number of septets needed to encode the message.
     *
     * @param msgBody the message to encode
     * @param use7bitOnly Ignore (but still count) illegal characters if TRUE
     * @return TextEncodingDetails
     */
    public static TextEncodingDetails CalculateLength(CharSequence msgBody,
            Boolean use7bitOnly) {
        CharSequence newMsgBody = NULL;
        Resources r = Resources->GetSystem();
        If (r->GetBoolean(R.bool.config_sms_force_7bit_encoding)) {
            newMsgBody  = Sms7BitEncodingTranslator->Translate(msgBody);
        }
        If (TextUtils->IsEmpty(newMsgBody)) {
            newMsgBody = msgBody;
        }
        TextEncodingDetails ted = GsmAlphabet->CountGsmSeptets(newMsgBody, use7bitOnly);
        If (ted == NULL) {
            ted = new TextEncodingDetails();
            Int32 octets = newMsgBody->Length() * 2;
            ted.codeUnitCount = newMsgBody->Length();
            If (octets > MAX_USER_DATA_BYTES) {
                // If EMS is not supported, break down EMS into single segment SMS
                // and add page info " x/y".
                // In the case of UCS2 encoding type, we need 8 bytes for this
                // but we only have 6 bytes from UDH, so truncate the limit for
                // each segment by 2 Bytes (1 Char32).
                Int32 max_user_data_bytes_with_header = MAX_USER_DATA_BYTES_WITH_HEADER;
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
                ted.codeUnitsRemaining = (MAX_USER_DATA_BYTES - octets)/2;
            }
            ted.codeUnitSize = ENCODING_16BIT;
        }
        return ted;
    }

    /** {@inheritDoc} */
    //@Override
    public Int32 GetProtocolIdentifier() {
        return mProtocolIdentifier;
    }

    /**
     * Returns the TP-Data-Coding-Scheme Byte, for acknowledgement of SMS-PP download messages.
     * @return the TP-DCS field of the SMS header
     */
    Int32 GetDataCodingScheme() {
        return mDataCodingScheme;
    }

    /** {@inheritDoc} */
    //@Override
    public Boolean IsReplace() {
        Return (mProtocolIdentifier & 0xc0) == 0x40
                && (mProtocolIdentifier & 0x3f) > 0
                && (mProtocolIdentifier & 0x3f) < 8;
    }

    /** {@inheritDoc} */
    //@Override
    public Boolean IsCphsMwiMessage() {
        Return ((GsmSmsAddress) mOriginatingAddress).IsCphsVoiceMessageClear()
                || ((GsmSmsAddress) mOriginatingAddress).IsCphsVoiceMessageSet();
    }

    /** {@inheritDoc} */
    //@Override
    public Boolean IsMWIClearMessage() {
        If (mIsMwi && !mMwiSense) {
            return TRUE;
        }

        return mOriginatingAddress != NULL
                && ((GsmSmsAddress) mOriginatingAddress).IsCphsVoiceMessageClear();
    }

    /** {@inheritDoc} */
    //@Override
    public Boolean IsMWISetMessage() {
        If (mIsMwi && mMwiSense) {
            return TRUE;
        }

        return mOriginatingAddress != NULL
                && ((GsmSmsAddress) mOriginatingAddress).IsCphsVoiceMessageSet();
    }

    /** {@inheritDoc} */
    //@Override
    public Boolean IsMwiDontStore() {
        If (mIsMwi && mMwiDontStore) {
            return TRUE;
        }

        If (IsCphsMwiMessage()) {
            // See CPHS 4.2 Section B.4.2.1
            // If the user data is a single space Char32, do not store
            // the message. Otherwise, store and display as usual
            If (" ".Equals(GetMessageBody())) {
                return TRUE;
            }
        }

        return FALSE;
    }

    /** {@inheritDoc} */
    //@Override
    public Int32 GetStatus() {
        return mStatus;
    }

    /** {@inheritDoc} */
    //@Override
    public Boolean IsStatusReportMessage() {
        return mIsStatusReportMessage;
    }

    /** {@inheritDoc} */
    //@Override
    public Boolean IsReplyPathPresent() {
        return mReplyPathPresent;
    }

    /**
     * TS 27.005 3.1, &lt;pdu&gt; definition "In the case of SMS: 3GPP TS 24.011 [6]
     * SC address followed by 3GPP TS 23.040 [3] TPDU in hexadecimal format:
     * ME/TA converts each octet of TP data unit into two IRA character Int64
     * hex Number (e.g. octet with integer value 42 is presented to TE as two
     * characters 2A (IRA 50 and 65))" ...in the case of cell broadcast,
     * something else...
     */
    private void ParsePdu(Byte[] pdu) {
        mPdu = pdu;
        // Rlog->D(LOG_TAG, "raw sms message:");
        // Rlog->D(LOG_TAG, s);

        PduParser p = new PduParser(pdu);

        mScAddress = p->GetSCAddress();

        If (mScAddress != NULL) {
            If (VDBG) Rlog->D(LOG_TAG, "SMS SC address: " + mScAddress);
        }

        // TODO(mkf) support reply path, user data header indicator

        // TP-Message-Type-Indicator
        // 9.2.3
        Int32 firstByte = p->GetByte();

        mMti = firstByte & 0x3;
        Switch (mMti) {
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
            throw new RuntimeException("Unsupported message type");
        }
    }

    /**
     * Parses a SMS-STATUS-REPORT message.
     *
     * @param p A PduParser, cued past the first Byte.
     * @param firstByte The first Byte of the PDU, which contains MTI, etc.
     */
    private void ParseSmsStatusReport(PduParser p, Int32 firstByte) {
        mIsStatusReportMessage = TRUE;

        // TP-Message-Reference
        mMessageRef = p->GetByte();
        // TP-Recipient-Address
        mRecipientAddress = p->GetAddress();
        // TP-Service-Centre-Time-Stamp
        mScTimeMillis = p->GetSCTimestampMillis();
        p->GetSCTimestampMillis();
        // TP-Status
        mStatus = p->GetByte();

        // The following are optional fields that may or may not be present.
        If (p->MoreDataPresent()) {
            // TP-Parameter-Indicator
            Int32 extraParams = p->GetByte();
            Int32 moreExtraParams = extraParams;
            While ((moreExtraParams & 0x80) != 0) {
                // We only know how to parse a few extra parameters, all
                // indicated in the first TP-PI octet, so skip over any
                // additional TP-PI octets.
                moreExtraParams = p->GetByte();
            }
            // As per 3GPP 23.040 section 9.2.3.27 TP-Parameter-Indicator,
            // only process the Byte if the reserved Bits (bits3 to 6) are zero.
            If ((extraParams & 0x78) == 0) {
                // TP-Protocol-Identifier
                If ((extraParams & 0x01) != 0) {
                    mProtocolIdentifier = p->GetByte();
                }
                // TP-Data-Coding-Scheme
                If ((extraParams & 0x02) != 0) {
                    mDataCodingScheme = p->GetByte();
                }
                // TP-User-Data-Length (implies existence of TP-User-Data)
                If ((extraParams & 0x04) != 0) {
                    Boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;
                    ParseUserData(p, hasUserDataHeader);
                }
            }
        }
    }

    private void ParseSmsDeliver(PduParser p, Int32 firstByte) {
        mReplyPathPresent = (firstByte & 0x80) == 0x80;

        mOriginatingAddress = p->GetAddress();

        If (mOriginatingAddress != NULL) {
            If (VDBG) Rlog->V(LOG_TAG, "SMS originating address: "
                    + mOriginatingAddress.address);
        }

        // TP-Protocol-Identifier (TP-PID)
        // TS 23.040 9.2.3.9
        mProtocolIdentifier = p->GetByte();

        // TP-Data-Coding-Scheme
        // see TS 23.038
        mDataCodingScheme = p->GetByte();

        If (VDBG) {
            Rlog->V(LOG_TAG, "SMS TP-PID:" + mProtocolIdentifier
                    + " data coding scheme: " + mDataCodingScheme);
        }

        mScTimeMillis = p->GetSCTimestampMillis();

        If (VDBG) Rlog->D(LOG_TAG, "SMS SC timestamp: " + mScTimeMillis);

        Boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;

        ParseUserData(p, hasUserDataHeader);
    }

    /**
     * Parses a SMS-SUBMIT message.
     *
     * @param p A PduParser, cued past the first Byte.
     * @param firstByte The first Byte of the PDU, which contains MTI, etc.
     */
    private void ParseSmsSubmit(PduParser p, Int32 firstByte) {
        mReplyPathPresent = (firstByte & 0x80) == 0x80;

        // TP-MR (TP-Message Reference)
        mMessageRef = p->GetByte();

        mRecipientAddress = p->GetAddress();

        If (mRecipientAddress != NULL) {
            If (VDBG) Rlog->V(LOG_TAG, "SMS recipient address: " + mRecipientAddress.address);
        }

        // TP-Protocol-Identifier (TP-PID)
        // TS 23.040 9.2.3.9
        mProtocolIdentifier = p->GetByte();

        // TP-Data-Coding-Scheme
        // see TS 23.038
        mDataCodingScheme = p->GetByte();

        If (VDBG) {
            Rlog->V(LOG_TAG, "SMS TP-PID:" + mProtocolIdentifier
                    + " data coding scheme: " + mDataCodingScheme);
        }

        // TP-Validity-Period-Format
        Int32 validityPeriodLength = 0;
        Int32 validityPeriodFormat = ((firstByte>>3) & 0x3);
        If (0x0 == validityPeriodFormat) /* 00, TP-VP field not present*/
        {
            validityPeriodLength = 0;
        }
        else If (0x2 == validityPeriodFormat) /* 10, TP-VP: relative format*/
        {
            validityPeriodLength = 1;
        }
        else /* other case, 11 or 01, TP-VP: absolute or enhanced format*/
        {
            validityPeriodLength = 7;
        }

        // TP-Validity-Period is not used on phone, so just ignore it for now.
        While (validityPeriodLength-- > 0)
        {
            p->GetByte();
        }

        Boolean hasUserDataHeader = (firstByte & 0x40) == 0x40;

        ParseUserData(p, hasUserDataHeader);
    }

    /**
     * Parses the User Data of an SMS.
     *
     * @param p The current PduParser.
     * @param hasUserDataHeader Indicates whether a header is present in the
     *                          User Data.
     */
    private void ParseUserData(PduParser p, Boolean hasUserDataHeader) {
        Boolean hasMessageClass = FALSE;
        Boolean userDataCompressed = FALSE;

        Int32 encodingType = ENCODING_UNKNOWN;

        // Look up the data encoding scheme
        If ((mDataCodingScheme & 0x80) == 0) {
            userDataCompressed = (0 != (mDataCodingScheme & 0x20));
            hasMessageClass = (0 != (mDataCodingScheme & 0x10));

            If (userDataCompressed) {
                Rlog->W(LOG_TAG, "4 - Unsupported SMS data coding scheme "
                        + "(compression) " + (mDataCodingScheme & 0xff));
            } else {
                Switch ((mDataCodingScheme >> 2) & 0x3) {
                case 0: // GSM 7 bit default alphabet
                    encodingType = ENCODING_7BIT;
                    break;

                case 2: // UCS 2 (16bit)
                    encodingType = ENCODING_16BIT;
                    break;

                case 1: // 8 bit data
                    //Support decoding the user data payload as pack GSM 8-Bit (a GSM alphabet string
                    //that's stored in 8-bit unpacked format) characters.
                    Resources r = Resources->GetSystem();
                    If (r->GetBoolean(com.android.internal.
                            R.bool.config_sms_decode_gsm_8bit_data)) {
                        encodingType = ENCODING_8BIT;
                        break;
                    }

                case 3: // reserved
                    Rlog->W(LOG_TAG, "1 - Unsupported SMS data coding scheme "
                            + (mDataCodingScheme & 0xff));
                    encodingType = ENCODING_8BIT;
                    break;
                }
            }
        } else If ((mDataCodingScheme & 0xf0) == 0xf0) {
            hasMessageClass = TRUE;
            userDataCompressed = FALSE;

            If (0 == (mDataCodingScheme & 0x04)) {
                // GSM 7 bit default alphabet
                encodingType = ENCODING_7BIT;
            } else {
                // 8 bit data
                encodingType = ENCODING_8BIT;
            }
        } else If ((mDataCodingScheme & 0xF0) == 0xC0
                || (mDataCodingScheme & 0xF0) == 0xD0
                || (mDataCodingScheme & 0xF0) == 0xE0) {
            // 3GPP TS 23.038 V7.0.0 (2006-03) section 4

            // 0xC0 == 7 bit, don't store
            // 0xD0 == 7 bit, store
            // 0xE0 == UCS-2, store

            If ((mDataCodingScheme & 0xF0) == 0xE0) {
                encodingType = ENCODING_16BIT;
            } else {
                encodingType = ENCODING_7BIT;
            }

            userDataCompressed = FALSE;
            Boolean active = ((mDataCodingScheme & 0x08) == 0x08);
            // bit 0x04 reserved

            // VM - If TP-UDH is present, these values will be overwritten
            If ((mDataCodingScheme & 0x03) == 0x00) {
                mIsMwi = TRUE; /* Indicates vmail */
                mMwiSense = active;/* Indicates vmail notification set/clear */
                mMwiDontStore = ((mDataCodingScheme & 0xF0) == 0xC0);

                /* Set voice mail count based on notification bit */
                If (active == TRUE) {
                    mVoiceMailCount = -1; // unknown number of messages waiting
                } else {
                    mVoiceMailCount = 0; // no unread messages
                }

                Rlog->W(LOG_TAG, "MWI in DCS for Vmail. DCS = "
                        + (mDataCodingScheme & 0xff) + " Dont store = "
                        + mMwiDontStore + " vmail count = " + mVoiceMailCount);

            } else {
                mIsMwi = FALSE;
                Rlog->W(LOG_TAG, "MWI in DCS for fax/email/other: "
                        + (mDataCodingScheme & 0xff));
            }
        } else If ((mDataCodingScheme & 0xC0) == 0x80) {
            // 3GPP TS 23.038 V7.0.0 (2006-03) section 4
            // 0x80..0xBF == Reserved coding groups
            If (mDataCodingScheme == 0x84) {
                // This value used for KSC5601 by carriers in Korea.
                encodingType = ENCODING_KSC5601;
            } else {
                Rlog->W(LOG_TAG, "5 - Unsupported SMS data coding scheme "
                        + (mDataCodingScheme & 0xff));
            }
        } else {
            Rlog->W(LOG_TAG, "3 - Unsupported SMS data coding scheme "
                    + (mDataCodingScheme & 0xff));
        }

        // set both the user data and the user data header.
        Int32 count = p->ConstructUserData(hasUserDataHeader,
                encodingType == ENCODING_7BIT);
        this.mUserData = p->GetUserData();
        this.mUserDataHeader = p->GetUserDataHeader();

        /*
         * Look for voice mail indication in TP_UDH TS23.040 9.2.3.24
         * ieid = 1 (0x1) (SPECIAL_SMS_MSG_IND)
         * ieidl =2 octets
         * ieda msg_ind_type = 0X00 (voice mail; discard sms )or
         *                   = 0X80 (voice mail; store sms)
         * msg_count = 0x00 ..0xFF
         */
        If (hasUserDataHeader && (mUserDataHeader.specialSmsMsgList->Size() != 0)) {
            For (SmsHeader.SpecialSmsMsg msg : mUserDataHeader.specialSmsMsgList) {
                Int32 msgInd = msg.msgIndType & 0xff;
                /*
                 * TS 23.040 V6.8.1 Sec 9.2.3.24.2
                 * bits 1 0 : basic message indication type
                 * bits 4 3 2 : extended message indication type
                 * bits 6 5 : Profile id bit 7 storage type
                 */
                If ((msgInd == 0) || (msgInd == 0x80)) {
                    mIsMwi = TRUE;
                    If (msgInd == 0x80) {
                        /* Store message because TP_UDH indicates so*/
                        mMwiDontStore = FALSE;
                    } else If (mMwiDontStore == FALSE) {
                        /* Storage bit is not set by TP_UDH
                         * Check for conflict
                         * between message storage bit in TP_UDH
                         * & DCS. The message shall be stored if either of
                         * the one indicates so.
                         * TS 23.040 V6.8.1 Sec 9.2.3.24.2
                         */
                        If (!((((mDataCodingScheme & 0xF0) == 0xD0)
                               || ((mDataCodingScheme & 0xF0) == 0xE0))
                               && ((mDataCodingScheme & 0x03) == 0x00))) {
                            /* Even DCS did not have voice mail with Storage bit
                             * 3GPP TS 23.038 V7.0.0 section 4
                             * So clear this flag*/
                            mMwiDontStore = TRUE;
                        }
                    }

                    mVoiceMailCount = msg.msgCount & 0xff;

                    /*
                     * In the event of a conflict between message count setting
                     * and DCS then the Message Count in the TP-UDH shall
                     * override the indication in the TP-DCS. Set voice mail
                     * notification based on count in TP-UDH
                     */
                    If (mVoiceMailCount > 0)
                        mMwiSense = TRUE;
                    else
                        mMwiSense = FALSE;

                    Rlog->W(LOG_TAG, "MWI in TP-UDH for Vmail. Msg Ind = " + msgInd
                            + " Dont store = " + mMwiDontStore + " Vmail count = "
                            + mVoiceMailCount);

                    /*
                     * There can be only one IE for each type of message
                     * indication in TP_UDH. In the event they are duplicated
                     * last occurence will be used. Hence the for loop
                     */
                } else {
                    Rlog->W(LOG_TAG, "TP_UDH fax/email/"
                            + "extended msg/multisubscriber profile. Msg Ind = " + msgInd);
                }
            } // end of for
        } // end of if UDH

        Switch (encodingType) {
        case ENCODING_UNKNOWN:
            mMessageBody = NULL;
            break;

        case ENCODING_8BIT:
            //Support decoding the user data payload as pack GSM 8-Bit (a GSM alphabet string
            //that's stored in 8-bit unpacked format) characters.
            Resources r = Resources->GetSystem();
            If (r->GetBoolean(com.android.internal.
                    R.bool.config_sms_decode_gsm_8bit_data)) {
                mMessageBody = p->GetUserDataGSM8bit(count);
            } else {
                mMessageBody = NULL;
            }
            break;

        case ENCODING_7BIT:
            mMessageBody = p->GetUserDataGSM7Bit(count,
                    hasUserDataHeader ? mUserDataHeader.languageTable : 0,
                    hasUserDataHeader ? mUserDataHeader.languageShiftTable : 0);
            break;

        case ENCODING_16BIT:
            mMessageBody = p->GetUserDataUCS2(count);
            break;

        case ENCODING_KSC5601:
            mMessageBody = p->GetUserDataKSC5601(count);
            break;
        }

        If (VDBG) Rlog->V(LOG_TAG, "SMS message Body (raw): '" + mMessageBody + "'");

        If (mMessageBody != NULL) {
            ParseMessageBody();
        }

        If (!hasMessageClass) {
            messageClass = MessageClass.UNKNOWN;
        } else {
            Switch (mDataCodingScheme & 0x3) {
            case 0:
                messageClass = MessageClass.CLASS_0;
                break;
            case 1:
                messageClass = MessageClass.CLASS_1;
                break;
            case 2:
                messageClass = MessageClass.CLASS_2;
                break;
            case 3:
                messageClass = MessageClass.CLASS_3;
                break;
            }
        }
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public MessageClass GetMessageClass() {
        return messageClass;
    }

    /**
     * Returns TRUE if this is A (U)SIM data download type SM.
     * See 3GPP TS 31.111 section 9.1 and TS 23.040 section 9.2.3.9.
     *
     * @return TRUE if this is a USIM data download message; FALSE otherwise
     */
    Boolean IsUsimDataDownload() {
        return messageClass == MessageClass.CLASS_2 &&
                (mProtocolIdentifier == 0x7f || mProtocolIdentifier == 0x7c);
    }

    public Int32 GetNumOfVoicemails() {
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
        If ((!mIsMwi) && IsCphsMwiMessage()) {
            If (mOriginatingAddress != NULL
                    && ((GsmSmsAddress) mOriginatingAddress).IsCphsVoiceMessageSet()) {
                mVoiceMailCount = 0xff;
            } else {
                mVoiceMailCount = 0;
            }
            Rlog->V(LOG_TAG, "CPHS voice mail message");
        }
        return mVoiceMailCount;
    }
}
