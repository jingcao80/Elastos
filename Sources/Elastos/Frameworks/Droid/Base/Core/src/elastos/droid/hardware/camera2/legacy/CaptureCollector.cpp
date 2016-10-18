
#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/hardware/camera2/legacy/CaptureCollector.h"
#include "elastos/droid/utility/CPair.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CPair;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraDeviceImplCameraDeviceCallbacks;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CArrayDeque;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL_2(CaptureCollector::CaptureHolder, Object,
        ICaptureCollectorCaptureHolder, IComparable)

CaptureCollector::CaptureHolder::CaptureHolder(
    /* [in] */ IRequestHolder* request,
    /* [in] */ ILegacyRequest* legacyHolder,
    /* [in] */ CaptureCollector* host)
    : mRequest(request)
    , mLegacy(legacyHolder)
    , mTimestamp(0)
    , mReceivedFlags(0)
    , mHasStarted(FALSE)
    , mFailedJpeg(FALSE)
    , mFailedPreview(FALSE)
    , mCompleted(FALSE)
    , mPreviewCompleted(FALSE)
    , mHost(host)
{
    request->HasJpegTargets(&mNeedsJpeg);
    request->HasPreviewTargets(&mNeedsPreview);
}

Boolean CaptureCollector::CaptureHolder::IsPreviewCompleted()
{
    return (mReceivedFlags & FLAG_RECEIVED_ALL_PREVIEW) == FLAG_RECEIVED_ALL_PREVIEW;
}

Boolean CaptureCollector::CaptureHolder::IsJpegCompleted()
{
    return (mReceivedFlags & FLAG_RECEIVED_ALL_JPEG) == FLAG_RECEIVED_ALL_JPEG;
}

Boolean CaptureCollector::CaptureHolder::IsCompleted()
{
    return (mNeedsJpeg == IsJpegCompleted()) && (mNeedsPreview == IsPreviewCompleted());
}

void CaptureCollector::CaptureHolder::TryComplete()
{
    if (!mPreviewCompleted && mNeedsPreview && IsPreviewCompleted()) {
        mHost->OnPreviewCompleted();
        mPreviewCompleted = TRUE;
    }

    if (IsCompleted() && !mCompleted) {
        if (mFailedPreview || mFailedJpeg) {
            if (!mHasStarted) {
                // Send a request error if the capture has not yet started.
                mRequest->FailRequest();
                Boolean result;
                mHost->mDeviceState->SetCaptureStart(mRequest, mTimestamp,
                        ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_REQUEST, &result);
            }
            else {
                // Send buffer dropped errors for each pending buffer if the request has
                // started.
                if (mFailedPreview) {
                    Int32 id;
                    mRequest->GetRequestId(&id);
                    Logger::W(TAG, "Preview buffers dropped for request: %d",
                            id);
                    Int32 num;
                    mRequest->NumPreviewTargets(&num);
                    for (Int32 i = 0; i < num; i++) {
                        Boolean result;
                        mHost->mDeviceState->SetCaptureResult(mRequest, /*result*/NULL,
                                ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_BUFFER, &result);
                    }
                }
                if (mFailedJpeg) {
                    Int32 id;
                    mRequest->GetRequestId(&id);
                    Logger::W(TAG, "Jpeg buffers dropped for request: %d",
                            id);
                    Int32 num;
                    mRequest->NumJpegTargets(&num);
                    for (Int32 i = 0; i < num; i++) {
                        Boolean result;
                        mHost->mDeviceState->SetCaptureResult(mRequest, /*result*/NULL,
                                ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_BUFFER, &result);
                    }
                }
            }
        }
        mHost->OnRequestCompleted(this);
        mCompleted = TRUE;
    }
    return;
}

