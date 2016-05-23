
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/camera2/impl/CameraDeviceImpl.h"
#include "elastos/droid/hardware/camera2/impl/CCameraCaptureSessionImpl.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImplFrameNumberTracker.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImplCaptureCallbackHolder.h"
#include "elastos/droid/hardware/camera2/impl/CCameraDeviceImplCameraDeviceCallbacks.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CTotalCaptureResult.h"
#include "elastos/droid/hardware/camera2/CCaptureFailure.h"
#include "elastos/droid/hardware/camera2/CCaptureResult.h"
#include "elastos/droid/hardware/camera2/CCaptureRequestBuilder.h"
#include "elastos/droid/hardware/camera2/utils/CameraBinderDecorator.h"
#include "elastos/droid/hardware/camera2/utils/LongParcelable.h"
#include "elastos/droid/utility/CPairHelper.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Hardware::Camera2::CCaptureResult;
using Elastos::Droid::Hardware::Camera2::CTotalCaptureResult;
using Elastos::Droid::Hardware::Camera2::ITotalCaptureResult;
using Elastos::Droid::Hardware::Camera2::CCaptureFailure;
using Elastos::Droid::Hardware::Camera2::ICaptureFailure;
using Elastos::Droid::Hardware::Camera2::CCaptureRequestBuilder;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraDeviceImplCameraDeviceCallbacks;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraDeviceImplFrameNumberTracker;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraDeviceImplCaptureCallbackHolder;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraCaptureSessionImpl;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Utils::CameraBinderDecorator;
using Elastos::Droid::Hardware::Camera2::Utils::ILongParcelable;
using Elastos::Droid::Hardware::Camera2::Utils::LongParcelable;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Core::Math;
using Elastos::Core::IInteger64;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CTreeSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

CAR_INTERFACE_IMPL(CameraDeviceImpl::CaptureCallback, Object, ICameraDeviceImplCaptureCallback)

