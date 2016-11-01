
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/hardware/camera2/legacy/RequestThreadManager.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyResultMapper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyCameraDevice.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestQueue.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyFocusStateMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyFaceDetectMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestHandlerThread.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyRequest.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/utils/UtilsSizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/utils/CUtilsSizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/legacy/CGLThreadManager.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"
#include "elastos/droid/hardware/camera2/legacy/CCaptureCollector.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestThreadManagerFpsCounter.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/graphics/CSurfaceTexture.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Debug.h"
#include "elastos/droid/os/ConditionVariable.h"
#include "elastos/droid/utility/CSize.h"
#include "elastos/droid/view/CSurface.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Legacy::LegacyCameraDevice;
using Elastos::Droid::Hardware::Camera2::Legacy::CLegacyResultMapper;
using Elastos::Droid::Hardware::Camera2::Legacy::CGLThreadManager;
using Elastos::Droid::Hardware::Camera2::Legacy::CRequestQueue;
using Elastos::Droid::Hardware::Camera2::Legacy::CLegacyFocusStateMapper;
using Elastos::Droid::Hardware::Camera2::Legacy::CLegacyFaceDetectMapper;
using Elastos::Droid::Hardware::Camera2::Legacy::CRequestHandlerThread;
using Elastos::Droid::Hardware::Camera2::Legacy::CLegacyRequest;
using Elastos::Droid::Hardware::Camera2::Legacy::LegacyMetadataMapper;
using Elastos::Droid::Hardware::Camera2::Legacy::CCaptureCollector;
using Elastos::Droid::Hardware::Camera2::Legacy::CRequestThreadManagerFpsCounter;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraDeviceImplCameraDeviceCallbacks;
using Elastos::Droid::Hardware::Camera2::Utils::UtilsSizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Utils::IUtilsSizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Utils::CUtilsSizeAreaComparator;
using Elastos::Droid::Hardware::IHardwareCamera;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::Debug;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ConditionVariable;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Graphics::EIID_IOnFrameAvailableListener;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::CSize;
using Elastos::Droid::View::CSurface;
using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::CoreUtils;
using Elastos::Core::IThread;
using Elastos::Core::IArrayOf;
using Elastos::Core::IComparator;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(RequestThreadManager::RequestThreadManagerFpsCounter,
        Object, IRequestThreadManagerFpsCounter)

const String RequestThreadManager::RequestThreadManagerFpsCounter
        ::TAG("RequestThreadManagerFpsCounter");
const Int64 RequestThreadManager::RequestThreadManagerFpsCounter
        ::NANO_PER_SECOND = 1000000000; //ns

RequestThreadManager::RequestThreadManagerFpsCounter::RequestThreadManagerFpsCounter()
    : mFrameCount(0)
    , mLastTime(0)
    , mLastPrintTime(0)
    , mLastFps(0)
    , mStreamType(NULL)
{
}

ECode RequestThreadManager::RequestThreadManagerFpsCounter::constructor(
    /* [in] */ const String& streamType)
{
    mStreamType = streamType;
    return NOERROR;
}

ECode RequestThreadManager::RequestThreadManagerFpsCounter::CountFrame()
{
    AutoLock syncLock(this);
    mFrameCount++;
    Int64 nextTime = SystemClock::GetElapsedRealtimeNanos();
    if (mLastTime == 0) {
        mLastTime = nextTime;
    }
    if (nextTime > mLastTime + NANO_PER_SECOND) {
        Int64 elapsed = nextTime - mLastTime;
        mLastFps = mFrameCount * (NANO_PER_SECOND / (Double) elapsed);
        mFrameCount = 0;
        mLastTime = nextTime;
    }
    return NOERROR;
}

ECode RequestThreadManager::RequestThreadManagerFpsCounter::CheckFps(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock syncLock(this);
    *result = mLastFps;
    return NOERROR;
}

ECode RequestThreadManager::RequestThreadManagerFpsCounter::StaggeredLog()
{
    AutoLock syncLock(this);
    if (mLastTime > mLastPrintTime + 5 * NANO_PER_SECOND) {
        mLastPrintTime = mLastTime;
        StringBuilder sb;
        sb += "FPS for ";
        sb += mStreamType;
        sb += " stream: ";
        sb += mLastFps;
        Logger::D(TAG, sb.ToString());
    }
    return NOERROR;
}

ECode RequestThreadManager::RequestThreadManagerFpsCounter::CountAndLog()
{
    AutoLock syncLock(this);
    CountFrame();
    StaggeredLog();
    return NOERROR;
}

RequestThreadManager::ConfigureHolder::ConfigureHolder(
    /* [in] */ IConditionVariable* condition,
    /* [in] */ ICollection* surfaces)
    : mCondition(condition)
    , mSurfaces(surfaces)
{
}

CAR_INTERFACE_IMPL(RequestThreadManager::CameraErrorCallback, Object, ICameraErrorCallback)

RequestThreadManager::CameraErrorCallback::CameraErrorCallback(
    /* [in] */ RequestThreadManager* host)
    : mHost(host)
{
}

ECode RequestThreadManager::CameraErrorCallback::OnError(
    /* [in] */ Int32 error,
    /* [in] */ IHardwareCamera* camera)
{
    Logger::E(mHost->TAG, "Received error %d from the Camera1 ErrorCallback", error);
    return mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
}

CAR_INTERFACE_IMPL(RequestThreadManager::CameraPictureCallback, Object, IPictureCallback)

RequestThreadManager::CameraPictureCallback::CameraPictureCallback(
    /* [in] */ RequestThreadManager* host)
    : mHost(host)
{
}

