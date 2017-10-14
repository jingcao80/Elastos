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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOW_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOW_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Transition.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/view/Window.h"
#include "elastos/droid/view/GestureDetector.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::View::IRootViewSurfaceTaker;
using Elastos::Droid::Internal::View::Menu::IContextMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IIconMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IListMenuPresenter;
using Elastos::Droid::Internal::View::Menu::IMenuBuilder;
using Elastos::Droid::Internal::View::Menu::IMenuBuilderCallback;
using Elastos::Droid::Internal::View::Menu::IMenuDialogHelper;
using Elastos::Droid::Internal::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::Internal::View::Menu::IMenuView;
using Elastos::Droid::Internal::View::IRootViewSurfaceTaker;
using Elastos::Droid::Internal::Widget::IActionBarContextView;
using Elastos::Droid::Internal::Widget::IBackgroundFallback;
using Elastos::Droid::Internal::Widget::IDecorContentParent;
using Elastos::Droid::Internal::Widget::IOnDismissedListener;
using Elastos::Droid::Internal::Widget::IOnSwipeProgressChangedListener;
using Elastos::Droid::Internal::Widget::ISwipeDismissLayout;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Runnable;
using Elastos::Droid::Transition::IScene;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Transition::ITransitionManager;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputQueueCallback;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ISurfaceHolderCallback2;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewRootImpl;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::IOnWindowDismissedCallback;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::Window;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

