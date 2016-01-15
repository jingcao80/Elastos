
#include "elastos/droid/graphics/CSurfaceTexture.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/webkit/native/ui/gl/SurfaceTexturePlatformWrapper.h"
#include "elastos/droid/webkit/native/ui/gl/SurfaceTextureListener.h"
#include "elastos/droid/webkit/native/ui/api/SurfaceTexturePlatformWrapper_dec.h"

using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::ISurfaceTextureListener;
using Elastos::Droid::Webkit::Ui::Gl::SurfaceTextureListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Gl {

//=====================================================================
//                    SurfaceTexturePlatformWrapper
//=====================================================================
const String SurfaceTexturePlatformWrapper::TAG("SurfaceTexturePlatformWrapper");

AutoPtr<IInterface> SurfaceTexturePlatformWrapper::Create(
    /* [in] */ Int32 textureId)
{
    // ==================before translated======================
    // return new SurfaceTexture(textureId);

    AutoPtr<ISurfaceTexture> surfaceTexture;
    CSurfaceTexture::New(textureId, (ISurfaceTexture**)&surfaceTexture);
    return TO_IINTERFACE(surfaceTexture);
}

AutoPtr<IInterface> SurfaceTexturePlatformWrapper::CreateSingleBuffered(
    /* [in] */ Int32 textureId)
{
    // ==================before translated======================
    // assert Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;
    // return new SurfaceTexture(textureId, true);

    assert (Build::VERSION::SDK_INT >= Build::VERSION_CODES::KITKAT);
    AutoPtr<ISurfaceTexture> surfaceTexture;
    CSurfaceTexture::New(textureId, TRUE, (ISurfaceTexture**)&surfaceTexture);
    return TO_IINTERFACE(surfaceTexture);
}

ECode SurfaceTexturePlatformWrapper::Destroy(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // surfaceTexture.setOnFrameAvailableListener(null);
    // surfaceTexture.release();

    surfaceTexture->SetOnFrameAvailableListener(NULL);
    surfaceTexture->ReleaseTexImage();
    return NOERROR;
}

ECode SurfaceTexturePlatformWrapper::SetFrameAvailableCallback(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Int64 nativeSurfaceTextureListener)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // surfaceTexture.setOnFrameAvailableListener(
    //         new SurfaceTextureListener(nativeSurfaceTextureListener));

    AutoPtr<SurfaceTextureListener> surfaceTextureListener = new SurfaceTextureListener(nativeSurfaceTextureListener);
    surfaceTexture->SetOnFrameAvailableListener((IOnFrameAvailableListener*)surfaceTextureListener);
    return NOERROR;
}

ECode SurfaceTexturePlatformWrapper::UpdateTexImage(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // try {
    //     surfaceTexture.updateTexImage();
    // } catch (RuntimeException e) {
    //     Log.e(TAG, "Error calling updateTexImage", e);
    // }

    //try {
        surfaceTexture->UpdateTexImage();
    //}
    // catch (RuntimeException e) {
    //    Log.e(TAG, "Error calling updateTexImage", e);
    //}
    return NOERROR;
}

ECode SurfaceTexturePlatformWrapper::ReleaseTexImage(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // assert Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;
    // surfaceTexture.releaseTexImage();

    assert (Build::VERSION::SDK_INT >= Build::VERSION_CODES::KITKAT);
    surfaceTexture->ReleaseTexImage();
    return NOERROR;
}

ECode SurfaceTexturePlatformWrapper::SetDefaultBufferSize(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // surfaceTexture.setDefaultBufferSize(width, height);

    surfaceTexture->SetDefaultBufferSize(width, height);
    return NOERROR;
}

ECode SurfaceTexturePlatformWrapper::GetTransformMatrix(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ ArrayOf<Float>* matrix)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    VALIDATE_NOT_NULL(matrix);
    // ==================before translated======================
    // surfaceTexture.getTransformMatrix(matrix);

    surfaceTexture->GetTransformMatrix(matrix);
    return NOERROR;
}

ECode SurfaceTexturePlatformWrapper::AttachToGLContext(
    /* [in] */ ISurfaceTexture* surfaceTexture,
    /* [in] */ Int32 texName)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // assert Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN;
    // surfaceTexture.attachToGLContext(texName);

    assert (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN);
    surfaceTexture->AttachToGLContext(texName);
    return NOERROR;
}

ECode SurfaceTexturePlatformWrapper::DetachFromGLContext(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    VALIDATE_NOT_NULL(surfaceTexture);
    // ==================before translated======================
    // assert Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN;
    // surfaceTexture.detachFromGLContext();

    assert (Build::VERSION::SDK_INT >= Build::VERSION_CODES::JELLY_BEAN);
    surfaceTexture->DetachFromGLContext();
    return NOERROR;
}

void SurfaceTexturePlatformWrapper::Destroy(
    /* [in] */ IInterface* surfaceTexture)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    Destroy(st);
}

void SurfaceTexturePlatformWrapper::SetFrameAvailableCallback(
    /* [in] */ IInterface* surfaceTexture,
    /* [in] */ Int64 nativeSurfaceTextureListener)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    SetFrameAvailableCallback(st, nativeSurfaceTextureListener);
}

void SurfaceTexturePlatformWrapper::UpdateTexImage(
    /* [in] */ IInterface* surfaceTexture)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    UpdateTexImage(st);
}

void SurfaceTexturePlatformWrapper::ReleaseTexImage(
    /* [in] */ IInterface* surfaceTexture)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    ReleaseTexImage(st);
}

void SurfaceTexturePlatformWrapper::SetDefaultBufferSize(
    /* [in] */ IInterface* surfaceTexture,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    SetDefaultBufferSize(st, width, height);
}

void SurfaceTexturePlatformWrapper::GetTransformMatrix(
    /* [in] */ IInterface* surfaceTexture,
    /* [in] */ ArrayOf<Float>* matrix)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    GetTransformMatrix(st, matrix);
}

void SurfaceTexturePlatformWrapper::AttachToGLContext(
    /* [in] */ IInterface* surfaceTexture,
    /* [in] */ Int32 texName)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    AttachToGLContext(st, texName);
}

void SurfaceTexturePlatformWrapper::DetachFromGLContext(
    /* [in] */ IInterface* surfaceTexture)
{
    ISurfaceTexture* st = ISurfaceTexture::Probe(surfaceTexture);
    DetachFromGLContext(st);
}

} // namespace Gl
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


