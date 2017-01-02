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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCaptureTango.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Hardware::IHardwareCamera;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Concurrent::Locks::EIID_ILock;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IBuffer;
using Elastos::IO::EIID_IBuffer;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

AutoPtr< ArrayOf<VideoCaptureFactory::CamParams*> > VideoCaptureTango::s_CAM_PARAMS_Init()
{
    AutoPtr< ArrayOf<VideoCaptureFactory::CamParams*> > array = ArrayOf<VideoCaptureFactory::CamParams*>::Alloc(3);
    AutoPtr<VideoCaptureFactory::CamParams> item0 = new VideoCaptureFactory::CamParams(VideoCaptureTango::DEPTH_CAMERA_ID, String("depth"), 320, 240);
    AutoPtr<VideoCaptureFactory::CamParams> item1 = new VideoCaptureFactory::CamParams(VideoCaptureTango::FISHEYE_CAMERA_ID, String("fisheye"), 640, 480);
    AutoPtr<VideoCaptureFactory::CamParams> item2 = new VideoCaptureFactory::CamParams(VideoCaptureTango::FOURMP_CAMERA_ID, String("4MP"), 1280, 720);
    array->Set(0, item0);
    array->Set(1, item1);
    array->Set(2, item2);
    return array;
}

// The indexes must coincide with the s_CAM_PARAMS used below.
const Int32 VideoCaptureTango::DEPTH_CAMERA_ID;
const Int32 VideoCaptureTango::FISHEYE_CAMERA_ID;
const Int32 VideoCaptureTango::FOURMP_CAMERA_ID;
const AutoPtr< ArrayOf<VideoCaptureFactory::CamParams*> > VideoCaptureTango::s_CAM_PARAMS = VideoCaptureTango::s_CAM_PARAMS_Init();

// SuperFrame size definitions. Note that total size is the amount of lines
// multiplied by 3/2 due to Chroma components following.
const Int32 VideoCaptureTango::SF_WIDTH;
const Int32 VideoCaptureTango::SF_HEIGHT;
const Int32 VideoCaptureTango::SF_FULL_HEIGHT;
const Int32 VideoCaptureTango::SF_LINES_HEADER;
const Int32 VideoCaptureTango::SF_LINES_FISHEYE;
const Int32 VideoCaptureTango::SF_LINES_RESERVED;
const Int32 VideoCaptureTango::SF_LINES_DEPTH;
const Int32 VideoCaptureTango::SF_LINES_DEPTH_PADDED;
const Int32 VideoCaptureTango::SF_LINES_BIGIMAGE;
const Int32 VideoCaptureTango::SF_OFFSET_4MP_CHROMA;

const Byte VideoCaptureTango::CHROMA_ZERO_LEVEL = 127;
const String VideoCaptureTango::TAG("VideoCaptureTango");

VideoCaptureTango::VideoCaptureTango(
  /* [in] */ IContext* context,
  /* [in] */ Int32 id,
  /* [in] */ Int64 nativeVideoCaptureDeviceAndroid)
  : VideoCapture(context, 0, nativeVideoCaptureDeviceAndroid)
  , mTangoCameraId(id)
{
}

Int32 VideoCaptureTango::NumberOfCameras()
{
    return s_CAM_PARAMS->GetLength();
}

AutoPtr<VideoCaptureFactory::CamParams> VideoCaptureTango::GetCamParams(
  /* [in] */ Int32 index)
{
    if (index >= s_CAM_PARAMS->GetLength()) return NULL;
    return (*s_CAM_PARAMS)[index];
}