ECode CameraDeviceImpl::CaptureCallback::OnCaptureStarted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int64 frameNumber)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallback::OnCapturePartial(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* result)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallback::OnCaptureProgressed(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* partialResult)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallback::OnCaptureCompleted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ITotalCaptureResult* result)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallback::OnCaptureFailed(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureFailure* failure)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallback::OnCaptureSequenceCompleted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId,
    /* [in] */ Int64 frameNumber)
{
    // default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallback::OnCaptureSequenceAborted(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 sequenceId)
{
    // default empty implementation
    return NOERROR;
}

CAR_INTERFACE_IMPL(CameraDeviceImpl::StateCallbackKK, CameraDevice::StateCallback,
        ICameraDeviceImplStateCallbackKK)

ECode CameraDeviceImpl::StateCallbackKK::OnUnconfigured(
    /* [in] */ ICameraDevice* camera)
{
    // Default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::StateCallbackKK::OnActive(
    /* [in] */ ICameraDevice* camera)
{
    // Default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::StateCallbackKK::OnBusy(
    /* [in] */ ICameraDevice* camera)
{
    // Default empty implementation
    return NOERROR;
}

ECode CameraDeviceImpl::StateCallbackKK::OnIdle(
    /* [in] */ ICameraDevice* camera)
{
    // Default empty implementation
    return NOERROR;
}

CAR_INTERFACE_IMPL(CameraDeviceImpl::CaptureCallbackHolder, Object, ICameraDeviceImplCaptureCallbackHolder)

CameraDeviceImpl::CaptureCallbackHolder::CaptureCallbackHolder()
    : mRepeating(FALSE)
{
}

ECode CameraDeviceImpl::CaptureCallbackHolder::constructor()
{
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallbackHolder::constructor(
    /* [in] */ ICameraDeviceImplCaptureCallback* _callback,
    /* [in] */ IList* requestList,
    /* [in] */ IHandler* handler,
    /* [in] */ Boolean repeating)
{
    if (_callback == NULL || handler == NULL) {
        // throw new UnsupportedOperationException(
        //     "Must have a valid handler and a valid callback");
        Slogger::E("CameraDeviceImpl::CaptureCallbackHolder::", "Must have a "
                "valid handler and a valid callback");
        //return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    mRepeating = repeating;
    mHandler = handler;
    CArrayList::New(ICollection::Probe(requestList), (IList**)&mRequestList);
    mCallback = _callback;
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallbackHolder::IsRepeating(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRepeating;
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallbackHolder::GetCallback(
    /* [out] */ ICameraDeviceImplCaptureCallback** _callback)
{
    VALIDATE_NOT_NULL(_callback);

    *_callback = mCallback;
    REFCOUNT_ADD(*_callback);
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallbackHolder::GetRequest(
    /* [in] */ Int32 subsequenceId,
    /* [out] */ ICaptureRequest** request)
{
    VALIDATE_NOT_NULL(request);
    *request = NULL;

    Int32 size;
    mRequestList->GetSize(&size);
    if (subsequenceId >= size) {
        // throw new IllegalArgumentException(
        //         String.format(
        //                 "Requested subsequenceId %d is larger than request list size %d.",
        //                 subsequenceId, mRequestList.size()));
        Slogger::E("CameraDeviceImpl::CaptureCallbackHolder::", "Requested subsequenceId %d is"
                "larger than request list size %d.", subsequenceId, size);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        if (subsequenceId < 0) {
            // throw new IllegalArgumentException(String.format(
            //         "Requested subsequenceId %d is negative", subsequenceId));
            Slogger::E("CameraDeviceImpl::CaptureCallbackHolder::", "Requested"
                    "subsequenceId %d is negative", subsequenceId);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;

        }
        else {
            AutoPtr<IInterface> obj;
            mRequestList->Get(subsequenceId, (IInterface**)&obj);
            *request = ICaptureRequest::Probe(obj);
            REFCOUNT_ADD(*request);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CameraDeviceImpl::CaptureCallbackHolder::GetRequest(
    /* [out] */ ICaptureRequest** request)
{
    VALIDATE_NOT_NULL(request);

    return GetRequest(0, request);
}

ECode CameraDeviceImpl::CaptureCallbackHolder::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler);

    *handler = mHandler;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CameraDeviceImpl::FrameNumberTracker, Object,
        ICameraDeviceImplFrameNumberTracker)

CameraDeviceImpl::FrameNumberTracker::FrameNumberTracker()
    : mCompletedFrameNumber(-1)
    , _mHost(NULL)
    , mHost(NULL)
{
}

ECode CameraDeviceImpl::FrameNumberTracker::constructor()
{
    return NOERROR;
}

ECode CameraDeviceImpl::FrameNumberTracker::constructor(
    /* [in] */ ICameraDeviceImpl* host)
{
    _mHost = host;
    mHost = (CameraDeviceImpl*)_mHost;
    return CTreeSet::New((ITreeSet**)&mFutureErrorSet);
}

ECode CameraDeviceImpl::FrameNumberTracker::Update()
{
    AutoPtr<IIterator> iter;
    mFutureErrorSet->GetIterator((IIterator**)&iter);

    Boolean result;
    while (iter->HasNext(&result), result) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IInteger64> errorFrameNumberObj = IInteger64::Probe(obj);
        Int64 errorFrameNumber;
        errorFrameNumberObj->GetValue(&errorFrameNumber);

        if (errorFrameNumber == mCompletedFrameNumber + 1) {
            mCompletedFrameNumber++;
            iter->Remove();
        }
        else {
            break;
        }
    }
    return NOERROR;
}

ECode CameraDeviceImpl::FrameNumberTracker::UpdateTracker(
    /* [in] */ Int64 frameNumber,
    /* [in] */ Boolean isError)
{
    if (isError) {
        AutoPtr<IInteger64> obj = CoreUtils::Convert(frameNumber);
        mFutureErrorSet->Add(TO_IINTERFACE(obj));
    }
    else {
        /**
         * HAL cannot send an OnResultReceived for frame N unless it knows for
         * sure that all frames prior to N have either errored out or completed.
         * So if the current frame is not an error, then all previous frames
         * should have arrived. The following line checks whether this holds.
         */
        if (frameNumber != mCompletedFrameNumber + 1) {
            Slogger::E(mHost->TAG, "result frame number %d comes out of order, should be %d + 1",
                    frameNumber, mCompletedFrameNumber);
            // Continue on to set the completed frame number to this frame anyway,
            // to be robust to lower-level errors and allow for clean shutdowns.
        }
        mCompletedFrameNumber = frameNumber;
    }
    return Update();
}

ECode CameraDeviceImpl::FrameNumberTracker::UpdateTracker(
    /* [in] */ Int64 frameNumber,
    /* [in] */ ICaptureResult* result,
    /* [in] */ Boolean partial)
{
    if (!partial) {
        // Update the total result's frame status as being successful
        return UpdateTracker(frameNumber, /*isError*/FALSE);
        // Don't keep a list of total results, we don't need to track them
    }

    if (result == NULL) {
        // Do not record blank results; this also means there will be no total result
        // so it doesn't matter that the partials were not recorded
        return NOERROR;
    }

    // Partial results must be aggregated in-order for that frame number
    AutoPtr<IList> partials = mPartialResults[frameNumber];
    if (partials == NULL) {
        CArrayList::New((IList**)&partials);
        mPartialResults[frameNumber] = partials;
    }

    return partials->Add(TO_IINTERFACE(result));
}

ECode CameraDeviceImpl::FrameNumberTracker::PopPartialResults(
    /* [in] */ Int64 frameNumber,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    HashMap<Int64, AutoPtr<IList> >::Iterator it = mPartialResults.Find(frameNumber);
    if (it != mPartialResults.End()) {
        mPartialResults.Erase(it);
    }
    return NOERROR;
}

ECode CameraDeviceImpl::FrameNumberTracker::GetCompletedFrameNumber(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    *number = mCompletedFrameNumber;
    return NOERROR;
}

CAR_INTERFACE_IMPL_3(CameraDeviceImpl::CameraDeviceCallbacks, Object,
        ICameraDeviceImplCameraDeviceCallbacks, IICameraDeviceCallbacks, IBinder)

CameraDeviceImpl::CameraDeviceCallbacks::CameraDeviceCallbacks()
    : _mHost(NULL)
    , mHost(NULL)
{
}

CameraDeviceImpl::CameraDeviceCallbacks::constructor()
{
    return NOERROR;
}

CameraDeviceImpl::CameraDeviceCallbacks::constructor(
    /* [in] */ ICameraDeviceImpl* host)
{
    _mHost = host;
    mHost = (CameraDeviceImpl*)_mHost;
    return NOERROR;
}

ECode CameraDeviceImpl::CameraDeviceCallbacks::AsBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    *binder = IBinder::Probe(this);
    REFCOUNT_ADD(*binder);
    return NOERROR;
}

ECode CameraDeviceImpl::CameraDeviceCallbacks::OnDeviceError(
    /* [in] */ Int32 errorCode,
    /* [in] */ ICaptureResultExtras* resultExtras)
{
    if (mHost->DEBUG) {
        Int64 number;
        resultExtras->GetFrameNumber(&number);
        Int32 requestId;
        resultExtras->GetRequestId(&requestId);
        Int32 subsequenceId;
        resultExtras->GetSubsequenceId(&subsequenceId);
        Slogger::D(mHost->TAG, "Device error received, code %d, frame number %d,"
                "request ID %d, subseq ID %d",
                errorCode, number, requestId, subsequenceId);
    }

    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) {
            return NOERROR; // Camera already closed
        }

        switch (errorCode) {
            case ERROR_CAMERA_DISCONNECTED:
            {
                Boolean tmp;
                mHost->mDeviceHandler->Post(mHost->mCallOnDisconnected, &tmp);
                break;
            }
            default:
                Slogger::E(mHost->TAG, "Unknown error from camera device: %d", errorCode);
                // no break
            case ERROR_CAMERA_DEVICE:
            case ERROR_CAMERA_SERVICE:
            {
                mHost->mInError = TRUE;
                AutoPtr<OnDeviceErrorRunnable> r = new OnDeviceErrorRunnable(mHost, errorCode);
                Boolean result;
                mHost->mDeviceHandler->Post(r, &result);
                break;
            }
            case ERROR_CAMERA_REQUEST:
            case ERROR_CAMERA_RESULT:
            case ERROR_CAMERA_BUFFER:
                OnCaptureErrorLocked(errorCode, resultExtras);
                break;
        }
    }
    return NOERROR;
}

ECode CameraDeviceImpl::CameraDeviceCallbacks::OnDeviceIdle()
{
    if (mHost->DEBUG) {
        Slogger::D(mHost->TAG, "Camera now idle");
    }

    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        if (!mHost->mIdle) {
            Boolean tmp;
            mHost->mDeviceHandler->Post(mHost->mCallOnIdle, &tmp);
        }
        mHost->mIdle = TRUE;
    }
    return NOERROR;
}

ECode CameraDeviceImpl::CameraDeviceCallbacks::OnCaptureStarted(
    /* [in] */ ICaptureResultExtras* resultExtras,
    /* [in] */ Int64 timestamp)
{
    Int32 requestId;
    resultExtras->GetRequestId(&requestId);
    Int64 frameNumber;
    resultExtras->GetFrameNumber(&frameNumber);

    if (mHost->DEBUG) {
        Slogger::D(mHost->TAG, "Capture started for id %d frame number %d", requestId, frameNumber);
    }
    AutoPtr<ICameraDeviceImplCaptureCallbackHolder> holder;

    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        // Get the callback for this frame ID, if there is one
        AutoPtr<IInterface> obj;
        mHost->mCaptureCallbackMap->Get(requestId, (IInterface**)&obj);
        holder = ICameraDeviceImplCaptureCallbackHolder::Probe(obj);

        if (holder == NULL) {
            return NOERROR;
        }

        if (mHost->IsClosed()) return NOERROR;

        // Dispatch capture start notice
        AutoPtr<OnCaptureStartedRunnable> r = new OnCaptureStartedRunnable(mHost,
                resultExtras, timestamp, frameNumber, holder);
        AutoPtr<IHandler> handler;
        holder->GetHandler((IHandler**)&handler);
        Boolean result;
        handler->Post(r, &result);
    }
    return NOERROR;
}

ECode CameraDeviceImpl::CameraDeviceCallbacks::OnResultReceived(
    /* [in] */ ICameraMetadataNative* result,
    /* [in] */ ICaptureResultExtras* resultExtras)
{
    Int32 requestId;
    resultExtras->GetRequestId(&requestId);
    Int64 frameNumber;
    resultExtras->GetFrameNumber(&frameNumber);

    if (mHost->DEBUG) {
        Slogger::V(mHost->TAG, "Received result frame %d for id %d"
                , frameNumber, requestId);
    }

    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        // TODO: Handle CameraCharacteristics access from CaptureResult correctly.
        AutoPtr<ICameraCharacteristics> characteristics = mHost->GetCharacteristics();
        AutoPtr<IInterface> obj;
        characteristics->Get(CameraCharacteristics::LENS_INFO_SHADING_MAP_SIZE, (IInterface**)&obj);
        result->Set(CameraCharacteristics::LENS_INFO_SHADING_MAP_SIZE, obj);

        AutoPtr<IInterface> obj2;
        mHost->mCaptureCallbackMap->Get(requestId, (IInterface**)&obj2);
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> holder = ICameraDeviceImplCaptureCallbackHolder::Probe(obj2);

        Int32 count;
        resultExtras->GetPartialResultCount(&count);
        Boolean isPartialResult = (count < mHost->mTotalPartialCount);

        // Check if we have a callback for this
        if (holder == NULL) {
            if (mHost->DEBUG) {
                Slogger::D(mHost->TAG,
                        "holder is null, early return at frame %d", frameNumber);
            }

            return mHost->mFrameNumberTracker->UpdateTracker(frameNumber, /*result*/NULL, isPartialResult);
        }

        if (mHost->IsClosed()) {
            if (mHost->DEBUG) {
                Slogger::D(mHost->TAG,
                        "camera is closed, early return at frame %d" ,frameNumber);
            }

            return mHost->mFrameNumberTracker->UpdateTracker(frameNumber, /*result*/NULL, isPartialResult);
        }

        Int32 subsequenceId;
        resultExtras->GetSubsequenceId(&subsequenceId);
        AutoPtr<ICaptureRequest> request;
        holder->GetRequest(subsequenceId, (ICaptureRequest**)&request);

        AutoPtr<IRunnable> resultDispatch;
        AutoPtr<ICaptureResult> finalResult;

        // Either send a partial result or the final capture completed result
        if (isPartialResult) {
            AutoPtr<ICaptureResult> resultAsCapture;
            CCaptureResult::New(result, request, resultExtras, (ICaptureResult**)&resultAsCapture);

            // Partial result
            resultDispatch = new OnResultReceivedRunnable(mHost, holder, request, resultAsCapture);
            finalResult = resultAsCapture;
        }
        else {
            AutoPtr<IList> partialResults;
            mHost->mFrameNumberTracker->PopPartialResults(frameNumber, (IList**)&partialResults);

            AutoPtr<ITotalCaptureResult> resultAsCapture;
            CTotalCaptureResult::New(result, request, resultExtras,
                    partialResults, (ITotalCaptureResult**)&resultAsCapture);

            // Final capture result
            resultDispatch = new OnResultReceivedRunnable2(mHost, holder, request, resultAsCapture);
            finalResult = ICaptureResult::Probe(resultAsCapture);
        }

        AutoPtr<IHandler> handler;
        holder->GetHandler((IHandler**)&handler);
        Boolean result;
        handler->Post(resultDispatch, &result);

        // Collect the partials for a total result; or mark the frame as totally completed
        mHost->mFrameNumberTracker->UpdateTracker(frameNumber, finalResult, isPartialResult);

        // Fire onCaptureSequenceCompleted
        if (!isPartialResult) {
            mHost->CheckAndFireSequenceComplete();
        }
    }
    return NOERROR;
}

