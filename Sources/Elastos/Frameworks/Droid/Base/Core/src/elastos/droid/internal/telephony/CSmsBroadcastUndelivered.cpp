
#include "elastos/droid/internal/telephony/CSmsBroadcastUndelivered.h"

using Elastos::Core::EIID_IRunnable;

//using Elastos::Droid::Database::ICursor;

//using Elastos::Droid::Provider::ITelephony;
//using Elastos::Droid::Telephony::IRlog;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CSmsBroadcastUndelivered::SmsReferenceKey::SmsReferenceKey(
    /* [in] */ IInboundSmsTracker* tracker)
{
    assert(0 && "TODO");
//    mAddress = tracker->GetAddress();
//    mReferenceNumber = tracker->GetReferenceNumber();
//    mMessageCount = tracker->GetMessageCount();
}

AutoPtr<ArrayOf<String> > CSmsBroadcastUndelivered::SmsReferenceKey::GetDeleteWhereArgs()
{
    assert(0 && "TODO");
//return new String[]{mAddress, Integer->ToString(mReferenceNumber),
//                Integer->ToString(mMessageCount)};
    return NOERROR;
}

ECode CSmsBroadcastUndelivered::SmsReferenceKey::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    assert(0 && "TODO");
//return ((mReferenceNumber * 31) + mMessageCount) * 31 + mAddress->HashCode();
    return NOERROR;
}

ECode CSmsBroadcastUndelivered::SmsReferenceKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
//SmsReferenceKey other = (SmsReferenceKey) o;
//        return other.mAddress->Equals(mAddress)
//                && (other.mReferenceNumber == mReferenceNumber)
//                && (other.mMessageCount == mMessageCount);
//    }
//    return FALSE;
    return NOERROR;
}

/**
 * Called at boot time to clean out the raw table, collecting all acknowledged messages and
 * deleting any partial message segments older than 30 days. Called from a worker thread to
 * avoid delaying phone app startup. The last step is to broadcast the first pending message
 * from the main thread, then the remaining pending messages will be broadcast after the
 * previous ordered broadcast completes.
 */
CAR_OBJECT_IMPL(CSmsBroadcastUndelivered)

CAR_INTERFACE_IMPL(CSmsBroadcastUndelivered, Object, IRunnable)


const String TAG("SmsBroadcastUndelivered");
const Boolean DBG = TRUE;//TODO   IInboundSmsHandler::DBG;

const AutoPtr<ArrayOf<String> > CSmsBroadcastUndelivered::PDU_PENDING_MESSAGE_PROJECTION = InitPDU_PENDING_MESSAGE();

//const Uri sRawUri = Uri->WithAppendedPath(Telephony.Sms.CONTENT_URI, "raw");


AutoPtr<ArrayOf<String> > CSmsBroadcastUndelivered::InitPDU_PENDING_MESSAGE()
{
    AutoPtr<ArrayOf<String> > sArray = ArrayOf<String>::Alloc(8);

    (*sArray)[0] = String("pdu");
    (*sArray)[1] = String("sequence");
    (*sArray)[2] = String("destination_port");
    (*sArray)[3] = String("date");
    (*sArray)[4] = String("reference_number");
    (*sArray)[5] = String("count");
    (*sArray)[6] = String("address");
    (*sArray)[7] = String("_id");
    return sArray;
}

ECode CSmsBroadcastUndelivered::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IGsmInboundSmsHandler* gsmInboundSmsHandler,
    /* [in] */ ICdmaInboundSmsHandler* cdmaInboundSmsHandler)
{
    mContext = context;
    context->GetContentResolver((IContentResolver**)&mResolver);
    mGsmInboundSmsHandler = gsmInboundSmsHandler;
    mCdmaInboundSmsHandler = cdmaInboundSmsHandler;
    return NOERROR;
}


//@Override
ECode CSmsBroadcastUndelivered::Run()
{
    assert(0 && "TODO");
//    If (DBG) Rlog->D(TAG, "scanning raw table for undelivered messages");
//    ScanRawTable();
//    // tell handlers to start processing new messages
//    If (mGsmInboundSmsHandler != NULL) {
//        mGsmInboundSmsHandler->SendMessage(InboundSmsHandler.EVENT_START_ACCEPTING_SMS);
//    }
//    If (mCdmaInboundSmsHandler != NULL) {
//        mCdmaInboundSmsHandler->SendMessage(InboundSmsHandler.EVENT_START_ACCEPTING_SMS);
//    }
}

