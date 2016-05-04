/*
 * Copyright (C) 2007-2008 Esmertec AG.
 * Copyright (C) 2007-2008 The Android Open Source Project
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

package com.google.android.mms.pdu;

using Elastos::Droid::Utility::ILog;


public class SendReq extends MultimediaMessagePdu {
    private static const String TAG = "SendReq";

    public SendReq() {
        Super();

        try {
            SetMessageType(PduHeaders.MESSAGE_TYPE_SEND_REQ);
            SetMmsVersion(PduHeaders.CURRENT_MMS_VERSION);
            // FIXME: Content-type must be decided according to whether
            // SMIL part present.
            SetContentType("application/vnd.wap.multipart.related".GetBytes());
            SetFrom(new EncodedStringValue(PduHeaders.FROM_INSERT_ADDRESS_TOKEN_STR->GetBytes()));
            SetTransactionId(GenerateTransactionId());
        } Catch (InvalidHeaderValueException e) {
            // Impossible to reach here since all headers we set above are valid.
            Logger::E(TAG, "Unexpected InvalidHeaderValueException.", e);
            throw new RuntimeException(e);
        }
    }

    private Byte[] GenerateTransactionId() {
        String transactionId = "T" + Long->ToHexString(System->CurrentTimeMillis());
        return transactionId->GetBytes();
    }

    /**
     * Constructor, used when composing a M-Send.req pdu.
     *
     * @param contentType the content type value
     * @param from the from value
     * @param mmsVersion current viersion of mms
     * @param transactionId the transaction-id value
     * @throws InvalidHeaderValueException if parameters are invalid.
     *         NullPointerException if contentType, form or transactionId is NULL.
     */
    public SendReq(Byte[] contentType,
                   EncodedStringValue from,
                   Int32 mmsVersion,
                   Byte[] transactionId) throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_SEND_REQ);
        SetContentType(contentType);
        SetFrom(from);
        SetMmsVersion(mmsVersion);
        SetTransactionId(transactionId);
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    SendReq(PduHeaders headers) {
        Super(headers);
    }

    /**
     * Constructor with given headers and body
     *
     * @param headers Headers for this PDU.
     * @param body Body of this PDu.
     */
    SendReq(PduHeaders headers, PduBody body) {
        Super(headers, body);
    }

    /**
     * Get Bcc value.
     *
     * @return the value
     */
    public EncodedStringValue[] GetBcc() {
        return mPduHeaders->GetEncodedStringValues(PduHeaders.BCC);
    }

    /**
     * Add a "BCC" value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI AddBcc(EncodedStringValue value) {
        mPduHeaders->AppendEncodedStringValue(value, PduHeaders.BCC);
    }

    /**
     * Set "BCC" value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetBcc(EncodedStringValue[] value) {
        mPduHeaders->SetEncodedStringValues(value, PduHeaders.BCC);
    }

    /**
     * Get CC value.
     *
     * @return the value
     */
    public EncodedStringValue[] GetCc() {
        return mPduHeaders->GetEncodedStringValues(PduHeaders.CC);
    }

    /**
     * Add a "CC" value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI AddCc(EncodedStringValue value) {
        mPduHeaders->AppendEncodedStringValue(value, PduHeaders.CC);
    }

    /**
     * Set "CC" value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetCc(EncodedStringValue[] value) {
        mPduHeaders->SetEncodedStringValues(value, PduHeaders.CC);
    }

    /**
     * Get Content-type value.
     *
     * @return the value
     */
    public Byte[] GetContentType() {
        return mPduHeaders->GetTextString(PduHeaders.CONTENT_TYPE);
    }

    /**
     * Set Content-type value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetContentType(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.CONTENT_TYPE);
    }

    /**
     * Get X-Mms-Delivery-Report value.
     *
     * @return the value
     */
    public Int32 GetDeliveryReport() {
        return mPduHeaders->GetOctet(PduHeaders.DELIVERY_REPORT);
    }

    /**
     * Set X-Mms-Delivery-Report value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     */
    CARAPI SetDeliveryReport(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.DELIVERY_REPORT);
    }

    /**
     * Get X-Mms-Expiry value.
     *
     * Expiry-value = Value-length
     *      (Absolute-token Date-value | Relative-token Delta-seconds-value)
     *
     * @return the value
     */
    public Int64 GetExpiry() {
        return mPduHeaders->GetLongInteger(PduHeaders.EXPIRY);
    }

    /**
     * Set X-Mms-Expiry value.
     *
     * @param value the value
     */
    CARAPI SetExpiry(Int64 value) {
        mPduHeaders->SetLongInteger(value, PduHeaders.EXPIRY);
    }

    /**
     * Get X-Mms-MessageSize value.
     *
     * Expiry-value = size of message
     *
     * @return the value
     */
    public Int64 GetMessageSize() {
        return mPduHeaders->GetLongInteger(PduHeaders.MESSAGE_SIZE);
    }

    /**
     * Set X-Mms-MessageSize value.
     *
     * @param value the value
     */
    CARAPI SetMessageSize(Int64 value) {
        mPduHeaders->SetLongInteger(value, PduHeaders.MESSAGE_SIZE);
    }

    /**
     * Get X-Mms-Message-Class value.
     * Message-class-value = Class-identifier | Token-text
     * Class-identifier = Personal | Advertisement | Informational | Auto
     *
     * @return the value
     */
    public Byte[] GetMessageClass() {
        return mPduHeaders->GetTextString(PduHeaders.MESSAGE_CLASS);
    }

    /**
     * Set X-Mms-Message-Class value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetMessageClass(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.MESSAGE_CLASS);
    }

    /**
     * Get X-Mms-Read-Report value.
     *
     * @return the value
     */
    public Int32 GetReadReport() {
        return mPduHeaders->GetOctet(PduHeaders.READ_REPORT);
    }

    /**
     * Set X-Mms-Read-Report value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     */
    CARAPI SetReadReport(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.READ_REPORT);
    }

    /**
     * Set "To" value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetTo(EncodedStringValue[] value) {
        mPduHeaders->SetEncodedStringValues(value, PduHeaders.TO);
    }

    /**
     * Get X-Mms-Transaction-Id field value.
     *
     * @return the X-Mms-Report-Allowed value
     */
    public Byte[] GetTransactionId() {
        return mPduHeaders->GetTextString(PduHeaders.TRANSACTION_ID);
    }

    /**
     * Set X-Mms-Transaction-Id field value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetTransactionId(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.TRANSACTION_ID);
    }

    /*
     * Optional, not supported header fields:
     *
     *     public Byte GetAdaptationAllowed() {return 0};
     *     CARAPI SetAdaptationAllowed(btye value) {};
     *
     *     public Byte[] GetApplicId() {return NULL;}
     *     CARAPI SetApplicId(Byte[] value) {}
     *
     *     public Byte[] GetAuxApplicId() {return NULL;}
     *     CARAPI GetAuxApplicId(Byte[] value) {}
     *
     *     public Byte GetContentClass() {return 0x00;}
     *     CARAPI SetApplicId(Byte value) {}
     *
     *     public Int64 GetDeliveryTime() {return 0};
     *     CARAPI SetDeliveryTime(Int64 value) {};
     *
     *     public Byte GetDrmContent() {return 0x00;}
     *     CARAPI SetDrmContent(Byte value) {}
     *
     *     public MmFlagsValue GetMmFlags() {return NULL;}
     *     CARAPI SetMmFlags(MmFlagsValue value) {}
     *
     *     public MmStateValue GetMmState() {return NULL;}
     *     CARAPI GetMmState(MmStateValue value) {}
     *
     *     public Byte[] GetReplyApplicId() {return 0x00;}
     *     CARAPI SetReplyApplicId(Byte[] value) {}
     *
     *     public Byte GetReplyCharging() {return 0x00;}
     *     CARAPI SetReplyCharging(Byte value) {}
     *
     *     public Byte GetReplyChargingDeadline() {return 0x00;}
     *     CARAPI SetReplyChargingDeadline(Byte value) {}
     *
     *     public Byte[] GetReplyChargingId() {return 0x00;}
     *     CARAPI SetReplyChargingId(Byte[] value) {}
     *
     *     public Int64 GetReplyChargingSize() {return 0;}
     *     CARAPI SetReplyChargingSize(Int64 value) {}
     *
     *     public Byte[] GetReplyApplicId() {return 0x00;}
     *     CARAPI SetReplyApplicId(Byte[] value) {}
     *
     *     public Byte GetStore() {return 0x00;}
     *     CARAPI SetStore(Byte value) {}
     */
}