ECode CameraDeviceImpl::CameraDeviceCallbacks::OnCaptureErrorLocked(
    /* [in] */ Int32 errorCode,
    /* [in] */ ICaptureResultExtras* resultExtras)
{
    Int32 requestId;
    resultExtras->GetRequestId(&requestId);
    Int32 subsequenceId;
    resultExtras->GetSubsequenceId(&subsequenceId);
    Int64 frameNumber;
    resultExtras->GetFrameNumber(&frameNumber);
    AutoPtr<IInterface> obj;
    mHost->mCaptureCallbackMap->Get(requestId, (IInterface**)&obj);
    AutoPtr<ICameraDeviceImplCaptureCallbackHolder> holder = ICameraDeviceImplCaptureCallbackHolder::Probe(obj);

    AutoPtr<ICaptureRequest> request;
    holder->GetRequest(subsequenceId, (ICaptureRequest**)&request);

    // No way to report buffer errors right now
    if (errorCode == ERROR_CAMERA_BUFFER) {
        Slogger::E(mHost->TAG, "Lost output buffer reported for frame %d", frameNumber);
        return NOERROR;
    }

    Boolean mayHaveBuffers = (errorCode == ERROR_CAMERA_RESULT);

    // This is only approximate - exact handling needs the camera service and HAL to
    // disambiguate between request failures to due abort and due to real errors.
    // For now, assume that if the session believes we're mid-abort, then the error
    // is due to abort.
    Boolean res;
    Int32 reason = (mHost->mCurrentSession != NULL && (mHost->mCurrentSession->IsAborting(&res), res)) ?
            ICaptureFailure::REASON_FLUSHED :
            ICaptureFailure::REASON_ERROR;

    AutoPtr<ICaptureFailure> failure;
    CCaptureFailure::New(
        request,
        reason,
        /*dropped*/ mayHaveBuffers,
        requestId,
        frameNumber,
        (ICaptureFailure**)&failure);

    AutoPtr<OnCaptureErrorLockedRunnable> failureDispatch =
            new OnCaptureErrorLockedRunnable(mHost, holder, request, failure);
    AutoPtr<IHandler> handler;
    holder->GetHandler((IHandler**)&handler);
    Boolean result;
    handler->Post(failureDispatch, &result);

    // Fire onCaptureSequenceCompleted if appropriate
    if (mHost->DEBUG) {
        Slogger::V(mHost->TAG, "got error frame %d", frameNumber);
    }
    mHost->mFrameNumberTracker->UpdateTracker(frameNumber, /*error*/TRUE);
    return mHost->CheckAndFireSequenceComplete();
}

ECode CameraDeviceImpl::CameraDeviceCallbacks::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = NULL;
    return NOERROR;
}

CameraDeviceImpl::CallOnOpeneRunnable::CallOnOpeneRunnable(
    /* [in] */ CameraDeviceImpl* host)
    : mHost(host)
{
}

ECode CameraDeviceImpl::CallOnOpeneRunnable::Run()
{
    AutoPtr<ICameraDeviceImplStateCallbackKK> sessionCallback;
    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        sessionCallback = mHost->mSessionStateCallback;
    }
    if (sessionCallback != NULL) {
        ICameraDeviceStateCallback::Probe(sessionCallback)->OnOpened(mHost);
    }
    return mHost->mDeviceCallback->OnOpened(mHost);
}

CameraDeviceImpl::CallOnUnconfiguredRunnable::CallOnUnconfiguredRunnable(
    /* [in] */ CameraDeviceImpl* host)
    : mHost(host)
{
}

ECode CameraDeviceImpl::CallOnUnconfiguredRunnable::Run()
{
    AutoPtr<ICameraDeviceImplStateCallbackKK> sessionCallback;
    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        sessionCallback = mHost->mSessionStateCallback;
    }
    if (sessionCallback != NULL) {
        sessionCallback->OnUnconfigured(mHost);
    }
    return NOERROR;
}

CameraDeviceImpl::CallOnActiveRunnable::CallOnActiveRunnable(
    /* [in] */ CameraDeviceImpl* host)
    : mHost(host)
{
}

ECode CameraDeviceImpl::CallOnActiveRunnable::Run()
{
    AutoPtr<ICameraDeviceImplStateCallbackKK> sessionCallback;
    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        sessionCallback = mHost->mSessionStateCallback;
    }
    if (sessionCallback != NULL) {
        sessionCallback->OnActive(mHost);
    }
    return NOERROR;
}

CameraDeviceImpl::CallOnBusyRunnable::CallOnBusyRunnable(
    /* [in] */ CameraDeviceImpl* host)
    : mHost(host)
{
}

ECode CameraDeviceImpl::CallOnBusyRunnable::Run()
{
    AutoPtr<ICameraDeviceImplStateCallbackKK> sessionCallback;
    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        sessionCallback = mHost->mSessionStateCallback;
    }
    if (sessionCallback != NULL) {
        sessionCallback->OnBusy(mHost);
    }
    return NOERROR;
}

CameraDeviceImpl::CallOnCloseRunnable::CallOnCloseRunnable(
    /* [in] */ CameraDeviceImpl* host)
    : mHost(host)
    , mClosedOnce(FALSE)
{
}

ECode CameraDeviceImpl::CallOnCloseRunnable::Run()
{
    if (mClosedOnce) {
        //throw new AssertionError("Don't post #onClosed more than once");
        Slogger::E(mHost->TAG, "Don't post #onClosed more than once");
        return E_ASSERTION_ERROR;
    }
    AutoPtr<ICameraDeviceImplStateCallbackKK> sessionCallback;
    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        sessionCallback = mHost->mSessionStateCallback;
    }
    if (sessionCallback != NULL) {
        ICameraDeviceStateCallback::Probe(sessionCallback)->OnClosed(mHost);
    }
    mHost->mDeviceCallback->OnClosed(mHost);
    mClosedOnce = TRUE;
    return NOERROR;
}

