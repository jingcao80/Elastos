
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