ECode CaptureCollector::CaptureHolder::SetJpegTimestamp(
    /* [in] */ Int64 timestamp)
{
    if (DEBUG) {
        Int32 id;
        mRequest->GetRequestId(&id);
        Logger::D(TAG, "setJpegTimestamp - called for request %d", id);
    }
    if (!mNeedsJpeg) {
        // throw new IllegalStateException(
        //         "setJpegTimestamp called for capture with no jpeg targets.");
        Logger::E(TAG, "setJpegTimestamp called for capture with no jpeg targets.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (IsCompleted()) {
        // throw new IllegalStateException(
        //         "setJpegTimestamp called on already completed request.");
        Logger::E(TAG, "setJpegTimestamp called on already completed request.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mReceivedFlags |= FLAG_RECEIVED_JPEG_TS;

    if (mTimestamp == 0) {
        mTimestamp = timestamp;
    }

    if (!mHasStarted) {
        mHasStarted = TRUE;
        Boolean result;
        mHost->mDeviceState->SetCaptureStart(mRequest, mTimestamp,
                ICameraDeviceState::NO_CAPTURE_ERROR, &result);
    }

    TryComplete();
    return NOERROR;
}

ECode CaptureCollector::CaptureHolder::SetJpegProduced()
{
    if (DEBUG) {
        Int32 id;
        mRequest->GetRequestId(&id);
        Logger::D(TAG, "setJpegProduced - called for request %d", id);
    }
    if (!mNeedsJpeg) {
        // throw new IllegalStateException(
        //         "setJpegProduced called for capture with no jpeg targets.");
        Logger::E(TAG, "setJpegProduced called for capture with no jpeg targets.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (IsCompleted()) {
        // throw new IllegalStateException(
        //         "setJpegProduced called on already completed request.");
        Logger::E(TAG, "setJpegProduced called on already completed request.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mReceivedFlags |= FLAG_RECEIVED_JPEG;
    TryComplete();
    return NOERROR;
}

void CaptureCollector::CaptureHolder::SetJpegFailed()
{
    if (DEBUG) {
        Int32 id;
        mRequest->GetRequestId(&id);
        Logger::D(TAG, "setJpegFailed - called for request %d", id);
    }
    if (!mNeedsJpeg || IsJpegCompleted()) {
        return;
    }
    mFailedJpeg = TRUE;

    mReceivedFlags |= FLAG_RECEIVED_JPEG;
    mReceivedFlags |= FLAG_RECEIVED_JPEG_TS;
    TryComplete();
    return;
}

ECode CaptureCollector::CaptureHolder::SetPreviewTimestamp(
    /* [in] */ Int64 timestamp)
{
    if (DEBUG) {
        Int32 id;
        mRequest->GetRequestId(&id);
        Logger::D(TAG, "setPreviewTimestamp - called for request %d", id);
    }
    if (!mNeedsPreview) {
        // throw new IllegalStateException(
        //         "setPreviewTimestamp called for capture with no preview targets.");
        Logger::E(TAG, "setPreviewTimestamp called for capture with no preview targets.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (IsCompleted()) {
        // throw new IllegalStateException(
        //         "setPreviewTimestamp called on already completed request.");
        Logger::E(TAG, "setPreviewTimestamp called on already completed request.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mReceivedFlags |= FLAG_RECEIVED_PREVIEW_TS;

    if (mTimestamp == 0) {
        mTimestamp = timestamp;
    }

    if (!mNeedsJpeg) {
        if (!mHasStarted) {
            mHasStarted = TRUE;
            Boolean result;
            mHost->mDeviceState->SetCaptureStart(mRequest, mTimestamp,
                    ICameraDeviceState::NO_CAPTURE_ERROR, &result);
        }
    }

    TryComplete();
    return NOERROR;
}

ECode CaptureCollector::CaptureHolder::SetPreviewProduced()
{
    if (DEBUG) {
        Int32 id;
        mRequest->GetRequestId(&id);
        Logger::D(TAG, "setPreviewProduced - called for request %d", id);
    }
    if (!mNeedsPreview) {
        // throw new IllegalStateException(
        //         "setPreviewProduced called for capture with no preview targets.");
        Logger::E(TAG, "setPreviewProduced called for capture with no preview targets.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (IsCompleted()) {
        // throw new IllegalStateException(
        //         "setPreviewProduced called on already completed request.");
        Logger::E(TAG, "setPreviewProduced called on already completed request.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mReceivedFlags |= FLAG_RECEIVED_PREVIEW;
    TryComplete();
    return NOERROR;
}

void CaptureCollector::CaptureHolder::SetPreviewFailed()
{
    if (DEBUG) {
        Int32 id;
        mRequest->GetRequestId(&id);
        Logger::D(TAG, "setPreviewFailed - called for request %d", id);
    }
    if (!mNeedsPreview || IsPreviewCompleted()) {
        return;
    }
    mFailedPreview = TRUE;

    mReceivedFlags |= FLAG_RECEIVED_PREVIEW;
    mReceivedFlags |= FLAG_RECEIVED_PREVIEW_TS;
    TryComplete();
    return;
}

CARAPI CaptureCollector::CaptureHolder::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int64 number;
    mRequest->GetFrameNumber(&number);

    AutoPtr<ICaptureCollectorCaptureHolder> collector =
            ICaptureCollectorCaptureHolder::Probe(another);
    Int64 anotherNumber;
    ((CaptureCollector::CaptureHolder*)collector.Get())->mRequest->GetFrameNumber(&anotherNumber);

    if (number > anotherNumber) {
        *result = 1;
        return NOERROR;
    }
    else if (number == anotherNumber) {
        *result = 0;
        return NOERROR;
    }
    else {
        *result = -1;
        return NOERROR;
    }
    return NOERROR;
}

CARAPI CaptureCollector::CaptureHolder::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (ICaptureCollectorCaptureHolder::Probe(other) != NULL) {
        Int32 _result;
        CompareTo(other, &_result);
        *result = _result;
    }
    else {
        *result = FALSE;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CaptureCollector, Object, ICaptureCollector)

const String CaptureCollector::TAG("CaptureCollector");
const Boolean CaptureCollector::DEBUG = FALSE;//Log.isLoggable(LegacyCameraDevice.DEBUG_PROP, Log.DEBUG);

const Int32 CaptureCollector::FLAG_RECEIVED_JPEG = 1;
const Int32 CaptureCollector::FLAG_RECEIVED_JPEG_TS = 2;
const Int32 CaptureCollector::FLAG_RECEIVED_PREVIEW = 4;
const Int32 CaptureCollector::FLAG_RECEIVED_PREVIEW_TS = 8;
const Int32 CaptureCollector::FLAG_RECEIVED_ALL_JPEG = FLAG_RECEIVED_JPEG | FLAG_RECEIVED_JPEG_TS;
const Int32 CaptureCollector::FLAG_RECEIVED_ALL_PREVIEW = FLAG_RECEIVED_PREVIEW |
        FLAG_RECEIVED_PREVIEW_TS;

const Int32 CaptureCollector::MAX_JPEGS_IN_FLIGHT = 1;

CaptureCollector::CaptureCollector()
    : mInFlight(0)
    , mInFlightPreviews(0)
    , mMaxInFlight(0)
{
    CArrayList::New((IArrayList**)&mCompletedRequests);
    CReentrantLock::New((IReentrantLock**)&mLock);
}

ECode CaptureCollector::constructor()
{
    return NOERROR;
}

ECode CaptureCollector::constructor(
    /* [in] */ Int32 maxInFlight,
    /* [in] */ ICameraDeviceState* deviceState)
{
    mMaxInFlight = maxInFlight;
    CArrayDeque::New(MAX_JPEGS_IN_FLIGHT, (IArrayDeque**)&mJpegCaptureQueue);
    CArrayDeque::New(MAX_JPEGS_IN_FLIGHT, (IArrayDeque**)&mJpegProduceQueue);
    CArrayDeque::New(mMaxInFlight, (IArrayDeque**)&mPreviewCaptureQueue);
    CArrayDeque::New(mMaxInFlight, (IArrayDeque**)&mPreviewProduceQueue);
    CTreeSet::New((ITreeSet**)&mActiveRequests);
    ILock::Probe(mLock)->NewCondition((ICondition**)&mIsEmpty);
    ILock::Probe(mLock)->NewCondition((ICondition**)&mNotFull);
    ILock::Probe(mLock)->NewCondition((ICondition**)&mPreviewsEmpty);
    mDeviceState = deviceState;
    return NOERROR;
}

ECode CaptureCollector::QueueRequest(
    /* [in] */ IRequestHolder* holder,
    /* [in] */ ILegacyRequest* legacy,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    AutoPtr<CaptureHolder> h = new CaptureHolder(holder, legacy, this);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    if (DEBUG) {
        Int32 id;
        holder->GetRequestId(&id);
        Logger::D(TAG, "queueRequest  for request %d - %d requests remain in flight.", id, mInFlight);
    }

    if (!(h->mNeedsJpeg || h->mNeedsPreview)) {
        //throw new IllegalStateException("Request must target at least one output surface!");
        Logger::E(TAG, "Request must target at least one output surface!");
        ILock::Probe(lock)->UnLock();
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec;
    if (h->mNeedsJpeg) {
        // Wait for all current requests to finish before queueing jpeg.
        while (mInFlight > 0) {
            if (nanos <= 0) {
                ILock::Probe(lock)->UnLock();
                *value = FALSE;
                return NOERROR;
            }
            ec = mIsEmpty->AwaitNanos(nanos, &nanos);
            if (FAILED(ec)) {
                ILock::Probe(lock)->UnLock();
                return NOERROR;
            }
        }
        mJpegCaptureQueue->Add(TO_IINTERFACE(h));
        mJpegProduceQueue->Add(TO_IINTERFACE(h));
    }
    if (h->mNeedsPreview) {
        while (mInFlight >= mMaxInFlight) {
            if (nanos <= 0) {
                ILock::Probe(lock)->UnLock();
                *value = FALSE;
                return NOERROR;
            }
            ec = mNotFull->AwaitNanos(nanos, &nanos);
            if (FAILED(ec)) {
                ILock::Probe(lock)->UnLock();
                return NOERROR;
            }
        }
        mPreviewCaptureQueue->Add(TO_IINTERFACE(h));
        mPreviewProduceQueue->Add(TO_IINTERFACE(h));
        mInFlightPreviews++;
    }
    mActiveRequests->Add(TO_IINTERFACE(h));

    mInFlight++;
    *value = TRUE;
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::WaitForEmpty(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    while (mInFlight > 0) {
        if (nanos <= 0) {
            *value = FALSE;
            ILock::Probe(lock)->UnLock();
            return NOERROR;
        }
        ECode ec = mIsEmpty->AwaitNanos(nanos, &nanos);
        if (FAILED(ec)) {
            ILock::Probe(lock)->UnLock();
            return NOERROR;
        }
    }
    *value = TRUE;
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::WaitForPreviewsEmpty(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    while (mInFlightPreviews > 0) {
        if (nanos <= 0) {
            *value = FALSE;
            ILock::Probe(lock)->UnLock();
            return NOERROR;
        }
        ECode ec = mPreviewsEmpty->AwaitNanos(nanos, &nanos);
        if (FAILED(ec)) {
            ILock::Probe(lock)->UnLock();
            return NOERROR;
        }
    }
    *value = TRUE;
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::WaitForRequestCompleted(
    /* [in] */IRequestHolder* holder,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [in] */ IInteger64* timestamp,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    while (!RemoveRequestIfCompleted(holder, /*out*/timestamp)) {
        if (nanos <= 0) {
            *value = FALSE;
            ILock::Probe(lock)->UnLock();
            return NOERROR;
        }
        ECode ec = mNotFull->AwaitNanos(nanos, &nanos);
        if (FAILED(ec)) {
            ILock::Probe(lock)->UnLock();
            return NOERROR;
        }
    }
    *value = TRUE;
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

Boolean CaptureCollector::RemoveRequestIfCompleted(
    /* [in] */ IRequestHolder* holder,
    /* [in] */ IInteger64* timestamp)
{
    Int32 size;
    mCompletedRequests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCompletedRequests->Get(i, (IInterface**)&obj);
        AutoPtr<CaptureHolder> h = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);

        Boolean result;
        IObject::Probe(h->mRequest)->Equals(holder, &result);
        if (result) {
            timestamp = CoreUtils::Convert(h->mTimestamp);
            mCompletedRequests->Remove(i);
            return TRUE;
        }
    }

    return FALSE;
}

ECode CaptureCollector::JpegCaptured(
    /* [in] */ Int64 timestamp,
    /* [out] */ IRequestHolder** outrh)
{
    VALIDATE_NOT_NULL(outrh);
    *outrh = NULL;

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    AutoPtr<IInterface> obj;
    assert(0 && "IArrayDeque::Poll");
    //mJpegCaptureQueue->Poll((IInterface**)&obj);
    AutoPtr<CaptureHolder> h = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);
    if (h == NULL) {
        Logger::W(TAG, "jpegCaptured called with no jpeg request on queue!");
        *outrh = NULL;
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    ECode ec = h->SetJpegTimestamp(timestamp);
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    *outrh = h->mRequest;
    REFCOUNT_ADD(*outrh);
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::JpegProduced(
    /* [out] */ IPair** outpair)
{
    VALIDATE_NOT_NULL(outpair);
    *outpair = NULL;

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    AutoPtr<IInterface> obj;
    assert(0 && "IArrayDeque::Poll");
    //mJpegProduceQueue->Poll((IInterface**)&obj);
    AutoPtr<CaptureHolder> h = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);
    if (h == NULL) {
        Logger::W(TAG, "jpegProduced called with no jpeg request on queue!");
        *outpair = NULL;
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    ECode ec = h->SetJpegProduced();
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    AutoPtr<IPair> tmp;
    AutoPtr<IInteger64> obj2 = CoreUtils::Convert(h->mTimestamp);
    CPair::New(TO_IINTERFACE(h->mRequest), TO_IINTERFACE(obj2), (IPair**)&tmp);
    *outpair = tmp;
    REFCOUNT_ADD(*outpair);
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}226.75
    return NOERROR;
}

ECode CaptureCollector::HasPendingPreviewCaptures(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    Boolean result;
    ECode ec = mPreviewCaptureQueue->IsEmpty(&result);
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    *value = !result;
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::PreviewCaptured(
    /* [in] */ Int64 timestamp,
    /* [out] */ IPair** outpair)
{
    VALIDATE_NOT_NULL(outpair);
    *outpair = NULL;

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    AutoPtr<IInterface> obj;
    assert(0 && "IArrayDeque::Poll");
    //mPreviewCaptureQueue->Poll((IInterface**)&obj);
    AutoPtr<CaptureHolder> h = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);
    if (h == NULL) {
        if (DEBUG) {
            Logger::D(TAG, "previewCaptured called with no preview request on queue!");
        }
        *outpair = NULL;
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    ECode ec = h->SetPreviewTimestamp(timestamp);
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    AutoPtr<IPair> tmp;
    AutoPtr<IInteger64> obj2 = CoreUtils::Convert(h->mTimestamp);
    CPair::New(TO_IINTERFACE(h->mRequest), TO_IINTERFACE(obj2), (IPair**)&tmp);
    *outpair = tmp;
    REFCOUNT_ADD(*outpair);
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::PreviewProduced(
    /* [out] */ IRequestHolder** outrh)
{
    VALIDATE_NOT_NULL(outrh);
    *outrh = NULL;

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    AutoPtr<IInterface> obj;
    assert(0 && "IArrayDeque::Poll");
    //mPreviewProduceQueue->Poll((IInterface**)&obj);
    AutoPtr<CaptureHolder> h = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);
    if (h == NULL) {
        Logger::W(TAG, "previewProduced called with no preview request on queue!");
        *outrh = NULL;
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    ECode ec = h->SetPreviewProduced();
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    *outrh = h->mRequest;
    REFCOUNT_ADD(*outrh);
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::FailNextPreview()
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    //try {
    AutoPtr<IInterface> obj;
    ECode ec = NOERROR;
    assert(0 && "IArrayDeque::Peek");
    //ec = mPreviewCaptureQueue->Peek((IInterface**)&obj);
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    AutoPtr<CaptureHolder> h1 = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);

    AutoPtr<IInterface> obj2;
    assert(0 && "IArrayDeque::Peek");
    //ec = mPreviewProduceQueue->Peek((IInterface**)&obj2);
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    AutoPtr<CaptureHolder> h2 = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj2);

    // Find the request with the lowest frame number.
    Int32 res;
    AutoPtr<CaptureHolder> h = (h1 == NULL) ? h2 :
                      ((h2 == NULL) ? h1 :
                      (((h1->CompareTo(TO_IINTERFACE(h2), &res), res) <= 0) ? h1 :
                      h2));

    if (h != NULL) {
        mPreviewCaptureQueue->Remove(TO_IINTERFACE(h));
        mPreviewProduceQueue->Remove(TO_IINTERFACE(h));
        mActiveRequests->Remove(TO_IINTERFACE(h));
        h->SetPreviewFailed();
    }
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::FailNextJpeg()
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();

    //try {
    AutoPtr<IInterface> obj;
    ECode ec = NOERROR;
    assert(0 && "IArrayDeque::Peek");
    //ec = mJpegCaptureQueue->Peek((IInterface**)&obj);
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    AutoPtr<CaptureHolder> h1 = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);
    AutoPtr<IInterface> obj2;
    assert(0 && "IArrayDeque::Peek");
    //ec = mJpegProduceQueue->Peek((IInterface**)&obj2);
    if (FAILED(ec)) {
        ILock::Probe(lock)->UnLock();
        return NOERROR;
    }
    AutoPtr<CaptureHolder> h2 = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj2);

    // Find the request with the lowest frame number.
    Int32 res;
    AutoPtr<CaptureHolder> h = (h1 == NULL) ? h2 :
                      ((h2 == NULL) ? h1 :
                      (((h1->CompareTo(TO_IINTERFACE(h2), &res), res) <= 0) ? h1 :
                      h2));

    if (h != NULL) {
        mJpegCaptureQueue->Remove(TO_IINTERFACE(h));
        mJpegProduceQueue->Remove(TO_IINTERFACE(h));
        mActiveRequests->Remove(TO_IINTERFACE(h));
        h->SetJpegFailed();
    }
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::FailAll()
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();

    //try {
    AutoPtr<CaptureHolder> h;
    do {
        AutoPtr<IInterface> obj;
        ECode ec = mActiveRequests->PollFirst((IInterface**)&obj);
        if (FAILED(ec)) {
            ILock::Probe(lock)->UnLock();
            return NOERROR;
        }
        h = (CaptureHolder*)ICaptureCollectorCaptureHolder::Probe(obj);
        if (h == NULL) {
            break;
        }
        h->SetPreviewFailed();
        h->SetJpegFailed();
    } while(h != NULL);

    mPreviewCaptureQueue->Clear();
    mPreviewProduceQueue->Clear();
    mJpegCaptureQueue->Clear();
    mJpegProduceQueue->Clear();
    //} finally {
    ILock::Probe(lock)->UnLock();
    //}
    return NOERROR;
}

ECode CaptureCollector::OnPreviewCompleted()
{
    mInFlightPreviews--;
    if (mInFlightPreviews < 0) {
        // throw new IllegalStateException(
        //         "More preview captures completed than requests queued.");
        Logger::E(TAG, "More preview captures completed than requests queued.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mInFlightPreviews == 0) {
        mPreviewsEmpty->SignalAll();
    }
    return NOERROR;
}

ECode CaptureCollector::OnRequestCompleted(
    /* [in] */ CaptureHolder* capture)
{
    AutoPtr<IRequestHolder> request = capture->mRequest;

    mInFlight--;
    if (DEBUG) {
        Int32 id;
        request->GetRequestId(&id);
        Logger::D(TAG, "Completed request %d, %d requests remain in flight.", id, mInFlight);
    }
    if (mInFlight < 0) {
        // throw new IllegalStateException(
        //         "More captures completed than requests queued.");
        Logger::E(TAG, "More captures completed than requests queued.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mCompletedRequests->Add(TO_IINTERFACE(capture));
    mActiveRequests->Remove(TO_IINTERFACE(capture));

    mNotFull->SignalAll();
    if (mInFlight == 0) {
        mIsEmpty->SignalAll();
    }
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
