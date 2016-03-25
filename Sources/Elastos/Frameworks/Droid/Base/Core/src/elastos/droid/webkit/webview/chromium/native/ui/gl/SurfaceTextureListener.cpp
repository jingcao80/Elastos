
#include "elastos/droid/webkit/webview/chromium/native/ui/gl/SurfaceTextureListener.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/api/SurfaceTextureListener_dec.h"

using Elastos::Droid::Graphics::EIID_IOnFrameAvailableListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Gl {

//=====================================================================
//                        SurfaceTextureListener
//=====================================================================
CAR_INTERFACE_IMPL(SurfaceTextureListener, Object, IOnFrameAvailableListener)

SurfaceTextureListener::SurfaceTextureListener(
    /* [in] */ Handle64 nativeSurfaceTextureListener)
{
    // ==================before translated======================
    // assert nativeSurfaceTextureListener != 0;
    // mNativeSurfaceTextureListener = nativeSurfaceTextureListener;

    assert (nativeSurfaceTextureListener != 0);
    mNativeSurfaceTextureListener = nativeSurfaceTextureListener;
}

ECode SurfaceTextureListener::OnFrameAvailable(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // nativeFrameAvailable(mNativeSurfaceTextureListener);

    NativeFrameAvailable(mNativeSurfaceTextureListener);
    return NOERROR;
}

ECode SurfaceTextureListener::Finalize()
{
    // ==================before translated======================
    // try {
    //     nativeDestroy(mNativeSurfaceTextureListener);
    // } finally {
    //     super.finalize();
    // }

    //try {
        NativeDestroy(mNativeSurfaceTextureListener);
    //}
    // finally {
    //}
    return NOERROR;
}

ECode SurfaceTextureListener::NativeFrameAvailable(
    /* [in] */ Handle64 nativeSurfaceTextureListener)
{
    Elastos_SurfaceTextureListener_nativeFrameAvailable(TO_IINTERFACE(this), (Handle64)nativeSurfaceTextureListener);
    return NOERROR;
}

ECode SurfaceTextureListener::NativeDestroy(
    /* [in] */ Handle64 nativeSurfaceTextureListener)
{
    Elastos_SurfaceTextureListener_nativeDestroy(TO_IINTERFACE(this), (Handle64)nativeSurfaceTextureListener);
    return NOERROR;
}

} // namespace Gl
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


