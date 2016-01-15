
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/native/content/browser/input/InputMethodManagerWrapper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

InputMethodManagerWrapper::InputMethodManagerWrapper(
    /* [in] */ IContext* context)
    : mContext(context)
{
}

AutoPtr<IInputMethodManager> InputMethodManagerWrapper::GetInputMethodManager()
{
    AutoPtr<IInputMethodManager> manager;
    mContext->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&manager);
    return manager;
}

/**
 * @see android.view.inputmethod.InputMethodManager#restartInput(View)
 */
void InputMethodManagerWrapper::RestartInput(
    /* [in] */ IView* view)
{
    GetInputMethodManager()->RestartInput(view);
}

/**
 * @see android.view.inputmethod.InputMethodManager#showSoftInput(View, int, ResultReceiver)
 */
void InputMethodManagerWrapper::ShowSoftInput(
    /* [in] */ IView* view,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    Boolean result = FALSE;
    GetInputMethodManager()->ShowSoftInput(view, flags, resultReceiver, &result);
}

/**
 * @see android.view.inputmethod.InputMethodManager#isActive(View)
 */
Boolean InputMethodManagerWrapper::IsActive(
    /* [in] */ IView* view)
{
    Boolean bFlag = FALSE;
    GetInputMethodManager()->IsActive(view, &bFlag);
    return bFlag;
}

/**
 * @see InputMethodManager#hideSoftInputFromWindow(IBinder, int, ResultReceiver)
 */
Boolean InputMethodManagerWrapper::HideSoftInputFromWindow(
    /* [in] */ IBinder* windowToken,
    /* [in] */ Int32 flags,
    /* [in] */ IResultReceiver* resultReceiver)
{
    Boolean bFlag = FALSE;
    GetInputMethodManager()->HideSoftInputFromWindow(windowToken, flags, resultReceiver, &bFlag);
    return bFlag;
}

/**
 * @see android.view.inputmethod.InputMethodManager#updateSelection(View, int, int, int, int)
 */
void InputMethodManagerWrapper::UpdateSelection(
    /* [in] */ IView* view,
    /* [in] */ Int32 selStart,
    /* [in] */ Int32 selEnd,
    /* [in] */ Int32 candidatesStart,
    /* [in] */ Int32 candidatesEnd)
{
    GetInputMethodManager()->UpdateSelection(view, selStart, selEnd, candidatesStart,
            candidatesEnd);
}

/**
 * @see android.view.inputmethod.InputMethodManager#isWatchingCursor(View)
 */
Boolean InputMethodManagerWrapper::IsWatchingCursor(
    /* [in] */ IView* view)
{
    Boolean bFlag = FALSE;
    GetInputMethodManager()->IsWatchingCursor(view, &bFlag);
    return bFlag;
}

/**
 * @see android.view.inputmethod.InputMethodManager#updateCursor(View, int, int, int, int)
 */
void InputMethodManagerWrapper::UpdateCursor(
    /* [in] */ IView* view,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    GetInputMethodManager()->UpdateCursor(view, left, top, right, bottom);
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
