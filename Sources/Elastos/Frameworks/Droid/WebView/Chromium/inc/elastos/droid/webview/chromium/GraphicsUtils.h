
#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_GRAPHICSUTILS_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_GRAPHICSUTILS_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

class GraphicsUtils
    : public Object
{
public:
    static CARAPI_(Int64) GetDrawSWFunctionTable();

    static CARAPI_(Int64) GetDrawGLFunctionTable();

private:
    static CARAPI_(Int64) NativeGetDrawSWFunctionTable();

    static CARAPI_(Int64) NativeGetDrawGLFunctionTable();
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_GRAPHICSUTILS_H_