ECode RequestThreadManager::CameraPictureCallback::OnPictureTaken(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IHardwareCamera* camera)
{
    Logger::I(mHost->TAG, String("Received jpeg."));
    AutoPtr<IPair> captureInfo;
    mHost->mCaptureCollector->JpegProduced((IPair**)&captureInfo);
    if (captureInfo == NULL) {
        Logger::E(mHost->TAG, "Dropping jpeg frame.");
        return NOERROR;
    }
    else {
        AutoPtr<IInterface> obj;
        captureInfo->GetFirst((IInterface**)&obj);
        if (obj != NULL) {
            Logger::E(mHost->TAG, "Dropping jpeg frame.");
            return NOERROR;
        }
    }

    AutoPtr<IInterface> obj;
    captureInfo->GetFirst((IInterface**)&obj);
    AutoPtr<IRequestHolder> holder = IRequestHolder::Probe(obj);
    AutoPtr<IInterface> obj2;
    captureInfo->GetSecond((IInterface**)&obj2);
    Int64 timestamp;
    IInteger64::Probe(obj2)->GetValue(&timestamp);

    AutoPtr<ICollection> targets;
    holder->GetHolderTargets((ICollection**)&targets);

    ICollection* jpegSurfaceIds = ICollection::Probe(mHost->mJpegSurfaceIds);
    Boolean hasNext;
    AutoPtr<IIterator> it;
    targets->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> sobj;
        it->GetNext((IInterface**)&sobj);
        ISurface* s = ISurface::Probe(sobj);

        //try {
        Boolean result = LegacyCameraDevice::ContainsSurfaceId(s, jpegSurfaceIds);
        ECode ec = NOERROR;
        if (result) {
            Logger::I(mHost->TAG, String("Producing jpeg buffer..."));

            Int32 size = LegacyCameraDevice::NativeGetJpegFooterSize();
            Int32 totalSize = data->GetLength() + size;
            totalSize = (totalSize + 3) & ~0x3; // round up to nearest octonibble
            ec = LegacyCameraDevice::SetNextTimestamp(s, timestamp);
            if (FAILED(ec)) {
                Logger::W(mHost->TAG, "Surface abandoned, dropping frame. %08x", ec);
                continue;
            }

            if (USE_BLOB_FORMAT_OVERRIDE) {
                // Override to RGBA_8888 format.
                LegacyCameraDevice::SetSurfaceFormat(s,
                        ILegacyMetadataMapper::HAL_PIXEL_FORMAT_RGBA_8888);

                Int32 dimen = (Int32) Elastos::Core::Math::Ceil(Elastos::Core::Math::Sqrt(totalSize));
                dimen = (dimen + 0xf) & ~0xf; // round up to nearest multiple of 16
                LegacyCameraDevice::SetSurfaceDimens(s, dimen, dimen);
                ec = LegacyCameraDevice::ProduceFrame(s, data, dimen, dimen,
                        ICameraMetadataNative::NATIVE_JPEG_FORMAT);
                if (FAILED(ec)) {
                    Logger::W(mHost->TAG, "Surface abandoned, dropping frame. %08x", ec);
                    continue;
                }
            }
            else {
                ec = LegacyCameraDevice::SetSurfaceDimens(s, totalSize, /*height*/1);
                if (FAILED(ec)) {
                    Logger::W(mHost->TAG, "Surface abandoned, dropping frame. %08x", ec);
                    continue;
                }
                ec = LegacyCameraDevice::ProduceFrame(s, data, totalSize, /*height*/1,
                        ICameraMetadataNative::NATIVE_JPEG_FORMAT);
                if (FAILED(ec)) {
                    Logger::W(mHost->TAG, "Surface abandoned, dropping frame. %08x", ec);
                    continue;
                }
            }
        }
    }

    return mHost->mReceivedJpeg->Open();
}

CAR_INTERFACE_IMPL(RequestThreadManager::CameraShutterCallback, Object, IShutterCallback)

RequestThreadManager::CameraShutterCallback::CameraShutterCallback(
    /* [in] */ RequestThreadManager* host)
    : mHost(host)
{
}

ECode RequestThreadManager::CameraShutterCallback::OnShutter()
{
    AutoPtr<IRequestHolder> tmp;
    return mHost->mCaptureCollector->JpegCaptured(SystemClock::GetElapsedRealtimeNanos(),
            (IRequestHolder**)&tmp);
}

CAR_INTERFACE_IMPL(RequestThreadManager::SurfaceTextureOnFrameAvailableListener,
        Object, IOnFrameAvailableListener)

RequestThreadManager::SurfaceTextureOnFrameAvailableListener::SurfaceTextureOnFrameAvailableListener(
    /* [in] */ RequestThreadManager* host)
    : mHost(host)
{
}

ECode RequestThreadManager::SurfaceTextureOnFrameAvailableListener::OnFrameAvailable(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    if (mHost->DEBUG) {
        mHost->mPrevCounter->CountAndLog();
    }
    return mHost->mGLThreadManager->QueueNewFrame();
}

CAR_INTERFACE_IMPL(RequestThreadManager::MyHandlerCallback, Object, IHandlerCallback)

RequestThreadManager::MyHandlerCallback::MyHandlerCallback(
    /* [in] */ RequestThreadManager* host)
    : mCleanup(FALSE)
    , mHost(host)
{
    CLegacyResultMapper::New((ILegacyResultMapper**)&mMapper);
}

