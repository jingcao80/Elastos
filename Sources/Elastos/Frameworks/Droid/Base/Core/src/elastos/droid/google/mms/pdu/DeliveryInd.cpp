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
 * M-Delivery.Ind Pdu.
 */
public class DeliveryInd extends GenericPdu {
    /**
     * Empty constructor.
     * Since the Pdu corresponding to this class is constructed
     * by the Proxy-Relay server, this class is only instantiated
     * by the Pdu Parser.
     *
     * @throws InvalidHeaderValueException if error occurs.
     */
    public DeliveryInd() throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_DELIVERY_IND);
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    DeliveryInd(PduHeaders headers) {
        Super(headers);
    }

    /**
     * Get Date value.
     *
     * @return the value
     */
    public Int64 GetDate() {
        return mPduHeaders->GetLongInteger(PduHeaders.DATE);
    }

    /**
     * Set Date value.
     *
     * @param value the value
     */
    CARAPI SetDate(Int64 value) {
        mPduHeaders->SetLongInteger(value, PduHeaders.DATE);
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
     * @param value the value, should not be NULL
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetMessageId(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.MESSAGE_ID);
    }

    /**
     * Get Status value.
     *
     * @return the value
     */
    public Int32 GetStatus() {
        return mPduHeaders->GetOctet(PduHeaders.STATUS);
    }

    /**
     * Set Status value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     */
    CARAPI SetStatus(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.STATUS);
    }

    /**
     * Get To value.
     *
     * @return the value
     */
    public EncodedStringValue[] GetTo() {
        return mPduHeaders->GetEncodedStringValues(PduHeaders.TO);
    }

    /**
     * set To value.
     *
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetTo(EncodedStringValue[] value) {
        mPduHeaders->SetEncodedStringValues(value, PduHeaders.TO);
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
     *     public Byte[] GetReplyApplicId() {return 0x00;}
     *     CARAPI SetReplyApplicId(Byte[] value) {}
     *
     *     public EncodedStringValue GetStatusText() {return NULL;}
     *     CARAPI SetStatusText(EncodedStringValue value) {}
     */
}
