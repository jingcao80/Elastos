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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTS_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/permission/AwPermissionRequest.h"
#include "elastos/droid/webkit/webview/chromium/native/components/InterceptNavigationDelegate.h"
#include "elastos/droid/webkit/webview/chromium/native/components/NavigationParams.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentSettings.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewClient.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewStatics.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/LoadUrlParams.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/NavigationHistory.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/PageTransitionTypes.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/SmartClipProvider.h"
#include "elastos/droid/webkit/webview/chromium/native/content_public/browser/GestureStateListener.h"
#include "elastos/droid/webkit/webview/chromium/native/content/common/CleanupReference.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwViewMethods.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsIoThreadClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebResourceResponse.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwLayoutSizer.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwScrollOffsetManager.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwSettings.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwBrowserContext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwPdfExporter.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwHttpAuthHandler.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwAutofillClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebContentsDelegate.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClientBridge.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwZoomControls.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/OverScrollGlow.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/ScrollAccessibilityHelper.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/DefaultVideoPosterRequestHandler.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentViewClient.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebContentsDelegateAdapter.h"
#include "elastos/droid/os/AsyncTask.h"
#include <Elastos.Droid.Webkit.h>

// import android.annotation.SuppressLint;

using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPicture;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Http::ISslCertificate;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnLayoutChangeListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::InputMethod::IEditorInfo;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::IGeolocationPermissionsCallback;
using Elastos::Droid::Widget::IOverScroller;

using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewStatics;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::PageTransitionTypes;
// import com.google.common.annotations.VisibleForTesting;

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::Permission::AwPermissionRequest;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
using Elastos::Droid::Webkit::Webview::Chromium::Components::InterceptNavigationDelegate;
using Elastos::Droid::Webkit::Webview::Chromium::Components::NavigationParams;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentSettings;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewClient;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::LoadUrlParams;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::NavigationHistory;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::SmartClipProvider;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Common::CleanupReference;
using Elastos::Droid::Webkit::Webview::Chromium::Content_Public::Browser::GestureStateListener;

