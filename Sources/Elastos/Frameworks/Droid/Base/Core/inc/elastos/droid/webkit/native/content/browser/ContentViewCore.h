
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIEWCORE_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIEWCORE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/SystemClock.h"
//#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/webkit/native/base/ObserverList.h"
#include "elastos/droid/webkit/native/content/browser/ContentSettings.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewClient.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewDownloadDelegate.h"
#include "elastos/droid/webkit/native/content/browser/InterstitialPageDelegateElastos.h"
#include "elastos/droid/webkit/native/content/browser/LoadUrlParams.h"
#include "elastos/droid/webkit/native/content/browser/NavigationClient.h"
#include "elastos/droid/webkit/native/content/browser/PopupZoomer.h"
#include "elastos/droid/webkit/native/content/browser/PositionObserver.h"
#include "elastos/droid/webkit/native/content/browser/RenderCoordinates.h"
#include "elastos/droid/webkit/native/content/browser/SelectActionModeCallback.h"
#include "elastos/droid/webkit/native/content/browser/ScreenOrientationListener.h"
#include "elastos/droid/webkit/native/content/browser/TouchEventSynthesizer.h"
#include "elastos/droid/webkit/native/content/browser/WebContentsObserverElastos.h"
#include "elastos/droid/webkit/native/content/browser/accessibility/AccessibilityInjector.h"
#include "elastos/droid/webkit/native/content/browser/accessibility/BrowserAccessibilityManager.h"
#include "elastos/droid/webkit/native/content/browser/input/AdapterInputConnection.h"
#include "elastos/droid/webkit/native/content/browser/input/ImeAdapter.h"
#include "elastos/droid/webkit/native/content/browser/input/InsertionHandleController.h"
#include "elastos/droid/webkit/native/content/browser/input/InputMethodManagerWrapper.h"
#include "elastos/droid/webkit/native/content/browser/input/SelectPopup.h"
#include "elastos/droid/webkit/native/content/browser/input/SelectionHandleController.h"
#include "elastos/droid/webkit/native/content_public/browser/GestureStateListener.h"
#include "elastos/droid/webkit/native/ui/base/ViewElastosDelegate.h"
#include "elastos/droid/webkit/native/ui/base/ViewElastos.h"
#include "elastos/droid/webkit/native/ui/base/WindowElastos.h"

using Elastos::Utility::IHashSet;
using Elastos::Utility::IMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::ISet;

// import android.annotation.SuppressLint;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::ISearchManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IFeatureInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IResultReceiver;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::IBrowser;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::IAnnotation;
//using Elastos::Droid::Text::TextUtils;
// import android.util.Log;
// import android.util.Pair;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerAccessibilityStateChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::IFrameLayout;

using Elastos::Droid::Webkit::Base::ObserverList;
using Elastos::Droid::Webkit::Ui::Base::ViewElastosDelegate;
using Elastos::Droid::Webkit::Ui::Base::ViewElastos;
using Elastos::Droid::Webkit::Ui::Base::WindowElastos;
using Elastos::Droid::Webkit::Content::Browser::Accessibility::AccessibilityInjector;
using Elastos::Droid::Webkit::Content::Browser::Accessibility::BrowserAccessibilityManager;
using Elastos::Droid::Webkit::Content::Browser::Input::AdapterInputConnection;
using Elastos::Droid::Webkit::Content::Browser::Input::ImeAdapter;
using Elastos::Droid::Webkit::Content::Browser::Input::InputMethodManagerWrapper;
using Elastos::Droid::Webkit::Content::Browser::Input::InsertionHandleController;
using Elastos::Droid::Webkit::Content::Browser::Input::SelectPopup;
using Elastos::Droid::Webkit::Content::Browser::Input::SelectionHandleController;
using Elastos::Droid::Webkit::Content_Public::Browser::GestureStateListener;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.ApiCompatibilityUtils;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.CommandLine;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ObserverList;
// import org.chromium.base.ObserverList.RewindableIterator;
// import org.chromium.base.TraceEvent;
// import org.chromium.content.R;
// import org.chromium.content.browser.ScreenOrientationListener.ScreenOrientationObserver;
// import org.chromium.content.browser.accessibility.AccessibilityInjector;
// import org.chromium.content.browser.accessibility.BrowserAccessibilityManager;
// import org.chromium.content.browser.input.AdapterInputConnection;
// import org.chromium.content.browser.input.GamepadList;
// import org.chromium.content.browser.input.HandleView;
// import org.chromium.content.browser.input.ImeAdapter;
// import org.chromium.content.browser.input.ImeAdapter.AdapterInputConnectionFactory;
// import org.chromium.content.browser.input.InputMethodManagerWrapper;
// import org.chromium.content.browser.input.InsertionHandleController;
// import org.chromium.content.browser.input.SelectPopup;
// import org.chromium.content.browser.input.SelectPopupDialog;
// import org.chromium.content.browser.input.SelectPopupDropdown;
// import org.chromium.content.browser.input.SelectPopupItem;
// import org.chromium.content.browser.input.SelectionHandleController;
// import org.chromium.content.common.ContentSwitches;
// import org.chromium.content_public.browser.GestureStateListener;
// import org.chromium.content_public.browser.WebContents;
// import org.chromium.ui.base.ViewAndroid;
// import org.chromium.ui.base.ViewAndroidDelegate;
// import org.chromium.ui.base.WindowAndroid;
// import org.chromium.ui.gfx.DeviceDisplayInfo;

// import java.lang.annotation.Annotation;
// import java.lang.reflect.Field;
// import java.util.ArrayList;
// import java.util.HashMap;
// import java.util.HashSet;
// import java.util.List;
// import java.util.Map;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

/**
 * Provides a Java-side 'wrapper' around a WebContent (native) instance.
 * Contains all the major functionality necessary to manage the lifecycle of a ContentView without
 * being tied to the view system.
 */
