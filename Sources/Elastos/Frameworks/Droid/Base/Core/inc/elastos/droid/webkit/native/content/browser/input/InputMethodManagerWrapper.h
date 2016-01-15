
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_INPUTMETHODMANAGERWRAPPER_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_INPUTMETHODMANAGERWRAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::InputMethod::IInputMethodManager;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * Wrapper around Android's InputMethodManager
 */
class InputMethodManagerWrapper : public Object
{
public:
    InputMethodManagerWrapper(
        /* [in] */ IContext* context);

    /**
     * @see android.view.inputmethod.InputMethodManager#restartInput(View)
     */
    CARAPI_(void) RestartInput(
        /* [in] */ IView* view);

    /**
     * @see android.view.inputmethod.InputMethodManager#showSoftInput(View, int, ResultReceiver)
     */
    CARAPI_(void) ShowSoftInput(
        /* [in] */ IView* view,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    /**
     * @see android.view.inputmethod.InputMethodManager#isActive(View)
     */
    CARAPI_(Boolean) IsActive(
        /* [in] */ IView* view);

    /**
     * @see InputMethodManager#hideSoftInputFromWindow(IBinder, int, ResultReceiver)
     */
    CARAPI_(Boolean) HideSoftInputFromWindow(
        /* [in] */ IBinder* windowToken,
        /* [in] */ Int32 flags,
        /* [in] */ IResultReceiver* resultReceiver);

    /**
     * @see android.view.inputmethod.InputMethodManager#updateSelection(View, int, int, int, int)
     */
    CARAPI_(void) UpdateSelection(
        /* [in] */ IView* view,
        /* [in] */ Int32 selStart,
        /* [in] */ Int32 selEnd,
        /* [in] */ Int32 candidatesStart,
        /* [in] */ Int32 candidatesEnd);

    /**
     * @see android.view.inputmethod.InputMethodManager#isWatchingCursor(View)
     */
    CARAPI_(Boolean) IsWatchingCursor(
        /* [in] */ IView* view);

    /**
     * @see android.view.inputmethod.InputMethodManager#updateCursor(View, int, int, int, int)
     */
    CARAPI_(void) UpdateCursor(
        /* [in] */ IView* view,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(AutoPtr<IInputMethodManager>) GetInputMethodManager();

private:
    const AutoPtr<IContext> mContext;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_INPUTMETHODMANAGERWRAPPER_H__
