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


public class GenericPdu {
    /**
     * The headers of pdu.
     */
    PduHeaders mPduHeaders = NULL;

    /**
     * Constructor.
     */
    public GenericPdu() {
        mPduHeaders = new PduHeaders();
    }

    /**
     * Constructor.
     *
     * @param headers Headers for this PDU.
     */
    GenericPdu(PduHeaders headers) {
        mPduHeaders = headers;
    }

    /**
     * Get the headers of this PDU.
     *
     * @return A PduHeaders of this PDU.
     */
    PduHeaders GetPduHeaders() {
        return mPduHeaders;
    }

    /**
     * Get X-Mms-Message-Type field value.
     *
     * @return the X-Mms-Report-Allowed value
     */
    public Int32 GetMessageType() {
        return mPduHeaders->GetOctet(PduHeaders.MESSAGE_TYPE);
    }

    /**
     * Set X-Mms-Message-Type field value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     *         RuntimeException if field's value is not Octet.
     */
    CARAPI SetMessageType(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.MESSAGE_TYPE);
    }

    /**
     * Get X-Mms-MMS-Version field value.
     *
     * @return the X-Mms-MMS-Version value
     */
    public Int32 GetMmsVersion() {
        return mPduHeaders->GetOctet(PduHeaders.MMS_VERSION);
    }

    /**
     * Set X-Mms-MMS-Version field value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     *         RuntimeException if field's value is not Octet.
     */
    CARAPI SetMmsVersion(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.MMS_VERSION);
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
}
