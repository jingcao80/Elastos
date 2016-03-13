
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_CURSORCONTROLLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_CURSORCONTROLLER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IOnTouchModeChangeListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

class HandleView;

/**
 * A CursorController instance can be used to control a cursor in the text.
 */
class CursorController
    : public Object
    , public IOnTouchModeChangeListener
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Hide the cursor controller from screen.
     */
    virtual CARAPI Hide() = 0;

    /**
     * @return true if the CursorController is currently visible
     */
    virtual CARAPI IsShowing(
        /* [out] */ Boolean* result) = 0;

    /**
     * Called when the handle is about to start updating its position.
     * @param handle
     */
    virtual CARAPI BeforeStartUpdatingPosition(
        /* [in] */ HandleView* handle) = 0;

    /**
     * Update the controller's position.
     */
    virtual CARAPI UpdatePosition(
        /* [in] */ HandleView* handle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y) = 0;

    /**
     * Called when the view is detached from window. Perform house keeping task, such as
     * stopping Runnable thread that would otherwise keep a reference on the context, thus
     * preventing the activity to be recycled.
     */
    virtual CARAPI OnDetached() = 0;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_CURSORCONTROLLER_H__
