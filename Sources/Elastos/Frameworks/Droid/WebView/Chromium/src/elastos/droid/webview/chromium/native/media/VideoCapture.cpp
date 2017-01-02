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

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Opengl.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCapture.h"
#include "elastos/droid/webkit/webview/chromium/native/media/api/VideoCapture_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/media/ImageFormat.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Hardware::ICameraSize;
using Elastos::Droid::Hardware::CHardwareCamera;
using Elastos::Droid::Hardware::EIID_IPreviewCallback;
using Elastos::Droid::Hardware::IHardwareCameraHelper;
using Elastos::Droid::Hardware::CHardwareCameraHelper;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::Opengl::CGLES20;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IWindowManager;

using Elastos::Core::Math;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Concurrent::Locks::EIID_ILock;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

//===============================================================
//                 VideoCapture::CaptureFormat
//===============================================================

VideoCapture::CaptureFormat::CaptureFormat(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 framerate,
    /* [in] */ Int32 pixelformat)
    : mWidth(width)
    , mHeight(height)
    , mFramerate(framerate)
    , mPixelFormat(pixelformat)
{
}

Int32 VideoCapture::CaptureFormat::GetWidth()
{
    return mWidth;
}

Int32 VideoCapture::CaptureFormat::GetHeight()
{
    return mHeight;
}

Int32 VideoCapture::CaptureFormat::GetFramerate()
{
    return mFramerate;
}

Int32 VideoCapture::CaptureFormat::GetPixelFormat()
{
    return mPixelFormat;
}

//===============================================================
//                         VideoCapture
//===============================================================

const Int32 VideoCapture::GL_TEXTURE_EXTERNAL_OES;
const String VideoCapture::TAG("VideoCapture");
CAR_INTERFACE_IMPL(VideoCapture, Object, IPreviewCallback)

// Lock to mutually exclude execution of OnPreviewFrame {start/stop}Capture.

VideoCapture::VideoCapture(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [in] */ Handle64 nativeVideoCaptureDeviceAndroid)
    : mContext(context)
    , mIsRunning(FALSE)
    , mId(id)
    , mNativeVideoCaptureDeviceAndroid(nativeVideoCaptureDeviceAndroid)
    , mCameraOrientation(0)
    , mCameraFacing(0)
    , mDeviceOrientation(0)
{

    CReentrantLock::New((IReentrantLock**)&mPreviewBufferLock);
}

