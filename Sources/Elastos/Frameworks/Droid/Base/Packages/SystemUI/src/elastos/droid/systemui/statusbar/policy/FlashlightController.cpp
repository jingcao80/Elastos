
#include "elastos/droid/systemui/statusbar/policy/FlashlightController.h"
#include "elastos/droid/systemui/Utils.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/hardware/camera2/CameraCharacteristics.h>
#include <elastos/droid/hardware/camera2/CaptureRequest.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/ClassLoader.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Hardware::Camera2::CameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::CaptureRequest;
using Elastos::Droid::Hardware::Camera2::EIID_ICameraCaptureSessionStateListener;
using Elastos::Droid::Hardware::Camera2::EIID_ICameraDeviceStateListener;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::ICameraMetadata;
using Elastos::Droid::Hardware::Camera2::ICameraDeviceStateCallback;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSessionStateCallback;
using Elastos::Droid::Hardware::Camera2::ICaptureRequestBuilder;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfigurationMap;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::View::CSurface;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::IThread;
using Elastos::Core::IInteger32;
using Elastos::Core::ClassLoader;
using Elastos::Core::IClassLoader;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String FlashlightController::TAG("FlashlightController");
const Boolean FlashlightController::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const Int32 FlashlightController::DISPATCH_ERROR = 0;
const Int32 FlashlightController::DISPATCH_OFF = 1;
const Int32 FlashlightController::DISPATCH_AVAILABILITY_CHANGED = 2;

FlashlightController::CameraListener::CameraListener(
    /* [in] */ FlashlightController* host)
    : mHost(host)
{}

ECode FlashlightController::CameraListener::OnOpened(
    /* [in] */ ICameraDevice* camera)
{
    mHost->mCameraDevice = camera;
    mHost->PostUpdateFlashlight();
    return NOERROR;
}

ECode FlashlightController::CameraListener::OnDisconnected(
    /* [in] */ ICameraDevice* camera)
{
    if (mHost->mCameraDevice.Get() == camera) {
        mHost->DispatchOff();
        mHost->Teardown();
    }
    return NOERROR;
}

ECode FlashlightController::CameraListener::OnError(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 error)
{
    Logger::E(TAG, "Camera error: camera=%p error=%d", camera, error);
    if (camera == mHost->mCameraDevice.Get() || mHost->mCameraDevice == NULL) {
        mHost->HandleError();
    }
    return NOERROR;
}

FlashlightController::SessionListener::SessionListener(
    /* [in] */ FlashlightController* host)
    : mHost(host)
{}

ECode FlashlightController::SessionListener::OnConfigured(
    /* [in] */ ICameraCaptureSession* session)
{
    mHost->mSession = session;
    mHost->PostUpdateFlashlight();
    return NOERROR;
}

ECode FlashlightController::SessionListener::OnConfigureFailed(
    /* [in] */ ICameraCaptureSession* session)
{
    Logger::E(TAG, "Configure failed.");
    if (mHost->mSession == NULL || mHost->mSession.Get() == session) {
        mHost->HandleError();
    }
    return NOERROR;
}

FlashlightController::UpdateFlashlightRunnable::UpdateFlashlightRunnable(
    /* [in] */ FlashlightController* host)
    : mHost(host)
{}

ECode FlashlightController::UpdateFlashlightRunnable::Run()
{
    mHost->UpdateFlashlight(FALSE /* forceDisable */);
    return NOERROR;
}

FlashlightController::KillFlashlightRunnable::KillFlashlightRunnable(
    /* [in] */ FlashlightController* host)
    : mHost(host)
{}

ECode FlashlightController::KillFlashlightRunnable::Run()
{
    {    AutoLock syncLock(this);
        mHost->mFlashlightEnabled = FALSE;
    }
    mHost->UpdateFlashlight(TRUE /* forceDisable */);
    mHost->DispatchOff();
    return NOERROR;
}

FlashlightController::AvailabilityCallback::AvailabilityCallback(
    /* [in] */ FlashlightController* host)
    : mHost(host)
{}

ECode FlashlightController::AvailabilityCallback::OnCameraAvailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraAvailable(%s)", cameraId.string());
    if (cameraId.Equals(mHost->mCameraId)) {
        SetCameraAvailable(TRUE);
    }
    return NOERROR;
}

