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

#include "CActivityOne.h"
#include "CAutoFitTextureView.h"
#include "R.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Widget.h>
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include <elastos/core/ClassLoader.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Media::IImagePlane;
using Elastos::Droid::Media::IImageReaderHelper;
using Elastos::Droid::Media::CImageReaderHelper;
using Elastos::Droid::Media::EIID_IImageReaderOnImageAvailableListener;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::CameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::CaptureRequest;
using Elastos::Droid::Hardware::Camera2::CaptureResult;
using Elastos::Droid::Hardware::Camera2::ICameraMetadata;
using Elastos::Droid::Hardware::Camera2::ICameraCaptureSessionStateCallback;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfigurationMap;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::MatrixScaleToFit_FILL;
using Elastos::Droid::View::CSurface;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_ISurfaceTextureListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Core::ClassLoader;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::EIID_IComparator;
using Elastos::IO::CFile;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IOutputStream;
using Elastos::IO::IAutoCloseable;
using Elastos::IO::CFileOutputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::CSemaphore;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace CameraDemo {

static const String TAG("CameraDemo::CActivityOne");


CAR_INTERFACE_IMPL(CActivityOne::CompareSizesByArea, Object, IComparator)

//=======================================================================
// CActivityOne::MyListener
//=======================================================================
ECode CActivityOne::CompareSizesByArea::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out) */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    ISize* ls = ISize::Probe(lhs);
    ISize* rs = ISize::Probe(rhs);

    Int32 lw, lh, rw, rh;
    ls->GetWidth(&lw);
    ls->GetHeight(&lh);
    rs->GetWidth(&rw);
    rs->GetHeight(&rh);

    // We cast here to ensure the multiplications won't overflow
    *result = Elastos::Core::Math::Signum((Int64)lw * lh - (Int64)rw * rh);
    return NOERROR;
}

//=======================================================================
// CActivityOne::MyListener
//=======================================================================
CAR_INTERFACE_IMPL(CActivityOne::MyListener, Object, IViewOnClickListener)

CActivityOne::MyListener::MyListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::MyListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

//=======================================================================
// CActivityOne::ImageSaver
//=======================================================================

CActivityOne::ImageSaver::ImageSaver(
    /* [in] */ CActivityOne* host,
    /* [in] */ IImage* image,
    /* [in] */ IFile* file)
    : mHost(host)
    , mImage(image)
    , mFile(file)
{}

ECode CActivityOne::ImageSaver::Run()
{
    Logger::I(TAG, " >> save image to %s", TO_CSTR(mFile));

    AutoPtr< ArrayOf<IImagePlane*> > planes;
    mImage->GetPlanes((ArrayOf<IImagePlane*>**)&planes);
    assert(planes != NULL && planes->GetLength() > 0);

    AutoPtr<IByteBuffer> buffer;
    (*planes)[0]->GetBuffer((IByteBuffer**)&buffer);

    Int32 remaining;
    IBuffer::Probe(buffer)->GetRemaining(&remaining);
    AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(remaining);
    buffer->Get(bytes);

    AutoPtr<IOutputStream> output;
    ECode ec = CFileOutputStream::New(mFile, (IOutputStream**)&output);
    FAIL_GOTO(ec, __EXIT__)

    ec = output->Write(bytes);


__EXIT__:
    IAutoCloseable::Probe(mImage)->Close();

    if (output != NULL) {
        ICloseable::Probe(output)->Close();
    }

    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to save image to %s", TO_CSTR(mFile));
    }
    return NOERROR;
}

//=======================================================================
// CActivityOne::SurfaceTextureListener
//=======================================================================
CAR_INTERFACE_IMPL(CActivityOne::SurfaceTextureListener, Object, ISurfaceTextureListener)

CActivityOne::SurfaceTextureListener::SurfaceTextureListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::SurfaceTextureListener::OnSurfaceTextureAvailable(
    /* [in] */ ISurfaceTexture* texture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Logger::I(TAG, " >> OnSurfaceTextureAvailable");
    return mHost->OpenCamera(width, height);
}

ECode CActivityOne::SurfaceTextureListener::OnSurfaceTextureSizeChanged(
    /* [in] */ ISurfaceTexture* texture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Logger::I(TAG, " >> OnSurfaceTextureSizeChanged");
    return mHost->ConfigureTransform(width, height);
}

