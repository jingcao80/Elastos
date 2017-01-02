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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_JELLYBEANBROWSERACCESSIBILITYMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_JELLYBEANBROWSERACCESSIBILITYMANAGER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/accessibility/BrowserAccessibilityManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;

// import org.chromium.base.JNINamespace;
// import org.chromium.content.browser.ContentViewCore;

// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class ContentViewCore;

namespace Accessibility {

/**
 * Subclass of BrowserAccessibilityManager for JellyBean that creates an
 * AccessibilityNodeProvider and delegates its implementation to this object.
 */
//@JNINamespace("content")
class JellyBeanBrowserAccessibilityManager : public BrowserAccessibilityManager
{
private:
    class InnerAccessibilityNodeProvider
        : public Object
        , public IAccessibilityNodeProvider
    {
    public:
        CAR_INTERFACE_DECL()

        InnerAccessibilityNodeProvider(
            /* [in] */ JellyBeanBrowserAccessibilityManager* owner);

        //@Override
        CARAPI CreateAccessibilityNodeInfo(
            /* [in] */ Int32 virtualViewId,
            /* [out] */ IAccessibilityNodeInfo** info);

        //@Override
        CARAPI FindAccessibilityNodeInfosByText(
            /* [in] */ const String& text,
            /* [in] */ Int32 virtualViewId,
            /* [out] */ IList** nodeInfos);

        //@Override
        CARAPI PerformAction(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments,
            /* [in] */ Boolean* result);

        CARAPI FindFocus(
            /* [in] */ Int32 focus,
            /* [out] */  IAccessibilityNodeInfo** info);

    private:
        JellyBeanBrowserAccessibilityManager* mOwner;
    };

public:
    JellyBeanBrowserAccessibilityManager(
        /* [in] */ Int64 nativeBrowserAccessibilityManagerAndroid,
        /* [in] */ ContentViewCore* contentViewCore);

    //@Override
    CARAPI_(AutoPtr<IAccessibilityNodeProvider>) GetAccessibilityNodeProvider();

private:
    AutoPtr<IAccessibilityNodeProvider> mAccessibilityNodeProvider;
};

} // namespace Accessibility
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_JELLYBEANBROWSERACCESSIBILITYMANAGER_H__
