#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWFORMDATABASE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWFORMDATABASE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
//import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Exposes a subset of Chromium form database to Webview database for managing autocomplete
 * functionality.
 */
//@JNINamespace("android_webview")
class AwFormDatabase
    : public Object
{
public:
    static CARAPI_(Boolean) HasFormData();

    static CARAPI_(void) ClearFormData();

private:
    //--------------------------------------------------------------------------------------------
    //  Native methods
    //--------------------------------------------------------------------------------------------
    static CARAPI_(Boolean) NativeHasFormData();

    static CARAPI_(void) NativeClearFormData();
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWFORMDATABASE_H__
