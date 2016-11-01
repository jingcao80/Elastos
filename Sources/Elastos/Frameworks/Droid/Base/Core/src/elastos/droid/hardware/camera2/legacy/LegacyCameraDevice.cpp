
#include "elastos/droid/hardware/camera2/legacy/LegacyCameraDevice.h"
#include "elastos/droid/hardware/camera2/legacy/CCameraDeviceState.h"
#include "elastos/droid/hardware/camera2/legacy/CRequestThreadManager.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyExceptionUtils.h"
#include "elastos/droid/hardware/camera2/utils/ArrayUtils.h"
#include "elastos/droid/hardware/camera2/impl/CCaptureResultExtras.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/os/ConditionVariable.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/utility/CSize.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <camera/CameraUtils.h>
#include <camera/CameraMetadata.h>
#include <gui/Surface.h>
#include <gui/GLConsumer.h>
#include <gui/IGraphicBufferProducer.h>
#include <ui/GraphicBuffer.h>
#include <system/window.h>
#include <hardware/camera3.h>
#include <system/camera_metadata.h>
#include <stdint.h>
#include <inttypes.h>

using Elastos::Droid::Hardware::Camera2::Legacy::IRequestHolder;
using Elastos::Droid::Hardware::Camera2::Legacy::LegacyExceptionUtils;
using Elastos::Droid::Hardware::Camera2::Legacy::ICameraDeviceState;
using Elastos::Droid::Hardware::Camera2::Legacy::CCameraDeviceState;
using Elastos::Droid::Hardware::Camera2::Legacy::IRequestThreadManager;
using Elastos::Droid::Hardware::Camera2::Legacy::CRequestThreadManager;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfigurationMap;
using Elastos::Droid::Hardware::Camera2::Utils::ICameraBinderDecorator;
using Elastos::Droid::Hardware::Camera2::Utils::ArrayUtils;
using Elastos::Droid::Hardware::Camera2::Impl::CCaptureResultExtras;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Os::ConditionVariable;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Utility::CSize;
using Elastos::Core::IThread;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger64;
using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::IIterator;
using Elastos::Utility::Arrays;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

#define CAMERA_DEVICE_BUFFER_SLACK  3
#define DONT_CARE 0
#define ALIGN(x, mask) ( ((x) + (mask) - 1) & ~((mask) - 1) )

LegacyCameraDevice::MyRunnableOnError::MyRunnableOnError(
    /* [in] */ LegacyCameraDevice* owner,
    /* [in] */ Int32 id,
    /* [in] */ Int32 errorCode,
    /* [in] */ ICaptureResultExtras* extras)
    : mOwner(owner)
    , mId(id)
    , mErrorCode(errorCode)
    , mExtras(extras)
{
}

