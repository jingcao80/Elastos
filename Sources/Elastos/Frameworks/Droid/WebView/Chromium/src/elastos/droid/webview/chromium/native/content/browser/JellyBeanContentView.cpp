
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/JellyBeanContentView.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                         JellyBeanContentView
//=====================================================================
JellyBeanContentView::JellyBeanContentView(
    /* [in] */ IContext* context,
    /* [in] */ ContentViewCore* cvc)
    : ContentView(context, cvc)
{
}

Boolean JellyBeanContentView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    if (mContentViewCore->SupportsAccessibilityAction(action)) {
        return mContentViewCore->PerformAccessibilityAction(action, arguments);
    }

    Boolean res;
    ContentView::PerformAccessibilityAction(action, arguments, &res);
    return res;
}

AutoPtr<IAccessibilityNodeProvider> JellyBeanContentView::GetAccessibilityNodeProvider()
{
    AutoPtr<IAccessibilityNodeProvider> provider = mContentViewCore->GetAccessibilityNodeProvider();
    if (provider != NULL) {
        return provider;
    } else {
        ContentView::GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        return provider;
    }
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