ECode FlashlightController::AvailabilityCallback::OnCameraUnavailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraUnavailable(%s)", cameraId.string());
    if (cameraId.Equals(mHost->mCameraId)) {
        SetCameraAvailable(FALSE);
    }
    return NOERROR;
}

void FlashlightController::AvailabilityCallback::SetCameraAvailable(
    /* [in] */ Boolean available)
{
    Boolean changed;
    {    AutoLock syncLock(mHost);
        changed = mHost->mCameraAvailable != available;
        mHost->mCameraAvailable = available;
    }
    if (changed) {
        if (DEBUG) Logger::D(TAG, "dispatchAvailabilityChanged(%d)", available);
        mHost->DispatchAvailabilityChanged(available);
    }
}

CAR_INTERFACE_IMPL(FlashlightController, Object, IFlashlightController)
FlashlightController::FlashlightController(
    /* [in] */ IContext* mContext)
{
    CArrayList::New(1, (IArrayList**)&mListeners);
    mCameraListener = (ICameraDeviceStateListener*)(new CameraListener(this))->Probe(EIID_ICameraDeviceStateListener);
    mSessionListener = (ICameraCaptureSessionStateListener*)
        (new SessionListener(this))->Probe(EIID_ICameraCaptureSessionStateListener);
    mUpdateFlashlightRunnable = new UpdateFlashlightRunnable(this);
    mKillFlashlightRunnable = new KillFlashlightRunnable(this);
    mAvailabilityCallback = new AvailabilityCallback(this);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::CAMERA_SERVICE, (IInterface**)&obj);
    mCameraManager = ICameraManager::Probe(obj);
    Initialize();
}

ECode FlashlightController::Initialize()
{
    if(FAILED(GetCameraId(&mCameraId))) {
        Logger::E(TAG, "Couldn't initialize.");
        return NOERROR;
    }

    if (mCameraId != NULL) {
        EnsureHandler();
        mCameraManager->RegisterAvailabilityCallback(mAvailabilityCallback, mHandler);
    }
    return NOERROR;
}

ECode FlashlightController::SetFlashlight(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(this);
    if (mFlashlightEnabled != enabled) {
        mFlashlightEnabled = enabled;
        PostUpdateFlashlight();
    }
    return NOERROR;
}

ECode FlashlightController::KillFlashlight()
{
    Boolean enabled;
    {    AutoLock syncLock(this);
        enabled = mFlashlightEnabled;
    }
    if (enabled) {
        Boolean tmp = FALSE;
        mHandler->Post(mKillFlashlightRunnable, &tmp);
    }
    return NOERROR;
}

ECode FlashlightController::IsAvailable(
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(result);
    *result = mCameraAvailable;
    return NOERROR;
}

ECode FlashlightController::AddListener(
    /* [in] */ IFlashlightListener* l)
{
    {    AutoLock syncLock(mListeners);
        CleanUpListenersLocked(l);
        AutoPtr<IWeakReference> wr;
        IWeakReferenceSource::Probe(l)->GetWeakReference((IWeakReference**)&wr);
        mListeners->Add(wr);
    }
    return NOERROR;
}

ECode FlashlightController::RemoveListener(
    /* [in] */ IFlashlightListener* l)
{
    {    AutoLock syncLock(mListeners);
        CleanUpListenersLocked(l);
    }
    return NOERROR;
}

void FlashlightController::EnsureHandler()
{
    AutoLock lock(this);
    if (mHandler == NULL) {
        AutoPtr<IHandlerThread> thread;
        CHandlerThread::New(TAG, IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&thread);
        IThread::Probe(thread)->Start();

        AutoPtr<ILooper> looper;
        thread->GetLooper((ILooper**)&looper);
        CHandler::New(looper, (IHandler**)&mHandler);
    }
}

ECode FlashlightController::StartDevice() /*throws CameraAccessException*/
{
    String id;
    GetCameraId(&id);
    return mCameraManager->OpenCamera(id, ICameraDeviceStateCallback::Probe(mCameraListener), mHandler);
}