CameraDeviceImpl::CallOnIdlRunnable::CallOnIdlRunnable(
    /* [in] */ CameraDeviceImpl* host)
    : mHost(host)
{
}

ECode CameraDeviceImpl::CallOnIdlRunnable::Run()
{
    AutoPtr<ICameraDeviceImplStateCallbackKK> sessionCallback;
    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        sessionCallback = mHost->mSessionStateCallback;
    }
    if (sessionCallback != NULL) {
        sessionCallback->OnIdle(mHost);
    }
    return NOERROR;
}

CameraDeviceImpl::CallOnDisconnectedRunnable::CallOnDisconnectedRunnable(
    /* [in] */ CameraDeviceImpl* host)
    : mHost(host)
{
}

ECode CameraDeviceImpl::CallOnDisconnectedRunnable::Run()
{
    AutoPtr<ICameraDeviceImplStateCallbackKK> sessionCallback;
    Object& lock = mHost->mInterfaceLock;
    {    AutoLock syncLock(lock);
        if (mHost->mRemoteDevice == NULL) return NOERROR; // Camera already closed

        sessionCallback = mHost->mSessionStateCallback;
    }
    if (sessionCallback != NULL) {
        ICameraDeviceStateCallback::Probe(sessionCallback)->OnDisconnected(mHost);
    }
    return mHost->mDeviceCallback->OnDisconnected(mHost);
}

CameraDeviceImpl::RemoteFailureRunnable::RemoteFailureRunnable(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ Boolean isError,
    /* [in] */ Int32 code)
    : mHost(host)
    , mIsError(isError)
    , mCode(code)
{
}

ECode CameraDeviceImpl::RemoteFailureRunnable::Run()
{
    if (mIsError) {
        return mHost->mDeviceCallback->OnError(mHost, mCode);
    }
    else {
        return mHost->mDeviceCallback->OnDisconnected(mHost);
    }
    return NOERROR;
}

CameraDeviceImpl::ResultDispatchRunnable::ResultDispatchRunnable(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ Int32 requestId,
    /* [in] */ Int64 lastFrameNumber,
    /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder)
    : mHost(host)
    , mRequestId(requestId)
    , mLastFrameNumber(lastFrameNumber)
    , mHolder(holder)
{
}

ECode CameraDeviceImpl::ResultDispatchRunnable::Run()
{
    if (!mHost->IsClosed()) {
        if (mHost->DEBUG) {
            Slogger::D(mHost->TAG,
                    "early trigger sequence complete for request %d",
                    mRequestId);
        }
        if (mLastFrameNumber < Elastos::Core::Math::INT32_MIN_VALUE
                || mLastFrameNumber > Elastos::Core::Math::INT32_MAX_VALUE) {
            Slogger::E(mHost->TAG, "%d cannot be cast to int", mLastFrameNumber);
            return E_ASSERTION_ERROR;
        }
        AutoPtr<ICameraDeviceImplCaptureCallback> _callback;
        mHolder->GetCallback((ICameraDeviceImplCaptureCallback**)&_callback);
        return _callback->OnCaptureSequenceAborted(mHost, mRequestId);
    }
    return NOERROR;
}

CameraDeviceImpl::ResultDispatchRunnable2::ResultDispatchRunnable2(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ Int32 requestId,
    /* [in] */ Int64 keyValue,
    /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder)
    : mHost(host)
    , mRequestId(requestId)
    , mKeyValue(keyValue)
    , mHolder(holder)
{
}

ECode CameraDeviceImpl::ResultDispatchRunnable2::Run()
{
    if (!mHost->IsClosed()){
        if (mHost->DEBUG) {
            Slogger::D(mHost->TAG, "fire sequence complete for request %d",mRequestId);
        }

        Int64 lastFrameNumber = mKeyValue;
        if (lastFrameNumber < Elastos::Core::Math::INT32_MIN_VALUE
                || lastFrameNumber > Elastos::Core::Math::INT32_MAX_VALUE) {
            // throw new AssertionError(lastFrameNumber
            //         + " cannot be cast to int");
            Slogger::E(mHost->TAG, "%d cannot be cast to int", lastFrameNumber);
            return E_ASSERTION_ERROR;
        }
        AutoPtr<ICameraDeviceImplCaptureCallback> _callback;
        mHolder->GetCallback((ICameraDeviceImplCaptureCallback**)&_callback);
        return _callback->OnCaptureSequenceCompleted(mHost, mRequestId, lastFrameNumber);
    }
    return NOERROR;
}

CameraDeviceImpl::OnDeviceErrorRunnable::OnDeviceErrorRunnable(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ Int32 errorCode)
    : mHost(host)
    , mErrorCode(errorCode)
{
}

ECode CameraDeviceImpl::OnDeviceErrorRunnable::Run()
{
    if (!mHost->IsClosed()) {
        return mHost->mDeviceCallback->OnError(mHost, mErrorCode);
    }
    return NOERROR;
}

CameraDeviceImpl::OnCaptureStartedRunnable::OnCaptureStartedRunnable(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ ICaptureResultExtras* resultExtras,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int64 frameNumber,
    /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder)
    : mHost(host)
    , mResultExtras(resultExtras)
    , mTimestamp(timestamp)
    , mFrameNumber(frameNumber)
    , mHolder(holder)
{
}

ECode CameraDeviceImpl::OnCaptureStartedRunnable::Run()
{
    if (!mHost->IsClosed()) {
        AutoPtr<ICameraDeviceImplCaptureCallback> _callback;
        mHolder->GetCallback((ICameraDeviceImplCaptureCallback**)&_callback);

        Int32 id;
        mResultExtras->GetSubsequenceId(&id);
        AutoPtr<ICaptureRequest> request;
        mHolder->GetRequest(id, (ICaptureRequest**)&request);
        return _callback->OnCaptureStarted(mHost, request,
            mTimestamp, mFrameNumber);
    }
    return NOERROR;
}

CameraDeviceImpl::OnResultReceivedRunnable::OnResultReceivedRunnable(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* result)
    : mHost(host)
    , mHolder(holder)
    , mRequest(request)
    , mResult(result)
{
}

ECode CameraDeviceImpl::OnResultReceivedRunnable::Run()
{
    if (!mHost->IsClosed()){
        AutoPtr<ICameraDeviceImplCaptureCallback> _callback;
        mHolder->GetCallback((ICameraDeviceImplCaptureCallback**)&_callback);
        return _callback->OnCaptureProgressed(mHost, mRequest, mResult);
    }
    return NOERROR;
}

CameraDeviceImpl::OnResultReceivedRunnable2::OnResultReceivedRunnable2(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ITotalCaptureResult* result)
    : mHost(host)
    , mHolder(holder)
    , mRequest(request)
    , mResult(result)
{
}

ECode CameraDeviceImpl::OnResultReceivedRunnable2::Run()
{
    if (!mHost->IsClosed()){
        AutoPtr<ICameraDeviceImplCaptureCallback> _callback;
        mHolder->GetCallback((ICameraDeviceImplCaptureCallback**)&_callback);
        return _callback->OnCaptureCompleted(mHost, mRequest, mResult);
    }
    return NOERROR;
}

CameraDeviceImpl::OnCaptureErrorLockedRunnable::OnCaptureErrorLockedRunnable(
    /* [in] */ CameraDeviceImpl* host,
    /* [in] */ ICameraDeviceImplCaptureCallbackHolder* holder,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureFailure* failure)
    : mHost(host)
    , mHolder(holder)
    , mRequest(request)
    , mFailure(failure)
{
}