//@CalledByNative
Boolean VideoCapture::Allocate(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 frameRate)
{
    Logger::D(TAG, "allocate: requested ( %d x %d)@%dfps",  width, height, frameRate);
    // try {
    AutoPtr<IHardwareCameraHelper> helper;
    CHardwareCameraHelper::AcquireSingleton((IHardwareCameraHelper**)&helper);
    helper->Open(mId, (IHardwareCamera**)&mCamera);
    // } catch (RuntimeException ex) {
    //     Log.e(TAG, "allocate: Camera.open: " + ex);
    //     return false;
    // }

    AutoPtr<IHardwareCameraInfo> cameraInfo = GetCameraInfo(mId);
    if (cameraInfo == NULL) {
        mCamera->Release();
        mCamera = NULL;
        return FALSE;
    }

    cameraInfo->GetOrientation(&mCameraOrientation);
    cameraInfo->GetFacing(&mCameraFacing);
    mDeviceOrientation = GetDeviceOrientation();
    Logger::D(TAG, "allocate: orientation dev=%d, cam=%d, facing=%d", mDeviceOrientation, mCameraOrientation, mCameraFacing);

    AutoPtr<IParameters> parameters = GetCameraParameters(mCamera);
    if (parameters == NULL) {
        mCamera = NULL;
        return FALSE;
    }

    // getSupportedPreviewFpsRange() returns a List with at least one
    // element, but when camera is in bad state, it can return null pointer.
    AutoPtr<IArrayList> listRange;
    CArrayList::New((IArrayList**)&listRange);
    //List<int[]> listFpsRange = parameters->GetSupportedPreviewFpsRange();
    AutoPtr<ArrayOf<Int32> > fpsMinMax;
    /*TODO the interface GetSupportedPreviewFpsRange is not stable
    parameters->GetSupportedPreviewFpsRange((IArrayList**)&listRange);
    Boolean emptyList;
    listFpsRange->IsEmpty(&emptyList);
    if (listFpsRange == NULL || emptyList) {
        Logger::E(TAG, "allocate: no fps range found");
        return FALSE;
    }

    Int32 frameRateInMs = frameRate * 1000;
    // Use the first range as default.
    AutoPtr<List<ArrayOf<Int32> > >::Iterator fpsRange = listFpsRange->Begin();
    fpsMinMax = *fpsRange;
    Int32 newFrameRate = ((*fpsMinMax)[0] + 999) / 1000;
    //for (int[] fpsRange : listFpsRange)
    for (; it != infos->End(); ++it) {

        if ((*fpsRange)[0] <= frameRateInMs && frameRateInMs <= (*fpsRange)[1]) {
            fpsMinMax = fpsRange;
            newFrameRate = frameRate;
            break;
        }
    }

    frameRate = newFrameRate;
    */
    Logger::D(TAG, "allocate: fps set to %d", frameRate);

    // Calculate size.
    AutoPtr<ArrayOf<ICameraSize*> > listCameraSize;
    parameters->GetSupportedPreviewSizes((ArrayOf<ICameraSize*>**)&listCameraSize);
    Int32 minDiff = Elastos::Core::Math::INT32_MAX_VALUE;
    Int32 matchedWidth = width;
    Int32 matchedHeight = height;

    AutoPtr<ICameraSize> size;
    Int32 length = listCameraSize->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        size = (*listCameraSize)[i];
        Int32 _width = 0, _height = 0;
        size->GetWidth(&_width);
        size->GetHeight(&_height);
        Int32 diff = Elastos::Core::Math::Abs(_width - width) +
                   Elastos::Core::Math::Abs(_height - height);
        Logger::D(TAG, "allocate: supported (%d, %d), diff=%d",  _width, _height, diff);
        // TODO(wjia): Remove this hack (forcing width to be multiple
        // of 32) by supporting stride in video frame buffer.
        // Right now, VideoCaptureController requires compact YV12
        // (i.e., with no padding).
        if (diff < minDiff && (_width % 32 == 0)) {
            minDiff = diff;
            matchedWidth = _width;
            matchedHeight = _height;
        }
    }

    if (minDiff == Elastos::Core::Math::INT32_MAX_VALUE) {
        Logger::E(TAG, "allocate: can not find a multiple-of-32 resolution");
        return FALSE;
    }

    Logger::D(TAG, "allocate: matched (%d x %d)",  matchedWidth, matchedHeight);

    Boolean isVideoStabilizationSupported;
    parameters->IsVideoStabilizationSupported(&isVideoStabilizationSupported);
    if (isVideoStabilizationSupported) {
        Boolean enabled;
        parameters->GetVideoStabilization(&enabled);
        Logger::D(TAG, "Image stabilization supported, currently: %d, setting it.", enabled);
        parameters->SetVideoStabilization(TRUE);
    }
    else {
        Logger::D(TAG, "Image stabilization not supported.");
    }

    SetCaptureParameters(matchedWidth, matchedHeight, frameRate, parameters);
    parameters->SetPreviewSize(mCaptureFormat->mWidth,
                              mCaptureFormat->mHeight);
    parameters->SetPreviewFpsRange((*fpsMinMax)[0], (*fpsMinMax)[1]);
    parameters->SetPreviewFormat(mCaptureFormat->mPixelFormat);
    mCamera->SetParameters(parameters);

    // Set SurfaceTexture. Android Capture needs a SurfaceTexture even if
    // it is not going to be used.
    mGlTextures = ArrayOf<Int32>::Alloc(1);

    // Generate one texture pointer and bind it as an external texture.
    AutoPtr<IGLES20> gles20;
    CGLES20::AcquireSingleton((IGLES20**)&gles20);
    gles20->GlGenTextures(1, mGlTextures, 0);
    gles20->GlBindTexture(GL_TEXTURE_EXTERNAL_OES, (*mGlTextures)[0]);
    // No mip-mapping with camera source.
    gles20->GlTexParameterf(GL_TEXTURE_EXTERNAL_OES,
            IGLES20::_GL_TEXTURE_MIN_FILTER, IGLES20::_GL_LINEAR);
    gles20->GlTexParameterf(GL_TEXTURE_EXTERNAL_OES,
            IGLES20::_GL_TEXTURE_MAG_FILTER, IGLES20::_GL_LINEAR);
    // Clamp to edge is only option.
    gles20->GlTexParameteri(GL_TEXTURE_EXTERNAL_OES,
            IGLES20::_GL_TEXTURE_WRAP_S, IGLES20::_GL_CLAMP_TO_EDGE);
    gles20->GlTexParameteri(GL_TEXTURE_EXTERNAL_OES,
            IGLES20::_GL_TEXTURE_WRAP_T, IGLES20::_GL_CLAMP_TO_EDGE);

    CSurfaceTexture::New((*mGlTextures)[0], (ISurfaceTexture**)&mSurfaceTexture);
    mSurfaceTexture->SetOnFrameAvailableListener(NULL);
    // try {
        mCamera->SetPreviewTexture(mSurfaceTexture);
    // } catch (IOException ex) {
    //     Log.e(TAG, "allocate: " + ex);
    //     return false;
    // }

    AllocateBuffers();

    return TRUE;
}

