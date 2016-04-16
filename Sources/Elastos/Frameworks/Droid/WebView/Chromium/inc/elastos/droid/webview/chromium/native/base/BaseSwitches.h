
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASESWITCHES_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASESWITCHES_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Contains all of the command line switches that are specific to the base/
 * portion of Chromium on Android.
 */
class BaseSwitches
{
public:
    // Block onCreate() of Chrome until a Java debugger is attached.
    static const String WAIT_FOR_JAVA_DEBUGGER;

    // Overrides low-end device detection, disabling low-end device optimizations.
    static const String DISABLE_LOW_END_DEVICE_MODE;

    // Overrides low-end device detection, enabling low-end device optimizations.
    static const String ENABLE_LOW_END_DEVICE_MODE;

    // Adds additional thread idle time information into the trace event output.
    static const String ENABLE_IDLE_TRACING;

private:
    // Prevent instantiation.
    BaseSwitches() {}
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_BASESWITCHES_H__
