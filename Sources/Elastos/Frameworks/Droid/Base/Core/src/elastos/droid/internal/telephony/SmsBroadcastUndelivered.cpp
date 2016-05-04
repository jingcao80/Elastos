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

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::Cdma::ICdmaInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::Gsm::IGsmInboundSmsHandler;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;

/**
 * Called at boot time to clean out the raw table, collecting all acknowledged messages and
 * deleting any partial message segments older than 30 days. Called from a worker thread to
 * avoid delaying phone app startup. The last step is to broadcast the first pending message
 * from the main thread, then the remaining pending messages will be broadcast after the
 * previous ordered broadcast completes.
 */
public class SmsBroadcastUndelivered implements Runnable {
    private static const String TAG = "SmsBroadcastUndelivered";
    private static const Boolean DBG = InboundSmsHandler.DBG;

    /**
     * Query projection for dispatching pending messages at boot time.
     * Column order must match the {@code *_COLUMN} constants in {@link InboundSmsHandler}.
     */
    private static const String[] PDU_PENDING_MESSAGE_PROJECTION = {
            "pdu",
            "sequence",
            "destination_port",
            "date",
            "reference_number",
            "count",
            "address",
            "_id"
    };

    /** URI for raw table from SmsProvider. */
    private static const Uri sRawUri = Uri->WithAppendedPath(Telephony.Sms.CONTENT_URI, "raw");

    /** Content resolver to use to access raw table from SmsProvider. */
    private final ContentResolver mResolver;

    /** Handler for 3GPP-format Messages (may be NULL). */
    private final GsmInboundSmsHandler mGsmInboundSmsHandler;

    /** Handler for 3GPP2-format Messages (may be NULL). */
    private final CdmaInboundSmsHandler mCdmaInboundSmsHandler;

    /** Use context get bool config resource from framework. */
    private final Context mContext;

    public SmsBroadcastUndelivered(Context context, GsmInboundSmsHandler gsmInboundSmsHandler,
            CdmaInboundSmsHandler cdmaInboundSmsHandler) {
        mContext = context;
        mResolver = context->GetContentResolver();
        mGsmInboundSmsHandler = gsmInboundSmsHandler;
        mCdmaInboundSmsHandler = cdmaInboundSmsHandler;
    }

    //@Override
    CARAPI Run() {
        If (DBG) Rlog->D(TAG, "scanning raw table for undelivered messages");
        ScanRawTable();
        // tell handlers to start processing new messages
        If (mGsmInboundSmsHandler != NULL) {
            mGsmInboundSmsHandler->SendMessage(InboundSmsHandler.EVENT_START_ACCEPTING_SMS);
        }
        If (mCdmaInboundSmsHandler != NULL) {
            mCdmaInboundSmsHandler->SendMessage(InboundSmsHandler.EVENT_START_ACCEPTING_SMS);
        }
    }