AutoPtr< ArrayOf<VideoCapture::CaptureFormat*> > VideoCaptureTango::GetDeviceSupportedFormats(
  /* [in] */ Int32 id)
{
  //ArrayList<CaptureFormat> formatList = new ArrayList<CaptureFormat>();
  AutoPtr<ArrayOf<CaptureFormat*> > result;
  if (id == DEPTH_CAMERA_ID) {
      //formatList->add(new CaptureFormat(320, 180, 5, ImageFormat.YV12));
      result = ArrayOf<CaptureFormat*>::Alloc(1);
      result->Set(0, new CaptureFormat(320, 180, 5, IImageFormat::YV12));
  }
  else if (id == FISHEYE_CAMERA_ID) {
      result = ArrayOf<CaptureFormat*>::Alloc(1);
      //formatList->add(new CaptureFormat(640, 480, 30, ImageFormat.YV12));
      result->Set(0, new CaptureFormat(640, 480, 30, IImageFormat::YV12));
  }
  else if (id == FOURMP_CAMERA_ID) {
      result = ArrayOf<CaptureFormat*>::Alloc(1);
      //formatList->add(new CaptureFormat(1280, 720, 20, ImageFormat.YV12));
      result->Set(0, new CaptureFormat(1280, 720, 20, IImageFormat::YV12));
  }

  //return formatList->ToArray(new CaptureFormat[formatList.size()]);
  return result;
}

//@Override
void VideoCaptureTango::SetCaptureParameters(
  /* [in] */ Int32 width,
  /* [in] */ Int32 height,
  /* [in] */ Int32 frameRate,
  /* [in] */ IParameters* cameraParameters)
{
  mCaptureFormat = new CaptureFormat((*s_CAM_PARAMS)[mTangoCameraId]->mWidth,
                                     (*s_CAM_PARAMS)[mTangoCameraId]->mHeight,
                                     frameRate,
                                     IImageFormat::YV12);
  // Connect Tango SuperFrame mode. Available sf modes are "all",
  // "big-rgb", "small-rgb", "depth", "ir".
    cameraParameters->Set(String("sf-mode"), String("all"));
}

//@Override
void VideoCaptureTango::AllocateBuffers()
{
    AutoPtr<IByteBufferHelper> helper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&helper);
    helper->AllocateDirect(
            mCaptureFormat->mWidth * mCaptureFormat->mHeight * 3 / 2,
            (IByteBuffer**)&mFrameBuffer);
    // Prefill Chroma to their zero-equivalent for the cameras that only
    // provide Luma component.
    AutoPtr<ArrayOf<Byte> > arrayByte;
    mFrameBuffer->GetArray((ArrayOf<Byte>**)&arrayByte);
    //Arrays.fill(mFrameBuffer.array(), CHROMA_ZERO_LEVEL);
    Int32 length = arrayByte->GetLength();
    for(Int32 i = 0; i < length; ++i) {
        (*arrayByte)[i] = CHROMA_ZERO_LEVEL;
    }

}

//@Override
void VideoCaptureTango::SetPreviewCallback(
  /* [in] */ IPreviewCallback* cb)
{
    mCamera->SetPreviewCallback(cb);
}