ECode LegacyCameraDevice::MyRunnableOnError::Run()
{
    if (DEBUG) {
        Logger::D(mOwner->TAG, "doing onError callback for request %d, with error code %d", mId, mErrorCode);
    }
    //try {
    ECode ec = mOwner->mDeviceCallbacks->OnDeviceError(mErrorCode, mExtras);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        // throw new IllegalStateException(
        //         "Received remote exception during onCameraError callback: ", e);
        Logger::E(mOwner->TAG, "Received remote exception during onCameraError callback: %d", ec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //}
    return NOERROR;
}

LegacyCameraDevice::MyRunnableOnIdle::MyRunnableOnIdle(
    /* [in] */ LegacyCameraDevice* owner)
    : mOwner(owner)
{
}

ECode LegacyCameraDevice::MyRunnableOnIdle::Run()
{
    if (DEBUG) {
        Logger::D(mOwner->TAG, "doing onIdle callback.");
    }
    //try {
    ECode ec = mOwner->mDeviceCallbacks->OnDeviceIdle();
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        // throw new IllegalStateException(
        //         "Received remote exception during onCameraIdle callback: ", e);
        Logger::E(mOwner->TAG, "Received remote exception during onCameraIdle callback: %d", ec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //}
    return NOERROR;
}

LegacyCameraDevice::MyRunnableOnCaptureStarted::MyRunnableOnCaptureStarted(
    /* [in] */ LegacyCameraDevice* owner,
    /* [in] */ Int32 id,
    /* [in] */ Int64 timestamp,
    /* [in] */ ICaptureResultExtras* extras)
    : mOwner(owner)
    , mId(id)
    , mTimestamp(timestamp)
    , mExtras(extras)
{
}

ECode LegacyCameraDevice::MyRunnableOnCaptureStarted::Run()
{
    if (DEBUG) {
        Logger::D(mOwner->TAG, "doing onCaptureStarted callback for request %d", mId);
    }
    //try {
    ECode ec = mOwner->mDeviceCallbacks->OnCaptureStarted(mExtras, mTimestamp);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        // throw new IllegalStateException(
        //         "Received remote exception during onCameraError callback: ", e);
        Logger::E(mOwner->TAG, "Received remote exception during onCameraError callback: %d", ec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //}
    return NOERROR;
}

LegacyCameraDevice::MyRunnableonCaptureResult::MyRunnableonCaptureResult(
    /* [in] */ LegacyCameraDevice* owner,
    /* [in] */ Int32 id,
    /* [in] */ ICameraMetadataNative* result,
    /* [in] */ ICaptureResultExtras* extras)
    : mOwner(owner)
    , mId(id)
    , mResult(result)
    , mExtras(extras)
{
}

ECode LegacyCameraDevice::MyRunnableonCaptureResult::Run()
{
    if (DEBUG) {
        Logger::D(mOwner->TAG, "doing onCaptureResult callback for request %d", mId);
    }
    //try {
    ECode ec = mOwner->mDeviceCallbacks->OnResultReceived(mResult, mExtras);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        // throw new IllegalStateException(
        //         "Received remote exception during onCameraError callback: ", e);
        Logger::E(mOwner->TAG, "Received remote exception during onCameraError callback: %d", ec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //}
    return NOERROR;
}

CAR_INTERFACE_IMPL(LegacyCameraDevice::MyListener, Object, ICameraDeviceStateListener)

LegacyCameraDevice::MyListener::MyListener(
    /* [in] */ LegacyCameraDevice* host)
    : mHost(host)
{
}

ECode LegacyCameraDevice::MyListener::OnError(
    /* [in] */ Int32 errorCode,
    /* [in] */ IRequestHolder* holder)
{
    if (DEBUG) {
        Logger::D(mHost->TAG, "onError called, errorCode = %d", errorCode);
    }
    switch (errorCode) {
        /*
         * Only be considered idle if we hit a fatal error
         * and no further requests can be processed.
         */
        case ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DISCONNECTED:
        case ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_SERVICE:
        case ICameraDeviceImplCameraDeviceCallbacks::ERROR_CAMERA_DEVICE:
        {
            mHost->mIdle->Open();

            if (DEBUG) {
                Logger::D(mHost->TAG, "onError - opening idle");
            }
        }
    }

    AutoPtr<ICaptureResultExtras> extras = mHost->GetExtrasFromRequest(holder);
    Int32 id;
    holder->GetRequestId(&id);
    AutoPtr<MyRunnableOnError> myRun = new MyRunnableOnError(mHost, id, errorCode, extras);
    Boolean result;
    return mHost->mResultHandler->Post(myRun, &result);
}

ECode LegacyCameraDevice::MyListener::OnConfiguring()
{
    // Do nothing
    if (DEBUG) {
        Logger::D(mHost->TAG, "doing onConfiguring callback.");
    }
    return NOERROR;
}

ECode LegacyCameraDevice::MyListener::OnIdle()
{
    if (DEBUG) {
        Logger::D(mHost->TAG, "onIdle called");
    }

    mHost->mIdle->Open();

    AutoPtr<MyRunnableOnIdle> myRun = new MyRunnableOnIdle(mHost);
    Boolean result;
    return mHost->mResultHandler->Post(myRun, &result);
}

ECode LegacyCameraDevice::MyListener::OnBusy()
{
    mHost->mIdle->Close();

    if (DEBUG) {
        Logger::D(mHost->TAG, "onBusy called");
    }
    return NOERROR;
}

ECode LegacyCameraDevice::MyListener::OnCaptureStarted(
    /* [in] */ IRequestHolder* holder,
    /* [in] */ Int64 timestamp)
{
    AutoPtr<ICaptureResultExtras> extras = mHost->GetExtrasFromRequest(holder);

    Int32 id;
    holder->GetRequestId(&id);
    AutoPtr<MyRunnableOnCaptureStarted> myRun = new MyRunnableOnCaptureStarted(mHost, id, timestamp, extras);
    Boolean result;
    return mHost->mResultHandler->Post(myRun, &result);
}

ECode LegacyCameraDevice::MyListener::OnCaptureResult(
    /* [in] */ ICameraMetadataNative* result,
    /* [in] */ IRequestHolder* holder)
{
    AutoPtr<ICaptureResultExtras> extras = mHost->GetExtrasFromRequest(holder);

    Int32 id;
    holder->GetRequestId(&id);
    AutoPtr<MyRunnableonCaptureResult> myRun = new MyRunnableonCaptureResult(mHost, id, result, extras);
    Boolean res;
    return mHost->mResultHandler->Post(myRun, &res);
}

CAR_INTERFACE_IMPL_2(LegacyCameraDevice, Object, ILegacyCameraDevice, ICloseable)

const String LegacyCameraDevice::DEBUG_PROP("HAL1ShimLogging");
const Boolean LegacyCameraDevice::DEBUG = FALSE;//Log.isLoggable(LegacyCameraDevice.DEBUG_PROP, Log.DEBUG);
const Int32 LegacyCameraDevice::ILLEGAL_VALUE = -1;

LegacyCameraDevice::LegacyCameraDevice()
    : TAG(NULL)
    , mCameraId(0)
    , mClosed(FALSE)
{
}

LegacyCameraDevice::~LegacyCameraDevice()
{
    //try {
    ECode ec = Close();
    //} catch (CameraRuntimeException e) {
    if (FAILED(ec)) {
        Logger::E(TAG, "Got error while trying to finalize, ignoring: %d", ec);
    }
    //} finally {
    //super.finalize();
    //}
}

ECode LegacyCameraDevice::constructor()
{
    return NOERROR;
}

ECode LegacyCameraDevice::constructor(
    /* [in] */ Int32 cameraId,
    /* [in] */ IHardwareCamera* camera,
    /* [in] */ ICameraCharacteristics* characteristics,
    /* [in] */ IICameraDeviceCallbacks* _callbacks)
{
    mCameraId = cameraId;
    mDeviceCallbacks = _callbacks;
    StringBuilder sb;
    sb += "LegacyCameraDevice-";
    sb += mCameraId;
    sb += "-LE";
    TAG = sb.ToString();

    CCameraDeviceState::New((ICameraDeviceState**)&mDeviceState);
    mIdle = new ConditionVariable(/*open*/TRUE);

    CHandlerThread::New(String("ResultThread"), (IHandlerThread**)&mResultThread);
    CHandlerThread::New(String("CallbackThread"), (IHandlerThread**)&mCallbackHandlerThread);

    mStateListener = new MyListener(this);

    IThread::Probe(mResultThread)->Start();
    AutoPtr<ILooper> lopper;
    mResultThread->GetLooper((ILooper**)&lopper);
    CHandler::New(lopper, (IHandler**)&mResultHandler);
    IThread::Probe(mCallbackHandlerThread)->Start();
    AutoPtr<ILooper> lopper2;
    mCallbackHandlerThread->GetLooper((ILooper**)&lopper2);
    CHandler::New(lopper2, (IHandler**)&mCallbackHandler);
    mDeviceState->SetCameraDeviceCallbacks(mCallbackHandler, mStateListener);
    mStaticCharacteristics = characteristics;
    CRequestThreadManager::New(cameraId, camera, characteristics, mDeviceState,
        (IRequestThreadManager**)&mRequestThreadManager);
    return mRequestThreadManager->Start();
}

AutoPtr<ICaptureResultExtras> LegacyCameraDevice::GetExtrasFromRequest(
    /* [in] */ IRequestHolder* holder)
{
    AutoPtr<ICaptureResultExtras> extras;
    if (holder == NULL) {
        CCaptureResultExtras::New(ILLEGAL_VALUE, ILLEGAL_VALUE, ILLEGAL_VALUE,
            ILLEGAL_VALUE, ILLEGAL_VALUE, ILLEGAL_VALUE, (ICaptureResultExtras**)&extras);
        return extras;
    }
    Int32 requestId;
    holder->GetRequestId(&requestId);
    Int32 subsequeceId;
    holder->GetSubsequeceId(&subsequeceId);
    Int64 frameNumber;
    holder->GetFrameNumber(&frameNumber);
    CCaptureResultExtras::New(requestId, subsequeceId,
       /*afTriggerId*/0, /*precaptureTriggerId*/0, frameNumber,
       /*partialResultCount*/1, (ICaptureResultExtras**)&extras);
    return extras;
}

Boolean LegacyCameraDevice::NeedsConversion(
    /* [in] */ ISurface* s)
{
    Int32 nativeType;
    DetectSurfaceType(s, &nativeType);
    return nativeType == IImageFormat::YUV_420_888 || nativeType == IImageFormat::YV12 ||
            nativeType == IImageFormat::NV21;
}

ECode LegacyCameraDevice::ConfigureOutputs(
    /* [in] */ IList* outputs,
    /* [out] */ Int32* value)
{
    Logger::I(TAG, " >> Start ConfigureOutputs");
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (outputs != NULL) {
        Int32 size;
        outputs->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            outputs->Get(i, (IInterface**)&obj);
            AutoPtr<ISurface> output = ISurface::Probe(obj);

            if (output == NULL) {
                Logger::E(TAG, "configureOutputs - null outputs are not allowed");
                *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
                return NOERROR;
            }

            AutoPtr<IInterface> tmp;
            mStaticCharacteristics->Get(
                CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP, (IInterface**)&tmp);
            AutoPtr<IStreamConfigurationMap> streamConfigurations = IStreamConfigurationMap::Probe(tmp);

            // Validate surface size and format.
            //try {
            AutoPtr<ISize> s;
            ECode ec = GetSurfaceSize(output, (ISize**)&s);
            if (FAILED(ec)) {
                Logger::E(TAG, "Surface bufferqueue is abandoned, cannot configure as output: %d", ec);
                *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
                return NOERROR;
            }
            Int32 surfaceType;
            ec = DetectSurfaceType(output, &surfaceType);
            if (FAILED(ec)) {
                Logger::E(TAG, "Surface bufferqueue is abandoned, cannot configure as output: %d", ec);
                *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
                return NOERROR;
            }
            AutoPtr<ArrayOf<ISize*> > sizes;
            streamConfigurations->GetOutputSizes(surfaceType, (ArrayOf<ISize*>**)&sizes);

            if (sizes == NULL) {
                // WAR: Override default format to IMPLEMENTATION_DEFINED for b/9487482
                if ((surfaceType >= ILegacyMetadataMapper::HAL_PIXEL_FORMAT_RGBA_8888 &&
                    surfaceType <= ILegacyMetadataMapper::HAL_PIXEL_FORMAT_BGRA_8888)) {

                    // YUV_420_888 is always present in LEGACY for all IMPLEMENTATION_DEFINED
                    // output sizes, and is publicly visible in the API (i.e.
                    // {@code #getOutputSizes} works here).
                    streamConfigurations->GetOutputSizes(IImageFormat::YUV_420_888, (ArrayOf<ISize*>**)&sizes);
                }
                else if (surfaceType == ILegacyMetadataMapper::HAL_PIXEL_FORMAT_BLOB) {
                    streamConfigurations->GetOutputSizes(IImageFormat::JPEG, (ArrayOf<ISize*>**)&sizes);
                }
            }

            Boolean contains = FALSE;
            if (sizes) {
                for (Int32 i = 0; i < sizes->GetLength(); i++) {
                    if (Object::Equals((*sizes)[i], s)) {
                        contains = TRUE;
                        break;
                    }
                }
            }

            if (!contains) {
                Int32 width, height;
                s->GetWidth(&width);
                s->GetHeight(&height);
                if (sizes == NULL) {
                    Logger::E(TAG, "Surface with size (w=%d, h=%d) and format 0x%x is not valid, "
                        "format is invalid.", width, height, surfaceType);
                }
                else {
                    Logger::E(TAG, "Surface with size (w=%d, h=%d) and format 0x%x is not valid, "
                        "size not in valid set: %s",
                        width, height, surfaceType, Arrays::ToString(sizes).string());

                }
                *value =  ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
                return NOERROR;
            }
            //} catch (BufferQueueAbandonedException e) {
                // Log.e(TAG, "Surface bufferqueue is abandoned, cannot configure as output: ", e);
                // return ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
            //}
        }
    }

    Boolean success = FALSE;
    Boolean result;
    mDeviceState->SetConfiguring(&result);
    if (result) {
        mRequestThreadManager->Configure(ICollection::Probe(outputs));
        mDeviceState->SetIdle(&success);
    }

    if (success) {
        if (outputs != NULL) {
            CArrayList::New(ICollection::Probe(outputs), (IList**)&mConfiguredSurfaces);
        }
        else {
            mConfiguredSurfaces = NULL;
        }
    }
    else {
        *value = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
        return NOERROR;
    }
    *value = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    Logger::I(TAG, " >> End ConfigureOutputs");
    return NOERROR;
}

ECode LegacyCameraDevice::SubmitRequestList(
    /* [in] */ IList* requestList,
    /* [in] */ Boolean repeating,
    /* [in] */ ILongParcelable* frameNumber,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    if (requestList == NULL) {
        Logger::E(TAG, "submitRequestList - Empty/null requests are not allowed");
        *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
        return NOERROR;
    }

    Boolean res;
    requestList->IsEmpty(&res);
    if (res) {
        Logger::E(TAG, "submitRequestList - Empty/null requests are not allowed");
        *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
        return NOERROR;
    }

    AutoPtr<IList> surfaceIds;
    if (mConfiguredSurfaces == NULL) {
        CArrayList::New((IList**)&surfaceIds);
    }
    else {
        FAIL_RETURN(GetSurfaceIds(ICollection::Probe(mConfiguredSurfaces), (IList**)&surfaceIds))
    }

    // Make sure that there all requests have at least 1 surface; all surfaces are non-null
    Int32 size;
    requestList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        requestList->Get(i, (IInterface**)&obj);
        AutoPtr<ICaptureRequest> request = ICaptureRequest::Probe(obj);

        AutoPtr<ICollection> targets;
        request->GetTargets((ICollection**)&targets);
        Boolean result;
        targets->IsEmpty(&result);
        if (result) {
            Logger::E(TAG, "submitRequestList - Each request must have at least one Surface target");
            *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
            return NOERROR;
        }

        AutoPtr<IIterator> it;
        targets->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> surObj;
            it->GetNext((IInterface**)&surObj);
            ISurface* surface = ISurface::Probe(surObj);

            if (surface == NULL) {
                Logger::E(TAG, "submitRequestList - Null Surface targets are not allowed");
                *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
                return NOERROR;
            }
            else if (mConfiguredSurfaces == NULL) {
                Logger::E(TAG, "submitRequestList - must configure device with valid surfaces before submitting requests");
                *value = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
                return NOERROR;
            }
            else if (!ContainsSurfaceId(surface, ICollection::Probe(surfaceIds))) {
                Logger::E(TAG, "submitRequestList - cannot use a surface that wasn't configured");
                *value = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
                return NOERROR;
            }
        }
    }

    // TODO: further validation of request here
    mIdle->Close();
    return mRequestThreadManager->SubmitCaptureRequests(requestList, repeating, frameNumber, value);
}

ECode LegacyCameraDevice::SubmitRequest(
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Boolean repeating,
    /* [in] */ ILongParcelable* frameNumber,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IArrayList> requestList;
    CArrayList::New((IArrayList**)&requestList);
    requestList->Add(TO_IINTERFACE(request));
    return SubmitRequestList(IList::Probe(requestList), repeating, frameNumber, value);
}

ECode LegacyCameraDevice::CancelRequest(
    /* [in] */ Int32 requestId,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    return mRequestThreadManager->CancelRepeating(requestId, value);
}

ECode LegacyCameraDevice::WaitUntilIdle()
{
    return mIdle->Block();
}

ECode LegacyCameraDevice::Flush(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);

    Int64 lastFrame;
    mRequestThreadManager->Flush(&lastFrame);
    WaitUntilIdle();
    *value = lastFrame;
    return NOERROR;
}