ECode CameraDeviceImpl::OnCaptureErrorLockedRunnable::Run()
{
    if (!mHost->IsClosed()){
        AutoPtr<ICameraDeviceImplCaptureCallback> _callback;
        mHolder->GetCallback((ICameraDeviceImplCaptureCallback**)&_callback);
        return _callback->OnCaptureFailed(mHost, mRequest, mFailure);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CameraDeviceImpl, CameraDevice, ICameraDeviceImpl)

const Int32 CameraDeviceImpl::REQUEST_ID_NONE = -1;

CameraDeviceImpl::CameraDeviceImpl()
    : TAG(NULL)
    , DEBUG(FALSE)
    , mClosing(FALSE)
    , mInError(FALSE)
    , mIdle(TRUE)
    , mRepeatingRequestId(REQUEST_ID_NONE)
    , mCameraId(NULL)
    , mTotalPartialCount(0)
    , mNextSessionId(0)

{
    CCameraDeviceImplCameraDeviceCallbacks::New(this, (ICameraDeviceImplCameraDeviceCallbacks**)&mCallbacks);
    CSparseArray::New((ISparseArray**)&mCaptureCallbackMap);
    CArrayList::New((IArrayList**)&mRepeatingRequestIdDeletedList);
    CSparseArray::New((ISparseArray**)&mConfiguredOutputs);
    CArrayList::New((IList**)&mFrameNumberRequestPairs);
    CCameraDeviceImplFrameNumberTracker::New(this, (ICameraDeviceImplFrameNumberTracker**)&mFrameNumberTracker);

    mCallOnOpened = new CallOnOpeneRunnable(this);
    mCallOnUnconfigured = new CallOnUnconfiguredRunnable(this);
    mCallOnActive = new CallOnActiveRunnable(this);
    mCallOnBusy = new CallOnBusyRunnable(this);
    mCallOnClosed = new CallOnCloseRunnable(this);
    mCallOnIdle = new CallOnIdlRunnable(this);
    mCallOnDisconnected = new CallOnDisconnectedRunnable(this);
}

CameraDeviceImpl::~CameraDeviceImpl()
{
    //try {
    Close();
    //}
    //finally {
    //    super.finalize();
    //}
}

ECode CameraDeviceImpl::constructor()
{
    return NOERROR;
}

ECode CameraDeviceImpl::constructor(
    /* [in] */ const String& cameraId,
    /* [in] */ ICameraDeviceStateCallback* _callback,
    /* [in] */ IHandler* handler,
    /* [in] */ ICameraCharacteristics* characteristics)
{
    if (cameraId == NULL || _callback == NULL || handler == NULL || characteristics == NULL) {
        //throw new IllegalArgumentException("Null argument given");
        Slogger::E("CameraDeviceImpl", "Null argument given");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mCameraId = cameraId;
    mDeviceCallback = _callback;
    mDeviceHandler = handler;
    mCharacteristics = characteristics;

    const Int32 MAX_TAG_LEN = 23;
    String tag = String("CameraDevice-JV-") + mCameraId;
    if (tag.GetLength() > MAX_TAG_LEN) {
        tag = tag.Substring(0, MAX_TAG_LEN);
    }
    TAG = tag;
    DEBUG = FALSE; //Log.isLoggable(TAG, Log.DEBUG);

    AutoPtr<IInterface> obj;
    mCharacteristics->Get(CameraCharacteristics::REQUEST_PARTIAL_RESULT_COUNT, (IInterface**)&obj);
    if (obj == NULL) {
        // 1 means partial result is not supported.
        mTotalPartialCount = 1;
    }
    else {
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        intObj->GetValue(&mTotalPartialCount);
    }
    return NOERROR;
}

ECode CameraDeviceImpl::GetCallbacks(
    /* [out] */ ICameraDeviceImplCameraDeviceCallbacks** outcd)
{
    VALIDATE_NOT_NULL(outcd);

    *outcd = mCallbacks;
    REFCOUNT_ADD(*outcd);
    return NOERROR;
}

ECode CameraDeviceImpl::SetRemoteDevice(
    /* [in] */ IICameraDeviceUser* remoteDevice)
{
    {    AutoLock syncLock(mInterfaceLock);
        // TODO: Move from decorator to direct binder-mediated exceptions
        // If setRemoteFailure already called, do nothing
        if (mInError) return NOERROR;

        AutoPtr<IInterface> obj;
        CameraBinderDecorator::NewInstance(TO_IINTERFACE(remoteDevice), (IInterface**)&obj);
        mRemoteDevice = IICameraDeviceUser::Probe(obj);

        Boolean tmp;
        mDeviceHandler->Post(mCallOnOpened, &tmp);
        mDeviceHandler->Post(mCallOnUnconfigured, &tmp);
    }
    return NOERROR;
}

ECode CameraDeviceImpl::SetRemoteFailure(
    /* [in] */ ECode failure)
{
    int failureCode = ICameraDeviceStateCallback::ERROR_CAMERA_DEVICE;
    Boolean failureIsError = TRUE;

    assert(0);
    // switch (failure.getReason()) {
    //     case ICameraAccessException::CAMERA_IN_USE:
    //         failureCode = ICameraDeviceImplStateCallback::ERROR_CAMERA_IN_USE;
    //         break;
    //     case ICameraAccessException::MAX_CAMERAS_IN_USE:
    //         failureCode = ICameraDeviceImplStateCallback::ERROR_MAX_CAMERAS_IN_USE;
    //         break;
    //     case ICameraAccessException::CAMERA_DISABLED:
    //         failureCode = ICameraDeviceImplStateCallback::ERROR_CAMERA_DISABLED;
    //         break;
    //     case ICameraAccessException::CAMERA_DISCONNECTED:
    //         failureIsError = false;
    //         break;
    //     case ICameraAccessException::CAMERA_ERROR:
    //         failureCode = ICameraDeviceStateCallback::ERROR_CAMERA_DEVICE;
    //         break;
    //     default:
    //         Log.wtf(TAG, "Unknown failure in opening camera device: " + failure.getReason());
    //         break;
    // }
    const Int32 code = failureCode;
    const Boolean isError = failureIsError;
    {    AutoLock syncLock(mInterfaceLock);
        mInError = TRUE;
        AutoPtr<RemoteFailureRunnable> run = new RemoteFailureRunnable(this, isError, code);
        Boolean result;
        mDeviceHandler->Post(run, &result);
    }
    return NOERROR;
}

ECode CameraDeviceImpl::GetId(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mCameraId;
    return NOERROR;
}

ECode CameraDeviceImpl::ConfigureOutputs(
    /* [in] */ IList* outputs)
{
    // Leave this here for backwards compatibility with older code using this directly
    Boolean result;
    return ConfigureOutputsChecked(outputs, &result);
}

ECode CameraDeviceImpl::ConfigureOutputsChecked(
    /* [in] */ IList* inOutputs,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    // Treat a null input the same an empty list
    AutoPtr<IList> outputs = inOutputs;
    if (outputs == NULL) {
        CArrayList::New((IList**)&outputs);
    }
    Boolean success = FALSE;

    {    AutoLock syncLock(mInterfaceLock);
        FAIL_RETURN(CheckIfCameraClosedOrInError())

        AutoPtr<IHashSet> addSet;
        CHashSet::New(ICollection::Probe(outputs), (IHashSet**)&addSet);    // Streams to create
        AutoPtr<IList> deleteList;
        CArrayList::New((IList**)&deleteList);        // Streams to delete

        // Determine which streams need to be created, which to be deleted
        Int32 size;
        mConfiguredOutputs->GetSize(&size);
        for (Int32 i = 0; i < size; ++i) {
            Int32 streamId;
            mConfiguredOutputs->KeyAt(i, &streamId);
            AutoPtr<IInterface> obj;
            mConfiguredOutputs->ValueAt(i, (IInterface**)&obj);
            AutoPtr<ISurface> s = ISurface::Probe(obj);

            Boolean result;
            outputs->Contains(s, &result);
            if (!result) {
                AutoPtr<IInteger32> intObj = CoreUtils::Convert(streamId);
                deleteList->Add(TO_IINTERFACE(intObj));
            }
            else {
                addSet->Remove(TO_IINTERFACE(s));  // Don't create a stream previously created
            }
        }

        Boolean tmp;
        mDeviceHandler->Post(mCallOnBusy, &tmp);
        FAIL_RETURN(StopRepeating())

        // try {
        ECode ec;
        ec = WaitUntilIdle();
        FAIL_GOTO(ec, error);

        Int32 result;
        ec = mRemoteDevice->BeginConfigure(&result);
        FAIL_GOTO(ec, error);
        // Delete all streams first (to free up HW resources)
        deleteList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            deleteList->Get(i, (IInterface**)&obj);
            AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
            Int32 streamId;
            intObj->GetValue(&streamId);

            Int32 res;
            ec = mRemoteDevice->DeleteStream(streamId, &res);
            FAIL_GOTO(ec, error);
            mConfiguredOutputs->Delete(streamId);
        }

        // Add all new streams
        {
            AutoPtr<ArrayOf<IInterface*> > array;
            addSet->ToArray((ArrayOf<IInterface*>**)&array);
            addSet->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj = (*array)[i];
                AutoPtr<ISurface> s = ISurface::Probe(obj);

                // TODO: remove width,height,format since we are ignoring
                // it.
                Int32 streamId;
                ec = mRemoteDevice->CreateStream(0, 0, 0, s, &streamId);
                FAIL_GOTO(ec, error);
                mConfiguredOutputs->Put(streamId, TO_IINTERFACE(s));
            }
        }

        //try {
        {
            Int32 res;
            ECode ec2 = mRemoteDevice->EndConfigure(&res);
            //}
            //catch (IllegalArgumentException e) {
            if (ec2 == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
                // OK. camera service can reject stream config if it's not supported by HAL
                // This is only the result of a programmer misusing the camera2 api.
                Slogger::W(TAG, "Stream configuration failed");
                *value = FALSE;

                Int32 size;
                outputs->GetSize(&size);
                if (success && size > 0) {
                    Boolean res;
                    mDeviceHandler->Post(mCallOnIdle, &res);
                }
                else {
                    // Always return to the 'unconfigured' state if we didn't hit a fatal error
                    Boolean res;
                    mDeviceHandler->Post(mCallOnUnconfigured, &res);
                }
                return NOERROR;
            }
        //}
        }
        success = TRUE;
error:
        // } catch (CameraRuntimeException e) {
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            // if (e.getReason() == CAMERA_IN_USE) {
            //     throw new IllegalStateException("The camera is currently busy." +
            //             " You must wait until the previous operation completes.");
            // }

            // throw e.asChecked();
            Int32 size;
            outputs->GetSize(&size);
            if (success && size > 0) {
                Boolean res;
                mDeviceHandler->Post(mCallOnIdle, &res);
            }
            else {
                // Always return to the 'unconfigured' state if we didn't hit a fatal error
                Boolean res;
                mDeviceHandler->Post(mCallOnUnconfigured, &res);
            }
            return ec;
        }
        // } catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // impossible
            *value = FALSE;
            //return NOERROR;
        }
        // } finally {
        Int32 _size;
        outputs->GetSize(&_size);
        if (success && _size > 0) {
            Boolean res;
            mDeviceHandler->Post(mCallOnIdle, &res);
        }
        else {
            // Always return to the 'unconfigured' state if we didn't hit a fatal error
            Boolean res;
            mDeviceHandler->Post(mCallOnUnconfigured, &res);
        }
        // }
    }

    *value = success;
    return NOERROR;
}