ECode CActivityOne::SurfaceTextureListener::OnSurfaceTextureDestroyed(
    /* [in] */ ISurfaceTexture* texture,
    /* [out] */ Boolean* result)
{
    Logger::I(TAG, " >> OnSurfaceTextureDestroyed");
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode CActivityOne::SurfaceTextureListener::OnSurfaceTextureUpdated(
    /* [in] */ ISurfaceTexture* texture)
{
    Logger::I(TAG, " >> OnSurfaceTextureUpdated");
    return NOERROR;
}

//=======================================================================
// CActivityOne::CameraCaptureSessionStateCallback
//=======================================================================
CActivityOne::CameraCaptureSessionStateCallback::CameraCaptureSessionStateCallback(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::CameraCaptureSessionStateCallback::OnConfigured(
    /* [in] */ ICameraCaptureSession* cameraCaptureSession)
{
    Logger::I(TAG, " >> CaptureSession OnConfigured");

    // The camera is already closed
    if (NULL == mHost->mCameraDevice) {
        return NOERROR;
    }

    // When the session is ready, we start displaying the preview.
    mHost->mCaptureSession = cameraCaptureSession;

    // Auto focus should be continuous for camera preview.
    AutoPtr<IInteger32> iobj = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_PICTURE);
    mHost->mPreviewRequestBuilder->Set(CaptureRequest::CONTROL_AF_MODE, iobj);

    // Flash is automatically enabled when necessary.
    mHost->SetAutoFlash(mHost->mPreviewRequestBuilder);

    // Finally, we start displaying the camera preview.
    Int32 id;
    mHost->mPreviewRequest = NULL;
    mHost->mPreviewRequestBuilder->Build((ICaptureRequest**)&mHost->mPreviewRequest);
    mHost->mCaptureSession->SetRepeatingRequest(
        mHost->mPreviewRequest,
        mHost->mCaptureCallback,
        mHost->mBackgroundHandler,
        &id);
    return NOERROR;
}

ECode CActivityOne::CameraCaptureSessionStateCallback::OnConfigureFailed(
    /* [in] */ ICameraCaptureSession* cameraCaptureSession)
{
    mHost->ShowToast(String("Failed"));
    return NOERROR;
}

//=======================================================================
// CActivityOne::StateCallback
//=======================================================================
CActivityOne::StateCallback::StateCallback(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::StateCallback::OnOpened(
    /* [in] */ ICameraDevice* cameraDevice)
{
    Logger::I(TAG, " >> Camera OnOpened");
    // This method is called when the camera is opened.  We start camera preview here.
    mHost->mCameraOpenCloseLock->ReleasePermit();
    mHost->mCameraDevice = cameraDevice;
    mHost->CreateCameraPreviewSession();
    return NOERROR;
}

ECode CActivityOne::StateCallback::OnDisconnected(
    /* [in] */ ICameraDevice* cameraDevice)
{
    Logger::I(TAG, " >> Camera OnDisconnected");
    mHost->mCameraOpenCloseLock->ReleasePermit();
    ICloseable::Probe(cameraDevice)->Close();
    mHost->mCameraDevice = NULL;
    return NOERROR;
}

ECode CActivityOne::StateCallback::OnError(
    /* [in] */ ICameraDevice* cameraDevice,
    /* [in] */ Int32 error)
{
    Logger::I(TAG, " >> Camera OnError");
    mHost->mCameraOpenCloseLock->ReleasePermit();
    ICloseable::Probe(cameraDevice)->Close();
    mHost->mCameraDevice = NULL;
    mHost->Finish();
    return NOERROR;
}

//=======================================================================
// CActivityOne::ImageAvailableListener
//=======================================================================
CAR_INTERFACE_IMPL(CActivityOne::ImageAvailableListener, Object, IImageReaderOnImageAvailableListener)

CActivityOne::ImageAvailableListener::ImageAvailableListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::ImageAvailableListener::OnImageAvailable(
    /* [in] */ IImageReader* reader)
{
    Logger::I(TAG, " >> OnImageAvailable");
    // This method is called when the camera is opened.  We start camera preview here.
    AutoPtr<IImage> image;
    reader->AcquireNextImage((IImage**)&image);
    AutoPtr<ImageSaver> saver = new ImageSaver(mHost, image, mHost->mFile);
    Boolean bval;
    mHost->mBackgroundHandler->Post(saver, &bval);
    return NOERROR;
}

//=======================================================================
// CActivityOne::CaptureStillPictureCallback
//=======================================================================
CActivityOne::CaptureStillPictureCallback::CaptureStillPictureCallback(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::CaptureStillPictureCallback::OnCaptureCompleted(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ITotalCaptureResult* result)
{
    Logger::I(TAG, " >> CaptureStillPictureCallback::OnCaptureCompleted");
    StringBuilder sb("Saved: ");
    sb += TO_CSTR(mHost->mFile);

    mHost->ShowToast(sb.ToString());
    Logger::D(TAG, sb.ToString());

    mHost->UnlockFocus();
    return NOERROR;
}

//=======================================================================
// CActivityOne::CaptureCallback
//=======================================================================
CActivityOne::CaptureCallback::CaptureCallback(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

ECode CActivityOne::CaptureCallback::Process(
    /* [in] */ ICaptureResult* result)
{
    if (mHost->mState != STATE_PREVIEW) {
        Logger::I(TAG, " >> CaptureCallback Process: %s, state: %d",
            TO_CSTR(result), mHost->mState);
    }

    switch (mHost->mState) {
        case STATE_PREVIEW: {
            // We have nothing to do when the camera preview is working normally.
            break;
        }
        case STATE_WAITING_LOCK: {
            AutoPtr<IInterface> obj;
            result->Get(CaptureResult::CONTROL_AF_STATE, (IInterface**)&obj);
            AutoPtr<IInteger32> afState = IInteger32::Probe(obj);
            Logger::I(TAG, " >> STATE_WAITING_LOCK: afState: %s", TO_CSTR(afState));
            if (afState == NULL) {
                mHost->CaptureStillPicture();
            }
            else {
                Int32 ival;
                afState->GetValue(&ival);
                if (ival == ICameraMetadata::CONTROL_AF_STATE_FOCUSED_LOCKED
                    || ival == ICameraMetadata::CONTROL_AF_STATE_NOT_FOCUSED_LOCKED) {
                    // CONTROL_AE_STATE can be NULL on some devices
                    obj = NULL;
                    result->Get(CaptureResult::CONTROL_AE_STATE, (IInterface**)&obj);
                    AutoPtr<IInteger32> aeState = IInteger32::Probe(obj);
                    Logger::I(TAG, " >> STATE_WAITING_LOCK: aeState: %s", TO_CSTR(aeState));
                    ival = -1;
                    if (aeState != NULL) aeState->GetValue(&ival);
                    if (aeState == NULL
                        || ival == ICameraMetadata::CONTROL_AE_STATE_CONVERGED) {
                        mHost->mState = STATE_PICTURE_TAKEN;
                        mHost->CaptureStillPicture();
                    }
                    else {
                        mHost->RunPrecaptureSequence();
                    }
                }
            }
            break;
        }
        case STATE_WAITING_PRECAPTURE: {
            // CONTROL_AE_STATE can be NULL on some devices
            AutoPtr<IInterface> obj;
            result->Get(CaptureResult::CONTROL_AE_STATE, (IInterface**)&obj);
            AutoPtr<IInteger32> aeState = IInteger32::Probe(obj);
            Logger::I(TAG, " >> STATE_WAITING_PRECAPTURE: aeState: %s", TO_CSTR(aeState));
            Int32 ival = -1;
            if (aeState != NULL) aeState->GetValue(&ival);
            if (aeState == NULL
                || ival == ICameraMetadata::CONTROL_AE_STATE_PRECAPTURE
                || ival == ICameraMetadata::CONTROL_AE_STATE_FLASH_REQUIRED) {
                mHost->mState = STATE_WAITING_NON_PRECAPTURE;
            }
            break;
        }
        case STATE_WAITING_NON_PRECAPTURE: {
            // CONTROL_AE_STATE can be NULL on some devices
            AutoPtr<IInterface> obj;
            result->Get(CaptureResult::CONTROL_AE_STATE, (IInterface**)&obj);
            AutoPtr<IInteger32> aeState = IInteger32::Probe(obj);
            Logger::I(TAG, " >> STATE_WAITING_NON_PRECAPTURE: aeState: %s", TO_CSTR(aeState));
            Int32 ival = -1;
            if (aeState != NULL) aeState->GetValue(&ival);
            if (aeState == NULL || ival != ICameraMetadata::CONTROL_AE_STATE_PRECAPTURE) {
                mHost->mState = STATE_PICTURE_TAKEN;
                mHost->CaptureStillPicture();
            }
            break;
        }
    }
    return NOERROR;
}

ECode CActivityOne::CaptureCallback::OnCaptureProgressed(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ICaptureResult* partialResult)
{
    return Process(partialResult);
}

ECode CActivityOne::CaptureCallback::OnCaptureCompleted(
    /* [in] */ ICameraCaptureSession* session,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ ITotalCaptureResult* result)
{
    return Process(ICaptureResult::Probe(result));
}

//=======================================================================
// CActivityOne
//=======================================================================

const Int32 CActivityOne::STATE_PREVIEW;
const Int32 CActivityOne::STATE_WAITING_LOCK;
const Int32 CActivityOne::STATE_WAITING_PRECAPTURE;
const Int32 CActivityOne::STATE_WAITING_NON_PRECAPTURE;
const Int32 CActivityOne::STATE_PICTURE_TAKEN;
const Int32 CActivityOne::MAX_PREVIEW_WIDTH;
const Int32 CActivityOne::MAX_PREVIEW_HEIGHT;

static AutoPtr<HashMap<Int32, Int32> > InitORIENTATIONS()
{
    AutoPtr<HashMap<Int32, Int32> > map = new HashMap<Int32, Int32>();
    (*map)[ISurface::ROTATION_0] = 90;
    (*map)[ISurface::ROTATION_90] = 0;
    (*map)[ISurface::ROTATION_180] = 270;
    (*map)[ISurface::ROTATION_270] = 180;
    return map;
}

const AutoPtr<HashMap<Int32, Int32> > CActivityOne::ORIENTATIONS = InitORIENTATIONS();

CAR_OBJECT_IMPL(CActivityOne)

CActivityOne::CActivityOne()
    : mState(STATE_PREVIEW)
{}

CActivityOne::~CActivityOne()
{
    Logger::I(TAG, " >> Destroy CameraDemo::CActivityOne");
}

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    Activity::constructor();

    mSurfaceTextureListener = new SurfaceTextureListener(this);
    mStateCallback = new StateCallback(this);
    mOnImageAvailableListener = new ImageAvailableListener(this);
    mCaptureCallback = new CaptureCallback(this);

    CSemaphore::New(1, (ISemaphore**)&mCameraOpenCloseLock);
    return NOERROR;
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);

    AutoPtr<MyListener> listener = new MyListener(this);
    AutoPtr<IView> view;
    FindViewById(R::id::picture, (IView**)&view);
    view->SetOnClickListener(listener);

    view = NULL;
    FindViewById(R::id::info, (IView**)&view);
    view->SetOnClickListener(listener);

    view = NULL;
    FindViewById(R::id::texture, (IView**)&view);
    mTextureView = ITextureView::Probe(view);

    AutoPtr<IFile> dir;
    GetExternalFilesDir(String(NULL), (IFile**)&dir);
    CFile::New(dir, String("pic.jpg"), (IFile**)&mFile);
    Logger::I(TAG, " >> pic path: %s", TO_CSTR(mFile));

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::CAMERA_SERVICE, (IInterface**)&obj);
    mCameraManager = ICameraManager::Probe(obj);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(TAG, " >> OnResume()");
    Activity::OnResume();

    StartBackgroundThread();

    // When the screen is turned off and turned back on, the SurfaceTexture is already
    // available, and "onSurfaceTextureAvailable" will not be called. In that case, we can open
    // a camera and start preview from here (otherwise, we wait until the surface is ready in
    // the SurfaceTextureListener).
    Boolean bval;
    mTextureView->IsAvailable(&bval);
    if (bval) {
        Int32 w, h;
        IView::Probe(mTextureView)->GetWidth(&w);
        IView::Probe(mTextureView)->GetHeight(&h);
        OpenCamera(w, h);
    }
    else {
        mTextureView->SetSurfaceTextureListener(mSurfaceTextureListener);
    }
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    Logger::I(TAG, " >> OnPause()");
    CloseCamera();
    StopBackgroundThread();
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

ECode CActivityOne::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    Logger::I(TAG, " >> OnActivityResult()");
    return Activity::OnActivityResult(requestCode, resultCode, data);
}

CActivityOne::ShowToastRunnable::ShowToastRunnable(
    /* [in] */ CActivityOne* host,
    /* [in] */ const String& text)
    : mHost(host)
    , mText(text)
{
}

ECode CActivityOne::ShowToastRunnable::Run()
{
    AutoPtr<IToastHelper> helper;
    CToastHelper::AcquireSingleton((IToastHelper**)&helper);
    AutoPtr<IToast> toast;
    helper->MakeText(mHost, CoreUtils::Convert(mText), IToast::LENGTH_SHORT, (IToast**)&toast);
    return toast->Show();
}

ECode CActivityOne::ShowToast(
    /* [in] */ const String& text)
{
    AutoPtr<ShowToastRunnable> runnable = new ShowToastRunnable(this, text);
    return RunOnUiThread(runnable);
}

AutoPtr<ISize> CActivityOne::ChooseOptimalSize(
    /* [in] */ ArrayOf<ISize*>* choices,
    /* [in] */ Int32 textureViewWidth,
    /* [in] */ Int32 textureViewHeight,
    /* [in] */ Int32 maxWidth,
    /* [in] */ Int32 maxHeight,
    /* [in] */ ISize* aspectRatio)
{
    AutoPtr<IList> bigEnough, notBigEnough;
    // Collect the supported resolutions that are at least as big as the preview Surface
    CArrayList::New((IList**)&bigEnough);

    // Collect the supported resolutions that are smaller than the preview Surface
    CArrayList::New((IList**)&notBigEnough);

    Int32 w, h, ow, oh;
    aspectRatio->GetWidth(&w);
    aspectRatio->GetHeight(&h);

    for (Int32 i = 0; i < choices->GetLength(); ++i) {
        ISize* option = (*choices)[i];
        option->GetWidth(&ow);
        option->GetHeight(&oh);
        if (ow <= maxWidth && oh <= maxHeight && oh == ow * h / w) {
            if (ow >= textureViewWidth && oh >= textureViewHeight) {
                bigEnough->Add(option);
            }
            else {
                notBigEnough->Add(option);
            }
        }
    }

    // Pick the smallest of those big enough. If there is no one big enough, pick the
    // largest of those not big enough.
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    AutoPtr<IComparator> cmp = new CompareSizesByArea();
    Int32 size;
    if ((bigEnough->GetSize(&size), size) > 0) {
        AutoPtr<IInterface> obj;
        collections->Min(ICollection::Probe(bigEnough), cmp, (IInterface**)&obj);
        return ISize::Probe(obj);
    }
    else if (notBigEnough->GetSize(&size), size > 0) {
        AutoPtr<IInterface> obj;
        collections->Min(ICollection::Probe(notBigEnough), cmp, (IInterface**)&obj);
        return ISize::Probe(obj);
    }

    Logger::E(TAG, "Couldn't find any suitable preview size");
    assert(choices != NULL && choices->GetLength() > 0);
    return (*choices)[0];

}

// ECode CActivityOne::RequestCameraPermission()
// {
//     if (FragmentCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.CAMERA)) {
//         new ConfirmationDialog().show(getChildFragmentManager(), FRAGMENT_DIALOG);
//     } else {
//         FragmentCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA},
//                 REQUEST_CAMERA_PERMISSION);
//     }
// }

// @Override
// public void onRequestPermissionsResult(Int32 requestCode, @NonNull String[] permissions,
//                                        @NonNull Int32[] grantResults) {
//     if (requestCode == REQUEST_CAMERA_PERMISSION) {
//         if (grantResults.length != 1 || grantResults[0] != PackageManager.PERMISSION_GRANTED) {
//             ErrorDialog.newInstance(getString(R.string.request_permission))
//                     .show(getChildFragmentManager(), FRAGMENT_DIALOG);
//         }
//     } else {
//         super.onRequestPermissionsResult(requestCode, permissions, grantResults);
//     }
// }

static AutoPtr<IList> AsList(
    /* [in] */ ArrayOf<ISize*>* sizes)
{
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    for (Int32 i = 0; i < sizes->GetLength(); ++i) {
        list->Add((*sizes)[i]);
    }
    return list;
}

ECode CActivityOne::SetUpCameraOutputs(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    ECode ec = NOERROR;
    String cameraId;
    AutoPtr<IStreamConfigurationMap> map;
    AutoPtr<ISize> largest;
    AutoPtr<IComparator> cmp = new CompareSizesByArea();
    Int32 w, h;
    AutoPtr<IInterface> obj;

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);

    AutoPtr<IImageReaderHelper> imageReader;
    CImageReaderHelper::AcquireSingleton((IImageReaderHelper**)&imageReader);

    AutoPtr<IWindowManager> wm = GetWindowManager();
    AutoPtr<IDisplay> display;
    wm->GetDefaultDisplay((IDisplay**)&display);
    Int32 displayRotation = 0;
    display->GetRotation(&displayRotation);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);

    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
    AutoPtr<IClassInfo> classInfo;
    FAIL_RETURN(cl->LoadClass(String("Elastos.Droid.Graphics.CSurfaceTexture"), (IClassInfo**)&classInfo))

    AutoPtr<ArrayOf<String> > ids;
    ec = mCameraManager->GetCameraIdList((ArrayOf<String>**)&ids);
    FAIL_GOTO(ec, __EXIT__)

    for (Int32 i = 0; i < ids->GetLength(); ++i) {
        cameraId = (*ids)[i];

        AutoPtr<ICameraCharacteristics> characteristics;
        ec = mCameraManager->GetCameraCharacteristics(cameraId, (ICameraCharacteristics**)&characteristics);
        FAIL_GOTO(ec, __EXIT__)

        // We don't use a front facing camera in this sample.
        obj = NULL;
        characteristics->Get(CameraCharacteristics::LENS_FACING, (IInterface**)&obj);
        if (obj == NULL) continue;
        Int32 facing = -1;
        IInteger32::Probe(obj)->GetValue(&facing);
        if (facing == ICameraMetadata::LENS_FACING_FRONT) {
            continue;
        }

        obj = NULL;
        characteristics->Get(CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP, (IInterface**)&obj);
        map = IStreamConfigurationMap::Probe(obj);
        if (map == NULL) {
            continue;
        }

        // For still image captures, we use the largest available size.
        AutoPtr< ArrayOf<ISize*> > jpegSizes;
        map->GetOutputSizes(IImageFormat::JPEG, (ArrayOf<ISize*>**)&jpegSizes);
        AutoPtr<IList> jpegList = AsList(jpegSizes);

        obj = NULL;
        collections->Max(ICollection::Probe(jpegList), cmp, (IInterface**)&obj);
        largest = ISize::Probe(obj);
        largest->GetWidth(&w);
        largest->GetHeight(&h);

        mImageReader = NULL;
        imageReader->NewInstance(w, h, IImageFormat::JPEG, /*maxImages*/2, (IImageReader**)&mImageReader);
        mImageReader->SetOnImageAvailableListener(
            mOnImageAvailableListener, IHandler::Probe(mBackgroundHandler));

        // Find out if we need to swap dimension to get the preview size relative to sensor
        // coordinate.
        //noinspection ConstantConditions
        obj = NULL;
        characteristics->Get(CameraCharacteristics::SENSOR_ORIENTATION, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&mSensorOrientation);
        Boolean swappedDimensions = FALSE;
        switch (displayRotation) {
            case ISurface::ROTATION_0:
            case ISurface::ROTATION_180:
                if (mSensorOrientation == 90 || mSensorOrientation == 270) {
                    swappedDimensions = TRUE;
                }
                break;
            case ISurface::ROTATION_90:
            case ISurface::ROTATION_270:
                if (mSensorOrientation == 0 || mSensorOrientation == 180) {
                    swappedDimensions = TRUE;
                }
                break;
            default:
                Logger::E(TAG, "Display rotation is invalid: %d", displayRotation);
        }

        AutoPtr<IPoint> displaySize;
        CPoint::New((IPoint**)&displaySize);
        display->GetSize(displaySize);
        Int32 rotatedPreviewWidth = width;
        Int32 rotatedPreviewHeight = height;
        Int32 maxPreviewWidth, maxPreviewHeight;
        displaySize->GetX(&maxPreviewWidth);
        displaySize->GetY(&maxPreviewHeight);

        if (swappedDimensions) {
            rotatedPreviewWidth = height;
            rotatedPreviewHeight = width;
            displaySize->GetY(&maxPreviewWidth);
            displaySize->GetX(&maxPreviewHeight);
        }

        if (maxPreviewWidth > MAX_PREVIEW_WIDTH) {
            maxPreviewWidth = MAX_PREVIEW_WIDTH;
        }

        if (maxPreviewHeight > MAX_PREVIEW_HEIGHT) {
            maxPreviewHeight = MAX_PREVIEW_HEIGHT;
        }

        // Danger, W.R.! Attempting to use too large a preview size could  exceed the camera
        // bus' bandwidth limitation, resulting in gorgeous previews but the storage of
        // garbage capture data.
        AutoPtr< ArrayOf<ISize*> > sizes;
        map->GetOutputSizes(classInfo, (ArrayOf<ISize*>**)&sizes);
        mPreviewSize = ChooseOptimalSize(sizes,
            rotatedPreviewWidth, rotatedPreviewHeight, maxPreviewWidth,
            maxPreviewHeight, largest);

        // We fit the aspect ratio of TextureView to the size of preview we picked.
        mPreviewSize->GetWidth(&w);
        mPreviewSize->GetHeight(&h);
        Logger::I(TAG, " >> TextureView SetAspectRatio: (%d, %d)", w, h);
        if (orientation == IConfiguration::ORIENTATION_LANDSCAPE) {
            ((CAutoFitTextureView*)mTextureView.Get())->SetAspectRatio(w, h);
        }
        else {
            ((CAutoFitTextureView*)mTextureView.Get())->SetAspectRatio(h, w);
        }

        // Check if the flash is supported.
        obj = NULL;
        characteristics->Get(CameraCharacteristics::FLASH_INFO_AVAILABLE, (IInterface**)&obj);
        mFlashSupported = FALSE;
        if (obj != NULL) {
            IBoolean::Probe(obj)->GetValue(&mFlashSupported);
        }

        mCameraId = cameraId;
        return NOERROR;
    }