ECode LegacyCameraDevice::IsClosed(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mClosed;
    return NOERROR;
}

ECode LegacyCameraDevice::Close()
{
    mRequestThreadManager->Quit();
    Boolean reuslt;
    mCallbackHandlerThread->QuitSafely(&reuslt);
    mResultThread->QuitSafely(&reuslt);

    //try {
    ECode ec = IThread::Probe(mCallbackHandlerThread)->Join();
    //} catch (InterruptedException e) {
    if (FAILED(ec)) {
        String name;
        IThread::Probe(mCallbackHandlerThread)->GetName(&name);
        Int64 id;
        IThread::Probe(mCallbackHandlerThread)->GetId(&id);
        Logger::E(TAG, "Thread %s (%d) interrupted while quitting.", name.string(), id);
    }
    //}

    //try {
    ec = IThread::Probe(mResultThread)->Join();
    //} catch (InterruptedException e) {
    if (FAILED(ec)) {
        String name;
        IThread::Probe(mResultThread)->GetName(&name);
        Int64 id;
        IThread::Probe(mResultThread)->GetId(&id);
        Logger::E(TAG, "Thread %s (%d) interrupted while quitting.", name.string(), id);
    }
    //}

    mClosed = TRUE;
    return NOERROR;
}

ECode LegacyCameraDevice::GetSurfaceSize(
    /* [in] */ ISurface* surface,
    /* [out] */ ISize** size)
{
    VALIDATE_NOT_NULL(size);
    *size = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(surface))

    AutoPtr<ArrayOf<Int32> > dimens = ArrayOf<Int32>::Alloc(2);
    Int32 tmp;
    FAIL_RETURN(LegacyExceptionUtils::ThrowOnError(NativeDetectSurfaceDimens(surface, dimens), &tmp))

    return CSize::New((*dimens)[0], (*dimens)[1], size);
}

