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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWSTATICS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWSTATICS_H__

#include "elastos/droid/ext/frameworkdef.h"

// import org.chromium.net.ProxyChangeListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Implementations of various static methods.
 */
class ContentViewStatics
{
public:
    /**
     * Return the first substring consisting of the address of a physical location.
     * @see {@link android.webkit.WebView#findAddress(String)}
     *
     * @param addr The string to search for addresses.
     * @return the address, or if no address is found, return null.
     */
    static CARAPI_(String) FindAddress(
        /* [in] */ const String& addr);

    /**
     * Suspends Webkit timers in all renderers.
     * New renderers created after this call will be created with the
     * default options.
     *
     * @param suspend true if timers should be suspended.
     */
    static CARAPI_(void) SetWebKitSharedTimersSuspended(
        /* [in] */ Boolean suspend);

    /**
     * Enables platform notifications of data state and proxy changes.
     * Notifications are enabled by default.
     */
    static CARAPI_(void) EnablePlatformNotifications();

    /**
     * Disables platform notifications of data state and proxy changes.
     * Notifications are enabled by default.
     */
    static CARAPI_(void) DisablePlatformNotifications ();

private:
    // Native functions

    static CARAPI_(String) NativeFindAddress(
        /* [in] */ const String& addr);

    static CARAPI_(void) NativeSetWebKitSharedTimersSuspended(
        /* [in] */ Boolean suspend);
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWSTATICS_H__
