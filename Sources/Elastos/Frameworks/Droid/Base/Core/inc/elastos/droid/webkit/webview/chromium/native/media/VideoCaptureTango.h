#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_VIDEOCAPTURETANGO_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_VIDEOCAPTURETANGO_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCapture.h"
#include "elastos/droid/webkit/webview/chromium/native/media/VideoCaptureFactory.h"

// import android.content.Context;
// import android.graphics.ImageFormat;
// import android.hardware.Camera;
// import android.util.Log;

using Elastos::IO::IByteBuffer;
// import java.nio.ByteBuffer;
// import java.util.ArrayList;
// import java.util.Arrays;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Media {

/**
 * This class extends the VideoCapture base class for manipulating a Tango
 * device's cameras, namely the associated Depth (z-Buffer), Fisheye and back-
 * facing 4MP video capture devices. These devices are differentiated via the
 * |id| passed on constructor, according to the index correspondence in
 * |s_CAM_PARAMS|; all devices |id| are index 0 towards the parent VideoCapture.
 **/
class VideoCaptureTango
    : public VideoCapture
{
public:
    VideoCaptureTango(
      /* [in] */ IContext* context,
      /* [in] */ Int32 id,
      /* [in] */ Int64 nativeVideoCaptureDeviceAndroid);

    static CARAPI_(Int32) NumberOfCameras();

    static CARAPI_(AutoPtr<VideoCaptureFactory::CamParams>) GetCamParams(
      /* [in] */ Int32 index);

    static CARAPI_(AutoPtr<ArrayOf<CaptureFormat*> >) GetDeviceSupportedFormats(
      /* [in] */ Int32 id);

    //@Override
    CARAPI OnPreviewFrame(
      /* [in] */ ArrayOf<Byte>* data,
      /* [in] */ IHardwareCamera* camera);

protected:
    //@Override
    CARAPI_(void) SetCaptureParameters(
      /* [in] */ Int32 width,
      /* [in] */ Int32 height,
      /* [in] */ Int32 frameRate,
      /* [in] */ IParameters* cameraParameters);

    //@Override
    CARAPI_(void) AllocateBuffers();

    //@Override
    CARAPI_(void) SetPreviewCallback(
      /* [in] */ IPreviewCallback* cb);

private:
    static AutoPtr< ArrayOf<VideoCaptureFactory::CamParams*> > s_CAM_PARAMS_Init();
    AutoPtr<IByteBuffer> mFrameBuffer;
    const Int32 mTangoCameraId;

    // The indexes must coincide with the s_CAM_PARAMS used below.
    static const Int32 DEPTH_CAMERA_ID = 0;
    static const Int32 FISHEYE_CAMERA_ID = 1;
    static const Int32 FOURMP_CAMERA_ID = 2;
    static const AutoPtr< ArrayOf<VideoCaptureFactory::CamParams*> > s_CAM_PARAMS;

    // SuperFrame size definitions. Note that total size is the amount of lines
    // multiplied by 3/2 due to Chroma components following.
    static const Int32 SF_WIDTH = 1280;
    static const Int32 SF_HEIGHT = 1168;
    static const Int32 SF_FULL_HEIGHT = SF_HEIGHT * 3 / 2;
    static const Int32 SF_LINES_HEADER = 16;
    static const Int32 SF_LINES_FISHEYE = 240;
    static const Int32 SF_LINES_RESERVED = 80;  // Spec says 96.
    static const Int32 SF_LINES_DEPTH = 60;
    static const Int32 SF_LINES_DEPTH_PADDED = 112;  // Spec says 96.
    static const Int32 SF_LINES_BIGIMAGE = 720;
    static const Int32 SF_OFFSET_4MP_CHROMA = 112;

    static const Byte CHROMA_ZERO_LEVEL;
    static const String TAG;
};

} // namespace Media
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_MEDIA_VIDEOCAPTURETANGO_H__
