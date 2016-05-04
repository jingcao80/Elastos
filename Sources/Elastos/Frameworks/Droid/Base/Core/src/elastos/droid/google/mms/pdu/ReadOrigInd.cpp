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


public class ReadOrigInd extends GenericPdu {
    /**
     * Empty constructor.
     * Since the Pdu corresponding to this class is constructed
     * by the Proxy-Relay server, this class is only instantiated
     * by the Pdu Parser.
     *
     * @throws InvalidHeaderValueException if error occurs.
     */
    public ReadOrigInd() throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_READ_ORIG_IND);
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    ReadOrigInd(PduHeaders headers) {
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
     * Get X-MMS-Read-status value.
     *
     * @return the value
     */
    public Int32 GetReadStatus() {
        return mPduHeaders->GetOctet(PduHeaders.READ_STATUS);
    }

    /**
     * Set X-MMS-Read-status value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     */
    CARAPI SetReadStatus(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.READ_STATUS);
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
     * Set To value.
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
     */
}
