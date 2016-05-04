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
 * M-Notification.ind PDU.
 */
public class NotificationInd extends GenericPdu {
    /**
     * Empty constructor.
     * Since the Pdu corresponding to this class is constructed
     * by the Proxy-Relay server, this class is only instantiated
     * by the Pdu Parser.
     *
     * @throws InvalidHeaderValueException if error occurs.
     *         RuntimeException if an undeclared error occurs.
     */
    public NotificationInd() throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND);
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    NotificationInd(PduHeaders headers) {
        Super(headers);
    }

    /**
     * Get X-Mms-Content-Class Value.
     *
     * @return the value
     */
    public Int32 GetContentClass() {
        return mPduHeaders->GetOctet(PduHeaders.CONTENT_CLASS);
    }

    /**
     * Set X-Mms-Content-Class Value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetContentClass(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.CONTENT_CLASS);
    }

    /**
     * Get X-Mms-Content-Location value.
     * When used in a PDU other than M-Mbox-Delete.conf and M-Delete.conf:
     * Content-location-value = Uri-value
     *
     * @return the value
     */
    public Byte[] GetContentLocation() {
        return mPduHeaders->GetTextString(PduHeaders.CONTENT_LOCATION);
    }

    /**
     * Set X-Mms-Content-Location value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetContentLocation(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.CONTENT_LOCATION);
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
     * @throws RuntimeException if an undeclared error occurs.
     */
    CARAPI SetExpiry(Int64 value) {
        mPduHeaders->SetLongInteger(value, PduHeaders.EXPIRY);
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
     *         RuntimeException if an undeclared error occurs.
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
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetMessageClass(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.MESSAGE_CLASS);
    }

    /**
     * Get X-Mms-Message-Size value.
     * Message-size-value = Long-integer
     *
     * @return the value
     */
    public Int64 GetMessageSize() {
        return mPduHeaders->GetLongInteger(PduHeaders.MESSAGE_SIZE);
    }

    /**
     * Set X-Mms-Message-Size value.
     *
     * @param value the value
     * @throws RuntimeException if an undeclared error occurs.
     */
    CARAPI SetMessageSize(Int64 value) {
        mPduHeaders->SetLongInteger(value, PduHeaders.MESSAGE_SIZE);
    }

    /**
     * Get subject.
     *
     * @return the value
     */
    public EncodedStringValue GetSubject() {
        return mPduHeaders->GetEncodedStringValue(PduHeaders.SUBJECT);
    }

    /**
     * Set subject.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetSubject(EncodedStringValue value) {
        mPduHeaders->SetEncodedStringValue(value, PduHeaders.SUBJECT);
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
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetTransactionId(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.TRANSACTION_ID);
    }

    /**
     * Get X-Mms-Delivery-Report Value.
     *
     * @return the value
     */
    public Int32 GetDeliveryReport() {
        return mPduHeaders->GetOctet(PduHeaders.DELIVERY_REPORT);
    }

    /**
     * Set X-Mms-Delivery-Report Value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetDeliveryReport(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.DELIVERY_REPORT);
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
     *     public Byte GetDrmContent() {return 0x00;}
     *     CARAPI SetDrmContent(Byte value) {}
     *
     *     public Byte GetDistributionIndicator() {return 0x00;}
     *     CARAPI SetDistributionIndicator(Byte value) {}
     *
     *     public ElementDescriptorValue GetElementDescriptor() {return NULL;}
     *     CARAPI GetElementDescriptor(ElementDescriptorValue value) {}
     *
     *     public Byte GetPriority() {return 0x00;}
     *     CARAPI SetPriority(Byte value) {}
     *
     *     public Byte GetRecommendedRetrievalMode() {return 0x00;}
     *     CARAPI SetRecommendedRetrievalMode(Byte value) {}
     *
     *     public Byte GetRecommendedRetrievalModeText() {return 0x00;}
     *     CARAPI SetRecommendedRetrievalModeText(Byte value) {}
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
     *
     *     public Byte GetStored() {return 0x00;}
     *     CARAPI SetStored(Byte value) {}
     */
}
