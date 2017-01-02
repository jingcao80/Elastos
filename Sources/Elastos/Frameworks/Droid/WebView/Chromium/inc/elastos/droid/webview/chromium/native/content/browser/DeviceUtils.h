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
