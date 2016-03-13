
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewStatics.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/ContentViewStatics_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/net/ProxyChangeListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Net::ProxyChangeListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Return the first substring consisting of the address of a physical location.
 * @see {@link android.webkit.WebView#findAddress(String)}
 *
 * @param addr The string to search for addresses.
 * @return the address, or if no address is found, return null.
 */
String ContentViewStatics::FindAddress(
    /* [in] */ const String& addr)
{
    if (addr == NULL) {
        //throw new NullPointerException("addr is null");
        Logger::E("ContentViewStatics::FindAddress", "addr is null");
        assert(0);
    }

    String result = NativeFindAddress(addr);

    return result == NULL || result.IsEmpty() ? String(NULL) : result;
}

/**
 * Suspends Webkit timers in all renderers.
 * New renderers created after this call will be created with the
 * default options.
 *
 * @param suspend true if timers should be suspended.
 */
void ContentViewStatics::SetWebKitSharedTimersSuspended(
    /* [in] */ Boolean suspend)
{
    NativeSetWebKitSharedTimersSuspended(suspend);
}

/**
 * Enables platform notifications of data state and proxy changes.
 * Notifications are enabled by default.
 */
void ContentViewStatics::EnablePlatformNotifications()
{
    ProxyChangeListener::SetEnabled(TRUE);
}

/**
 * Disables platform notifications of data state and proxy changes.
 * Notifications are enabled by default.
 */
void ContentViewStatics::DisablePlatformNotifications ()
{
    ProxyChangeListener::SetEnabled(FALSE);
}

// Native functions

String ContentViewStatics::NativeFindAddress(
    /* [in] */ const String& addr)
{
    return Elastos_ContentViewStatics_nativeFindAddress(addr);
}

void ContentViewStatics::NativeSetWebKitSharedTimersSuspended(
    /* [in] */ Boolean suspend)
{
    Elastos_ContentViewStatics_nativeSetWebKitSharedTimersSuspended(suspend);
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
