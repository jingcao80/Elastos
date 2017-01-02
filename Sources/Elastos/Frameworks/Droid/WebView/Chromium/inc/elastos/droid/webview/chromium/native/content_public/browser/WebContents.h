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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_WEBCONTENTS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_WEBCONTENTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/NavigationController.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content_Public {
namespace Browser {

/**
 * The WebContents Java wrapper to allow communicating with the native WebContents object.
 */
class WebContents
    : public Object
{
public:
    /**
     * @return The navigation controller associated with this WebContents.
     */
    virtual CARAPI_(AutoPtr<NavigationController>) GetNavigationController() = 0;

    /**
     * @return The title for the current visible page.
     */
    virtual CARAPI_(String) GetTitle() = 0;

    /**
     * @return The URL for the current visible page.
     */
    virtual CARAPI_(String) GetVisibleUrl() = 0;

    /**
     * Stop any pending navigation.
     */
    virtual CARAPI_(void) Stop() = 0;
};

} // namespace Browser
} // namespace Content_Public
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_WEBCONTENTS_H__