//@Override
ECode VideoCaptureTango::OnPreviewFrame(
  /* [in] */ ArrayOf<Byte>* data,
  /* [in] */ IHardwareCamera* camera)
{
    //mPreviewBufferLock->Lock();
    AutoPtr<ILock> lock = ILock::Probe( mPreviewBufferLock);
    lock->Lock();
    AutoPtr<IByteBufferHelper> bfHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bfHelper);
    //try {
        if (!mIsRunning) {
            lock->UnLock();
            return NOERROR;
        }

        if (data->GetLength() == SF_WIDTH * SF_FULL_HEIGHT) {
            Int32 rotation = GetDeviceOrientation();
            if (rotation != mDeviceOrientation) {
                mDeviceOrientation = rotation;
            }

            if (mCameraFacing == IHardwareCameraInfo::CAMERA_FACING_BACK) {
                rotation = 360 - rotation;
            }

            rotation = (mCameraOrientation + rotation) % 360;

            if (mTangoCameraId == DEPTH_CAMERA_ID) {
                Int32 sizeY = SF_WIDTH * SF_LINES_DEPTH;
                Int32 startY =
                    SF_WIDTH * (SF_LINES_HEADER + SF_LINES_FISHEYE +
                                SF_LINES_RESERVED);
                // Depth is composed of 16b samples in which only 12b are
                // used. Throw away lowest 4 resolution bits. Android
                // platforms are big endian, LSB in lowest address. In this
                // case Chroma components are unused. No need to write them
                // explicitly since they're filled to 128 on creation.
                Byte depthsample;
                for (Int32 j = startY; j < startY + 2 * sizeY; j += 2) {
                    depthsample = (Byte)(((*data)[j + 1] << 4) |
                                         (((*data)[j] & 0xF0) >> 4));
                    mFrameBuffer->Put(depthsample);
                }

                for (Int32 j = 0;
                     j < mCaptureFormat->mWidth * mCaptureFormat->mHeight - sizeY;
                     ++j)
                  mFrameBuffer->Put((Byte)0);
            }
            else if (mTangoCameraId == FISHEYE_CAMERA_ID) {
                Int32 sizeY = SF_WIDTH * SF_LINES_FISHEYE;
                Int32 startY = SF_WIDTH * SF_LINES_HEADER;
                // Fisheye is black and white so Chroma components are
                // unused. No need to write them explicitly since they're
                // filled to 128 on creation.
                AutoPtr<IByteBuffer> buffer;
                bfHelper->Wrap(data, startY, sizeY, (IByteBuffer**)&buffer);
                AutoPtr< ArrayOf<Byte> > array;// = ArrayOf<Byte>::Alloc(sizeY);
                mFrameBuffer->GetArray((ArrayOf<Byte>**)&array);
                buffer->Get(array, 0, sizeY);
            }
            else if (mTangoCameraId == FOURMP_CAMERA_ID) {
                Int32 startY =
                    SF_WIDTH * (SF_LINES_HEADER + SF_LINES_FISHEYE +
                                SF_LINES_RESERVED + SF_LINES_DEPTH_PADDED);
                Int32 sizeY = SF_WIDTH * SF_LINES_BIGIMAGE;

                // The spec is completely inaccurate on the location, sizes
                // and format of these channels.
                Int32 startU = SF_WIDTH * (SF_HEIGHT + SF_OFFSET_4MP_CHROMA);
                Int32 sizeU = SF_WIDTH * SF_LINES_BIGIMAGE / 4;
                Int32 startV = (SF_WIDTH * SF_HEIGHT * 5 / 4) +
                        SF_WIDTH * SF_OFFSET_4MP_CHROMA;
                Int32 sizeV = SF_WIDTH * SF_LINES_BIGIMAGE / 4;

                // Equivalent to the following |for| loop but much faster:
                // for (int i = START; i < START + SIZE; ++i)
                //     mFrameBuffer.put(data[i]);
                AutoPtr<IByteBuffer> buffer1;
                bfHelper->Wrap(data, startY, sizeY, (IByteBuffer**)&buffer1);
                AutoPtr< ArrayOf<Byte> > array1;// = ArrayOf<Byte>::Alloc(sizeY);
                mFrameBuffer->GetArray((ArrayOf<Byte>**)&array1);
                buffer1->Get(array1, 0, sizeY);

                AutoPtr<IByteBuffer> buffer2;
                bfHelper->Wrap(data, startU, sizeU, (IByteBuffer**)&buffer2);
                AutoPtr< ArrayOf<Byte> > array2;// = ArrayOf<Byte>::Alloc(sizeU);
                mFrameBuffer->GetArray((ArrayOf<Byte>**)&array2);
                buffer2->Get(array2, sizeY, sizeU);

                AutoPtr<IByteBuffer> buffer3;
                bfHelper->Wrap(data, startV, sizeV, (IByteBuffer**)&buffer3);
                AutoPtr< ArrayOf<Byte> > array3;// = ArrayOf<Byte>::Alloc(sizeV);
                mFrameBuffer->GetArray((ArrayOf<Byte>**)&array3);
                buffer3->Get(array3, sizeY + sizeU, sizeV);
            }
            else {
                Logger::E(TAG, "Unknown camera, #id: %d", mTangoCameraId);
                lock->UnLock();
                return NOERROR;
            }

            AutoPtr<IBuffer> ibuf = IBuffer::Probe(mFrameBuffer);
            ibuf->Rewind();  // Important!
            Int32 capacity;
            ibuf->GetCapacity(&capacity);
            AutoPtr< ArrayOf<Byte> > array;// = ArrayOf<Byte>::Alloc(capacity);
            mFrameBuffer->GetArray((ArrayOf<Byte>**)&array);
            NativeOnFrameAvailable(mNativeVideoCaptureDeviceAndroid,
                                   array,
                                   capacity,
                                   rotation);
        }
    //} finally {
        lock->UnLock();
    //}
    return NOERROR;
}

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