ECode LegacyCameraDevice::DetectSurfaceType(
    /* [in] */ ISurface* surface,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = 0;

    FAIL_RETURN(Preconditions::CheckNotNull(surface))
    return LegacyExceptionUtils::ThrowOnError(NativeDetectSurfaceType(surface), type);
}

ECode LegacyCameraDevice::ConfigureSurface(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 pixelFormat)
{
    FAIL_RETURN(Preconditions::CheckNotNull(surface))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("width must be positive.")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("height must be positive.")))

    Int32 tmp;
    return LegacyExceptionUtils::ThrowOnError(NativeConfigureSurface(surface, width, height,
            pixelFormat), &tmp);
}

ECode LegacyCameraDevice::ProduceFrame(
    /* [in] */ ISurface* surface,
    /* [in] */ ArrayOf<Byte>* pixelBuffer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 pixelFormat)
{
    FAIL_RETURN(Preconditions::CheckNotNull(surface))
    if (pixelBuffer == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("width must be positive.")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("height must be positive.")))

    Int32 frame;
    NativeProduceFrame(surface, pixelBuffer, width, height, pixelFormat, &frame);
    Int32 tmp;
    return LegacyExceptionUtils::ThrowOnError(frame, &tmp);
}

ECode LegacyCameraDevice::SetSurfaceFormat(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 pixelFormat)
{
    FAIL_RETURN(Preconditions::CheckNotNull(surface))

    Int32 tmp;
    return LegacyExceptionUtils::ThrowOnError(NativeSetSurfaceFormat(surface, pixelFormat), &tmp);
}

ECode LegacyCameraDevice::SetSurfaceDimens(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(Preconditions::CheckNotNull(surface))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(width, String("width must be positive.")))
    FAIL_RETURN(Preconditions::CheckArgumentPositive(height, String("height must be positive.")))

    Int32 tmp;
    return LegacyExceptionUtils::ThrowOnError(NativeSetSurfaceDimens(surface, width, height), &tmp);
}

Int64 LegacyCameraDevice::GetSurfaceId(
    /* [in] */ ISurface* surface)
{
    FAIL_RETURN(Preconditions::CheckNotNull(surface))

    return NativeGetSurfaceId(surface);
}

ECode LegacyCameraDevice::GetSurfaceIds(
    /* [in] */ ICollection* surfaces,
    /* [out] */ IList** ids)
{
    VALIDATE_NOT_NULL(ids);
    *ids = NULL;

    if (surfaces == NULL) {
        Logger::E("LegacyCameraDevice", "Null argument surfaces");
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IList> surfaceIds;
    CArrayList::New((IList**)&surfaceIds);

    AutoPtr<ArrayOf<IInterface*> > array;
    surfaces->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<ISurface> s = ISurface::Probe((*array)[i]);

        Int64 id = GetSurfaceId(s);
        if (id == 0) {
            Logger::E("LegacyCameraDevice", "Configured surface had null native GraphicBufferProducer pointer!");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
        surfaceIds->Add(TO_IINTERFACE(obj));
    }

    *ids = surfaceIds;
    REFCOUNT_ADD(*ids);
    return NOERROR;
}

Boolean LegacyCameraDevice::ContainsSurfaceId(
    /* [in] */ ISurface* s,
    /* [in] */ ICollection* ids)
{
    Int64 id = GetSurfaceId(s);
    AutoPtr<IInteger64> obj = CoreUtils::Convert(id);
    Boolean result;
    ids->Contains(TO_IINTERFACE(obj), &result);
    return result;
}

ECode LegacyCameraDevice::SetSurfaceOrientation(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 facing,
    /* [in] */ Int32 sensorOrientation)
{
    FAIL_RETURN(Preconditions::CheckNotNull(surface))

    Int32 tmp;
    return LegacyExceptionUtils::ThrowOnError(NativeSetSurfaceOrientation(surface, facing,
            sensorOrientation), &tmp);
}

ECode LegacyCameraDevice::GetTextureSize(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [out] */ ISize** size)
{
    VALIDATE_NOT_NULL(size);
    *size = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(surfaceTexture))

    AutoPtr<ArrayOf<Int32> > dimens = ArrayOf<Int32>::Alloc(2);
    Int32 tmp;
    LegacyExceptionUtils::ThrowOnError(NativeDetectTextureDimens(surfaceTexture,
            /*out*/dimens), &tmp);

    return CSize::New((*dimens)[0], (*dimens)[1], size);
}

ECode LegacyCameraDevice::SetNextTimestamp(
    /* [in] */ ISurface* surface,
    /* [in] */ Int64 timestamp)
{
    FAIL_RETURN(Preconditions::CheckNotNull(surface))

    Int32 tmp;
    return LegacyExceptionUtils::ThrowOnError(NativeSetNextTimestamp(surface, timestamp), &tmp);
}

