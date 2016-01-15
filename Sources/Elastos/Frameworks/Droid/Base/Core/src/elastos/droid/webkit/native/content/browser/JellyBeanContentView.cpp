
#include "elastos/droid/webkit/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/native/content/browser/JellyBeanContentView.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
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

    assert(0);
    // TODO
    // return ContentView::PerformAccessibilityAction(action, arguments);
    return FALSE;
}

AutoPtr<IAccessibilityNodeProvider> JellyBeanContentView::GetAccessibilityNodeProvider()
{
    AutoPtr<IAccessibilityNodeProvider> provider = mContentViewCore->GetAccessibilityNodeProvider();
    if (provider != NULL) {
        return provider;
    } else {
        assert(0);
        // TODO
        // return ContentView::GetAccessibilityNodeProvider();
        return NULL;
    }
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
