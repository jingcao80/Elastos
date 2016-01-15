
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/legacy/RequestQueue.h"
#include "elastos/droid/hardware/camera2/legacy/CBurstHolder.h"
#include "elastos/droid/utility/CPairHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::IInteger64;
using Elastos::Utility::IDeque;
using Elastos::Utility::CArrayDeque;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(RequestQueue, Object, IRequestQueue)

const String RequestQueue::TAG("RequestQueue");
const Int64 RequestQueue::INVALID_FRAME = -1;

RequestQueue::RequestQueue()
    : mCurrentFrameNumber(0)
    , mCurrentRepeatingFrameNumber(INVALID_FRAME)
    , mCurrentRequestId(0)
{
    CArrayDeque::New((IArrayDeque**)&mRequestQueue);
}

ECode RequestQueue::constructor()
{
    return NOERROR;
}

ECode RequestQueue::constructor(
    /* [in] */ IList* jpegSurfaceIds)
{
    mJpegSurfaceIds = jpegSurfaceIds;
    return NOERROR;
}

ECode RequestQueue::GetNext(
    /* [out] */ IPair** outpair)
{
    VALIDATE_NOT_NULL(outpair)
    *outpair = NULL;

    synchronized(this) {
        AutoPtr<IInterface> obj;
        assert(0);
        //mRequestQueue->Poll((IInterface**)&obj);
        AutoPtr<IBurstHolder> next = IBurstHolder::Probe(obj);
        if (next == NULL && mRepeatingRequest != NULL) {
            next = mRepeatingRequest;
            Int32 value;
            next->GetNumberOfRequests(&value);
            mCurrentRepeatingFrameNumber = mCurrentFrameNumber + value;
        }

        if (next == NULL) {
            *outpair = NULL;
            return  NOERROR;
        }

        AutoPtr<IPairHelper> helper;
        CPairHelper::AcquireSingleton((IPairHelper**)&helper);
        AutoPtr<IPair> ret;
        AutoPtr<IInteger64> num = CoreUtils::Convert(mCurrentFrameNumber);
        helper->Create(TO_IINTERFACE(next), TO_IINTERFACE(num), (IPair**)&ret);

        Int32 value;
        next->GetNumberOfRequests(&value);
        mCurrentFrameNumber += value;
        *outpair = ret;
        REFCOUNT_ADD(*outpair);
    }
    return NOERROR;
}

ECode RequestQueue::StopRepeating(
    /* [in] */ Int32 requestId,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    synchronized(this) {
        Int64 ret = INVALID_FRAME;
        if (mRepeatingRequest != NULL) {
            Int32 id;
            mRepeatingRequest->GetRequestId(&id);
            if (id == requestId) {
                mRepeatingRequest = NULL;
                ret = (mCurrentRepeatingFrameNumber == INVALID_FRAME) ? INVALID_FRAME :
                        mCurrentRepeatingFrameNumber - 1;
                mCurrentRepeatingFrameNumber = INVALID_FRAME;
                Slogger::I(TAG, "Repeating capture request cancelled.");
            }
        }
        else {
            Slogger::E(TAG, "cancel failed: no repeating request exists for request id: %d", requestId);
        }
        *value = ret;
    }
    return NOERROR;
}

ECode RequestQueue::StopRepeating(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    synchronized(this) {
        if (mRepeatingRequest == NULL) {
            Slogger::E(TAG, "cancel failed: no repeating request exists.");
            *value = INVALID_FRAME;
            return NOERROR;
        }

        Int32 id;
        mRepeatingRequest->GetRequestId(&id);
        StopRepeating(id, value);
    }
    return NOERROR;
}

ECode RequestQueue::Submit(
    /* [in] */ IList* requests,
    /* [in] */ Boolean repeating,
    /* [in] */ ILongParcelable* frameNumber,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    synchronized(this) {
        Int32 requestId = mCurrentRequestId++;
        AutoPtr<IBurstHolder> burst;
        CBurstHolder::New(requestId, repeating, requests,
                ICollection::Probe(mJpegSurfaceIds), (IBurstHolder**)&burst);
        Int64 ret = INVALID_FRAME;

        Boolean result;
        burst->IsRepeating(&result);
        if (result) {
            Slogger::I(TAG, "Repeating capture request set.");
            if (mRepeatingRequest != NULL) {
                ret = (mCurrentRepeatingFrameNumber == INVALID_FRAME) ? INVALID_FRAME :
                        mCurrentRepeatingFrameNumber - 1;
            }
            mCurrentRepeatingFrameNumber = INVALID_FRAME;
            mRepeatingRequest = burst;
        }
        else {
            IDeque::Probe(mRequestQueue)->Offer(TO_IINTERFACE(burst), &result);
            Int32 id;
            burst->GetRequestId(&id);
            FAIL_RETURN(CalculateLastFrame(id, &ret))
        }
        frameNumber->SetNumber(ret);
        *value = requestId;
    }
    return NOERROR;
}

ECode RequestQueue::CalculateLastFrame(
    /* [in] */ Int32 requestId,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int64 total = mCurrentFrameNumber;
    Int32 size;
    mRequestQueue->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
    mRequestQueue->ToArray((ArrayOf<IInterface*>**)&array);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj = (*array)[i];
        AutoPtr<IBurstHolder> b = IBurstHolder::Probe(obj);

        Int32 num;
        b->GetNumberOfRequests(&num);
        total += num;
        Int32 id;
        b->GetRequestId(&id);
        if (id == requestId) {
            *result = total - 1;
            return NOERROR;
        }
    }

    // throw new IllegalStateException(
    //         "At least one request must be in the queue to calculate frame number");
    Slogger::E(TAG, "At least one request must be in the queue to calculate frame number");
    return E_ILLEGAL_STATE_EXCEPTION;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
