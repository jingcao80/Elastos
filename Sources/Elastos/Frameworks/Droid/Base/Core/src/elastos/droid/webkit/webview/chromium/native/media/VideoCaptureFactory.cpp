
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCaptureFactory.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCaptureElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCaptureTango.h"

#include "elastos/droid/os/Build.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::IHardwareCamera;
using Elastos::Droid::Hardware::IHardwareCameraHelper;
using Elastos::Droid::Hardware::CHardwareCamera;
using Elastos::Droid::Hardware::CHardwareCameraHelper;

using Elastos::Core::StringUtils;
using Elastos::Droid::Os::Build;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

//===============================================================
//               VideoCaptureFactory::CamParams
//===============================================================

VideoCaptureFactory::CamParams::CamParams(
    /* [in] */ Int32 id,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
    : mId(id)
    , mName(name)
    , mWidth(width)
    , mHeight(height)
{
}

//===============================================================
//             VideoCaptureFactory::ChromiumCameraInfo
//===============================================================

static AutoPtr<ArrayOf<AutoPtr<ArrayOf<String> > > > s_SPECIAL_DEVICE_LIST_Init()
{
    AutoPtr< ArrayOf< AutoPtr< ArrayOf<String> > > > array = ArrayOf< AutoPtr< ArrayOf<String> > >::Alloc(1);
    AutoPtr< ArrayOf<String> > strs = ArrayOf<String>::Alloc(2);

    (*strs)[0] = String("Peanut");
    (*strs)[1] = String("peanut");

    (*array)[0] = strs;

    return array;
}

// Special devices have more cameras than usual. Those devices are
// identified by model & device. Currently only the Tango is supported.
// Note that these devices have no Camera.CameraInfo.
const AutoPtr< ArrayOf< AutoPtr< ArrayOf<String> > > > VideoCaptureFactory::ChromiumCameraInfo::s_SPECIAL_DEVICE_LIST = s_SPECIAL_DEVICE_LIST_Init();
const String VideoCaptureFactory::ChromiumCameraInfo::TAG("ChromiumCameraInfo");
Int32 VideoCaptureFactory::ChromiumCameraInfo::sNumberOfSystemCameras = -1;

VideoCaptureFactory::ChromiumCameraInfo::ChromiumCameraInfo(
    /* [in] */ Int32 index)
    : mId(index)
    , mCameraInfo(IsSpecialCamera(index) ? NULL : GetCameraInfo(mId))
{
}

Boolean VideoCaptureFactory::ChromiumCameraInfo::IsSpecialDevice()
{
    AutoPtr< ArrayOf<String> > device;
    Int32 length = s_SPECIAL_DEVICE_LIST->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        device = ((*s_SPECIAL_DEVICE_LIST)[i]);
        if ((*device)[0].Equals(Build::MODEL) &&
                (*device)[1].Equals(Build::DEVICE)) {
            return TRUE;
        }
    }

    return FALSE;
}

Boolean VideoCaptureFactory::ChromiumCameraInfo::IsSpecialCamera(
    /* [in] */ Int32 id)
{
    return id >= sNumberOfSystemCameras;
}

Int32 VideoCaptureFactory::ChromiumCameraInfo::ToSpecialCameraId(
    /* [in] */ Int32 id)
{
    assert(IsSpecialCamera(id));
    return id - sNumberOfSystemCameras;
}

//@CalledByNative("ChromiumCameraInfo")
Int32 VideoCaptureFactory::ChromiumCameraInfo::GetNumberOfCameras(
    /* [in] */ IContext* appContext)
{
    // Camera.getNumberOfCammeras() will not fail without permission, but the
    // following operation on camera will do. Without permission isn't fatal
    // error in WebView, specially for those application which has no purpose
    // to use camera, but happens to load page required it.
    // So, we output a warning log and pretend system have no camera at all.
    if (sNumberOfSystemCameras == -1) {
        AutoPtr<IPackageManager> pm;
        appContext->GetPackageManager((IPackageManager**)&pm);
        String packageName;
        appContext->GetPackageName(&packageName);
        Int32 permission;
        pm->CheckPermission(String("android.permission.CAMERA"), packageName, &permission);
        if (IPackageManager::PERMISSION_GRANTED == permission) {
            AutoPtr<IHardwareCameraHelper> helper;
            CHardwareCameraHelper::AcquireSingleton((IHardwareCameraHelper**)&helper);
            helper->GetNumberOfCameras(&sNumberOfSystemCameras);
        }
        else {
            sNumberOfSystemCameras = 0;
            Logger::W(TAG, "Missing android.permission.CAMERA permission, no system camera available.");
        }
    }

    if (IsSpecialDevice()) {
        Logger::D(TAG, "Special device: %s", Build::MODEL.string());
        return sNumberOfSystemCameras +
               VideoCaptureTango::NumberOfCameras();
    }
    else {
        return sNumberOfSystemCameras;
    }
}

