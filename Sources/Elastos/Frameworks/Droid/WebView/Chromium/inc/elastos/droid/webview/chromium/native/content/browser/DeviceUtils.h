
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DEVICEUTILS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DEVICEUTILS_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;

// import org.chromium.base.CommandLine;
// import org.chromium.content.common.ContentSwitches;
// import org.chromium.ui.base.DeviceFormFactor;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * A utility class that has helper methods for device configuration.
 */
class DeviceUtils
{
public:
    /**
     * @param context Android's context
     * @return        Whether the app is should treat the device as a tablet for layout.
     */
    // TODO(tedchoc): Transition all call sites to use DeviceFormFactor directly.  Then
    //                remove this method.
    static CARAPI_(Boolean) IsTablet(
        /* [in] */ IContext* context);

    /**
     * Appends the switch specifying which user agent should be used for this device.
     * @param context The context for the caller activity.
     */
    static CARAPI_(void) AddDeviceSpecificUserAgentSwitch(
        /* [in] */ IContext* context);
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DEVICEUTILS_H__
