
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_WINDOWCALLBACKWRAPPER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_WINDOWCALLBACKWRAPPER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"

// import android.annotation.SuppressLint;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * A wrapper for a Window.Callback instance, allowing subclasses to listen to or override specific
 * window messages.
 */
class WindowCallbackWrapper
    //: public Object
    : public IWindowCallback
{
public:
    WindowCallbackWrapper(
        /* [in] */ IWindowCallback* callback);

    CAR_INTERFACE_DECL();

    //@Override
    CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnActionModeFinished(
        /* [in] */ IActionMode* mode);

    //@Override
    CARAPI OnActionModeStarted(
        /* [in] */ IActionMode* mode);

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnContentChanged();

    //@Override
    CARAPI OnCreatePanelMenu(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreatePanelView(
        /* [in] */ Int32 featureId,
        /* [out] */ IView** view);

    //@Override
    //@SuppressLint("MissingSuperCall")
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnMenuItemSelected(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnMenuOpened(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPanelClosed(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu);

    //@Override
    CARAPI OnPreparePanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IView* view,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnSearchRequested(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnWindowAttributesChanged(
        /* [in] */ IWindowManagerLayoutParams* attrs);

    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    //@Override
    CARAPI OnWindowStartingActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** mode);

    virtual CARAPI OnWindowDismissed();

private:
    const AutoPtr<IWindowCallback> mCallback;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_WINDOWCALLBACKWRAPPER_H__