class PhoneWindow
    : public Window
    , public IPhoneWindow
    , public IMenuBuilderCallback
{
    friend class ViewRootImpl;
    friend class PhoneWindowRotationWatcher;

private:
    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("PhoneWindow::SettingsObserver")

        SettingsObserver(
            /* [in] */ PhoneWindow* host);

        ~SettingsObserver();

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        CARAPI Observe();

        CARAPI Unobserve();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        CARAPI CheckGestures();

    private:
        PhoneWindow* mHost;
    };

    class InvalidatePanelMenuRunnable
        : public Runnable
    {
    public:
        InvalidatePanelMenuRunnable(
            /* [in] */ PhoneWindow* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class WindowManagerHolder
    {
    public:
        static CARAPI_(AutoPtr<IIWindowManager>) GetWindowManager();

    private:
        static AutoPtr<IIWindowManager> sWindowManager;
    };

    class PanelMenuPresenterCallback
        : public Object
        , public IMenuPresenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        PanelMenuPresenterCallback(
            /* [in] */ PhoneWindow* host);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class ActionMenuPresenterCallback
        : public Object
        , public IMenuPresenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        ActionMenuPresenterCallback(
            /* [in] */ PhoneWindow* owner);

        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* result);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class _DecorView
        : public FrameLayout
        , public IRootViewSurfaceTaker
    {
    private:
        class ShowActionModePopupRunnable
            : public Runnable
        {
        public:
            ShowActionModePopupRunnable(
                /* [in] */ _DecorView* host);

            CARAPI Run();

        private:
            _DecorView* mHost;
        };

        /**
         * Clears out internal reference when the action mode is destroyed.
         */
        class ActionModeCallbackWrapper
            : public Object
            , public IActionModeCallback
        {
        public:
            CAR_INTERFACE_DECL()

            ActionModeCallbackWrapper(
                /* [in] */ IActionModeCallback* wrapped,
                /* [in] */ _DecorView* host);

            CARAPI OnCreateActionMode(
                /* [in] */ IActionMode* mode,
                /* [in] */ IMenu* menu,
                /* [out] */ Boolean* result);

            CARAPI OnPrepareActionMode(
                /* [in] */ IActionMode* mode,
                /* [in] */ IMenu* menu,
                /* [out] */ Boolean* result);

            CARAPI OnActionItemClicked(
                /* [in] */ IActionMode* mode,
                /* [in] */ IMenuItem* item,
                /* [out] */ Boolean* result);

            CARAPI OnDestroyActionMode(
                /* [in] */ IActionMode* mode);

        private:
            AutoPtr<IActionModeCallback> mWrapped;
            _DecorView* mHost;
        };

        class StylusGestureFilter
            : public Object
        {
        private:
            class StylusOnGestureListener
                : public GestureDetector::SimpleOnGestureListener
            {
            public:
                StylusOnGestureListener(
                    /* [in] */ StylusGestureFilter* host);

                CARAPI OnFling(
                    /* [in] */ IMotionEvent* e1,
                    /* [in] */ IMotionEvent* e2,
                    /* [in] */ Float velocityX,
                    /* [in] */ Float velocityY,
                    /* [out] */ Boolean* res);

                CARAPI OnDoubleTap(
                    /* [in] */ IMotionEvent* e,
                    /* [out] */ Boolean* res);

                CARAPI OnLongPress(
                    /* [in] */ IMotionEvent* e);
            private:
                StylusGestureFilter* mHost;
            };
        public:
            StylusGestureFilter(
                /* [in] */ _DecorView* host);

            CARAPI constructor();

            CARAPI OnTouchEvent(
                /* [in] */ IMotionEvent* e,
                /* [out] */ Boolean* res);

            CARAPI OnFling(
                /* [in] */ IMotionEvent* e1,
                /* [in] */ IMotionEvent* e2,
                /* [in] */ Float velocityX,
                /* [in] */ Float velocityY,
                /* [out] */ Boolean* res);

            CARAPI OnDoubleTap(
                /* [in] */ IMotionEvent* e,
                /* [out] */ Boolean* res);

            CARAPI OnLongPress(
                /* [in] */ IMotionEvent* e);

        private:
            friend class _DecorView;

            static const Int32 SWIPE_UP;// = 1;
            static const Int32 SWIPE_DOWN;// = 2;
            static const Int32 SWIPE_LEFT;// = 3;
            static const Int32 SWIPE_RIGHT;// = 4;
            static const Int32 PRESS_LONG;// = 5;
            static const Int32 TAP_DOUBLE;// = 6;
            static const Double SWIPE_MIN_DISTANCE;// = 25.0;
            static const Double SWIPE_MIN_VELOCITY;// = 50.0;
            static const Int32 KEY_NO_ACTION;// = 1000;
            static const Int32 KEY_HOME;// = 1001;
            static const Int32 KEY_BACK;// = 1002;
            static const Int32 KEY_MENU;// = 1003;
            static const Int32 KEY_SEARCH;// = 1004;
            static const Int32 KEY_RECENT;// = 1005;
            static const Int32 KEY_APP;// = 1006;
            AutoPtr<IGestureDetector> mDetector;
            static const String TAG;// = "StylusGestureFilter";

            _DecorView* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("PhoneWindow::DecorView")

        _DecorView(
            /* [in] */ PhoneWindow* host);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 featureId);

        CARAPI WillYouTakeTheSurface(
            /* [out] */ ISurfaceHolderCallback2** cback);

        CARAPI SetSurfaceType(
            /* [in] */ Int32 type);

        CARAPI SetSurfaceFormat(
            /* [in] */ Int32 format);

        CARAPI SetSurfaceKeepScreenOn(
            /* [in] */ Boolean keepOn);

        CARAPI WillYouTakeTheInputQueue(
            /* [out] */ IInputQueueCallback** inputQueueCallback);

        CARAPI StartChanging();

        CARAPI FinishChanging();

        CARAPI SetWindowBackground(
            /* [in] */ IDrawable* drawable);

        CARAPI SetBackgroundDrawable(
            /* [in] */ IDrawable* d);

        CARAPI SetBackgroundFallback(
            /* [in] */ Int32 resId);

        // @Override
        CARAPI_(void) OnDraw(
            /* [in] */ ICanvas* c);

        // @Override ViewGroup
        CARAPI DispatchKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI DispatchKeyShortcutEvent(
            /* [in] */ IKeyEvent* ev,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI DispatchTouchEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI DispatchTrackballEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI DispatchGenericMotionEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

        virtual CARAPI SuperDispatchKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI SuperDispatchTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI SuperDispatchTrackballEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI SuperDispatchGenericMotionEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI SuperDispatchKeyShortcutEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI DispatchApplyWindowInsets(
            /* [in] */ IWindowInsets* insets,
            /* [out] */ IWindowInsets** result);

        // @Override
        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnInterceptTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI SetWindowFrame(
            /* [in] */ IDrawable* drawable);

        // @Override
        CARAPI OnWindowSystemUiVisibilityChanged(
            /* [in] */ Int32 visible);

        // @Override IView
        CARAPI OnApplyWindowInsets(
            /* [in] */ IWindowInsets* insets,
            /* [out] */ IWindowInsets** result);

        // @Override ViewGroup
        CARAPI IsTransitionGroup(
            /* [out] */ Boolean* result);

        CARAPI OnWindowFocusChanged(
            /* [in] */ Boolean hasWindowFocus);

        CARAPI OnCloseSystemDialogs(
            /* [in] */ const String& reason);

        // @Override
        CARAPI ShowContextMenuForChild(
            /* [in] */ IView* originalView,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SendAccessibilityEvent(
            /* [in] */ Int32 eventType);

        // @Override
        CARAPI DispatchPopulateAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Draw(
            /* [in] */ ICanvas* canvas);

        // @Override
        CARAPI StartActionModeForChild(
            /* [in] */ IView* originalView,
            /* [in] */ IActionModeCallback* callback,
            /* [out] */ IActionMode** res);

        // @Override
        CARAPI StartActionMode(
            /* [in] */ IActionModeCallback* callback,
            /* [out] */ IActionMode** res);

    protected:
        CARAPI_(Boolean) SetFrame(
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 r,
            /* [in] */ Int32 b);

        CARAPI OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        CARAPI UpdateWindowResizeState();

        CARAPI OnDetachedFromWindow();

    private:
        CARAPI MenuAction();

        CARAPI BackAction();

        CARAPI DispatchStylusAction(
            /* [in] */ Int32 gestureAction);

        CARAPI_(void) DrawableChanged();

        CARAPI OnAttachedToWindow();

        CARAPI_(Boolean) IsOutOfBounds(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        CARAPI_(AutoPtr<IWindowInsets>) UpdateColorViews(
            /* [in] */ IWindowInsets* insets);

        CARAPI_(AutoPtr<IView>) UpdateColorViewInt(
            /* [in] */ IView* view,
            /* [in] */ Int32 sysUiVis,
            /* [in] */ Int32 systemUiHideFlag,
            /* [in] */ Int32 translucentFlag,
            /* [in] */ Int32 color,
            /* [in] */ Int32 height,
            /* [in] */ Int32 verticalGravity,
            /* [in] */ const String& transitionName,
            /* [in] */ Int32 id,
            /* [in] */ Boolean hiddenByWindowFlag);

        CARAPI_(AutoPtr<IWindowInsets>) UpdateStatusGuard(
            /* [in] */ IWindowInsets* insets);

        CARAPI_(void) UpdateNavigationGuard(
            /* [in] */ IWindowInsets* insets);

        CARAPI_(AutoPtr<IWindowCallback>) GetCallback();

        CARAPI_(Boolean) IsDestroyed();

    protected:
        AutoPtr<PhoneWindow> mHost; // hold host

    private:

        /** The feature ID of the panel, or -1 if this is the application's DecorView */
        Int32 mFeatureId;

        AutoPtr<CRect> mDrawingBounds;
        AutoPtr<CRect> mBackgroundPadding;
        AutoPtr<CRect> mFramePadding;
        AutoPtr<CRect> mFrameOffsets;

        Boolean mChanging;

        AutoPtr<IDrawable> mMenuBackground;
        Boolean mWatchingForMenu;
        Int32 mDownY;

        AutoPtr<IActionMode> mActionMode;
        AutoPtr<IActionBarContextView> mActionModeView;
        AutoPtr<IPopupWindow> mActionModePopup;
        AutoPtr<IRunnable> mShowActionModePopup;

        // View added at runtime to draw under the status bar area
        AutoPtr<IView> mStatusGuard;
        // View added at runtime to draw under the navigation bar area
        AutoPtr<IView> mNavigationGuard;
        AutoPtr<SettingsObserver> mSettingsObserver;

        AutoPtr<IView> mStatusColorView;
        AutoPtr<IView> mNavigationColorView;
        AutoPtr<IBackgroundFallback> mBackgroundFallback;

        Int32 mLastTopInset;
        Int32 mLastBottomInset;
        Int32 mLastRightInset;

        AutoPtr<StylusGestureFilter> mStylusFilter;
        friend class ActionModeCallbackWrapper;
        friend class PhoneWindow;

    public:
        Int32 mDefaultOpacity;
    };

    class DecorView
       : public _DecorView
    {
    private:
        class DecorViewWeakReferenceImpl
           : public Object
           , public IWeakReference
        {
        public:
            CAR_INTERFACE_DECL()

            DecorViewWeakReferenceImpl(
                /* [in] */ IInterface* object,
                /* [in] */ ElRefBase::WeakRefType* ref);

            ~DecorViewWeakReferenceImpl();

            CARAPI Resolve(
               /* [in] */ const InterfaceID& riid,
               /* [out] */ IInterface** objectReference);

        private:
            IInterface* mObject;
            ElRefBase::WeakRefType* mRef;
        };

    public:
       DecorView(
            /* [in] */ PhoneWindow* host);

       ~DecorView();

       CARAPI constructor(
           /* [in] */ IContext* context,
           /* [in] */ Int32 featureId,
           /* [in] */ Boolean useSelfRef = FALSE);

       CARAPI_(PInterface) Probe(
           /* [in] */ REIID riid);

       CARAPI GetInterfaceID(
           /* [in] */ IInterface *pObject,
           /* [out] */ InterfaceID *pIID);

       CARAPI_(UInt32) AddRef();

       CARAPI_(UInt32) Release();

       CARAPI_(UInt32) _AddRef();

       CARAPI_(UInt32) _Release();

       CARAPI GetWeakReference(
           /* [out] */ IWeakReference** weakReference);

    private:
       Boolean mUseSelfRef;
    };

public:
    class PanelFeatureState
        : public Object
    {
    public:
        class SavedState
            : public Object
            , public IPhoneWindowSavedState
            , public IParcelable
        {
        public:
            CAR_INTERFACE_DECL()

            SavedState();

            CARAPI constructor();

            CARAPI ReadFromParcel(
                /* [in] */ IParcel* in);

            //@Override
            CARAPI WriteToParcel(
                /* [in] */ IParcel* out);

        public:
            Int32 mFeatureId;
            Boolean mIsOpen;
            Boolean mIsInExpandedMode;
            AutoPtr<IBundle> mMenuState;
        };

    public:
        PanelFeatureState(
            /* [in] */ PhoneWindow* owner,
            /* [in] */ Int32 featureId);

        virtual CARAPI IsInListMode(
            /* [out] */ Boolean* result);

        virtual CARAPI HasPanelItems(
            /* [out] */ Boolean* result);

        /**
          * Unregister and free attached MenuPresenters. They will be recreated as needed.
          */
        CARAPI ClearMenuPresenters();

        CARAPI SetStyle(
            /* [in] */ IContext* context);

        CARAPI SetMenu(
            /* [in] */ IMenuBuilder* menu);

        CARAPI_(AutoPtr<IMenuView>) GetListMenuView(
            /* [in] */ IContext* context,
            /* [in] */ IMenuPresenterCallback* cb);

        CARAPI_(AutoPtr<IMenuView>) GetIconMenuView(
            /* [in] */ IContext* context,
            /* [in] */ IMenuPresenterCallback* cb);

        CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

        CARAPI OnRestoreInstanceState(
            /* [in] */ IParcelable* state);

    protected:
        CARAPI_(void) ApplyFrozenState();

    public:
        /** Feature ID for this panel. */
        Int32 mFeatureId;
        // Information pulled from the style for this panel.
        Int32 mBackground;
        /** The background when the panel spans the entire available width. */
        Int32 mFullBackground;
        Int32 mGravity;
        Int32 mX;
        Int32 mY;
        Int32 mWindowAnimations;
        /** Dynamic state of the panel. */
        DecorView* mDecorView;
        /** The panel that was returned by onCreatePanelView(). */
        AutoPtr<IView> mCreatedPanelView;
        /** The panel that we are actually showing. */
        AutoPtr<IView> mShownPanelView;
        /** Use {@link #setMenu} to set this. */
        AutoPtr<IMenuBuilder> mMenu;
        AutoPtr<IIconMenuPresenter> mIconMenuPresenter;
        AutoPtr<IListMenuPresenter> mListMenuPresenter;
        /** true if this menu will show in single-list compact mode */
        Boolean mIsCompact;
        /** Theme resource ID for list elements of the panel menu */
        Int32 mListPresenterTheme;
        /**
         * Whether the panel has been prepared (see
         * {@link PhoneWindow#preparePanel}).
         */
        Boolean mIsPrepared;
        /**
         * Whether an item's action has been performed. This happens in obvious
         * scenarios (user clicks on menu item), but can also happen with
         * chording menu+(shortcut key).
         */
        Boolean mIsHandled;
        Boolean mIsOpen;
        /**
         * True if the menu is in expanded mode, false if the menu is in icon
         * mode
         */
        Boolean mIsInExpandedMode;
        Boolean mQwertyMode;
        Boolean mRefreshDecorView;
        Boolean mRefreshMenuContent;
        Boolean mWasLastOpen;
        Boolean mWasLastExpanded;
        /**
         * Contains the state of the menu when told to freeze.
         */
        AutoPtr<IBundle> mFrozenMenuState;
        /**
         * Contains the state of associated action views when told to freeze.
         * These are saved across invalidations.
         */
        AutoPtr<IBundle> mFrozenActionViewState;
        PhoneWindow* mOwner;
        friend class PhoneWindow;
    };

private:

    /**
      * Simple implementation of MenuBuilder.Callback that:
      * <li> Opens a submenu when selected.
      * <li> Calls back to the callback's onMenuItemSelected when an item is
      * selected.
      */
    class DialogMenuCallback
        : public Object
        , public IMenuBuilderCallback
        , public IMenuPresenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        DialogMenuCallback(
            /* [in] */ Int32 featureId,
            /* [in] */ PhoneWindow* host);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        CARAPI OnCloseSubMenu(
            /* [in] */ IMenuBuilder* menu);

        CARAPI OnMenuItemSelected(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* state);

        CARAPI OnMenuModeChange(
            /* [in] */ IMenuBuilder* menu);

        CARAPI OnOpenSubMenu(
            /* [in] */ IMenuBuilder* subMenu,
            /* [out] */ Boolean* state);

    private:
        Int32 mFeatureId;
        AutoPtr<IMenuDialogHelper> mSubMenuHelper;
        AutoPtr<IWeakReference> mWeakHost;
    };

    class DrawableFeatureState
        : public Object
    {
    public:
        DrawableFeatureState(
            /* [in] */ Int32 _featureId);

    public:
        Int32 mFeatureId;
        Int32 mResid;
        AutoPtr<IUri> mUri;
        AutoPtr<IDrawable> mLocal;
        AutoPtr<IDrawable> mChild;
        AutoPtr<IDrawable> mDef;
        AutoPtr<IDrawable> mCur;
        Int32 mAlpha;
        Int32 mCurAlpha;
    };

    class InnerSwipeDismissLayoutOnDismissedListener1
        : public Object
        , public IOnDismissedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSwipeDismissLayoutOnDismissedListener1(
            /* [in] */ PhoneWindow* owner);

        // @Override
        CARAPI OnDismissed(
            /* [in] */ ISwipeDismissLayout* layout);

    private:
        PhoneWindow* mOwner;
    };

    class InnerSwipeDismissLayoutOnSwipeProgressChangedListener1
        : public Object
        , public IOnSwipeProgressChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSwipeDismissLayoutOnSwipeProgressChangedListener1(
            /* [in] */ PhoneWindow* owner);

        // @Override
        CARAPI OnSwipeProgressChanged(
            /* [in] */ ISwipeDismissLayout* layout,
            /* [in] */ Float progress,
            /* [in] */ Float translate);

        // @Override
        CARAPI OnSwipeCancelled(
            /* [in] */ ISwipeDismissLayout* layout);

    private:
        static const Float ALPHA_DECREASE;
        Boolean mIsTranslucent;
        PhoneWindow* mOwner;
    };

   class MyMenuBuilderCallback
       : public Object
       , public IMenuBuilderCallback
   {
   public:
       CAR_INTERFACE_DECL()

       MyMenuBuilderCallback(
           /* [in] */ IWeakReference* host);

       CARAPI OnMenuItemSelected(
           /* [in] */ IMenuBuilder* menu,
           /* [in] */ IMenuItem* item,
           /* [out] */ Boolean* state);

       CARAPI OnMenuModeChange(
           /* [in] */ IMenuBuilder* menu);

   private:
       AutoPtr<IWeakReference> mWeakHost;
   };

public:
    CAR_INTERFACE_DECL()

    PhoneWindow();

    virtual ~PhoneWindow();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetContainer(
        /* [in] */ IWindow* container);

    CARAPI TakeSurface(
        /* [in] */ ISurfaceHolderCallback2* cb);

    CARAPI TakeInputQueue(
        /* [in] */ IInputQueueCallback* callback);

    CARAPI IsFloating(
        /* [out] */ Boolean* isFloating);

    CARAPI RequestFeature(
        /* [in] */ Int32 featureId,
        /* [out] */ Boolean* result);

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

    CARAPI GetCurrentFocus(
        /* [out] */ IView** view);

    /**
      * Return a LayoutInflater instance that can be used to inflate XML view layout
      * resources for use in this Window.
      *
      * @return LayoutInflater The shared LayoutInflater.
      */
    CARAPI GetLayoutInflater(
        /* [out] */ ILayoutInflater** inflater);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetTitleColor(
        /* [in] */ Int32 textColor);

    /**
     * Prepares the panel to either be opened or chorded. This creates the Menu
     * instance for the panel and populates it via the Activity callbacks.
     *
     * @param st The panel state to prepare.
     * @param event The event that triggered the preparing of the panel.
     * @return Whether the panel was prepared. If the panel should not be shown,
     *         returns false.
     */
    CARAPI PreparePanel(
        /* [in] */ PanelFeatureState* st,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* prepared);

    static CARAPI_(void) ClearMenuViews(
        /* [in] */ PanelFeatureState* st);

    CARAPI OpenPanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IKeyEvent* event);

    CARAPI ClosePanel(
        /* [in] */ Int32 featureId);

    /**
     * Closes the given panel.
     *
     * @param st The panel to be closed.
     * @param doCallback Whether to notify the callback that the panel was
     *            closed. If the panel is in the process of re-opening or
     *            opening another panel (e.g., menu opening a sub menu), the
     *            callback should not happen and this variable should be false.
     *            In addition, this method internally will only perform the
     *            callback if the panel is open.
     */
    CARAPI ClosePanel(
        /* [in] */ PanelFeatureState* st,
        /* [in] */ Boolean doCallback);

    CARAPI TogglePanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IKeyEvent* event);

    /**
     * Called when the panel key is pushed down.
     * @param featureId The feature ID of the relevant panel (defaults to FEATURE_OPTIONS_PANEL).
     * @param event The key event.
     * @return Whether the key was handled.
     */
    CARAPI OnKeyDownPanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* handled);

    /**
     * Called when the panel key is released.
     * @param featureId The feature ID of the relevant panel (defaults to FEATURE_OPTIONS_PANEL).
     * @param event The key event.
     */
    CARAPI OnKeyUpPanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IKeyEvent* event);

    CARAPI CloseAllPanels();

    CARAPI PerformPanelShortcut(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded);

    CARAPI PerformPanelIdentifierAction(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 id,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded);

    CARAPI PerformContextMenuIdentifierAction(
        /* [in] */ Int32 id,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* succeeded);

    CARAPI_(AutoPtr<PanelFeatureState>) FindMenuPanel(
        /* [in] */ IMenu* menu);

    CARAPI OnMenuItemSelected(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* state);

    //CARAPI OnCloseMenu(
    //    /* [in] */ IMenuBuilder* menu,
    //    /* [in] */ Boolean allMenusAreClosing);

    //CARAPI OnCloseSubMenu(
    //    /* [in] */ ISubMenuBuilder* menu);

    CARAPI OnMenuModeChange(
        /* [in] */ IMenuBuilder* menu);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //CARAPI SetBackgroundDrawableResource(
    //    /* [in] */ Int32 resid);

    CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* drawable);

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

    CARAPI SetFeatureInt(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 value);

    // @Override
    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetDefaultIcon(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetDefaultLogo(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI SetLocalFocus(
        /* [in] */ Boolean hasFocus,
        /* [in] */ Boolean inTouchMode);

    // @Override
    CARAPI InjectInputEvent(
        /* [in] */ IInputEvent* event);

    /**
     * Request that key events come to this activity. Use this if your activity
     * has no views with focus, but the activity still wants a chance to process
     * key events.
     */
    CARAPI TakeKeyEvents(
        /* [in] */ Boolean get);

    CARAPI SuperDispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* succeeded);

    CARAPI SuperDispatchKeyShortcutEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* succeeded);

    CARAPI SuperDispatchTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* succeeded);

    CARAPI SuperDispatchTrackballEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* succeeded);

    CARAPI SuperDispatchGenericMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* succeeded);

    CARAPI GetDecorView(
        /* [out] */ IView** view);

    CARAPI PeekDecorView(
        /* [out] */ IView** view);

    CARAPI SaveHierarchyState(
        /* [out] */ IBundle** instanceState);

    CARAPI RestoreHierarchyState(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI SetChildDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable);

    CARAPI SetChildInt(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 value);

    CARAPI IsShortcutKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* isShortcutKey);

    virtual CARAPI SetVolumeControlStream(
        /* [in] */ Int32 streamType);

    virtual CARAPI GetVolumeControlStream(
        /* [out] */ Int32* streamType);

    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions);

    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions,
        /* [in] */ Int32 mask);

    CARAPI InvalidatePanelMenu(
        /* [in] */ Int32 featureId);

    CARAPI AlwaysReadCloseOnTouchAttr();

    CARAPI DoPendingInvalidatePanelMenu();

    // @Override
    CARAPI GetTransitionManager(
        /* [out] */ ITransitionManager** tm);

    // @Override
    CARAPI SetTransitionManager(
        /* [in] */ ITransitionManager* tm);

    // @Override
    CARAPI GetContentScene(
        /* [out] */ IScene** scene);

	// @Override
    CARAPI SetMediaController(
        /* [in] */ IMediaController* controller);

    // @Override
    CARAPI GetMediaController(
        /* [out] */ IMediaController** controller);

    // @Override
    CARAPI SetEnterTransition(
        /* [in] */ ITransition* enterTransition);

    // @Override
    CARAPI SetReturnTransition(
        /* [in] */ ITransition* transition);

    // @Override
    CARAPI SetExitTransition(
        /* [in] */ ITransition* exitTransition);

    // @Override
    CARAPI SetReenterTransition(
        /* [in] */ ITransition* transition);

    // @Override
    CARAPI SetSharedElementEnterTransition(
        /* [in] */ ITransition* sharedElementEnterTransition);

    // @Override
    CARAPI SetSharedElementReturnTransition(
        /* [in] */ ITransition* transition);

    // @Override
    CARAPI SetSharedElementExitTransition(
        /* [in] */ ITransition* sharedElementExitTransition);

    // @Override
    CARAPI SetSharedElementReenterTransition(
        /* [in] */ ITransition* transition);

    // @Override
    CARAPI GetEnterTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI GetReturnTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI GetExitTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI GetReenterTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI GetSharedElementEnterTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI GetSharedElementReturnTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI GetSharedElementExitTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI GetSharedElementReenterTransition(
        /* [out] */ ITransition** transition);

    // @Override
    CARAPI SetAllowEnterTransitionOverlap(
        /* [in] */ Boolean allow);

    // @Override
    CARAPI GetAllowEnterTransitionOverlap(
        /* [out] */ Boolean* allow);

    // @Override
    CARAPI SetAllowReturnTransitionOverlap(
        /* [in] */ Boolean allowExitTransitionOverlap);

    // @Override
    CARAPI GetAllowReturnTransitionOverlap(
        /* [out] */ Boolean* allow);

    // @Override
    CARAPI GetTransitionBackgroundFadeDuration(
        /* [out] */ Int64* duration);

    // @Override
    CARAPI SetTransitionBackgroundFadeDuration(
        /* [in] */ Int64 fadeDurationMillis);

    // @Override
    CARAPI SetSharedElementsUseOverlay(
        /* [in] */ Boolean sharedElementsUseOverlay);

    // @Override
    CARAPI GetSharedElementsUseOverlay(
        /* [out] */ Boolean* shared);

    // @Override
    CARAPI GetStatusBarColor(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetStatusBarColor(
        /* [in] */ Int32 color);

    // @Override
    CARAPI GetNavigationBarColor(
        /* [out] */ Int32* result);

    // @Override
    CARAPI SetNavigationBarColor(
        /* [in] */ Int32 color);

protected:
    /**
     * Initializes the menu associated with the given panel feature state. You
     * must at the very least set PanelFeatureState.menu to the Menu to be
     * associated with the given panel state. The default implementation creates
     * a new menu for the panel state.
     *
     * @param st The panel whose menu is being initialized.
     * @return Whether the initialization was successful.
     */
    CARAPI_(Boolean) InitializePanelMenu(
        /* [in] */ PanelFeatureState* st);

    /**
     * Perform initial setup of a panel. This should at the very least set the
     * style information in the PanelFeatureState and must set
     * PanelFeatureState.decor to the panel's window decor view.
     *
     * @param st The panel being initialized.
     */
    CARAPI_(Boolean) InitializePanelDecor(
        /* [in] */ PanelFeatureState* st);

    /**
     * Initializes the panel associated with the panel feature state. You must
     * at the very least set PanelFeatureState.panel to the View implementing
     * its contents. The default implementation gets the panel from the menu.
     *
     * @param st The panel state being initialized.
     * @return Whether the initialization was successful.
     */
    CARAPI_(Boolean) InitializePanelContent(
        /* [in] */ PanelFeatureState* st);

    CARAPI_(Boolean) OnKeyDown(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    CARAPI_(Boolean) OnKeyUp(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) OnActive();

    CARAPI_(void) CheckCloseActionMenu(
        /* [in] */ IMenu* menu);

    CARAPI_(void) DoInvalidatePanelMenu(
        /* [in] */ Int32 featureId);

    CARAPI_(void) OnOptionsPanelRotationChanged();

    CARAPI_(AutoPtr<IAudioManager>) GetAudioManager();

    /**
     * Update the state of a drawable feature. This should be called, for every
     * drawable feature supported, as part of onActive(), to make sure that the
     * contents of a containing window is properly updated.
     *
     * @see #onActive
     * @param featureId The desired drawable feature to change.
     * @param fromActive Always true when called from onActive().
     */
    CARAPI_(void) UpdateDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ Boolean fromActive);

    CARAPI_(void) UpdateDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ DrawableFeatureState* st,
        /* [in] */ Boolean fromResume);

    /**
     * Called when a Drawable feature changes, for the window to update its
     * graphics.
     *
     * @param featureId The feature being changed.
     * @param drawable The new Drawable to show, or null if none.
     * @param alpha The new alpha blending of the Drawable.
     */
    CARAPI_(void) OnDrawableChanged(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable,
        /* [in] */ Int32 alpha);

    /**
     * Called when an int feature changes, for the window to update its
     * graphics.
     *
     * @param featureId The feature being changed.
     * @param value The new integer value.
     */
    CARAPI_(void) OnIntChanged(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 value);

    CARAPI_(void) SendCloseSystemWindows();

    CARAPI_(void) SendCloseSystemWindows(
        /* [in] */ const String& reason);

    virtual CARAPI_(void) SetFeatureDefaultDrawable(
        /* [in] */ Int32 featureId,
        /* [in] */ IDrawable* drawable);

    virtual CARAPI_(void) SetFeatureFromAttrs(
        /* [in] */ Int32 featureId,
        /* [in] */ ITypedArray* attrs,
        /* [in] */ Int32 drawableAttr,
        /* [in] */ Int32 alphaAttr);

    // @Override
    CARAPI_(void) DispatchWindowAttributesChanged(
        /* [in] */ IWindowManagerLayoutParams* attrs);

