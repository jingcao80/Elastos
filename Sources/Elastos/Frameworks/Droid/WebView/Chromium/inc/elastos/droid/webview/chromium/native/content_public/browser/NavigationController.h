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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_NAVIGATIONCONTROLLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_NAVIGATIONCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content_Public {
namespace Browser {

/**
 * The NavigationController Java wrapper to allow communicating with the native
 * NavigationController object.
 */
class NavigationController
    : public Object
{
public:
    /**
     * @return Whether back navigation is possible from the "current entry".
     */
    virtual CARAPI_(Boolean) CanGoBack() = 0;

    /**
     * @return Whether forward navigation is possible from the "current entry".
     */
    virtual CARAPI_(Boolean) CanGoForward() = 0;

    /**
     * @param offset The offset into the navigation history.
     * @return Whether we can move in history by given offset
     */
    virtual CARAPI_(Boolean) CanGoToOffset(
        /* [in] */ Int32 offset) = 0;

    /**
     * Navigates to the specified offset from the "current entry". Does nothing if the offset is
     * out of bounds.
     * @param offset The offset into the navigation history.
     */
    virtual CARAPI_(void) GoToOffset(
        /* [in] */ Int32 offset) = 0;

    /**
     * Navigates to the specified index in the navigation entry for this page.
     * @param index The navigation index to navigate to.
     */
    virtual CARAPI_(void) GoToNavigationIndex(
        /* [in] */ Int32 index) = 0;

    /**
     * Goes to the navigation entry before the current one.
     */
    virtual CARAPI_(void) GoBack() = 0;

    /**
     * Goes to the navigation entry following the current one.
     */
    virtual CARAPI_(void) GoForward() = 0;
};

} // namespace Browser
} // namespace Content_Public
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_PUBLIC_BROWSER_NAVIGATIONCONTROLLER_H__
