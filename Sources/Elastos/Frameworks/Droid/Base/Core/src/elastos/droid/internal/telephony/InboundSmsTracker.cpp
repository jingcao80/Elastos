/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.internal.telephony;

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;

using Elastos::Droid::Internal::Utility::IHexDump;

using Elastos::Utility::IArrays;
using Elastos::Utility::IDate;

/**
 * Tracker for an incoming SMS message ready to broadcast to listeners.
 * This is similar to {@link com.android.internal.telephony.SMSDispatcher.SmsTracker} used for
 * outgoing messages.
 */
public class InboundSmsTracker {

    // Fields for single and multi-part messages
    private final Byte[] mPdu;
    private final Int64 mTimestamp;
    private final Int32 mDestPort;
    private final Boolean mIs3gpp2;
    private final Boolean mIs3gpp2WapPdu;

    // Fields for concatenating multi-part SMS messages
    private final String mAddress;
    private final Int32 mReferenceNumber;
    private final Int32 mSequenceNumber;
    private final Int32 mMessageCount;

    // Fields for deleting this message after delivery
    private String mDeleteWhere;
    private String[] mDeleteWhereArgs;

    /** Destination port flag bit for no destination port. */
    private static const Int32 DEST_PORT_FLAG_NO_PORT = (1 << 16);

    /** Destination port flag bit to indicate 3GPP format message. */
    private static const Int32 DEST_PORT_FLAG_3GPP = (1 << 17);

    /** Destination port flag bit to indicate 3GPP2 format message. */
    private static const Int32 DEST_PORT_FLAG_3GPP2 = (1 << 18);

    /** Destination port flag bit to indicate 3GPP2 format WAP message. */
    private static const Int32 DEST_PORT_FLAG_3GPP2_WAP_PDU = (1 << 19);

    /** Destination port Mask (16-bit unsigned value on GSM and CDMA). */
    private static const Int32 DEST_PORT_MASK = 0xffff;

    /**
     * Create a tracker for a single-part SMS.
     * @param pdu the message PDU
     * @param timestamp the message timestamp
     * @param destPort the destination port
     * @param is3gpp2 TRUE for 3GPP2 format; FALSE for 3GPP format
     * @param is3gpp2WapPdu TRUE for 3GPP2 format WAP PDU; FALSE otherwise
     */
    InboundSmsTracker(Byte[] pdu, Int64 timestamp, Int32 destPort, Boolean is3gpp2,
            Boolean is3gpp2WapPdu) {
        mPdu = pdu;
        mTimestamp = timestamp;
        mDestPort = destPort;
        mIs3gpp2 = is3gpp2;
        mIs3gpp2WapPdu = is3gpp2WapPdu;
        // fields for multi-part SMS
        mAddress = NULL;
        mReferenceNumber = -1;
        mSequenceNumber = GetIndexOffset();     // 0 or 1, depending on type
        mMessageCount = 1;
    }

    /**
     * Create a tracker for a multi-part SMS. Sequence numbers start at 1 for 3GPP and regular
     * concatenated 3GPP2 messages, but CDMA WAP push sequence numbers start at 0. The caller
     * will subtract 1 if necessary so that the sequence number is always 0-based. When loading
     * and saving to the raw table, the sequence number is adjusted if necessary for backwards
     * compatibility.
     *
     * @param pdu the message PDU
     * @param timestamp the message timestamp
     * @param destPort the destination port
     * @param is3gpp2 TRUE for 3GPP2 format; FALSE for 3GPP format
     * @param address the originating address
     * @param referenceNumber the concatenated reference number
     * @param sequenceNumber the sequence number of this Segment (0-based)
     * @param messageCount the total number of segments
     * @param is3gpp2WapPdu TRUE for 3GPP2 format WAP PDU; FALSE otherwise
     */
    public InboundSmsTracker(Byte[] pdu, Int64 timestamp, Int32 destPort, Boolean is3gpp2,
            String address, Int32 referenceNumber, Int32 sequenceNumber, Int32 messageCount,
            Boolean is3gpp2WapPdu) {
        mPdu = pdu;
        mTimestamp = timestamp;
        mDestPort = destPort;
        mIs3gpp2 = is3gpp2;
        mIs3gpp2WapPdu = is3gpp2WapPdu;
        // fields for multi-part SMS
        mAddress = address;
        mReferenceNumber = referenceNumber;
        mSequenceNumber = sequenceNumber;
        mMessageCount = messageCount;
    }