__EXIT__:
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to SetUpCameraOutputs.");
    }

    return ec;
}

ECode CActivityOne::OpenCamera(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    SetUpCameraOutputs(width, height);
    ConfigureTransform(width, height);

    AutoPtr<ITimeUnitHelper> helper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
    AutoPtr<ITimeUnit> ms;
    helper->GetMILLISECONDS((ITimeUnit**)&ms);
    Boolean bval;
    mCameraOpenCloseLock->TryAcquire(2500, ms, &bval);
    if (!bval) {
        Logger::E(TAG, "Time out waiting to lock camera opening.");
        return E_RUNTIME_EXCEPTION;
    }

    Logger::I(TAG, " >> OpenCamera: %s", mCameraId.string());
    ECode ec = mCameraManager->OpenCamera(mCameraId, mStateCallback, mBackgroundHandler);
    if (FAILED(ec)) {
        Logger::E(TAG, "Interrupted while trying to lock camera opening.");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CActivityOne::CloseCamera()
{
    ECode ec = NOERROR;

    mCameraOpenCloseLock->Acquire();
    if (NULL != mCaptureSession) {
        ec = ICloseable::Probe(mCaptureSession)->Close();
        FAIL_GOTO(ec, _EXIT_)
        mCaptureSession = NULL;
    }
    if (NULL != mCameraDevice) {
        ec = ICloseable::Probe(mCameraDevice)->Close();
        FAIL_GOTO(ec, _EXIT_)
        mCameraDevice = NULL;
    }
    if (NULL != mImageReader) {
        ec = IAutoCloseable::Probe(mImageReader)->Close();
        FAIL_GOTO(ec, _EXIT_)
        mImageReader = NULL;
    }

_EXIT_:
    if (FAILED(ec)) {
        Logger::E(TAG, "Interrupted while trying to lock camera closing.");
        ec = E_RUNTIME_EXCEPTION;
    }

    mCameraOpenCloseLock->ReleasePermit();
    return ec;
}

ECode CActivityOne::StartBackgroundThread()
{
    CHandlerThread::New(String("CameraBackground"), (IHandlerThread**)&mBackgroundThread);
    IThread::Probe(mBackgroundThread)->Start();
    AutoPtr<ILooper> looper;
    mBackgroundThread->GetLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mBackgroundHandler);
    return NOERROR;
}

ECode CActivityOne::StopBackgroundThread()
{
    Boolean bval;
    mBackgroundThread->QuitSafely(&bval);
    IThread::Probe(mBackgroundThread)->Join();
    mBackgroundThread = NULL;
    mBackgroundHandler = NULL;
    return NOERROR;
}

ECode CActivityOne::CreateCameraPreviewSession()
{
    Logger::I(TAG, " >> CreateCameraPreviewSession");

    AutoPtr<ISurfaceTexture> texture;
    mTextureView->GetSurfaceTexture((ISurfaceTexture**)&texture);
    assert(texture != NULL);

    // We configure the size of default buffer to be the size of camera preview we want.
    Int32 w, h;
    mPreviewSize->GetWidth(&w);
    mPreviewSize->GetHeight(&h);
    texture->SetDefaultBufferSize(w, h);

    // This is the output Surface we need to start preview.
    AutoPtr<ISurface> surface;
    CSurface::New(texture, (ISurface**)&surface);

    // We set up a CaptureRequest.Builder with the output Surface.
    mPreviewRequestBuilder = NULL;
    mCameraDevice->CreateCaptureRequest(ICameraDevice::TEMPLATE_PREVIEW,
        (ICaptureRequestBuilder**)&mPreviewRequestBuilder);
    mPreviewRequestBuilder->AddTarget(surface);
    Logger::I(TAG, " >> CreateCaptureRequest: %s", TO_CSTR(mPreviewRequestBuilder));

    // Here, we create a CameraCaptureSession for camera preview.
    AutoPtr<ISurface> irSurface;
    mImageReader->GetSurface((ISurface**)&irSurface);
    AutoPtr<IList> surfaceList;
    CArrayList::New(2, (IList**)&surfaceList);
    surfaceList->Add(surface);
    surfaceList->Add(irSurface);
    AutoPtr<ICameraCaptureSessionStateCallback> cb = new CameraCaptureSessionStateCallback(this);

    mCameraDevice->CreateCaptureSession(surfaceList, cb, NULL);
    return NOERROR;
}

ECode CActivityOne::ConfigureTransform(
    /* [in] */ Int32 viewWidth,
    /* [in] */ Int32 viewHeight)
{
    if (NULL == mTextureView || NULL == mPreviewSize) {
        return NOERROR;
    }

    AutoPtr<IWindowManager> wm = GetWindowManager();
    AutoPtr<IDisplay> display;
    wm->GetDefaultDisplay((IDisplay**)&display);
    Int32 rotation = 0;
    display->GetRotation(&rotation);

    Int32 w, h;
    mPreviewSize->GetWidth(&w);
    mPreviewSize->GetHeight(&h);
    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);
    AutoPtr<IRectF> viewRect, bufferRect;
    CRectF::New(0.0f, 0.0f, viewWidth * 1.0f, viewHeight * 1.0f, (IRectF**)&viewRect);
    CRectF::New(0.0f, 0.0f, w * 1.0f, h * 1.0f, (IRectF**)&bufferRect);

    Float centerX, centerY;
    viewRect->GetCenterX(&centerX);
    viewRect->GetCenterY(&centerY);

    Boolean bval;
    if (ISurface::ROTATION_90 == rotation || ISurface::ROTATION_270 == rotation) {
        Float bx, by;
        bufferRect->GetCenterX(&bx);
        bufferRect->GetCenterY(&by);
        bufferRect->Offset(centerX - bx, centerY - by);
        matrix->SetRectToRect(viewRect, bufferRect, MatrixScaleToFit_FILL, &bval);

        Float scale = Elastos::Core::Math::Max(
            (Float) viewHeight * 1.0f / h,
            (Float) viewWidth * 1.0f / w);
        matrix->PostScale(scale, scale, centerX, centerY, &bval);
        matrix->PostRotate(90 * (rotation - 2), centerX, centerY, &bval);
    }
    else if (ISurface::ROTATION_180 == rotation) {
        matrix->PostRotate(180, centerX, centerY, &bval);
    }

    mTextureView->SetTransform(matrix);
    return NOERROR;
}

