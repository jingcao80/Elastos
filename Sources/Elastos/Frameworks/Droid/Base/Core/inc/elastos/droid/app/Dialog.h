//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_APP_DIALOG_H__
#define __ELASTOS_DROID_APP_DIALOG_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Runnable.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnShowListener;
using Elastos::Droid::Content::IDialogInterfaceOnKeyListener;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IOnWindowDismissedCallback;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::IContextMenuInfo;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IViewOnCreateContextMenuListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC Dialog
    : public Object
    , public IDialog
    , public IDialogInterface
    , public IWindowCallback
    , public IKeyEventCallback
    , public IViewOnCreateContextMenuListener
    , public IOnWindowDismissedCallback
{
private:
    ECO_LOCAL static const Int32 DISMISS = 0x43;
    ECO_LOCAL static const Int32 CANCEL = 0x44;
    ECO_LOCAL static const Int32 SHOW = 0x45;

    ECO_LOCAL static const String DIALOG_SHOWING_TAG;
    ECO_LOCAL static const String DIALOG_HIERARCHY_TAG;

private:
    class ECO_LOCAL DismissAction
        : public Runnable
    {
    public:
        DismissAction(
            /* [in] */ IWeakReference* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class ECO_LOCAL ListenersHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("Dialog::ListenersHandler")

        ListenersHandler(
            /* [in] */ IWeakReference* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class ECO_LOCAL ViewCreateContextMenuListener
        : public Object
        , public IViewOnCreateContextMenuListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewCreateContextMenuListener(
            /* [in] */ Dialog* host);

        CARAPI OnCreateContextMenu(
            /* [in] */ IContextMenu* menu,
            /* [in] */ IView* v,
            /* [in] */ IContextMenuInfo* menuInfo);

    private:
        Dialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    Dialog();

    virtual ~Dialog();

    /**
     * Create a Dialog window that uses the default dialog frame style.
     *
     * @param context The Context the Dialog is to run it.  In particular, it
     *                uses the window manager and theme in this context to
     *                present its UI.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Create a Dialog window that uses a custom dialog style.
     *
     * @param context The Context in which the Dialog should run. In particular, it
     *                uses the window manager and theme from this context to
     *                present its UI.
     * @param theme A style resource describing the theme to use for the
     * window. See <a href="{@docRoot}guide/topics/resources/available-resources.html#stylesandthemes">Style
     * and Theme Resources</a> for more information about defining and using
     * styles.  This theme is applied on top of the current theme in
     * <var>context</var>.  If 0, the default dialog theme will be used.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ Boolean createContextThemeWrapper);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean cancelable,
        /* [in] */ IDialogInterfaceOnCancelListener* cancelListener);

    /**
     * Retrieve the Context this Dialog is running in.
     *
     * @return Context The Context used by the Dialog.
     */
    CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * Retrieve the {@link ActionBar} attached to this dialog, if present.
     *
     * @return The ActionBar attached to the dialog or null if no ActionBar is present.
     */
    CARAPI GetActionBar(
        /* [out] */ IActionBar** actionBar);

    CARAPI SetOwnerActivity(
        /* [in] */ IActivity* activity);

    CARAPI GetOwnerActivity(
        /* [out] */ IActivity** activity);

    CARAPI IsShowing(
        /* [out] */ Boolean* showing);

    CARAPI Create();

    CARAPI Show();

    CARAPI Hide();

    CARAPI Dismiss();

    CARAPI DispatchOnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnSaveInstanceState(
        /* [out] */ IBundle** result);

    CARAPI OnRestoreInstanceState(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI GetWindow(
        /* [out] */ IWindow** window);

    CARAPI GetCurrentFocus(
        /* [out] */ IView** view);

   /**
     * Finds a child view with the given identifier. Returns null if the
     * specified child view does not exist or the dialog has not yet been fully
     * created (for example, via {@link #show()} or {@link #create()}).
     *
     * @param id the identifier of the view to find
     * @return The view with the given id or null.
     */
    CARAPI FindViewById(
        /* [in] */ Int32 id,
        /* [out] */ IView** view);

    CARAPI SetContentView(
        /* [in] */ Int32 layoutResID);

    CARAPI SetContentView(
        /* [in] */ IView* view);

    CARAPI SetContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI AddContentView(
        /* [in] */ IView* view,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetTitle(
        /* [in] */ Int32 titleId);

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnBackPressed();

    CARAPI OnKeyShortcut(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Called when a generic motion event was not handled by any of the
     * views inside of the dialog.
     * <p>
     * Generic motion events describe joystick movements, mouse hovers, track pad
     * touches, scroll wheel movements and other input events.  The
     * {@link MotionEvent#getSource() source} of the motion event specifies
     * the class of input that was received.  Implementations of this method
     * must examine the bits in the source before processing the event.
     * The following code example shows how this is done.
     * </p><p>
     * Generic motion events with source class
     * {@link android.view.InputDevice#SOURCE_CLASS_POINTER}
     * are delivered to the view under the pointer.  All other generic motion events are
     * delivered to the focused view.
     * </p><p>
     * See {@link View#onGenericMotionEvent(MotionEvent)} for an example of how to
     * handle this event.
     * </p>
     *
     * @param event The generic motion event being processed.
     *
     * @return Return true if you have consumed the event, false if you haven't.
     * The default implementation always returns false.
     */
    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnWindowAttributesChanged(
        /* [in] */ IWindowManagerLayoutParams* params);

    CARAPI OnContentChanged();

    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasFocus);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    /** @hide */
    //@Override
    CARAPI OnWindowDismissed();

    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
     * Called to process a key shortcut event.
     * You can override this to intercept all key shortcut events before they are
     * dispatched to the window.  Be sure to call this implementation for key shortcut
     * events that should be handled normally.
     *
     * @param event The key shortcut event.
     * @return True if this event was consumed.
     */
    CARAPI DispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI DispatchTrackballEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    /**
     * Called to process generic motion events.  You can override this to
     * intercept all generic motion events before they are dispatched to the
     * window.  Be sure to call this implementation for generic motion events
     * that should be handled normally.
     *
     * @param ev The generic motion event.
     *
     * @return boolean Return true if this event was consumed.
     */
    CARAPI DispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnCreatePanelView(
        /* [in] */ Int32 featureId,
        /* [out] */ IView** view);

    CARAPI OnCreatePanelMenu(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnPreparePanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IView* view,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnMenuOpened(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnMenuItemSelected(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    CARAPI OnPanelClosed(
        /* [in] */ Int32 featureId,
        /* [in] */ IMenu* menu);

    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    CARAPI OnOptionsMenuClosed(
        /* [in] */ IMenu* menu);

    CARAPI OpenOptionsMenu();

    CARAPI CloseOptionsMenu();

    /**
     * @see Activity#invalidateOptionsMenu()
     */
    CARAPI InvalidateOptionsMenu();

    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

    CARAPI RegisterForContextMenu(
        /* [in] */ IView* view);

    CARAPI UnregisterForContextMenu(
        /* [in] */ IView* view);

    CARAPI OpenContextMenu(
        /* [in] */ IView* view);

    CARAPI OnContextItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    CARAPI OnContextMenuClosed(
        /* [in] */ IMenu* menu);

    CARAPI OnSearchRequested(
        /* [out] */ Boolean* result);

    CARAPI OnWindowStartingActionMode(
        /* [in] */ IActionModeCallback* callback,
        /* [out] */ IActionMode** mode);

    /**
     * {@inheritDoc}
     *
     * Note that if you override this method you should always call through
     * to the superclass implementation by calling super.onActionModeStarted(mode).
     */
    CARAPI OnActionModeStarted(
        /* [in] */ IActionMode* mode);

    /**
     * {@inheritDoc}
     *
     * Note that if you override this method you should always call through
     * to the superclass implementation by calling super.onActionModeFinished(mode).
     */
    CARAPI OnActionModeFinished(
        /* [in] */ IActionMode* mode);

    CARAPI TakeKeyEvents(
        /* [in] */ Boolean get);

    CARAPI RequestWindowFeature(
        /* [in] */ Int32 featureId,
        /* [out] */ Boolean* result);

    CARAPI SetFeatureDrawableResource(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 resId);

    CARAPI SetFeatureDrawableUri(
        /* [in] */ Int32 featureId,
        /* [in] */ IUri* uri);

    CARAPI SetFeatureDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable);

    CARAPI SetFeatureDrawableAlpha(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 alpha);

    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    CARAPI SetCancelable(
        /* [in] */ Boolean flag);

    CARAPI SetCanceledOnTouchOutside(
        /* [in] */ Boolean cancel);

    CARAPI Cancel();

    CARAPI SetOnCancelListener(
        /* [in] */ IDialogInterfaceOnCancelListener* listener);

    CARAPI SetOnDismissListener(
        /* [in] */ IDialogInterfaceOnDismissListener* listener);

    CARAPI SetOnShowListener(
        /* [in] */ IDialogInterfaceOnShowListener* listener);

    CARAPI SetCancelMessage(
        /* [in] */ IMessage* msg);

    CARAPI SetDismissMessage(
        /* [in] */ IMessage* msg);

    CARAPI TakeCancelAndDismissListeners(
        /* [in] */ const String& msg,
        /* [in] */ IDialogInterfaceOnCancelListener* cancel,
        /* [in] */ IDialogInterfaceOnDismissListener* dismiss,
        /* [out] */ Boolean* result);

    CARAPI SetVolumeControlStream(
        /* [in] */ Int32 streamType);

    CARAPI GetVolumeControlStream(
        /* [out] */ Int32* volume);

    CARAPI SetOnKeyListener(
        /* [in] */ IDialogInterfaceOnKeyListener* onKeyListener);

    static CARAPI_(Int32) GetResourceId(
            /* [in] */ IContext* context,
            /* [in] */ Int32 attrId);

protected:
    virtual CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    virtual CARAPI OnStart();

    virtual CARAPI OnStop();

private:
    CARAPI_(void) DismissDialog();

    CARAPI_(void) SendDismissMessage();

    CARAPI_(void) SendShowMessage();

    CARAPI_(AutoPtr<IComponentName>) GetAssociatedActivity();

public:
    AutoPtr<IContext> mContext;     // memory leak! IContext may hold Dialog. luo.zhaohui
    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IWindow> mWindow;
    AutoPtr<IView> mDecor;

    /**
     * This field should be made private, so it is hidden from the SDK.
     * {@hide}
     */
    Boolean mCancelable;

protected:
    AutoPtr<IViewOnCreateContextMenuListener> mViewCreateContextMenuListener;

private:
    ECO_LOCAL const static String TAG;

    AutoPtr<IWeakReference> mWeakOwnerActivity;
    // AutoPtr<IActivity> mOwnerActivity;
    AutoPtr<IActionBar> mActionBar;

    String mCancelAndDismissTaken;
    AutoPtr<IMessage> mCancelMessage;
    AutoPtr<IMessage> mDismissMessage;
    AutoPtr<IMessage> mShowMessage;

    AutoPtr<IDialogInterfaceOnCancelListener> mCancelListener;
    AutoPtr<IDialogInterfaceOnDismissListener> mDismissListener;
    AutoPtr<IDialogInterfaceOnShowListener> mShowListener;

    AutoPtr<IDialogInterfaceOnKeyListener> mOnKeyListener;

    Boolean mCreated;
    Boolean mShowing;
    Boolean mCanceled;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ListenersHandler> mListenersHandler;

    AutoPtr<IActionMode> mActionMode;

    AutoPtr<IRunnable> mDismissAction;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_DIALOG_H__
