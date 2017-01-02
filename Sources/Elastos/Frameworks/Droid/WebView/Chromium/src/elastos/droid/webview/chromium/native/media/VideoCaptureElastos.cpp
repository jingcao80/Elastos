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
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCaptureElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/media/ImageFormat.h"
#include "elastos/droid/os/Build.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::CImageFormat;
using Elastos::Droid::Hardware::ICameraSize;
using Elastos::Droid::Hardware::IHardwareCameraHelper;
using Elastos::Droid::Hardware::CHardwareCameraHelper;
using Elastos::Droid::Hardware::CHardwareCameraSize;

using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Concurrent::Locks::EIID_ILock;
using Elastos::Utility::IListIterator;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Droid::Os::Build;
using Elastos::Core::IInteger32;
//using Elastos::Core::IObjectContainer;
//using Elastos::Core::IObjectEnumerator;
using Elastos::Core::CInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

//===============================================================
//       VideoCaptureElastos::BuggyDeviceHack::IdAndSizes
//===============================================================

VideoCaptureElastos::BuggyDeviceHack::IdAndSizes::IdAndSizes(
    /* [in] */ const String& model,
    /* [in] */ const String& device,
    /* [in] */ Int32 minWidth,
    /* [in] */ Int32 minHeight)
    : mModel(model)
    , mDevice(device)
    , mMinWidth(minWidth)
    , mMinHeight(minHeight)
{
}

//===============================================================
//              VideoCaptureElastos::BuggyDeviceHack
//===============================================================

AutoPtr<ArrayOf<VideoCaptureElastos::BuggyDeviceHack::IdAndSizes*> > VideoCaptureElastos::s_CAPTURESIZE_BUGGY_DEVICE_LIST_Init()
{
    AutoPtr<BuggyDeviceHack::IdAndSizes> idAndSizes = new BuggyDeviceHack::IdAndSizes(String("Nexus 7"), String("flo"), 640, 480);
    AutoPtr<ArrayOf<BuggyDeviceHack::IdAndSizes*> > array = ArrayOf<BuggyDeviceHack::IdAndSizes*>::Alloc(1);
    array->Set(0, idAndSizes);
    return array;
}

static AutoPtr< ArrayOf<String> > s_COLORSPACE_BUGGY_DEVICE_LIST_Init()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = String("SAMSUNG-SGH-I747");
    (*array)[1] = String("ODROID-U2");
    return array;
}

const AutoPtr<ArrayOf<VideoCaptureElastos::BuggyDeviceHack::IdAndSizes*> > VideoCaptureElastos::BuggyDeviceHack::s_CAPTURESIZE_BUGGY_DEVICE_LIST = s_CAPTURESIZE_BUGGY_DEVICE_LIST_Init();
const AutoPtr< ArrayOf<String> > VideoCaptureElastos::BuggyDeviceHack::s_COLORSPACE_BUGGY_DEVICE_LIST = s_COLORSPACE_BUGGY_DEVICE_LIST_Init();

void VideoCaptureElastos::BuggyDeviceHack::ApplyMinDimensions(
    /* [in] */ CaptureFormat* format)
{
    // NOTE: this can discard requested aspect ratio considerations.
    Int32 length = s_CAPTURESIZE_BUGGY_DEVICE_LIST->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IdAndSizes> buggyDevice = (*s_CAPTURESIZE_BUGGY_DEVICE_LIST)[i];
        if (buggyDevice->mModel.Equals(Build::MODEL) &&
                buggyDevice->mDevice.Equals(Build::DEVICE)) {
            format->mWidth = (buggyDevice->mMinWidth > format->mWidth)
                    ? buggyDevice->mMinWidth : format->mWidth;
            format->mHeight = (buggyDevice->mMinHeight > format->mHeight)
                    ? buggyDevice->mMinHeight : format->mHeight;
        }
    }
}

Int32 VideoCaptureElastos::BuggyDeviceHack::GetImageFormat()
{
    if (Build::VERSION::SDK_INT < Build::VERSION_CODES::JELLY_BEAN) {
        return IImageFormat::NV21;
    }

    Int32 length = s_COLORSPACE_BUGGY_DEVICE_LIST->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        String buggyDevice = (*s_COLORSPACE_BUGGY_DEVICE_LIST)[i];
        if (buggyDevice.Equals(Build::MODEL)) {
            return IImageFormat::NV21;
        }
    }

    return IImageFormat::YV12;
}

//===============================================================
//                    VideoCaptureElastos
//===============================================================

