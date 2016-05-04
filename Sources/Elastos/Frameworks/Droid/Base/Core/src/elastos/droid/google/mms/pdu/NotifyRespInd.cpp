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
 * M-NofifyResp.ind PDU.
 */
public class NotifyRespInd extends GenericPdu {
    /**
     * Constructor, used when composing a M-NotifyResp.ind pdu.
     *
     * @param mmsVersion current version of mms
     * @param transactionId the transaction-id value
     * @param status the status value
     * @throws InvalidHeaderValueException if parameters are invalid.
     *         NullPointerException if transactionId is NULL.
     *         RuntimeException if an undeclared error occurs.
     */
    public NotifyRespInd(Int32 mmsVersion,
                         Byte[] transactionId,
                         Int32 status) throws InvalidHeaderValueException {
        Super();
        SetMessageType(PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND);
        SetMmsVersion(mmsVersion);
        SetTransactionId(transactionId);
        SetStatus(status);
    }

    /**
     * Constructor with given headers.
     *
     * @param headers Headers for this PDU.
     */
    NotifyRespInd(PduHeaders headers) {
        Super(headers);
    }

    /**
     * Get X-Mms-Report-Allowed field value.
     *
     * @return the X-Mms-Report-Allowed value
     */
    public Int32 GetReportAllowed() {
        return mPduHeaders->GetOctet(PduHeaders.REPORT_ALLOWED);
    }

    /**
     * Set X-Mms-Report-Allowed field value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetReportAllowed(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.REPORT_ALLOWED);
    }

    /**
     * Set X-Mms-Status field value.
     *
     * @param value the value
     * @throws InvalidHeaderValueException if the value is invalid.
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetStatus(Int32 value) throws InvalidHeaderValueException {
        mPduHeaders->SetOctet(value, PduHeaders.STATUS);
    }

    /**
     * GetX-Mms-Status field value.
     *
     * @return the X-Mms-Status value
     */
    public Int32 GetStatus() {
        return mPduHeaders->GetOctet(PduHeaders.STATUS);
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
     *         RuntimeException if an undeclared error occurs.
     */
    CARAPI SetTransactionId(Byte[] value) {
            mPduHeaders->SetTextString(value, PduHeaders.TRANSACTION_ID);
    }
}