ECode CameraDeviceImpl::CreateCaptureSession(
    /* [in] */ IList* outputs,
    /* [in] */ ICameraCaptureSessionStateCallback* _callback,
    /* [in] */ IHandler* handler)
{
    {    AutoLock syncLock(mInterfaceLock);
        if (DEBUG) {
            Slogger::D(TAG, "createCaptureSession");
        }

        FAIL_RETURN(CheckIfCameraClosedOrInError())

        // Notify current session that it's going away, before starting camera operations
        // After this call completes, the session is not allowed to call into CameraDeviceImpl
        if (mCurrentSession != NULL) {
            mCurrentSession->ReplaceSessionClose();
        }

        // TODO: dont block for this
        Boolean configureSuccess = TRUE;
        //AutoPtr<ICameraAccessException> pendingException;
        //try {
        ECode ec = ConfigureOutputsChecked(outputs, &configureSuccess); // and then block until IDLE
        //} catch (CameraAccessException e) {
        if (ec == (ECode)E_CAMERA_ACCESS_EXCEPTION) {
            configureSuccess = FALSE;
            //pendingException = ec;
            if (DEBUG) {
                Slogger::V(TAG, "createCaptureSession - failed with exception %d", ec);
            }
        }
        //}

        // Fire onConfigured if configureOutputs succeeded, fire onConfigureFailed otherwise.
        AutoPtr<ICameraCaptureSessionImpl> newSession;
        CCameraCaptureSessionImpl::New(mNextSessionId++,
                outputs, _callback, handler, this, mDeviceHandler,
                configureSuccess, (ICameraCaptureSessionImpl**)&newSession);

        // TODO: wait until current session closes, then create the new session
        mCurrentSession = newSession;

        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            return ec;
        }
        // if (pendingException != NULL) {
        //     throw pendingException;
        // }

        mCurrentSession->GetDeviceStateCallback((ICameraDeviceImplStateCallbackKK**)&mSessionStateCallback);
    }
    return NOERROR;
}

ECode CameraDeviceImpl::SetSessionListener(
    /* [in] */ ICameraDeviceImplStateCallbackKK* sessionCallback)
{
    {    AutoLock syncLock(mInterfaceLock);
        mSessionStateCallback = sessionCallback;
    }
    return NOERROR;
}

ECode CameraDeviceImpl::CreateCaptureRequest(
    /* [in] */ Int32 templateType,
    /* [out] */ ICaptureRequestBuilder** builder)
{
    VALIDATE_NOT_NULL(builder);
    *builder = NULL;

    {    AutoLock syncLock(mInterfaceLock);
        FAIL_RETURN(CheckIfCameraClosedOrInError())

        AutoPtr<ICameraMetadataNative> templatedRequest;
        CCameraMetadataNative::New((ICameraMetadataNative**)&templatedRequest);

        //try {
        Int32 tmp;
        ECode ec = mRemoteDevice->CreateDefaultRequest(templateType, /*out*/templatedRequest, &tmp);
        //} catch (CameraRuntimeException e) {
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            //throw e.asChecked();
            return ec;
        }
        //} catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // impossible
            *builder = NULL;
            return NOERROR;
        }
        //}

        AutoPtr<ICaptureRequestBuilder> _builder;
        CCaptureRequestBuilder::New(templatedRequest, (ICaptureRequestBuilder**)&_builder);

        *builder = _builder;
        REFCOUNT_ADD(*builder);
        return NOERROR;
    }
    return NOERROR;
}

ECode CameraDeviceImpl::Capture(
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (DEBUG) {
        Slogger::D(TAG, "calling capture");
    }
    AutoPtr<IList> requestList;
    CArrayList::New((IList**)&requestList);
    requestList->Add(TO_IINTERFACE(request));
    return SubmitCaptureRequest(requestList, ccallback, handler, /*streaming*/FALSE, value);
}