const Int32 VideoCaptureElastos::NUM_CAPTURE_BUFFERS;
const String VideoCaptureElastos::TAG("VideoCaptureElastos");

VideoCaptureElastos::VideoCaptureElastos(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [in] */ Int64 nativeVideoCaptureDeviceAndroid)
    : VideoCapture(context, id, nativeVideoCaptureDeviceAndroid)
    , mExpectedFrameSize(0)
{
}

AutoPtr<ArrayOf<VideoCapture::CaptureFormat*> > VideoCaptureElastos::GetDeviceSupportedFormats(
    /* [in] */ Int32 id)
{
    AutoPtr<IHardwareCamera> camera;
    // try {
        AutoPtr<IHardwareCameraHelper> helper;
        CHardwareCameraHelper::AcquireSingleton((IHardwareCameraHelper**)&helper);
        helper->Open(id, (IHardwareCamera**)&camera);
    // } catch (RuntimeException ex) {
    //     Log.e(TAG, "Camera.open: " + ex);
    //     return null;
    // }

    AutoPtr<IParameters> parameters = GetCameraParameters(camera);
    if (parameters == NULL) {
        return NULL;
    }

    //ArrayList<CaptureFormat> formatList = new ArrayList<CaptureFormat>();
    AutoPtr<ArrayOf<CaptureFormat*> > formatList;
    // getSupportedPreview{Formats,FpsRange,PreviewSizes}() returns Lists
    // with at least one element, but when the camera is in bad state, they
    // can return null pointers; in that case we use a 0 entry, so we can
    // retrieve as much information as possible.
    AutoPtr<ArrayOf<IInteger32*> > pixelFormats;
    //List<Integer> pixelFormats = parameters->GetSupportedPreviewFormats();
    parameters->GetSupportedPreviewFormats((ArrayOf<IInteger32*>**)&pixelFormats);
    if ((pixelFormats == NULL) || (pixelFormats->GetLength() == 0)) {
        //pixelFormats = new ArrayList<Integer>();
        pixelFormats = ArrayOf<IInteger32*>::Alloc(1);
        AutoPtr<IInteger32> obj;
        CInteger32::New(IImageFormat::UNKNOWN, (IInteger32**)&obj);
        pixelFormats->Set(0, obj.Get());

    }
    //if (pixelFormats->GetLength() == 0) {
    //    //pixelFormats.add(ImageFormat.UNKNOWN);
    //}
    //for (Integer previewFormat : pixelFormats)
    for (Int32 i = 0; i < pixelFormats->GetLength(); ++i) {
        AutoPtr<IInteger32> pPreviewFormat = (*pixelFormats)[i];
        Int32 previewFormat;
        pPreviewFormat->GetValue(&previewFormat);
        int pixelFormat = AndroidImageFormatList::ANDROID_IMAGEFORMAT_UNKNOWN;
        if (previewFormat == IImageFormat::YV12) {
            pixelFormat = AndroidImageFormatList::ANDROID_IMAGEFORMAT_YV12;
        } else if (previewFormat == IImageFormat::NV21) {
            continue;
        }

        AutoPtr<IList> listFpsRange;
        //List<int[]> listFpsRange = parameters->GetSupportedPreviewFpsRange();
        parameters->GetSupportedPreviewFpsRange((IList**)&listFpsRange);
        if (listFpsRange == NULL) {
            //listFpsRange = new ArrayList<int[]>();
            AutoPtr<IArrayList> arrayList;
            CArrayList::New((IArrayList**)&arrayList);
            listFpsRange = IList::Probe(arrayList);
        }
        Int32 count;
        listFpsRange->GetSize(&count);
        if (count == 0) {
            //listFpsRange->Add(new int[] {0, 0});
            //TODO should check the implementation of GetSupportedPreviewFpsRange
            // now not implemented
            AutoPtr<IArrayList> _range;
            CArrayList::New((IArrayList**)&_range);

            AutoPtr<IInteger32> obj;
            CInteger32::New(0, (IInteger32**)&obj);
            _range->Add(obj.Get());

            obj = NULL;
            CInteger32::New(0, (IInteger32**)&obj);
            _range->Add(obj.Get());

            listFpsRange->Add(_range);
            count = 1;
        }
        //for (int[] fpsRange : listFpsRange)
        for(Int32 i = 0; i < count; ++i)
        {
            AutoPtr<IInterface> fpsRangeObj;
            listFpsRange->Get(i, (IInterface**)&fpsRangeObj);
            IArrayList* fpsRange = IArrayList::Probe(fpsRangeObj);
            AutoPtr<IInterface> range_0Obj;
            fpsRange->Get(0, (IInterface**)&range_0Obj);
            IInteger32* range_0 = IInteger32::Probe(range_0Obj);
            Int32 value_range_0;
            range_0->GetValue(&value_range_0);

            //List<Camera.Size> supportedSizes = parameters.GetSupportedPreviewSizes();
            AutoPtr<ArrayOf<ICameraSize*> > supportedSizes;
            parameters->GetSupportedPreviewSizes((ArrayOf<ICameraSize*>**)&supportedSizes);
            if ((supportedSizes == NULL)||(supportedSizes->GetLength() == 0)) {
                //supportedSizes = new ArrayList<Camera.Size>();
                supportedSizes = ArrayOf<ICameraSize*>::Alloc(1);
                AutoPtr<ICameraSize> cameraSize;
                CHardwareCameraSize::New(0, 0, (ICameraSize**)&cameraSize);
                supportedSizes->Set(0, cameraSize.Get());
            }
            //if (supportedSizes.size() == 0) {
            //    supportedSizes.add(camera.new Size(0, 0));
            //}
            //for (Camera.Size size : supportedSizes)
            Int32 count_size = supportedSizes->GetLength();
            formatList = ArrayOf<CaptureFormat*>::Alloc(count_size);
            for(Int32 i = 0; i < count_size; ++i)
            {
                AutoPtr<ICameraSize> size = (*supportedSizes)[i];
                //TODO if ICameraSize and the interface to get width/heigth
                //below cast should be removed
                Int32 width = 0;
                Int32 height = 0;
                size->GetWidth(&width);
                size->GetHeight(&height);
                //formatList.add(new CaptureFormat
                formatList->Set(i, new VideoCapture::CaptureFormat(width,
                                                 height,
                                                 (value_range_0 + 999) / 1000,
                                                 pixelFormat));
            }
        }
    }

    camera->Release();

    //return formatList.toArray(new CaptureFormat[formatList.size()]);
    return formatList;
}