ECode FlashlightController::StartSession() /*throws CameraAccessException*/
{
    CSurfaceTexture::New(FALSE, (ISurfaceTexture**)&mSurfaceTexture);
    AutoPtr<ISize> size;
    String id;
    mCameraDevice->GetId(&id);
    GetSmallestSize(id, (ISize**)&size);

    Int32 w = 0, h = 0;
    size->GetWidth(&w);
    size->GetHeight(&h);
    mSurfaceTexture->SetDefaultBufferSize(w, w);
    CSurface::New(mSurfaceTexture, (ISurface**)&mSurface);
    AutoPtr<IArrayList> outputs;  /*<Surface*/
    CArrayList::New(1, (IArrayList**)&outputs);
    outputs->Add(mSurface);
    return mCameraDevice->CreateCaptureSession(IList::Probe(outputs),
        ICameraCaptureSessionStateCallback::Probe(mSessionListener), mHandler);
}

ECode FlashlightController::GetSmallestSize(
    /* [in] */ const String& cameraId,
    /* [out] */ ISize** result) /*throws CameraAccessException*/
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICameraCharacteristics> outcc;
    FAIL_RETURN(mCameraManager->GetCameraCharacteristics(cameraId, (ICameraCharacteristics**)&outcc));
    AutoPtr<IInterface> values;
    outcc->Get(CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP, (IInterface**)&values);

    assert(IStreamConfigurationMap::Probe(values));
    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
    AutoPtr<IClassInfo> classInfo;
    FAIL_RETURN(cl->LoadClass(String("Elastos.Droid.Graphics.CSurfaceTexture"), (IClassInfo**)&classInfo))

    AutoPtr<ArrayOf<ISize*> > outputSizes;
    FAIL_RETURN(IStreamConfigurationMap::Probe(values)->GetOutputSizes(classInfo, (ArrayOf<ISize*>**)&outputSizes))
    if (outputSizes == NULL || outputSizes->GetLength() == 0) {
        Logger::E(TAG, "Camera %s doesn't support any outputSize.", cameraId.string());
        *result = NULL;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<ISize> chosen = (*outputSizes)[0];
    for (Int32 i = 0; i < outputSizes->GetLength(); i++) {
        AutoPtr<ISize> s = (*outputSizes)[i];
        Int32 w = 0, h = 0, cw = 0, ch = 0;
        chosen->GetWidth(&cw);
        chosen->GetHeight(&ch);
        s->GetWidth(&w);
        s->GetHeight(&h);
        if (cw >= w && ch >= h) {
            chosen = s;
        }
    }
    *result = chosen;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void FlashlightController::PostUpdateFlashlight()
{
    EnsureHandler();
    Boolean tmp = FALSE;
    mHandler->Post(mUpdateFlashlightRunnable, &tmp);
}

ECode FlashlightController::GetCameraId(
    /* [out] */ String* result) /*throws CameraAccessException*/
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<String> > ids;
    FAIL_RETURN(mCameraManager->GetCameraIdList((ArrayOf<String>**)&ids));
    for (Int32 i = 0; i < ids->GetLength(); i++) {
        String id = (*ids)[i];
        AutoPtr<ICameraCharacteristics> c;
        mCameraManager->GetCameraCharacteristics(id, (ICameraCharacteristics**)&c);
        AutoPtr<IInterface> bObj;
        c->Get(CameraCharacteristics::FLASH_INFO_AVAILABLE, (IInterface**)&bObj);
        Boolean flashAvailable = FALSE;

        AutoPtr<IInterface> obj;
        c->Get(CameraCharacteristics::LENS_FACING, (IInterface**)&obj);
        Int32 lensFacing = 0;
        if (bObj != NULL && (IBoolean::Probe(bObj)->GetValue(&flashAvailable), flashAvailable)
            && obj.Get() != NULL
            && (IInteger32::Probe(obj)->GetValue(&lensFacing), lensFacing) == ICameraMetadata::LENS_FACING_BACK)
        {
            *result = id;
            return NOERROR;
        }
    }
    *result = String(NULL);
    return NOERROR;
}