static android::sp<ANativeWindow> getNativeWindow(
    /* [in] */ ISurface* surface)
{
    android::sp<ANativeWindow> anw;
    if (surface) {
        Int64 nativeSurf;
        surface->GetNativeSurface(&nativeSurf);
        anw = reinterpret_cast<android::Surface*>(nativeSurf);
    }
    else {
        Logger::E("LegacyCameraDevice", "jniThrowNullPointerException: surface");
        return NULL;
    }
    if (anw == NULL) {
        Logger::E("LegacyCameraDevice", "java/lang/IllegalArgumentException"
                " Surface had no valid native window.");
        return NULL;
    }
    return anw;
}

Int32 LegacyCameraDevice::NativeDetectSurfaceType(
    /* [in] */ ISurface* surface)
{
    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native window from surface.", __FUNCTION__);
        return android::BAD_VALUE;
    }
    int32_t fmt = 0;
    android::status_t err = anw->query(anw.get(), NATIVE_WINDOW_FORMAT, &fmt);
    if(err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while querying surface pixel format %s (%d).",
            __FUNCTION__, strerror(-err), err);
        return err;
    }

    Logger::V("LegacyCameraDevice", "nativeDetectSurfaceType: %d, %x", fmt, fmt);
    return fmt;
}

Int32 LegacyCameraDevice::NativeDetectSurfaceDimens(
    /* [in] */ ISurface* surface,
    /* [in] */ ArrayOf<Int32>* dimens)
{
    Logger::V("LegacyCameraDevice", "nativeGetSurfaceDimens");

    if (dimens == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Null dimens argument passed"
            " to nativeDetectSurfaceDimens", __FUNCTION__);
        return android::BAD_VALUE;
    }

    if (dimens->GetLength() < 2) {
        Logger::E("LegacyCameraDevice", "%s: Invalid length of dimens "
            "argument in nativeDetectSurfaceDimens", __FUNCTION__);
        return android::BAD_VALUE;
    }

    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native "
            "window from surface.", __FUNCTION__);
        return android::BAD_VALUE;
    }
    int32_t dimenBuf[2];
    android::status_t err = anw->query(anw.get(), NATIVE_WINDOW_WIDTH, dimenBuf);
    if(err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while querying "
            "surface width %s (%d).", __FUNCTION__, strerror(-err),
            err);
        return err;
    }
    err = anw->query(anw.get(), NATIVE_WINDOW_HEIGHT, dimenBuf + 1);
    if(err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while querying surface "
            "height %s (%d).", __FUNCTION__, strerror(-err),
            err);
        return err;
    }
    //Arrays::CopyOfRange(dimens, /*start*/0, /*length*/ARRAY_SIZE(dimenBuf), dimenBuf);
    (*dimens)[0] = dimenBuf[0];
    (*dimens)[1] = dimenBuf[1];
    return android::NO_ERROR;
}

static android::status_t configureSurface(
    /* [in] */ const android::sp<ANativeWindow>& anw,
    /* [in] */ int32_t width,
    /* [in] */ int32_t height,
    /* [in] */ int32_t pixelFmt,
    /* [in] */ int32_t maxBufferSlack)
{
    android::status_t err = android::NO_ERROR;
    assert(0);
    //err = native_window_set_buffers_dimensions(anw.get(), width, height);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Failed to set native window"
            " buffer dimensions, error %s (%d).", __FUNCTION__,
            strerror(-err), err);
        return err;
    }

    assert(0);
    //err = native_window_set_buffers_format(anw.get(), pixelFmt);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Failed to set native window"
                " buffer format, error %s (%d).", __FUNCTION__,
                strerror(-err), err);
        return err;
    }

    assert(0);
    //err = native_window_set_usage(anw.get(), GRALLOC_USAGE_SW_WRITE_OFTEN);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Failed to set native window"
            " usage flag, error %s (%d).", __FUNCTION__,
            strerror(-err), err);
        return err;
    }

    int minUndequeuedBuffers;
    err = anw.get()->query(anw.get(),
            NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS,
            &minUndequeuedBuffers);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Failed to get native window"
            " min undequeued buffers, error %s (%d).",
            __FUNCTION__, strerror(-err), err);
        return err;
    }

    Logger::V("%s: Setting buffer count to %d, size to (%dx%d), fmt (0x%x)", __FUNCTION__,
            maxBufferSlack + 1 + minUndequeuedBuffers,
            width, height, pixelFmt);
    assert(0);
    //err = native_window_set_buffer_count(anw.get(), maxBufferSlack + 1 + minUndequeuedBuffers);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Failed to set native window"
            " buffer count, error %s (%d).", __FUNCTION__,
            strerror(-err), err);
        return err;
    }
    return android::NO_ERROR;
}

Int32 LegacyCameraDevice::NativeConfigureSurface(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 pixelFormat)
{
    Logger::V("LegacyCameraDevice", "nativeConfigureSurface");
    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native "
                "window from surface.", __FUNCTION__);
        return android::BAD_VALUE;
    }
    android::status_t err = configureSurface(anw, width, height, pixelFormat,
            CAMERA_DEVICE_BUFFER_SLACK);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while configuring "
                "surface %s (%d).", __FUNCTION__, strerror(-err), err);
        return err;
    }
    return android::NO_ERROR;
}

/**
 * Convert from RGB 888 to Y'CbCr using the conversion specified in ITU-R BT.601 for
 * digital RGB with K_b = 0.114, and K_r = 0.299.
 */