ECode CameraDeviceImpl::CaptureBurst(
    /* [in] */ IList* requests,
    /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (requests == NULL) {
        //throw new IllegalArgumentException("At least one request must be given");
        Slogger::E(TAG, "At least one request must be given");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean result;
    requests->IsEmpty(&result);
    if (result) {
        //throw new IllegalArgumentException("At least one request must be given");
        Slogger::E(TAG, "At least one request must be given");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return SubmitCaptureRequest(requests, ccallback, handler, /*streaming*/FALSE, value);
}

ECode CameraDeviceImpl::CheckEarlyTriggerSequenceComplete(
    /* [in] */ Int32 requestId,
    /* [in] */ Int64 lastFrameNumber)
{
    // lastFrameNumber being equal to NO_FRAMES_CAPTURED means that the request
    // was never sent to HAL. Should trigger onCaptureSequenceAborted immediately.
    if (lastFrameNumber == ICameraDeviceImplCaptureCallback::NO_FRAMES_CAPTURED) {
        AutoPtr<ICameraDeviceImplCaptureCallbackHolder> holder;
        Int32 index;
        mCaptureCallbackMap->IndexOfKey(requestId, &index);

        if (index >= 0) {
            AutoPtr<IInterface> obj;
            mCaptureCallbackMap->ValueAt(index, (IInterface**)&obj);
            holder = ICameraDeviceImplCaptureCallbackHolder::Probe(obj);
        }
        else {
            holder = NULL;
        }

        if (holder != NULL) {
            mCaptureCallbackMap->RemoveAt(index);
            if (DEBUG) {
                Slogger::V(TAG,
                        "remove holder for requestId %d, "
                        "because lastFrame is %d.",
                        requestId, lastFrameNumber);
            }
        }

        if (holder != NULL) {
            if (DEBUG) {
                Slogger::V(TAG, "immediately trigger onCaptureSequenceAborted because"
                        " request did not reach HAL");
            }

            AutoPtr<ResultDispatchRunnable> resultDispatch = new ResultDispatchRunnable(this,
                    requestId, lastFrameNumber, holder);
            AutoPtr<IHandler> handler;
            holder->GetHandler((IHandler**)&handler);
            Boolean result;
            handler->Post(resultDispatch, &result);
        }
        else {
            Slogger::W(TAG,
                    "did not register callback to request %d",
                    requestId);
        }
    }
    else {
        AutoPtr<IInteger64> keyObj = CoreUtils::Convert(lastFrameNumber);
        AutoPtr<IInteger32> valueObj = CoreUtils::Convert(requestId);
        AutoPtr<IPairHelper> helper;
        CPairHelper::AcquireSingleton((IPairHelper**)&helper);
        AutoPtr<IPair> pair;
        helper->Create(TO_IINTERFACE(keyObj), TO_IINTERFACE(valueObj), (IPair**)&pair);
        mFrameNumberRequestPairs->Add(TO_IINTERFACE(pair));
        // It is possible that the last frame has already arrived, so we need to check
        // for sequence completion right away
        CheckAndFireSequenceComplete();
    }
    return NOERROR;
}

ECode CameraDeviceImpl::SubmitCaptureRequest(
    /* [in] */ IList* requestList,
    /* [in] */ ICameraDeviceImplCaptureCallback* _callback,
    /* [in] */ IHandler* handler,
    /* [in] */ Boolean repeating,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    // Need a valid handler, or current thread needs to have a looper, if
    // callback is valid
    AutoPtr<IHandler> newHandler;
    FAIL_RETURN(CheckHandler(handler, _callback, (IHandler**)&newHandler))

    // Make sure that there all requests have at least 1 surface; all surfaces are non-null
    Int32 size;
    requestList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        requestList->Get(i, (IInterface**)&obj);
        AutoPtr<ICaptureRequest> request = ICaptureRequest::Probe(obj);

        AutoPtr<ICollection> coll;
        request->GetTargets((ICollection**)&coll);
        Boolean result;
        coll->IsEmpty(&result);
        if (result) {
            // throw new IllegalArgumentException(
            //         "Each request must have at least one Surface target");
            Slogger::E(TAG, "Each request must have at least one Surface target");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<ArrayOf<IInterface*> > array;
        coll->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<ISurface> surface = ISurface::Probe((*array)[i]);
            if (surface == NULL) {
                //throw new IllegalArgumentException("Null Surface targets are not allowed");
                Slogger::E(TAG, "Null Surface targets are not allowed");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    {    AutoLock syncLock(mInterfaceLock);
        FAIL_RETURN(CheckIfCameraClosedOrInError())
        Int32 requestId;

        if (repeating) {
            FAIL_RETURN(StopRepeating())
        }

        AutoPtr<ILongParcelable> lastFrameNumberRef = new LongParcelable();
        //try {
        ECode ec = mRemoteDevice->SubmitRequestList(requestList, repeating,
                /*out*/lastFrameNumberRef, &requestId);
        if (DEBUG) {
            Int64 number;
            lastFrameNumberRef->GetNumber(&number);
            Slogger::V(TAG, "last frame number %d" + number);
        }
        //} catch (CameraRuntimeException e) {
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            //throw e.asChecked();
            return ec;
        }
        //} catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // impossible
            *result = -1;
            return NOERROR;
        }
        //}

        if (_callback != NULL) {
            AutoPtr<ICameraDeviceImplCaptureCallbackHolder> holder;
            CCameraDeviceImplCaptureCallbackHolder::New(_callback,
                    requestList, newHandler, repeating, (ICameraDeviceImplCaptureCallbackHolder**)&holder);
            mCaptureCallbackMap->Put(requestId, TO_IINTERFACE(holder));
        }
        else {
            if (DEBUG) {
                Slogger::D(TAG, "Listen for request %d is null", requestId);
            }
        }

        Int64 lastFrameNumber;
        lastFrameNumberRef->GetNumber(&lastFrameNumber);

        if (repeating) {
            if (mRepeatingRequestId != REQUEST_ID_NONE) {
                CheckEarlyTriggerSequenceComplete(mRepeatingRequestId, lastFrameNumber);
            }
            mRepeatingRequestId = requestId;
        }
        else {
            AutoPtr<IInteger64> keyObj = CoreUtils::Convert(lastFrameNumber);
            AutoPtr<IInteger32> valueObj = CoreUtils::Convert(requestId);
            AutoPtr<IPairHelper> helper;
            CPairHelper::AcquireSingleton((IPairHelper**)&helper);
            AutoPtr<IPair> pair;
            helper->Create(TO_IINTERFACE(keyObj), TO_IINTERFACE(valueObj), (IPair**)&pair);
            mFrameNumberRequestPairs->Add(TO_IINTERFACE(pair));
        }

        if (mIdle) {
            Boolean tmp;
            mDeviceHandler->Post(mCallOnActive, &tmp);
        }
        mIdle = FALSE;

        *result = requestId;
        return NOERROR;
    }
    return NOERROR;
}

ECode CameraDeviceImpl::SetRepeatingRequest(
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    AutoPtr<IList> requestList;
    CArrayList::New((IList**)&requestList);
    requestList->Add(TO_IINTERFACE(request));
    return SubmitCaptureRequest(requestList, ccallback, handler, /*streaming*/TRUE, value);
}

ECode CameraDeviceImpl::SetRepeatingBurst(
    /* [in] */ IList* requests,
    /* [in] */ ICameraDeviceImplCaptureCallback* ccallback,
    /* [in] */ IHandler* handler,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (requests == NULL) {
        //throw new IllegalArgumentException("At least one request must be given");
        Slogger::E(TAG, "At least one request must be given");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean _result;
    requests->IsEmpty(&_result);
    if (_result) {
        //throw new IllegalArgumentException("At least one request must be given");
        Slogger::E(TAG, "At least one request must be given");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return SubmitCaptureRequest(requests, ccallback, handler, /*streaming*/TRUE, result);
}

ECode CameraDeviceImpl::StopRepeating()
{
    {    AutoLock syncLock(mInterfaceLock);
        FAIL_RETURN(CheckIfCameraClosedOrInError())
        if (mRepeatingRequestId != REQUEST_ID_NONE) {

            Int32 requestId = mRepeatingRequestId;
            mRepeatingRequestId = REQUEST_ID_NONE;

            // Queue for deletion after in-flight requests finish
            AutoPtr<IInterface> tmp;
            mCaptureCallbackMap->Get(requestId, (IInterface**)&tmp);
            if (tmp != NULL) {
                AutoPtr<IInteger32> obj = CoreUtils::Convert(requestId);
                mRepeatingRequestIdDeletedList->Add(TO_IINTERFACE(obj));
            }

            //try {
            AutoPtr<ILongParcelable> lastFrameNumberRef = new LongParcelable();
            Int32 result;
            mRemoteDevice->CancelRequest(requestId, /*out*/lastFrameNumberRef, &result);
            Int64 lastFrameNumber;
            lastFrameNumberRef->GetNumber(&lastFrameNumber);

            ECode ec = CheckEarlyTriggerSequenceComplete(requestId, lastFrameNumber);

            //} catch (CameraRuntimeException e) {
            if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
                //throw e.asChecked();
                return ec;
            }
            //} catch (RemoteException e) {
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                // impossible
                return NOERROR;
            }
            //}
        }
    }
    return NOERROR;
}

ECode CameraDeviceImpl::WaitUntilIdle()
{
    {    AutoLock syncLock(mInterfaceLock);
        FAIL_RETURN(CheckIfCameraClosedOrInError())

        if (mRepeatingRequestId != REQUEST_ID_NONE) {
            //throw new IllegalStateException("Active repeating request ongoing");
            Slogger::E(TAG, "Active repeating request ongoing");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        //try {
        Int32 result;
        ECode ec = mRemoteDevice->WaitUntilIdle(&result);
        //} catch (CameraRuntimeException e) {
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            return ec;
        }
        //} catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // impossible
            return NOERROR;
        }
        //}
    }
    return NOERROR;
}

ECode CameraDeviceImpl::Flush()
{
    {    AutoLock syncLock(mInterfaceLock);
        FAIL_RETURN(CheckIfCameraClosedOrInError())

        Boolean tmp;
        mDeviceHandler->Post(mCallOnBusy, &tmp);

        // If already idle, just do a busy->idle transition immediately, don't actually
        // flush.
        if (mIdle) {
            return mDeviceHandler->Post(mCallOnIdle, &tmp);
        }
        //try {
        ECode ec = NOERROR;
        AutoPtr<ILongParcelable> lastFrameNumberRef = new LongParcelable();
        Int32 result;
        mRemoteDevice->Flush(/*out*/lastFrameNumberRef, &result);
        if (mRepeatingRequestId != REQUEST_ID_NONE) {
            Int64 lastFrameNumber;
            lastFrameNumberRef->GetNumber(&lastFrameNumber);
            ec = CheckEarlyTriggerSequenceComplete(mRepeatingRequestId, lastFrameNumber);
            mRepeatingRequestId = REQUEST_ID_NONE;
        }
        //} catch (CameraRuntimeException e) {
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            return ec;
        }
        //} catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            // impossible
            return NOERROR;
        }
        //}
    }
    return NOERROR;
}