//@JNINamespace("content")
class ContentViewCore
    : public NavigationClient
    , public IAccessibilityManagerAccessibilityStateChangeListener
    , public ScreenOrientationListener::ScreenOrientationObserver
{
public:
    /**
     * Interface that consumers of {@link ContentViewCore} must implement to allow the proper
     * dispatching of view methods through the containing view.
     *
     * <p>
     * All methods with the "super_" prefix should be routed to the parent of the
     * implementing container view.
     */
    //@SuppressWarnings("javadoc")
    class InternalAccessDelegate
        : public Object
    {
    public:
        /**
         * @see View#drawChild(Canvas, View, long)
         */
        virtual CARAPI_(Boolean) DrawChild(
            /* [in] */ ICanvas* canvas,
            /* [in] */ IView* child,
            /* [in] */ Int64 drawingTime) = 0;

        /**
         * @see View#onKeyUp(keyCode, KeyEvent)
         */
        virtual CARAPI_(Boolean) Super_onKeyUp(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event) = 0;

        /**
         * @see View#dispatchKeyEventPreIme(KeyEvent)
         */
        virtual CARAPI_(Boolean) Super_dispatchKeyEventPreIme(
            /* [in] */ IKeyEvent* event) = 0;

        /**
         * @see View#dispatchKeyEvent(KeyEvent)
         */
        virtual CARAPI_(Boolean) Super_dispatchKeyEvent(
            /* [in] */ IKeyEvent* event) = 0;

        /**
         * @see View#onGenericMotionEvent(MotionEvent)
         */
        virtual CARAPI_(Boolean) Super_onGenericMotionEvent(
            /* [in] */ IMotionEvent* event) = 0;

        /**
         * @see View#onConfigurationChanged(Configuration)
         */
        virtual CARAPI_(void) Super_onConfigurationChanged(
            /* [in] */ IConfiguration* newConfig) = 0;

        /**
         * @see View#onScrollChanged(int, int, int, int)
         */
        virtual CARAPI_(void) OnScrollChanged(
            /* [in] */ Int32 lPix,
            /* [in] */ Int32 tPix,
            /* [in] */ Int32 oldlPix,
            /* [in] */ Int32 oldtPix) = 0;

        /**
         * @see View#awakenScrollBars()
         */
        virtual CARAPI_(Boolean) AwakenScrollBars() = 0;

        /**
         * @see View#awakenScrollBars(int, boolean)
         */
        virtual CARAPI_(Boolean) Super_awakenScrollBars(
            /* [in] */ Int32 startDelay,
            /* [in] */ Boolean invalidate) = 0;
    };

    /**
     * An interface for controlling visibility and state of embedder-provided zoom controls.
     */
    class ZoomControlsDelegate
        : public Object
    {
    public:
        /**
         * Called when it's reasonable to show zoom controls.
         */
        virtual CARAPI_(void) InvokeZoomPicker() = 0;

        /**
         * Called when zoom controls need to be hidden (e.g. when the view hides).
         */
        virtual CARAPI_(void) DismissZoomPicker() = 0;

        /**
         * Called when page scale has been changed, so the controls can update their state.
         */
        virtual CARAPI_(void) UpdateZoomControls() = 0;
    };

    /**
     * An interface that allows the embedder to be notified when the results of
     * extractSmartClipData are available.
     */
    class SmartClipDataListener
        : public Object
    {
    public:
        virtual CARAPI_(void) OnSmartClipDataExtracted(
            /* [in] */ const String& text,
            /* [in] */ const String& html,
            /* [in] */ IRect* clipRect) = 0;
    };

    /** Callback interface for evaluateJavaScript(). */
    class JavaScriptCallback
        : public Object
    {
    public:
        virtual CARAPI_(void) HandleJavaScriptResult(
            /* [in] */ const String& jsonResult) = 0;
    };

private:
    class InnerViewAndroidDelegate
        : public ViewElastosDelegate
    {
    public:
        InnerViewAndroidDelegate(
            /* [in] */ ContentViewCore* owner);

        //@Override
        CARAPI_(AutoPtr<IView>) AcquireAnchorView();

        //@Override
        //@SuppressWarnings("deprecation")  // AbsoluteLayout
        CARAPI SetAnchorViewPosition(
            /* [in] */ IView* view,
            /* [in] */ Float x,
            /* [in] */ Float y,
            /* [in] */ Float width,
            /* [in] */ Float height);

        //@Override
        CARAPI ReleaseAnchorView(
            /* [in] */ IView* anchorView);

    private:
        ContentViewCore* mOwner;

        // mContainerView can change, but this ViewAndroidDelegate can only be used to
        // add and remove views from the mContainerViewAtCreation.
        const IViewGroup* mContainerViewAtCreation;
    };

    class InnerImeAdapterDelegate
        : public ImeAdapter::ImeAdapterDelegate
    {
    private:
        class InnerResultReceiver
            : public Object
            , public IResultReceiver
        {
        public:
            InnerResultReceiver(
                /* [in] */ InnerImeAdapterDelegate* owner);

            CARAPI OnReceiveResult(
                /* [in] */ Int32 resultCode,
                /* [in] */ IBundle* resultData);

        private:
            InnerImeAdapterDelegate* mOwner;
        };

    public:
        InnerImeAdapterDelegate(
            /* [in] */ ContentViewCore* owner);

        //@Override
        CARAPI_(void) OnImeEvent(
            /* [in] */ Boolean isFinish);

        //@Override
        CARAPI_(void) OnDismissInput();

        //@Override
        CARAPI_(AutoPtr<IView>) GetAttachedView();

        //@Override
        CARAPI_(AutoPtr<IResultReceiver>) GetNewShowKeyboardReceiver();

    private:
        ContentViewCore* mOwner;
    };

    class InnerListener
        : public PositionObserver::Listener
    {
    public:
        InnerListener(
            /* [in] */ ContentViewCore* owner);

        CARAPI OnPositionChanged(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

    private:
        ContentViewCore* mOwner;
    };

    class InnerZoomControlsDelegate
        : public ZoomControlsDelegate
    {
    public:
        InnerZoomControlsDelegate(
            /* [in] */ ContentViewCore* owner);

        //@Override
        CARAPI_(void) InvokeZoomPicker();

        //@Override
        CARAPI_(void) DismissZoomPicker();

        //@Override
        CARAPI_(void) UpdateZoomControls();

    private:
        ContentViewCore* mOwner;
    };

    class InnerWebContentsObserverAndroid
        : public WebContentsObserverElastos
    {
    public:
        InnerWebContentsObserverAndroid(
            /* [in] */ ContentViewCore* owner);

        //@Override
        CARAPI DidNavigateMainFrame(
            /* [in] */ const String& url,
            /* [in] */ const String& baseUrl,
            /* [in] */ Boolean isNavigationToDifferentPage,
            /* [in] */ Boolean isFragmentNavigation);

        //@Override
        CARAPI RenderProcessGone(
            /* [in] */ Boolean wasOomProtected);

    private:
        ContentViewCore* mOwner;
    };

    class InnerOnVisibilityChangedListener
        : public PopupZoomer::OnVisibilityChangedListener
    {
    private:
        class OnPopupZoomerShownRunnable
            : public Runnable
        {
        public:
            OnPopupZoomerShownRunnable(
                /* [in] */ InnerOnVisibilityChangedListener* owner,
                /* [in] */ const PopupZoomer* zoomer);

            CARAPI Run();

        private:
            InnerOnVisibilityChangedListener* mOwner;
            const PopupZoomer* mZoomer;
        };

        class OnPopupZoomerHiddenRunnable
            : public Runnable
        {
        public:
            OnPopupZoomerHiddenRunnable(
                /* [in] */ InnerOnVisibilityChangedListener* owner,
                /* [in] */ const PopupZoomer* zoomer);

            CARAPI Run();

        private:
            InnerOnVisibilityChangedListener* mOwner;
            const PopupZoomer* mZoomer;
        };

    public:
        InnerOnVisibilityChangedListener(
            /* [in] */ ContentViewCore* owner);

        CAR_INTERFACE_DECL();

        //@Override
        CARAPI OnPopupZoomerShown(
            /* [in] */ PopupZoomer* zoomer);

        //@Override
        CARAPI OnPopupZoomerHidden(
            /* [in] */ PopupZoomer* zoomer);

    private:
        ContentViewCore* mOwner;
    };

    class InnerOnTapListener
        : public PopupZoomer::OnTapListener
    {
    public:
        InnerOnTapListener(
            /* [in] */ ContentViewCore* owner,
            /* [in] */ IViewGroup* vg);

        //@Override
        CARAPI_(Boolean) OnSingleTap(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* e);

        //@Override
        CARAPI_(Boolean) OnLongPress(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* e);

    private:
        ContentViewCore* mOwner;

        // mContainerView can change, but this OnTapListener can only be used
        // with the mContainerViewAtCreation.
        const IViewGroup* mContainerViewAtCreation;
    };

    class FakeMouseMoveRunnable
        : public Runnable
    {
    public:
        FakeMouseMoveRunnable(
            /* [in] */ ContentViewCore* owner,
            /* [in] */ IMotionEvent* eventFakeMouseMove);

        CARAPI Run();

    private:
        ContentViewCore* mOwner;
        IMotionEvent* mEventFakeMouseMove;
    };

    class InnerSelectionHandleController
        : public SelectionHandleController
    {
    public:
        InnerSelectionHandleController(
            /* [in] */ ContentViewCore* owner,
            /* [in] */ IView* parent,
            /* [in] */ PositionObserver* positionObserver);

        CAR_INTERFACE_DECL();

        //@Override
        CARAPI_(void) SelectBetweenCoordinates(
            /* [in] */ Int32 x1,
            /* [in] */ Int32 y1,
            /* [in] */ Int32 x2,
            /* [in] */ Int32 y2);

        //@Override
        CARAPI_(void) ShowHandles(
            /* [in] */ Int32 startDir,
            /* [in] */ Int32 endDir);

    private:
        ContentViewCore* mOwner;
    };

    class InnerInsertionHandleController
        : public InsertionHandleController
    {
    public:
        InnerInsertionHandleController(
            /* [in] */ ContentViewCore* owner,
            /* [in] */ IView* parent,
            /* [in] */ PositionObserver* positionObserver);

        CAR_INTERFACE_DECL();

        //@Override
        CARAPI_(void) SetCursorPosition(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        //@Override
        CARAPI_(void) Paste();

        //@Override
        CARAPI_(Int32) GetLineHeight();

        //@Override
        CARAPI_(void) ShowHandle();

    private:
        ContentViewCore* mOwner;
        static const Int32 AVERAGE_LINE_HEIGHT = 14;
    };

    class InnerActionHandler
        : public SelectActionModeCallback::ActionHandler
    {
    public:
        InnerActionHandler(
            /* [in] */ ContentViewCore* owner);

        //@Override
        CARAPI SelectAll();

        //@Override
        CARAPI Cut();

        //@Override
        CARAPI Copy();

        //@Override
        CARAPI Paste();

        //@Override
        CARAPI Share();

        //@Override
        CARAPI Search();

        //@Override
        CARAPI IsSelectionPassword(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI IsSelectionEditable(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnDestroyActionMode();

        //@Override
        CARAPI IsShareAvailable(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI IsWebSearchAvailable(
            /* [out] */ Boolean* result);

    private:
        ContentViewCore* mOwner;
    };

    class DeferredHandleFadeInRunnable
        : public Runnable
    {
    public:
        DeferredHandleFadeInRunnable(
            /* [in] */ ContentViewCore* owner);

        CARAPI Run();

    private:
        ContentViewCore* mOwner;
    };

    class InnerContentObserver
        : public ContentObserver
        , public Handler
    {
    public:
        InnerContentObserver(
            /* [in] */ ContentViewCore* owner);

        CAR_INTERFACE_DECL();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        ContentViewCore* mOwner;
    };

public:
    /**
     * Constructs a new ContentViewCore. Embedders must call initialize() after constructing
     * a ContentViewCore and before using it.
     *
     * @param context The context used to create this.
     */
    ContentViewCore(
        /* [in] */ IContext* context);

    CAR_INTERFACE_DECL()

    /**
     * @return The context used for creating this ContentViewCore.
     */
    //@CalledByNative
    CARAPI_(AutoPtr<IContext>) GetContext();

    /**
     * @return The ViewGroup that all view actions of this ContentViewCore should interact with.
     */
    CARAPI_(AutoPtr<IViewGroup>) GetContainerView();

    /**
     * @return The WebContents currently being rendered.
     */
    CARAPI_(AutoPtr<WebContents>) GetWebContents();

    /**
     * Specifies how much smaller the WebKit layout size should be relative to the size of this
     * view.
     * @param offsetXPix The X amount in pixels to shrink the viewport by.
     * @param offsetYPix The Y amount in pixels to shrink the viewport by.
     */
    CARAPI_(void) SetViewportSizeOffset(
        /* [in] */ Int32 offsetXPix,
        /* [in] */ Int32 offsetYPix);

    /**
     * Returns a delegate that can be used to add and remove views from the ContainerView.
     *
     * NOTE: Use with care, as not all ContentViewCore users setup their ContainerView in the same
     * way. In particular, the Android WebView has limitations on what implementation details can
     * be provided via a child view, as they are visible in the API and could introduce
     * compatibility breaks with existing applications. If in doubt, contact the
     * android_webview/OWNERS
     *
     * @return A ViewAndroidDelegate that can be used to add and remove views.
     */
    //@VisibleForTesting
    CARAPI_(AutoPtr<ViewElastosDelegate>) GetViewAndroidDelegate();

    //@VisibleForTesting
    CARAPI_(void) SetImeAdapterForTest(
        /* [in] */ ImeAdapter* imeAdapter);

    //@VisibleForTesting
    CARAPI_(AutoPtr<ImeAdapter>) GetImeAdapterForTest();

    //@VisibleForTesting
    CARAPI_(void) SetAdapterInputConnectionFactory(
        /* [in] */ ImeAdapter::AdapterInputConnectionFactory* factory);

    //@VisibleForTesting
    CARAPI_(void) SetInputMethodManagerWrapperForTest(
        /* [in] */ InputMethodManagerWrapper* immw);

    //@VisibleForTesting
    CARAPI_(AutoPtr<AdapterInputConnection>) GetInputConnectionForTest();

    /**
     *
     * @param containerView The view that will act as a container for all views created by this.
     * @param internalDispatcher Handles dispatching all hidden or super methods to the
     *                           containerView.
     * @param nativeWebContents A pointer to the native web contents.
     * @param windowAndroid An instance of the WindowAndroid.
     */
    // Perform important post-construction set up of the ContentViewCore.
    // We do not require the containing view in the constructor to allow embedders to create a
    // ContentViewCore without having fully created its containing view. The containing view
    // is a vital component of the ContentViewCore, so embedders must exercise caution in what
    // they do with the ContentViewCore before calling initialize().
    // We supply the nativeWebContents pointer here rather than in the constructor to allow us
    // to set the private browsing mode at a later point for the WebView implementation.
    // Note that the caller remains the owner of the nativeWebContents and is responsible for
    // deleting it after destroying the ContentViewCore.
    CARAPI_(void) Initialize(
        /* [in] */ IViewGroup* containerView,
        /* [in] */ InternalAccessDelegate* internalDispatcher,
        /* [in] */ Int64 nativeWebContents,
        /* [in] */ WindowElastos* windowElastos);

    /**
     * Sets a new container view for this {@link ContentViewCore}.
     *
     * <p>WARNING: This is not a general purpose method and has been designed with WebView
     * fullscreen in mind. Please be aware that it might not be appropriate for other use cases
     * and that it has a number of limitations. For example the PopupZoomer only works with the
     * container view with which this ContentViewCore has been initialized.
     *
     * <p>This method only performs a small part of replacing the container view and
     * embedders are responsible for:
     * <ul>
     *     <li>Disconnecting the old container view from this ContentViewCore</li>
     *     <li>Updating the InternalAccessDelegate</li>
     *     <li>Reconciling the state of this ContentViewCore with the new container view</li>
     *     <li>Tearing down and recreating the native GL rendering where appropriate</li>
     *     <li>etc.</li>
     * </ul>
     */
    CARAPI_(void) SetContainerView(
        /* [in] */ IViewGroup* containerView);

    //@CalledByNative
    CARAPI_(void) OnNativeContentViewCoreDestroyed(
        /* [in] */ Int64 nativeContentViewCore);

    /**
     * Set the Container view Internals.
     * @param internalDispatcher Handles dispatching all hidden or super methods to the
     *                           containerView.
     */
    CARAPI_(void) SetContainerViewInternals(
        /* [in] */ InternalAccessDelegate* internalDispatcher);

    /**
     * Destroy the internal state of the ContentView. This method may only be
     * called after the ContentView has been removed from the view system. No
     * other methods may be called on this ContentView after this method has
     * been called.
     */
    CARAPI_(void) Destroy();

    /**
     * Returns true initially, false after destroy() has been called.
     * It is illegal to call any other public method after destroy().
     */
    CARAPI_(Boolean) IsAlive();

    /**
     * This is only useful for passing over JNI to native code that requires ContentViewCore*.
     * @return native ContentViewCore pointer.
     */
    //@CalledByNative
    CARAPI_(Int64) GetNativeContentViewCore();

    CARAPI_(void) SetContentViewClient(
        /* [in] */ ContentViewClient* client);

    //@VisibleForTesting
    CARAPI_(AutoPtr<ContentViewClient>) GetContentViewClient();

    CARAPI_(Int32) GetBackgroundColor();

    /**
     * Load url without fixing up the url string. Consumers of ContentView are responsible for
     * ensuring the URL passed in is properly formatted (i.e. the scheme has been added if left
     * off during user input).
     *
     * @param params Parameters for this load.
     */
    CARAPI_(void) LoadUrl(
        /* [in] */ LoadUrlParams* params);

    /**
     * Stops loading the current web contents.
     */
    CARAPI_(void) StopLoading();

    /**
     * Get the URL of the current page.
     *
     * @return The URL of the current page.
     */
    CARAPI_(String) GetUrl();

    /**
     * Get the title of the current page.
     *
     * @return The title of the current page.
     */
    CARAPI_(String) GetTitle();

    /**
     * Shows an interstitial page driven by the passed in delegate.
     *
     * @param url The URL being blocked by the interstitial.
     * @param delegate The delegate handling the interstitial.
     */
    //@VisibleForTesting
    CARAPI_(void) ShowInterstitialPage(
        /* [in] */ const String& url,
        /* [in] */ InterstitialPageDelegateElastos* delegate);

    /**
     * @return Whether the page is currently showing an interstitial, such as a bad HTTPS page.
     */
    CARAPI_(Boolean) IsShowingInterstitialPage();

    /**
     * @return Viewport width in physical pixels as set from onSizeChanged.
     */
    //@CalledByNative
    CARAPI_(Int32) GetViewportWidthPix();

    /**
     * @return Viewport height in physical pixels as set from onSizeChanged.
     */
    //@CalledByNative
    CARAPI_(Int32) GetViewportHeightPix();

    /**
     * @return Width of underlying physical surface.
     */
    //@CalledByNative
    CARAPI_(Int32) GetPhysicalBackingWidthPix();

    /**
     * @return Height of underlying physical surface.
     */
    //@CalledByNative
    CARAPI_(Int32) GetPhysicalBackingHeightPix();

    /**
     * @return Amount the output surface extends past the bottom of the window viewport.
     */
    //@CalledByNative
    CARAPI_(Int32) GetOverdrawBottomHeightPix();

    /**
     * @return The amount to shrink the viewport relative to {@link #getViewportWidthPix()}.
     */
    //@CalledByNative
    CARAPI_(Int32) GetViewportSizeOffsetWidthPix();

    /**
     * @return The amount to shrink the viewport relative to {@link #getViewportHeightPix()}.
     */
    //@CalledByNative
    CARAPI_(Int32) GetViewportSizeOffsetHeightPix();

    /**
     * @see android.webkit.WebView#getContentHeight()
     */
    CARAPI_(Float) GetContentHeightCss();

    /**
     * @see android.webkit.WebView#getContentWidth()
     */
    CARAPI_(Float) GetContentWidthCss();

    // TODO(teddchoc): Remove all these navigation controller methods from here and have the
    //                 embedders manage it.
    /**
     * @return Whether the current WebContents has a previous navigation entry.
     */
    CARAPI_(Boolean) CanGoBack();

    /**
     * @return Whether the current WebContents has a navigation entry after the current one.
     */
    CARAPI_(Boolean) CanGoForward();

    /**
     * @param offset The offset into the navigation history.
     * @return Whether we can move in history by given offset
     */
    CARAPI_(Boolean) CanGoToOffset(
        /* [in] */ Int32 offset);

    /**
     * Navigates to the specified offset from the "current entry". Does nothing if the offset is out
     * of bounds.
     * @param offset The offset into the navigation history.
     */
    CARAPI_(void) GoToOffset(
        /* [in] */ Int32 offset);

    //@Override
    CARAPI GoToNavigationIndex(
        /* [in] */ Int32 index);

    /**
     * Goes to the navigation entry before the current one.
     */
    CARAPI_(void) GoBack();

    /**
     * Goes to the navigation entry following the current one.
     */
    CARAPI_(void) GoForward();

    /**
     * Loads the current navigation if there is a pending lazy load (after tab restore).
     */
    CARAPI_(void) LoadIfNecessary();

    /**
     * Requests the current navigation to be loaded upon the next call to loadIfNecessary().
     */
    CARAPI_(void) RequestRestoreLoad();

    /**
     * Reload the current page.
     */
    CARAPI_(void) Reload(
        /* [in] */ Boolean checkForRepost);

    /**
     * Reload the current page, ignoring the contents of the cache.
     */
    CARAPI_(void) ReloadIgnoringCache(
        /* [in] */ Boolean checkForRepost);

    /**
     * Cancel the pending reload.
     */
    CARAPI_(void) CancelPendingReload();

    /**
     * Continue the pending reload.
     */
    CARAPI_(void) ContinuePendingReload();

    /**
     * Clears the ContentViewCore's page history in both the backwards and
     * forwards directions.
     */
    CARAPI_(void) ClearHistory();

    /**
     * @return The selected text (empty if no text selected).
     */
    CARAPI_(String) GetSelectedText();

    /**
     * @return Whether the current selection is editable (false if no text selected).
     */
    CARAPI_(Boolean) IsSelectionEditable();

    // End FrameLayout overrides.

    /**
     * TODO(changwan): refactor SPen related code into a separate class. See
     * http://crbug.com/398169.
     * @return Whether SPen is supported on the device.
     */
    static CARAPI_(Boolean) IsSPenSupported(
        /* [in] */ IContext* context);

    /**
     * Convert SPen event action into normal event action.
     * TODO(changwan): refactor SPen related code into a separate class. See
     * http://crbug.com/398169.
     *
     * @param eventActionMasked Input event action. It is assumed that it is masked as the values
                                cannot be ORed.
     * @return Event action after the conversion
     */
    static CARAPI_(Int32) ConvertSPenEventAction(
        /* [in] */ Int32 eventActionMasked);

    /**
     * @see View#onTouchEvent(MotionEvent)
     */
    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(void) SetIgnoreRemainingTouchEvents();

    CARAPI_(Boolean) IsScrollInProgress();

    //@VisibleForTesting
    CARAPI_(void) SendDoubleTapForTest(
        /* [in] */ Int64 timeMs,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@VisibleForTesting
    CARAPI_(void) FlingForTest(
        /* [in] */ Int64 timeMs,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 velocityX,
        /* [in] */ Int32 velocityY);

    /**
     * Cancel any fling gestures active.
     * @param timeMs Current time (in milliseconds).
     */
    CARAPI_(void) CancelFling(
        /* [in] */ Int64 timeMs);

    /**
     * Add a listener that gets alerted on gesture state changes.
     * @param listener Listener to add.
     */
    CARAPI_(void) AddGestureStateListener(
        /* [in] */ GestureStateListener* listener);

    /**
     * Removes a listener that was added to watch for gesture state changes.
     * @param listener Listener to remove.
     */
    CARAPI_(void) RemoveGestureStateListener(
        /* [in] */ GestureStateListener* listener);

    CARAPI_(void) UpdateGestureStateListener(
        /* [in] */ Int32 gestureType);

    /**
     * Requests the renderer insert a link to the specified stylesheet in the
     * main frame's document.
     */
    CARAPI_(void) AddStyleSheetByURL(
        /* [in] */ const String& url);

    /**
     * Injects the passed Javascript code in the current page and evaluates it.
     * If a result is required, pass in a callback.
     * Used in automation tests.
     *
     * @param script The Javascript to execute.
     * @param callback The callback to be fired off when a result is ready. The script's
     *                 result will be json encoded and passed as the parameter, and the call
     *                 will be made on the main thread.
     *                 If no result is required, pass null.
     */
    CARAPI_(void) EvaluateJavaScript(
        /* [in] */ const String& script,
        /* [in] */ JavaScriptCallback* callback);

    /**
     * Injects the passed Javascript code in the current page and evaluates it.
     * If there is no page existing, a new one will be created.
     *
     * @param script The Javascript to execute.
     */
    CARAPI_(void) EvaluateJavaScriptEvenIfNotYetNavigated(
        /* [in] */ const String& script);

    /**
     * To be called when the ContentView is shown.
     */
    CARAPI_(void) OnShow();

    /**
     * @return The ID of the renderer process that backs this tab or
     *         {@link #INVALID_RENDER_PROCESS_PID} if there is none.
     */
    CARAPI_(Int32) GetCurrentRenderProcessId();

    /**
     * To be called when the ContentView is hidden.
     */
    CARAPI_(void) OnHide();

    /**
     * Return the ContentSettings object used to retrieve the settings for this
     * ContentViewCore. For modifications, ChromeNativePreferences is to be used.
     * @return A ContentSettings object that can be used to retrieve this
     *         ContentViewCore's settings.
     */
    CARAPI_(AutoPtr<ContentSettings>) GetContentSettings();

    CARAPI_(void) HideSelectActionBar();

    CARAPI_(Boolean) IsSelectActionBarShowing();

    /**
     * @see View#onAttachedToWindow()
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(void) OnAttachedToWindow();

    /**
     * @see View#onDetachedFromWindow()
     */
    //@SuppressWarnings("javadoc")
    //@SuppressLint("MissingSuperCall")
    CARAPI_(void) OnDetachedFromWindow();

    /**
     * @see View#onVisibilityChanged(android.view.View, int)
     */
    CARAPI_(void) OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    /**
     * @see View#onCreateInputConnection(EditorInfo)
     */
    CARAPI_(AutoPtr<IInputConnection>) OnCreateInputConnection(
        /* [in] */ IEditorInfo* outAttrs);

    //@VisibleForTesting
    CARAPI_(AutoPtr<AdapterInputConnection>) GetAdapterInputConnectionForTest();

    //@VisibleForTesting
    CARAPI_(AutoPtr<IEditable>) GetEditableForTest();

    /**
     * @see View#onCheckIsTextEditor()
     */
    CARAPI_(Boolean) OnCheckIsTextEditor();

    /**
     * @see View#onConfigurationChanged(Configuration)
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * @see View#onSizeChanged(int, int, int, int)
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 wPix,
        /* [in] */ Int32 hPix,
        /* [in] */ Int32 owPix,
        /* [in] */ Int32 ohPix);

    /**
     * Called when the underlying surface the compositor draws to changes size.
     * This may be larger than the viewport size.
     */
    CARAPI_(void) OnPhysicalBackingSizeChanged(
        /* [in] */ Int32 wPix,
        /* [in] */ Int32 hPix);

    /**
     * Called when the amount the surface is overdrawing off the bottom has changed.
     * @param overdrawHeightPix The overdraw height.
     */
    CARAPI_(void) OnOverdrawBottomHeightChanged(
        /* [in] */ Int32 overdrawHeightPix);

    /**
     * Selects the word around the caret, if any.
     * The caller can check if selection actually occurred by listening to OnSelectionChanged.
     */
    CARAPI_(void) SelectWordAroundCaret();

    /**
     * @see View#onWindowFocusChanged(boolean)
     */
    CARAPI_(void) OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus);

    /**
     * @see View#onKeyUp(int, KeyEvent)
     */
    CARAPI_(Boolean) OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    /**
     * @see View#dispatchKeyEventPreIme(KeyEvent)
     */
    CARAPI_(Boolean) DispatchKeyEventPreIme(
        /* [in] */ IKeyEvent* event);

    /**
     * @see View#dispatchKeyEvent(KeyEvent)
     */
    CARAPI_(Boolean) DispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    /**
     * @see View#onHoverEvent(MotionEvent)
     * Mouse move events are sent on hover enter, hover move and hover exit.
     * They are sent on hover exit because sometimes it acts as both a hover
     * move and hover exit.
     */
    CARAPI_(Boolean) OnHoverEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * @see View#onGenericMotionEvent(MotionEvent)
     */
    CARAPI_(Boolean) OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * Sets the current amount to offset incoming touch events by.  This is used to handle content
     * moving and not lining up properly with the android input system.
     * @param dx The X offset in pixels to shift touch events.
     * @param dy The Y offset in pixels to shift touch events.
     */
    CARAPI_(void) SetCurrentMotionEventOffsets(
        /* [in] */ Float dx,
        /* [in] */ Float dy);

    /**
     * @see View#scrollBy(int, int)
     * Currently the ContentView scrolling happens in the native side. In
     * the Java view system, it is always pinned at (0, 0). scrollBy() and scrollTo()
     * are overridden, so that View's mScrollX and mScrollY will be unchanged at
     * (0, 0). This is critical for drawing ContentView correctly.
     */
    CARAPI_(void) ScrollBy(
        /* [in] */ Int32 xPix,
        /* [in] */ Int32 yPix);

    /**
     * @see View#scrollTo(int, int)
     */
    CARAPI_(void) ScrollTo(
        /* [in] */ Int32 xPix,
        /* [in] */ Int32 yPix);

    // NOTE: this can go away once ContentView.getScrollX() reports correct values.
    //       see: b/6029133
    CARAPI_(Int32) GetNativeScrollXForTest();

    // NOTE: this can go away once ContentView.getScrollY() reports correct values.
    //       see: b/6029133
    CARAPI_(Int32) GetNativeScrollYForTest();

    /**
     * @see View#computeHorizontalScrollExtent()
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(Int32) ComputeHorizontalScrollExtent();

    /**
     * @see View#computeHorizontalScrollOffset()
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(Int32) ComputeHorizontalScrollOffset();

    /**
     * @see View#computeHorizontalScrollRange()
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(Int32) ComputeHorizontalScrollRange();

    /**
     * @see View#computeVerticalScrollExtent()
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(Int32) ComputeVerticalScrollExtent();

    /**
     * @see View#computeVerticalScrollOffset()
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(Int32) ComputeVerticalScrollOffset();

    /**
     * @see View#computeVerticalScrollRange()
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(Int32) ComputeVerticalScrollRange();

    // End FrameLayout overrides.

    /**
     * @see View#awakenScrollBars(int, boolean)
     */
    //@SuppressWarnings("javadoc")
    CARAPI_(Boolean) AwakenScrollBars(
        /* [in] */ Int32 startDelay,
        /* [in] */ Boolean invalidate);

    /**
     * @return The x coordinate for the last point that a tap or press gesture was initiated from.
     */
    CARAPI_(Int32) GetLastTapX();

    /**
     * @return The y coordinate for the last point that a tap or press gesture was initiated from.
     */
    CARAPI_(Int32) GetLastTapY();

    CARAPI_(void) SetZoomControlsDelegate(
        /* [in] */ ZoomControlsDelegate* zoomControlsDelegate);

    CARAPI_(void) UpdateMultiTouchZoomSupport(
        /* [in] */ Boolean supportsMultiTouchZoom);

    CARAPI_(void) UpdateDoubleTapSupport(
        /* [in] */ Boolean supportsDoubleTap);

    CARAPI_(void) SelectPopupMenuItems(
        /* [in] */ ArrayOf<Int32>* indices);

    /**
     * Send the screen orientation value to the renderer.
     */
    //@VisibleForTesting
    CARAPI_(void) SendOrientationChangeEvent(
        /* [in] */ Int32 orientation);

    /**
     * Register the delegate to be used when content can not be handled by
     * the rendering engine, and should be downloaded instead. This will replace
     * the current delegate, if any.
     * @param delegate An implementation of ContentViewDownloadDelegate.
     */
    CARAPI_(void) SetDownloadDelegate(
        /* [in] */ ContentViewDownloadDelegate* delegate);

    // Called by DownloadController.
    CARAPI_(AutoPtr<ContentViewDownloadDelegate>) GetDownloadDelegate();

    //@VisibleForTesting
    CARAPI_(AutoPtr<InsertionHandleController>) GetInsertionHandleControllerForTest();

    //@VisibleForTesting
    CARAPI_(AutoPtr<SelectionHandleController>) GetSelectionHandleControllerForTest();

    CARAPI_(Boolean) GetUseDesktopUserAgent();

    /**
     * Set whether or not we're using a desktop user agent for the currently loaded page.
     * @param override If true, use a desktop user agent.  Use a mobile one otherwise.
     * @param reloadOnChange Reload the page if the UA has changed.
     */
    CARAPI_(void) SetUseDesktopUserAgent(
        /* [in] */ Boolean override,
        /* [in] */ Boolean reloadOnChange);

    CARAPI_(void) ClearSslPreferences();

    /**
     * Shows the IME if the focused widget could accept text input.
     */
    CARAPI_(void) ShowImeIfNeeded();

    /**
     * Hides the IME if the containerView is the active view for IME.
     */
    CARAPI_(void) HideImeIfNeeded();

    /**
     * @return The visible select popup being shown.
     */
    CARAPI_(AutoPtr<SelectPopup>) GetSelectPopupForTest();

    /**
     * Attaches the native ImeAdapter object to the java ImeAdapter to allow communication via JNI.
     */
    CARAPI_(void) AttachImeAdapter();

    /**
     * @see View#hasFocus()
     */
    //@CalledByNative
    CARAPI_(Boolean) HasFocus();

    /**
     * Checks whether the ContentViewCore can be zoomed in.
     *
     * @return True if the ContentViewCore can be zoomed in.
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    CARAPI_(Boolean) CanZoomIn();

    /**
     * Checks whether the ContentViewCore can be zoomed out.
     *
     * @return True if the ContentViewCore can be zoomed out.
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    CARAPI_(Boolean) CanZoomOut();

    /**
     * Zooms in the ContentViewCore by 25% (or less if that would result in
     * zooming in more than possible).
     *
     * @return True if there was a zoom change, false otherwise.
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    CARAPI_(Boolean) ZoomIn();

    /**
     * Zooms out the ContentViewCore by 20% (or less if that would result in
     * zooming out more than possible).
     *
     * @return True if there was a zoom change, false otherwise.
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    CARAPI_(Boolean) ZoomOut();

    /**
     * Resets the zoom factor of the ContentViewCore.
     *
     * @return True if there was a zoom change, false otherwise.
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    CARAPI_(Boolean) ZoomReset();

    /**
     * Simulate a pinch zoom gesture.
     *
     * @param delta the factor by which the current page scale should be multiplied by.
     * @return whether the gesture was sent.
     */
    CARAPI_(Boolean) PinchByDelta(
        /* [in] */ Float delta);

    /**
     * Invokes the graphical zoom picker widget for this ContentView.
     */
    CARAPI_(void) InvokeZoomPicker();

    /**
     * Enables or disables inspection of JavaScript objects added via
     * {@link #addJavascriptInterface(Object, String)} by means of Object.keys() method and
     * &quot;for .. in&quot; loop. Being able to inspect JavaScript objects is useful
     * when debugging hybrid Android apps, but can't be enabled for legacy applications due
     * to compatibility risks.
     *
     * @param allow Whether to allow JavaScript objects inspection.
     */
    CARAPI_(void) SetAllowJavascriptInterfacesInspection(
        /* [in] */ Boolean allow);

    /**
     * Returns JavaScript interface objects previously injected via
     * {@link #addJavascriptInterface(Object, String)}.
     *
     * @return the mapping of names to interface objects and corresponding annotation classes
     */
//    CARAPI_(Map<String, Pair<Object, Class>>) GetJavascriptInterfaces();
     CARAPI_(AutoPtr<IMap>) GetJavascriptInterfaces();

    /**
     * This will mimic {@link #addPossiblyUnsafeJavascriptInterface(Object, String, Class)}
     * and automatically pass in {@link JavascriptInterface} as the required annotation.
     *
     * @param object The Java object to inject into the ContentViewCore's JavaScript context.  Null
     *               values are ignored.
     * @param name   The name used to expose the instance in JavaScript.
     */
    CARAPI_(void) AddJavascriptInterface(
        /* [in] */ IInterface* object,
        /* [in] */ const String& name);

    /**
     * This method injects the supplied Java object into the ContentViewCore.
     * The object is injected into the JavaScript context of the main frame,
     * using the supplied name. This allows the Java object to be accessed from
     * JavaScript. Note that that injected objects will not appear in
     * JavaScript until the page is next (re)loaded. For example:
     * <pre> view.addJavascriptInterface(new Object(), "injectedObject");
     * view.loadData("<!DOCTYPE html><title></title>", "text/html", null);
     * view.loadUrl("javascript:alert(injectedObject.toString())");</pre>
     * <p><strong>IMPORTANT:</strong>
     * <ul>
     * <li> addJavascriptInterface() can be used to allow JavaScript to control
     * the host application. This is a powerful feature, but also presents a
     * security risk. Use of this method in a ContentViewCore containing
     * untrusted content could allow an attacker to manipulate the host
     * application in unintended ways, executing Java code with the permissions
     * of the host application. Use extreme care when using this method in a
     * ContentViewCore which could contain untrusted content. Particular care
     * should be taken to avoid unintentional access to inherited methods, such
     * as {@link Object#getClass()}. To prevent access to inherited methods,
     * pass an annotation for {@code requiredAnnotation}.  This will ensure
     * that only methods with {@code requiredAnnotation} are exposed to the
     * Javascript layer.  {@code requiredAnnotation} will be passed to all
     * subsequently injected Java objects if any methods return an object.  This
     * means the same restrictions (or lack thereof) will apply.  Alternatively,
     * {@link #addJavascriptInterface(Object, String)} can be called, which
     * automatically uses the {@link JavascriptInterface} annotation.
     * <li> JavaScript interacts with Java objects on a private, background
     * thread of the ContentViewCore. Care is therefore required to maintain
     * thread safety.</li>
     * </ul></p>
     *
     * @param object             The Java object to inject into the
     *                           ContentViewCore's JavaScript context. Null
     *                           values are ignored.
     * @param name               The name used to expose the instance in
     *                           JavaScript.
     * @param requiredAnnotation Restrict exposed methods to ones with this
     *                           annotation.  If {@code null} all methods are
     *                           exposed.
     *
     */
    CARAPI_(void) AddPossiblyUnsafeJavascriptInterface(
        /* [in] */ IInterface* object,
        /* [in] */ const String& name,
        /* [in] */ IInterface* requiredAnnotation);

    /**
     * Removes a previously added JavaScript interface with the given name.
     *
     * @param name The name of the interface to remove.
     */
    CARAPI_(void) RemoveJavascriptInterface(
        /* [in] */ const String& name);

    /**
     * Return the current scale of the ContentView.
     * @return The current page scale factor.
     */
    CARAPI_(Float) GetScale();

    /**
     * If the view is ready to draw contents to the screen. In hardware mode,
     * the initialization of the surface texture may not occur until after the
     * view has been added to the layout. This method will return {@code true}
     * once the texture is actually ready.
     */
    CARAPI_(Boolean) IsReady();

    //@Override
    CARAPI OnAccessibilityStateChanged(
        /* [in] */ Boolean enabled);

    /**
     * Determines whether or not this ContentViewCore can handle this accessibility action.
     * @param action The action to perform.
     * @return Whether or not this action is supported.
     */
    CARAPI_(Boolean) SupportsAccessibilityAction(
        /* [in] */ Int32 action);

    /**
     * Attempts to perform an accessibility action on the web content.  If the accessibility action
     * cannot be processed, it returns {@code null}, allowing the caller to know to call the
     * super {@link View#performAccessibilityAction(int, Bundle)} method and use that return value.
     * Otherwise the return value from this method should be used.
     * @param action The action to perform.
     * @param arguments Optional action arguments.
     * @return Whether the action was performed or {@code null} if the call should be delegated to
     *         the super {@link View} class.
     */
    CARAPI_(Boolean) PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    /**
     * Set the BrowserAccessibilityManager, used for native accessibility
     * (not script injection). This is only set when system accessibility
     * has been enabled.
     * @param manager The new BrowserAccessibilityManager.
     */
    CARAPI_(void) SetBrowserAccessibilityManager(
        /* [in] */ BrowserAccessibilityManager* manager);

    /**
     * Get the BrowserAccessibilityManager, used for native accessibility
     * (not script injection). This will return null when system accessibility
     * is not enabled.
     * @return This view's BrowserAccessibilityManager.
     */
    CARAPI_(AutoPtr<BrowserAccessibilityManager>) GetBrowserAccessibilityManager();

    /**
     * If native accessibility (not script injection) is enabled, and if this is
     * running on JellyBean or later, returns an AccessibilityNodeProvider that
     * implements native accessibility for this view. Returns null otherwise.
     * Lazily initializes native accessibility here if it's allowed.
     * @return The AccessibilityNodeProvider, if available, or null otherwise.
     */
    CARAPI_(AutoPtr<IAccessibilityNodeProvider>) GetAccessibilityNodeProvider();

    /**
     * @see View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
     */
    CARAPI_(void) OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * @see View#onInitializeAccessibilityEvent(AccessibilityEvent)
     */
    CARAPI_(void) OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Returns whether accessibility script injection is enabled on the device
     */
    CARAPI_(Boolean) IsDeviceAccessibilityScriptInjectionEnabled();

    /**
     * Returns whether or not accessibility injection is being used.
     */
    CARAPI_(Boolean) IsInjectingAccessibilityScript();

    /**
     * Returns true if accessibility is on and touch exploration is enabled.
     */
    CARAPI_(Boolean) IsTouchExplorationEnabled();

    /**
     * Turns browser accessibility on or off.
     * If |state| is |false|, this turns off both native and injected accessibility.
     * Otherwise, if accessibility script injection is enabled, this will enable the injected
     * accessibility scripts. Native accessibility is enabled on demand.
     */
    CARAPI_(void) SetAccessibilityState(
        /* [in] */ Boolean state);

    /**
     * Enable or disable injected accessibility features
     */
    CARAPI_(void) SetInjectedAccessibility(
        /* [in] */ Boolean enabled);

    /**
     * Stop any TTS notifications that are currently going on.
     */
    CARAPI_(void) StopCurrentAccessibilityNotifications();

    /**
     * Return whether or not we should set accessibility focus on page load.
     */
    CARAPI_(Boolean) ShouldSetAccessibilityFocusOnPageLoad();

    /**
     * Sets whether or not we should set accessibility focus on page load.
     * This only applies if an accessibility service like TalkBack is running.
     * This is desirable behavior for a browser window, but not for an embedded
     * WebView.
     */
    CARAPI_(void) SetShouldSetAccessibilityFocusOnPageLoad(
        /* [in] */ Boolean on);

    /**
     * Inform WebKit that Fullscreen mode has been exited by the user.
     */
    CARAPI_(void) ExitFullscreen();

    /**
     * Changes whether hiding the top controls is enabled.
     *
     * @param enableHiding Whether hiding the top controls should be enabled or not.
     * @param enableShowing Whether showing the top controls should be enabled or not.
     * @param animate Whether the transition should be animated or not.
     */
    CARAPI_(void) UpdateTopControlsState(
        /* [in] */ Boolean enableHiding,
        /* [in] */ Boolean enableShowing,
        /* [in] */ Boolean animate);

    /**
     * Get a copy of the navigation history of the view.
     */
    CARAPI_(AutoPtr<NavigationHistory>) GetNavigationHistory();

    //@Override
    CARAPI_(AutoPtr<NavigationHistory>) GetDirectedNavigationHistory(
        /* [in] */ Boolean isForward,
        /* [in] */ Int32 itemLimit);

    /**
     * @return The original request URL for the current navigation entry, or null if there is no
     *         current entry.
     */
    CARAPI_(String) GetOriginalUrlForActiveNavigationEntry();

    /**
     * @return The cached copy of render positions and scales.
     */
    CARAPI_(AutoPtr<RenderCoordinates>) GetRenderCoordinates();

    CARAPI_(void) ExtractSmartClipData(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Set offsets for smart clip.
     *
     * <p>This should be called if there is a viewport change introduced by,
     * e.g., show and hide of a location bar.
     *
     * @param offsetX Offset for X position.
     * @param offsetY Offset for Y position.
     */
    CARAPI_(void) SetSmartClipOffsets(
        /* [in] */ Int32 offsetX,
        /* [in] */ Int32 offsetY);

    CARAPI_(void) SetSmartClipDataListener(
        /* [in] */ SmartClipDataListener* listener);

    CARAPI_(void) SetBackgroundOpaque(
        /* [in] */ Boolean opaque);

    //@Override
    CARAPI OnScreenOrientationChanged(
        /* [in] */ Int32 orientation);

    static CARAPI_(void*) ElaContentViewCoreCallback_Init();

private:

    static CARAPI_(AutoPtr<IInterface>) GetContext(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnNativeContentViewCoreDestroyed(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 nativeContentViewCore);

    static CARAPI_(Int64) GetNativeContentViewCore(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnBackgroundColorChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 color);

    static CARAPI_(Int32) GetViewportWidthPix(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetViewportHeightPix(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetPhysicalBackingWidthPix(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetPhysicalBackingHeightPix(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetOverdrawBottomHeightPix(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetViewportSizeOffsetWidthPix(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetViewportSizeOffsetHeightPix(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnFlingStartEventConsumed(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 vx,
        /* [in] */ Int32 vy);

    static CARAPI_(void) OnFlingStartEventHadNoConsumer(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 vx,
        /* [in] */ Int32 vy);

    static CARAPI_(void) OnFlingCancelEventAck(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnScrollBeginEventAck(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnScrollUpdateGestureConsumed(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnScrollEndEventAck(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnPinchBeginEventAck(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnPinchEndEventAck(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnSingleTapEventAck(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean consumed,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    static CARAPI_(void) OnDoubleTapEventAck(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) FilterTapOrPressEvent(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 type,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    static CARAPI_(void) UpdateFrameInfo(
        /* [in] */ IInterface* obj,
        /* [in] */ Float scrollOffsetX,
        /* [in] */ Float scrollOffsetY,
        /* [in] */ Float pageScaleFactor,
        /* [in] */ Float minPageScaleFactor,
        /* [in] */ Float maxPageScaleFactor,
        /* [in] */ Float contentWidth,
        /* [in] */ Float contentHeight,
        /* [in] */ Float viewportWidth,
        /* [in] */ Float viewportHeight,
        /* [in] */ Float controlsOffsetYCss,
        /* [in] */ Float contentOffsetYCss,
        /* [in] */ Float overdrawBottomHeightCss);

    static CARAPI_(void) UpdateImeAdapter(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 nativeImeAdapterAndroid,
        /* [in] */ Int32 textInputType,
        /* [in] */ const String& text,
        /* [in] */ Int32 selectionStart,
        /* [in] */ Int32 selectionEnd,
        /* [in] */ Int32 compositionStart,
        /* [in] */ Int32 compositionEnd,
        /* [in] */ Boolean showImeIfNeeded,
        /* [in] */ Boolean isNonImeChange);

    static CARAPI_(void) SetTitle(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& title);

    static CARAPI_(void) ShowSelectPopup(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* bounds,
        /* [in] */ ArrayOf<String>* items,
        /* [in] */ ArrayOf<Int32>* enabled,
        /* [in] */ Boolean multiple,
        /* [in] */ ArrayOf<Int32>* selectedIndices);

    static CARAPI_(void) HideSelectPopup(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) ShowDisambiguationPopup(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* targetRect,
        /* [in] */ IInterface* zoomedBitmap);

    static CARAPI_(AutoPtr<IInterface>) CreateTouchEventSynthesizer(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnSelectionChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& text);

    static CARAPI_(void) ShowSelectionHandlesAutomatically(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnSelectionBoundsChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* anchorRectDip,
        /* [in] */ Int32 anchorDir,
        /* [in] */ IInterface* focusRectDip,
        /* [in] */ Int32 focusDir,
        /* [in] */ Boolean isAnchorFirst);

    static CARAPI_(void) OnEvaluateJavaScriptResult(
        /* [in] */ const String& jsonResult,
        /* [in] */ IInterface* callback);

    static CARAPI_(void) ShowPastePopup(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 xDip,
        /* [in] */ Int32 yDip);

    static CARAPI_(void) OnRenderProcessChange(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) HasFocus(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) StartContentIntent(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& contentUrl);

    static CARAPI_(void) AddToNavigationHistory(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* history,
        /* [in] */ Int32 index,
        /* [in] */ const String& url,
        /* [in] */ const String& virtualUrl,
        /* [in] */ const String& originalUrl,
        /* [in] */ const String& title,
        /* [in] */ IInterface* favicon);

    static CARAPI_(void) OnSmartClipDataExtracted(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& text,
        /* [in] */ const String& html,
        /* [in] */ IInterface* clipRect);

    static CARAPI_(AutoPtr<IInterface>) GetContentVideoViewClient(
        /* [in] */ IInterface* obj);

    static CARAPI_(Boolean) ShouldBlockMediaRequest(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url);

    static CARAPI_(void) OnNativeFlingStopped(
        /* [in] */ IInterface* obj);

    CARAPI_(AutoPtr<ImeAdapter>) CreateImeAdapter(
        /* [in] */ IContext* context);

    CARAPI_(void) InitPopupZoomer(
        /* [in] */ IContext* context);

    CARAPI_(void) UnregisterAccessibilityContentObserver();

    //@CalledByNative
    CARAPI_(void) OnBackgroundColorChanged(
        /* [in] */ Int32 color);

    static CARAPI_(Boolean) DetectSPenSupport(
        /* [in] */ IContext* context);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnFlingStartEventConsumed(
        /* [in] */ Int32 vx,
        /* [in] */ Int32 vy);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnFlingStartEventHadNoConsumer(
        /* [in] */ Int32 vx,
        /* [in] */ Int32 vy);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnFlingCancelEventAck();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnScrollBeginEventAck();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnScrollUpdateGestureConsumed();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnScrollEndEventAck();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnPinchBeginEventAck();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnPinchEndEventAck();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnSingleTapEventAck(
        /* [in] */ Boolean consumed,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnDoubleTapEventAck();

    /**
     * Called just prior to a tap or press gesture being forwarded to the renderer.
     */
    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(Boolean) FilterTapOrPressEvent(
        /* [in] */ Int32 type,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) HidePopups();

    CARAPI_(void) ResetGestureDetection();

    CARAPI_(void) UpdateAfterSizeChanged();

    CARAPI_(void) CancelRequestToScrollFocusedEditableNodeIntoView();

    CARAPI_(void) ScrollFocusedEditableNodeIntoView();

    CARAPI_(AutoPtr<IMotionEvent>) CreateOffsetMotionEvent(
        /* [in] */ IMotionEvent* src);

    CARAPI_(void) UpdateForTapOrPress(
        /* [in] */ Int32 type,
        /* [in] */ Float xPix,
        /* [in] */ Float yPix);

    CARAPI_(AutoPtr<SelectionHandleController>) GetSelectionHandleController();

    CARAPI_(AutoPtr<InsertionHandleController>) GetInsertionHandleController();

    CARAPI_(void) UpdateHandleScreenPositions();

    CARAPI_(void) HideHandles();

    CARAPI_(void) ShowSelectActionBar();

    CARAPI_(Boolean) IsSelectionHandleShowing();

    CARAPI_(Boolean) IsInsertionHandleShowing();

    // Makes the insertion/selection handles invisible. They will fade back in shortly after the
    // last call to scheduleTextHandleFadeIn (or temporarilyHideTextHandles).
    CARAPI_(void) TemporarilyHideTextHandles();

    CARAPI_(Boolean) AllowTextHandleFadeIn();

    // Cancels any pending fade in and schedules a new one.
    CARAPI_(void) ScheduleTextHandleFadeIn();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) UpdateFrameInfo(
        /* [in] */ Float scrollOffsetX,
        /* [in] */ Float scrollOffsetY,
        /* [in] */ Float pageScaleFactor,
        /* [in] */ Float minPageScaleFactor,
        /* [in] */ Float maxPageScaleFactor,
        /* [in] */ Float contentWidth,
        /* [in] */ Float contentHeight,
        /* [in] */ Float viewportWidth,
        /* [in] */ Float viewportHeight,
        /* [in] */ Float controlsOffsetYCss,
        /* [in] */ Float contentOffsetYCss,
        /* [in] */ Float overdrawBottomHeightCss);

    //@CalledByNative
    CARAPI_(void) UpdateImeAdapter(
        /* [in] */ Int64 nativeImeAdapterAndroid,
        /* [in] */ Int32 textInputType,
        /* [in] */ const String& text,
        /* [in] */ Int32 selectionStart,
        /* [in] */ Int32 selectionEnd,
        /* [in] */ Int32 compositionStart,
        /* [in] */ Int32 compositionEnd,
        /* [in] */ Boolean showImeIfNeeded,
        /* [in] */ Boolean isNonImeChange);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) SetTitle(
        /* [in] */ const String& title);

    /**
     * Called (from native) when the <select> popup needs to be shown.
     * @param items           Items to show.
     * @param enabled         POPUP_ITEM_TYPEs for items.
     * @param multiple        Whether the popup menu should support multi-select.
     * @param selectedIndices Indices of selected items.
     */
    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) ShowSelectPopup(
        /* [in] */ IRect* bounds,
        /* [in] */ ArrayOf<String>* items,
        /* [in] */ ArrayOf<Int32>* enabled,
        /* [in] */ Boolean multiple,
        /* [in] */ ArrayOf<Int32>* selectedIndices);

    /**
     * Called when the <select> popup needs to be hidden.
     */
    //@CalledByNative
    CARAPI_(void) HideSelectPopup();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) ShowDisambiguationPopup(
        /* [in] */ IRect* targetRect,
        /* [in] */ IBitmap* zoomedBitmap);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(AutoPtr<TouchEventSynthesizer>) CreateTouchEventSynthesizer();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnSelectionChanged(
        /* [in] */ const String& text);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) ShowSelectionHandlesAutomatically();

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnSelectionBoundsChanged(
        /* [in] */ IRect* anchorRectDip,
        /* [in] */ Int32 anchorDir,
        /* [in] */ IRect* focusRectDip,
        /* [in] */ Int32 focusDir,
        /* [in] */ Boolean isAnchorFirst);

    //@SuppressWarnings("unused")
    //@CalledByNative
    static CARAPI_(void) OnEvaluateJavaScriptResult(
        /* [in] */ const String& jsonResult,
        /* [in] */ JavaScriptCallback* callback);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) ShowPastePopup(
        /* [in] */ Int32 xDip,
        /* [in] */ Int32 yDip);

    //@SuppressWarnings("unused")
    //@CalledByNative
    CARAPI_(void) OnRenderProcessChange();

    //@CalledByNative
    CARAPI_(void) StartContentIntent(
        /* [in] */ const String& contentUrl);

    /**
     * Callback factory method for nativeGetNavigationHistory().
     */
    //@CalledByNative
    CARAPI_(void) AddToNavigationHistory(
        /* [in] */ IInterface* history,
        /* [in] */ Int32 index,
        /* [in] */ const String& url,
        /* [in] */ const String& virtualUrl,
        /* [in] */ const String& originalUrl,
        /* [in] */ const String& title,
        /* [in] */ IBitmap* favicon);

    //@CalledByNative return IRect
    static CARAPI_(AutoPtr<IInterface>) CreateRect(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    //@CalledByNative
    CARAPI_(void) OnSmartClipDataExtracted(
        /* [in] */ const String& text,
        /* [in] */ const String& html,
        /* [in] */ IRect* clipRect);

    /**
     * Offer a long press gesture to the embedding View, primarily for WebView compatibility.
     *
     * @return true if the embedder handled the event.
     */
    CARAPI_(Boolean) OfferLongPressToEmbedder();

    /**
     * Reset scroll and fling accounting, notifying listeners as appropriate.
     * This is useful as a failsafe when the input stream may have been interruped.
     */
    CARAPI_(void) ResetScrollInProgress();

    CARAPI_(Int64) NativeInit(
        /* [in] */ Int64 webContentsPtr,
        /* [in] */ Int64 viewAndroidPtr,
        /* [in] */ Int64 windowAndroidPtr,
        /* [in] */ IHashSet* retainedObjectSet);

    //@CalledByNative
    CARAPI_(AutoPtr<ContentVideoViewClient>) GetContentVideoViewClient();

    //@CalledByNative
    CARAPI_(Boolean) ShouldBlockMediaRequest(
        /* [in] */ const String& url);

    //@CalledByNative
    CARAPI_(void) OnNativeFlingStopped();

    CARAPI_(AutoPtr<WebContents>) NativeGetWebContentsAndroid(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeOnJavaContentViewCoreDestroyed(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeLoadUrl(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ const String& url,
        /* [in] */ Int32 loadUrlType,
        /* [in] */ Int32 transitionType,
        /* [in] */ const String& referrerUrl,
        /* [in] */ Int32 referrerPolicy,
        /* [in] */ Int32 uaOverrideOption,
        /* [in] */ const String& extraHeaders,
        /* [in] */ ArrayOf<Byte>* postData,
        /* [in] */ const String& baseUrlForDataUrl,
        /* [in] */ const String& virtualUrlForDataUrl,
        /* [in] */ Boolean canLoadLocalResources,
        /* [in] */ Boolean isRendererInitiated);

    CARAPI_(String) NativeGetURL(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeShowInterstitialPage(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ const String& url,
        /* [in] */ Int64 nativeInterstitialPageDelegateAndroid);

    CARAPI_(Boolean) NativeIsShowingInterstitialPage(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(Boolean) NativeIsIncognito(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeSetFocus(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean focused);

    CARAPI_(void) NativeSendOrientationChangeEvent(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int32 orientation);

    // All touch events (including flings, scrolls etc) accept coordinates in physical pixels.
    CARAPI_(Boolean) NativeOnTouchEvent(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int64 timeMs,
        /* [in] */ Int32 action,
        /* [in] */ Int32 pointerCount,
        /* [in] */ Int32 historySize,
        /* [in] */ Int32 actionIndex,
        /* [in] */ Float x0,
        /* [in] */ Float y0,
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Int32 pointerId0,
        /* [in] */ Int32 pointerId1,
        /* [in] */ Float touchMajor0,
        /* [in] */ Float touchMajor1,
        /* [in] */ Float rawX,
        /* [in] */ Float rawY,
        /* [in] */ Int32 androidToolType0,
        /* [in] */ Int32 androidToolType1,
        /* [in] */ Int32 androidButtonState);

    CARAPI_(Int32) NativeSendMouseMoveEvent(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(Int32) NativeSendMouseWheelEvent(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float verticalAxis);

    CARAPI_(void) NativeScrollBegin(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float hintX,
        /* [in] */ Float hintY);

    CARAPI_(void) NativeScrollEnd(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs);

    CARAPI_(void) NativeScrollBy(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float deltaX,
        /* [in] */ Float deltaY);

    CARAPI_(void) NativeFlingStart(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float vx,
        /* [in] */ Float vy);

    CARAPI_(void) NativeFlingCancel(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs);

    CARAPI_(void) NativeSingleTap(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) NativeDoubleTap(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) NativeLongPress(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) NativePinchBegin(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) NativePinchEnd(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs);

    CARAPI_(void) NativePinchBy(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int64 timeMs,
        /* [in] */ Float anchorX,
        /* [in] */ Float anchorY,
        /* [in] */ Float deltaScale);

    CARAPI_(void) NativeSelectBetweenCoordinates(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Float x2,
        /* [in] */ Float y2);

    CARAPI_(void) NativeMoveCaret(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Float x,
        /* [in] */ Float y);

    CARAPI_(void) NativeResetGestureDetection(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeSetDoubleTapSupportEnabled(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean enabled);

    CARAPI_(void) NativeSetMultiTouchZoomSupportEnabled(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean enabled);

    CARAPI_(void) NativeLoadIfNecessary(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeRequestRestoreLoad(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeReload(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean checkForRepost);

    CARAPI_(void) NativeReloadIgnoringCache(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean checkForRepost);

    CARAPI_(void) NativeCancelPendingReload(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeContinuePendingReload(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeSelectPopupMenuItems(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ ArrayOf<Int32>* indices);

    CARAPI_(void) NativeScrollFocusedEditableNodeIntoView(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeSelectWordAroundCaret(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeClearHistory(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeAddStyleSheetByURL(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ const String& stylesheetUrl);

    CARAPI_(void) NativeEvaluateJavaScript(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ const String& script,
        /* [in] */ JavaScriptCallback* callback,
        /* [in] */ Boolean startRenderer);

    CARAPI_(Int64) NativeGetNativeImeAdapter(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(Int32) NativeGetCurrentRenderProcessId(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(Int32) NativeGetBackgroundColor(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeOnShow(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeOnHide(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeSetUseDesktopUserAgent(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean reloadOnChange);

    CARAPI_(Boolean) NativeGetUseDesktopUserAgent(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeClearSslPreferences(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeSetAllowJavascriptInterfacesInspection(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean allow);

    CARAPI_(void) NativeAddJavascriptInterface(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ IInterface* object,
        /* [in] */ const String& name,
        /* [in] */ IInterface* requiredAnnotation);

    CARAPI_(void) NativeRemoveJavascriptInterface(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ const String& name);

    CARAPI_(Int32) NativeGetNavigationHistory(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ IInterface* context);

    CARAPI_(void) NativeGetDirectedNavigationHistory(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ IInterface* context,
        /* [in] */ Boolean isForward,
        /* [in] */ Int32 maxEntries);

    CARAPI_(String) NativeGetOriginalUrlForActiveNavigationEntry(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeWasResized(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(Boolean) NativeIsRenderWidgetHostViewReady(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeExitFullscreen(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeUpdateTopControlsState(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean enableHiding,
        /* [in] */ Boolean enableShowing,
        /* [in] */ Boolean animate);

    CARAPI_(void) NativeShowImeIfNeeded(
        /* [in] */ Int64 nativeContentViewCoreImpl);

    CARAPI_(void) NativeSetAccessibilityEnabled(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean enabled);

    CARAPI_(void) NativeExtractSmartClipData(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    CARAPI_(void) NativeSetBackgroundOpaque(
        /* [in] */ Int64 nativeContentViewCoreImpl,
        /* [in] */ Boolean opaque);

public:
    /**
     * PID used to indicate an invalid render process.
     */
    // Keep in sync with the value returned from ContentViewCoreImpl::GetCurrentRendererProcessId()
    // if there is no render process.
    static const Int32 INVALID_RENDER_PROCESS_PID = 0;

private:
    static const String TAG;

    // Used to avoid enabling zooming in / out if resulting zooming will
    // produce little visible difference.
    static const Float ZOOM_CONTROLS_EPSILON;

    // Used to represent gestures for long press and long tap.
    static const Int32 IS_LONG_PRESS = 1;
    static const Int32 IS_LONG_TAP = 2;

    // Length of the delay (in ms) before fading in handles after the last page movement.
    static const Int32 TEXT_HANDLE_FADE_IN_DELAY = 300;

    // These values are obtained from Samsung.
    // TODO(changwan): refactor SPen related code into a separate class. See
    // http://crbug.com/398169.
    static const Int32 SPEN_ACTION_DOWN = 211;
    static const Int32 SPEN_ACTION_UP = 212;
    static const Int32 SPEN_ACTION_MOVE = 213;
    static const Int32 SPEN_ACTION_CANCEL = 214;
    static Boolean sIsSPenSupported;

    // If the embedder adds a JavaScript interface object that contains an indirect reference to
    // the ContentViewCore, then storing a strong ref to the interface object on the native
    // side would prevent garbage collection of the ContentViewCore (as that strong ref would
    // create a new GC root).
    // For that reason, we store only a weak reference to the interface object on the
    // native side. However we still need a strong reference on the Java side to
    // prevent garbage collection if the embedder doesn't maintain their own ref to the
    // interface object - the Java side ref won't create a new GC root.
    // This map stores those references. We put into the map on addJavaScriptInterface()
    // and remove from it in removeJavaScriptInterface(). The annotation class is stored for
    // the purpose of migrating injected objects from one instance of CVC to another, which
    // is used by Android WebView to support WebChromeClient.onCreateWindow scenario.
//    const Map<String, Pair<Object, Class>> mJavaScriptInterfaces =
//            new HashMap<String, Pair<Object, Class>>();
    AutoPtr<IMap> mJavaScriptInterfaces;

    // Additionally, we keep track of all Java bound JS objects that are in use on the
    // current page to ensure that they are not garbage collected until the page is
    // navigated. This includes interface objects that have been removed
    // via the removeJavaScriptInterface API and transient objects returned from methods
    // on the interface object. Note we use HashSet rather than Set as the native side
    // expects HashSet (no bindings for interfaces).
//    const HashSet<Object> mRetainedJavaScriptObjects = new HashSet<Object>();
    AutoPtr<IHashSet> mRetainedJavaScriptObjects;

    AutoPtr<IContext> mContext;
    AutoPtr<IViewGroup> mContainerView;
    AutoPtr<InternalAccessDelegate> mContainerViewInternals;
    AutoPtr<WebContents> mWebContents;
    AutoPtr<WebContentsObserverElastos> mWebContentsObserver;

    AutoPtr<ContentViewClient> mContentViewClient;

    AutoPtr<ContentSettings> mContentSettings;

    // Native pointer to C++ ContentViewCoreImpl object which will be set by nativeInit().
    Int64 mNativeContentViewCore;

    ObserverList mGestureStateListeners;
    AutoPtr<ObserverList::RewindableIterator> mGestureStateListenersIterator;
    AutoPtr<ZoomControlsDelegate> mZoomControlsDelegate;

    AutoPtr<PopupZoomer> mPopupZoomer;
    AutoPtr<SelectPopup> mSelectPopup;

    AutoPtr<IRunnable> mFakeMouseMoveRunnable;

    // Only valid when focused on a text / password field.
    AutoPtr<ImeAdapter> mImeAdapter;
    AutoPtr<ImeAdapter::AdapterInputConnectionFactory> mAdapterInputConnectionFactory;
    AutoPtr<AdapterInputConnection> mInputConnection;
    AutoPtr<InputMethodManagerWrapper> mInputMethodManagerWrapper;

    AutoPtr<SelectionHandleController> mSelectionHandleController;
    AutoPtr<InsertionHandleController> mInsertionHandleController;

    AutoPtr<IRunnable> mDeferredHandleFadeInRunnable;

    AutoPtr<PositionObserver> mPositionObserver;
    AutoPtr<PositionObserver::Listener> mPositionListener;

    // Size of the viewport in physical pixels as set from onSizeChanged.
    Int32 mViewportWidthPix;
    Int32 mViewportHeightPix;
    Int32 mPhysicalBackingWidthPix;
    Int32 mPhysicalBackingHeightPix;
    Int32 mOverdrawBottomHeightPix;
    Int32 mViewportSizeOffsetWidthPix;
    Int32 mViewportSizeOffsetHeightPix;

    // Cached copy of all positions and scales as reported by the renderer.
    AutoPtr<RenderCoordinates> mRenderCoordinates;

    AutoPtr<RenderCoordinates::NormalizedPoint> mStartHandlePoint;
    AutoPtr<RenderCoordinates::NormalizedPoint> mEndHandlePoint;
    AutoPtr<RenderCoordinates::NormalizedPoint> mInsertionHandlePoint;

    // Tracks whether a selection is currently active.  When applied to selected text, indicates
    // whether the last selected text is still highlighted.
    Boolean mHasSelection;
    String mLastSelectedText;
    Boolean mSelectionEditable;
    AutoPtr<IActionMode> mActionMode;
    Boolean mUnselectAllOnActionModeDismiss;

    // Delegate that will handle GET downloads, and be notified of completion of POST downloads.
    AutoPtr<ContentViewDownloadDelegate> mDownloadDelegate;

    // The AccessibilityInjector that handles loading Accessibility scripts into the web page.
    AutoPtr<AccessibilityInjector> mAccessibilityInjector;

    // Whether native accessibility, i.e. without any script injection, is allowed.
    Boolean mNativeAccessibilityAllowed;

    // Whether native accessibility, i.e. without any script injection, has been enabled.
    Boolean mNativeAccessibilityEnabled;

    // Handles native accessibility, i.e. without any script injection.
    AutoPtr<BrowserAccessibilityManager> mBrowserAccessibilityManager;

    // System accessibility service.
    const AutoPtr<IAccessibilityManager> mAccessibilityManager;

    // Accessibility touch exploration state.
    Boolean mTouchExplorationEnabled;

    // Whether accessibility focus should be set to the page when it finishes loading.
    // This only applies if an accessibility service like TalkBack is running.
    // This is desirable behavior for a browser window, but not for an embedded
    // WebView.
    Boolean mShouldSetAccessibilityFocusOnPageLoad;

    // Allows us to dynamically respond when the accessibility script injection flag changes.
    AutoPtr<IContentObserver> mAccessibilityScriptInjectionObserver;

    // Temporary notification to tell onSizeChanged to focus a form element,
    // because the OSK was just brought up.
    const AutoPtr<IRect> mFocusPreOSKViewportRect;

    // On tap this will store the x, y coordinates of the touch.
    Int32 mLastTapX;
    Int32 mLastTapY;

    // Whether a touch scroll sequence is active, used to hide text selection
    // handles. Note that a scroll sequence will *always* bound a pinch
    // sequence, so this will also be true for the duration of a pinch gesture.
    Boolean mTouchScrollInProgress;

    // The outstanding fling start events that hasn't got fling end yet. It may be > 1 because
    // onNativeFlingStopped() is called asynchronously.
    Int32 mPotentiallyActiveFlingCount;

    AutoPtr<ViewElastos> mViewElastos;

    AutoPtr<SmartClipDataListener> mSmartClipDataListener;

    // This holds the state of editable text (e.g. contents of <input>, contenteditable) of
    // a focused element.
    // Every time the user, IME, javascript (Blink), autofill etc. modifies the content, the new
    //  state must be reflected to this to keep consistency.
    const AutoPtr<IEditable> mEditable;

    // Offsets for the events that passes through this ContentViewCore.
    Float mCurrentTouchOffsetX;
    Float mCurrentTouchOffsetY;

    // Offsets for smart clip
    Int32 mSmartClipOffsetX;
    Int32 mSmartClipOffsetY;

};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTVIEWCORE_H__