private:
   CARAPI_(void) TransitionTo(
        /* [in] */ IScene* scene);

	CARAPI_(void) RegisterSwipeCallbacks();

	CARAPI_(AutoPtr<IViewRootImpl>) GetViewRootImpl();

	CARAPI_(Boolean) IsTranslucent();

	CARAPI_(AutoPtr<ITransition>) GetTransition(
        /* [in] */ ITransition* currentValue,
        /* [in] */ ITransition* defaultValue,
        /* [in] */ Int32 id);

    CARAPI_(void) OpenPanel(
        /* [in] */ PanelFeatureState* st,
        /* [in] */ IKeyEvent* event);

    /**
     * Closes the context menu. This notifies the menu logic of the close, along
     * with dismissing it from the UI.
     */
    /* synchronized */
    CARAPI_(void) CloseContextMenu();

    /**
     * Dismisses just the context menu UI. To close the context menu, use
     * {@link #closeContextMenu()}.
     */
    /* synchronized */
    CARAPI_(void) DismissContextMenu();

    CARAPI_(void) ReopenMenu(
        /* [in] */ Boolean toggleMenuMode);

    /**
     * Opens the panels that have had their state restored. This should be
     * called sometime after {@link #restorePanelState} when it is safe to add
     * to the window manager.
     */
    CARAPI_(void) OpenPanelsAfterRestore();

    CARAPI_(AutoPtr<DecorView>) GenerateDecor();

    CARAPI GenerateLayout(
        /* [in] */ DecorView* decor,
        /* [out] */ IViewGroup** viewGroup);

    CARAPI_(void) InstallDecor();

    CARAPI_(AutoPtr<IDrawable>) LoadImageURI(
        /* [in] */ IUri* uri);

    CARAPI GetDrawableState(
        /* [in] */ Int32 featureId,
        /* [in] */ Boolean required,
        /* [out] */ DrawableFeatureState** state);

    /**
     * Gets a panel's state based on its feature ID.
     *
     * @param featureId The feature ID of the panel.
     * @param required Whether the panel is required (if it is required and it
     *            isn't in our features, this throws an exception).
     * @return The panel state.
     */
    CARAPI GetPanelState(
        /* [in] */ Int32 featureId,
        /* [in] */ Boolean required,
        /* [out] */ PanelFeatureState** state);

    /**
     * Gets a panel's state based on its feature ID.
     *
     * @param featureId The feature ID of the panel.
     * @param required Whether the panel is required (if it is required and it
     *            isn't in our features, this throws an exception).
     * @param convertPanelState Optional: If the panel state does not exist, use
     *            this as the panel state.
     * @return The panel state.
     */
    CARAPI GetPanelState(
        /* [in] */ Int32 featureId,
        /* [in] */ Boolean required,
        /* [in] */ PanelFeatureState* convertPanelState,
        /* [out] */ PanelFeatureState** state);

    /**
     * Helper method for calling the {@link Callback#onPanelClosed(int, Menu)}
     * callback. This method will grab whatever extra state is needed for the
     * callback that isn't given in the parameters. If the panel is not open,
     * this will not perform the callback.
     *
     * @param featureId Feature ID of the panel that was closed. Must be given.
     * @param panel Panel that was closed. Optional but useful if there is no
     *            menu given.
     * @param menu The menu that was closed. Optional, but give if you have.
     */
    CARAPI_(void) CallOnPanelClosed(
        /* [in] */ Int32 featureId,
        /* [in] */ PanelFeatureState* panel,
        /* [in] */ IMenu* menu);

    /**
     * Helper method for adding launch-search to most applications. Opens the
     * search window using default settings.
     *
     * @return true if search window opened
     */
    CARAPI_(Boolean) LaunchDefaultSearch();

    /**
     * Determine the gravity value for the options panel. This can
     * differ in compact mode.
     *
     * @return gravity value to use for the panel window
     */
    CARAPI_(Int32) GetOptionsPanelGravity();

    CARAPI_(AutoPtr<IKeyguardManager>) GetKeyguardManager();

    CARAPI_(void) UpdateInt(
        /* [in] */ Int32 featureId,
        /* [in] */ Int32 value,
        /* [in] */ Boolean fromResume);

    CARAPI_(AutoPtr<IImageView>) GetLeftIconView();

    CARAPI_(AutoPtr<IProgressBar>) GetCircularProgressBar(
        /* [in] */ Boolean shouldInstallDecor);

    CARAPI_(AutoPtr<IProgressBar>) GetHorizontalProgressBar(
        /* [in] */ Boolean shouldInstallDecor);

    CARAPI_(AutoPtr<IImageView>) GetRightIconView();

    CARAPI_(Boolean) PerformPanelShortcut(
        /* [in] */ PanelFeatureState* st,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 flags);

    /**
     * Updates the progress bars that are shown in the title bar.
     *
     * @param value Can be one of {@link Window#PROGRESS_VISIBILITY_ON},
     *            {@link Window#PROGRESS_VISIBILITY_OFF},
     *            {@link Window#PROGRESS_INDETERMINATE_ON},
     *            {@link Window#PROGRESS_INDETERMINATE_OFF}, or a value
     *            starting at {@link Window#PROGRESS_START} through
     *            {@link Window#PROGRESS_END} for setting the default
     *            progress (if {@link Window#PROGRESS_END} is given,
     *            the progress bar widgets in the title will be hidden after an
     *            animation), a value between
     *            {@link Window#PROGRESS_SECONDARY_START} -
     *            {@link Window#PROGRESS_SECONDARY_END} for the
     *            secondary progress (if
     *            {@link Window#PROGRESS_SECONDARY_END} is given, the
     *            progress bar widgets will still be shown with the secondary
     *            progress bar will be completely filled in.)
     */
    CARAPI_(void) UpdateProgressBars(
        /* [in] */ Int32 value);

    CARAPI_(void) ShowProgressBars(
        /* [in] */ IProgressBar* horizontalProgressBar,
        /* [in] */ IProgressBar* spinnyProgressBar);

    CARAPI_(void) HideProgressBars(
        /* [in] */ IProgressBar*  horizontalProgressBar,
        /* [in] */ IProgressBar*  spinnyProgressBar);

    /**
     * Invoked when the panels should freeze their state.
     *
     * @param icicles Save state into this. This is usually indexed by the
     *            featureId. This will be given to {@link #restorePanelState} in the
     *            future.
     */
    CARAPI_(void) SavePanelState(
        /* [in] */ ISparseArray* icicles);//IParcelable

    /**
     * Invoked when the panels should thaw their state from a previously frozen state.
     *
     * @param icicles The state saved by {@link #savePanelState} that needs to be thawed.
     */
    CARAPI_(void) RestorePanelState(
        /* [in] */ ISparseArray* icicles);//IParcelable