ECode CActivityOne::TakePicture()
{
    return LockFocus();
}

ECode CActivityOne::LockFocus()
{
    Logger::I(TAG, " >> LockFocus: %s", TO_CSTR(mPreviewRequestBuilder));

    // This is how to tell the camera to lock focus.
    AutoPtr<IInteger32> iobj = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_TRIGGER_START);
    mPreviewRequestBuilder->Set(CaptureRequest::CONTROL_AF_TRIGGER, iobj);

    // Tell #mCaptureCallback to wait for the lock.
    Int32 id;
    mState = STATE_WAITING_LOCK;
    AutoPtr<ICaptureRequest> request;
    mPreviewRequestBuilder->Build((ICaptureRequest**)&request);
    ECode ec = mCaptureSession->Capture(request, mCaptureCallback, mBackgroundHandler, &id);
    if (FAILED(ec)) {
        Logger::E(TAG, " >> Failed to capture request: %s", TO_CSTR(request));
    }
    return NOERROR;
}

ECode CActivityOne::RunPrecaptureSequence()
{
    Logger::I(TAG, " >> RunPrecaptureSequence: %s", TO_CSTR(mPreviewRequestBuilder));

    // This is how to tell the camera to trigger.
    AutoPtr<IInteger32> iobj = CoreUtils::Convert(ICameraMetadata::CONTROL_AE_PRECAPTURE_TRIGGER_START);
    mPreviewRequestBuilder->Set(CaptureRequest::CONTROL_AE_PRECAPTURE_TRIGGER, iobj);

    // Tell #mCaptureCallback to wait for the precapture sequence to be set.
    Int32 id;
    mState = STATE_WAITING_PRECAPTURE;
    AutoPtr<ICaptureRequest> request;
    mPreviewRequestBuilder->Build((ICaptureRequest**)&request);
    ECode ec = mCaptureSession->Capture(request, mCaptureCallback, mBackgroundHandler, &id);
    if (FAILED(ec)) {
        Logger::E(TAG, " >> Failed to precapture request: %s", TO_CSTR(request));
    }

    return NOERROR;
}