static void rgbToYuv420(
    /* [in] */ uint8_t* rgbBuf,
    /* [in] */ size_t width,
    /* [in] */ size_t height,
    /* [in] */ uint8_t* yPlane,
    /* [in] */ uint8_t* uPlane,
    /* [in] */ uint8_t* vPlane,
    /* [in] */ size_t chromaStep,
    /* [in] */ size_t yStride,
    /* [in] */ size_t chromaStride)
{
    uint8_t R, G, B;
    size_t index = 0;

    size_t cStrideDiff = chromaStride - width;

    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            R = rgbBuf[index++];
            G = rgbBuf[index++];
            B = rgbBuf[index++];
            *(yPlane + i) = ((66 * R + 129 * G +  25 * B + 128) >> 8) +  16;

            if (j % 2 == 0 && i % 2 == 0){
                *uPlane = (( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
                *vPlane = (( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;
                uPlane += chromaStep;
                vPlane += chromaStep;
            }
            // Skip alpha
            index++;
        }
        yPlane += yStride;
        if (j % 2 == 0) {
            uPlane += cStrideDiff;
            vPlane += cStrideDiff;
        }
    }
}

static void rgbToYuv420(
    /* [in] */ uint8_t* rgbBuf,
    /* [in] */ size_t width,
    /* [in] */ size_t height,
    /* [in] */ android_ycbcr* ycbcr)
{
    size_t cStep = ycbcr->chroma_step;
    size_t cStride = ycbcr->cstride;
    size_t yStride = ycbcr->ystride;
    rgbToYuv420(rgbBuf, width, height, reinterpret_cast<uint8_t*>(ycbcr->y),
            reinterpret_cast<uint8_t*>(ycbcr->cb), reinterpret_cast<uint8_t*>(ycbcr->cr),
            cStep, yStride, cStride);
}

/**
 * Produce a frame in the given surface.
 *
 * Args:
 *    anw - a surface to produce a frame in.
 *    pixelBuffer - image buffer to generate a frame from.
 *    width - width of the pixelBuffer in pixels.
 *    height - height of the pixelBuffer in pixels.
 *    pixelFmt - format of the pixelBuffer, one of:
 *               HAL_PIXEL_FORMAT_YCrCb_420_SP,
 *               HAL_PIXEL_FORMAT_YCbCr_420_888,
 *               HAL_PIXEL_FORMAT_BLOB
 *    bufSize - the size of the pixelBuffer in bytes.
 */
static android::status_t produceFrame(
    /* [in] */ const android::sp<ANativeWindow>& anw,
    /* [in] */ uint8_t* pixelBuffer,
    /* [in] */ int32_t bufWidth, // Width of the pixelBuffer
    /* [in] */ int32_t bufHeight, // Height of the pixelBuffer
    /* [in] */ int32_t pixelFmt, // Format of the pixelBuffer
    /* [in] */ int32_t bufSize)
{
    assert(0);
    //ATRACE_CALL();
    android::status_t err = android::NO_ERROR;
    ANativeWindowBuffer* anb;
    ALOGV("%s: Dequeue buffer from %p %dx%d (fmt=%x, size=%x)",
            __FUNCTION__, anw.get(), bufWidth, bufHeight, pixelFmt, bufSize);

    if (anw == 0) {
        Logger::E("LegacyCameraDevice", "%s: anw must not be NULL", __FUNCTION__);
        return android::BAD_VALUE;
    }
    else if (pixelBuffer == NULL) {
        Logger::E("LegacyCameraDevice", "%s: pixelBuffer must not be NULL", __FUNCTION__);
        return android::BAD_VALUE;
    }
    else if (bufWidth < 0) {
        Logger::E("LegacyCameraDevice", "%s: width must be non-negative", __FUNCTION__);
        return android::BAD_VALUE;
    }
    else if (bufHeight < 0) {
        Logger::E("LegacyCameraDevice", "%s: height must be non-negative", __FUNCTION__);
        return android::BAD_VALUE;
    }
    else if (bufSize < 0) {
        Logger::E("LegacyCameraDevice", "%s: bufSize must be non-negative", __FUNCTION__);
        return android::BAD_VALUE;
    }

    size_t width = static_cast<size_t>(bufWidth);
    size_t height = static_cast<size_t>(bufHeight);
    size_t bufferLength = static_cast<size_t>(bufSize);

    // TODO: Switch to using Surface::lock and Surface::unlockAndPost
    err = native_window_dequeue_buffer_and_wait(anw.get(), &anb);
    if (err != android::NO_ERROR) return err;

    android::sp<android::GraphicBuffer> buf(new android::GraphicBuffer(anb, /*keepOwnership*/false));
    uint32_t grallocBufWidth = buf->getWidth();
    uint32_t grallocBufHeight = buf->getHeight();
    uint32_t grallocBufStride = buf->getStride();
    if (grallocBufWidth != width || grallocBufHeight != height) {
        Logger::E("LegacyCameraDevice", "%s: Received gralloc buffer with bad "
                "dimensions %" PRIu32 "x%" PRIu32
                ", expecting dimensions %zu x %zu",  __FUNCTION__, grallocBufWidth,
                grallocBufHeight, width, height);
        return android::BAD_VALUE;
    }

    int32_t bufFmt = 0;
    err = anw->query(anw.get(), NATIVE_WINDOW_FORMAT, &bufFmt);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while querying surface pixel "
                "format %s (%d).", __FUNCTION__, strerror(-err), err);
        return err;
    }

    uint64_t tmpSize = (pixelFmt == HAL_PIXEL_FORMAT_BLOB) ? grallocBufWidth :
            4 * grallocBufHeight * grallocBufWidth;
    if (bufFmt != pixelFmt) {
        if (bufFmt == HAL_PIXEL_FORMAT_RGBA_8888 && pixelFmt == HAL_PIXEL_FORMAT_BLOB) {
            Logger::V("LegacyCameraDevice", "%s: Using BLOB to RGBA format override.", __FUNCTION__);
            tmpSize = 4 * (grallocBufWidth + grallocBufStride * (grallocBufHeight - 1));
        }
        else {
            Logger::W("LegacyCameraDevice", "%s: Format mismatch in produceFrame: expecting format %#" PRIx32
                    ", but received buffer with format %#" PRIx32, __FUNCTION__, pixelFmt, bufFmt);
        }
    }

    if (tmpSize > SIZE_MAX) {
        Logger::E("LegacyCameraDevice", "%s: Overflow calculating size, buffer "
                "with dimens %zu x %zu is absurdly large...",
                __FUNCTION__, width, height);
        return android::BAD_VALUE;
    }

    size_t totalSizeBytes = tmpSize;

    switch(pixelFmt) {
        case HAL_PIXEL_FORMAT_YCrCb_420_SP: {
            if (bufferLength < totalSizeBytes) {
                Logger::E("LegacyCameraDevice", "%s: PixelBuffer size %zu "
                        "too small for given dimensions",
                        __FUNCTION__, bufferLength);
                return android::BAD_VALUE;
            }
            uint8_t* img = NULL;
            Logger::V("LegacyCameraDevice", "%s: Lock buffer from %p for "
                    "write", __FUNCTION__, anw.get());
            err = buf->lock(GRALLOC_USAGE_SW_WRITE_OFTEN, (void**)(&img));
            if (err != android::NO_ERROR) return err;

            uint8_t* yPlane = img;
            uint8_t* uPlane = img + height * width;
            uint8_t* vPlane = uPlane + 1;
            size_t chromaStep = 2;
            size_t yStride = width;
            size_t chromaStride = width;

            rgbToYuv420(pixelBuffer, width, height, yPlane,
                    uPlane, vPlane, chromaStep, yStride, chromaStride);
            break;
        }
        case HAL_PIXEL_FORMAT_YV12: {
            if (bufferLength < totalSizeBytes) {
                Logger::E("LegacyCameraDevice", "%s: PixelBuffer size %zu too "
                        "small for given dimensions",
                        __FUNCTION__, bufferLength);
                return android::BAD_VALUE;
            }

            if ((width & 1) || (height & 1)) {
                Logger::E("LegacyCameraDevice", "%s: Dimens %zu x %zu are "
                        "not divisible by 2.", __FUNCTION__, width, height);
                return android::BAD_VALUE;
            }

            uint8_t* img = NULL;
            Logger::V("LegacyCameraDevice", "%s: Lock buffer from %p for "
                    "write", __FUNCTION__, anw.get());
            err = buf->lock(GRALLOC_USAGE_SW_WRITE_OFTEN, (void**)(&img));
            if (err != android::NO_ERROR) {
                Logger::E("LegacyCameraDevice", "%s: Error %s (%d) while "
                        "locking gralloc buffer for write.", __FUNCTION__,
                        strerror(-err), err);
                return err;
            }

            uint32_t stride = buf->getStride();
            LOG_ALWAYS_FATAL_IF(stride % 16, "Stride is not 16 pixel aligned %d", stride);

            uint32_t cStride = ALIGN(stride / 2, 16);
            size_t chromaStep = 1;

            uint8_t* yPlane = img;
            uint8_t* crPlane = img + static_cast<uint32_t>(height) * stride;
            uint8_t* cbPlane = crPlane + cStride * static_cast<uint32_t>(height) / 2;

            rgbToYuv420(pixelBuffer, width, height, yPlane,
                    crPlane, cbPlane, chromaStep, stride, cStride);
            break;
        }
        case HAL_PIXEL_FORMAT_YCbCr_420_888: {
            // Software writes with YCbCr_420_888 format are unsupported
            // by the gralloc module for now
            if (bufferLength < totalSizeBytes) {
                Logger::E("LegacyCameraDevice", "%s: PixelBuffer size %zu too "
                        "small for given dimensions",
                        __FUNCTION__, bufferLength);
                return android::BAD_VALUE;
            }
            android_ycbcr ycbcr = android_ycbcr();
            Logger::V("LegacyCameraDevice", "%s: Lock buffer from %p for "
                    "write", __FUNCTION__, anw.get());

            err = buf->lockYCbCr(GRALLOC_USAGE_SW_WRITE_OFTEN, &ycbcr);
            if (err != android::NO_ERROR) {
                Logger::E("LegacyCameraDevice", "%s: Failed to lock ycbcr "
                        "buffer, error %s (%d).", __FUNCTION__,
                        strerror(-err), err);
                return err;
            }
            rgbToYuv420(pixelBuffer, width, height, &ycbcr);
            break;
        }
        case HAL_PIXEL_FORMAT_BLOB: {
            int8_t* img = NULL;
            struct camera3_jpeg_blob footer = {
                jpeg_blob_id: CAMERA3_JPEG_BLOB_ID,
                jpeg_size: (uint32_t)bufferLength
            };

            size_t totalJpegSize = bufferLength + sizeof(footer);
            totalJpegSize = (totalJpegSize + 3) & ~0x3; // round up to nearest octonibble

            if (totalJpegSize > totalSizeBytes) {
                Logger::E("LegacyCameraDevice", "%s: Pixel buffer needs size %zu, "
                        "cannot fit in gralloc buffer of size %zu",
                        __FUNCTION__, totalJpegSize, totalSizeBytes);
                return android::BAD_VALUE;
            }

            err = buf->lock(GRALLOC_USAGE_SW_WRITE_OFTEN, (void**)(&img));
            if (err != android::NO_ERROR) {
                Logger::E("LegacyCameraDevice", "%s: Failed to lock buffer, "
                        "error %s (%d).", __FUNCTION__, strerror(-err),
                        err);
                return err;
            }

            memcpy(img, pixelBuffer, bufferLength);
            memcpy(img + totalSizeBytes - sizeof(footer), &footer, sizeof(footer));
            break;
        }
        default: {
            Logger::E("LegacyCameraDevice", "%s: Invalid pixel format in "
                    "produceFrame: %x", __FUNCTION__, pixelFmt);
            return android::BAD_VALUE;
        }
    }

    ALOGV("%s: Unlock buffer from %p", __FUNCTION__, anw.get());
    err = buf->unlock();
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Failed to unlock buffer, "
                "error %s (%d).", __FUNCTION__, strerror(-err), err);
        return err;
    }

    ALOGV("%s: Queue buffer to %p", __FUNCTION__, anw.get());
    err = anw->queueBuffer(anw.get(), buf->getNativeBuffer(), /*fenceFd*/-1);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Failed to queue buffer, "
                "error %s (%d).", __FUNCTION__, strerror(-err), err);
        return err;
    }
    return android::NO_ERROR;
}

