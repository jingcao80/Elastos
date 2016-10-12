
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/server/CTorchService.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Hardware::EIID_IITorchService;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Hardware::Camera2::EIID_ICameraCaptureSessionStateListener;
using Elastos::Droid::Hardware::Camera2::EIID_ICameraManagerAvailabilityCallback;
using Elastos::Droid::Hardware::Camera2::EIID_ICameraDeviceStateListener;
using Elastos::Droid::Hardware::Camera2::EIID_ICameraDeviceStateCallback;
using Elastos::Droid::Hardware::Camera2::EIID_ICameraCaptureSessionStateCallback;
using Elastos::Droid::Hardware::Camera2::ICameraDeviceStateCallback;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSessionStateCallback;
using Elastos::Droid::Hardware::Camera2::CameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::ICaptureRequestBuilder;
using Elastos::Droid::Hardware::Camera2::ICameraMetadata;
using Elastos::Droid::Hardware::Camera2::CaptureRequest;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::Manifest;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IThread;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CTorchService::TAG("CTorchService");
const Boolean CTorchService::DEBUG = TRUE;

const Int32 CTorchService::DISPATCH_ERROR = 0;
const Int32 CTorchService::DISPATCH_STATE_CHANGE = 1;
const Int32 CTorchService::DISPATCH_AVAILABILITY_CHANGED = 2;

//==========================================================================================
// CTorchService::CameraUserRecord::
//==========================================================================================

CTorchService::CameraUserRecord::CameraUserRecord(
    /* [in] */ IBinder* token)
{
    mToken = token;
    mPid = Binder::GetCallingPid();
    mUid = Binder::GetCallingUid();
}

//==========================================================================================
// CTorchService::UpdateRunnable::
//==========================================================================================
CTorchService::UpdateRunnable::UpdateRunnable(
    /* [in] */ CTorchService* host)
    : mHost(host)
{}

ECode CTorchService::UpdateRunnable::Run()
{
    mHost->UpdateFlashlight(FALSE /* forceDisable */);
    return NOERROR;
}

//==========================================================================================
// CTorchService::KillRunnable::
//==========================================================================================
CTorchService::KillRunnable::KillRunnable(
    /* [in] */ CTorchService* host)
    : mHost(host)
{}

ECode CTorchService::KillRunnable::Run()
{
    {    AutoLock syncLock(this);
        mHost->mTorchEnabled = FALSE;
    }
    mHost->UpdateFlashlight(TRUE /* forceDisable */);
    mHost->DispatchStateChange(FALSE);
    return NOERROR;
}

//==========================================================================================
// CTorchService::CameraDeviceStateListener::
//==========================================================================================
CAR_INTERFACE_IMPL_2(CTorchService::CameraDeviceStateListener, Object,
    ICameraDeviceStateListener, ICameraDeviceStateCallback)

CTorchService::CameraDeviceStateListener::CameraDeviceStateListener(
    /* [in] */ CTorchService* host)
    : mHost(host)
{}

ECode CTorchService::CameraDeviceStateListener::OnOpened(
    /* [in] */ ICameraDevice* camera)
{
    if (mHost->mOpeningCamera) {
        mHost->mCameraDevice = camera;
        mHost->mOpeningCamera = FALSE;
        mHost->PostUpdateFlashlight();
    }
    else {
        mHost->TeardownTorch();
    }
    return NOERROR;
}

ECode CTorchService::CameraDeviceStateListener::OnDisconnected(
    /* [in] */ ICameraDevice* camera)
{
    if (mHost->mCameraDevice.Get() == camera) {
        mHost->DispatchStateChange(FALSE);
        mHost->TeardownTorch();
    }
    return NOERROR;
}

ECode CTorchService::CameraDeviceStateListener::OnError(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 error)
{
    Logger::E(TAG, "Camera error: camera=%p error=%d", camera, error);
    if (camera == mHost->mCameraDevice || mHost->mCameraDevice == NULL) {
        mHost->HandleError();
    }
    return NOERROR;
}

//==========================================================================================
// CTorchService::CameraCaptureSessionStateListener::
//==========================================================================================
CAR_INTERFACE_IMPL_2(CTorchService::CameraCaptureSessionStateListener,
    Object, ICameraCaptureSessionStateListener, ICameraCaptureSessionStateCallback)