ECode CActivityOne::CaptureStillPicture()
{
    if (NULL == mCameraDevice) {
        return NOERROR;
    }

    Logger::I(TAG, " >> CaptureStillPicture");

    AutoPtr<ISurface> surface;
    mImageReader->GetSurface((ISurface**)&surface);

    // This is the CaptureRequest.Builder that we use to take a picture.
    AutoPtr<ICaptureRequestBuilder> captureBuilder;
    ECode ec = mCameraDevice->CreateCaptureRequest(
        ICameraDevice::TEMPLATE_STILL_CAPTURE,
        (ICaptureRequestBuilder**)&captureBuilder);
    if (FAILED(ec)) {
        Logger::E(TAG, " >> Failed to CreateCaptureRequest, ec=%08x", ec);
        return ec;
    }
    captureBuilder->AddTarget(surface);

    // Use the same AE and AF modes as the preview.
    AutoPtr<IInteger32> iobj = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_PICTURE);
    captureBuilder->Set(CaptureRequest::CONTROL_AF_MODE, iobj);
    SetAutoFlash(captureBuilder);

    // Orientation
    AutoPtr<IWindowManager> wm = GetWindowManager();
    AutoPtr<IDisplay> display;
    wm->GetDefaultDisplay((IDisplay**)&display);
    Int32 rotation = 0;
    display->GetRotation(&rotation);
    iobj = CoreUtils::Convert(GetOrientation(rotation));
    captureBuilder->Set(CaptureRequest::JPEG_ORIENTATION, iobj);

    AutoPtr<ICameraCaptureSessionCaptureCallback> captureCallback = new CaptureStillPictureCallback(this);
    mCaptureSession->StopRepeating();

    Int32 id;
    AutoPtr<ICaptureRequest> request;
    captureBuilder->Build((ICaptureRequest**)&request);
    ec = mCaptureSession->Capture(request, captureCallback, NULL, &id);
    if (FAILED(ec)) {
        Logger::E(TAG, " >> Failed to Capture, ec=%08x", ec);
        return ec;
    }
    return NOERROR;
}

