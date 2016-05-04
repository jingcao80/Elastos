/*
 * Copyright (C) 2007 Esmertec AG.
 * Copyright (C) 2007 The Android Open Source Project
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


/**
 * M-Retrive.conf Pdu.
 */
public class RetrieveConf extends MultimediaMessagePdu {
    /**
     * Empty constructor.
     * Since the Pdu corresponding to this class is constructed
     * by the Proxy-Relay server, this class is only instantiated
     * by the Pdu Parser.
     *
     * @throws InvalidHeaderValueException if error occurs.
     */
    public RetrieveConf() throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF);
        SetTransactionId(GenerateTransactionId());
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    RetrieveConf(PduHeaders headers) {
        Super(headers);
        SetTransactionId(GenerateTransactionId());
    }

    /**
     * Constructor with given headers and body
     *
     * @param headers Headers for this PDU.
     * @param body Body of this PDu.
     */
    RetrieveConf(PduHeaders headers, PduBody body) {
        Super(headers, body);
        SetTransactionId(GenerateTransactionId());
    }

    private Byte[] GenerateTransactionId() {
        String transactionId = "T" + Long->ToHexString(System->CurrentTimeMillis());
        return transactionId->GetBytes();
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
     * Get From value.
     * From-value = Value-length
     *      (Address-present-token Encoded-string-value | Insert-address-token)
     *
     * @return the value
     */
    public EncodedStringValue GetFrom() {
       return mPduHeaders->GetEncodedStringValue(PduHeaders.FROM);
    }

    /**
     * Set From value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetFrom(EncodedStringValue value) {
        mPduHeaders->SetEncodedStringValue(value, PduHeaders.FROM);
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
     * Get Message-ID value.
     *
     * @return the value
     */
    public Byte[] GetMessageId() {
        return mPduHeaders->GetTextString(PduHeaders.MESSAGE_ID);
    }

    /**
     * Set Message-ID value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetMessageId(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.MESSAGE_ID);
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
     * Get X-Mms-Retrieve-Status value.
     *
     * @return the value
     */
    public Int32 GetRetrieveStatus() {
        return mPduHeaders->GetOctet(PduHeaders.RETRIEVE_STATUS);
    }

    /**
     * Set X-Mms-Retrieve-Status value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     */
    CARAPI SetRetrieveStatus(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.RETRIEVE_STATUS);
    }

    /**
     * Get X-Mms-Retrieve-Text value.
     *
     * @return the value
     */
    public EncodedStringValue GetRetrieveText() {
        return mPduHeaders->GetEncodedStringValue(PduHeaders.RETRIEVE_TEXT);
    }

    /**
     * Set X-Mms-Retrieve-Text value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetRetrieveText(EncodedStringValue value) {
        mPduHeaders->SetEncodedStringValue(value, PduHeaders.RETRIEVE_TEXT);
    }

    /**
     * Get X-Mms-Transaction-Id.
     *
     * @return the value
     */
    public Byte[] GetTransactionId() {
        return mPduHeaders->GetTextString(PduHeaders.TRANSACTION_ID);
    }

    /**
     * Set X-Mms-Transaction-Id.
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
     *     public Byte[] GetApplicId() {return NULL;}
     *     CARAPI SetApplicId(Byte[] value) {}
     *
     *     public Byte[] GetAuxApplicId() {return NULL;}
     *     CARAPI GetAuxApplicId(Byte[] value) {}
     *
     *     public Byte GetContentClass() {return 0x00;}
     *     CARAPI SetApplicId(Byte value) {}
     *
     *     public Byte GetDrmContent() {return 0x00;}
     *     CARAPI SetDrmContent(Byte value) {}
     *
     *     public Byte GetDistributionIndicator() {return 0x00;}
     *     CARAPI SetDistributionIndicator(Byte value) {}
     *
     *     public PreviouslySentByValue GetPreviouslySentBy() {return NULL;}
     *     CARAPI SetPreviouslySentBy(PreviouslySentByValue value) {}
     *
     *     public PreviouslySentDateValue GetPreviouslySentDate() {}
     *     CARAPI SetPreviouslySentDate(PreviouslySentDateValue value) {}
     *
     *     public MmFlagsValue GetMmFlags() {return NULL;}
     *     CARAPI SetMmFlags(MmFlagsValue value) {}
     *
     *     public MmStateValue GetMmState() {return NULL;}
     *     CARAPI GetMmState(MmStateValue value) {}
     *
     *     public Byte[] GetReplaceId() {return 0x00;}
     *     CARAPI SetReplaceId(Byte[] value) {}
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
     */
}