CTorchService::CameraCaptureSessionStateListener::CameraCaptureSessionStateListener(
    /* [in] */ CTorchService* host)
    : mHost(host)
{}

ECode CTorchService::CameraCaptureSessionStateListener::OnConfigured(
    /* [in] */ ICameraCaptureSession* session)
{
    if (mHost->mSurface != NULL) {
        mHost->mSession = session;
        mHost->PostUpdateFlashlight();
    }
    return NOERROR;
}

ECode CTorchService::CameraCaptureSessionStateListener::OnConfigureFailed(
    /* [in] */ ICameraCaptureSession* session)
{
    Logger::E(TAG, "Configure failed.");
    if (mHost->mSession == NULL || mHost->mSession.Get() == session) {
        mHost->HandleError();
    }
    return NOERROR;
}

//==========================================================================================
// CTorchService::CameraManagerAvailabilityCallback::
//==========================================================================================
CAR_INTERFACE_IMPL(CTorchService::CameraManagerAvailabilityCallback, Object,
    ICameraManagerAvailabilityCallback)

CTorchService::CameraManagerAvailabilityCallback::CameraManagerAvailabilityCallback(
    /* [in] */ CTorchService* host)
    : mHost(host)
{}

ECode CTorchService::CameraManagerAvailabilityCallback::OnCameraAvailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraAvailable(%s)", cameraId.string());
    if (cameraId.Equals(StringUtils::ToString(mHost->mTorchCameraId))) {
        SetTorchAvailable(TRUE);
    }
    return NOERROR;
}

ECode CTorchService::CameraManagerAvailabilityCallback::OnCameraUnavailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraUnavailable(%s)", cameraId.string());
    Boolean openedOurselves = mHost->mOpeningCamera || mHost->mCameraDevice != NULL;
    if (!openedOurselves && cameraId.Equals(StringUtils::ToString(mHost->mTorchCameraId))) {
        SetTorchAvailable(FALSE);
    }
    return NOERROR;
}

ECode CTorchService::CameraManagerAvailabilityCallback::SetTorchAvailable(
    /* [in] */ Boolean available)
{
    Boolean oldAvailable;
    {
        AutoLock syncLock(mHost);
        oldAvailable = mHost->mTorchAvailable;
        mHost->mTorchAvailable = available;
    }
    if (oldAvailable != available) {
        if (DEBUG) Logger::D(TAG, "dispatchAvailabilityChanged(%d)", available);
        mHost->DispatchAvailabilityChanged(available);
    }
    return NOERROR;
}

//==========================================================================================
// CTorchService::BinderDeathRecipient::
//==========================================================================================
CAR_INTERFACE_IMPL(CTorchService::BinderDeathRecipient, Object, IProxyDeathRecipient)

CTorchService::BinderDeathRecipient::BinderDeathRecipient(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 cameraId,
    /* [in] */ CTorchService* host)
    : mHost(host)
    , mCameraId(cameraId)
    , mToken(token)
{}

ECode CTorchService::BinderDeathRecipient::ProxyDied()
{
    {    AutoLock syncLock(mHost->mCamerasInUse);
        if (DEBUG) Logger::D(TAG, "Camera %d client died", mCameraId);
        mHost->RemoveCameraUserLocked(mToken, mCameraId);
    }
    return NOERROR;
}

//==========================================================================================
// CTorchService::
//==========================================================================================
CAR_OBJECT_IMPL(CTorchService)

CAR_INTERFACE_IMPL(CTorchService, Object, IITorchService)

CTorchService::CTorchService()
    : mTorchEnabled(FALSE)
    , mTorchAvailable(FALSE)
    , mTorchAppUid(0)
    , mTorchCameraId(-1)
    , mOpeningCamera(FALSE)
{
}

