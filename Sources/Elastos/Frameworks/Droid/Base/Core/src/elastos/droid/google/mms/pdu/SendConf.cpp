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


public class SendConf extends GenericPdu {
    /**
     * Empty constructor.
     * Since the Pdu corresponding to this class is constructed
     * by the Proxy-Relay server, this class is only instantiated
     * by the Pdu Parser.
     *
     * @throws InvalidHeaderValueException if error occurs.
     */
    public SendConf() throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_SEND_CONF);
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    SendConf(PduHeaders headers) {
        Super(headers);
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
     * Get X-Mms-Response-Status.
     *
     * @return the value
     */
    public Int32 GetResponseStatus() {
        return mPduHeaders->GetOctet(PduHeaders.RESPONSE_STATUS);
    }

    /**
     * Set X-Mms-Response-Status.
     *
     * @param value the values
     * @throws InvalidHeaderValueException if the value is invalid.
     */
    CARAPI SetResponseStatus(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.RESPONSE_STATUS);
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
     *    public Byte[] GetContentLocation() {return NULL;}
     *    CARAPI SetContentLocation(Byte[] value) {}
     *
     *    public EncodedStringValue GetResponseText() {return NULL;}
     *    CARAPI SetResponseText(EncodedStringValue value) {}
     *
     *    public Byte GetStoreStatus() {return 0x00;}
     *    CARAPI SetStoreStatus(Byte value) {}
     *
     *    public Byte[] GetStoreStatusText() {return NULL;}
     *    CARAPI SetStoreStatusText(Byte[] value) {}
     */
}
