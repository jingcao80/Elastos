
#ifndef __ELASTOS_DROID_VIEW_WINDOWCALLBACKWRAPPER_H__
#define __ELASTOS_DROID_VIEW_WINDOWCALLBACKWRAPPER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * A simple decorator stub for Window.Callback that passes through any calls
 * to the wrapped instance as a base implementation. Call super.foo() to call into
 * the wrapped callback for any subclasses.
 *
 * @hide for internal use
 */
class WindowCallbackWrapper
    : public Object
    , public IWindowCallback
{
public:
    WindowCallbackWrapper();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IWindowCallback* wrapped);

    // @Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnCreatePanelView(
        /* [in] */ Int32 featureId,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnCreatePanelMenu(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnPreparePanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IView* view,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnMenuOpened(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnMenuItemSelected(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnWindowAttributesChanged(
        /* [in] */ IWindowManagerLayoutParams* attrs);

    // @Override
    CARAPI OnContentChanged();

    // @Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnDetachedFromWindow();

    // @Override
    CARAPI OnPanelClosed(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu);

    // @Override
    CARAPI OnSearchRequested(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnWindowStartingActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** mode);

    // @Override
    CARAPI OnActionModeStarted(
        /* [in] */ IActionMode* mode);

    // @Override
    CARAPI OnActionModeFinished(
        /* [in] */ IActionMode* mode);

private:
    AutoPtr<IWindowCallback> mWrapped;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_VIEW_WINDOWCALLBACKWRAPPER_H__
