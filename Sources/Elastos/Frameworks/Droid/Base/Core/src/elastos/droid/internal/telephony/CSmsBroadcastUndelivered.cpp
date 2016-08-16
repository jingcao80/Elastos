#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/CSmsBroadcastUndelivered.h"
#include "elastos/droid/internal/telephony/InboundSmsHandler.h"
#include "elastos/droid/internal/telephony/InboundSmsTracker.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/R.h"
#include "elastos/core/StringUtils.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
//using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::R;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::ICloseable;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CSmsBroadcastUndelivered::SmsReferenceKey::SmsReferenceKey(
    /* [in] */ IInboundSmsTracker* _tracker)
{
    InboundSmsTracker* tracker = (InboundSmsTracker*)(IObject::Probe(_tracker));
    mAddress = tracker->GetAddress();
    mReferenceNumber = tracker->GetReferenceNumber();
    mMessageCount = tracker->GetMessageCount();
}

AutoPtr<ArrayOf<String> > CSmsBroadcastUndelivered::SmsReferenceKey::GetDeleteWhereArgs()
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(3);
    result->Set(0, mAddress);
    result->Set(1, StringUtils::ToString(mReferenceNumber));
    result->Set(2, StringUtils::ToString(mMessageCount));
    return result;
}

ECode CSmsBroadcastUndelivered::SmsReferenceKey::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return ((mReferenceNumber * 31) + mMessageCount) * 31 + mAddress.GetHashCode();
}

ECode CSmsBroadcastUndelivered::SmsReferenceKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    SmsReferenceKey* other = (SmsReferenceKey*)(IObject::Probe(obj));
    if (other != NULL)
    {
       if( other->mAddress.Equals(mAddress)
               && (other->mReferenceNumber == mReferenceNumber)
               && (other->mMessageCount == mMessageCount))
       {
           *result = TRUE;
       }
    }
    return NOERROR;
}

/**
 * Called at boot time to clean out the raw table, collecting all acknowledged messages and
 * deleting any partial message segments older than 30 days. Called from a worker thread to
 * avoid delaying phone app startup. The last step is to broadcast the first pending message
 * from the main thread, then the remaining pending messages will be broadcast after the
 * previous ordered broadcast completes.
 */

CAR_INTERFACE_IMPL(CSmsBroadcastUndelivered, Object, IRunnable)

CAR_OBJECT_IMPL(CSmsBroadcastUndelivered)

const String CSmsBroadcastUndelivered::TAG("SmsBroadcastUndelivered");
const Boolean CSmsBroadcastUndelivered::DBG = TRUE;//TODO   IInboundSmsHandler::DBG;

const AutoPtr<ArrayOf<String> > CSmsBroadcastUndelivered::PDU_PENDING_MESSAGE_PROJECTION = InitPDU_PENDING_MESSAGE();

static AutoPtr<IUri> Init_Uri_WithAppendedPath(
    /* [in] */ IUri* path,
    /* [in] */ const String& str)
{
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->WithAppendedPath(path, str, (IUri**)&uri);
    return uri;
}
const AutoPtr<IUri> CSmsBroadcastUndelivered::sRawUri = Init_Uri_WithAppendedPath(Elastos::Droid::Provider::Telephony::Sms::CONTENT_URI, String("raw"));


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
    if (DBG) Logger::D(TAG, "scanning raw table for undelivered messages");
    ScanRawTable();
    // tell handlers to start processing new messages
    if (mGsmInboundSmsHandler != NULL) {
        IStateMachine::Probe(mGsmInboundSmsHandler)->SendMessage(InboundSmsHandler::EVENT_START_ACCEPTING_SMS);
    }
    if (mCdmaInboundSmsHandler != NULL) {
        IStateMachine::Probe(mCdmaInboundSmsHandler)->SendMessage(InboundSmsHandler::EVENT_START_ACCEPTING_SMS);
    }
    return NOERROR;
}

