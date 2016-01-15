
#ifndef __ELASTOS_DROID_WEBKIT_MEDIA_VIDEOCAPTUREFACTORY_H__
#define __ELASTOS_DROID_WEBKIT_MEDIA_VIDEOCAPTUREFACTORY_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/media/VideoCapture.h"

// import android.content.Context;
// import android.content.pm.PackageManager;
// import android.hardware.Camera;
// import android.util.Log;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// // Needed for jni_generator.py to guess correctly the origin of
// // VideoCapture.CaptureFormat.
// import org.chromium.media.VideoCapture;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Media {

/**
 * This class implements a factory of Android Video Capture objects for Chrome.
 * The static createVideoCapture() returns either a "normal" VideoCaptureAndroid
 * or a "special" VideoCaptureTango. Cameras are identified by |id|, where Tango
 * cameras have |id| above the standard ones. Video Capture objects allocated
 * via createVideoCapture() are explicitly owned by the caller.
 * ChromiumCameraInfo is an internal class with some static methods needed from
 * the native side to enumerate devices and collect their names and info. It
 * takes into account the mentioned special devices.
 **/
//@JNINamespace("media")
class VideoCaptureFactory
    : public Object
{
public:
    class CamParams
        : public Object
    {
    public:
        CamParams(
            /* [in] */ Int32 id,
            /* [in] */ const String& name,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

    public:
        const Int32 mId;
        const String mName;
        const Int32 mWidth;
        const Int32 mHeight;
    };

    class ChromiumCameraInfo
        : public Object
    {
    public:
        ChromiumCameraInfo(
            /* [in] */ Int32 index);

        static CARAPI_(Boolean) IsSpecialDevice();

        static CARAPI_(Boolean) IsSpecialCamera(
            /* [in] */ Int32 id);

        static CARAPI_(Int32) ToSpecialCameraId(
            /* [in] */ Int32 id);

        //@CalledByNative("ChromiumCameraInfo")
        static CARAPI_(Int32) GetNumberOfCameras(
            /* [in] */ IContext* appContext);

        //@CalledByNative("ChromiumCameraInfo")
        //static CARAPI_(AutoPtr<ChromiumCameraInfo>) GetAt(
        static CARAPI_(AutoPtr<IInterface>) GetAt(
            /* [in] */ Int32 index);

        //@CalledByNative("ChromiumCameraInfo")
        CARAPI_(Int32) GetId();

        //@CalledByNative("ChromiumCameraInfo")
        CARAPI_(String) GetDeviceName();

        //@CalledByNative("ChromiumCameraInfo")
        CARAPI_(Int32) GetOrientation();

        CARAPI_(AutoPtr<IHardwareCameraInfo>) GetCameraInfo(
            /* [in] */ Int32 id);

    private:
        static CARAPI_(Int32) GetNumberOfCameras(
            /* [in] */ IInterface* appContext);

        static CARAPI_(Int32) GetId(
            /* [in] */ IInterface* obj);

        static CARAPI_(String) GetDeviceName(
            /* [in] */ IInterface* obj);

        static CARAPI_(Int32) GetOrientation(
            /* [in] */ IInterface* obj);

    private:
        // Special devices have more cameras than usual. Those devices are
        // identified by model & device. Currently only the Tango is supported.
        // Note that these devices have no Camera.CameraInfo.
        static const AutoPtr< ArrayOf< AutoPtr< ArrayOf<String> > > > s_SPECIAL_DEVICE_LIST;

        static const String TAG;

        static Int32 sNumberOfSystemCameras;

        const Int32 mId;
        const AutoPtr<IHardwareCameraInfo> mCameraInfo;
    };

public:
    // Factory methods.
    //@CalledByNative
    //static CARAPI_(AutoPtr<VideoCapture>) CreateVideoCapture(
    static CARAPI_(AutoPtr<IInterface>) CreateVideoCapture(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [in] */ Int64 nativeVideoCaptureDeviceElastos);

    //@CalledByNative
    //static CARAPI_(AutoPtr<ArrayOf<VideoCapture::CaptureFormat> >) GetDeviceSupportedFormats(
    static CARAPI_(AutoPtr<ArrayOf<IInterface*> >) GetDeviceSupportedFormats(
        /* [in] */ Int32 id);

    //@CalledByNative
    static CARAPI_(Int32) GetCaptureFormatWidth(
        /* [in] */ VideoCapture::CaptureFormat* format);

    //@CalledByNative
    static CARAPI_(Int32) GetCaptureFormatHeight(
        /* [in] */ VideoCapture::CaptureFormat* format);

    //@CalledByNative
    static CARAPI_(Int32) GetCaptureFormatFramerate(
        /* [in] */ VideoCapture::CaptureFormat* format);

    //@CalledByNative
    static CARAPI_(Int32) GetCaptureFormatPixelFormat(
        /* [in] */ VideoCapture::CaptureFormat* format);
//callback function declaration
public:
    static CARAPI_(void*) ElaVideoCaptureFactoryCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) CreateVideoCapture(
        /* [in] */ IInterface* context,
        /* [in] */ Int32 id,
        /* [in] */ Int64 nativeVideoCaptureDeviceElastos);

    static CARAPI_(Int32) GetCaptureFormatWidth(
        /* [in] */ IInterface* format);

    static CARAPI_(Int32) GetCaptureFormatHeight(
        /* [in] */ IInterface* format);

    static CARAPI_(Int32) GetCaptureFormatFramerate(
        /* [in] */ IInterface* format);

    static CARAPI_(Int32) GetCaptureFormatPixelFormat(
        /* [in] */ IInterface* format);
};

} // namespace Media
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Webkit::Media::VideoCaptureFactory::CamParams, IInterface);

#endif//__ELASTOS_DROID_WEBKIT_MEDIA_VIDEOCAPTUREFACTORY_H__
