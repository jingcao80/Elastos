
#include "elastos/droid/webkit/native/ui/gl/SurfaceTextureListener.h"
#include "elastos/droid/webkit/native/ui/api/SurfaceTextureListener_dec.h"

using Elastos::Droid::Graphics::EIID_IOnFrameAvailableListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Gl {

//=====================================================================
//                        SurfaceTextureListener
//=====================================================================
CAR_INTERFACE_IMPL(SurfaceTextureListener, Object, IOnFrameAvailableListener)

SurfaceTextureListener::SurfaceTextureListener(
    /* [in] */ Int64 nativeSurfaceTextureListener)
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

    assert(0);
    //try {
        NativeDestroy(mNativeSurfaceTextureListener);
    //}
    // finally {
        //--: base class has no this func; super.finalize();
    //}
    return NOERROR;
}

ECode SurfaceTextureListener::NativeFrameAvailable(
    /* [in] */ Int64 nativeSurfaceTextureListener)
{
    Elastos_SurfaceTextureListener_nativeFrameAvailable(THIS_PROBE(IInterface), (Handle32)nativeSurfaceTextureListener);
    return NOERROR;
}

ECode SurfaceTextureListener::NativeDestroy(
    /* [in] */ Int64 nativeSurfaceTextureListener)
{
    Elastos_SurfaceTextureListener_nativeDestroy(THIS_PROBE(IInterface), (Handle32)nativeSurfaceTextureListener);
    return NOERROR;
}

} // namespace Gl
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