Int32 CActivityOne::GetOrientation(
    /* [in] */ Int32 rotation)
{
    // Sensor orientation is 90 for most devices, or 270 for some devices (eg. Nexus 5X)
    // We have to take that into account and rotate JPEG properly.
    // For devices with orientation of 90, we simply return our mapping from ORIENTATIONS.
    // For devices with orientation of 270, we need to rotate the JPEG 180 degrees.
    Int32 r = (*ORIENTATIONS)[rotation];
    r = (r + mSensorOrientation + 270) % 360;
    return r;
}

ECode CActivityOne::UnlockFocus()
{
    Logger::I(TAG, " >> UnlockFocus");

    // try {
    // Reset the auto-focus trigger
    AutoPtr<IInteger32> iobj = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_TRIGGER_CANCEL);
    mPreviewRequestBuilder->Set(CaptureRequest::CONTROL_AF_TRIGGER, iobj);
    SetAutoFlash(mPreviewRequestBuilder);

    Int32 id;
    AutoPtr<ICaptureRequest> request;
    mPreviewRequestBuilder->Build((ICaptureRequest**)&request);
    ECode ec = NOERROR;
    ec = mCaptureSession->Capture(request, mCaptureCallback, mBackgroundHandler, &id);
    if (FAILED(ec)) {
        Logger::E(TAG, " >> Failed to Capture in UnlockFocus. ec=%08x", ec);
        return NOERROR;
    }

    // After this, the camera will go back to the normal state of preview.
    mState = STATE_PREVIEW;
    mCaptureSession->SetRepeatingRequest(
        mPreviewRequest, mCaptureCallback, mBackgroundHandler, &id);
    return NOERROR;
}

