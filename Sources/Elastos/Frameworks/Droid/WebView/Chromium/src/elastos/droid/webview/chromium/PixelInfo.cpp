#include "elastos/droid/webkit/webview/chromium/PixelInfo.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

PixelInfo::PixelInfo(SkCanvas* canvas) {
    memset(this, 0, sizeof(AwPixelInfo));
    version = kAwPixelInfoVersion;
    state = SkCanvasStateUtils::CaptureCanvasState(canvas);
}

PixelInfo::~PixelInfo() {
    if (state)
        SkCanvasStateUtils::ReleaseCanvasState(state);
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

