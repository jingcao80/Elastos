
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
        CAR_INTERFACE_DECL();

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
