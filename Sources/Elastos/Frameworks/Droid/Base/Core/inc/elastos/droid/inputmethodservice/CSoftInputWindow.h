
#ifndef  __ELASTOS_DROID_INPUTMETHODSERVICE_CSOFTINPUTWINDOW_H__
#define  __ELASTOS_DROID_INPUTMETHODSERVICE_CSOFTINPUTWINDOW_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_InputMethodService_CSoftInputWindow.h"
#include "elastos/droid/app/Dialog.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Dialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

/**
 * A SoftInputWindow is a Dialog that is intended to be used for a top-level input
 * method window.  It will be displayed along the edge of the screen, moving
 * the application user interface away from it so that the focused item is
 * always visible.
 */
CarClass(CSoftInputWindow)
    , public Dialog
    , public ISoftInputWindow
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CSoftInputWindow();

    CARAPI SetToken(
        /* [in] */ IBinder* token);

    // @Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * Set which boundary of the screen the DockWindow sticks to.
     *
     * @param gravity The boundary of the screen to stick. See {#link
     *        android.view.Gravity.LEFT}, {#link android.view.Gravity.TOP},
     *        {#link android.view.Gravity.BOTTOM}, {#link
     *        android.view.Gravity.RIGHT}.
     */
    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    virtual CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    virtual CARAPI OnBackPressed();

    /**
     * Create a DockWindow that uses a custom style.
     *
     * @param context The Context in which the DockWindow should run. In
     *        particular, it uses the window manager and theme from this context
     *        to present its UI.
     * @param theme A style resource describing the theme to use for the window.
     *        See <a href="{@docRoot}reference/available-resources.html#stylesandthemes">Style
     *        and Theme Resources</a> for more information about defining and
     *        using styles. This theme is applied on top of the current theme in
     *        <var>context</var>. If 0, the default dialog theme will be used.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& name,
        /* [in] */ Int32 theme,
        /* [in] */ ISoftInputWindowCallback* callback,
        /* [in] */ IKeyEventCallback* keyEventCallback,
        /* [in] */ IDispatcherState* dispatcherState,
        /* [in] */ Int32 windowType,
        /* [in] */ Int32 gravity,
        /* [in] */ Boolean takesFocus);

private:
    CARAPI_(void) InitDockWindow();

    CARAPI_(void) UpdateWidthHeight(
        /* [in] */ IWindowManagerLayoutParams* lp);

private:
    String mName;
    AutoPtr<ISoftInputWindowCallback> mCallback;
    AutoPtr<IKeyEventCallback> mKeyEventCallback;
    AutoPtr<IDispatcherState> mDispatcherState;
    Int32 mWindowType;
    Int32 mGravity;
    Boolean mTakesFocus;
    AutoPtr<IRect> mBounds;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INPUTMETHODSERVICE_CSOFTINPUTWINDOW_H__