using Elastos::Core::IRunnable;
using Elastos::IO::IFile;
using Elastos::Net::IURL;
// import java.lang.annotation.Annotation;
// import java.net.MalformedURLException;
// import java.util.HashMap;
// import java.util.Locale;
// import java.util.Map;
using Elastos::Utility::Concurrent::ICallable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {
class FullScreenView;

/**
 * Exposes the native AwContents class, and together these classes wrap the ContentViewCore
 * and Browser components that are required to implement Android WebView API. This is the
 * primary entry point for the WebViewProvider implementation; it holds a 1:1 object
 * relationship with application WebView instances.
 * (We define this class independent of the hidden WebViewProvider interfaces, to allow
 * continuous build & test in the open source SDK-based tree).
 */
//@JNINamespace("android_webview")
class AwContents
    : public Object
    , public SmartClipProvider
{
public:
    /**
     * WebKit hit test related data strcutre. These are used to implement
     * getHitTestResult, requestFocusNodeHref, requestImageRef methods in WebView.
     * All values should be updated together. The native counterpart is
     * AwHitTestData.
     */
    struct HitTestData
        : public Object
    {
        HitTestData()
            : hitTestResultType(0)
        {
        }

        // Used in getHitTestResult.
        Int32 hitTestResultType;
        String hitTestResultExtraData;

        // Used in requestFocusNodeHref (all three) and requestImageRef (only imgSrc).
        String href;
        String anchorText;
        String imgSrc;
    };

    /**
     * Interface that consumers of {@link AwContents} must implement to allow the proper
     * dispatching of view methods through the containing view.
     */
    class InternalAccessDelegate
        : public ContentViewCore::InternalAccessDelegate
    {
    public:
        /**
         * @see View#overScrollBy(int, int, int, int, int, int, int, int, boolean);
         */
        virtual CARAPI_(void) OverScrollBy(
            /* [in] */ Int32 deltaX,
            /* [in] */ Int32 deltaY,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY,
            /* [in] */ Int32 scrollRangeX,
            /* [in] */ Int32 scrollRangeY,
            /* [in] */ Int32 maxOverScrollX,
            /* [in] */ Int32 maxOverScrollY,
            /* [in] */ Boolean isTouchEvent) = 0;

        /**
         * @see View#scrollTo(int, int)
         */
        virtual CARAPI_(void) Super_scrollTo(
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY) = 0;

        /**
         * @see View#setMeasuredDimension(int, int)
         */
        virtual CARAPI_(void) SetMeasuredDimension(
            /* [in] */ Int32 measuredWidth,
            /* [in] */ Int32 measuredHeight) = 0;

        /**
         * @see View#getScrollBarStyle()
         */
        virtual CARAPI_(Int32) Super_getScrollBarStyle() = 0;
    };

    /**
     * Interface that consumers of {@link AwContents} must implement to support
     * native GL rendering.
     */
    class NativeGLDelegate
        : public Object
    {
    public:
        /**
         * Requests a callback on the native DrawGL method (see getAwDrawGLFunction)
         * if called from within onDraw, |canvas| will be non-null and hardware accelerated.
         * Otherwise, |canvas| will be null, and the container view itself will be hardware
         * accelerated. If |waitForCompletion| is true, this method will not return until
         * functor has returned.
         * Should avoid setting |waitForCompletion| when |canvas| is not null.
         * |containerView| is the view where the AwContents should be drawn.
         *
         * @return false indicates the GL draw request was not accepted, and the caller
         *         should fallback to the SW path.
         */
        virtual CARAPI_(Boolean) RequestDrawGL(
            /* [in] */ ICanvas* canvas,
            /* [in] */ Boolean waitForCompletion,
            /* [in] */ IView* containerView) = 0;

        /**
         * Detaches the GLFunctor from the view tree.
         */
        virtual CARAPI_(void) DetachGLFunctor() = 0;
    };

    /**
     * Class to facilitate dependency injection. Subclasses by test code to provide mock versions of
     * certain AwContents dependencies.
     */
    class DependencyFactory
        : public Object
    {
    public:
        virtual CARAPI_(AutoPtr<AwLayoutSizer>) CreateLayoutSizer();

        virtual CARAPI_(AutoPtr<AwScrollOffsetManager>) CreateScrollOffsetManager(
            /* [in] */ AwScrollOffsetManager::Delegate* delegate,
            /* [in] */ IOverScroller* overScroller);
    };

public:
    class DestroyRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        //@Override
        CARAPI Run();

    public:
        DestroyRunnable(
            /* [in] */ Handle64 nativeAwContents);

        const Handle64 mNativeAwContents;
    };

    /**
     * A class that stores the state needed to enter and exit fullscreen.
     */
    class FullScreenTransitionsState
        : public Object
    {
    public:
        FullScreenTransitionsState(
            /* [in] */ IViewGroup* initialContainerView,
            /* [in] */ InternalAccessDelegate* initialInternalAccessAdapter,
            /* [in] */ AwViewMethods* initialAwViewMethods);

        CARAPI_(void) EnterFullScreen(
            /* [in] */ FullScreenView* fullScreenView);

        CARAPI_(void) ExitFullScreen();

        CARAPI_(Boolean) IsFullScreen();

        CARAPI_(AutoPtr<IViewGroup>) GetInitialContainerView();

        CARAPI_(AutoPtr<AwContents::InternalAccessDelegate>) GetInitialInternalAccessDelegate();

        CARAPI_(AutoPtr<AwViewMethods>) GetInitialAwViewMethods();

        CARAPI_(AutoPtr<FullScreenView>) GetFullScreenView();

    private:
        AutoPtr<IViewGroup> mInitialContainerView;
        AutoPtr<InternalAccessDelegate> mInitialInternalAccessAdapter;
        AutoPtr<AwViewMethods> mInitialAwViewMethods;
        AutoPtr<FullScreenView> mFullScreenView;
    };

    //--------------------------------------------------------------------------------------------
    class IoThreadClientImpl
        : public AwContentsIoThreadClient
    {
    public:
        IoThreadClientImpl(
            /* [in] */ AwContents* owner);

        // All methods are called on the IO thread.

        //@Override
        CARAPI_(Int32) GetCacheMode();

        //@Override
        CARAPI_(AutoPtr<AwWebResourceResponse>) ShouldInterceptRequest(
            /* [in] */ AwContentsClient::ShouldInterceptRequestParams* params);

        //@Override
        CARAPI_(Boolean) ShouldBlockContentUrls();

        //@Override
        CARAPI_(Boolean) ShouldBlockFileUrls();

        //@Override
        CARAPI_(Boolean) ShouldBlockNetworkLoads();

        //@Override
        CARAPI_(Boolean) ShouldAcceptThirdPartyCookies();

        //@Override
        CARAPI_(void) OnDownloadStart(
            /* [in] */ const String& url,
            /* [in] */ const String& userAgent,
            /* [in] */ const String& contentDisposition,
            /* [in] */ const String& mimeType,
            /* [in] */ Int64 contentLength);

        //@Override
        CARAPI_(void) NewLoginRequest(
            /* [in] */ const String& realm,
            /* [in] */ const String& account,
            /* [in] */ const String& args);

    private:
        AwContents* mOwner;
    };

    //--------------------------------------------------------------------------------------------
    // When the navigation is for a newly created WebView (i.e. a popup), intercept the navigation
    // here for implementing shouldOverrideUrlLoading. This is to send the shouldOverrideUrlLoading
    // callback to the correct WebViewClient that is associated with the WebView.
    // Otherwise, use this delegate only to post onPageStarted messages.
    //
    // We are not using WebContentsObserver.didStartLoading because of stale URLs, out of order
    // onPageStarted's and double onPageStarted's.
    //
    class InterceptNavigationDelegateImpl
        : public InterceptNavigationDelegate
    {
    public:
        InterceptNavigationDelegateImpl(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI_(Boolean) ShouldIgnoreNavigation(
            /* [in] */ NavigationParams* navigationParams);

    private:
        AwContents* mOwner;
    };

    //--------------------------------------------------------------------------------------------
    class AwLayoutSizerDelegate
        : public AwLayoutSizer::Delegate
    {
    public:
        AwLayoutSizerDelegate(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI_(void) RequestLayout();

        //@Override
        CARAPI_(void) SetMeasuredDimension(
            /* [in] */ Int32 measuredWidth,
            /* [in] */ Int32 measuredHeight);

        //@Override
        CARAPI_(Boolean) IsLayoutParamsHeightWrapContent();

        //@Override
        CARAPI_(void) SetForceZeroLayoutHeight(
            /* [in] */ Boolean forceZeroHeight);

    private:
        AwContents* mOwner;
    };

    //--------------------------------------------------------------------------------------------
    class AwScrollOffsetManagerDelegate
        : public AwScrollOffsetManager::Delegate
    {
    public:
        AwScrollOffsetManagerDelegate(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI_(void) OverScrollContainerViewBy(
            /* [in] */ Int32 deltaX,
            /* [in] */ Int32 deltaY,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY,
            /* [in] */ Int32 scrollRangeX,
            /* [in] */ Int32 scrollRangeY,
            /* [in] */ Boolean isTouchEvent);

        //@Override
        CARAPI_(void) ScrollContainerViewTo(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        //@Override
        CARAPI_(void) ScrollNativeTo(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        //@Override
        CARAPI_(Int32) GetContainerViewScrollX();

        //@Override
        CARAPI_(Int32) GetContainerViewScrollY();

        //@Override
        CARAPI_(void) Invalidate();

    private:
        AwContents* mOwner;
    };

    //--------------------------------------------------------------------------------------------
    class AwGestureStateListener
        : public GestureStateListener
    {
    public:
        AwGestureStateListener(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI_(void) OnPinchStarted();

        //@Override
        CARAPI_(void) OnPinchEnded();

        //@Override
        CARAPI_(void) OnFlingCancelGesture();

        //@Override
        CARAPI_(void) OnUnhandledFlingStartEvent(
            /* [in] */ Int32 velocityX,
            /* [in] */ Int32 velocityY);

        //@Override
        CARAPI_(void) OnScrollUpdateGestureConsumed();

    private:
        AwContents* mOwner;
    };

    //--------------------------------------------------------------------------------------------
    class AwComponentCallbacks
        : public Object
        , public IComponentCallbacks2
    {
    public:
        CAR_INTERFACE_DECL()
        AwComponentCallbacks(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI OnTrimMemory(
            /* [in] */ Int32 level);

        //@Override
        CARAPI_(void) OnLowMemory();

        //@Override
        CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* configuration);

    private:
        AwContents* mOwner;
    };

    //--------------------------------------------------------------------------------------------
    class AwLayoutChangeListener
        : public Object
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL()
        AwLayoutChangeListener(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI OnLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);

    private:
        AwContents* mOwner;
    };

    class AwGeolocationCallback
        : public Object
        , public IGeolocationPermissionsCallback
    {
    public:
        class InnerRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()
            InnerRunnable(
                /* [in] */ AwGeolocationCallback* owner,
                /* [in] */ const String& origin,
                /* [in] */ Boolean allow,
                /* [in] */ Boolean retain);

            CARAPI Run();

        private:
            AwGeolocationCallback* mOwner;
            const String& mOrigin;
            Boolean mAllow;
            Boolean mRetain;
        };

    public:
        CAR_INTERFACE_DECL()

        AwGeolocationCallback(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI Invoke(
            /* [in] */ const String& origin,
            /* [in] */ Boolean allow,
            /* [in] */ Boolean retain);

    private:
        AwContents* mOwner;
    };

    // --------------------------------------------------------------------------------------------
    // This is the AwViewMethods implementation that does real work. The AwViewMethodsImpl is
    // hooked up to the WebView in embedded mode and to the FullScreenView in fullscreen mode,
    // but not to both at the same time.
    class AwViewMethodsImpl
        : public AwViewMethods
    {
    public:
        AwViewMethodsImpl(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI_(void) OnDraw(
            /* [in] */ ICanvas* canvas);

        //@Override
        CARAPI OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        //@Override
        CARAPI_(void) RequestFocus();

        //@Override
        CARAPI_(void) SetLayerType(
            /* [in] */ Int32 layerType,
            /* [in] */ IPaint* paint);

        //@Override
        CARAPI_(AutoPtr<IInputConnection>) OnCreateInputConnection(
            /* [in] */ IEditorInfo* outAttrs);

        //@Override
        CARAPI_(Boolean) OnKeyUp(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event);

        //@Override
        CARAPI_(Boolean) DispatchKeyEvent(
            /* [in] */ IKeyEvent* event);

        //@Override
        CARAPI_(Boolean) OnTouchEvent(
            /* [in] */ IMotionEvent* event);

        //@Override
        CARAPI_(Boolean) OnHoverEvent(
            /* [in] */ IMotionEvent* event);

        //@Override
        CARAPI_(Boolean) OnGenericMotionEvent(
            /* [in] */ IMotionEvent* event);

        //@Override
        CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* newConfig);

        //@Override
        CARAPI_(void) OnAttachedToWindow();

        //@Override
        CARAPI_(void) OnDetachedFromWindow();

        //@Override
        CARAPI_(void) OnWindowFocusChanged(
            /* [in] */ Boolean hasWindowFocus);

        //@Override
        CARAPI_(void) OnFocusChanged(
            /* [in] */ Boolean focused,
            /* [in] */ Int32 direction,
            /* [in] */ IRect* previouslyFocusedRect);

        //@Override
        CARAPI OnSizeChanged(
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ Int32 ow,
            /* [in] */ Int32 oh);

        //@Override
        CARAPI OnVisibilityChanged(
            /* [in] */ IView* changedView,
            /* [in] */ Int32 visibility);

        //@Override
        CARAPI OnWindowVisibilityChanged(
            /* [in] */ Int32 visibility);

    private:
        CARAPI_(void) UpdateHardwareAcceleratedFeaturesToggle();

        CARAPI_(Boolean) IsDpadEvent(
            /* [in] */ IKeyEvent* event);

    private:
        AwContents* mOwner;
        // Only valid within software onDraw().
        AutoPtr<IRect> mClipBoundsTemporary;
        Int32 mLayerType;
        AutoPtr<IComponentCallbacks2> mComponentCallbacks;
    };

    class InnerZoomSupportChangeListener
        : public AwSettings::ZoomSupportChangeListener
    {
    public:
        InnerZoomSupportChangeListener(
            /* [in] */ AwContents* owner);

        //@Override
        CARAPI_(void) OnGestureZoomSupportChanged(
            /* [in] */ Boolean supportsDoubleTapZoom,
            /* [in] */ Boolean supportsMultiTouchZoom);

    private:
        AwContents* mOwner;
    };

    class InnerCallable
        : public Object
        , public ICallable
    {
    public:
        CAR_INTERFACE_DECL()
        InnerCallable(
            /* [in] */ AwContents* owner);

        CARAPI Call(
            /* [out] */ IInterface** pic);//IPicture

    private:
        AwContents* mOwner;
    };

    class InnerValueCallback
        : public Object
        , public IValueCallback
    {
    private:
        class InnerRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()
            InnerRunnable(
                /* [in] */ InnerValueCallback* owner,
                /* [in] */ ArrayOf<String>* value);

            CARAPI Run();

        private:
            InnerValueCallback* mOwner;
            AutoPtr<ArrayOf<String> > mValue;
        };

    public:
        CAR_INTERFACE_DECL()

        InnerValueCallback(
            /* [in] */ AwContents* owner);

        CARAPI OnReceiveValue(
            /* [in] */ IInterface* value);//ArrayOf<String>

    private:
        AwContents* mOwner;
    };

    class InnerJavaScriptCallback
        : public ContentViewCore::JavaScriptCallback
    {
    public:
        InnerJavaScriptCallback(
            /* [in] */ AwContents* owner,
            /* [in] */ IValueCallback* callback);

        CARAPI_(void) HandleJavaScriptResult(
            /* [in] */ const String& jsonResult);

    private:
        AwContents* mOwner;
        IValueCallback* mCallback;
    };

    class SaveWebArchiveInternalTask
        : public AsyncTask
    {
        public:
            SaveWebArchiveInternalTask(
                    /* [in] */ AwContents* owner,
                    /* [in] */ const String& basename,
                    /* [in] */ IValueCallback* callback);

        protected:
            //@Override
            CARAPI DoInBackground(
                    /* [in] */ ArrayOf<IInterface*>* params,
                    /* [out] */ IInterface** result);

            //@Override
            CARAPI OnPostExecute(
                    /* [in] */ IInterface* result);

        private:
            AwContents* mOwner;
            const String mBasename;
            AutoPtr<IValueCallback> mCallback;
    };

    class SaveWebArchiveInternalRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        SaveWebArchiveInternalRunnable(
            /* [in] */ AwContents* owner,
            /* [in] */ IValueCallback* callback);

        CARAPI Run();

    private:
        AwContents* mOwner;
        IValueCallback* mCallback;
    };

    class InnerSmartClipDataListener
        : public ContentViewCore::SmartClipDataListener
    {
    public:
        InnerSmartClipDataListener(
            /* [in] */ AwContents* owner,
            /* [in] */ IHandler* resultHandler);

        CARAPI_(void) OnSmartClipDataExtracted(
            /* [in] */ const String& text,
            /* [in] */ const String& html,
            /* [in] */ IRect* clipRect);

    private:
        AwContents* mOwner;
        AutoPtr<IHandler> mResultHandler;
    };

public:
    /**
     * Key for opaque state in bundle. Note this is only public for tests.
     */
    static const String SAVE_RESTORE_STATE_KEY;

public:
    CAR_INTERFACE_DECL()

    /**
     * @param browserContext the browsing context to associate this view contents with.
     * @param containerView the view-hierarchy item this object will be bound to.
     * @param context the context to use, usually containerView.getContext().
     * @param internalAccessAdapter to access private methods on containerView.
     * @param nativeGLDelegate to access the GL functor provided by the WebView.
     * @param contentsClient will receive API callbacks from this WebView Contents.
     * @param awSettings AwSettings instance used to configure the AwContents.
     *
     * This constructor uses the default view sizing policy.
     */
    AwContents(
        /* [in] */ AwBrowserContext* browserContext,
        /* [in] */ IViewGroup* containerView,
        /* [in] */ IContext* context,
        /* [in] */ InternalAccessDelegate* internalAccessAdapter,
        /* [in] */ NativeGLDelegate* nativeGLDelegate,
        /* [in] */ AwContentsClient* contentsClient,
        /* [in] */ AwSettings* awSettings);

    /**
     * @param dependencyFactory an instance of the DependencyFactory used to provide instances of
     *                          classes that this class depends on.
     *
     * This version of the constructor is used in test code to inject test versions of the above
     * documented classes.
     */
    AwContents(
        /* [in] */ AwBrowserContext* browserContext,
        /* [in] */ IViewGroup* containerView,
        /* [in] */ IContext* context,
        /* [in] */ InternalAccessDelegate* internalAccessAdapter,
        /* [in] */ NativeGLDelegate* nativeGLDelegate,
        /* [in] */ AwContentsClient* contentsClient,
        /* [in] */ AwSettings* settings,
        /* [in] */ DependencyFactory* dependencyFactory);

    virtual CARAPI_(Boolean) IsFullScreen();

    /**
     * Transitions this {@link AwContents} to fullscreen mode and returns the
     * {@link View} where the contents will be drawn while in fullscreen.
     */
    virtual CARAPI_(AutoPtr<IView>) EnterFullScreen();

    /**
     * Returns this {@link AwContents} to embedded mode, where the {@link AwContents} are drawn
     * in the WebView.
     */
    virtual CARAPI_(void) ExitFullScreen();

    /**
     * Called on the "source" AwContents that is opening the popup window to
     * provide the AwContents to host the pop up content.
     */
    virtual CARAPI_(void) SupplyContentsForPopup(
        /* [in] */ AwContents* newContents);

    /**
     * Deletes the native counterpart of this object.
     */
    virtual CARAPI_(void) Destroy();

    //@VisibleForTesting
    virtual CARAPI_(AutoPtr<ContentViewCore>) GetContentViewCore();

    // Can be called from any thread.
    virtual CARAPI_(AutoPtr<AwSettings>) GetSettings();

    virtual CARAPI_(AutoPtr<AwPdfExporter>) GetPdfExporter();

    static CARAPI_(void) SetAwDrawSWFunctionTable(
        /* [in] */ Handle64 functionTablePointer);

    static CARAPI_(void) SetAwDrawGLFunctionTable(
        /* [in] */ Handle64 functionTablePointer);

    static CARAPI_(Handle64) GetAwDrawGLFunction();

    static CARAPI_(void) SetShouldDownloadFavicons();

    /**
     * Disables contents of JS-to-Java bridge objects to be inspectable using
     * Object.keys() method and "for .. in" loops. This is intended for applications
     * targeting earlier Android releases where this was not possible, and we want
     * to ensure backwards compatible behavior.
     */
    virtual CARAPI_(void) DisableJavascriptInterfacesInspection();

    /**
     * Intended for test code.
     * @return the number of native instances of this class.
     */
    //@VisibleForTesting
    static CARAPI_(Int32) GetNativeInstanceCount();

    virtual CARAPI_(Handle64) GetAwDrawGLViewContext();

    //--------------------------------------------------------------------------------------------
    //  WebView[Provider] method implementations (where not provided by ContentViewCore)
    //--------------------------------------------------------------------------------------------

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI_(Int32) GetContentHeightCss();

    virtual CARAPI_(Int32) GetContentWidthCss();

    virtual CARAPI_(AutoPtr<IPicture>) CapturePicture();

    virtual CARAPI_(void) ClearView();

    /**
     * Enable the onNewPicture callback.
     * @param enabled Flag to enable the callback.
     * @param invalidationOnly Flag to call back only on invalidation without providing a picture.
     */
    virtual CARAPI_(void) EnableOnNewPicture(
        /* [in] */ Boolean enabled,
        /* [in] */ Boolean invalidationOnly);

    virtual CARAPI_(void) FindAllAsync(
        /* [in] */ const String& searchString);

    virtual CARAPI_(void) FindNext(
        /* [in] */ Boolean forward);

    virtual CARAPI_(void) ClearMatches();

    /**
     * @return load progress of the WebContents.
     */
    virtual CARAPI_(Int32) GetMostRecentProgress();

    virtual CARAPI_(AutoPtr<IBitmap>) GetFavicon();

    /**
     * Load url without fixing up the url string. Consumers of ContentView are responsible for
     * ensuring the URL passed in is properly formatted (i.e. the scheme has been added if left
     * off during user input).
     *
     * @param params Parameters for this load.
     */
    virtual CARAPI_(void) LoadUrl(
        /* [in] */ LoadUrlParams* params);

    /**
     * Get the URL of the current page.
     *
     * @return The URL of the current page or null if it's empty.
     */
    virtual CARAPI_(String) GetUrl();

    virtual CARAPI_(void) RequestFocus();

    virtual CARAPI_(void) SetBackgroundColor(
        /* [in] */ Int32 color);

    /**
     * @see android.view.View#setLayerType()
     */
    virtual CARAPI_(void) SetLayerType(
        /* [in] */ Int32 layerType,
        /* [in] */ IPaint* paint);

    virtual CARAPI_(Int32) GetEffectiveBackgroundColor();

    virtual CARAPI_(Boolean) IsMultiTouchZoomSupported();

    virtual CARAPI_(AutoPtr<IView>) GetZoomControlsForTest();

    /**
     * @see ContentViewCore#getContentSettings()
     */
    virtual CARAPI_(AutoPtr<ContentSettings>) GetContentSettings();

    /**
     * @see View#setOverScrollMode(int)
     */
    virtual CARAPI_(void) SetOverScrollMode(
        /* [in] */ Int32 mode);

    /**
     * @see View#setScrollBarStyle(int)
     */
    virtual CARAPI_(void) SetScrollBarStyle(
        /* [in] */ Int32 style);

    /**
     * @see View#setHorizontalScrollbarOverlay(boolean)
     */
    virtual CARAPI_(void) SetHorizontalScrollbarOverlay(
        /* [in] */ Boolean overlay);

    /**
     * @see View#setVerticalScrollbarOverlay(boolean)
     */
    virtual CARAPI_(void) SetVerticalScrollbarOverlay(
        /* [in] */ Boolean overlay);

    /**
     * @see View#overlayHorizontalScrollbar()
     */
    virtual CARAPI_(Boolean) OverlayHorizontalScrollbar();

    /**
     * @see View#overlayVerticalScrollbar()
     */
    virtual CARAPI_(Boolean) OverlayVerticalScrollbar();

    /**
     * Called by the embedder when the scroll offset of the containing view has changed.
     * @see View#onScrollChanged(int,int)
     */
    virtual CARAPI_(void) OnContainerViewScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

    /**
     * Called by the embedder when the containing view is to be scrolled or overscrolled.
     * @see View#onOverScrolled(int,int,int,int)
     */
    virtual CARAPI_(void) OnContainerViewOverScrolled(
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Boolean clampedX,
        /* [in] */ Boolean clampedY);

    /**
     * @see android.webkit.WebView#requestChildRectangleOnScreen(View, Rect, boolean)
     */
    virtual CARAPI_(Boolean) RequestChildRectangleOnScreen(
        /* [in] */ IView* child,
        /* [in] */ IRect* rect,
        /* [in] */ Boolean immediate);

    /**
     * @see View.computeScroll()
     */
    virtual CARAPI_(void) ComputeScroll();

    /**
     * @see View#computeHorizontalScrollRange()
     */
    virtual CARAPI_(Int32) ComputeHorizontalScrollRange();

    /**
     * @see View#computeHorizontalScrollOffset()
     */
    virtual CARAPI_(Int32) ComputeHorizontalScrollOffset();

    /**
     * @see View#computeVerticalScrollRange()
     */
    virtual CARAPI_(Int32) ComputeVerticalScrollRange();

    /**
     * @see View#computeVerticalScrollOffset()
     */
    virtual CARAPI_(Int32) ComputeVerticalScrollOffset();

    /**
     * @see View#computeVerticalScrollExtent()
     */
    virtual CARAPI_(Int32) ComputeVerticalScrollExtent();

    /**
     * @see android.webkit.WebView#stopLoading()
     */
    virtual CARAPI_(void) StopLoading();

    /**
     * @see android.webkit.WebView#reload()
     */
    virtual CARAPI_(void) Reload();

    /**
     * @see android.webkit.WebView#canGoBack()
     */
    virtual CARAPI_(Boolean) CanGoBack();

    /**
     * @see android.webkit.WebView#goBack()
     */
    virtual CARAPI_(void) GoBack();

    /**
     * @see android.webkit.WebView#canGoForward()
     */
    virtual CARAPI_(Boolean) CanGoForward();

    /**
     * @see android.webkit.WebView#goForward()
     */
    virtual CARAPI_(void) GoForward();

    /**
     * @see android.webkit.WebView#canGoBackOrForward(int)
     */
    virtual CARAPI_(Boolean) CanGoBackOrForward(int steps);

    /**
     * @see android.webkit.WebView#goBackOrForward(int)
     */
    virtual CARAPI_(void) GoBackOrForward(int steps);

    /**
     * @see android.webkit.WebView#pauseTimers()
     */
    virtual CARAPI_(void) PauseTimers();

    /**
     * @see android.webkit.WebView#resumeTimers()
     */
    virtual CARAPI_(void) ResumeTimers();

    /**
     * @see android.webkit.WebView#onPause()
     */
    virtual CARAPI_(void) OnPause();

    /**
     * @see android.webkit.WebView#onResume()
     */
    virtual CARAPI_(void) OnResume();

    /**
     * @see android.webkit.WebView#isPaused()
     */
    virtual CARAPI_(Boolean) IsPaused();

    /**
     * @see android.webkit.WebView#onCreateInputConnection(EditorInfo)
     */
    virtual CARAPI_(AutoPtr<IInputConnection>) OnCreateInputConnection(
        /* [in] */ IEditorInfo* outAttrs);

    /**
     * @see android.webkit.WebView#onKeyUp(int, KeyEvent)
     */
    virtual CARAPI_(Boolean) OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    /**
     * @see android.webkit.WebView#dispatchKeyEvent(KeyEvent)
     */
    virtual CARAPI_(Boolean) DispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    /**
     * Clears the resource cache. Note that the cache is per-application, so this will clear the
     * cache for all WebViews used.
     *
     * @param includeDiskFiles if false, only the RAM cache is cleared
     */
    virtual CARAPI_(void) ClearCache(
        /* [in] */ Boolean includeDiskFiles);

    virtual CARAPI_(void) DocumentHasImages(
        /* [in] */ IMessage* message);

    virtual CARAPI_(void) SaveWebArchive(
        /* [in] */ const String& basename,
        /* [in] */ Boolean autoname,
        /* [in] */ IValueCallback* callback);

    virtual CARAPI_(String) GetOriginalUrl();

    /**
     * @see ContentViewCore#getNavigationHistory()
     */
    virtual CARAPI_(AutoPtr<NavigationHistory>) GetNavigationHistory();

    /**
     * @see android.webkit.WebView#getTitle()
     */
    virtual CARAPI_(String) GetTitle();

    /**
     * @see android.webkit.WebView#clearHistory()
     */
    virtual CARAPI_(void) ClearHistory();

    virtual CARAPI_(AutoPtr< ArrayOf<String> >) GetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm);

    virtual CARAPI_(void) SetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    /**
     * @see android.webkit.WebView#getCertificate()
     */
    virtual CARAPI_(AutoPtr<ISslCertificate>) GetCertificate();

    /**
     * @see android.webkit.WebView#clearSslPreferences()
     */
    virtual CARAPI_(void) ClearSslPreferences();

    // TODO(sgurun) remove after this rolls in. To keep internal tree happy.
    virtual CARAPI_(void) ClearClientCertPreferences();

    /**
     * Method to return all hit test values relevant to public WebView API.
     * Note that this expose more data than needed for WebView.getHitTestResult.
     * Unsafely returning reference to mutable internal object to avoid excessive
     * garbage allocation on repeated calls.
     */
    virtual CARAPI_(AutoPtr<HitTestData>) GetLastHitTestResult();

    /**
     * @see android.webkit.WebView#requestFocusNodeHref()
     */
    virtual CARAPI_(void) RequestFocusNodeHref(
        /* [in] */ IMessage* msg);

    /**
     * @see android.webkit.WebView#requestImageRef()
     */
    virtual CARAPI_(void) RequestImageRef(
        /* [in] */ IMessage* msg);

    //@VisibleForTesting
    virtual CARAPI_(Float) GetPageScaleFactor();

    /**
     * @see android.webkit.WebView#getScale()
     *
     * Please note that the scale returned is the page scale multiplied by
     * the screen density factor. See CTS WebViewTest.testSetInitialScale.
     */
    virtual CARAPI_(Float) GetScale();

    /**
     * @see android.webkit.WebView#flingScroll(int, int)
     */
    virtual CARAPI_(void) FlingScroll(
        /* [in] */ Int32 velocityX,
        /* [in] */ Int32 velocityY);

    /**
     * @see android.webkit.WebView#pageUp(boolean)
     */
    virtual CARAPI_(Boolean) PageUp(
        /* [in] */ Boolean top);

    /**
     * @see android.webkit.WebView#pageDown(boolean)
     */
    virtual CARAPI_(Boolean) PageDown(
        /* [in] */ Boolean bottom);

    /**
     * @see android.webkit.WebView#canZoomIn()
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    virtual CARAPI_(Boolean) CanZoomIn();

    /**
     * @see android.webkit.WebView#canZoomOut()
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    virtual CARAPI_(Boolean) CanZoomOut();

    /**
     * @see android.webkit.WebView#zoomIn()
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    virtual CARAPI_(Boolean) ZoomIn();

    /**
     * @see android.webkit.WebView#zoomOut()
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    virtual CARAPI_(Boolean) ZoomOut();

    /**
     * @see android.webkit.WebView#zoomBy()
     */
    // This method uses the term 'zoom' for legacy reasons, but relates
    // to what chrome calls the 'page scale factor'.
    virtual CARAPI_(Boolean) ZoomBy(
        /* [in] */ Float delta);

    /**
     * @see android.webkit.WebView#invokeZoomPicker()
     */
    virtual CARAPI_(void) InvokeZoomPicker();

    /**
     * @see android.webkit.WebView#preauthorizePermission(Uri, long)
     */
    virtual CARAPI_(void) PreauthorizePermission(
        /* [in] */ IUri* origin,
        /* [in] */ Int64 resources);

    /**
     * @see ContentViewCore.evaluateJavaScript(String, ContentViewCore.JavaScriptCallback)
     */
    virtual CARAPI_(void) EvaluateJavaScript(
        /* [in] */ const String& script,
        /* [in] */ IValueCallback* callback);

    /**
     * @see ContentViewCore.evaluateJavaScriptEvenIfNotYetNavigated(String)
     */
    virtual CARAPI_(void) EvaluateJavaScriptEvenIfNotYetNavigated(
        /* [in] */ const String& script);

    //--------------------------------------------------------------------------------------------
    //  View and ViewGroup method implementations
    //--------------------------------------------------------------------------------------------

    /**
     * @see android.webkit.View#onTouchEvent()
     */
    virtual CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * @see android.view.View#onHoverEvent()
     */
    virtual CARAPI_(Boolean) OnHoverEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * @see android.view.View#onGenericMotionEvent()
     */
    virtual CARAPI_(Boolean) OnGenericMotionEvent(
        /* [in] */ IMotionEvent* event);

    /**
     * @see android.view.View#onConfigurationChanged()
     */
    virtual CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /**
     * @see android.view.View#onAttachedToWindow()
     */
    virtual CARAPI_(void) OnAttachedToWindow();

    /**
     * @see android.view.View#onDetachedFromWindow()
     */
    //@SuppressLint("MissingSuperCall")
    virtual CARAPI_(void) OnDetachedFromWindow();

    /**
     * @see android.view.View#onWindowFocusChanged()
     */
    virtual CARAPI_(void) OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    /**
     * @see android.view.View#onFocusChanged()
     */
    virtual CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean focused,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    /**
     * @see android.view.View#onStartTemporaryDetach()
     */
    virtual CARAPI_(void) OnStartTemporaryDetach();

    /**
     * @see android.view.View#onFinishTemporaryDetach()
     */
    virtual CARAPI_(void) OnFinishTemporaryDetach();

    /**
     * @see android.view.View#onSizeChanged()
     */
    virtual CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 ow,
        /* [in] */ Int32 oh);

    /**
     * @see android.view.View#onVisibilityChanged()
     */
    virtual CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    /**
     * @see android.view.View#onWindowVisibilityChanged()
     */
    virtual CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);

    /**
     * Save the state of this AwContents into provided Bundle.
     * @return False if saving state failed.
     */
    virtual CARAPI_(Boolean) SaveState(
        /* [in] */ IBundle* outState);

    /**
     * Restore the state of this AwContents into provided Bundle.
     * @param inState Must be a bundle returned by saveState.
     * @return False if restoring state failed.
     */
    virtual CARAPI_(Boolean) RestoreState(
        /* [in] */ IBundle* inState);

    /**
     * @see ContentViewCore#addPossiblyUnsafeJavascriptInterface(Object, String, Class)
     */
    virtual CARAPI_(void) AddPossiblyUnsafeJavascriptInterface(
        /* [in] */ Object* object,
        /* [in] */ const String& name,
        /* [in] */ IInterface* requiredAnnotation);

    /**
     * @see android.webkit.WebView#removeJavascriptInterface(String)
     */
    virtual CARAPI_(void) RemoveJavascriptInterface(
        /* [in] */ const String& interfaceName);

    /**
     * If native accessibility (not script injection) is enabled, and if this is
     * running on JellyBean or later, returns an AccessibilityNodeProvider that
     * implements native accessibility for this view. Returns null otherwise.
     * @return The AccessibilityNodeProvider, if available, or null otherwise.
     */
    virtual CARAPI_(AutoPtr<IAccessibilityNodeProvider>) GetAccessibilityNodeProvider();

    /**
     * @see android.webkit.WebView#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
     */
    virtual CARAPI_(void) OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * @see android.webkit.WebView#onInitializeAccessibilityEvent(AccessibilityEvent)
     */
    virtual CARAPI_(void) OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI_(Boolean) SupportsAccessibilityAction(
        /* [in] */ Int32 action);

    /**
     * @see android.webkit.WebView#performAccessibilityAction(int, Bundle)
     */
    virtual CARAPI_(Boolean) PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

    /**
     * @see android.webkit.WebView#clearFormData()
     */
    virtual CARAPI_(void) HideAutofillPopup();

    virtual CARAPI_(void) SetNetworkAvailable(
        /* [in] */ Boolean networkUp);

    //@CalledByNative
    virtual CARAPI_(void) OnFindResultReceived(
        /* [in] */ Int32 activeMatchOrdinal,
        /* [in] */ Int32 numberOfMatches,
        /* [in] */ Boolean isDoneCounting);

    //@CalledByNative
    virtual CARAPI_(void) OnNewPicture();

    //@Override
    virtual CARAPI ExtractSmartClipData(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    //@Override
    virtual CARAPI SetSmartClipResultHandler(
        /* [in] */ IHandler* resultHandler);

private:
    static CARAPI_(AutoPtr<ContentViewCore>) CreateAndInitializeContentViewCore(
        /* [in] */ IViewGroup* containerView,
        /* [in] */ IContext* context,
        /* [in] */ InternalAccessDelegate* internalDispatcher,
        /* [in] */ Handle64 nativeWebContents,
        /* [in] */ GestureStateListener* gestureStateListener,
        /* [in] */ ContentViewClient* contentViewClient,
        /* [in] */ ContentViewCore::ZoomControlsDelegate* zoomControlsDelegate);

    CARAPI_(void) SetInternalAccessAdapter(
        /* [in] */ InternalAccessDelegate* internalAccessAdapter);

    CARAPI_(void) SetContainerView(
        /* [in] */ IViewGroup* newContainerView);

    /**
     * Reconciles the state of this AwContents object with the state of the new container view.
     */
    CARAPI_(void) OnContainerViewChanged();

    /* Common initialization routine for adopting a native AwContents instance into this
     * java instance.
     *
     * TAKE CARE! This method can get called multiple times per java instance. Code accordingly.
     * ^^^^^^^^^  See the native class declaration for more details on relative object lifetimes.
     */
    CARAPI_(void) SetNewAwContents(
        /* [in] */ Handle64 newAwContentsPtr);


    // Recap: supplyContentsForPopup() is called on the parent window's content, this method is
    // called on the popup window's content.
    CARAPI_(void) ReceivePopupContents(
        /* [in] */ Handle64 popupNativeAwContents);

    CARAPI_(AutoPtr<IRect>) GetGlobalVisibleRect();

    CARAPI_(void) RequestVisitedHistoryFromClient();

    CARAPI_(void) SetViewVisibilityInternal(
        /* [in] */ Boolean visible);

    CARAPI_(void) SetWindowVisibilityInternal(
        /* [in] */ Boolean visible);

    //--------------------------------------------------------------------------------------------
    //  Methods called from native via JNI
    //--------------------------------------------------------------------------------------------

    //@CalledByNative
    static CARAPI_(void) OnDocumentHasImagesResponse(
        /* [in] */ Boolean result,
        /* [in] */ IMessage* message);

    //@CalledByNative
    CARAPI_(void) OnReceivedTouchIconUrl(
        /* [in] */ const String& url,
        /* [in] */ Boolean precomposed);

    //@CalledByNative
    CARAPI_(void) OnReceivedIcon(
        /* [in] */ IBitmap* bitmap);

    /** Callback for generateMHTML. */
    //@CalledByNative
    static CARAPI_(void) GenerateMHTMLCallback(
        /* [in] */ const String& path,
        /* [in] */ Int64 size,
        /* [in] */ IInterface* callback);

    //@CalledByNative
    CARAPI_(void) OnReceivedHttpAuthRequest(
        /* [in] */ AwHttpAuthHandler* handler,
        /* [in] */ const String& host,
        /* [in] */ const String& realm);

    //@CalledByNative
    CARAPI_(void) OnGeolocationPermissionsShowPrompt(
        /* [in] */ const String& origin);

    //@CalledByNative
    CARAPI_(void) OnGeolocationPermissionsHidePrompt();

    //@CalledByNative
    CARAPI_(void) OnPermissionRequest(
        /* [in] */ AwPermissionRequest* awPermissionRequest);

    //@CalledByNative
    CARAPI_(void) OnPermissionRequestCanceled(
        /* [in] */ AwPermissionRequest* awPermissionRequest);

    // Called as a result of nativeUpdateLastHitTestData.
    //@CalledByNative
    CARAPI_(void) UpdateHitTestData(
        /* [in] */ Int32 type,
        /* [in] */ const String& extra,
        /* [in] */ const String& href,
        /* [in] */ const String& anchorText,
        /* [in] */ const String& imgSrc);

    //@CalledByNative
    CARAPI_(Boolean) RequestDrawGL(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Boolean waitForCompletion);

    //@CalledByNative
    CARAPI_(void) PostInvalidateOnAnimation();

    // Call postInvalidateOnAnimation for invalidations. This is only used to synchronize
    // draw functor destruction.
    //@CalledByNative
    CARAPI_(void) InvalidateOnFunctorDestroy();

    //@CalledByNative
    CARAPI_(AutoPtr< ArrayOf<Int32> >) GetLocationOnScreen();

    //@CalledByNative
    CARAPI_(void) OnWebLayoutPageScaleFactorChanged(
        /* [in] */ Float webLayoutPageScaleFactor);

    //@CalledByNative
    CARAPI_(void) OnWebLayoutContentsSizeChanged(
        /* [in] */ Int32 widthCss,
        /* [in] */ Int32 heightCss);

    //@CalledByNative
    CARAPI_(void) ScrollContainerViewTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@CalledByNative
    CARAPI_(Boolean) IsFlingActive();

    //@CalledByNative
    CARAPI_(void) UpdateScrollState(
        /* [in] */ Int32 maxContainerViewScrollOffsetX,
        /* [in] */ Int32 maxContainerViewScrollOffsetY,
        /* [in] */ Int32 contentWidthDip,
        /* [in] */ Int32 contentHeightDip,
        /* [in] */ Float pageScaleFactor,
        /* [in] */ Float minPageScaleFactor,
        /* [in] */ Float maxPageScaleFactor);

    //@CalledByNative
    CARAPI_(void) SetAwAutofillClient(
        /* [in] */ AwAutofillClient* client);

    //@CalledByNative
    CARAPI_(void) DidOverscroll(
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    // -------------------------------------------------------------------------------------------
    // Helper methods
    // -------------------------------------------------------------------------------------------

    CARAPI_(void) SetPageScaleFactorAndLimits(
        /* [in] */ Float pageScaleFactor,
        /* [in] */ Float minPageScaleFactor,
        /* [in] */ Float maxPageScaleFactor);

    CARAPI_(void) SaveWebArchiveInternal(
        /* [in] */ const String& path,
        /* [in] */ IValueCallback* callback);

    /**
     * Try to generate a pathname for saving an MHTML archive. This roughly follows WebView's
     * autoname logic.
     */
    static CARAPI_(String) GenerateArchiveAutoNamePath(
        /* [in] */ const String& originalUrl,
        /* [in] */ const String& baseName);


    // Return true if the GeolocationPermissionAPI should be used.
    //@CalledByNative
    CARAPI_(Boolean) UseLegacyGeolocationPermissionAPI();

private:
    //--------------------------------------------------------------------------------------------
    //  Native methods
    //--------------------------------------------------------------------------------------------

    static CARAPI_(Handle64) NativeInit(
        /* [in] */ AwBrowserContext* browserContext);

    static CARAPI_(void) NativeDestroy(
        /* [in] */ Handle64 nativeAwContents);

    static CARAPI_(void) NativeSetAwDrawSWFunctionTable(
        /* [in] */ Handle64 functionTablePointer);

    static CARAPI_(void) NativeSetAwDrawGLFunctionTable(
        /* [in] */ Handle64 functionTablePointer);

    static CARAPI_(Handle64) NativeGetAwDrawGLFunction();

    static CARAPI_(Int32) NativeGetNativeInstanceCount();

    static CARAPI_(void) NativeSetShouldDownloadFavicons();

    CARAPI_(void) NativeSetJavaPeers(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ AwContents* awContents,
        /* [in] */ AwWebContentsDelegate* webViewWebContentsDelegate,
        /* [in] */ AwContentsClientBridge* contentsClientBridge,
        /* [in] */ AwContentsIoThreadClient* ioThreadClient,
        /* [in] */ InterceptNavigationDelegate* navigationInterceptionDelegate);

    CARAPI_(Handle64) NativeGetWebContents(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(void) NativeDocumentHasImages(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ IMessage* message);

    CARAPI_(void) NativeGenerateMHTML(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ const String& path,
        /* [in] */ IValueCallback* callback);

    CARAPI_(void) NativeAddVisitedLinks(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ ArrayOf<String>* visitedLinks);

    CARAPI_(Boolean) NativeOnDraw(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ ICanvas* canvas,
        /* [in] */ Boolean isHardwareAccelerated,
        /* [in] */ Int32 scrollX,
        /* [in] */ Int32 scrollY,
        /* [in] */ Int32 visibleLeft,
        /* [in] */ Int32 visibleTop,
        /* [in] */ Int32 visibleRight,
        /* [in] */ Int32 visibleBottom);

    CARAPI_(void) NativeFindAllAsync(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ const String& searchString);

    CARAPI_(void) NativeFindNext(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean forward);

    CARAPI_(void) NativeClearMatches(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(void) NativeClearCache(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean includeDiskFiles);

    CARAPI_(AutoPtr<ArrayOf<Byte> >) NativeGetCertificate(
        /* [in] */ Handle64 nativeAwContents);

    // Coordinates in desity independent pixels.
    CARAPI_(void) NativeRequestNewHitTestDataAt(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) NativeUpdateLastHitTestData(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(void) NativeOnSizeChanged(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 ow,
        /* [in] */ Int32 oh);

    CARAPI_(void) NativeScrollTo(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) NativeSetViewVisibility(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean visible);

    CARAPI_(void) NativeSetWindowVisibility(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean visible);

    CARAPI_(void) NativeSetIsPaused(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean paused);

    CARAPI_(void) NativeOnAttachedToWindow(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Int32 w,
        /* [in] */ Int32 h);

    static CARAPI_(void) NativeOnDetachedFromWindow(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(void) NativeSetDipScale(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Float dipScale);

    // Returns null if save state fails.
    CARAPI_(AutoPtr<ArrayOf<Byte> >) NativeGetOpaqueState(
        /* [in] */ Handle64 nativeAwContents);

    // Returns false if restore state fails.
    CARAPI_(Boolean) NativeRestoreFromOpaqueState(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ ArrayOf<Byte>* state);

    CARAPI_(Handle64) NativeReleasePopupAwContents(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(void) NativeFocusFirstNode(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(void) NativeSetBackgroundColor(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Int32 color);

    CARAPI_(Handle64) NativeGetAwDrawGLViewContext(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(Handle64) NativeCapturePicture(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) NativeEnableOnNewPicture(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean enabled);

    CARAPI_(void) NativeClearView(
        /* [in] */ Handle64 nativeAwContents);

    CARAPI_(void) NativeSetExtraHeadersForUrl(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ const String& url,
        /* [in] */ const String& extraHeaders);

    CARAPI_(void) NativeInvokeGeolocationCallback(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean value,
        /* [in] */ const String& requestingFrame);

    CARAPI_(void) NativeSetJsOnlineProperty(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Boolean networkUp);

    CARAPI_(void) NativeTrimMemory(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ Int32 level,
        /* [in] */ Boolean visible);

    CARAPI_(void) NativeCreatePdfExporter(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ AwPdfExporter* awPdfExporter);

    CARAPI_(void) NativePreauthorizePermission(
        /* [in] */ Handle64 nativeAwContents,
        /* [in] */ const String& origin,
        /* [in] */ Int64 resources);

    CARAPI_(void) Init(
        /* [in] */ AwBrowserContext* browserContext,
        /* [in] */ IViewGroup* containerView,
        /* [in] */ IContext* context,
        /* [in] */ InternalAccessDelegate* internalAccessAdapter,
        /* [in] */ NativeGLDelegate* nativeGLDelegate,
        /* [in] */ AwContentsClient* contentsClient,
        /* [in] */ AwSettings* settings,
        /* [in] */ DependencyFactory* dependencyFactory);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwContentsCallback_Init();

private:
    static CARAPI_(void) OnDocumentHasImagesResponse(
        /* [in] */ Boolean result,
        /* [in] */ IInterface* message);

    static CARAPI_(void) OnReceivedTouchIconUrl(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& url,
        /* [in] */ Boolean precomposed);

    static CARAPI_(void) OnReceivedIcon(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* bitmap);

    static CARAPI_(void) OnReceivedHttpAuthRequest(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* handler,
        /* [in] */ const String& host,
        /* [in] */ const String& realm);

    static CARAPI_(void) OnGeolocationPermissionsShowPrompt(
        /* [in] */ IInterface* obj,
        /* [in] */ const String& origin);

    static CARAPI_(void) OnGeolocationPermissionsHidePrompt(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnPermissionRequest(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* awPermissionRequest);

    static CARAPI_(void) OnPermissionRequestCanceled(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* awPermissionRequest);

    static CARAPI_(void) OnFindResultReceived(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 activeMatchOrdinal,
        /* [in] */ Int32 numberOfMatches,
        /* [in] */ Boolean isDoneCounting);

    static CARAPI_(void) OnNewPicture(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) UpdateHitTestData(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 type,
        /* [in] */ const String& extra,
        /* [in] */ const String& href,
        /* [in] */ const String& anchorText,
        /* [in] */ const String& imgSrc);

    static CARAPI_(Boolean) RequestDrawGL(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* canvas,
        /* [in] */ Boolean waitForCompletion);

    static CARAPI_(void) PostInvalidateOnAnimation(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) InvalidateOnFunctorDestroy(
        /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetLocationOnScreen(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) OnWebLayoutPageScaleFactorChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Float webLayoutPageScaleFactor);

    static CARAPI_(void) OnWebLayoutContentsSizeChanged(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 widthCss,
        /* [in] */ Int32 heightCss);

    static CARAPI_(void) ScrollContainerViewTo(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    static CARAPI_(Boolean) IsFlingActive(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) UpdateScrollState(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 maxContainerViewScrollOffsetX,
        /* [in] */ Int32 maxContainerViewScrollOffsetY,
        /* [in] */ Int32 contentWidthDip,
        /* [in] */ Int32 contentHeightDip,
        /* [in] */ Float pageScaleFactor,
        /* [in] */ Float minPageScaleFactor,
        /* [in] */ Float maxPageScaleFactor);

    static CARAPI_(void) SetAwAutofillClient(
        /* [in] */ IInterface* obj,
        /* [in] */ IInterface* client);

    static CARAPI_(void) DidOverscroll(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 deltaX,
        /* [in] */ Int32 deltaY);

    static CARAPI_(Boolean) UseLegacyGeolocationPermissionAPI(
        /* [in] */ IInterface* obj);

private:
    // This is only to avoid heap allocations inside getGlobalVisibleRect. It should treated
    // as a local variable in the function and not used anywhere else.
    static const AutoPtr<IRect> sLocalGlobalVisibleRect;

    // TODO(mkosiba): In WebViewClassic these appear in some of the scroll extent calculation
    // methods but toggling them has no visiual effect on the content (in other words the scrolling
    // code behaves as if the scrollbar-related padding is in place but the onDraw code doesn't
    // take that into consideration).
    // http://crbug.com/269032
    Boolean mOverlayHorizontalScrollbar;
    Boolean mOverlayVerticalScrollbar;

    static const String TAG;

    static const String WEB_ARCHIVE_EXTENSION;

    // Used to avoid enabling zooming in / out if resulting zooming will
    // produce little visible difference.
    static const Float ZOOM_CONTROLS_EPSILON;

    Handle64 mNativeAwContents;
    AutoPtr<AwBrowserContext> mBrowserContext;
    AutoPtr<IViewGroup> mContainerView;
    AutoPtr<AwLayoutChangeListener> mLayoutChangeListener;
    AutoPtr<IContext> mContext;
    AutoPtr<ContentViewCore> mContentViewCore;
    AutoPtr<AwContentsClient> mContentsClient;
    AutoPtr<AwContentViewClient> mContentViewClient;
    AutoPtr<AwContentsClientBridge> mContentsClientBridge;
    AutoPtr<AwWebContentsDelegateAdapter> mWebContentsDelegate;
    AutoPtr<AwContentsIoThreadClient> mIoThreadClient;
    AutoPtr<InterceptNavigationDelegateImpl> mInterceptNavigationDelegate;
    AutoPtr<InternalAccessDelegate> mInternalAccessAdapter;
    AutoPtr<NativeGLDelegate> mNativeGLDelegate;
    AutoPtr<AwLayoutSizer> mLayoutSizer;
    AutoPtr<AwZoomControls> mZoomControls;
    AutoPtr<AwScrollOffsetManager> mScrollOffsetManager;
    AutoPtr<OverScrollGlow> mOverScrollGlow;
    // This can be accessed on any thread after construction. See AwContentsIoThreadClient.
    AutoPtr<AwSettings> mSettings;
    AutoPtr<ScrollAccessibilityHelper> mScrollAccessibilityHelper;

    Boolean mIsPaused;
    Boolean mIsViewVisible;
    Boolean mIsWindowVisible;
    Boolean mIsAttachedToWindow;
    AutoPtr<IBitmap> mFavicon;
    Boolean mHasRequestedVisitedHistoryFromClient;
    // TODO(boliu): This should be in a global context, not per webview.
    Double mDIPScale;

    // The base background color, i.e. not accounting for any CSS body from the current page.
    Int32 mBaseBackgroundColor;

    // Must call nativeUpdateLastHitTestData first to update this before use.
    AutoPtr<HitTestData> mPossiblyStaleHitTestData;

    AutoPtr<DefaultVideoPosterRequestHandler> mDefaultVideoPosterRequestHandler;

    // Bound method for suppling Picture instances to the AwContentsClient. Will be null if the
    // picture listener API has not yet been enabled, or if it is using invalidation-only mode.
    AutoPtr<ICallable> mPictureListenerContentProvider;

    Boolean mContainerViewFocused;
    Boolean mWindowFocused;

    // These come from the compositor and are updated synchronously (in contrast to the values in
    // ContentViewCore, which are updated at end of every frame).
    Float mPageScaleFactor;
    Float mMinPageScaleFactor;
    Float mMaxPageScaleFactor;
    Float mContentWidthDip;
    Float mContentHeightDip;

    AutoPtr<AwAutofillClient> mAwAutofillClient;

    AutoPtr<AwPdfExporter> mAwPdfExporter;

    AutoPtr<AwViewMethods> mAwViewMethods;

    AutoPtr<FullScreenTransitionsState> mFullScreenTransitionsState;

    // This flag indicates that ShouldOverrideUrlNavigation should be posted
    // through the resourcethrottle. This is only used for popup windows.
    Boolean mDeferredShouldOverrideUrlLoadingIsPendingForPopup;

    // The framework may temporarily detach our container view, for example during layout if
    // we are a child of a ListView. This may cause many toggles of View focus, which we suppress
    // when in this state.
    Boolean mTemporarilyDetached;

    // Reference to the active mNativeAwContents pointer while it is active use
    // (ie before it is destroyed).
    AutoPtr<CleanupReference> mCleanupReference;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTS_H__
