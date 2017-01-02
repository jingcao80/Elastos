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
const Boolean FlashlightController::DEBUG = TRUE;
const Int32 FlashlightController::DISPATCH_ERROR = 0;
const Int32 FlashlightController::DISPATCH_OFF = 1;
const Int32 FlashlightController::DISPATCH_AVAILABILITY_CHANGED = 2;

FlashlightController::CameraListener::CameraListener(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode FlashlightController::CameraListener::OnOpened(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    host->mCameraDevice = camera;
    host->PostUpdateFlashlight();
    return NOERROR;
}

ECode FlashlightController::CameraListener::OnDisconnected(
    /* [in] */ ICameraDevice* camera)
{
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    if (host->mCameraDevice.Get() == camera) {
        host->DispatchOff();
        host->Teardown();
    }
    return NOERROR;
}

ECode FlashlightController::CameraListener::OnError(
    /* [in] */ ICameraDevice* camera,
    /* [in] */ Int32 error)
{
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    if (camera == host->mCameraDevice.Get() || host->mCameraDevice == NULL) {
        host->HandleError();
    }
    return NOERROR;
}

FlashlightController::SessionListener::SessionListener(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode FlashlightController::SessionListener::OnConfigured(
    /* [in] */ ICameraCaptureSession* session)
{
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    host->mSession = session;
    host->PostUpdateFlashlight();
    return NOERROR;
}

ECode FlashlightController::SessionListener::OnConfigureFailed(
    /* [in] */ ICameraCaptureSession* session)
{
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    if (host->mSession == NULL || host->mSession.Get() == session) {
        host->HandleError();
    }
    return NOERROR;
}

FlashlightController::UpdateFlashlightRunnable::UpdateFlashlightRunnable(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode FlashlightController::UpdateFlashlightRunnable::Run()
{
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    host->UpdateFlashlight(FALSE /* forceDisable */);
    return NOERROR;
}

FlashlightController::KillFlashlightRunnable::KillFlashlightRunnable(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode FlashlightController::KillFlashlightRunnable::Run()
{
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    {
        AutoLock syncLock(this);
        host->mFlashlightEnabled = FALSE;
    }
    host->UpdateFlashlight(TRUE /* forceDisable */);
    host->DispatchOff();

    return NOERROR;
}

FlashlightController::AvailabilityCallback::AvailabilityCallback(
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode FlashlightController::AvailabilityCallback::OnCameraAvailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraAvailable(%s)", cameraId.string());
    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    if (cameraId.Equals(host->mCameraId)) {
        SetCameraAvailable(TRUE,  host);
    }
    return NOERROR;
}

ECode FlashlightController::AvailabilityCallback::OnCameraUnavailable(
    /* [in] */ const String& cameraId)
{
    if (DEBUG) Logger::D(TAG, "onCameraUnavailable(%s)", cameraId.string());

    AutoPtr<IFlashlightController> hostObj;
    mWeakHost->Resolve(EIID_IFlashlightController, (IInterface**)&hostObj);
    FlashlightController* host = (FlashlightController*)hostObj.Get();
    if (NULL == host) return NOERROR;

    if (cameraId.Equals(host->mCameraId)) {
        SetCameraAvailable(FALSE, host);
    }
    return NOERROR;
}

void FlashlightController::AvailabilityCallback::SetCameraAvailable(
    /* [in] */ Boolean available,
    /* [in] */ FlashlightController* host)
{
    Boolean changed;
    {
        AutoLock syncLock(host);
        changed = host->mCameraAvailable != available;
        host->mCameraAvailable = available;
    }
    if (changed) {
        if (DEBUG) Logger::D(TAG, "dispatchAvailabilityChanged(%d)", available);
        host->DispatchAvailabilityChanged(available);
    }
}

CAR_INTERFACE_IMPL(FlashlightController, Object, IFlashlightController)

FlashlightController::FlashlightController()
    : mFlashlightEnabled(FALSE)
    , mCameraAvailable(FALSE)
{
}

FlashlightController::~FlashlightController()
{
    Logger::I(TAG, " >> Destory FlashlightController %p", this);
}

ECode FlashlightController::constructor(
    /* [in] */ IContext* mContext)
{
    CArrayList::New(1, (IArrayList**)&mListeners);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mCameraListener = new CameraListener(wr);
    mSessionListener = new SessionListener(wr);
    mUpdateFlashlightRunnable = new UpdateFlashlightRunnable(wr);
    mKillFlashlightRunnable = new KillFlashlightRunnable(wr);
    mAvailabilityCallback = new AvailabilityCallback(wr);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::CAMERA_SERVICE, (IInterface**)&obj);
    mCameraManager = ICameraManager::Probe(obj);

    if (FAILED(GetCameraId(&mCameraId))) {
        Logger::E(TAG, "Error: Couldn't initialize FlashlightController.");
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
    {
        AutoLock syncLock(this);
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
    AutoLock syncLock(mListeners);
    CleanUpListenersLocked(l);
    AutoPtr<IWeakReference> wr;
    IWeakReferenceSource::Probe(l)->GetWeakReference((IWeakReference**)&wr);
    mListeners->Add(wr);
    return NOERROR;
}

ECode FlashlightController::RemoveListener(
    /* [in] */ IFlashlightListener* l)
{
    AutoLock syncLock(mListeners);
    CleanUpListenersLocked(l);
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
    FAIL_RETURN(GetCameraId(&id));
    return mCameraManager->OpenCamera(id, mCameraListener, mHandler);
}

ECode FlashlightController::StartSession() /*throws CameraAccessException*/
{
    mSurfaceTexture = NULL;
    CSurfaceTexture::New(FALSE, (ISurfaceTexture**)&mSurfaceTexture);
    AutoPtr<ISize> size;
    String id;
    mCameraDevice->GetId(&id);
    GetSmallestSize(id, (ISize**)&size);

    Int32 w = 0, h = 0;
    size->GetWidth(&w);
    size->GetHeight(&h);
    mSurfaceTexture->SetDefaultBufferSize(w, h);
    mSurface = NULL;
    CSurface::New(mSurfaceTexture, (ISurface**)&mSurface);
    AutoPtr<IList> outputs;  /*<Surface*/
    CArrayList::New(1, (IList**)&outputs);
    outputs->Add(mSurface);
    return mCameraDevice->CreateCaptureSession(outputs, mSessionListener.Get(), mHandler);
}

ECode FlashlightController::GetSmallestSize(
    /* [in] */ const String& cameraId,
    /* [out] */ ISize** result) /*throws CameraAccessException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<ICameraCharacteristics> outcc;
    FAIL_RETURN(mCameraManager->GetCameraCharacteristics(cameraId, (ICameraCharacteristics**)&outcc));
    AutoPtr<IInterface> values;
    outcc->Get(CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP, (IInterface**)&values);
    assert(IStreamConfigurationMap::Probe(values));

    Logger::I(TAG, " >> GetSmallestSize %s: %s", cameraId.string(), TO_CSTR(values));
    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
    AutoPtr<IClassInfo> classInfo;
    FAIL_RETURN(cl->LoadClass(String("Elastos.Droid.Graphics.CSurfaceTexture"), (IClassInfo**)&classInfo))
    assert(classInfo != NULL);
    AutoPtr<ArrayOf<ISize*> > outputSizes;
    FAIL_RETURN(IStreamConfigurationMap::Probe(values)->GetOutputSizes(classInfo, (ArrayOf<ISize*>**)&outputSizes))
    if (outputSizes == NULL || outputSizes->GetLength() == 0) {
        Logger::E(TAG, "Camera %s doesn't support any outputSize.", cameraId.string());
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 w = 0, h = 0, cw = 0, ch = 0;
    AutoPtr<ISize> chosen = (*outputSizes)[0];
    for (Int32 i = 0; i < outputSizes->GetLength(); i++) {
        ISize* s = (*outputSizes)[i];
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
    *result = String(NULL);

    AutoPtr<ArrayOf<String> > ids;
    ECode ec = mCameraManager->GetCameraIdList((ArrayOf<String>**)&ids);
    if (FAILED(ec)) {
        Logger::W(TAG, "Failed to get camera id list.");
        return ec;
    }

    for (Int32 i = 0; i < ids->GetLength(); i++) {
        String id = (*ids)[i];
        AutoPtr<ICameraCharacteristics> c;
        mCameraManager->GetCameraCharacteristics(id, (ICameraCharacteristics**)&c);
        AutoPtr<IInterface> obj;
        c->Get(CameraCharacteristics::FLASH_INFO_AVAILABLE, (IInterface**)&obj);
        Boolean flashAvailable = FALSE;
        IBoolean* bobj = IBoolean::Probe(obj);
        if (bobj) {
            bobj->GetValue(&flashAvailable);
        }

        if (flashAvailable) {
            obj = NULL;
            c->Get(CameraCharacteristics::LENS_FACING, (IInterface**)&obj);
            Int32 lensFacing = 0;
            IInteger32* iobj = IInteger32::Probe(obj);
            if (iobj) {
                iobj->GetValue(&lensFacing);
                if (lensFacing == ICameraMetadata::LENS_FACING_BACK) {
                    *result = id;
                    return NOERROR;
                }
            }
        }
    }

    Logger::I(TAG, "Flash camera is not available.");
    return E_CAMERA_ACCESS_EXCEPTION;
}

void FlashlightController::UpdateFlashlight(
    /* [in] */ Boolean forceDisable)
{
    ECode ec = NOERROR;
    do {
        Boolean enabled = FALSE;
        {
            AutoLock syncLock(this);
            enabled = mFlashlightEnabled && !forceDisable;
        }

        if (enabled) {
            if (mCameraDevice == NULL) {
                ec = StartDevice();
                if (FAILED(ec)) {
                    Logger::E(TAG, " >> failed to StartDevice");
                    break;
                }
                return;
            }

            if (mSession == NULL) {
                ec = StartSession();
                if (FAILED(ec)) {
                    Logger::E(TAG, " >> failed to StartSession");
                    break;
                }
                return;
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
                    Logger::I(TAG, " >> failed to Build request");
                    break;
                }

                Int32 v = 0;
                ec = mSession->Capture(request, NULL, mHandler, &v);
                if (FAILED(ec)) {
                    Logger::I(TAG, " >> failed to Capture");
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
        Logger::E(TAG, "Error in updateFlashlight, ec=%08x", ec);
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
        mSurface->ReleaseResources();
        mSurfaceTexture->ReleaseResources();
    }
    mSurface = NULL;
    mSurfaceTexture = NULL;
}

void FlashlightController::HandleError()
{
    {
        AutoLock syncLock(this);
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