ECode LegacyCameraDevice::NativeProduceFrame(
    /* [in] */ ISurface* surface,
    /* [in] */ ArrayOf<Byte>* pixelBuffer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 pixelFormat,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Logger::V("LegacyCameraDevice", "nativeProduceFrame");
    android::sp<ANativeWindow> anw;

    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native window "
                "from surface.", __FUNCTION__);
        *result = android::BAD_VALUE;
        return NOERROR;
    }

    if (pixelBuffer == NULL) {
        //jniThrowNullPointerException(env, "pixelBuffer");
        Logger::E("LegacyCameraDevice", "jniThrowNullPointerException: pixelBuffer");
        *result = DONT_CARE;
        return E_NULL_POINTER_EXCEPTION;
    }

    int32_t bufSize = static_cast<int32_t>(pixelBuffer->GetLength());
    Byte* pixels = pixelBuffer->GetPayload();

    if (pixels == NULL) {
        Logger::E("LegacyCameraDevice", "jniThrowNullPointerException: pixels");
        *result = DONT_CARE;
        return E_NULL_POINTER_EXCEPTION;
    }

    android::status_t err = produceFrame(anw, reinterpret_cast<uint8_t*>(pixels), width, height,
            pixelFormat, bufSize);

    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while producing "
                "frame %s (%d).", __FUNCTION__, strerror(-err), err);
        *result = err;
        return NOERROR;
    }
    *result = android::NO_ERROR;
    return NOERROR;
}

Int32 LegacyCameraDevice::NativeSetSurfaceFormat(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 pixelFormat)
{
    Logger::V("LegacyCameraDevice", "nativeSetSurfaceType: %d, %x", pixelFormat, pixelFormat);
    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native window "
                "from surface.", __FUNCTION__);
        return android::BAD_VALUE;
    }
    android::status_t err = native_window_set_buffers_format(anw.get(), pixelFormat);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while setting surface "
                "format %s (%d).", __FUNCTION__, strerror(-err), err);
        return err;
    }
    return android::NO_ERROR;
}

Int32 LegacyCameraDevice::NativeSetSurfaceDimens(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Logger::V("LegacyCameraDevice", "nativeSetSurfaceDimens:(%d, %d)", width, height);
    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native window "
            "from surface.", __FUNCTION__);
        return android::BAD_VALUE;
    }
    android::status_t err = native_window_set_buffers_dimensions(anw.get(), width, height);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while setting surface "
            "dimens %s (%d).", __FUNCTION__, strerror(-err), err);
        return err;
    }

    // WAR - Set user dimensions also to avoid incorrect scaling after TextureView orientation
    // change.
    err = native_window_set_buffers_user_dimensions(anw.get(), width, height);
    if (err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while setting surface user "
            "dimens %s (%d).", __FUNCTION__, strerror(-err), err);
        return err;
    }
    return android::NO_ERROR;
}

