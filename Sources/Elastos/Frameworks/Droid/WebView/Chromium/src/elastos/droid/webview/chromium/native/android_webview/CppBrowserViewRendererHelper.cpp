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

#include <Elastos.Droid.Graphics.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/CppBrowserViewRendererHelper.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/CppBrowserViewRendererHelper_dec.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

const String CppBrowserViewRendererHelper::LOGTAG("CppBrowserViewRendererHelper");

// Should never be instantiated.
CppBrowserViewRendererHelper::CppBrowserViewRendererHelper()
{
}

/**
 * Provides a Bitmap object with a given width and height used for auxiliary rasterization.
 * |canvas| is optional and if supplied indicates the Canvas that this Bitmap will be
 * drawn into. Note the Canvas will not be modified in any way.
 */
//@CalledByNative IBitmap
AutoPtr<IInterface> CppBrowserViewRendererHelper::CreateBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ ICanvas* canvas)
{
    if (canvas != NULL) {
        // When drawing into a Canvas, there is a maximum size imposed
        // on Bitmaps that can be drawn. Respect that limit.
        Int32 w, h;
        canvas->GetMaximumBitmapWidth(&w);
        canvas->GetMaximumBitmapHeight(&h);
        width = Elastos::Core::Math::Min(width, w);
        height = Elastos::Core::Math::Min(height, h);
    }

    AutoPtr<IBitmap> bitmap;
    // try {
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        ECode ecode = helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    // } catch (OutOfMemoryError e) {
    //     android.util.Log.w(LOGTAG, "Error allocating bitmap");
    // }
        if (FAILED(ecode))
            Logger::W(LOGTAG, "Error allocation bitmap");

    return bitmap;
}

/**
 * Draws a provided bitmap into a canvas.
 * Used for convenience from the native side and other static helper methods.
 */
//@CalledByNative
void CppBrowserViewRendererHelper::DrawBitmapIntoCanvas(
    /* [in] */ IBitmap* bitmap,
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 scroll_x,
    /* [in] */ Int32 scroll_y)
{
    canvas->Translate(scroll_x, scroll_y);
    canvas->DrawBitmap(bitmap, 0.0, 0.0, NULL);
}
//callback function definition
AutoPtr<IInterface> CppBrowserViewRendererHelper::CreateBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IInterface* canvas)
{
    AutoPtr<ICanvas> c = ICanvas::Probe(canvas);
    return CreateBitmap(width, height, canvas);
}

void CppBrowserViewRendererHelper::DrawBitmapIntoCanvas(
    /* [in] */ IInterface* bitmap,
    /* [in] */ IInterface* canvas,
    /* [in] */ Int32 scroll_x,
    /* [in] */ Int32 scroll_y)
{
    AutoPtr<IBitmap> b = IBitmap::Probe(bitmap);
    AutoPtr<ICanvas> c = ICanvas::Probe(canvas);
    DrawBitmapIntoCanvas(b, c, scroll_x, scroll_y);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