ECode CActivityOne::OnClick(
    /* [in] */ IView* view)
{
    Int32 id;
    view->GetId(&id);
    switch (id) {
        case R::id::picture: {
            TakePicture();
            break;
        }
        case R::id::info: {
            // new AlertDialog.Builder(activity)
            //         .setMessage(R.string.intro_message)
            //         .setPositiveButton(android.R.string.ok, NULL)
            //         .show();
            break;
        }
    }
    return NOERROR;
}

ECode CActivityOne::SetAutoFlash(
    /* [in] */ ICaptureRequestBuilder* requestBuilder)
{
    if (mFlashSupported) {
        AutoPtr<IInteger32> iobj = CoreUtils::Convert(ICameraMetadata::CONTROL_AE_MODE_ON_AUTO_FLASH);
        requestBuilder->Set(CaptureRequest::CONTROL_AE_MODE, iobj);
    }
    return NOERROR;
}

// /**
//  * Shows an error message dialog.
//  */
// public static class ErrorDialog extends DialogFragment {

//     private static final String ARG_MESSAGE = "message";

//     public static ErrorDialog newInstance(String message) {
//         ErrorDialog dialog = new ErrorDialog();
//         Bundle args = new Bundle();
//         args.putString(ARG_MESSAGE, message);
//         dialog.setArguments(args);
//         return dialog;
//     }