    /**
     * Scan the raw table for complete SMS messages to broadcast, and old PDUs to delete.
     */
    private void ScanRawTable() {
        Int64 startTime = System->NanoTime();
        HashMap<SmsReferenceKey, Integer> multiPartReceivedCount =
                new HashMap<SmsReferenceKey, Integer>(4);
        HashSet<SmsReferenceKey> oldMultiPartMessages = new HashSet<SmsReferenceKey>(4);
        Cursor cursor = NULL;
        try {
            cursor = mResolver->Query(sRawUri, PDU_PENDING_MESSAGE_PROJECTION, NULL, NULL, NULL);
            If (cursor == NULL) {
                Rlog->E(TAG, "error getting pending message cursor");
                return;
            }

            Boolean isCurrentFormat3gpp2 = InboundSmsHandler->IsCurrentFormat3gpp2();
            While (cursor->MoveToNext()) {
                InboundSmsTracker tracker;
                try {
                    tracker = new InboundSmsTracker(cursor, isCurrentFormat3gpp2);
                } Catch (IllegalArgumentException e) {
                    Rlog->E(TAG, "error loading SmsTracker: " + e);
                    continue;
                }

                If (tracker->GetMessageCount() == 1) {
                    // deliver single-part message
                    BroadcastSms(tracker);
                } else {
                    SmsReferenceKey reference = new SmsReferenceKey(tracker);
                    Integer receivedCount = multiPartReceivedCount->Get(reference);
                    // get partial segment expire age from resource which in config.xml
                    // Add this function for international roaming requirement.
                    String expireAgeString = mContext->GetResources()->GetString(
                            R::string::config_partial_segment_expire_age);
                    Int64 expireAge = Long->ValueOf(expireAgeString);
                    If (receivedCount == NULL) {
                        multiPartReceivedCount->Put(reference, 1);    // first segment seen
                        If (tracker->GetTimestamp() <
                                (System->CurrentTimeMillis() - expireAge)) {
                            // older than 30 days; delete if we don't find all the segments
                            oldMultiPartMessages->Add(reference);
                        }
                    } else {
                        Int32 newCount = receivedCount + 1;
                        If (newCount == tracker->GetMessageCount()) {
                            // looks like we've got all the pieces; send a single tracker
                            // to state machine which will find the other pieces to broadcast
                            If (DBG) Rlog->D(TAG, "found complete multi-part message");
                            BroadcastSms(tracker);
                            // don't delete this old message until after we broadcast it
                            oldMultiPartMessages->Remove(reference);
                        } else {
                            multiPartReceivedCount->Put(reference, newCount);
                        }
                    }
                }
            }
            // Delete old incomplete message segments
            For (SmsReferenceKey message : oldMultiPartMessages) {
                Int32 rows = mResolver->Delete(sRawUri, InboundSmsHandler.SELECT_BY_REFERENCE,
                        message->GetDeleteWhereArgs());
                If (rows == 0) {
                    Rlog->E(TAG, "No rows were deleted from raw table!");
                } else If (DBG) {
                    Rlog->D(TAG, "Deleted " + rows + " rows from raw table for incomplete "
                            + message.mMessageCount + " part message");
                }
            }
        } Catch (SQLException e) {
            Rlog->E(TAG, "error reading pending SMS messages", e);
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
            If (DBG) Rlog->D(TAG, "finished scanning raw table in "
                    + ((System->NanoTime() - startTime) / 1000000) + " ms");
        }
    }

    /**
     * Send tracker to Appropriate (3GPP or 3GPP2) inbound SMS handler for broadcast.
     */
    private void BroadcastSms(InboundSmsTracker tracker) {
        InboundSmsHandler handler;
        If (tracker->Is3gpp2()) {
            handler = mCdmaInboundSmsHandler;
        } else {
            handler = mGsmInboundSmsHandler;
        }
        If (handler != NULL) {
            handler->SendMessage(InboundSmsHandler.EVENT_BROADCAST_SMS, tracker);
        } else {
            Rlog->E(TAG, "NULL handler for " + tracker->GetFormat() + " format, can't deliver.");
        }
    }

    /**
     * Used as the HashMap key for matching concatenated message segments.
     */
    private static class SmsReferenceKey {
        final String mAddress;
        final Int32 mReferenceNumber;
        final Int32 mMessageCount;

        SmsReferenceKey(InboundSmsTracker tracker) {
            mAddress = tracker->GetAddress();
            mReferenceNumber = tracker->GetReferenceNumber();
            mMessageCount = tracker->GetMessageCount();
        }

        String[] GetDeleteWhereArgs() {
            return new String[]{mAddress, Integer->ToString(mReferenceNumber),
                    Integer->ToString(mMessageCount)};
        }

        //@Override
        public Int32 HashCode() {
            Return ((mReferenceNumber * 31) + mMessageCount) * 31 + mAddress->HashCode();
        }

        //@Override
        public Boolean Equals(Object o) {
            If (o instanceof SmsReferenceKey) {
                SmsReferenceKey other = (SmsReferenceKey) o;
                return other.mAddress->Equals(mAddress)
                        && (other.mReferenceNumber == mReferenceNumber)
                        && (other.mMessageCount == mMessageCount);
            }
            return FALSE;
        }
    }
}