ECode CTorchService::constructor(
    /* [in] */ IContext* context)
{
    CRemoteCallbackList::New((IRemoteCallbackList**)&mListeners);
    mTorchCameraListener = new CameraDeviceStateListener(this);
    mTorchSessionListener = new CameraCaptureSessionStateListener(this);
    mAvailabilityCallback = new CameraManagerAvailabilityCallback(this);

    mContext = context;
    CSparseArray::New((ISparseArray**)&mCamerasInUse);
    AutoPtr<IInterface> cm;
    context->GetSystemService(IContext::CAMERA_SERVICE, (IInterface**)&cm);
    mCameraManager = ICameraManager::Probe(cm);
    Initialize();
    return NOERROR;
}

ECode CTorchService::Initialize()
{
    String cameraId = GetCameraId();
    if (DEBUG) Logger::D(TAG, "get torch camera id %s.", cameraId.string());

    if (!cameraId.IsNull()) {
        mTorchCameraId = StringUtils::ParseInt32(cameraId);
        if (mTorchCameraId != -1) {
            EnsureHandler();
            mCameraManager->RegisterAvailabilityCallback(mAvailabilityCallback, mHandler);
        }
    }
    else {
        Logger::W(TAG, "failed to get torch camera id.");
    }
    return NOERROR;
}

ECode CTorchService::OnCameraOpened(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraOpened(token= %p, cameraId=%d)", token, cameraId);
    Boolean needTorchShutdown = FALSE;

    {    AutoLock syncLock(mCamerasInUse);
        if (mTorchAppUid != -1 && Binder::GetCallingUid() == mTorchAppUid) {
            if (DEBUG) Logger::D(TAG, "Camera was opened by torch app");
            mTorchCameraId = cameraId;
        }
        else {
            // As a synchronous broadcast is an expensive operation, only
            // attempt to kill torch if it actually grabbed the camera before
            if (cameraId == mTorchCameraId) {
                AutoPtr<IInterface> p;
                mCamerasInUse->Get(cameraId, (IInterface**)&p);
                if (p != NULL) {
                    if (DEBUG) Logger::D(TAG, "Need to kill torch");
                    needTorchShutdown = TRUE;
                }
            }
        }
    }

    // Shutdown torch outside of lock - torch shutdown will call into onCameraClosed()
    if (needTorchShutdown) {
        mKillFlashlightRunnable->Run();
    }

    AutoPtr<IProxy> proxy = (IProxy*)token->Probe(EIID_IProxy);
    if (proxy != NULL) {
        AutoPtr<BinderDeathRecipient> p = new BinderDeathRecipient(token, cameraId, this);
        proxy->LinkToDeath(p, 0);
    }
    {
        AutoLock syncLock(mCamerasInUse);
        AutoPtr<CameraUserRecord> p = new CameraUserRecord(token);
        mCamerasInUse->Put(cameraId, (IObject*)p.Get());
    }
    return NOERROR;
}

ECode CTorchService::OnCameraClosed(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraClosed(token=%p, cameraId=%d)", token, cameraId);
    {
        AutoLock syncLock(mCamerasInUse);
        RemoveCameraUserLocked(token, cameraId);
    }
    return NOERROR;
}

ECode CTorchService::SetTorchEnabled(
    /* [in] */ Boolean enabled)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_TORCH_SERVICE, String(NULL));
    if (mTorchEnabled != enabled) {
        mTorchEnabled = enabled;
        PostUpdateFlashlight();
    }
    return NOERROR;
}

ECode CTorchService::ToggleTorch()
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_TORCH_SERVICE, String(NULL));
    SetTorchEnabled(!mTorchEnabled);
    return NOERROR;
}

ECode CTorchService::IsAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_TORCH_SERVICE, String(NULL));
    *result = mTorchAvailable;
    return NOERROR;
}

ECode CTorchService::IsTorchOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_TORCH_SERVICE, String(NULL));
    *result = mTorchEnabled;
    return NOERROR;
}

ECode CTorchService::AddListener(
    /* [in] */ IITorchCallback* l)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_TORCH_SERVICE, String(NULL));
    Boolean b = FALSE;
    mListeners->Register(l, &b);
    return NOERROR;
}

ECode CTorchService::RemoveListener(
    /* [in] */ IITorchCallback* l)
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::ACCESS_TORCH_SERVICE, String(NULL));
    Boolean b = FALSE;
    mListeners->Unregister(l, &b);
    return NOERROR;
}