ECode CameraDeviceImpl::Close()
{
    {    AutoLock syncLock(mInterfaceLock);
        //try {
        ECode ec = NOERROR;
        if (mRemoteDevice != NULL) {
            ec = mRemoteDevice->Disconnect();
        }
        //} catch (CameraRuntimeException e) {
        if (ec == (ECode)E_CAMERA_RUNTIME_EXCEPTION) {
            Slogger::E(TAG, "Exception while closing: %d",ec);
        }
        //} catch (RemoteException e) {
            // impossible
        //}

        // Only want to fire the onClosed callback once;
        // either a normal close where the remote device is valid
        // or a close after a startup error (no remote device but in error state)
        if (mRemoteDevice != NULL || mInError) {
            Boolean tmp;
            mDeviceHandler->Post(mCallOnClosed, &tmp);
        }

        mRemoteDevice = NULL;
        mInError = FALSE;
    }
    return NOERROR;
}


ECode CameraDeviceImpl::CheckAndFireSequenceComplete()
{
    Int64 completedFrameNumber;
    mFrameNumberTracker->GetCompletedFrameNumber(&completedFrameNumber);

    Boolean hasNext;
    AutoPtr<IIterator> reqIter;
    mFrameNumberRequestPairs->GetIterator((IIterator**)&reqIter);
    while (reqIter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        reqIter->GetNext((IInterface**)&obj);

        AutoPtr<IPair> pair = IPair::Probe(obj);
        AutoPtr<IInterface> keyObj;
        pair->GetFirst((IInterface**)&keyObj);
        AutoPtr<IInteger64> keyValueObj = IInteger64::Probe(keyObj);
        Int64 keyValue;
        keyValueObj->GetValue(&keyValue);

        if (keyValue <= completedFrameNumber) {

            // remove request from mCaptureCallbackMap
            AutoPtr<IInterface> valueObj;
            pair->GetSecond((IInterface**)&valueObj);
            AutoPtr<IInteger32> int32Obj = IInteger32::Probe(valueObj);
            Int32 value;
            int32Obj->GetValue(&value);

            Int32 requestId = value;
            AutoPtr<ICameraDeviceImplCaptureCallbackHolder> holder;
            {    AutoLock syncLock(mInterfaceLock);
                if (mRemoteDevice == NULL) {
                    Slogger::W(TAG, "Camera closed while checking sequences");
                    return NOERROR;
                }

                Int32 index;
                mCaptureCallbackMap->IndexOfKey(requestId, &index);
                if (index >= 0) {
                    AutoPtr<IInterface> outface;
                    mCaptureCallbackMap->ValueAt(index, (IInterface**)&outface);
                    holder = ICameraDeviceImplCaptureCallbackHolder::Probe(outface);
                }
                else {
                    holder = NULL;
                }

                if (holder != NULL) {
                    mCaptureCallbackMap->RemoveAt(index);
                    if (DEBUG) {
                        Slogger::V(TAG,
                                "remove holder for requestId %d, "
                                "because lastFrame %d is <= %d",
                                requestId, keyValue,
                                completedFrameNumber);
                    }
                }
            }
            mFrameNumberRequestPairs->Remove(reqIter);

            // Call onCaptureSequenceCompleted
            if (holder != NULL) {
                AutoPtr<ResultDispatchRunnable2> resultDispatch = new ResultDispatchRunnable2(this, requestId,
                        keyValue, holder);
                AutoPtr<IHandler> handler;
                holder->GetHandler((IHandler**)&handler);
                Boolean result;
                handler->Post(resultDispatch, &result);
            }
        }
    }
    return NOERROR;
}

ECode CameraDeviceImpl::CheckHandler(
    /* [in] */ IHandler* handler,
    /* [out] */ IHandler** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IHandler> _handler = handler;
    if (_handler == NULL) {
        AutoPtr<ILooper> looper = Looper::GetMyLooper();
        if (looper == NULL) {
            // throw new IllegalArgumentException(
            //     "No handler given, and current thread has no looper!");
            Slogger::E("CameraDeviceImpl", "No handler given, and current thread has no looper!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        CHandler::New(looper, (IHandler**)&_handler);
    }
    *result = _handler;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CameraDeviceImpl::CheckHandler(
    /* [in] */ IHandler* handler,
    /* [in] */ IInterface* _callback,
    /* [out] */ IHandler** result)
{
    VALIDATE_NOT_NULL(result)

    if (_callback != NULL) {
        return CheckHandler(handler, result);
    }
    *result = handler;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CameraDeviceImpl::CheckIfCameraClosedOrInError()
{
    if (mInError) {
        // throw new CameraAccessException(CameraAccessException.CAMERA_ERROR,
        //         "The camera device has encountered a serious error");
        Slogger::E(TAG, "The camera device has encountered a serious error");
        return E_CAMERA_RUNTIME_EXCEPTION;
    }
    if (mRemoteDevice == NULL) {
        //throw new IllegalStateException("CameraDevice was already closed");
        Slogger::E(TAG, "CameraDevice was already closed");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Boolean CameraDeviceImpl::IsClosed()
{
    return mClosing;
}

AutoPtr<ICameraCharacteristics> CameraDeviceImpl::GetCharacteristics()
{
    return mCharacteristics;
}

} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos