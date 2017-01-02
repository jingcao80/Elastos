//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/legacy/CameraDeviceUserShim.h"
#include "elastos/droid/hardware/camera2/legacy/CCameraDeviceUserShim.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacyCameraDevice.h"
#include "elastos/droid/hardware/camera2/utils/CameraBinderDecorator.h"
#include "elastos/droid/hardware/HardwareCamera.h"
#include "elastos/droid/os/ConditionVariable.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::HardwareCamera;
using Elastos::Droid::Hardware::Camera2::Legacy::CLegacyCameraDevice;
using Elastos::Droid::Hardware::Camera2::Legacy::CCameraDeviceUserShim;
using Elastos::Droid::Hardware::Camera2::Utils::CameraBinderDecorator;
using Elastos::Droid::Hardware::Camera2::Utils::ICameraBinderDecorator;
using Elastos::Droid::Os::ConditionVariable;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CThread;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::IO::EIID_ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL_3(CameraDeviceUserShim::CameraLooper, Object, ICameraDeviceUserShimCameraLooper,
        IRunnable, ICloseable)

CameraDeviceUserShim::CameraLooper::CameraLooper()
    : mCameraId(0)
{}

ECode CameraDeviceUserShim::CameraLooper::constructor(
    /* [in] */ Int32 cameraId)
{
    mCameraId = cameraId;
    mCamera = HardwareCamera::OpenUninitialized();
    mStartDone = new ConditionVariable();

    CThread::New(this, (IThread**)&mThread);
    mThread->Start();
    return NOERROR;
}

ECode CameraDeviceUserShim::CameraLooper::GetCamera(
    /* [out] */ IHardwareCamera** camera)
{
    VALIDATE_NOT_NULL(camera);

    *camera = mCamera;
    REFCOUNT_ADD(*camera);
    return NOERROR;
}

ECode CameraDeviceUserShim::CameraLooper::Run()
{
    // Set up a looper to be used by camera.
    Looper::Prepare();

    // Save the looper so that we can terminate this thread
    // after we are done with it.
    mLooper = Looper::GetMyLooper();
    Int32 error;
    mCamera->CameraInitUnspecified(mCameraId, &error);
    mInitErrors = CameraDeviceUserShim::TranslateErrorsFromCamera1(error);
    mStartDone->Open();
    return Looper::Loop();  // Blocks forever until #close is called.
}

ECode CameraDeviceUserShim::CameraLooper::Close()
{
    if (mLooper == NULL) {
        return NOERROR;
    }

    mLooper->QuitSafely();
    //try {
    ECode ec = mThread->Join();
    //} catch (InterruptedException e) {
    if (FAILED(ec)) {
        //throw new AssertionError(e);
        Logger::E(TAG,"AssertionError : %d", ec);
        return E_ASSERTION_ERROR;
    }
    //}

    mLooper = NULL;
    mThread = NULL;
    return NOERROR;
}

ECode CameraDeviceUserShim::CameraLooper::WaitForOpen(
    /* [in] */ Int32 timeoutMs,
    /* [out] */ ECode* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NOERROR;

    // Block until the camera is open asynchronously
    Boolean _result;
    mStartDone->Block(timeoutMs, &_result);
    if (!_result) {
        Logger::E(TAG, "waitForOpen - Camera failed to open after timeout of %d ms", OPEN_CAMERA_TIMEOUT_MS);
        //try {
        ECode ec = mCamera->ReleaseResources();
        //} catch (RuntimeException e) {
        if (FAILED(ec)) {
            Logger::E(TAG, "connectBinderShim - Failed to release camera after timeout %d", ec);
        }
        //}
        //throw new CameraRuntimeException(CameraAccessException.CAMERA_ERROR);
        Logger::E(TAG, "CameraAccessException.CAMERA_ERROR");
        *result = E_CAMERA_RUNTIME_EXCEPTION;
        return E_CAMERA_RUNTIME_EXCEPTION;
    }

    *result = mInitErrors;
    return NOERROR;
}

CameraDeviceUserShim::CallbackHandler::CallbackHandler(
    /* [in] */ CameraCallbackThread* host)
    : mHost(host)
{
}

ECode CameraDeviceUserShim::CallbackHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    //try {
    ECode ec;
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch (what) {
        case CameraCallbackThread::CAMERA_ERROR: {
            Int32 errorCode = arg1;
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICaptureResultExtras> resultExtras = ICaptureResultExtras::Probe(obj);
            ec = mHost->mCallbacks->OnDeviceError(errorCode, resultExtras);
            if (FAILED(ec)) {
                // throw new IllegalStateException(
                //     "Received remote exception during camera callback " + msg.what, e);
                Logger::E("CameraDeviceUserShim::CallbackHandler", "Received remote exception "
                        "during camera callback %d, %d", what, ec);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            break;
        }
        case CameraCallbackThread::CAMERA_IDLE:
            ec = mHost->mCallbacks->OnDeviceIdle();
            if (FAILED(ec)) {
                // throw new IllegalStateException(
                //     "Received remote exception during camera callback " + msg.what, e);
                Logger::E("CameraDeviceUserShim::CallbackHandler","Received remote exception "
                        "during camera callback %d, %d", what, ec);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            break;
        case CameraCallbackThread::CAPTURE_STARTED: {
            Int64 timestamp = arg2 & 0xFFFFFFFFL;
            timestamp = (timestamp << 32) | (arg1 & 0xFFFFFFFFL);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICaptureResultExtras> resultExtras = ICaptureResultExtras::Probe(obj);
            ec = mHost->mCallbacks->OnCaptureStarted(resultExtras, timestamp);
            if (FAILED(ec)) {
                // throw new IllegalStateException(
                //     "Received remote exception during camera callback " + msg.what, e);
                Logger::E("CameraDeviceUserShim::CallbackHandler","Received remote exception "
                        "during camera callback %d, %d", what, ec);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            break;
        }
        case CameraCallbackThread::RESULT_RECEIVED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IArrayOf> resultArray = IArrayOf::Probe(obj);

            AutoPtr<IInterface> obj2;
            resultArray->Get(0, (IInterface**)&obj2);
            AutoPtr<ICameraMetadataNative> result = ICameraMetadataNative::Probe(obj2);

            AutoPtr<IInterface> obj3;
            resultArray->Get(1, (IInterface**)&obj3);
            AutoPtr<ICaptureResultExtras> resultExtras = ICaptureResultExtras::Probe(obj3);

            ec = mHost->mCallbacks->OnResultReceived(result, resultExtras);
            if (FAILED(ec)) {
                // throw new IllegalStateException(
                //     "Received remote exception during camera callback " + msg.what, e);
                Logger::E("CameraDeviceUserShim::CallbackHandler","Received remote exception "
                        "during camera callback %d, %d", what, ec);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            break;
        }
        default:
            // throw new IllegalArgumentException(
            //     "Unknown callback message " + msg.what);
            Logger::E("CameraDeviceUserShim::CallbackHandler","CameraDeviceUserShim::CallbackHandler",
                    "Unknown callback message %d",what);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        // throw new IllegalStateException(
        //     "Received remote exception during camera callback " + msg.what, e);
        Logger::E("CameraDeviceUserShim::CallbackHandler","Received remote exception"
                "during camera callback %d, %d", what, ec);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    //}

    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CameraDeviceUserShim::CameraCallbackThread, Object,
        ICameraDeviceUserShimCameraCallbackThread, IICameraDeviceCallbacks)

const Int32 CameraDeviceUserShim::CameraCallbackThread::CAMERA_ERROR;
const Int32 CameraDeviceUserShim::CameraCallbackThread::CAMERA_IDLE;
const Int32 CameraDeviceUserShim::CameraCallbackThread::CAPTURE_STARTED;
const Int32 CameraDeviceUserShim::CameraCallbackThread::RESULT_RECEIVED;

CameraDeviceUserShim::CameraCallbackThread::CameraCallbackThread(
    /* [in] */ IICameraDeviceCallbacks* callbacks)
    : mCallbacks(callbacks)
{
    CHandlerThread::New(String("LegacyCameraCallback"), (IHandlerThread**)&mHandlerThread);
    IThread::Probe(mHandlerThread)->Start();
}

ECode CameraDeviceUserShim::CameraCallbackThread::Close()
{
    Boolean result;
    return mHandlerThread->QuitSafely(&result);
}

ECode CameraDeviceUserShim::CameraCallbackThread::OnDeviceError(
    /* [in] */ Int32 errorCode,
    /* [in] */ ICaptureResultExtras* resultExtras)
{
    AutoPtr<IHandler> handler = GetHandler();
    AutoPtr<IMessage> msg;
    handler->ObtainMessage(CAMERA_ERROR,
        /*arg1*/ errorCode, /*arg2*/ 0,
        /*obj*/ resultExtras, (IMessage**)&msg);

    Boolean result;
    return handler->SendMessage(msg, &result);
}

ECode CameraDeviceUserShim::CameraCallbackThread::OnDeviceIdle()
{
    AutoPtr<IHandler> handler = GetHandler();
    AutoPtr<IMessage> msg;
    handler->ObtainMessage(CAMERA_IDLE, (IMessage**)&msg);

    Boolean result;
    return handler->SendMessage(msg, &result);
}

ECode CameraDeviceUserShim::CameraCallbackThread::OnCaptureStarted(
    /* [in] */ ICaptureResultExtras* resultExtras,
    /* [in] */ Int64 timestamp)
{
    AutoPtr<IHandler> handler = GetHandler();
    AutoPtr<IMessage> msg;
    handler->ObtainMessage(CAPTURE_STARTED,
            /*arg1*/ (Int32) (timestamp & 0xFFFFFFFFL),
            /*arg2*/ (Int32) ( (timestamp >> 32) & 0xFFFFFFFFL),
            /*obj*/ resultExtras, (IMessage**)&msg);

    Boolean result;
    return handler->SendMessage(msg, &result);
}

ECode CameraDeviceUserShim::CameraCallbackThread::OnResultReceived(
    /* [in] */ ICameraMetadataNative* result,
    /* [in] */ ICaptureResultExtras* resultExtras)
{
    AutoPtr<IArrayOf> resultArray;
    CArrayOf::New(EIID_IInterface, 2, (IArrayOf**)&resultArray);
    resultArray->Set(0, TO_IINTERFACE(result));
    resultArray->Set(1, TO_IINTERFACE(resultExtras));

    AutoPtr<IHandler> handler = GetHandler();
    AutoPtr<IMessage> msg;
    handler->ObtainMessage(RESULT_RECEIVED,
            /*obj*/ TO_IINTERFACE(resultArray), (IMessage**)&msg);

    Boolean res;
    return handler->SendMessage(msg, &res);
}

ECode CameraDeviceUserShim::CameraCallbackThread::AsBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    // This is solely intended to be used for in-process binding.
    *binder = NULL;
    return NOERROR;
}

AutoPtr<IHandler> CameraDeviceUserShim::CameraCallbackThread::GetHandler()
{
    if (mHandler == NULL) {
        AutoPtr<ILooper> lopper;
        mHandlerThread->GetLooper((ILooper**)&lopper);
        AutoPtr<CallbackHandler> ch = new CallbackHandler(this);
        ch->constructor(lopper);
        mHandler = ch.Get();
    }
    return mHandler;
}

CAR_INTERFACE_IMPL_2(CameraDeviceUserShim, Object, ICameraDeviceUserShim, IICameraDeviceUser)

const String CameraDeviceUserShim::TAG("CameraDeviceUserShim");
const Boolean CameraDeviceUserShim::DEBUG = FALSE;//Log.isLoggable(LegacyCameraDevice.DEBUG_PROP, Log.DEBUG);
const Int32 CameraDeviceUserShim::OPEN_CAMERA_TIMEOUT_MS = 5000; // 5 sec (same as api1 cts timeout)

CameraDeviceUserShim::CameraDeviceUserShim()
    : mSurfaceIdCounter(0)
    , mConfiguring(FALSE)
{
}

CameraDeviceUserShim::~CameraDeviceUserShim()
{
}

ECode CameraDeviceUserShim::constructor()
{
    return NOERROR;
}

ECode CameraDeviceUserShim::constructor(
    /* [in] */ Int32 cameraId,
    /* [in] */ ILegacyCameraDevice* legacyCamera,
    /* [in] */ ICameraCharacteristics* characteristics,
    /* [in] */ ICameraDeviceUserShimCameraLooper* cameraInit,
    /* [in] */ ICameraDeviceUserShimCameraCallbackThread* cameraCallbacks)
{
    mLegacyDevice = legacyCamera;
    mConfiguring = FALSE;
    CSparseArray::New((ISparseArray**)&mSurfaces);
    mCameraCharacteristics = characteristics;
    mCameraInit = cameraInit;
    mCameraCallbacks = cameraCallbacks;

    mSurfaceIdCounter = 0;
    return NOERROR;
}

static const Int32 HardwareCamera_NO_ERROR = 0;
static const Int32 HardwareCamera_EACCESS = -13;
static const Int32 HardwareCamera_ENODEV = -19;
static const Int32 HardwareCamera_EBUSY = -16;
static const Int32 HardwareCamera_EINVAL = -22;
static const Int32 HardwareCamera_ENOSYS = -38;
static const Int32 HardwareCamera_EUSERS = -87;
static const Int32 HardwareCamera_EOPNOTSUPP = -95;

ECode CameraDeviceUserShim::TranslateErrorsFromCamera1(
    /* [in] */ Int32 errorCode)
{
    switch(errorCode) {
        case HardwareCamera_EACCESS:
            Logger::E(TAG, "RuntimeException Fail to connect to camera service");
            return E_RUNTIME_EXCEPTION;
        case HardwareCamera_ENODEV:
            Logger::E(TAG, "RuntimeException Camera initialization failed");
            return E_RUNTIME_EXCEPTION;
        case HardwareCamera_ENOSYS:
            Logger::E(TAG, "RuntimeException Camera initialization failed because some methods"
                " are not implemented");
            return E_RUNTIME_EXCEPTION;
        case HardwareCamera_EOPNOTSUPP:
            Logger::E(TAG, "RuntimeException Camera initialization failed because the hal"
                " version is not supported by this device");
            return E_RUNTIME_EXCEPTION;
        case HardwareCamera_EINVAL:
            Logger::E(TAG, "RuntimeException Camera initialization failed because the input"
                " arugments are invalid");
            return E_RUNTIME_EXCEPTION;
        case HardwareCamera_EBUSY:
            Logger::E(TAG, "RuntimeException Camera initialization failed because the camera"
                " device was already opened");
            return E_RUNTIME_EXCEPTION;
        case HardwareCamera_EUSERS:
            Logger::E(TAG, "RuntimeException Camera initialization failed because the max"
                " number of camera devices were already opened");
            return E_RUNTIME_EXCEPTION;
        case HardwareCamera_NO_ERROR:
            return NOERROR;
        default:
            // Should never hit this.
            Logger::E(TAG, "RuntimeException Unknown camera error: %d(%08x)", errorCode);
            return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CameraDeviceUserShim::ConnectBinderShim(
    /* [in] */ IICameraDeviceCallbacks* callbacks,
    /* [in] */ Int32 cameraId,
    /* [out] */ ICameraDeviceUserShim** shim)
{
    VALIDATE_NOT_NULL(shim);
    *shim = NULL;

    if (DEBUG) {
        Logger::D(TAG, "Opening shim Camera device");
    }

    /*
     * Put the camera open on a separate thread with its own looper; otherwise
     * if the main thread is used then the callbacks might never get delivered
     * (e.g. in CTS which run its own default looper only after tests)
     */

    AutoPtr<CameraLooper> init = new CameraLooper();
    init->constructor(cameraId);

    AutoPtr<CameraCallbackThread> threadCallbacks = new CameraCallbackThread(callbacks);

    // TODO: Make this async instead of blocking
    ECode initErrors;
    init->WaitForOpen(OPEN_CAMERA_TIMEOUT_MS, &initErrors);
    if (FAILED(initErrors)) {
        Logger::E(TAG, "Failed to open Camera %d", cameraId);
        return NOERROR;
    }

    AutoPtr<IHardwareCamera> legacyCamera;
    init->GetCamera((IHardwareCamera**)&legacyCamera);

    // Disable shutter sounds (this will work unconditionally) for api2 clients
    Boolean result;
    legacyCamera->DisableShutterSound(&result);

    AutoPtr<IHardwareCameraInfo> info = new HardwareCamera::CameraInfo();
    HardwareCamera::GetCameraInfo(cameraId, info);

    AutoPtr<IParameters> legacyParameters;
    //try {
    ECode ec = legacyCamera->GetParameters((IParameters**)&legacyParameters);
    //} catch (RuntimeException e) {
    if (FAILED(ec)) {
        Logger::E(TAG,  "Unable to get initial parameters");
        return E_CAMERA_RUNTIME_EXCEPTION;
    }
    //}

    AutoPtr<ICameraCharacteristics> characteristics;
    LegacyMetadataMapper::CreateCharacteristics(
        legacyParameters, info, (ICameraCharacteristics**)&characteristics);
    AutoPtr<ILegacyCameraDevice> device;
    CLegacyCameraDevice::New(cameraId, legacyCamera, characteristics,
        threadCallbacks, (ILegacyCameraDevice**)&device);
    return CCameraDeviceUserShim::New(
        cameraId, device, characteristics, init, threadCallbacks, shim);
}

ECode CameraDeviceUserShim::Disconnect()
{
    if (DEBUG) {
        Logger::D(TAG, "disconnect called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::W(TAG, "Cannot disconnect, device has already been closed.");
    }

    //try {
    ICloseable::Probe(mLegacyDevice)->Close();
    //} finally {
    ICloseable::Probe(mCameraInit)->Close();
    return mCameraCallbacks->Close();
    //}
}

ECode CameraDeviceUserShim::SubmitRequest(
    /* [in] */ ICaptureRequest* request,
    /* [in] */ Boolean streaming,
    /* [in] */ ILongParcelable* lastFrameNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "submitRequest called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot submit request, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {    AutoLock syncLock(mConfigureLock);
        if (mConfiguring) {
            Logger::E(TAG, "Cannot submit request, configuration change in progress.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
    }
    return mLegacyDevice->SubmitRequest(request, streaming, lastFrameNumber, result);
}

ECode CameraDeviceUserShim::SubmitRequestList(
    /* [in] */ IList* requestList,
    /* [in] */ Boolean streaming,
    /* [in] */ ILongParcelable* lastFrameNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "submitRequestList called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot submit request list, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {    AutoLock syncLock(mConfigureLock);
        if (mConfiguring) {
            Logger::E(TAG, "Cannot submit request, configuration change in progress.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
    }
    return mLegacyDevice->SubmitRequestList(requestList, streaming, lastFrameNumber, result);
}

ECode CameraDeviceUserShim::CancelRequest(
    /* [in] */ Int32 requestId,
    /* [in] */ ILongParcelable* lastFrameNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "cancelRequest called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot cancel request, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {    AutoLock syncLock(mConfigureLock);
        if (mConfiguring) {
            Logger::E(TAG, "Cannot cancel request, configuration change in progress.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
    }
    Int64 lastFrame;
    mLegacyDevice->CancelRequest(requestId, &lastFrame);
    lastFrameNumber->SetNumber(lastFrame);
    *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    return NOERROR;
}

ECode CameraDeviceUserShim::BeginConfigure(
   /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "beginConfigure called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot begin configure, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {
        AutoLock syncLock(mConfigureLock);
        if (mConfiguring) {
            Logger::E(TAG, "Cannot begin configure, configuration change already in progress.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
        mConfiguring = TRUE;
    }
    *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    return NOERROR;
}

ECode CameraDeviceUserShim::EndConfigure(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "endConfigure called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot end configure, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    AutoPtr<IList> surfaces;
    {
        AutoLock syncLock(mConfigureLock);
        if (!mConfiguring) {
            Logger::E(TAG, "Cannot end configure, no configuration change in progress.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
        Int32 numSurfaces;
        mSurfaces->GetSize(&numSurfaces);
        if (numSurfaces > 0) {
            CArrayList::New((IList**)&surfaces);
            for (Int32 i = 0; i < numSurfaces; ++i) {
                AutoPtr<IInterface> obj;
                mSurfaces->ValueAt(i, (IInterface**)&obj);
                surfaces->Add(obj);
            }
        }
        mConfiguring = FALSE;
    }
    return mLegacyDevice->ConfigureOutputs(surfaces, result);
}

ECode CameraDeviceUserShim::DeleteStream(
    /* [in] */ Int32 streamId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "deleteStream called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot delete stream, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {    AutoLock syncLock(mConfigureLock);
        if (!mConfiguring) {
            Logger::E(TAG, "Cannot delete stream, beginConfigure hasn't been called yet.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
        Int32 index;
        mSurfaces->IndexOfKey(streamId, &index);
        if (index < 0) {
            Logger::E(TAG, "Cannot delete stream, stream id %d doesn't exist.", streamId);
            *result = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
            return NOERROR;
        }
        mSurfaces->RemoveAt(index);
    }
    *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    return NOERROR;
}

ECode CameraDeviceUserShim::CreateStream(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 format,
    /* [in] */ ISurface* surface,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "createStream called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot create stream, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {
        AutoLock syncLock(mConfigureLock);
        if (!mConfiguring) {
            Logger::E(TAG, "Cannot create stream, beginConfigure hasn't been called yet.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
        Int32 id = ++mSurfaceIdCounter;
        mSurfaces->Put(id, TO_IINTERFACE(surface));
        *result = id;

        if (DEBUG) {
            Logger::I(TAG, " >> CreateStream: (%d, %d), format: %08x, surface: %s",
                width, height, format, TO_CSTR(surface));
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode CameraDeviceUserShim::CreateDefaultRequest(
    /* [in] */ Int32 templateId,
    /* [in] */ ICameraMetadataNative* request,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "createDefaultRequest called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot create default request, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    AutoPtr<ICameraMetadataNative> _template;
    //try {
    ECode ec = LegacyMetadataMapper::CreateRequestTemplate(mCameraCharacteristics,
            templateId, (ICameraMetadataNative**)&_template);
    //} catch (IllegalArgumentException e) {
    if (FAILED(ec)) {
        Logger::E(TAG, "createDefaultRequest - invalid templateId specified");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_BAD_VALUE;
        return NOERROR;
    }
    //}

    request->Swap(_template);
    *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    return NOERROR;
}

ECode CameraDeviceUserShim::GetCameraInfo(
    /* [in] */ ICameraMetadataNative* info,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "getCameraInfo called.");
    }
    // TODO: implement getCameraInfo.
    Logger::E(TAG, "getCameraInfo unimplemented.");
    *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    return NOERROR;
}

ECode CameraDeviceUserShim::WaitUntilIdle(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "waitUntilIdle called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot wait until idle, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {    AutoLock syncLock(mConfigureLock);
        if (mConfiguring) {
            Logger::E(TAG, "Cannot wait until idle, configuration change in progress.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
    }
    mLegacyDevice->WaitUntilIdle();
    *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    return NOERROR;
}

ECode CameraDeviceUserShim::Flush(
    /* [in] */ ILongParcelable* lastFrameNumber,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (DEBUG) {
        Logger::D(TAG, "flush called.");
    }

    Boolean res;
    mLegacyDevice->IsClosed(&res);
    if (res) {
        Logger::E(TAG, "Cannot flush, device has been closed.");
        *result = ICameraBinderDecorator::ICameraBinderDecorator_ENODEV;
        return NOERROR;
    }

    {    AutoLock syncLock(mConfigureLock);
        if (mConfiguring) {
            Logger::E(TAG, "Cannot flush, configuration change in progress.");
            *result = ICameraBinderDecorator::ICameraBinderDecorator_INVALID_OPERATION;
            return NOERROR;
        }
    }
    Int64 lastFrame;
    mLegacyDevice->Flush(&lastFrame);
    if (lastFrameNumber != NULL) {
        lastFrameNumber->SetNumber(lastFrame);
    }
    *result = ICameraBinderDecorator::ICameraBinderDecorator_NO_ERROR;
    return NOERROR;
}

ECode CameraDeviceUserShim::AsBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);

    // This is solely intended to be used for in-process binding.
    *binder = NULL;
    return NOERROR;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
