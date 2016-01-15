#include "elastos/droid/webkit/native/android_webview/AwPicture.h"
#include "elastos/droid/webkit/native/android_webview/api/AwPicture_dec.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Graphics::EIID_IPicture;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

//===============================================================
//                 AwPicture::DestroyRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwPicture::DestroyRunnable, Object, IRunnable);

AwPicture::DestroyRunnable::DestroyRunnable(
    /* [in] */ Int64 nativeAwPicture)
    : mNativeAwPicture(nativeAwPicture)
{
}

//@Override
ECode AwPicture::DestroyRunnable::Run()
{
    NativeDestroy(mNativeAwPicture);
    return NOERROR;
}

//===============================================================
//                         AwPicture
//===============================================================
CAR_INTERFACE_IMPL(AwPicture, Object, IPicture);

/**
 * @param nativeAwPicture is an instance of the AwPicture native class. Ownership is
 *                        taken by this java instance.
 */
AwPicture::AwPicture(
    /* [in] */ Int64 nativeAwPicture)
    : mNativeAwPicture(nativeAwPicture)
{
    AutoPtr<IRunnable> runnable = new DestroyRunnable(nativeAwPicture);
    //TODO mCleanupReference = new CleanupReference(this, runnable);
}

//@Override
ECode AwPicture::BeginRecording(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas)
    *canvas = NULL;
    UnsupportedOperation();
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode AwPicture::EndRecording()
{
    // Intentional no-op. The native picture ended recording prior to java c'tor call.
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode AwPicture::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    Int32 w = NativeGetWidth(mNativeAwPicture);
    *width = w;
    return NOERROR;
}

//@Override
ECode AwPicture::GetHeight(
        /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    Int32 h = NativeGetHeight(mNativeAwPicture);
    *height = h;
    return NOERROR;
}

//@Override
ECode AwPicture::Draw(
    /* [in] */ ICanvas* canvas)
{
    NativeDraw(mNativeAwPicture, canvas);
    return NOERROR;
}

ECode GetNativePicture(
    /* [out] */ Handle32* handle)
{
    return E_NOT_IMPLEMENTED;
}

//@Override
//@SuppressWarnings("deprecation")
ECode AwPicture::WriteToStream(
    /* [in] */ IOutputStream* stream)
{
    UnsupportedOperation();
    return E_NOT_IMPLEMENTED;
}

void AwPicture::UnsupportedOperation()
{
    //throw new IllegalStateException("Unsupported in AwPicture");
    assert(0);
}

void AwPicture::NativeDestroy(
    /* [in] */ Int64 nativeAwPicture)
{
    Elastos_AwPicture_nativeDestroy((Handle32)nativeAwPicture);
}

Int32 AwPicture::NativeGetWidth(
    /* [in] */ Int64 nativeAwPicture)
{
    return Elastos_AwPicture_nativeGetWidth(THIS_PROBE(IInterface), (Handle32)nativeAwPicture);
}

Int32 AwPicture::NativeGetHeight(
    /* [in] */ Int64 nativeAwPicture)
{
    return Elastos_AwPicture_nativeGetHeight(THIS_PROBE(IInterface), (Handle32)nativeAwPicture);
}

void AwPicture::NativeDraw(
    /* [in] */ Int64 nativeAwPicture,
    /* [in] */ ICanvas* canvas)
{
    Elastos_AwPicture_nativeDraw(THIS_PROBE(IInterface), (Handle32)nativeAwPicture, TO_IINTERFACE(canvas));
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
