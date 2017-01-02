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

#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwPicture.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwPicture_dec.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Graphics::EIID_IPicture;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//                 AwPicture::DestroyRunnable
//===============================================================
CAR_INTERFACE_IMPL(AwPicture::DestroyRunnable, Object, IRunnable);

AwPicture::DestroyRunnable::DestroyRunnable(
    /* [in] */ Handle64 nativeAwPicture)
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
    /* [in] */ Handle64 nativeAwPicture)
    : mNativeAwPicture(nativeAwPicture)
{
    AutoPtr<IRunnable> runnable = new DestroyRunnable(nativeAwPicture);
    mCleanupReference = new CleanupReference(this, runnable);
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
    /* [out] */ Handle32/*TODO Handle64*/* handle)
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
    /* [in] */ Handle64 nativeAwPicture)
{
    Elastos_AwPicture_nativeDestroy(nativeAwPicture);
}

Int32 AwPicture::NativeGetWidth(
    /* [in] */ Handle64 nativeAwPicture)
{
    return Elastos_AwPicture_nativeGetWidth(TO_IINTERFACE(this), nativeAwPicture);
}

Int32 AwPicture::NativeGetHeight(
    /* [in] */ Handle64 nativeAwPicture)
{
    return Elastos_AwPicture_nativeGetHeight(TO_IINTERFACE(this), nativeAwPicture);
}

void AwPicture::NativeDraw(
    /* [in] */ Handle64 nativeAwPicture,
    /* [in] */ ICanvas* canvas)
{
    Elastos_AwPicture_nativeDraw(TO_IINTERFACE(this), nativeAwPicture, TO_IINTERFACE(canvas));
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