//@CalledByNative
Int32 VideoCapture::StartCapture()
{
    if (mCamera == NULL) {
        Logger::E(TAG, "startCapture: camera is null");
        return -1;
    }

    AutoPtr<ILock> lock = ILock::Probe(mPreviewBufferLock);
    lock->Lock();
    //try {
        if (mIsRunning) {
            lock->UnLock();
            return 0;
        }
        mIsRunning = TRUE;
    //} finally {
        lock->UnLock();
    //}
    SetPreviewCallback(this);
    // try {
        mCamera->StartPreview();
    // } catch (RuntimeException ex) {
    //     Log.e(TAG, "startCapture: Camera.startPreview: " + ex);
    //     return -1;
    // }

    return 0;
}

//@CalledByNative
Int32 VideoCapture::StopCapture()
{
    if (mCamera == NULL) {
        Logger::E(TAG, "stopCapture: camera is null");
        return 0;
    }

    AutoPtr<ILock> lock = ILock::Probe(mPreviewBufferLock);
    lock->Lock();
    //try {
        if (!mIsRunning) {
            lock->UnLock();
            return 0;
        }
        mIsRunning = FALSE;
    //} finally {
        lock->UnLock();
    //}

    mCamera->StopPreview();
    SetPreviewCallback(NULL);

    return 0;
}

//@CalledByNative
void VideoCapture::Deallocate()
{
    if (mCamera == NULL)
        return;

    StopCapture();
    // try {
        mCamera->SetPreviewTexture(NULL);
        if (mGlTextures != NULL) {
            AutoPtr<IGLES20> gles20;
            CGLES20::AcquireSingleton((IGLES20**)&gles20);
            gles20->GlDeleteTextures(1, mGlTextures, 0);
        }
        mCaptureFormat = NULL;
        mCamera->Release();
        mCamera = NULL;
    // } catch (IOException ex) {
    //     Log.e(TAG, "deallocate: failed to deallocate camera, " + ex);
    //     return;
    // }
}

//@CalledByNative
Int32 VideoCapture::QueryWidth()
{
    return mCaptureFormat->mWidth;
}

//@CalledByNative
Int32 VideoCapture::QueryHeight()
{
    return mCaptureFormat->mHeight;
}

//@CalledByNative
Int32 VideoCapture::QueryFrameRate()
{
    return mCaptureFormat->mFramerate;
}

//@CalledByNative
Int32 VideoCapture::GetColorspace()
{
    switch (mCaptureFormat->GetPixelFormat()) {
        case IImageFormat::YV12:
            return AndroidImageFormatList::ANDROID_IMAGEFORMAT_YV12;
        case IImageFormat::NV21:
            return AndroidImageFormatList::ANDROID_IMAGEFORMAT_NV21;
        case IImageFormat::UNKNOWN:
        default:
            return AndroidImageFormatList::ANDROID_IMAGEFORMAT_UNKNOWN;
    }
}