void FlashlightController::UpdateFlashlight(
    /* [in] */ Boolean forceDisable)
{
    ECode ec = NOERROR;
    do {
        Boolean enabled;
        {    AutoLock syncLock(this);
            enabled = mFlashlightEnabled && !forceDisable;
        }
        if (enabled) {
            if (mCameraDevice == NULL) {
                ec = StartDevice();
                if (FAILED(ec)) {
                    break;
                }
            }
            if (mSession == NULL) {
                ec = StartSession();
                if (FAILED(ec)) {
                    break;
                }
            }
            if (mFlashlightRequest == NULL) {
                AutoPtr<ICaptureRequestBuilder> builder;
                ec = mCameraDevice->CreateCaptureRequest(
                        ICameraDevice::TEMPLATE_PREVIEW, (ICaptureRequestBuilder**)&builder);
                AutoPtr<IInteger32> obj;
                CInteger32::New(ICameraMetadata::FLASH_MODE_TORCH, (IInteger32**)&obj);
                builder->Set(CaptureRequest::FLASH_MODE, obj);
                builder->AddTarget(mSurface);
                AutoPtr<ICaptureRequest> request;
                ec = builder->Build((ICaptureRequest**)&request);
                if (FAILED(ec)) {
                    break;
                }
                Int32 v = 0;
                ec = mSession->Capture(request, NULL, mHandler, &v);
                if (FAILED(ec)) {
                    break;
                }
                mFlashlightRequest = request;
            }
        }
        else {
            if (mCameraDevice != NULL) {
                ICloseable::Probe(mCameraDevice)->Close();
                Teardown();
            }
        }

    } while (0);

    if (FAILED(ec)) {
        Logger::E(TAG, "Error in updateFlashlight");
        HandleError();
        return;
    }
}

void FlashlightController::Teardown()
{
    mCameraDevice = NULL;
    mSession = NULL;
    mFlashlightRequest = NULL;
    if (mSurface != NULL) {
        mSurface->ReleaseSurface();
        mSurfaceTexture->ReleaseBuffers();
    }
    mSurface = NULL;
    mSurfaceTexture = NULL;
}

void FlashlightController::HandleError()
{
    {    AutoLock syncLock(this);
        mFlashlightEnabled = FALSE;
    }
    DispatchError();
    DispatchOff();
    UpdateFlashlight(TRUE /* forceDisable */);
}

void FlashlightController::DispatchOff()
{
    DispatchListeners(DISPATCH_OFF, FALSE /* argument (ignored) */);
}

void FlashlightController::DispatchError()
{
    DispatchListeners(DISPATCH_ERROR, FALSE /* argument (ignored) */);
}

void FlashlightController::DispatchAvailabilityChanged(
    /* [in] */ Boolean available)
{
    DispatchListeners(DISPATCH_AVAILABILITY_CHANGED, available);
}

void FlashlightController::DispatchListeners(
    /* [in] */ Int32 message,
    /* [in] */ Boolean argument)
{
    {    AutoLock syncLock(mListeners);
        Int32 N = 0;
        mListeners->GetSize(&N);
        Boolean cleanup = FALSE;
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> wr;
            mListeners->Get(i, (IInterface**)&wr);
            AutoPtr<IInterface> obj;
            IWeakReference::Probe(wr)->Resolve(EIID_IInterface, (IInterface**)&obj);
            AutoPtr<IFlashlightListener> l = IFlashlightListener::Probe(obj);
            if (l != NULL) {
                if (message == DISPATCH_ERROR) {
                    l->OnFlashlightError();
                }
                else if (message == DISPATCH_OFF) {
                    l->OnFlashlightOff();
                }
                else if (message == DISPATCH_AVAILABILITY_CHANGED) {
                    l->OnFlashlightAvailabilityChanged(argument);
                }
            }
            else {
                cleanup = TRUE;
            }
        }
        if (cleanup) {
            CleanUpListenersLocked(NULL);
        }
    }
}

void FlashlightController::CleanUpListenersLocked(
    /* [in] */ IFlashlightListener* listener)
{
    Int32 size = 0;
    mListeners->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> wr;
        mListeners->Get(i, (IInterface**)&wr);
        AutoPtr<IInterface> obj;
        IWeakReference::Probe(wr)->Resolve(EIID_IInterface, (IInterface**)&obj);
        AutoPtr<IFlashlightListener> found = IFlashlightListener::Probe(obj);

        if (found.Get() == NULL || found.Get() == listener) {
            mListeners->Remove(i);
        }
    }
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