//     @Override
//     public Dialog onCreateDialog(Bundle savedInstanceState) {
//         final Activity activity = getActivity();
//         return new AlertDialog.Builder(activity)
//                 .setMessage(getArguments().getString(ARG_MESSAGE))
//                 .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
//                     @Override
//                     public void onClick(DialogInterface dialogInterface, Int32 i) {
//                         activity.finish();
//                     }
//                 })
//                 .create();
//     }

// }

// /**
//  * Shows OK/Cancel confirmation dialog about camera permission.
//  */
// public static class ConfirmationDialog extends DialogFragment {

//     @Override
//     public Dialog onCreateDialog(Bundle savedInstanceState) {
//         final Fragment parent = getParentFragment();
//         return new AlertDialog.Builder(getActivity())
//                 .setMessage(R.string.request_permission)
//                 .setPositiveButton(android.R.string.ok, new DialogInterface.OnClickListener() {
//                     @Override
//                     public void onClick(DialogInterface dialog, Int32 which) {
//                         FragmentCompat.requestPermissions(parent,
//                                 new String[]{Manifest.permission.CAMERA},
//                                 REQUEST_CAMERA_PERMISSION);
//                     }
//                 })
//                 .setNegativeButton(android.R.string.cancel,
//                         new DialogInterface.OnClickListener() {
//                             @Override
//                             public void onClick(DialogInterface dialog, Int32 which) {
//                                 Activity activity = parent.getActivity();
//                                 if (activity != NULL) {
//                                     activity.finish();
//                                 }
//                             }
//                         })
//                 .create();
//     }
// }

} // namespace CameraDemo
} // namespace DevSamples
} // namespace Elastos