static android::sp<android::Surface> getSurface(
    /* [in] */ ISurface* surface)
{
    android::sp<android::Surface> s;
    if (surface) {
        Int64 nativeSurf;
        surface->GetNativeSurface(&nativeSurf);
        s = reinterpret_cast<android::Surface*>(nativeSurf);
    }
    else {
        Logger::E("LegacyCameraDevice", "jniThrowNullPointerException: surface is null");
        return NULL;
    }
    if (s == NULL) {
        Logger::E("LegacyCameraDevice", "java/lang/IllegalArgumentException"
                " Surface had no valid native Surface.");
        return NULL;
    }

    return s;
}

Int64 LegacyCameraDevice::NativeGetSurfaceId(
    /* [in] */ ISurface* surface)
{
    android::sp<android::Surface> s;
    if ((s = getSurface(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native Surface from "
                "surface.", __FUNCTION__);
        return 0;
    }
    android::sp<android::IGraphicBufferProducer> gbp = s->getIGraphicBufferProducer();
    if (gbp == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve IGraphicBufferProducer "
                "from surface.", __FUNCTION__);
        return 0;
    }
    android::sp<android::IBinder> b = gbp->asBinder();
    if (b == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve IBinder from "
                "surface.", __FUNCTION__);
        return 0;
    }
    /*
     * FIXME: Use better unique ID for surfaces than native IBinder pointer.  Fix also in the camera
     * service (CameraDeviceClient.h).
     */
    return reinterpret_cast<Int64>(b.get());
}

Int32 LegacyCameraDevice::NativeSetSurfaceOrientation(
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 facing,
    /* [in] */ Int32 sensorOrientation)
{
    Logger::V("LegacyCameraDevice", "nativeSetSurfaceOrientation");
    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native window "
                "from surface.", __FUNCTION__);
        return android::BAD_VALUE;
    }

    android::status_t err = android::NO_ERROR;
    android::CameraMetadata staticMetadata;

    int32_t orientVal = static_cast<int32_t>(sensorOrientation);
    uint8_t facingVal = static_cast<uint8_t>(facing);
    staticMetadata.update(ANDROID_SENSOR_ORIENTATION, &orientVal, 1);
    staticMetadata.update(ANDROID_LENS_FACING, &facingVal, 1);

    int32_t transform = 0;

    if ((err = android::CameraUtils::getRotationTransform(staticMetadata,
            /*out*/&transform)) != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Invalid rotation transform %s (%d)",
                __FUNCTION__, strerror(-err),
                err);
        return err;
    }

    Logger::V("LegacyCameraDevice", "%s: Setting buffer sticky transform to %d",
            __FUNCTION__, transform);

    if ((err = native_window_set_buffers_sticky_transform(anw.get(),
            transform)) != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Unable to configure surface transform, "
                "error %s (%d)", __FUNCTION__,
                strerror(-err), err);
        return err;
    }

    return android::NO_ERROR;
}

static android::sp<ANativeWindow> android_SurfaceTexture_getNativeWindow(
    /* [in] */ ISurfaceTexture* st)
{
    Int64 stptr, pptr;
    st->GetSurfaceTexture(&stptr);
    st->GetProducer(&pptr);
    android::sp<android::GLConsumer> surfaceTexture((android::GLConsumer*)stptr);
    android::sp<android::IGraphicBufferProducer> producer((android::IGraphicBufferProducer*)pptr);
    android::sp<android::Surface> surfaceTextureClient(
        surfaceTexture != NULL ? new android::Surface(producer) : NULL);
    return surfaceTextureClient;
}

static android::sp<ANativeWindow> getNativeWindowFromTexture(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    android::sp<ANativeWindow> anw;
    if (surfaceTexture) {
        anw = android_SurfaceTexture_getNativeWindow(surfaceTexture);
    }
    else {
        Logger::E("LegacyCameraDevice", "jniThrowNullPointerException: surfaceTexture");
        return NULL;
    }
    if (anw == NULL) {
        Logger::E("LegacyCameraDevice", "java/lang/IllegalArgumentException"
                "SurfaceTexture had no valid native window.");
        return NULL;
    }
    return anw;
}

Int32 LegacyCameraDevice::NativeDetectTextureDimens(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ ArrayOf<Int32>* dimens)
{
    Logger::V("LegacyCameraDevice", "nativeDetectTextureDimens");
    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindowFromTexture(surfaceTexture)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native window "
                "from SurfaceTexture.", __FUNCTION__);
        return android::BAD_VALUE;
    }

    int32_t dimenBuf[2];
    android::status_t err = anw->query(anw.get(), NATIVE_WINDOW_WIDTH, dimenBuf);
    if(err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while querying SurfaceTexture "
                "width %s (%d)", __FUNCTION__,
                strerror(-err), err);
        return err;
    }

    err = anw->query(anw.get(), NATIVE_WINDOW_HEIGHT, dimenBuf + 1);
    if(err != android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Error while querying SurfaceTexture "
                "height %s (%d)", __FUNCTION__,
                strerror(-err), err);
        return err;
    }

    //env->SetIntArrayRegion(dimens, /*start*/0, /*length*/ARRAY_SIZE(dimenBuf), dimenBuf);
    (*dimens)[0] = dimenBuf[0];
    (*dimens)[1] = dimenBuf[1];
    // if (env->ExceptionCheck()) {
    //     return android::BAD_VALUE;
    // }
    return android::NO_ERROR;
}

Int32 LegacyCameraDevice::NativeSetNextTimestamp(
    /* [in] */ ISurface* surface,
    /* [in] */ Int64 timestamp)
{
    Logger::V("LegacyCameraDevice", "nativeSetNextTimestamp");
    android::sp<ANativeWindow> anw;
    if ((anw = getNativeWindow(surface)) == NULL) {
        Logger::E("LegacyCameraDevice", "%s: Could not retrieve native window from "
                "surface.", __FUNCTION__);
        return android::BAD_VALUE;
    }

    android::status_t err = android::NO_ERROR;

    if ((err = native_window_set_buffers_timestamp(anw.get(), static_cast<int64_t>(timestamp))) !=
            android::NO_ERROR) {
        Logger::E("LegacyCameraDevice", "%s: Unable to set surface timestamp, "
                "error %s (%d)", __FUNCTION__, strerror(-err),
                err);
        return err;
    }
    return android::NO_ERROR;
}

Int32 LegacyCameraDevice::NativeGetJpegFooterSize()
{
    Logger::V("LegacyCameraDevice", "nativeGetJpegFooterSize");
    return static_cast<Int32>(sizeof(struct camera3_jpeg_blob));
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
