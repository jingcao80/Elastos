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


public class ReadRecInd extends GenericPdu {
    /**
     * Constructor, used when composing a M-ReadRec.ind pdu.
     *
     * @param from the from value
     * @param messageId the message ID value
     * @param mmsVersion current viersion of mms
     * @param readStatus the read status value
     * @param to the to value
     * @throws InvalidHeaderValueException if parameters are invalid.
     *         NullPointerException if messageId or to is NULL.
     */
    public ReadRecInd(EncodedStringValue from,
                      Byte[] messageId,
                      Int32 mmsVersion,
                      Int32 readStatus,
                      EncodedStringValue[] to) throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_READ_REC_IND);
        SetFrom(from);
        SetMessageId(messageId);
        SetMmsVersion(mmsVersion);
        SetTo(to);
        SetReadStatus(readStatus);
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    ReadRecInd(PduHeaders headers) {
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
     * @param value the value
     * @throws NullPointerException if the value is NULL.
     */
    CARAPI SetMessageId(Byte[] value) {
        mPduHeaders->SetTextString(value, PduHeaders.MESSAGE_ID);
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
