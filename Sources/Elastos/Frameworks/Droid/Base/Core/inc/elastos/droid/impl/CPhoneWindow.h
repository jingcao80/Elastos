
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOW_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOW_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneWindow.h"
#include "elastos/droid/view/Window.h"
#include "elastos/droid/view/BaseSurfaceHolder.h"

#include "elastos/droid/view/menu/CIconMenuPresenter.h"
#include "elastos/droid/view/menu/CListMenuPresenter.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/view/menu/CMenuDialogHelper.h"

class ViewRootImpl;
class CPhoneWindowRotationWatcher;

using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::Menu::IMenuBuilder;
using Elastos::Droid::View::Menu::IMenuView;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IWindowCallback;
using Elastos::Droid::View::ISurfaceHolderCallback2;
using Elastos::Droid::View::Window;
using Elastos::Droid::View::IInputQueueCallback;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IPhoneWindowRotationWatcher;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::BaseSurfaceHolder;
using Elastos::Droid::View::IRootViewSurfaceTaker;
using Elastos::Droid::View::Accessibility::IAccessibilityEventSource;
using Elastos::Droid::View::Menu::IMenuPresenterCallback;
using Elastos::Droid::View::Menu::ISubMenuBuilder;
using Elastos::Droid::View::Menu::IMenuDialogHelper;
using Elastos::Droid::View::Menu::CMenuDialogHelper;
using Elastos::Droid::View::Menu::IMenuBuilderCallback;
using Elastos::Droid::View::Menu::CIconMenuPresenter;
using Elastos::Droid::View::Menu::IIconMenuPresenter;
using Elastos::Droid::View::Menu::CListMenuPresenter;
using Elastos::Droid::View::Menu::IListMenuPresenter;
using Elastos::Droid::View::Menu::IContextMenuBuilder;
using Elastos::Droid::View::Menu::IMenuDialogHelper;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::Internal::IActionBarView;
using Elastos::Droid::Widget::Internal::IActionBarContextView;
using Elastos::Droid::Media::IAudioManager;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

extern "C" const InterfaceID EIID_CPhoneWindow;

