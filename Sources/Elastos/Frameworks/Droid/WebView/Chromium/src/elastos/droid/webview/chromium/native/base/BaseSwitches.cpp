
#include "elastos/droid/webkit/webview/chromium/native/base/BaseSwitches.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

// Block onCreate() of Chrome until a Java debugger is attached.
const String BaseSwitches::WAIT_FOR_JAVA_DEBUGGER("wait-for-java-debugger");

// Overrides low-end device detection, disabling low-end device optimizations.
const String BaseSwitches::DISABLE_LOW_END_DEVICE_MODE("disable-low-end-device-mode");

// Overrides low-end device detection, enabling low-end device optimizations.
const String BaseSwitches::ENABLE_LOW_END_DEVICE_MODE("enable-low-end-device-mode");

// Adds additional thread idle time information into the trace event output.
const String BaseSwitches::ENABLE_IDLE_TRACING("enable-idle-tracing");

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