void CTorchService::EnsureHandler()
{
    if (mHandler == NULL) {
        AutoPtr<IHandlerThread> thread;
        CHandlerThread::New(TAG, IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&thread);
        IThread::Probe(thread)->Start();
        AutoPtr<ILooper> lp;
        thread->GetLooper((ILooper**)&lp);
        CHandler::New(lp, (IHandler**)&mHandler);
    }
}

void CTorchService::StartDevice()
{
    mTorchAppUid = Binder::GetCallingUid();
    String cameraId = GetCameraId();
    if (DEBUG) Logger::D(TAG, "startDevice(), cameraID: %s", cameraId.string());
    mTorchCameraId = StringUtils::ParseInt32(cameraId);
    mOpeningCamera = TRUE;
    mCameraManager->OpenCamera(cameraId, mTorchCameraListener, mHandler);
}

void CTorchService::StartSession()
{
    CSurfaceTexture::New(FALSE, (ISurfaceTexture**)&mSurfaceTexture);
    String str;
    mCameraDevice->GetId(&str);
    AutoPtr<ISize> size = GetSmallestSize(str);
    Int32 width = 0, height = 0;
    size->GetWidth(&width);
    size->GetHeight(&height);
    mSurfaceTexture->SetDefaultBufferSize(width, height);
    CSurface::New(mSurfaceTexture, (ISurface**)&mSurface);
    AutoPtr<IList> outputs;
    CArrayList::New(1, (IList**)&outputs);
    outputs->Add(IInterface::Probe(mSurface));
    mCameraDevice->CreateCaptureSession(outputs, mTorchSessionListener, mHandler);
}

AutoPtr<ISize> CTorchService::GetSmallestSize(
    /* [in] */ String cameraId)
{
    AutoPtr<ICameraCharacteristics> cc;
    mCameraManager->GetCameraCharacteristics(cameraId, (ICameraCharacteristics**)&cc);
    AutoPtr<IInterface> p;
    cc->Get(CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP, (IInterface**)&p);
    assert(0);
    AutoPtr<ArrayOf<ISize*> > outputSizes;// = p->GetOutputSizes(SurfaceTexture.class);
    if (outputSizes == NULL || outputSizes->GetLength() == 0) {
        // throw new IllegalStateException(
        //         "Camera " + cameraId + "doesn't support any outputSize.");
        return NULL;
    }
    AutoPtr<ISize> chosen = (*outputSizes)[0];
    for (Int32 i = 0; i < outputSizes->GetLength(); i++) {
        AutoPtr<ISize> s = (*outputSizes)[i];
        Int32 cWidth = 0, sWidth = 0, cHeight = 0, sHeight = 0;
        chosen->GetWidth(&cWidth);
        s->GetWidth(&sWidth);
        chosen->GetHeight(&cHeight);
        s->GetHeight(&sHeight);
        if (cWidth >= sWidth && cHeight >= sHeight) {
            chosen = s;
        }
    }
    return chosen;
}

void CTorchService::PostUpdateFlashlight()
{
    EnsureHandler();
    Boolean b = FALSE;
    mHandler->Post(mUpdateFlashlightRunnable, &b);
}

String CTorchService::GetCameraId()
{
    AutoPtr<ArrayOf<String> > ids;
    mCameraManager->GetCameraIdList((ArrayOf<String>**)&ids);
    for (Int32 i = 0; i < ids->GetLength(); i++) {
        String id = (*ids)[i];
        AutoPtr<ICameraCharacteristics> c;
        mCameraManager->GetCameraCharacteristics(id, (ICameraCharacteristics**)&c);
        Logger::I(TAG, " >> ================== Start CTorchService::GetCameraId %s =======================", id.string());
        AutoPtr<IInterface> p;
        c->Get(CameraCharacteristics::FLASH_INFO_AVAILABLE, (IInterface**)&p);
        Boolean flashAvailable = FALSE;
        if (IBoolean::Probe(p)) {
            IBoolean::Probe(p)->GetValue(&flashAvailable);
        }
        Logger::I(TAG, " >> ================== End CTorchService::GetCameraId %s flashAvailable: %d", id.string(), flashAvailable);
        if (flashAvailable) {
            p = NULL;
            c->Get(CameraCharacteristics::LENS_FACING, (IInterface**)&p);
            Int32 lensFacing = 0;
            IInteger32::Probe(p)->GetValue(&lensFacing);
            if (lensFacing == CameraCharacteristics::LENS_FACING_BACK) {
                return id;
            }
        }
    }
    return String(NULL);
}