void CSmsBroadcastUndelivered::ScanRawTable()
{
    Int64 startTime;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetNanoTime(&startTime);

    //HashMap<SmsReferenceKey, Integer> multiPartReceivedCount = new HashMap<SmsReferenceKey, Integer>(4);
    AutoPtr<IHashMap> multiPartReceivedCount;
    CHashMap::New(4, (IHashMap**)&multiPartReceivedCount);

    //HashSet<SmsReferenceKey> oldMultiPartMessages = new HashSet<SmsReferenceKey>(4);
    AutoPtr<IHashSet> oldMultiPartMessages;
    CHashSet::New(4, (IHashSet**)&oldMultiPartMessages);

    AutoPtr<ICursor> cursor = NULL;
    //try {
        mResolver->Query(sRawUri, PDU_PENDING_MESSAGE_PROJECTION, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
        if (cursor == NULL) {
            //Rlog->E(TAG, "error getting pending message cursor");
            Logger::E(TAG, "error getting pending message cursor");
            return;
        }

        Boolean isCurrentFormat3gpp2 = InboundSmsHandler::IsCurrentFormat3gpp2();
        Boolean next = FALSE;
        while (cursor->MoveToNext(&next), next) {
            AutoPtr<InboundSmsTracker> tracker;
            //try {
                tracker = new InboundSmsTracker(cursor, isCurrentFormat3gpp2);
            //} Catch (IllegalArgumentException e) {
            //    Rlog->E(TAG, "error loading SmsTracker: " + e);
            //    continue;
            //}

            if (tracker->GetMessageCount() == 1) {
                // deliver single-part message
                BroadcastSms(tracker);
            } else {
                AutoPtr<SmsReferenceKey> reference = new SmsReferenceKey(tracker);
                AutoPtr<IInterface> obj;
                multiPartReceivedCount->Get(TO_IINTERFACE(reference), (IInterface**)&obj);
                IInteger32* receivedCount = IInteger32::Probe(obj);

                // get partial segment expire age from resource which in config.xml
                // Add this function for international roaming requirement.
                AutoPtr<IResources> res;
                mContext->GetResources((IResources**)&res);
                String expireAgeString;
                res->GetString(R::string::config_partial_segment_expire_age, &expireAgeString);
                Int64 expireAge = StringUtils::ParseInt64(expireAgeString);
                if (obj == NULL) {
                    // first segment seen
                    multiPartReceivedCount->Put(TO_IINTERFACE(reference), CoreUtils::Convert(1));
                    Int64 currentTimeMillis;
                    system->GetCurrentTimeMillis(&currentTimeMillis);
                    if (tracker->GetTimestamp() < (currentTimeMillis - expireAge)) {
                        // older than 30 days; delete if we don't find all the segments
                        oldMultiPartMessages->Add(TO_IINTERFACE(reference));
                    }
                } else {
                    //Int32 newCount = receivedCount + 1;
                    Int32 newCount;
                    receivedCount->GetValue(&newCount);
                    ++newCount;
                    if (newCount == tracker->GetMessageCount()) {
                        // looks like we've got all the pieces; send a single tracker
                        // to state machine which will find the other pieces to broadcast
                        if (DBG) {
                            //Rlog->D(TAG, "found complete multi-part message");
                            Logger::D(TAG, "found complete multi-part message");
                        }
                        BroadcastSms(tracker);
                        // don't delete this old message until after we broadcast it
                        oldMultiPartMessages->Remove(TO_IINTERFACE(reference));
                    } else {
                        multiPartReceivedCount->Put(TO_IINTERFACE(reference), CoreUtils::Convert(newCount));
                    }
                }
            }
        }
        // Delete old incomplete message segments
        AutoPtr<ArrayOf<IInterface*> > array;
        oldMultiPartMessages->ToArray((ArrayOf<IInterface*>**)&array);
        Int32 size = array->GetLength();
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj = (*array)[i];
            AutoPtr<SmsReferenceKey> message = (SmsReferenceKey*)(IObject::Probe(obj));
            Int32 rows;
            mResolver->Delete(sRawUri, InboundSmsHandler::SELECT_BY_REFERENCE,
                    message->GetDeleteWhereArgs(), &rows);
            if (rows == 0) {
                //Rlog->E(TAG, "No rows were deleted from raw table!");
                Logger::E(TAG, "No rows were deleted from raw table!");
            } else if (DBG) {
                //Rlog->D(TAG, "Deleted " + rows + " rows from raw table for incomplete "
                //        + message.mMessageCount + " part message");
                Logger::D(TAG, "Deleted %d rows from raw table for incomplete %d part message",
                        rows, message->mMessageCount);
            }
        }
    //} Catch (SQLException e) {
    //    Rlog->E(TAG, "error reading pending SMS messages", e);
    //} finally {
        if (cursor != NULL) {
            ICloseable::Probe(cursor)->Close();
        }
        if (DBG)
        {
            //Rlog->D(TAG, "finished scanning raw table in "
            //    + ((System->NanoTime() - startTime) / 1000000) + " ms");
            Int64 endTime;
            system->GetNanoTime(&endTime);
            Logger::D(TAG, "finished scanning raw table in %lld ms", (endTime - startTime) / 1000000);
        }
    //}
}

void CSmsBroadcastUndelivered::BroadcastSms(
    /* [in] */ IInboundSmsTracker* _tracker)
{
    InboundSmsTracker* tracker = (InboundSmsTracker*)(IObject::Probe(_tracker));
    AutoPtr<IInboundSmsHandler> handler;
    if (tracker->Is3gpp2()) {
        handler = IInboundSmsHandler::Probe(mCdmaInboundSmsHandler);
    } else {
        handler = IInboundSmsHandler::Probe(mGsmInboundSmsHandler);
    }
    if (handler != NULL) {
        IStateMachine::Probe(handler)->SendMessage(InboundSmsHandler::EVENT_BROADCAST_SMS, TO_IINTERFACE(tracker));
    } else {
        //Rlog->E(TAG, "NULL handler for " + tracker->GetFormat() + " format, can't deliver.");
        Logger::E(TAG, "NULL handler for %s format, can't deliver.", tracker->GetFormat().string());
    }
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