Int32 VideoCapture::GetDeviceOrientation()
{
    Int32 orientation = 0;
    if (mContext != NULL) {
        AutoPtr<IWindowManager> wm;
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(
                IContext::WINDOW_SERVICE,
                (IInterface**)&obj);
        wm = IWindowManager::Probe(obj);

        AutoPtr<IDisplay> display;
        wm->GetDefaultDisplay((IDisplay**)&display);
        Int32 rotation;
        display->GetRotation(&rotation);
        switch(rotation) {
            case ISurface::ROTATION_90:
                orientation = 90;
                break;
            case ISurface::ROTATION_180:
                orientation = 180;
                break;
            case ISurface::ROTATION_270:
                orientation = 270;
                break;
            case ISurface::ROTATION_0:
            default:
                orientation = 0;
                break;
        }
    }

    return orientation;
}

// Method for VideoCapture implementations to call back native code.
void VideoCapture::NativeOnFrameAvailable(
    /* [in] */ Handle64 nativeVideoCaptureDeviceAndroid,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length,
    /* [in] */ Int32 rotation)
{
    Elastos_VideoCapture_nativeOnFrameAvailable(TO_IINTERFACE(this), nativeVideoCaptureDeviceAndroid, data,length, rotation);
}

AutoPtr<IParameters> VideoCapture::GetCameraParameters(
    /* [in] */ IHardwareCamera* camera)
{
    AutoPtr<IParameters> parameters;
    // try {
        camera->GetParameters((IParameters**)&parameters);
    // } catch (RuntimeException ex) {
    //     Log.e(TAG, "getCameraParameters: Camera.getParameters: " + ex);
    //     camera.release();
    //     return null;
    // }

    return parameters;
}

AutoPtr<IHardwareCameraInfo> VideoCapture::GetCameraInfo(
    /* [in] */ Int32 id)
{
    AutoPtr<IHardwareCameraInfo> cameraInfo;
    //TODO: CHardwareCamera::CameraInfo::New((IHardwareCameraInfo**)&cameraInfo);
    // try {
        AutoPtr<IHardwareCameraHelper> helper;
        CHardwareCameraHelper::AcquireSingleton((IHardwareCameraHelper**)&helper);
        helper->GetCameraInfo(id, cameraInfo);
    // } catch (RuntimeException ex) {
    //     Log.e(TAG, "getCameraInfo: Camera.getCameraInfo: " + ex);
    //     return null;
    // }

    return cameraInfo;
}
//callback function definition
Boolean VideoCapture::Allocate(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 frameRate)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::Allocate, mObj is NULL");
        return FALSE;
    }
    return mObj->Allocate(width, height, frameRate);
}

Int32 VideoCapture::StartCapture(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::StartCapture, mObj is NULL");
        return -1;
    }
    return mObj->StartCapture();
}

Int32 VideoCapture::StopCapture(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::StopCapture, mObj is NULL");
        return 0;
    }
    return mObj->StopCapture();
}

void VideoCapture::Deallocate(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::Deallocate, mObj is NULL");
        return;
    }
    mObj->Deallocate();
}

Int32 VideoCapture::QueryWidth(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::QueryWidth, mObj is NULL");
        return 0;
    }
    return mObj->QueryWidth();
}

Int32 VideoCapture::QueryHeight(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::QueryHeight, mObj is NULL");
        return 0;
    }
    return mObj->QueryHeight();
}

Int32 VideoCapture::QueryFrameRate(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::QueryFrameRate, mObj is NULL");
        return 0;
    }
    return mObj->QueryFrameRate();
}

Int32 VideoCapture::GetColorspace(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCapture> mObj = (VideoCapture*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCapture::GetColorspace, mObj is NULL");
        return AndroidImageFormatList::ANDROID_IMAGEFORMAT_UNKNOWN;
    }
    return mObj->GetColorspace();
}

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