CarClass(CPhoneWindow), public Window
{
    friend class ViewRootImpl;
    friend class CPhoneWindowRotationWatcher;

private:
    class InvalidatePanelMenuRunnable
        : public Runnable
    {
    public:
        InvalidatePanelMenuRunnable(
            /* [in] */ CPhoneWindow* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class WindowManagerHolder
    {
    public:
        static CARAPI_(AutoPtr<IIWindowManager>) GetWindowManager();

    public:
        static AutoPtr<IIWindowManager> sWindowManager;
    };

    class PanelMenuPresenterCallback
        : public ElRefBase
        , public IMenuPresenterCallback
    {
    public:
        CAR_INTERFACE_DECL()

        PanelMenuPresenterCallback(
            /* [in] */ CPhoneWindow* host);

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
        : public ElRefBase
        , public IMenuPresenterCallback
    {
    public:
        ActionMenuPresenterCallback(
            /* [in] */ CPhoneWindow* host);

        CAR_INTERFACE_DECL()

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
            : public ElRefBase
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

        class DecorViewWeakReferenceImpl
            : public ElLightRefBase
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
        _DecorView(
            /* [in] */ CPhoneWindow* host,
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

    public:
        CARAPI_(void) StartChanging();

        CARAPI_(void) FinishChanging();

        CARAPI_(void) SetWindowBackground(
            /* [in] */ IDrawable* drawable);

        CARAPI SetBackgroundDrawable(
            /* [in] */ IDrawable* d);

        CARAPI_(Boolean) DispatchKeyEvent(
            /* [in] */ IKeyEvent* event);

        CARAPI_(Boolean) DispatchKeyShortcutEvent(
            /* [in] */ IKeyEvent* ev);

        CARAPI_(Boolean) DispatchTouchEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(Boolean) DispatchTrackballEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(Boolean) DispatchGenericMotionEvent(
            /* [in] */ IMotionEvent* ev);

        CARAPI_(Boolean) SuperDispatchKeyEvent(
            /* [in] */ IKeyEvent* event);

        CARAPI_(Boolean) SuperDispatchTouchEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(Boolean) SuperDispatchTrackballEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(Boolean) SuperDispatchGenericMotionEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(Boolean) SuperDispatchKeyShortcutEvent(
            /* [in] */ IKeyEvent* event);

        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* res);

        CARAPI_(Boolean) OnInterceptTouchEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(void) SetWindowFrame(
            /* [in] */ IDrawable* drawable);

        CARAPI OnWindowFocusChanged(
            /* [in] */ Boolean hasWindowFocus);

        CARAPI OnCloseSystemDialogs(
            /* [in] */ const String& reason);

        CARAPI_(Boolean) ShowContextMenuForChild(
            /* [in] */ IView* originalView);

        CARAPI SendAccessibilityEvent(
            /* [in] */ Int32 eventType);

        CARAPI_(Boolean) DispatchPopulateAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event);

        CARAPI_(AutoPtr<IActionMode>) StartActionModeForChild(
            /* [in] */ IView* originalView,
            /* [in] */ IActionModeCallback* callback);

        CARAPI_(AutoPtr<IActionMode>) StartActionMode(
            /* [in] */ IActionModeCallback* callback);

    protected:
        CARAPI_(Boolean) SetFrame(
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 r,
            /* [in] */ Int32 b);

        CARAPI_(void) OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        CARAPI_(void) UpdateWindowResizeState();

        virtual CARAPI OnDetachedFromWindow();

    private:
        CARAPI_(void) DrawableChanged();

        virtual CARAPI OnAttachedToWindow();

        CARAPI_(Boolean) IsOutOfBounds(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        CARAPI_(AutoPtr<IWindowCallback>) GetCallback();

        CARAPI_(Boolean) IsDestroyed();

    protected:
        AutoPtr<CPhoneWindow> mHost;

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

        friend class ActionModeCallbackWrapper;
        friend class CPhoneWindow;

    public:
        Int32 mDefaultOpacity;
    };

    class DecorView
        : public ElRefBase
        , public _DecorView
        , public IFrameLayout
        , public IViewParent
        , public IViewManager
        , public IDrawableCallback
        , public IKeyEventCallback
        , public IAccessibilityEventSource
        , public IWeakReferenceSource
    {
    public:
        DecorView(
            /* [in] */ CPhoneWindow* host,
            /* [in] */ IContext* context,
            /* [in] */ Int32 featureId,
            /* [in] */ Boolean useSelfRef = FALSE);

        ~DecorView();

        CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI_(UInt32) _AddRef();

        CARAPI_(UInt32) _Release();

        // =========================================================
        // IView
        // =========================================================
        IVIEW_METHODS_DECL()

        // =========================================================
        // IViewGroup
        // =========================================================
        IVIEWGROUP_METHODS_DECL()

        // =========================================================
        // IViewParent
        // =========================================================
        IVIEWPARENT_METHODS_DECL()

        // =========================================================
        // IViewManager
        // =========================================================
        IVIEWMANAGER_METHODS_DECL()

        IDRAWABLECALLBACK_METHODS_DECL()

        IKEYEVENTCALLBACK_METHODS_DECL()

        IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

        CARAPI GetForegroundGravity(
            /* [out] */ Int32* foregroundGravity);

        CARAPI SetForegroundGravity(
            /* [in] */ Int32 foregroundGravity);

        CARAPI SetForeground(
            /* [in] */ IDrawable* drawable);

        CARAPI GetForeground(
            /* [out] */ IDrawable** foreground);

        CARAPI SetMeasureAllChildren(
            /* [in] */ Boolean measureAll);

        CARAPI GetMeasureAllChildren(
            /* [out] */ Boolean* measureAll);

        CARAPI GetConsiderGoneChildrenWhenMeasuring(
            /* [out] */ Boolean* measureAll);

        CARAPI GetWeakReference(
            /* [out] */ IWeakReference** weakReference);

    private:
        Boolean mUseSelfRef;
    };

    class PanelFeatureState : public ElRefBase
    {
    public:
        PanelFeatureState(
            /* [in] */ Int32 featureId);

        CARAPI_(Boolean) IsInListMode();

        CARAPI_(Boolean) HasPanelItems();

        /**
         * Unregister and free attached MenuPresenters. They will be recreated as needed.
         */
        CARAPI_(void) ClearMenuPresenters();

        CARAPI_(void) SetStyle(
            /* [in] */ IContext* context);

        CARAPI_(void) SetMenu(
            /* [in] */ IMenuBuilder* menu);

        CARAPI_(AutoPtr<IMenuView>) GetListMenuView(
            /* [in] */ IContext* context,
            /* [in] */ IMenuPresenterCallback* cb);

        CARAPI_(AutoPtr<IMenuView>) GetIconMenuView(
            /* [in] */ IContext* context,
            /* [in] */ IMenuPresenterCallback* cb);

        CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

        CARAPI_(void) OnRestoreInstanceState(
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
        AutoPtr<DecorView> mDecorView;

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

        friend class CPhoneWindow;
    };

    /**
     * Simple implementation of MenuBuilder.Callback that:
     * <li> Opens a submenu when selected.
     * <li> Calls back to the callback's onMenuItemSelected when an item is
     * selected.
     */
    class DialogMenuCallback
        : public ElRefBase
        , public IMenuBuilderCallback
        , public IMenuPresenterCallback
    {
    public:
        DialogMenuCallback(
            /* [in] */ Int32 featureId,
            /* [in] */ CPhoneWindow* host);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface* iinterface,
            /* [in] */ InterfaceID* id);

        CARAPI OnCloseMenu(
            /* [in] */ IMenuBuilder* menu,
            /* [in] */ Boolean allMenusAreClosing);

        CARAPI OnCloseSubMenu(
            /* [in] */ ISubMenuBuilder* menu);

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

    class DecorRunnable
        : public Runnable
    {
    public:
        DecorRunnable(
            /* [in] */ CPhoneWindow* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

    class DrawableFeatureState : public ElRefBase
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

    class MyMenuBuilderCallback
        : public ElRefBase
        , public IMenuBuilderCallback
    {
    public:
        MyMenuBuilderCallback(
            /* [in] */ CPhoneWindow* host);

        CAR_INTERFACE_DECL();

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
    CPhoneWindow();

    ~CPhoneWindow();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    CARAPI GetWindowStyle(
        /* [out] */ ITypedArray** attrs);

    CARAPI SetContainer(
        /* [in] */ IWindow* container);

    CARAPI GetContainer(
        /* [out] */ IWindow** container);

    CARAPI IsDestroyed(
        /* [out] */ Boolean* destroyed);

    CARAPI HasChildren(
        /* [out] */  Boolean* hasChildren);

    CARAPI SetWindowManager(
        /* [in] */ IWindowManager* wm,
        /* [in] */ IBinder* appToken,
        /* [in] */ const String& appName);

    CARAPI GetWindowManager(
        /* [out] */ IWindowManager** wm);

    CARAPI SetCallback(
        /* [in] */ IWindowCallback* cb);

    CARAPI GetCallback(
        /* [out] */ IWindowCallback** cb);

    CARAPI TakeSurface(
        /* [in] */ ISurfaceHolderCallback2* cb);

    CARAPI TakeInputQueue(
        /* [in] */ IInputQueueCallback* callback);

    CARAPI IsFloating(
        /* [out] */ Boolean* isFloating);

    CARAPI SetLayout(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    CARAPI SetType(
        /* [in] */ Int32 type);

    CARAPI SetFormat(
        /* [in] */ Int32 format);

    CARAPI SetWindowAnimations(
        /* [in] */ Int32 resId);

    CARAPI SetSoftInputMode(
        /* [in] */ Int32 mode);

    CARAPI AddFlags(
        /* [in] */ Int32 flags);

    CARAPI ClearFlags(
        /* [in] */ Int32 flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags,
        /* [in] */ Int32 mask);

    CARAPI SetAttributes(
        /* [in] */ IWindowManagerLayoutParams* a);

    CARAPI GetAttributes(
        /* [out] */ IWindowManagerLayoutParams** params);

    CARAPI RequestFeature(
        /* [in] */ Int32 featureId,
        /* [out] */ Boolean* result);

    CARAPI MakeActive();

    CARAPI IsActive(
        /* [out] */ Boolean* isActive);

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

    static void ClearMenuViews(
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
     * @param featureId The feature ID of the relevant panel (defaults to FEATURE_OPTIONS_PANEL}.
     * @param event The key event.
     * @return Whether the key was handled.
     */
    CARAPI OnKeyDownPanel(
        /* [in] */ Int32 featureId,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* handled);

    /**
     * Called when the panel key is released.
     * @param featureId The feature ID of the relevant panel (defaults to FEATURE_OPTIONS_PANEL}.
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

    CARAPI OnCloseMenu(
        /* [in] */ IMenuBuilder* menu,
        /* [in] */ Boolean allMenusAreClosing);

    CARAPI OnCloseSubMenu(
        /* [in] */ ISubMenuBuilder* menu);

    CARAPI OnMenuModeChange(
        /* [in] */ IMenuBuilder* menu);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI SetBackgroundDrawableResource(
        /* [in] */ Int32 resid);

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

    CARAPI HasFeature(
        /* [in] */ Int32 feature,
        /* [out] */ Boolean* hasFeature);

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

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions);

    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions,
        /* [in] */ Int32 mask);

    CARAPI InvalidatePanelMenu(
        /* [in] */ Int32 featureId);

    CARAPI SetCloseOnTouchOutsideIfNotSet(
        /* [in] */ Boolean close);

    CARAPI AlwaysReadCloseOnTouchAttr();

    CARAPI Destroy();

    CARAPI SetWindowManager(
        /* [in] */ IWindowManager* wm,
        /* [in] */ IBinder* appToken,
        /* [in] */ const String& appName,
        /* [in] */ Boolean hardwareAccelerated);

    CARAPI AdjustLayoutParamsForSubWindow(
        /* [in] */ IWindowManagerLayoutParams* wp);

    CARAPI SetDimAmount(
        /* [in] */ Float amount);

    CARAPI SetCloseOnTouchOutside(
        /* [in] */ Boolean close);

    CARAPI ShouldCloseOnTouch(
        /* [in] */ IContext* context,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    CARAPI SetUiOptions(
        /* [in] */ Int32 uiOptions,
        /* [in] */ Int32 mask);

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

private:
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
        /* [in] */ IObjectInt32Map* icicles);

    /**
     * Invoked when the panels should thaw their state from a previously frozen state.
     *
     * @param icicles The state saved by {@link #savePanelState} that needs to be thawed.
     */
    CARAPI_(void) RestorePanelState(
        /* [in] */ IObjectInt32Map* icicles);

private:
    static String TAG;

    static const Boolean SWEEP_OPEN_MENU;

    static const String FOCUSED_ID_TAG;
    static const String VIEWS_TAG;
    static const String PANELS_TAG;
    static const String ACTION_BAR_TAG;

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

    // This is the top-level view of the window, containing the window decor.
    DecorView* mDecor;

    // This is the view in which the window contents are placed. It is either
    // mDecor itself, or a child of mDecor where the contents go.
    AutoPtr<IViewGroup> mContentParent;

    AutoPtr<ISurfaceHolderCallback2> mTakeSurfaceCallback;

    AutoPtr<BaseSurfaceHolder> mSurfaceHolder;

    AutoPtr<IInputQueueCallback> mTakeInputQueueCallback;

    Boolean mIsFloating;

    AutoPtr<ILayoutInflater> mLayoutInflater;

    AutoPtr<ITextView> mTitleView;

    AutoPtr<IActionBarView> mActionBar;
    AutoPtr<ActionMenuPresenterCallback> mActionMenuPresenterCallback;
    AutoPtr<PanelMenuPresenterCallback> mPanelMenuPresenterCallback;

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

    AutoPtr<IDrawable> mBackgroundDrawable;

    Int32 mFrameResource;

    Int32 mTextColor;

    AutoPtr<ICharSequence> mTitle;

    Int32 mTitleColor;
    Boolean mAlwaysReadCloseOnTouchAttr;

    AutoPtr<IContextMenuBuilder> mContextMenu;
    AutoPtr<IMenuDialogHelper> mContextMenuHelper;

    Boolean mClosingActionMenu;
    Int32 mVolumeControlStreamType;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IKeyguardManager> mKeyguardManager;

    Int32 mUiOptions;

    Boolean mInvalidatePanelMenuPosted;
    Int32 mInvalidatePanelMenuFeatures;
    AutoPtr<InvalidatePanelMenuRunnable> mInvalidatePanelMenuRunnable;
    static AutoPtr<IPhoneWindowRotationWatcher> sRotationWatcher;

    friend class _DecorView;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOW_H__