private:
    static String TAG;
    static const Boolean SWEEP_OPEN_MENU;
    static const String FOCUSED_ID_TAG;
    static const String VIEWS_TAG;
    static const String PANELS_TAG;
    static const String ACTION_BAR_TAG;

    static const Int32 DEFAULT_BACKGROUND_FADE_DURATION_MS = 300;
    static const Int32 CUSTOM_TITLE_COMPATIBLE_FEATURES;
    static AutoPtr<ITransition> USE_DEFAULT_TRANSITION;

    /**
     * Simple callback used by the context menu and its submenus. The options
     * menu submenus do not use this (their behavior is more complex).
     */
    AutoPtr<DialogMenuCallback> mContextMenuCallback;

    AutoPtr<ITypedValue> mMinWidthMajor;
    AutoPtr<ITypedValue> mMinWidthMinor;
    AutoPtr<ITypedValue> mFixedWidthMajor;
    AutoPtr<ITypedValue> mFixedWidthMinor;
    AutoPtr<ITypedValue> mFixedHeightMajor;
    AutoPtr<ITypedValue> mFixedHeightMinor;
    AutoPtr<ITypedValue> mOutsetBottom;

    // The icon resource has been explicitly set elsewhere
    // and should not be overwritten with a default.
    static const Int32 FLAG_RESOURCE_SET_ICON = 1 << 0;
    // The logo resource has been explicitly set elsewhere
    // and should not be overwritten with a default.
    static const Int32 FLAG_RESOURCE_SET_LOGO = 1 << 1;
    // The icon resource is currently configured to use the system fallback
    // as no default was previously specified. Anything can override this.
    static const Int32 FLAG_RESOURCE_SET_ICON_FALLBACK = 1 << 2;

    Int32 mResourcesSetFlags;
    Int32 mIconRes;
    Int32 mLogoRes;

    // This is the top-level view of the window, containing the window decor.
    DecorView* mDecor;

    // This is the view in which the window contents are placed. It is either
    // mDecor itself, or a child of mDecor where the contents go.
    AutoPtr<IViewGroup> mContentParent;
    AutoPtr<IViewGroup> mContentRoot;
    AutoPtr<ISurfaceHolderCallback2> mTakeSurfaceCallback;
    //AutoPtr<BaseSurfaceHolder> mSurfaceHolder;
    AutoPtr<IInputQueueCallback> mTakeInputQueueCallback;
    Boolean mIsFloating;
    AutoPtr<ILayoutInflater> mLayoutInflater;
    AutoPtr<ITextView> mTitleView;
    AutoPtr<IDecorContentParent> mDecorContentParent;
    //AutoPtr<IActionBarView> mActionBar;
    AutoPtr<ActionMenuPresenterCallback> mActionMenuPresenterCallback;
    AutoPtr<PanelMenuPresenterCallback> mPanelMenuPresenterCallback;

    AutoPtr<ITransitionManager> mTransitionManager;
    AutoPtr<IScene> mContentScene;

    AutoPtr< ArrayOf<DrawableFeatureState*> > mDrawables;
    AutoPtr< ArrayOf<PanelFeatureState*> > mPanels;

    /**
     * The panel that is prepared or opened (the most recent one if there are
     * multiple panels). Shortcuts will go to this panel. It gets set in
     * {@link #preparePanel} and cleared in {@link #closePanel}.
     */
    AutoPtr<PanelFeatureState> mPreparedPanel;

    /**
     * The keycode that is currently held down (as a modifier) for chording. If
     * this is 0, there is no key held down.
     */
    Int32 mPanelChordingKey;
    AutoPtr<IImageView> mLeftIconView;
    AutoPtr<IImageView> mRightIconView;
    AutoPtr<IProgressBar> mCircularProgressBar;
    AutoPtr<IProgressBar> mHorizontalProgressBar;
    Int32 mBackgroundResource;
    Int32 mBackgroundFallbackResource;
    AutoPtr<IDrawable> mBackgroundDrawable;
    Float mElevation;
    /** Whether window content should be clipped to the background outline. */
    Boolean mClipToOutline;
    Int32 mFrameResource;

    Int32 mTextColor;
    Int32 mStatusBarColor;
    Int32 mNavigationBarColor;
    Boolean mForcedStatusBarColor;
    Boolean mForcedNavigationBarColor;
    AutoPtr<ICharSequence> mTitle;

    Int32 mTitleColor;

    Boolean mAlwaysReadCloseOnTouchAttr;
    Boolean mEnableGestures;

    AutoPtr<IContextMenuBuilder> mContextMenu;
    AutoPtr<IMenuDialogHelper> mContextMenuHelper;
    Boolean mClosingActionMenu;

    Int32 mVolumeControlStreamType;
    AutoPtr<IMediaController> mMediaController;

    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IKeyguardManager> mKeyguardManager;
    Int32 mUiOptions;
    Boolean mInvalidatePanelMenuPosted;
    Int32 mInvalidatePanelMenuFeatures;
    AutoPtr<InvalidatePanelMenuRunnable> mInvalidatePanelMenuRunnable;
    static AutoPtr<IPhoneWindowRotationWatcher> sRotationWatcher;

    AutoPtr<ITransition> mEnterTransition;
    AutoPtr<ITransition> mReturnTransition;
    AutoPtr<ITransition> mExitTransition;
    AutoPtr<ITransition> mReenterTransition;
    AutoPtr<ITransition> mSharedElementEnterTransition;
    AutoPtr<ITransition> mSharedElementReturnTransition;
    AutoPtr<ITransition> mSharedElementExitTransition;
    AutoPtr<ITransition> mSharedElementReenterTransition;

    Boolean mAllowReturnTransitionOverlap;
    Boolean mAllowEnterTransitionOverlap;
    Int64 mBackgroundFadeDurationMillis;
    Boolean mSharedElementsUseOverlay;
    AutoPtr<IRect> mTempRect;

    friend class _DecorView;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Internal::Policy::Impl::PhoneWindow::DrawableFeatureState, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Internal::Policy::Impl::PhoneWindow::PanelFeatureState, IInterface)
#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_PHONEWINDOW_H__
