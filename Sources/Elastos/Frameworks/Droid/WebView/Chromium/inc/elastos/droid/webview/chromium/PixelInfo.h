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

// Provides a webviewchromium glue layer adapter from the internal Android
// graphics types into the types the chromium stack expects, and back.
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_PIXEL_INFO_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_PIXEL_INFO_H_

#include "android_webview/public/browser/draw_gl.h"
#include "android_webview/public/browser/draw_sw.h"

#include <cstdlib>
#include <utils/Log.h>
#include "elastos/droid/webkit/webview/chromium/GraphicBufferImpl.h"
#include <Elastos.Droid.Graphics.h>
//#include "elastos/droid/graphics/GraphicsNative.h"
#include "SkCanvasStateUtils.h"
#include "SkGraphics.h"
//TODO: compile error; #include "SkPicture.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

class PixelInfo : public AwPixelInfo {
public:
     PixelInfo(SkCanvas* canvas);
     ~PixelInfo();
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_PIXEL_INFO_H_