//@CalledByNative("ChromiumCameraInfo")
AutoPtr<IInterface> VideoCaptureFactory::ChromiumCameraInfo::GetAt(
    /* [in] */ Int32 index)
{
    AutoPtr<ChromiumCameraInfo> info = new ChromiumCameraInfo(index);
    AutoPtr<IInterface> result = TO_IINTERFACE(info);
    return result;
}

//@CalledByNative("ChromiumCameraInfo")
Int32 VideoCaptureFactory::ChromiumCameraInfo::GetId()
{
    return mId;
}

//@CalledByNative("ChromiumCameraInfo")
String VideoCaptureFactory::ChromiumCameraInfo::GetDeviceName()
{
    if (IsSpecialCamera(mId)) {
        return VideoCaptureTango::GetCamParams(ToSpecialCameraId(mId))->mName;
    }
    else {
        if (mCameraInfo == NULL) {
            return String("");
        }

        String enumerated("front");
        Int32 facing;
        mCameraInfo->GetFacing(&facing);
        if (facing != IHardwareCameraInfo::CAMERA_FACING_FRONT)
            enumerated = String("back");
        Logger::D(TAG, "Camera enumerated: %s", enumerated.string());
        String str("camera ");
        str += StringUtils::ToString(mId);
        str += ", facing ";
        str += (facing ==
                IHardwareCameraInfo::CAMERA_FACING_FRONT ? "front" :
                "back");

        return str;
    }
}

//@CalledByNative("ChromiumCameraInfo")
Int32 VideoCaptureFactory::ChromiumCameraInfo::GetOrientation()
{
    if (IsSpecialCamera(mId)) {
        return IHardwareCameraInfo::CAMERA_FACING_BACK;
    }
    else {
        Int32 orientation;
        mCameraInfo->GetOrientation(&orientation);
        return mCameraInfo == NULL ? 0 : orientation;
    }
}

AutoPtr<IHardwareCameraInfo> VideoCaptureFactory::ChromiumCameraInfo::GetCameraInfo(
    /* [in] */ Int32 id)
{
    AutoPtr<IHardwareCameraInfo> cameraInfo;
    //TODO: CHardwareCamera::CCameraInfo::New((IHardwareCameraInfo**)&cameraInfo);
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

Int32 VideoCaptureFactory::ChromiumCameraInfo::GetNumberOfCameras(
    /* [in] */ IInterface* appContext)
{
    AutoPtr<IContext> c = IContext::Probe(appContext);
    return GetNumberOfCameras(c);
}

Int32 VideoCaptureFactory::ChromiumCameraInfo::GetId(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCaptureFactory::ChromiumCameraInfo> mObj = (VideoCaptureFactory::ChromiumCameraInfo*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCaptureFactory::ChromiumCameraInfo::GetId, mObj is NULL");
        return 0;
    }
    return mObj->GetId();
}

String VideoCaptureFactory::ChromiumCameraInfo::GetDeviceName(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCaptureFactory::ChromiumCameraInfo> mObj = (VideoCaptureFactory::ChromiumCameraInfo*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCaptureFactory::ChromiumCameraInfo::GetDeviceName, mObj is NULL");
        return String(NULL);
    }
    return mObj->GetDeviceName();
}

Int32 VideoCaptureFactory::ChromiumCameraInfo::GetOrientation(
    /* [in] */ IInterface* obj)
{
    AutoPtr<VideoCaptureFactory::ChromiumCameraInfo> mObj = (VideoCaptureFactory::ChromiumCameraInfo*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "VideoCaptureFactory::ChromiumCameraInfo::GetOrientation, mObj is NULL");
        return 0;
    }
    return mObj->GetOrientation();
}

//===============================================================
//                      VideoCaptureFactory
//===============================================================

// Factory methods.
//@CalledByNative
AutoPtr<IInterface> VideoCaptureFactory::CreateVideoCapture(
    /* [in] */ IContext* context,
    /* [in] */ Int32 id,
    /* [in] */ Int64 nativeVideoCaptureDeviceElastos)
{
  AutoPtr<VideoCapture> vc;
  AutoPtr<IInterface> result;
  if (ChromiumCameraInfo::IsSpecialCamera(id)) {
      vc = new VideoCaptureTango(context, ChromiumCameraInfo::ToSpecialCameraId(id),
              nativeVideoCaptureDeviceElastos);
  }
  else {
      vc = new VideoCaptureElastos(context, id,
              nativeVideoCaptureDeviceElastos);
  }
  if (vc.Get())
  {
      result = TO_IINTERFACE(vc);
  }
  return result;
}

