
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/WindowCallbackWrapper.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(WindowCallbackWrapper, Object, IWindowCallback)

WindowCallbackWrapper::WindowCallbackWrapper()
{
}

ECode WindowCallbackWrapper::constructor(
    /* [in] */ IWindowCallback* wrapped)
{
    if (wrapped == NULL) {
        // throw new IllegalArgumentException("Window callback may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mWrapped = wrapped;
    return NOERROR;
}

ECode WindowCallbackWrapper::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->DispatchKeyEvent(event, result);
}

ECode WindowCallbackWrapper::DispatchKeyShortcutEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->DispatchKeyShortcutEvent(event, result);
}

ECode WindowCallbackWrapper::DispatchTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->DispatchTouchEvent(event, result);
}

ECode WindowCallbackWrapper::DispatchTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->DispatchTrackballEvent(event, result);
}

ECode WindowCallbackWrapper::DispatchGenericMotionEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->DispatchGenericMotionEvent(event, result);
}

ECode WindowCallbackWrapper::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->DispatchPopulateAccessibilityEvent(event, result);
}

ECode WindowCallbackWrapper::OnCreatePanelView(
    /* [in] */ Int32 featureId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mWrapped->OnCreatePanelView(featureId, view);
}

ECode WindowCallbackWrapper::OnCreatePanelMenu(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->OnCreatePanelMenu(featureId, menu, result);
}

ECode WindowCallbackWrapper::OnPreparePanel(
    /* [in] */ Int32 featureId,
    /* [in] */ IView* view,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->OnPreparePanel(featureId, view, menu, result);
}

ECode WindowCallbackWrapper::OnMenuOpened(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->OnMenuOpened(featureId, menu, result);
}

ECode WindowCallbackWrapper::OnMenuItemSelected(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->OnMenuItemSelected(featureId, item, result);
}

ECode WindowCallbackWrapper::OnWindowAttributesChanged(
    /* [in] */ IWindowManagerLayoutParams* attrs)
{
    return mWrapped->OnWindowAttributesChanged(attrs);
}

ECode WindowCallbackWrapper::OnContentChanged()
{
    return mWrapped->OnContentChanged();
}

ECode WindowCallbackWrapper::OnWindowFocusChanged(
    /* [in] */ Boolean hasFocus)
{
    return mWrapped->OnWindowFocusChanged(hasFocus);
}

ECode WindowCallbackWrapper::OnAttachedToWindow()
{
    return mWrapped->OnAttachedToWindow();
}

ECode WindowCallbackWrapper::OnDetachedFromWindow()
{
    return mWrapped->OnDetachedFromWindow();
}

ECode WindowCallbackWrapper::OnPanelClosed(
    /* [in] */ Int32 featureId,
    /* [in] */ IMenu* menu)
{
    return mWrapped->OnPanelClosed(featureId, menu);
}

ECode WindowCallbackWrapper::OnSearchRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mWrapped->OnSearchRequested(result);
}

ECode WindowCallbackWrapper::OnWindowStartingActionMode(
    /* [in] */ IActionModeCallback* callback,
    /* [out] */ IActionMode** mode)
{
    VALIDATE_NOT_NULL(mode);
    return mWrapped->OnWindowStartingActionMode(callback, mode);
}

ECode WindowCallbackWrapper::OnActionModeStarted(
    /* [in] */ IActionMode* mode)
{
    return mWrapped->OnActionModeStarted(mode);
}

ECode WindowCallbackWrapper::OnActionModeFinished(
    /* [in] */ IActionMode* mode)
{
    return mWrapped->OnActionModeFinished(mode);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