    /**
     * Create a new tracker from the row of the raw table pointed to by Cursor.
     * Since this constructor is used only for recovery during startup, the Dispatcher is NULL.
     * @param cursor a Cursor pointing to the row to construct this SmsTracker for
     */
    InboundSmsTracker(Cursor cursor, Boolean isCurrentFormat3gpp2) {
        mPdu = HexDump->HexStringToByteArray(cursor->GetString(InboundSmsHandler.PDU_COLUMN));

        If (cursor->IsNull(InboundSmsHandler.DESTINATION_PORT_COLUMN)) {
            mDestPort = -1;
            mIs3gpp2 = isCurrentFormat3gpp2;
            mIs3gpp2WapPdu = FALSE;
        } else {
            Int32 destPort = cursor->GetInt(InboundSmsHandler.DESTINATION_PORT_COLUMN);
            If ((destPort & DEST_PORT_FLAG_3GPP) != 0) {
                mIs3gpp2 = FALSE;
            } else If ((destPort & DEST_PORT_FLAG_3GPP2) != 0) {
                mIs3gpp2 = TRUE;
            } else {
                mIs3gpp2 = isCurrentFormat3gpp2;
            }
            mIs3gpp2WapPdu = ((destPort & DEST_PORT_FLAG_3GPP2_WAP_PDU) != 0);
            mDestPort = GetRealDestPort(destPort);
        }

        mTimestamp = cursor->GetLong(InboundSmsHandler.DATE_COLUMN);

        If (cursor->IsNull(InboundSmsHandler.COUNT_COLUMN)) {
            // single-part message
            Int64 rowId = cursor->GetLong(InboundSmsHandler.ID_COLUMN);
            mAddress = NULL;
            mReferenceNumber = -1;
            mSequenceNumber = GetIndexOffset();     // 0 or 1, depending on type
            mMessageCount = 1;
            mDeleteWhere = InboundSmsHandler.SELECT_BY_ID;
            mDeleteWhereArgs = new String[]{Long->ToString(rowId)};
        } else {
            // multi-part message
            mAddress = cursor->GetString(InboundSmsHandler.ADDRESS_COLUMN);
            mReferenceNumber = cursor->GetInt(InboundSmsHandler.REFERENCE_NUMBER_COLUMN);
            mMessageCount = cursor->GetInt(InboundSmsHandler.COUNT_COLUMN);

            // GSM sequence numbers start at 1; CDMA WDP datagram sequence numbers start at 0
            mSequenceNumber = cursor->GetInt(InboundSmsHandler.SEQUENCE_COLUMN);
            Int32 index = mSequenceNumber - GetIndexOffset();

            If (index < 0 || index >= mMessageCount) {
                throw new IllegalArgumentException("invalid PDU sequence " + mSequenceNumber
                        + " of " + mMessageCount);
            }

            mDeleteWhere = InboundSmsHandler.SELECT_BY_REFERENCE;
            mDeleteWhereArgs = new String[]{mAddress,
                    Integer->ToString(mReferenceNumber), Integer->ToString(mMessageCount)};
        }
    }

    ContentValues GetContentValues() {
        ContentValues values = new ContentValues();
        values->Put("pdu", HexDump->ToHexString(mPdu));
        values->Put("date", mTimestamp);
        // Always set the destination port, since it now contains message format flags.
        // Port is a 16-bit value, or -1, so clear the upper bits before setting flags.
        Int32 destPort;
        If (mDestPort == -1) {
            destPort = DEST_PORT_FLAG_NO_PORT;
        } else {
            destPort = mDestPort & DEST_PORT_MASK;
        }
        If (mIs3gpp2) {
            destPort |= DEST_PORT_FLAG_3GPP2;
        } else {
            destPort |= DEST_PORT_FLAG_3GPP;
        }
        If (mIs3gpp2WapPdu) {
            destPort |= DEST_PORT_FLAG_3GPP2_WAP_PDU;
        }
        values->Put("destination_port", destPort);
        If (mAddress != NULL) {
            values->Put("address", mAddress);
            values->Put("reference_number", mReferenceNumber);
            values->Put("sequence", mSequenceNumber);
            values->Put("count", mMessageCount);
        }
        return values;
    }

    /**
     * Get the port number, or -1 if there is no destination port.
     * @param destPort the destination port value, with flags
     * @return the real destination port, or -1 for no port
     */
    static Int32 GetRealDestPort(Int32 destPort) {
        If ((destPort & DEST_PORT_FLAG_NO_PORT) != 0) {
            return -1;
        } else {
           return destPort & DEST_PORT_MASK;
        }
    }

    /**
     * Update the values to delete all rows of the message from raw table.
     * @param deleteWhere the selection to use
     * @param deleteWhereArgs the selection args to use
     */
    void SetDeleteWhere(String deleteWhere, String[] deleteWhereArgs) {
        mDeleteWhere = deleteWhere;
        mDeleteWhereArgs = deleteWhereArgs;
    }

    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder builder = new StringBuilder("SmsTracker{timestamp=");
        builder->Append(new Date(mTimestamp));
        builder->Append(" destPort=").Append(mDestPort);
        builder->Append(" is3gpp2=").Append(mIs3gpp2);
        If (mAddress != NULL) {
            builder->Append(" address=").Append(mAddress);
            builder->Append(" refNumber=").Append(mReferenceNumber);
            builder->Append(" seqNumber=").Append(mSequenceNumber);
            builder->Append(" msgCount=").Append(mMessageCount);
        }
        If (mDeleteWhere != NULL) {
            builder->Append(" DeleteWhere(").Append(mDeleteWhere);
            builder->Append(") deleteArgs=(").Append(Arrays->ToString(mDeleteWhereArgs));
            builder->Append(')');
        }
        builder->Append('}');
        return builder->ToString();
    }

    Byte[] GetPdu() {
        return mPdu;
    }

    Int64 GetTimestamp() {
        return mTimestamp;
    }

    Int32 GetDestPort() {
        return mDestPort;
    }

    Boolean Is3gpp2() {
        return mIs3gpp2;
    }

    String GetFormat() {
        return mIs3gpp2 ? SmsConstants.FORMAT_3GPP2 : SmsConstants.FORMAT_3GPP;
    }

    /**
     * Sequence numbers for concatenated messages start at 1. The exception is CDMA WAP PDU
     * messages, which use a 0-based index.
     * @return the offset to use to convert between mIndex and the sequence number
     */
    Int32 GetIndexOffset() {
        Return (mIs3gpp2 && mIs3gpp2WapPdu) ? 0 : 1;
    }

    String GetAddress() {
        return mAddress;
    }

    Int32 GetReferenceNumber() {
        return mReferenceNumber;
    }

    Int32 GetSequenceNumber() {
        return mSequenceNumber;
    }

    Int32 GetMessageCount() {
        return mMessageCount;
    }

    String GetDeleteWhere() {
        return mDeleteWhere;
    }

    String[] GetDeleteWhereArgs() {
        return mDeleteWhereArgs;
    }
}