//@CalledByNative
AutoPtr<ArrayOf<IInterface*> > VideoCaptureFactory::GetDeviceSupportedFormats(
    /* [in] */ Int32 id)
{
    AutoPtr<ArrayOf<VideoCapture::CaptureFormat*> > cf =
    ChromiumCameraInfo::IsSpecialCamera(id) ?
            VideoCaptureTango::GetDeviceSupportedFormats(
                    ChromiumCameraInfo::ToSpecialCameraId(id)) :
            VideoCaptureElastos::GetDeviceSupportedFormats(id);
    Int32 n = cf->GetLength();
    AutoPtr<ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(n);
    for(Int32 i = 0; i < n; ++i)
    {
        result->Set(i, TO_IINTERFACE((*cf)[i]));
    }
    return result;
}

//@CalledByNative
Int32 VideoCaptureFactory::GetCaptureFormatWidth(
    /* [in] */ VideoCapture::CaptureFormat* format)
{
    return format->GetWidth();
}

//@CalledByNative
Int32 VideoCaptureFactory::GetCaptureFormatHeight(
    /* [in] */ VideoCapture::CaptureFormat* format)
{
    return format->GetHeight();
}

//@CalledByNative
Int32 VideoCaptureFactory::GetCaptureFormatFramerate(
    /* [in] */ VideoCapture::CaptureFormat* format)
{
    return format->GetFramerate();
}

//@CalledByNative
Int32 VideoCaptureFactory::GetCaptureFormatPixelFormat(
    /* [in] */ VideoCapture::CaptureFormat* format)
{
    return format->GetPixelFormat();
}
//callback function definition
AutoPtr<IInterface> VideoCaptureFactory::CreateVideoCapture(
    /* [in] */ IInterface* context,
    /* [in] */ Int32 id,
    /* [in] */ Int64 nativeVideoCaptureDeviceElastos)
{
    AutoPtr<IContext> c = IContext::Probe(context);
    return CreateVideoCapture(c, id, nativeVideoCaptureDeviceElastos);
}

Int32 VideoCaptureFactory::GetCaptureFormatWidth(
    /* [in] */ IInterface* format)
{
    AutoPtr<VideoCapture::CaptureFormat> f = (VideoCapture::CaptureFormat*)(IObject::Probe(format));
    if (NULL == f)
    {
        Logger::E("VideoCaptureFactory", "VideoCaptureFactory::GetCaptureFormatWidth, mObj is NULL");
        return 0;
    }
    return VideoCaptureFactory::GetCaptureFormatWidth(f);
}

Int32 VideoCaptureFactory::GetCaptureFormatHeight(
    /* [in] */ IInterface* format)
{
    AutoPtr<VideoCapture::CaptureFormat> f = (VideoCapture::CaptureFormat*)(IObject::Probe(format));
    if (NULL == f)
    {
        Logger::E("VideoCaptureFactory", "VideoCaptureFactory::GetCaptureFormatHeight, mObj is NULL");
        return 0;
    }
    return VideoCaptureFactory::GetCaptureFormatHeight(f);
}

Int32 VideoCaptureFactory::GetCaptureFormatFramerate(
    /* [in] */ IInterface* format)
{
    AutoPtr<VideoCapture::CaptureFormat> f = (VideoCapture::CaptureFormat*)(IObject::Probe(format));
    if (NULL == f)
    {
        Logger::E("VideoCaptureFactory", "VideoCaptureFactory::GetCaptureFormatFramerate, mObj is NULL");
        return 0;
    }
    return VideoCaptureFactory::GetCaptureFormatFramerate(f);
}

Int32 VideoCaptureFactory::GetCaptureFormatPixelFormat(
    /* [in] */ IInterface* format)
{
    AutoPtr<VideoCapture::CaptureFormat> f = (VideoCapture::CaptureFormat*)(IObject::Probe(format));
    if (NULL == f)
    {
        Logger::E("VideoCaptureFactory", "VideoCaptureFactory::GetCaptureFormatPixelFormat, mObj is NULL");
        return 0;
    }
    return VideoCaptureFactory::GetCaptureFormatPixelFormat(f);
}

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
