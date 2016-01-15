// Provides a webviewchromium glue layer adapter from the internal Android
// graphics types into the types the chromium stack expects, and back.
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_PIXEL_INFO_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_PIXEL_INFO_H_

#include "android_webview/public/browser/draw_gl.h"
#include "android_webview/public/browser/draw_sw.h"

#include <cstdlib>
#include <utils/Log.h>
#include "elastos/droid/webkit/webview/chromium/GraphicBufferImpl.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "SkCanvasStateUtils.h"
#include "SkGraphics.h"
#include "SkPicture.h"

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

