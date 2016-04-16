
#include "elastos/droid/webkit/webview/chromium/native/base/WindowCallbackWrapper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

WindowCallbackWrapper::WindowCallbackWrapper(
    /* [in] */ IWindowCallback* callback)
    : mCallback(callback)
{
}

//@Override
ECode WindowCallbackWrapper::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return mCallback->DispatchGenericMotionEvent(event, result);
}

//@Override
ECode WindowCallbackWrapper::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mCallback->DispatchKeyEvent(event, result);
}

//@Override
ECode WindowCallbackWrapper::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return mCallback->DispatchKeyShortcutEvent(event, result);
}

//@Override
ECode WindowCallbackWrapper::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    return mCallback->DispatchPopulateAccessibilityEvent(event, result);
}

//@Override
ECode WindowCallbackWrapper::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return mCallback->DispatchTouchEvent(event, result);
}

//@Override
ECode WindowCallbackWrapper::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    return mCallback->DispatchTrackballEvent(event, result);
}

//@Override
ECode WindowCallbackWrapper::OnActionModeFinished(
    /* [in] */ IActionMode* mode)
{
    return mCallback->OnActionModeFinished(mode);
}

//@Override
ECode WindowCallbackWrapper::OnActionModeStarted(
    /* [in] */ IActionMode* mode)
{
    return mCallback->OnActionModeStarted(mode);
}

//@Override
ECode WindowCallbackWrapper::OnAttachedToWindow()
{
    return mCallback->OnAttachedToWindow();
}

//@Override
ECode WindowCallbackWrapper::OnContentChanged()
{
    return mCallback->OnContentChanged();
}

//@Override
ECode WindowCallbackWrapper::OnCreatePanelMenu(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    return mCallback->OnCreatePanelMenu(featureId, menu, result);
}

//@Override
ECode WindowCallbackWrapper::OnCreatePanelView(
    /* [in] */ Int32 featureId,
    /* [out] */ IView** view)
{
    return mCallback->OnCreatePanelView(featureId, view);
}

//@Override
//@SuppressLint("MissingSuperCall")
ECode WindowCallbackWrapper::OnDetachedFromWindow()
{
    return mCallback->OnDetachedFromWindow();
}

//@Override
ECode WindowCallbackWrapper::OnMenuItemSelected(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    return mCallback->OnMenuItemSelected(featureId, item, result);
}

//@Override
ECode WindowCallbackWrapper::OnMenuOpened(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    return mCallback->OnMenuOpened(featureId, menu, result);
}

//@Override
ECode WindowCallbackWrapper::OnPanelClosed(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu)
{
    return mCallback->OnPanelClosed(featureId, menu);
}

//@Override
ECode WindowCallbackWrapper::OnPreparePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IView* view,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    return mCallback->OnPreparePanel(featureId, view, menu, result);
}

//@Override
ECode WindowCallbackWrapper::OnSearchRequested(
    /* [out] */ Boolean* result)
{
    return mCallback->OnSearchRequested(result);
}

//@Override
ECode WindowCallbackWrapper::OnWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    return mCallback->OnWindowAttributesChanged(attrs);
}

//@Override
ECode WindowCallbackWrapper::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    return mCallback->OnWindowFocusChanged(hasFocus);
}

//@Override
ECode WindowCallbackWrapper::OnWindowStartingActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    return mCallback->OnWindowStartingActionMode(callback, mode);
}

ECode WindowCallbackWrapper::OnWindowDismissed()
{
    // TODO(benm): implement me.
    return NOERROR;
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