//@Override
void VideoCaptureElastos::SetCaptureParameters(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 frameRate,
    /* [in] */ IParameters* cameraParameters)
{
    mCaptureFormat = new VideoCapture::CaptureFormat(
            width, height, frameRate, BuggyDeviceHack::GetImageFormat());
    // Hack to avoid certain capture resolutions under a minimum one,
    // see http://crbug.com/305294.
    BuggyDeviceHack::ApplyMinDimensions(mCaptureFormat);
}

//@Override
void VideoCaptureElastos::AllocateBuffers()
{
    AutoPtr<IImageFormat> format;
    CImageFormat::AcquireSingleton((IImageFormat**)&format);
    Int32 bits;
    format->GetBitsPerPixel(mCaptureFormat->mPixelFormat, &bits);
    mExpectedFrameSize = mCaptureFormat->mWidth * mCaptureFormat->mHeight * bits / 8;

    for (Int32 i = 0; i < NUM_CAPTURE_BUFFERS; i++) {
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(mExpectedFrameSize);
        mCamera->AddCallbackBuffer(buffer);
    }
}

//@Override
void VideoCaptureElastos::SetPreviewCallback(
    /* [in] */ IPreviewCallback* cb)
{
    mCamera->SetPreviewCallbackWithBuffer(cb);
}

//@Override
ECode VideoCaptureElastos::OnPreviewFrame(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IHardwareCamera* camera)
{
    //mPreviewBufferLock.lock();
    AutoPtr<ILock> lock = ILock::Probe(mPreviewBufferLock);
    lock->Lock();
    // try {
        if (!mIsRunning) {
            lock->UnLock();
            return NOERROR;
        }

        if (data->GetLength() == mExpectedFrameSize) {
            Int32 rotation = GetDeviceOrientation();
            if (rotation != mDeviceOrientation) {
                mDeviceOrientation = rotation;
            }
            if (mCameraFacing == IHardwareCameraInfo::CAMERA_FACING_BACK) {
                rotation = 360 - rotation;
            }
            rotation = (mCameraOrientation + rotation) % 360;
            NativeOnFrameAvailable(mNativeVideoCaptureDeviceAndroid,
                    data, mExpectedFrameSize, rotation);
        }
    // } finally {
        //mPreviewBufferLock.unlock();
        lock->UnLock();
        if (camera != NULL) {
            camera->AddCallbackBuffer(data);
        }
    // }
    return NOERROR;
}

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
