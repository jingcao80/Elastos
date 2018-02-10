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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPICTURE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPICTURE_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/CleanupReference.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPicture;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Common::CleanupReference;
using Elastos::Core::IRunnable;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

// A simple wrapper around a SkPicture, that allows final rendering to be performed using the
// chromium skia library.
//@JNINamespace("android_webview")
class AwPicture
    : public Object
    , public IPicture
{
public:
    CAR_INTERFACE_DECL()
private:
    // There is no explicit destroy method on Picture base-class, so cleanup is always
    // handled via the CleanupReference.
    class DestroyRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        //@Override
        CARAPI Run();

    public:
        DestroyRunnable(
            /* [in] */ HANDLE nativeAwPicture);

        HANDLE mNativeAwPicture;
    };

public:
    /**
     * @param nativeAwPicture is an instance of the AwPicture native class. Ownership is
     *                        taken by this java instance.
     */
    AwPicture(
        /* [in] */ HANDLE nativeAwPicture);

    //@Override
    CARAPI BeginRecording(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ ICanvas** canvas);

    //@Override
    CARAPI EndRecording();

    //@Override
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //override
    CARAPI GetNativePicture(
        /* [out] */ HANDLE/*TODO HANDLE*/* handle);
    //@Override
    //@SuppressWarnings("deprecation")
    CARAPI WriteToStream(
        /* [in] */ IOutputStream* stream);

private:
    CARAPI_(void) UnsupportedOperation();

    static CARAPI_(void) NativeDestroy(
        /* [in] */ HANDLE nativeAwPicture);

    CARAPI_(Int32) NativeGetWidth(
        /* [in] */ HANDLE nativeAwPicture);

    CARAPI_(Int32) NativeGetHeight(
        /* [in] */ HANDLE nativeAwPicture);

    CARAPI_(void) NativeDraw(
        /* [in] */ HANDLE nativeAwPicture, ICanvas* canvas);

private:
    HANDLE mNativeAwPicture;
    AutoPtr<CleanupReference> mCleanupReference;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPICTURE_H__
