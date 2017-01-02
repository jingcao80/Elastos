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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JAVABROWSERVIEWRENDERERHELPER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JAVABROWSERVIEWRENDERERHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Core::Object;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Provides auxiliary methods related to Picture objects and native SkPictures.
 */
//@JNINamespace("android_webview")
class CppBrowserViewRendererHelper
    : public Object
{
private:
    static const String LOGTAG;

    // Should never be instantiated.
    CppBrowserViewRendererHelper();

    /**
     * Provides a Bitmap object with a given width and height used for auxiliary rasterization.
     * |canvas| is optional and if supplied indicates the Canvas that this Bitmap will be
     * drawn into. Note the Canvas will not be modified in any way.
     */
    //@CalledByNative return IBitmap
    static CARAPI_(AutoPtr<IInterface>) CreateBitmap(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ICanvas* canvas);

    /**
     * Draws a provided bitmap into a canvas.
     * Used for convenience from the native side and other static helper methods.
     */
    //@CalledByNative
    static CARAPI_(void) DrawBitmapIntoCanvas(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 scroll_x,
        /* [in] */ Int32 scroll_y);

//callback function declaration
public:
    static CARAPI_(void*) ElaCppBrowserViewRendererHelperCallback_Init();

private:
    static CARAPI_(AutoPtr<IInterface>) CreateBitmap(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IInterface* canvas);

    static CARAPI_(void) DrawBitmapIntoCanvas(
        /* [in] */ IInterface* bitmap,
        /* [in] */ IInterface* canvas,
        /* [in] */ Int32 scroll_x,
        /* [in] */ Int32 scroll_y);
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JAVABROWSERVIEWRENDERERHELPER_H__
