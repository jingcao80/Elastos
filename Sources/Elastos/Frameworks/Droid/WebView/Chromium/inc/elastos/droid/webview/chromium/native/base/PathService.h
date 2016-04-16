
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHSERVICE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * This class provides java side access to the native PathService.
 */
//@JNINamespace("base::android")
class PathService
    : public Object
{
public:
    // Must match the value of DIR_MODULE in base/base_paths.h!
    static const Int32 DIR_MODULE = 3;

    static CARAPI_(void) Override(
        /* [in] */ Int32 what,
        /* [in] */ const String& path);

private:
    // Prevent instantiation.
    PathService();

    static CARAPI_(void) NativeOverride(
        /* [in] */ Int32 what,
        /* [in] */ const String& path);
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHSERVICE_H__