ECode RequestThreadManager::MyHandlerCallback::HandleMessage(
   /* [in] */ IMessage* msg,
   /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (mCleanup) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    if (DEBUG) {
        Logger::D(mHost->TAG, "Request thread handling message: %d", what);
    }
    Int64 startTime = 0;
    if (DEBUG) {
        startTime = SystemClock::GetElapsedRealtimeNanos();
    }

    AutoPtr<ITimeUnitHelper> helper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);

    switch (what) {
        case RequestThreadManager::MSG_CONFIGURE_OUTPUTS:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ConfigureHolder> config = (ConfigureHolder*)IObject::Probe(obj);
            Int32 sizes = 0;
            if (config->mSurfaces != NULL) {
                config->mSurfaces->GetSize(&sizes);
            }

            Logger::I(mHost->TAG, "Configure outputs: %d surfaces configured.", sizes);

            //try {
            Boolean success;
            AutoPtr<ITimeUnit> milliSeconds;
            helper->GetMILLISECONDS((ITimeUnit**)&milliSeconds);
            ECode ec = mHost->mCaptureCollector->WaitForEmpty(JPEG_FRAME_TIMEOUT,
                    milliSeconds, &success);
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Interrupted while waiting for requests to complete.");
                mHost->mDeviceState->SetError(
                        ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                break;
            }
            if (!success) {
                Logger::E(mHost->TAG, "Timed out while queueing configure request.");
                mHost->mCaptureCollector->FailAll();
            }

            mHost->ConfigureOutputs(config->mSurfaces);
            config->mCondition->Open();
            if (DEBUG) {
                Int64 totalTime = SystemClock::GetElapsedRealtimeNanos() - startTime;
                Logger::D(mHost->TAG, "Configure took %d ns",totalTime);
            }
            break;
        }
        case MSG_SUBMIT_CAPTURE_REQUEST:
        {
            AutoPtr<IHandler> handler;
            mHost->mRequestThread->GetHandler((IHandler**)&handler);

            // Get the next burst from the request queue.
            AutoPtr<IPair> nextBurst;
            mHost->mRequestQueue->GetNext((IPair**)&nextBurst);

            Logger::I(mHost->TAG, " >> Get next burst %s", TO_CSTR(nextBurst));
            if (nextBurst == NULL) {
                // If there are no further requests queued, wait for any currently executing
                // requests to complete, then switch to idle state.
                //try {
                Boolean success;
                AutoPtr<ITimeUnit> milliSeconds;
                helper->GetMILLISECONDS((ITimeUnit**)&milliSeconds);
                ECode ec = mHost->mCaptureCollector->WaitForEmpty(JPEG_FRAME_TIMEOUT,
                        milliSeconds, &success);
                if (FAILED(ec)) {
                    Logger::E(mHost->TAG, "Interrupted while waiting for requests to complete: %08x", ec);
                    mHost->mDeviceState->SetError(
                            ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                    break;
                }
                if (!success) {
                    Logger::E(mHost->TAG, "Timed out while waiting for prior requests to complete.");
                    mHost->mCaptureCollector->FailAll();
                }

                {
                    AutoLock syncLock(mHost->mIdleLock);
                    // Retry the the request queue.
                    mHost->mRequestQueue->GetNext((IPair**)&nextBurst);

                    // If we still have no queued requests, go idle.
                    if (nextBurst == NULL) {
                        Boolean result;
                        mHost->mDeviceState->SetIdle(&result);
                        break;
                    }
                }
            }

            if (nextBurst != NULL) {
                // Queue another capture if we did not get the last burst.
                Boolean result;
                handler->SendEmptyMessage(MSG_SUBMIT_CAPTURE_REQUEST, &result);
            }

            // Complete each request in the burst
            AutoPtr<IInterface> firstObj;
            nextBurst->GetFirst((IInterface**)&firstObj);
            AutoPtr<IBurstHolder> first = IBurstHolder::Probe(firstObj);
            AutoPtr<IInterface> secondObj;
            nextBurst->GetSecond((IInterface**)&secondObj);
            Int64 value;
            IInteger64::Probe(secondObj)->GetValue(&value);
            AutoPtr<IList> requests;
            first->ProduceRequestHolders(value, (IList**)&requests);

            Int32 size;
            requests->GetSize(&size);
            Logger::I(mHost->TAG, " >> Get next burst %s, requests: %d", TO_CSTR(nextBurst), size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                requests->Get(i, (IInterface**)&obj);
                AutoPtr<IRequestHolder> holder = IRequestHolder::Probe(obj);

                AutoPtr<ICaptureRequest> request;
                holder->GetRequest((ICaptureRequest**)&request);

                Boolean paramsChanged = FALSE;

                // Only update parameters if the request has changed
                AutoPtr<ICaptureRequest> _request;
                if (mHost->mLastRequest != NULL) {
                    mHost->mLastRequest->GetCaptureRequest((ICaptureRequest**)&_request);
                }
                if (mHost->mLastRequest == NULL || _request != request) {

                    // The intermediate buffer is sometimes null, but we always need
                    // the Camera1 API configured preview size
                    AutoPtr<ICameraSize> csize;
                    mHost->mParams->GetPreviewSize((ICameraSize**)&csize);
                    AutoPtr<ISize> previewSize;
                    ParameterUtils::ConvertSize(csize, (ISize**)&previewSize);

                    AutoPtr<ILegacyRequest> legacyRequest;
                    CLegacyRequest::New(mHost->mCharacteristics,
                        request, previewSize, mHost->mParams, (ILegacyRequest**)&legacyRequest); // params are copied

                    // Parameters are mutated as a side-effect
                    LegacyMetadataMapper::ConvertRequestMetadata(/*inout*/legacyRequest);

                    // If the parameters have changed, set them in the Camera1 API.
                    Boolean result;
                    AutoPtr<IParameters> para;
                    legacyRequest->GetParameters((IParameters**)&para);
                    mHost->mParams->Same(para, &result);
                    if (!result) {
                        //try {
                        ECode ec = mHost->mCamera->SetParameters(para);
                        //} catch (RuntimeException e) {
                        // If setting the parameters failed, report a request error to
                        // the camera client, and skip any further work for this request
                        if (FAILED(ec)) {
                            Logger::E(mHost->TAG, "Exception while setting camera parameters: %08x", ec);
                            holder->FailRequest();
                            Boolean result;
                            mHost->mDeviceState->SetCaptureStart(holder, /*timestamp*/0,
                                    ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_REQUEST, &result);
                            continue;
                        }
                        //}
                        paramsChanged = TRUE;
                        mHost->mParams = para;
                    }

                    mHost->mLastRequest = legacyRequest;
                }

                //try {
                Boolean success;
                AutoPtr<ITimeUnit> milliSeconds;
                helper->GetMILLISECONDS((ITimeUnit**)&milliSeconds);
                ECode ec = mHost->mCaptureCollector->QueueRequest(holder,
                    mHost->mLastRequest, JPEG_FRAME_TIMEOUT, milliSeconds, &success);
                FAIL_GOTO(ec, error);

                if (!success) {
                    // Report a request error if we timed out while queuing this.
                    Logger::E(mHost->TAG, "Timed out while queueing capture request.");
                    ec = holder->FailRequest();
                    FAIL_GOTO(ec, error);
                    Boolean result;
                    ec = mHost->mDeviceState->SetCaptureStart(holder, /*timestamp*/0,
                            ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_REQUEST, &result);
                    FAIL_GOTO(ec, error);
                    continue;
                }

                // Starting the preview needs to happen before enabling
                // face detection or auto focus
                Boolean result;
                ec = holder->HasPreviewTargets(&result);
                FAIL_GOTO(ec, error);
                if (result) {
                    mHost->DoPreviewCapture(holder);
                }

                ec = holder->HasJpegTargets(&result);
                FAIL_GOTO(ec, error);
                if (result) {
                    Boolean res;
                    AutoPtr<ITimeUnit> milliSeconds;
                    helper->GetMILLISECONDS((ITimeUnit**)&milliSeconds);
                    while((mHost->mCaptureCollector->WaitForPreviewsEmpty(PREVIEW_FRAME_TIMEOUT,
                            milliSeconds, &res), !res)) {
                        // Fail preview requests until the queue is empty.
                        Logger::E(mHost->TAG, "Timed out while waiting for preview requests to complete.");
                        ec = mHost->mCaptureCollector->FailNextPreview();
                        FAIL_GOTO(ec, error);
                    }
                    ec = mHost->mReceivedJpeg->Close();
                    FAIL_GOTO(ec, error);
                    mHost->DoJpegCapturePrepare(holder);
                }

                /*
                 * Do all the actions that require a preview to have been started
                 */

                // Toggle face detection on/off
                // - do this before AF to give AF a chance to use faces
                ec = mHost->mFaceDetectMapper->ProcessFaceDetectMode(request, /*in*/mHost->mParams);
                FAIL_GOTO(ec, error);

                // Unconditionally process AF triggers, since they're non-idempotent
                // - must be done after setting the most-up-to-date AF mode
                ec = mHost->mFocusStateMapper->ProcessRequestTriggers(request, mHost->mParams);
                FAIL_GOTO(ec, error);

                ec = holder->HasJpegTargets(&result);
                FAIL_GOTO(ec, error);
                if (result) {
                    mHost->DoJpegCapture(holder);
                    Boolean res;
                    ec = mHost->mReceivedJpeg->Block(JPEG_FRAME_TIMEOUT, &res);
                    FAIL_GOTO(ec, error);
                    if (!res) {
                        Logger::E(mHost->TAG, "Hit timeout for jpeg callback!");
                        ec = mHost->mCaptureCollector->FailNextJpeg();
                        FAIL_GOTO(ec, error);
                    }
                }
        error:
                //} catch (IOException e) {
                if (ec == (ECode)E_IO_EXCEPTION) {
                    Logger::E(mHost->TAG, "Received device exception: %08x", ec);
                    mHost->mDeviceState->SetError(
                            ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                    break;
                }
                //} catch (InterruptedException e) {
                else if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    Logger::E(mHost->TAG, "Interrupted during capture: %08x", ec);
                    mHost->mDeviceState->SetError(
                            ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                    break;
                }
                else if (FAILED(ec)) {
                    Logger::E(mHost->TAG, "error during capture: %08x", ec);
                    break;
                }
                //}

                if (paramsChanged) {
                    if (DEBUG) {
                        Logger::D(mHost->TAG, "Params changed -- getting new Parameters from HAL.");
                    }
                    //try {
                    mHost->mParams = NULL;
                    ECode ec = mHost->mCamera->GetParameters((IParameters**)&(mHost->mParams));
                    //} catch (RuntimeException e) {
                    if (FAILED(ec)) {
                        Logger::E(mHost->TAG, "Received device exception: %08x", ec);
                        mHost->mDeviceState->SetError(
                            ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                        break;
                    }
                    //}

                    // Update parameters to the latest that we think the camera is using
                    mHost->mLastRequest->SetParameters(mHost->mParams);
                }

                AutoPtr<IInteger64> timestampMutable = CoreUtils::Convert(/*value*/(Int64)0);
                //try {
                AutoPtr<ITimeUnit> milliSeconds2;
                helper->GetMILLISECONDS((ITimeUnit**)&milliSeconds2);
                Boolean tmp;
                ec = mHost->mCaptureCollector->WaitForRequestCompleted(holder,
                        REQUEST_COMPLETE_TIMEOUT, milliSeconds2,
                        /*out*/timestampMutable, &tmp);

                if (!success) {
                    Logger::E(mHost->TAG, "Timed out while waiting for request to complete.");
                    mHost->mCaptureCollector->FailAll();
                }
                //} catch (InterruptedException e) {
                if (FAILED(ec)) {
                    Logger::E(mHost->TAG, "Interrupted waiting for request completion: %08x", ec);
                    mHost->mDeviceState->SetError(
                            ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
                    break;
                }
                //}

                Int64 value;
                AutoPtr<ICameraMetadataNative> _result;
                timestampMutable->GetValue(&value);
                mMapper->CachedConvertResultMetadata(
                        mHost->mLastRequest, value, (ICameraMetadataNative**)&_result);
                /*
                 * Order matters: The default result mapper is state-less; the
                 * other mappers carry state and may override keys set by the default
                 * mapper with their own values.
                 */

                // Update AF state
                mHost->mFocusStateMapper->MapResultTriggers(_result);
                // Update face-related results
                mHost->mFaceDetectMapper->MapResultFaces(_result, mHost->mLastRequest);

                holder->RequestFailed(&success);
                if (!success) {
                    mHost->mDeviceState->SetCaptureResult(holder, _result,
                            ICameraDeviceState::NO_CAPTURE_ERROR, &success);
                }
            }

            if (DEBUG) {
                Int64 totalTime = SystemClock::GetElapsedRealtimeNanos() - startTime;
                Logger::D(mHost->TAG, "Capture request took %d ns", totalTime);
                mHost->mRequestCounter->CountAndLog();
            }
            break;
        }
        case MSG_CLEANUP:
        {
            mCleanup = TRUE;
            //try {
            Boolean success;
            AutoPtr<ITimeUnit> milliSeconds;
            helper->GetMILLISECONDS((ITimeUnit**)&milliSeconds);
            ECode ec = mHost->mCaptureCollector->WaitForEmpty(JPEG_FRAME_TIMEOUT, milliSeconds, &success);
            if (FAILED(ec)) {
                Logger::E(mHost->TAG, "Interrupted while waiting for requests to complete: %08x", ec);
                mHost->mDeviceState->SetError(ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
            }
            if (!success) {
                Logger::E(mHost->TAG, "Timed out while queueing cleanup request.");
                mHost->mCaptureCollector->FailAll();
            }
            //} catch (InterruptedException e) {
            // Log.e(TAG, "Interrupted while waiting for requests to complete: ", e);
            // mDeviceState.setError(
            //         CameraDeviceImpl.CameraDeviceCallbacks.ERROR_CAMERA_DEVICE);
            //}
            if (mHost->mGLThreadManager != NULL) {
                mHost->mGLThreadManager->Quit();
            }
            if (mHost->mCamera != NULL) {
                mHost->mCamera->ReleaseResources();
            }
            mHost->ResetJpegSurfaceFormats(ICollection::Probe(mHost->mCallbackOutputs));
            break;
        }
        case IRequestHandlerThread::MSG_POKE_IDLE_HANDLER:
            // OK: Ignore message.
            break;
        default:
            // throw new AssertionError("Unhandled message " + msg.what +
            //         " on RequestThread.");
            Logger::E(mHost->TAG, "Unhandled message %d on RequestThread.", what);
            return E_ASSERTION_ERROR;
    }
    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(RequestThreadManager, Object, IRequestThreadManager)

const Boolean RequestThreadManager::DEBUG = TRUE;//Log.isLoggable(LegacyCameraDevice.DEBUG_PROP, Log.DEBUG);
const Boolean RequestThreadManager::VERBOSE = TRUE;//Log.isLoggable(LegacyCameraDevice.DEBUG_PROP, Log.VERBOSE);

const Int32 RequestThreadManager::MSG_CONFIGURE_OUTPUTS;
const Int32 RequestThreadManager::MSG_SUBMIT_CAPTURE_REQUEST;
const Int32 RequestThreadManager::MSG_CLEANUP;

const Int32 RequestThreadManager::MAX_IN_FLIGHT_REQUESTS = 2;

const Int32 RequestThreadManager::PREVIEW_FRAME_TIMEOUT = 1000; // ms
const Int32 RequestThreadManager::JPEG_FRAME_TIMEOUT = 3000; // ms (same as CTS for API2)
const Int32 RequestThreadManager::REQUEST_COMPLETE_TIMEOUT = 3000; // ms (same as JPEG timeout)

const Float RequestThreadManager::ASPECT_RATIO_TOLERANCE = 0.01f;

const Boolean RequestThreadManager::USE_BLOB_FORMAT_OVERRIDE = TRUE;

RequestThreadManager::RequestThreadManager()
    : TAG(NULL)
    , mCameraId(0)
    , mPreviewRunning(FALSE)
{
}

ECode RequestThreadManager::constructor(
    /* [in] */ Int32 cameraId,
    /* [in] */ IHardwareCamera* camera,
    /* [in] */ ICameraCharacteristics* characteristics,
    /* [in] */ ICameraDeviceState* deviceState)
{
    FAIL_RETURN(Preconditions::CheckNotNull(camera, String("camera must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(characteristics, String("characteristics must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(deviceState, String("deviceState must not be null")))

    StringBuilder sb;
    sb += "RequestThread-";
    sb += cameraId;
    TAG = sb.ToString();

    CArrayList::New((IList**)&mPreviewOutputs);
    CArrayList::New((IList**)&mCallbackOutputs);
    CArrayList::New((IList**)&mJpegSurfaceIds);

    CRequestQueue::New(mJpegSurfaceIds, (IRequestQueue**)&mRequestQueue);
    CRequestThreadManagerFpsCounter::New(String("Incoming Preview"),
            (IRequestThreadManagerFpsCounter**)&mPrevCounter);
    CRequestThreadManagerFpsCounter::New(String("Incoming Requests"),
            (IRequestThreadManagerFpsCounter**)&mRequestCounter);

    mErrorCallback = new CameraErrorCallback(this);
    mReceivedJpeg = new ConditionVariable(FALSE);
    mJpegCallback = new CameraPictureCallback(this);
    mJpegShutterCallback = new CameraShutterCallback(this);
    mPreviewCallback = new SurfaceTextureOnFrameAvailableListener(this);
    mRequestHandlerCb = new MyHandlerCallback(this);

    mCamera = camera;
    mCameraId = cameraId;
    mCharacteristics = characteristics;
    mDeviceState = deviceState;

    CLegacyFocusStateMapper::New(mCamera, (ILegacyFocusStateMapper**)&mFocusStateMapper);
    CLegacyFaceDetectMapper::New(mCamera, mCharacteristics, (ILegacyFaceDetectMapper**)&mFaceDetectMapper);
    CCaptureCollector::New(MAX_IN_FLIGHT_REQUESTS, mDeviceState, (ICaptureCollector**)&mCaptureCollector);
    CRequestHandlerThread::New(TAG, mRequestHandlerCb, (IRequestHandlerThread**)&mRequestThread);
    return mCamera->SetErrorCallback(mErrorCallback);
}

void RequestThreadManager::CreateDummySurface()
{
    if (mDummyTexture == NULL || mDummySurface == NULL) {
        mDummyTexture = NULL;
        CSurfaceTexture::New(/*ignored*/0, (ISurfaceTexture**)&mDummyTexture);
        // TODO: use smallest default sizes
        mDummyTexture->SetDefaultBufferSize(640, 480);
        mDummySurface = NULL;
        CSurface::New(mDummyTexture, (ISurface**)&mDummySurface);
    }
    return;
}

void RequestThreadManager::StopPreview()
{
    if (VERBOSE) {
        Logger::V(TAG, "stopPreview - preview running? %d", mPreviewRunning);
    }
    if (mPreviewRunning) {
        mCamera->StopPreview();
        mPreviewRunning = FALSE;
    }
    return;
}

void RequestThreadManager::StartPreview()
{
    if (VERBOSE) {
        Logger::V(TAG, "startPreview - preview running? %d", mPreviewRunning);
    }
    if (!mPreviewRunning) {
        // XX: CameraClient:;startPreview is not getting called after a stop
        mCamera->StartPreview();
        mPreviewRunning = TRUE;
    }
    return;
}

ECode RequestThreadManager::DoJpegCapturePrepare(
    /* [in] */ IRequestHolder* request)
{
    if (DEBUG) {
        Logger::D(TAG, "doJpegCapturePrepare - preview running? %d", mPreviewRunning);
    }

    if (!mPreviewRunning) {
        if (DEBUG) {
            Logger::D(TAG, "doJpegCapture - create fake surface");
        }

        CreateDummySurface();
        FAIL_RETURN(mCamera->SetPreviewTexture(mDummyTexture))
        StartPreview();
    }
    return NOERROR;
}

void RequestThreadManager::DoJpegCapture(
    /* [in] */ IRequestHolder* request)
{
    if (DEBUG) {
        Logger::D(TAG, "doJpegCapturePrepare");
    }

    mCamera->TakePicture(mJpegShutterCallback, /*raw*/NULL, mJpegCallback);
    mPreviewRunning = FALSE;
    return;
}

ECode RequestThreadManager::DoPreviewCapture(
    /* [in] */ IRequestHolder* request)
{
    if (VERBOSE) {
        Logger::V(TAG, "DoPreviewCapture - preview running? %d", mPreviewRunning);
    }

    if (mPreviewRunning) {
        return NOERROR; // Already running
    }

    if (mPreviewTexture == NULL) {
        Logger::E(TAG, "Preview capture called with no preview surfaces configured.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 size, height;
    mIntermediateBufferSize->GetWidth(&size);
    mIntermediateBufferSize->GetHeight(&height);
    mPreviewTexture->SetDefaultBufferSize(size, height);
    FAIL_RETURN(mCamera->SetPreviewTexture(mPreviewTexture))

    StartPreview();
    return NOERROR;
}

ECode RequestThreadManager::ConfigureOutputs(
    /* [in] */ ICollection* outputs)
{
    if (DEBUG) {
        String outputsStr;
        if(outputs == NULL) {
            outputsStr = "null";
        }
        else {
            Int32 size;
            outputs->GetSize(&size);
            StringBuilder sb;
            sb += size;
            sb += " surfaces";
            outputsStr = sb.ToString();
        }
        Logger::D(TAG, "configureOutputs with %s", outputsStr.string());
    }

    StopPreview();
    /*
     * Try to release the previous preview's surface texture earlier if we end up
     * using a different one; this also reduces the likelihood of getting into a deadlock
     * when disconnecting from the old previous texture at a later time.
     */
    //try {
    ECode ec = mCamera->SetPreviewTexture(/*surfaceTexture*/NULL);
    //} catch (IOException e) {
    if (FAILED(ec)) {
        Logger::W(TAG, "Failed to clear prior SurfaceTexture, may cause GL deadlock: %08x", ec);
    }
    //}

    if (mGLThreadManager != NULL) {
        mGLThreadManager->WaitUntilStarted();
        mGLThreadManager->IgnoreNewFrames();
        mGLThreadManager->WaitUntilIdle();
    }
    ResetJpegSurfaceFormats(ICollection::Probe(mCallbackOutputs));
    mPreviewOutputs->Clear();
    mCallbackOutputs->Clear();
    mJpegSurfaceIds->Clear();
    mPreviewTexture = NULL;

    AutoPtr<IInterface> facingObj;
    mCharacteristics->Get(CameraCharacteristics::LENS_FACING, (IInterface**)&facingObj);
    Int32 facing;
    IInteger32::Probe(facingObj)->GetValue(&facing);

    AutoPtr<IInterface> orientationObj;
    mCharacteristics->Get(CameraCharacteristics::SENSOR_ORIENTATION, (IInterface**)&orientationObj);
    Int32 orientation;
    IInteger32::Probe(orientationObj)->GetValue(&orientation);

    Int32 width, height;
    Boolean hasNext;
    if (outputs != NULL) {
        AutoPtr<IIterator> it;
        outputs->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            ISurface* s = ISurface::Probe(obj);

            //try {
            Int32 format;
            ECode ec = LegacyCameraDevice::DetectSurfaceType(s, &format);
            if (FAILED(ec)) {
                Logger::W(TAG, "Surface abandoned, skipping...%08x", ec);
                continue;
            }
            LegacyCameraDevice::SetSurfaceOrientation(s, facing, orientation);
            switch (format) {
                case ICameraMetadataNative::NATIVE_JPEG_FORMAT:
                {
                    if (USE_BLOB_FORMAT_OVERRIDE) {
                        // Override to RGBA_8888 format.
                        LegacyCameraDevice::SetSurfaceFormat(s,
                                ILegacyMetadataMapper::HAL_PIXEL_FORMAT_RGBA_8888);
                    }
                    Int64 id = LegacyCameraDevice::GetSurfaceId(s);
                    AutoPtr<IInteger64> idObj = CoreUtils::Convert(id);
                    mJpegSurfaceIds->Add(idObj);
                    mCallbackOutputs->Add(s);
                    break;
                }
                default:
                    mPreviewOutputs->Add(s);
                    break;
            }
        }
    }

    //try {
    ec = mCamera->GetParameters((IParameters**)&mParams);
    //} catch (RuntimeException e) {
    if (FAILED(ec)) {
        Logger::E(TAG, "Received device exception: %08x", ec);
        mDeviceState->SetError(
            ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE);
        return ec;
    }
    //}

    AutoPtr<IList> supportedFpsRanges;
    mParams->GetSupportedPreviewFpsRange((IList**)&supportedFpsRanges);
    AutoPtr<ArrayOf<Int32> > bestRange = GetPhotoPreviewFpsRange(supportedFpsRanges);
    if (DEBUG) {
        Logger::D(TAG, "doPreviewCapture - Selected range [%d,%d]",
                (*bestRange)[IParameters::PREVIEW_FPS_MIN_INDEX],
                (*bestRange)[IParameters::PREVIEW_FPS_MAX_INDEX]);
    }
    mParams->SetPreviewFpsRange((*bestRange)[IParameters::PREVIEW_FPS_MIN_INDEX],
            (*bestRange)[IParameters::PREVIEW_FPS_MAX_INDEX]);

    Int32 size;
    mPreviewOutputs->GetSize(&size);
    if (size > 0) {
        Int32 oSize;
        outputs->GetSize(&oSize);
        AutoPtr<IList> outputSizes;
        CArrayList::New(oSize, (IList**)&outputSizes);

        AutoPtr<IIterator> it;
        mPreviewOutputs->GetIterator((IIterator**)&it);
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            ISurface* s = ISurface::Probe(obj);

            //try {
            AutoPtr<ISize> sizeObj;
            ec = LegacyCameraDevice::GetSurfaceSize(s, (ISize**)&sizeObj);
            outputSizes->Add(sizeObj);
            //} catch (LegacyExceptionUtils.BufferQueueAbandonedException e) {
            if (FAILED(ec)) {
                Logger::W(TAG, "Surface abandoned, skipping...%08x", ec);
            }
            //}
        }

        AutoPtr<ISize> largestOutput;
        UtilsSizeAreaComparator::FindLargestByArea(outputSizes, (ISize**)&largestOutput);

        // Find largest jpeg dimension - assume to have the same aspect ratio as sensor.
        AutoPtr<ISize> largestJpegDimen;
        ParameterUtils::GetLargestSupportedJpegSizeByArea(mParams, (ISize**)&largestJpegDimen);

        AutoPtr<ArrayOf<ICameraSize*> > cSize;
        mParams->GetSupportedPreviewSizes((ArrayOf<ICameraSize*>**)&cSize);
        AutoPtr<IList> supportedPreviewSizes;
        ParameterUtils::ConvertSizeArrayToList(cSize, (IList**)&supportedPreviewSizes);

        // Use smallest preview dimension with same aspect ratio as sensor that is >= than all
        // of the configured output dimensions.  If none exists, fall back to using the largest
        // supported preview size.
        largestOutput->GetHeight(&height);
        largestOutput->GetWidth(&width);
        Int64 largestOutputArea = height * (Int64)width;
        AutoPtr<ISize> bestPreviewDimen;
        UtilsSizeAreaComparator::FindLargestByArea(supportedPreviewSizes, (ISize**)&bestPreviewDimen);

        Int32 length;
        supportedPreviewSizes->GetSize(&length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IInterface> obj;
            supportedPreviewSizes->Get(0, (IInterface**)&obj);
            AutoPtr<ISize> s = ISize::Probe(obj);

            s->GetWidth(&width);
            s->GetHeight(&height);
            Int64 currArea = width * height;

            bestPreviewDimen->GetWidth(&width);
            bestPreviewDimen->GetHeight(&height);
            Int64 bestArea = width * height;
            if (CheckAspectRatiosMatch(largestJpegDimen, s) && (currArea < bestArea &&
                    currArea >= largestOutputArea)) {
                bestPreviewDimen = s;
            }

        }

        mIntermediateBufferSize = bestPreviewDimen;
        mIntermediateBufferSize->GetWidth(&width);
        mIntermediateBufferSize->GetHeight(&height);
        mParams->SetPreviewSize(width, height);

        if (DEBUG) {
            Logger::D(TAG, "Intermediate buffer selected with dimens: %s", TO_CSTR(bestPreviewDimen));
        }
    }
    else {
        mIntermediateBufferSize = NULL;
        if (DEBUG) {
            Logger::D(TAG, "No Intermediate buffer selected, no preview outputs were configured");
        }
    }

    AutoPtr<ISize> smallestSupportedJpegSize;
    CalculatePictureSize(ICollection::Probe(mCallbackOutputs), mParams, (ISize**)&smallestSupportedJpegSize);
    if (smallestSupportedJpegSize != NULL) {
        /*
         * Set takePicture size to the smallest supported JPEG size large enough
         * to scale/crop out of for the bounding rectangle of the configured JPEG sizes.
         */
        Logger::I(TAG, "configureOutputs - set take picture size to %s", TO_CSTR(smallestSupportedJpegSize));
        Int32 width, height;
        smallestSupportedJpegSize->GetWidth(&width);
        smallestSupportedJpegSize->GetHeight(&height);
        mParams->SetPictureSize(width, height);
    }

    // TODO: Detect and optimize single-output paths here to skip stream teeing.
    if (mGLThreadManager == NULL) {
        CGLThreadManager::New(mCameraId, facing, mDeviceState, (IGLThreadManager**)&mGLThreadManager);
        mGLThreadManager->Start();
    }
    mGLThreadManager->WaitUntilStarted();
    mGLThreadManager->SetConfigurationAndWait(ICollection::Probe(mPreviewOutputs), mCaptureCollector);
    mGLThreadManager->AllowNewFrames();
    mGLThreadManager->GetCurrentSurfaceTexture((ISurfaceTexture**)&mPreviewTexture);
    if (mPreviewTexture != NULL) {
        mPreviewTexture->SetOnFrameAvailableListener(mPreviewCallback);
    }

    return mCamera->SetParameters(mParams);
}

void RequestThreadManager::ResetJpegSurfaceFormats(
    /* [in] */ ICollection* surfaces)
{
    if (!USE_BLOB_FORMAT_OVERRIDE || surfaces == NULL) {
        return;
    }

    Boolean hasNext;
    AutoPtr<IIterator> it;
    surfaces->GetIterator((IIterator**)&it);
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        ISurface* s = ISurface::Probe(obj);
        ECode ec = LegacyCameraDevice::SetSurfaceFormat(s, ILegacyMetadataMapper::HAL_PIXEL_FORMAT_BLOB);
        if (FAILED(ec)) {
            Logger::W(TAG, "Surface %s abandoned, skipping...%08x", TO_CSTR(s), ec);
        }
    }
    return;
}

ECode RequestThreadManager::CalculatePictureSize(
    /* [in] */ ICollection* callbackOutputs,
    /* [in] */ IParameters* params,
    /* [out] */ ISize** size)
{
    VALIDATE_NOT_NULL(size);
    *size = NULL;

    ICollection* jpegSurfaceIds = ICollection::Probe(mJpegSurfaceIds);

    /*
     * Find the largest JPEG size (if any), from the configured outputs:
     * - the api1 picture size should be set to the smallest legal size that's at least as large
     *   as the largest configured JPEG size
     */
    AutoPtr<IList> configuredJpegSizes;
    CArrayList::New((IList**)&configuredJpegSizes);

    AutoPtr<IIterator> it;
    callbackOutputs->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        ISurface* callbackSurface = ISurface::Probe(obj);

        //try {
        Boolean result = LegacyCameraDevice::ContainsSurfaceId(callbackSurface, jpegSurfaceIds);
        if (!result) {
            continue; // Ignore non-JPEG callback formats
        }

        AutoPtr<ISize> jpegSize;
        ECode ec = LegacyCameraDevice::GetSurfaceSize(callbackSurface, (ISize**)&jpegSize);
        if (FAILED(ec)) {
            Logger::W(TAG, "Surface abandoned, skipping...%08x", ec);
            continue;
        }

        configuredJpegSizes->Add(jpegSize);
    }

    Int32 width, height;
    Boolean result;
    configuredJpegSizes->IsEmpty(&result);
    if (!result) {
        /*
         * Find the largest configured JPEG width, and height, independently
         * of the rest.
         *
         * The rest of the JPEG streams can be cropped out of this smallest bounding
         * rectangle.
         */
        Int32 maxConfiguredJpegWidth = -1;
        Int32 maxConfiguredJpegHeight = -1;
        Int32 num;
        configuredJpegSizes->GetSize(&num);
        for (Int32 i = 0; i < num; i++) {
            AutoPtr<IInterface> obj;
            configuredJpegSizes->Get(i, (IInterface**)&obj);
            ISize* jpegSize = ISize::Probe(obj);

            jpegSize->GetWidth(&width);
            jpegSize->GetHeight(&height);
            maxConfiguredJpegWidth = width > maxConfiguredJpegWidth ?
                    width : maxConfiguredJpegWidth;
            maxConfiguredJpegHeight = height > maxConfiguredJpegHeight ?
                    height : maxConfiguredJpegHeight;
        }

        AutoPtr<ISize> smallestBoundJpegSize;
        CSize::New(maxConfiguredJpegWidth, maxConfiguredJpegHeight, (ISize**)&smallestBoundJpegSize);

        AutoPtr<ArrayOf<ICameraSize*> > array;
        params->GetSupportedPictureSizes((ArrayOf<ICameraSize*>**)&array);
        AutoPtr<IList> pSizes;
        Arrays::AsList(array, (IList**)&pSizes);
        AutoPtr<IList> supportedJpegSizes;
        ParameterUtils::ConvertSizeList(pSizes, (IList**)&supportedJpegSizes);

        /*
         * Find the smallest supported JPEG size that can fit the smallest bounding
         * rectangle for the configured JPEG sizes.
         */
        AutoPtr<IList> candidateSupportedJpegSizes;
        CArrayList::New((IList**)&candidateSupportedJpegSizes);

        supportedJpegSizes->GetSize(&num);
        for (Int32 i = 0; i < num; i++) {
            AutoPtr<IInterface> obj;
            supportedJpegSizes->Get(i, (IInterface**)&obj);
            ISize* supportedJpegSize = ISize::Probe(obj);

            supportedJpegSize->GetWidth(&width);
            supportedJpegSize->GetHeight(&height);
            if (width >= maxConfiguredJpegWidth &&
                height >= maxConfiguredJpegHeight) {
                candidateSupportedJpegSizes->Add(obj);
            }
        }

        Boolean res;
        candidateSupportedJpegSizes->IsEmpty(&res);
        if (res) {
            Logger::E(TAG, "Could not find any supported JPEG sizes large enough to fit %s",
                TO_CSTR(smallestBoundJpegSize));
            return E_ASSERTION_ERROR;
        }

        AutoPtr<IComparator> comp;
        CUtilsSizeAreaComparator::New((IComparator**)&comp);
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        AutoPtr<IInterface> obj;
        helper->Min(ICollection::Probe(candidateSupportedJpegSizes), comp, (IInterface**)&obj);
        AutoPtr<ISize> smallestSupportedJpegSize = ISize::Probe(obj);

        smallestSupportedJpegSize->Equals(smallestBoundJpegSize, &res);
        if (!res) {
            Logger::W(TAG, "configureOutputs - Will need to crop picture %s into "
                "smallest bound size %s",
                TO_CSTR(smallestSupportedJpegSize), TO_CSTR(smallestBoundJpegSize));
        }

        *size = smallestSupportedJpegSize;
        REFCOUNT_ADD(*size);
    }

    return NOERROR;
}

Boolean RequestThreadManager::CheckAspectRatiosMatch(
    /* [in] */ ISize* a,
    /* [in] */ ISize* b)
{
    Int32 width, height;
    a->GetWidth(&width);
    a->GetHeight(&height);
    Float aAspect = width / (float) height;

    b->GetWidth(&width);
    b->GetHeight(&height);
    Float bAspect = width / (float) height;

    return Elastos::Core::Math::Abs(aAspect - bAspect) < ASPECT_RATIO_TOLERANCE;
}

AutoPtr<ArrayOf<Int32> > RequestThreadManager::GetPhotoPreviewFpsRange(
    /* [in] */ IList* frameRates)
{
    Int32 size;
    frameRates->GetSize(&size);
    if (size == 0) {
        Logger::E(TAG, "No supported frame rates returned!");
        return NULL;
    }

    Int32 bestMin = 0;
    Int32 bestMax = 0;
    Int32 bestIndex = 0;
    Int32 index = 0;

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        frameRates->Get(i, (IInterface**)&obj);
        AutoPtr<IArrayOf> rate = IArrayOf::Probe(obj);

        AutoPtr<IInterface> obj2;
        rate->Get(IParameters::PREVIEW_FPS_MIN_INDEX, (IInterface**)&obj2);
        Int32 minFps;
        IInteger32::Probe(obj2)->GetValue(&minFps);

        AutoPtr<IInterface> obj3;
        rate->Get(IParameters::PREVIEW_FPS_MAX_INDEX, (IInterface**)&obj3);
        Int32 maxFps;
        IInteger32::Probe(obj3)->GetValue(&maxFps);

        if (maxFps > bestMax || (maxFps == bestMax && minFps > bestMin)) {
            bestMin = minFps;
            bestMax = maxFps;
            bestIndex = index;
        }
        index++;
    }

    AutoPtr<IInterface> obj;
    frameRates->Get(bestIndex, (IInterface**)&obj);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);

    Int32 length;
    array->GetLength(&length);
    AutoPtr<ArrayOf<Int32> > res = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i< length; i++) {
        AutoPtr<IInterface> obj4;
        array->Get(i, (IInterface**)&obj4);
        Int32 value;
        IInteger32::Probe(obj4)->GetValue(&value);

        (*res)[i] = value;
    }
    return res;
}

ECode RequestThreadManager::Start()
{
    return IThread::Probe(mRequestThread)->Start();
}

ECode RequestThreadManager::Flush(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    Logger::I(TAG, "Flushing all pending requests.");
    Int64 lastFrame;
    mRequestQueue->StopRepeating(&lastFrame);
    mCaptureCollector->FailAll();
    *value = lastFrame;
    return NOERROR;
}

ECode RequestThreadManager::Quit()
{
    AutoPtr<IHandler> handler;
    mRequestThread->WaitAndGetHandler((IHandler**)&handler);
    AutoPtr<IMessage> message;
    handler->ObtainMessage(MSG_CLEANUP, (IMessage**)&message);
    Boolean result;
    handler->SendMessageAtFrontOfQueue(message, &result);
    IHandlerThread::Probe(mRequestThread)->QuitSafely(&result);
    //try {
    ECode ec = IThread::Probe(mRequestThread)->Join();
    //} catch (InterruptedException e) {
    if (FAILED(ec)) {
        String name;
        IThread::Probe(mRequestThread)->GetName(&name);
        Int64 id;
        IThread::Probe(mRequestThread)->GetId(&id);
        Logger::E(TAG, "Thread %s (%d) interrupted while quitting.",
                name.string(), id);
    }
    //}
    return NOERROR;
}

ECode RequestThreadManager::SubmitCaptureRequests(
    /* [in] */ IList* requests,
    /* [in] */ Boolean repeating,
    /* [in] */ ILongParcelable* frameNumber,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    AutoPtr<IHandler> handler;
    mRequestThread->WaitAndGetHandler((IHandler**)&handler);
    Int32 ret = 0;
    {
        AutoLock syncLock(mIdleLock);
        mRequestQueue->Submit(requests, repeating, frameNumber, &ret);
        Boolean result;
        handler->SendEmptyMessage(MSG_SUBMIT_CAPTURE_REQUEST, &result);
    }
    *value = ret;
    return NOERROR;
}

ECode RequestThreadManager::CancelRepeating(
    /* [in] */ Int32 requestId,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    return mRequestQueue->StopRepeating(requestId, value);
}

ECode RequestThreadManager::Configure(
    /* [in] */ ICollection* outputs)
{
    AutoPtr<IHandler> handler;
    mRequestThread->WaitAndGetHandler((IHandler**)&handler);
    AutoPtr<IConditionVariable> condition = new ConditionVariable(/*closed*/FALSE);
    AutoPtr<ConfigureHolder> holder = new ConfigureHolder(condition, outputs);
    AutoPtr<IMessage> message;
    handler->ObtainMessage(MSG_CONFIGURE_OUTPUTS, 0, 0, TO_IINTERFACE(holder), (IMessage**)&message);
    Boolean result;
    handler->SendMessage(message, &result);
    return condition->Block();
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