void CTorchService::UpdateFlashlight(
    /* [in] */ Boolean forceDisable)
{
    Boolean enabled;
    {    AutoLock syncLock(this);
        enabled = mTorchEnabled && !forceDisable;
    }
    if (enabled) {
        if (mCameraDevice == NULL) {
            if (!mOpeningCamera) {
                StartDevice();
            }
            return;
        }
        if (mSession == NULL) {
            StartSession();
            return;
        }
        if (mFlashlightRequest == NULL) {
            AutoPtr<ICaptureRequestBuilder> builder;
            mCameraDevice->CreateCaptureRequest(
                    ICameraDevice::TEMPLATE_PREVIEW, (ICaptureRequestBuilder**)&builder);
            AutoPtr<IInteger32> pTorch;
            CInteger32::New(ICameraMetadata::FLASH_MODE_TORCH, (IInteger32**)&pTorch);
            builder->Set(CaptureRequest::FLASH_MODE, pTorch);
            builder->AddTarget(mSurface);
            AutoPtr<ICaptureRequest> request;
            builder->Build((ICaptureRequest**)&request);
            Int32 val = 0;
            mSession->Capture(request, NULL, mHandler, &val);
            mFlashlightRequest = request;
            DispatchStateChange(TRUE);
        }
    }
    else {
        TeardownTorch();
    }
}

void CTorchService::RemoveCameraUserLocked(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 cameraId)
{
    AutoPtr<IInterface> _record;
    mCamerasInUse->Get(cameraId, (IInterface**)&_record);
    AutoPtr<CameraUserRecord> record = (CameraUserRecord*)IObject::Probe(_record);
    if (record != NULL && record->mToken.Get() == token) {
        if (DEBUG) Logger::D(TAG, "Removing camera user %p", token);
        mCamerasInUse->Delete(cameraId);
    }
}

void CTorchService::TeardownTorch()
{
    if (mCameraDevice != NULL) {
        ICloseable::Probe(mCameraDevice)->Close();
        mCameraDevice = NULL;
    }
    mOpeningCamera = FALSE;
    mSession = NULL;
    mFlashlightRequest = NULL;
    if (mSurface != NULL) {
        mSurface->ReleaseResources();
        mSurface = NULL;
        mSurfaceTexture->ReleaseResources();
        mSurfaceTexture = NULL;
    }
}

void CTorchService::HandleError()
{
    {    AutoLock syncLock(this);
        mTorchEnabled = FALSE;
    }
    DispatchError();
    DispatchStateChange(FALSE);
    UpdateFlashlight(TRUE /* forceDisable */);
}

void CTorchService::DispatchStateChange(
    /* [in] */ Boolean on)
{
    DispatchListeners(DISPATCH_STATE_CHANGE, on);
}

void CTorchService::DispatchError()
{
    DispatchListeners(DISPATCH_ERROR, FALSE /* argument (ignored) */);
}

void CTorchService::DispatchAvailabilityChanged(
    /* [in] */ Boolean available)
{
    DispatchListeners(DISPATCH_AVAILABILITY_CHANGED, available);
}

void CTorchService::DispatchListeners(
    /* [in] */ Int32 message,
    /* [in] */ Boolean argument)
{
    {    AutoLock syncLock(mListeners);
        Int32 N = 0;
        mListeners->BeginBroadcast(&N);
        for(Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> _l;
            mListeners->GetBroadcastItem(i, (IInterface**)&_l);
            AutoPtr<IITorchCallback> l = IITorchCallback::Probe(_l);

            if (message == DISPATCH_ERROR) {
                l->OnTorchError();
            }
            else if (message == DISPATCH_STATE_CHANGE) {
                l->OnTorchStateChanged(argument);
            }
            else if (message == DISPATCH_AVAILABILITY_CHANGED) {
                l->OnTorchAvailabilityChanged(argument);
            }
        }
        mListeners->FinishBroadcast();
    }
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