void CSmsBroadcastUndelivered::ScanRawTable()
{
    assert(0 && "TODO");
//    Int64 startTime = System->NanoTime();
//    HashMap<SmsReferenceKey, Integer> multiPartReceivedCount =
//            new HashMap<SmsReferenceKey, Integer>(4);
//    HashSet<SmsReferenceKey> oldMultiPartMessages = new HashSet<SmsReferenceKey>(4);
//    Cursor cursor = NULL;
//    try {
//        cursor = mResolver->Query(sRawUri, PDU_PENDING_MESSAGE_PROJECTION, NULL, NULL, NULL);
//        If (cursor == NULL) {
//            Rlog->E(TAG, "error getting pending message cursor");
//            return;
//        }
//
//        Boolean isCurrentFormat3gpp2 = InboundSmsHandler->IsCurrentFormat3gpp2();
//        While (cursor->MoveToNext()) {
//            InboundSmsTracker tracker;
//            try {
//                tracker = new InboundSmsTracker(cursor, isCurrentFormat3gpp2);
//            } Catch (IllegalArgumentException e) {
//                Rlog->E(TAG, "error loading SmsTracker: " + e);
//                continue;
//            }
//
//            If (tracker->GetMessageCount() == 1) {
//                // deliver single-part message
//                BroadcastSms(tracker);
//            } else {
//                SmsReferenceKey reference = new SmsReferenceKey(tracker);
//                Integer receivedCount = multiPartReceivedCount->Get(reference);
//                // get partial segment expire age from resource which in config.xml
//                // Add this function for international roaming requirement.
//                String expireAgeString = mContext->GetResources()->GetString(
//                        R::string::config_partial_segment_expire_age);
//                Int64 expireAge = Long->ValueOf(expireAgeString);
//                If (receivedCount == NULL) {
//                    multiPartReceivedCount->Put(reference, 1);    // first segment seen
//                    If (tracker->GetTimestamp() <
//                            (System->CurrentTimeMillis() - expireAge)) {
//                        // older than 30 days; delete if we don't find all the segments
//                        oldMultiPartMessages->Add(reference);
//                    }
//                } else {
//                    Int32 newCount = receivedCount + 1;
//                    If (newCount == tracker->GetMessageCount()) {
//                        // looks like we've got all the pieces; send a single tracker
//                        // to state machine which will find the other pieces to broadcast
//                        If (DBG) Rlog->D(TAG, "found complete multi-part message");
//                        BroadcastSms(tracker);
//                        // don't delete this old message until after we broadcast it
//                        oldMultiPartMessages->Remove(reference);
//                    } else {
//                        multiPartReceivedCount->Put(reference, newCount);
//                    }
//                }
//            }
//        }
//        // Delete old incomplete message segments
//        For (SmsReferenceKey message : oldMultiPartMessages) {
//            Int32 rows = mResolver->Delete(sRawUri, InboundSmsHandler.SELECT_BY_REFERENCE,
//                    message->GetDeleteWhereArgs());
//            If (rows == 0) {
//                Rlog->E(TAG, "No rows were deleted from raw table!");
//            } else If (DBG) {
//                Rlog->D(TAG, "Deleted " + rows + " rows from raw table for incomplete "
//                        + message.mMessageCount + " part message");
//            }
//        }
//    } Catch (SQLException e) {
//        Rlog->E(TAG, "error reading pending SMS messages", e);
//    } finally {
//        If (cursor != NULL) {
//            cursor->Close();
//        }
//        If (DBG) Rlog->D(TAG, "finished scanning raw table in "
//                + ((System->NanoTime() - startTime) / 1000000) + " ms");
//    }
}

void CSmsBroadcastUndelivered::BroadcastSms(
    /* [in] */ IInboundSmsTracker* tracker)
{
    assert(0 && "TODO");
//    InboundSmsHandler handler;
//    If (tracker->Is3gpp2()) {
//        handler = mCdmaInboundSmsHandler;
//    } else {
//        handler = mGsmInboundSmsHandler;
//    }
//    If (handler != NULL) {
//        handler->SendMessage(InboundSmsHandler.EVENT_BROADCAST_SMS, tracker);
//    } else {
//        Rlog->E(TAG, "NULL handler for " + tracker->GetFormat() + " format, can't deliver.");
//    }
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
