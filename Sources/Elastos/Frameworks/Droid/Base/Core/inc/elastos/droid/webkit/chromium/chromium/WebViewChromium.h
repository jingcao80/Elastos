
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCHROMIUM_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCHROMIUM_H__

// import android.content.Context;
// import android.content.ContextWrapper;
// import android.content.pm.PackageManager;
// import android.content.res.Configuration;
// import android.content.res.Resources;
// import android.graphics.Bitmap;
// import android.graphics.Canvas;
// import android.graphics.Paint;
// import android.graphics.Picture;
// import android.graphics.Rect;
// import android.graphics.drawable.Drawable;
// import android.net.Uri;
// import android.net.http.SslCertificate;
// import android.os.Build;
// import android.os.Bundle;
// import android.os.Looper;
// import android.os.Handler;
// import android.os.Message;
// import android.print.PrintDocumentAdapter;
// import android.text.TextUtils;
// import android.util.Base64;
// import android.util.Log;
// import android.view.HardwareCanvas;
// import android.view.KeyEvent;
// import android.view.LayoutInflater;
// import android.view.MotionEvent;
// import android.view.View;
// import android.view.View.MeasureSpec;
// import android.view.ViewGroup;
// import android.view.accessibility.AccessibilityEvent;
// import android.view.accessibility.AccessibilityNodeInfo;
// import android.view.accessibility.AccessibilityNodeProvider;
// import android.view.inputmethod.EditorInfo;
// import android.view.inputmethod.InputConnection;
// import android.webkit.DownloadListener;
// import android.webkit.FindActionModeCallback;
// import android.webkit.JavascriptInterface;
// import android.webkit.ValueCallback;
// import android.webkit.WebBackForwardList;
// import android.webkit.WebChromeClient;
// import android.webkit.WebSettings;
// import android.webkit.WebView;
// import android.webkit.WebViewClient;
// import android.webkit.WebViewFactory;
// import android.webkit.WebViewProvider;
// import android.webkit.WebChromeClient.CustomViewCallback;
// import android.widget.TextView;

// import org.chromium.android_webview.AwBrowserContext;
// import org.chromium.android_webview.AwContents;
// import org.chromium.android_webview.AwContentsStatics;
// import org.chromium.android_webview.AwLayoutSizer;
// import org.chromium.android_webview.AwSettings;
// import org.chromium.android_webview.AwPrintDocumentAdapter;
// import org.chromium.base.ThreadUtils;
// import org.chromium.content.browser.LoadUrlParams;
// import org.chromium.content.browser.SmartClipProvider;
// import org.chromium.net.NetworkChangeNotifier;

// import java.io.BufferedWriter;
// import java.io.File;
// import java.lang.annotation.Annotation;
// import java.util.concurrent.Callable;
// import java.util.concurrent.ConcurrentLinkedQueue;
// import java.util.concurrent.FutureTask;
// import java.util.concurrent.TimeUnit;
// import java.util.HashMap;
// import java.util.Map;
// import java.util.Queue;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/**
 * This class is the delegate to which WebViewProxy forwards all API calls.
 *
 * Most of the actual functionality is implemented by AwContents (or ContentViewCore within
 * it). This class also contains WebView-specific APIs that require the creation of other
 * adapters (otherwise org.chromium.content would depend on the webview.chromium package)
 * and a small set of no-op deprecated APIs.
 */
class WebViewChromium
    : public IWebViewProvider
    , public IWebViewProviderScrollDelegate
    , public IWebViewProviderViewDelegate
    , public SmartClipProvider
{
private:
    class WebViewChromiumRunQueue
    {
    private:
        class InnerRunnable
            : public Object
            , public IRunnable
        {
        public:
            InnerRunnable(
                /* [in] */ WebViewChromiumRunQueue* owner);

            CAR_INTERFACE_DECL();

            CARAPI Run();

        private:
            WebViewChromiumRunQueue* mOwner;
        };

    public:
        WebViewChromiumRunQueue(
            /* [in] */ WebViewChromium* owner);

        CARAPI_(void) AddTask(Runnable task);

        CARAPI_(void) DrainQueue();

    private:
        WebViewChromium* mOwner;
        Queue<Runnable> mQueue;
    };

    // AwContents.NativeGLDelegate implementation --------------------------------------
    class WebViewNativeGLDelegate
        : public AwContents::NativeGLDelegate
    {
    public:
        WebViewNativeGLDelegate(
            /* [in] */ WebViewChromium* owner);

        //@Override
        CARAPI_(Boolean) RequestDrawGL(
            /* [in] */ ICanvas* canvas,
            /* [in] */ Boolean waitForCompletion,
            /* [in] */ IView* containerView);

        //@Override
        CARAPI_(void) DetachGLFunctor();

    private:
        WebViewChromium* mOwner;
    };

    // AwContents.InternalAccessDelegate implementation --------------------------------------
    class InternalAccessAdapter
        : public AwContents::InternalAccessDelegate
    {
    public:
        InternalAccessAdapter(
            /* [in] */ WebViewChromium* owner);

        //@Override
        CARAPI_(Boolean) DrawChild(
            /* [in] */ ICanvas* arg0,
            /* [in] */ IView* arg1,
            /* [in] */ Int64 arg2);

        //@Override
        CARAPI_(Boolean) Super_onKeyUp(
            /* [in] */ Int32 arg0,
            /* [in] */ IKeyEvent* arg1);

        //@Override
        CARAPI_(Boolean) Super_dispatchKeyEventPreIme(
            /* [in] */ IKeyEvent* arg0);

        //@Override
        CARAPI_(Boolean) Super_dispatchKeyEvent(
            /* [in] */ IKeyEvent* event);

        //@Override
        CARAPI_(Boolean) Super_onGenericMotionEvent(
            /* [in] */ IMotionEvent* arg0);

        //@Override
        CARAPI_(void) Super_onConfigurationChanged(
            /* [in] */ IConfiguration* arg0);

        //@Override
        CARAPI_(Int32) Super_getScrollBarStyle();

        //@Override
        CARAPI_(Boolean) AwakenScrollBars();

        //@Override
        CARAPI_(Boolean) Super_awakenScrollBars(
            /* [in] */ Int32 arg0,
            /* [in] */ Boolean arg1);

        //@Override
        CARAPI_(void) OnScrollChanged(
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 oldl,
            /* [in] */ Int32 oldt);

        //@Override
        CARAPI_(void) OverScrollBy(
            /* [in] */ Int32 deltaX,
            /* [in] */ Int32 deltaY,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY,
            /* [in] */ Int32 scrollRangeX,
            /* [in] */ Int32 scrollRangeY,
            /* [in] */ Int32 maxOverScrollX,
            /* [in] */ Int32 maxOverScrollY,
            /* [in] */ Boolean isTouchEvent);

        //@Override
        CARAPI_(void) Super_scrollTo(
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY);

        //@Override
        CARAPI_(void) SetMeasuredDimension(
            /* [in] */ Int32 measuredWidth,
            /* [in] */ Int32 measuredHeight);

        // @Override
        CARAPI_(Boolean) Super_onHoverEvent(
            /* [in] */ IMotionEvent* event);

    private:
        WebViewChromium* mOwner;
    };

    class InitRunnable
        : public Object
        , public IRunnable
    {
    public:
        InitRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean privateBrowsing);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mPrivateBrowsing;
    };

    class CheckThreadRunnable
        : public Object
        , public IRunnable
    {
    public:
        CheckThreadRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class SetHorizontalScrollbarOverlayRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetHorizontalScrollbarOverlayRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean overlay);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mOverlay
    };

    class SetVerticalScrollbarOverlayRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetVerticalScrollbarOverlayRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean overlay);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mOverlay
    };

    class OverlayHorizontalScrollbarCallable
        : public Object
        , public ICallable
    {
    public:
        OverlayHorizontalScrollbarCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class OverlayVerticalScrollbarCallable
        : public Object
        , public ICallable
    {
    public:
        OverlayVerticalScrollbarCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class GetCertificateCallable
        : public Object
        , public ICallable
    {
    public:
        GetCertificateCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class SetHttpAuthUsernamePasswordRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetHttpAuthUsernamePasswordRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& host,
            /* [in] */ const String& realm,
            /* [in] */ const String& username,
            /* [in] */ const String& password);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String& mHost,
        const String& mRealm,
        const String& mUsername,
        const String& mPassword
    };

    class GetHttpAuthUsernamePasswordCallable
        : public Object
        , public ICallable
    {
    public:
        GetHttpAuthUsernamePasswordCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& host,
            /* [in] */ const String& realm);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const String& mHost,
        const String& mRealm
    };

    class DestroyRunnable
        : public Object
        , public IRunnable
    {
    public:
        DestroyRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class SetNetworkAvailableRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetNetworkAvailableRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean networkUp);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mNetworkUp;
    };

    class SaveStateCallable
        : public Object
        , public ICallable
    {
    public:
        SaveStateCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IBundle* outState);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const IBundle* mOutState
    };

    class RestoreStateCallable
        : public Object
        , public ICallable
    {
    public:
        RestoreStateCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IBundle* inState);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const IBundle* mInState
    };

    class LoadUrlRunnable
        : public Object
        , public IRunnable
    {
    public:
        LoadUrlRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& url,
            /* [in] */ const String& JAVASCRIPT_SCHEME);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String& mUrl;
        const String& mJAVASCRIPT_SCHEME;
    };

    class LoadUrlOnUiThreadRunnable
        : public Object
        , public IRunnable
    {
    public:
        LoadUrlOnUiThreadRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const LoadUrlParams* loadUrlParams);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const LoadUrlParams* mLoadUrlParams;
    };

    class SaveWebArchiveRunnable
        : public Object
        , public IRunnable
    {
    public:
        SaveWebArchiveRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& basename,
            /* [in] */ const Boolean autoname,
            /* [in] */ IValueCallback* callback);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String& mBasename,
        const Boolean mAutoname,
        IValueCallback* mCallback
    };

    class StopLoadingRunnable
        : public Object
        , public IRunnable
    {
    public:
        StopLoadingRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class ReloadRunnable
        : public Object
        , public IRunnable
    {
    public:
        ReloadRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class CanGoBackCallable
        : public Object
        , public ICallable
    {
    public:
        CanGoBackCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class GoBackRunnable
        : public Object
        , public IRunnable
    {
    public:
        GoBackRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class CanGoForwardCallable
        : public Object
        , public ICallable
    {
    public:
        CanGoForwardCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class GoForwardRunnable
        : public Object
        , public IRunnable
    {
    public:
        GoForwardRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class CanGoBackOrForwardCallable
        : public Object
        , public ICallable
    {
    public:
        CanGoBackOrForwardCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 steps);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mSteps;
    };

    class GoBackOrForwardRunnable
        : public Object
        , public IRunnable
    {
    public:
        GoBackOrForwardRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 steps);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mSteps;
    };

    class PageUpCallable
        : public Object
        , public ICallable
    {
    public:
        PageUpCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean top);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Boolean mTop;
    };

    class PageDownCallable
        : public Object
        , public ICallable
    {
    public:
        PageDownCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean bottom);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Boolean mBottomp;
    };

    class ClearViewRunnable
        : public Object
        , public IRunnable
    {
    public:
        ClearViewRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class CapturePictureCallable
        : public Object
        , public ICallable
    {
    public:
        CapturePictureCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class InvokeZoomPickerRunnable
        : public Object
        , public IRunnable
    {
    public:
        InvokeZoomPickerRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class GetHitTestResultCallable
        : public Object
        , public ICallable
    {
    public:
        GetHitTestResultCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class RequestFocusNodeHrefRunnable
        : public Object
        , public IRunnable
    {
    public:
        RequestFocusNodeHrefRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IMessage* hrefMsg);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IMessage* mHrefMsg;
    };

    class RequestImageRefRunnable
        : public Object
        , public IRunnable
    {
    public:
        RequestImageRefRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IMessage* msg);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IMessage* mMsg;
    };

    class GetUrlCallable
        : public Object
        , public ICallable
    {
    public:
        GetUrlCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class GetOriginalUrlCallable
        : public Object
        , public ICallable
    {
    public:
        GetOriginalUrlCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class GetTitleCallable
        : public Object
        , public ICallable
    {
    public:
        GetTitleCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class GetFaviconCallable
        : public Object
        , public ICallable
    {
    public:
        GetFaviconCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class PauseTimersRunnable
        : public Object
        , public IRunnable
    {
    public:
        PauseTimersRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class ResumeTimersRunnable
        : public Object
        , public IRunnable
    {
    public:
        ResumeTimersRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class OnPauseRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnPauseRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class OnResumeRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnResumeRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class IsPausedCallable
        : public Object
        , public ICallable
    {
    public:
        IsPausedCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class ClearCacheRunnable
        : public Object
        , public IRunnable
    {
    public:
        ClearCacheRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean includeDiskFiles);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mIncludeDiskFiles;
    };

    class ClearFormDataRunnable
        : public Object
        , public IRunnable
    {
    public:
        ClearFormDataRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class ClearHistoryRunnable
        : public Object
        , public IRunnable
    {
    public:
        ClearHistoryRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class ClearSslPreferencesRunnable
        : public Object
        , public IRunnable
    {
    public:
        ClearSslPreferencesRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class CopyBackForwardListCallable
        : public Object
        , public ICallable
    {
    public:
        CopyBackForwardListCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class FindNextRunnable
        : public Object
        , public IRunnable
    {
    public:
        FindNextRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean forwards);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mForwards;
    };

    class FindAllAsyncRunnable
        : public Object
        , public IRunnable
    {
    public:
        FindAllAsyncRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& searchString);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String& mSearchString;
    };

    class NotifyFindDialogDismissedRunnable
        : public Object
        , public IRunnable
    {
    public:
        NotifyFindDialogDismissedRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class ClearMatchesRunnable
        : public Object
        , public IRunnable
    {
    public:
        ClearMatchesRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class DocumentHasImagesRunnable
        : public Object
        , public IRunnable
    {
    public:
        DocumentHasImagesRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IMessage* response);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IMessage* mResponse;
    };

    class SetPictureListenerRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetPictureListenerRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IWebViewPictureListener* listener);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IWebViewPictureListener* mListener;
    };

    class AddJavascriptInterfaceRunnable
        : public Object
        , public IRunnable
    {
    public:
        AddJavascriptInterfaceRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ IInterface* obj,
            /* [in] */ const String& interfaceName);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        IInterface* mObj;
        const String& mInterfaceName;
    };

    class RemoveJavascriptInterfaceRunnable
        : public Object
        , public IRunnable
    {
    public:
        RemoveJavascriptInterfaceRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const String& interfaceName);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const String& mInterfaceName;
    };

    class FlingScrollRunnable
        : public Object
        , public IRunnable
    {
    public:
        FlingScrollRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 vx,
            /* [in] */ const Int32 vy);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mVx;
        const Int32 mVy;
    };

    class ZoomInCallable
        : public Object
        , public ICallable
    {
    public:
        ZoomInCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class ZoomOutCallable
        : public Object
        , public ICallable
    {
    public:
        ZoomOutCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };


    class ShouldDelayChildPressedStateCallable
        : public Object
        , public ICallable
    {
    public:
        ShouldDelayChildPressedStateCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class GetAccessibilityNodeProviderCallable
        : public Object
        , public ICallable
    {
    public:
        GetAccessibilityNodeProviderCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class OnInitializeAccessibilityNodeInfoRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnInitializeAccessibilityNodeInfoRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IAccessibilityNodeInfo* info);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IAccessibilityNodeInfo* mInfo;
    };

    class OnInitializeAccessibilityEventRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnInitializeAccessibilityEventRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IAccessibilityEvent* event);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IAccessibilityEvent* mEvent;
    };

    class PerformAccessibilityActionCallable
        : public Object
        , public ICallable
    {
    public:
        PerformAccessibilityActionCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 action,
            /* [in] */ const IBundle* arguments);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mAction;
        const IBundle* mArguments;
    };

    class SetOverScrollModeRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetOverScrollModeRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 mode);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mMode;
    };

    class SetScrollBarStyleRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetScrollBarStyleRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 style);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mStyle;
    };

    class OnOverScrolledRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnOverScrolledRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 scrollX,
            /* [in] */ const Int32 scrollY,
            /* [in] */ const Boolean clampedX,
            /* [in] */ const Boolean clampedY);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mScrollX,
        const Int32 mScrollY,
        const Boolean mClampedX,
        const Boolean mClampedY
    };

    class OnWindowVisibilityChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnWindowVisibilityChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 visibility);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 visibility;
    };

    class OnDrawRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnDrawRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const ICanvas* canvas);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const ICanvas* mCanvas;
    };

    class OnConfigurationChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnConfigurationChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IConfiguration* newConfig);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IConfiguration* mNewConfig;
    };

    class OnKeyMultipleCallable
        : public Object
        , public ICallable
    {
    public:
        OnKeyMultipleCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 keyCode,
            /* [in] */ const Int32 repeatCount,
            /* [in] */ const IKeyEvent* event);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mKeyCode;
        const Int32 mRepeatCount;
        const IKeyEvent* mEvent;
    };

    class OnKeyDownCallable
        : public Object
        , public ICallable
    {
    public:
        OnKeyDownCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 keyCode,
            /* [in] */ const IKeyEvent* event);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mKeyCode;
        const IKeyEvent* mEvent;
    };

    class OnKeyUpCallable
        : public Object
        , public ICallable
    {
    public:
        OnKeyUpCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 keyCode,
            /* [in] */ const IKeyEvent* event);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mKeyCode;
        const IKeyEvent* mEvent;
    };

    class OnDetachedFromWindowRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnDetachedFromWindowRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

    class OnVisibilityChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnVisibilityChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IView* changedView,
            /* [in] */ const Int32 visibility);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const IView* mChangedView;
        const Int32 mVisibility;
    };

    class OnWindowFocusChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnWindowFocusChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean hasWindowFocus);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mHasWindowFocus;
    };

    class OnFocusChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnFocusChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Boolean focused,
            /* [in] */ const Int32 direction,
            /* [in] */ const IRect* previouslyFocusedRect);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Boolean mFocused;
        const Int32 mDirection,
        const IRect* mPreviouslyFocusedRect
    };

    class OnSizeChangedRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnSizeChangedRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 w,
            /* [in] */ const Int32 h,
            /* [in] */ const Int32 ow,
            /* [in] */ const Int32 oh);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mW;
        const Int32 mH;
        const Int32 mOw;
        const Int32 mOh;
    };

    class DispatchKeyEventCallable
        : public Object
        , public ICallable
    {
    public:
        DispatchKeyEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IKeyEvent* event);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const IKeyEvent* mEvent;
    };

    class OnTouchEventCallable
        : public Object
        , public ICallable
    {
    public:
        OnTouchEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IMotionEvent* ev);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const IMotionEvent* mEv;
    };

    class OnHoverEventCallable
        : public Object
        , public ICallable
    {
    public:
        OnHoverEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IMotionEvent* event);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const IMotionEvent* mEvent;
    };

    class OnGenericMotionEventCallable
        : public Object
        , public ICallable
    {
    public:
        OnGenericMotionEventCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IMotionEvent* event);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const IMotionEvent* mEvent;
    };

    class RequestFocusCallable
        : public Object
        , public ICallable
    {
    public:
        RequestFocusCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 direction,
            /* [in] */ const IRect* previouslyFocusedRect);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const Int32 mDirection;
        const IRect* mPreviouslyFocusedRect;
    };

    class OnMeasureRunnable
        : public Object
        , public IRunnable
    {
    public:
        OnMeasureRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 widthMeasureSpec,
            /* [in] */ const Int32 heightMeasureSpec);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mWidthMeasureSpec;
        const Int32 mHeightMeasureSpec;
    };

    class RequestChildRectangleOnScreenCallable
        : public Object
        , public ICallable
    {
    public:
        RequestChildRectangleOnScreenCallable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const IView* child,
            /* [in] */ const IRect* rect,
            /* [in] */ const Boolean immediate);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
        const IView* mChild;
        const IRect* mRect;
        const Boolean mImmediate;
    };

    class SetBackgroundColorRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetBackgroundColorRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 color);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mColor;
    };

    class SetLayerTypeRunnable
        : public Object
        , public IRunnable
    {
    public:
        SetLayerTypeRunnable(
            /* [in] */ WebViewChromium* owner,
            /* [in] */ const Int32 layerType,
            /* [in] */ const IPaint* paint);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
        const Int32 mLayerType;
        const IPaint* mPaint;
    };

    class ComputeHorizontalScrollRangeCallable
        : public Object
        , public ICallable
    {
    public:
        ComputeHorizontalScrollRangeCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };


    class ComputeHorizontalScrollOffsetCallable
        : public Object
        , public ICallable
    {
    public:
        ComputeHorizontalScrollOffsetCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class ComputeVerticalScrollRangeCallable
        : public Object
        , public ICallable
    {
    public:
        ComputeVerticalScrollRangeCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class ComputeVerticalScrollOffsetCallable
        : public Object
        , public ICallable
    {
    public:
        ComputeVerticalScrollOffsetCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class ComputeVerticalScrollExtentCallable
        : public Object
        , public ICallable
    {
    public:
        ComputeVerticalScrollExtentCallable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        WebViewChromium* mOwner;
    };

    class ComputeScrollRunnable
        : public Object
        , public IRunnable
    {
    public:
        ComputeScrollRunnable(
            /* [in] */ WebViewChromium* owner);

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        WebViewChromium* mOwner;
    };

public:
    // This does not touch any global / non-threadsafe state, but note that
    // init is ofter called right after and is NOT threadsafe.
    WebViewChromium(
        /* [in] */ WebViewChromiumFactoryProvider* factory,
        /* [in] */ IWebView* webView,
        /* [in] */ IWebViewPrivateAccess* webViewPrivate);

    static CARAPI_(void) EnableSlowWholeDocumentDraw();

    static CARAPI_(void) CompleteWindowCreation(
        /* [in] */ IWebView* parent,
        /* [in] */ IWebView* child);

    // WebViewProvider methods --------------------------------------------------------------------

    //@Override
    // BUG=6790250 |javaScriptInterfaces| was only ever used by the obsolete DumpRenderTree
    // so is ignored. TODO: remove it from WebViewProvider.
    CARAPI Init(
        /* [in] */ const IMap* javaScriptInterfaces,
        /* [in] */ const Boolean privateBrowsing);

    CARAPI_(void) StartYourEngine();

    //@Override
    CARAPI SetHorizontalScrollbarOverlay(
        /* [in] */ const Boolean overlay);

    //@Override
    CARAPI SetVerticalScrollbarOverlay(
        /* [in] */ const Boolean overlay);

    //@Override
    CARAPI OverlayHorizontalScrollbar(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OverlayVerticalScrollbar(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetVisibleTitleHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetCertificate(
        /* [out] */ ISslCertificate** ssl);

    //@Override
    CARAPI SetCertificate(
        /* [in] */ ISslCertificate* certificate);

    //@Override
    CARAPI SavePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    //@Override
    CARAPI SetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm,
        /* [in] */ const String& username,
        /* [in] */ const String& password);

    //@Override
    CARAPI GetHttpAuthUsernamePassword(
        /* [in] */ const String& host,
        /* [in] */ const String& realm,
        /* [out] */ ArrayOf<String>** up);

    //@Override
    CARAPI Destroy();

    //@Override
    CARAPI SetNetworkAvailable(
        /* [in] */ const Boolean networkUp);

    //@Override
    CARAPI SaveState(
        /* [in] */ const IBundle* outState,
        /* [out] */ IWebBackForwardList** wfl);

    //@Override
    CARAPI SavePicture(
        /* [in] */ IBundle* b,
        /* [in] */ IFile* dest,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RestorePicture(
        /* [in] */ IBundle* b,
        /* [in] */ IFile* src,
        /* [in] */ Boolean* result);

    //@Override
    CARAPI RestoreState(
        /* [in] */ const IBundle* inState,
        /* [out] */ IWebBackForwardList** wfl);

    //@Override
    CARAPI LoadUrl(
        /* [in] */ const String& url,
        /* [in] */ Map<String, String> additionalHttpHeaders);

    //@Override
    CARAPI LoadUrl(
        /* [in] */ const String& url);

    //@Override
    CARAPI PostUrl(
        /* [in] */ const String& url,
        /* [in] */ ArrayOf<Byte>* postData);

    //@Override
    CARAPI LoadData(
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& encoding);

    //@Override
    CARAPI LoadDataWithBaseURL(
        /* [in] */ const String& baseUrl,
        /* [in] */ const String& data,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& encoding,
        /* [in] */ const String& historyUrl);

    CARAPI_(void) EvaluateJavaScript(
        /* [in] */ const String& script,
        /* [in] */ IValueCallback* resultCallback);

    //@Override
    CARAPI SaveWebArchive(
        /* [in] */ const String& filename);

    //@Override
    CARAPI SaveWebArchive(
        /* [in] */ const String basename,
        /* [in] */ const Boolean autoname,
        /* [in] */ const IValueCallback* callback);

    //@Override
    CARAPI StopLoading();

    //@Override
    CARAPI Reload();

    //@Override
    CARAPI CanGoBack(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GoBack();

    //@Override
    CARAPI CanGoForward(
        /* [out] */ Boolean *result);

    //@Override
    CARAPI GoForward();

    //@Override
    CARAPI CanGoBackOrForward(
        /* [in] */ const Int32 steps,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GoBackOrForward(
        /* [in] */ const Int32 steps);

    //@Override
    CARAPI IsPrivateBrowsingEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI PageUp(
        /* [in] */ const Boolean top,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI PageDown(
        /* [in] */ const Boolean bottom,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ClearView();

    //@Override
    CARAPI CapturePicture(
        /* [out] */ IPicture** pic);

    //@Override
    CARAPI GetScale(
        /* [out] */ Float* scale);

    //@Override
    CARAPI SetInitialScale(
        /* [in] */ const Int32 scaleInPercent);

    //@Override
    CARAPI InvokeZoomPicker();

    //@Override
    CARAPI GetHitTestResult(
        /* [out] */ IWebViewHitTestResult** result);

    //@Override
    CARAPI RequestFocusNodeHref(
        /* [in] */ const IMessage* hrefMsg);

    //@Override
    CARAPI RequestImageRef(
        /* [in] */ const IMessage* msg);

    //@Override
    CARAPI GetUrl(
        /* [out] */ String* url);

    //@Override
    CARAPI GetOriginalUrl(
        /* [out] */ String* url);

    //@Override
    CARAPI GetTitle(
        /* [out] */ String* title);

    //@Override
    CARAPI GetFavicon(
        /* [out] */ IBitmap** bitmap);

    //@Override
    CARAPI GetTouchIconUrl(
        /* [out] */ String* url);

    //@Override
    CARAPI GetProgress(
        /* [out] */ Int32* progress);

    //@Override
    CARAPI GetContentHeight(
        /* [out] */ Int32* height);

    //@Override
    Int32 GetContentWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI PauseTimers();

    //@Override
    CARAPI ResumeTimers();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI IsPaused(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI FreeMemory();

    //@Override
    CARAPI ClearCache(
        /* [in] */ const Boolean includeDiskFiles);

    /**
     * This is a poorly named method, but we keep it for historical reasons.
     */
    //@Override
    CARAPI ClearFormData();

    //@Override
    CARAPI ClearHistory();

    //@Override
    CARAPI ClearSslPreferences();

    //@Override
    CARAPI CopyBackForwardList(
        /* [out] */ IWebBackForwardList** wfl);

    //@Override
    CARAPI SetFindListener(
        /* [in] */ IWebViewFindListener* listener);

    //@Override
    CARAPI FindNext(
        /* [in] */ const Boolean forwards);

    //@Override
    CARAPI FindAll(
        /* [in] */ const String& searchString,
        /* [out] */ Int32* all);

    //@Override
    CARAPI FindAllAsync(
        /* [in] */ const String& searchString);

    //@Override
    CARAPI ShowFindDialog(
        /* [in] */ const String& text,
        /* [in] */ const Boolean showIme,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI NotifyFindDialogDismissed();

    //@Override
    CARAPI ClearMatches();

    //@Override
    CARAPI DocumentHasImages(
        /* [in] */ const IMessage* response);

    //@Override
    CARAPI SetWebViewClient(
        /* [in] */ IWebViewClient* client);

    //@Override
    CARAPI SetDownloadListener(
        /* [in] */ IDownloadListener* listener);

    //@Override
    CARAPI SetWebChromeClient(
        /* [in] */ IWebChromeClient* client);

    //@Override
    CARAPI SetPictureListener(
        /* [in] */ const IWebViewPictureListener* listener);

    //@Override
    CARAPI AddJavascriptInterface(
        /* [in] */ const IInterface* obj,
        /* [in] */ const String& interfaceName);

    //@Override
    CARAPI RemoveJavascriptInterface(
        /* [in] */ const String& interfaceName);

    //@Override
    CARAPI GetSettings(
        /* [out] */ IWebSettings** ws);

    //@Override
    CARAPI SetMapTrackballToArrowKeys(
        /* [in] */ Boolean setMap);

    //@Override
    CARAPI FlingScroll(
        /* [in] */ const Int32 vx,
        /* [in] */ const Int32 vy);

    //@Override
    CARAPI GetZoomControls(
        /* [out] */ IView** view);

    //@Override
    CARAPI CanZoomIn(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI CanZoomOut(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ZoomIn(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ZoomOut(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI ZoomBy(
        /* [in] */ Float factor,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI DumpViewHierarchyWithProperties(
        /* [in] */ IBufferedWriter* out,
        /* [in] */ Int32 level);

    //@Override
    CARAPI FindHierarchyView(
        /* [in] */ const String& className,
        /* [in] */ Int32 hashCode,
        /* [out] */ IView** view);

    // WebViewProvider glue methods ---------------------------------------------------------------

    //@Override
    // This needs to be kept thread safe!
    CARAPI GetViewDelegate(
        /* [out] */ IWebViewProviderViewDelegate** delegate);

    //@Override
    // This needs to be kept thread safe!
    CARAPI GetScrollDelegate(
        /* [out] */ IWebViewProviderScrollDelegate** delegate);

    // WebViewProvider.ViewDelegate implementation ------------------------------------------------

    // TODO: remove from WebViewProvider and use default implementation from
    // ViewGroup.
    // @Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetAccessibilityNodeProvider(
        /* [out] */ IAccessibilityNodeProvider** provider);

    //@Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ const IAccessibilityNodeInfo* info);

    //@Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ const IAccessibilityEvent* event);

    //@Override
    CARAPI PerformAccessibilityAction(
        /* [in] */ const Int32 action,
        /* [in] */ const IBundle* arguments,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetOverScrollMode(
        /* [in] */ const Int32 mode);

    //@Override
    CARAPI SetScrollBarStyle(
        /* [in] */ const Int32 style);

    //@Override
    CARAPI OnDrawVerticalScrollBar(
        /* [in] */ const ICanvas* canvas,
        /* [in] */ const IDrawable* scrollBar,
        /* [in] */ const Int32 l,
        /* [in] */ const Int32 t,
        /* [in] */ const Int32 r,
        /* [in] */ const Int32 b);

    //@Override
    CARAPI OnOverScrolled(
        /* [in] */ const Int32 scrollX,
        /* [in] */ const Int32 scrollY,
        /* [in] */ const Boolean clampedX,
        /* [in] */ const Boolean clampedY);

    //@Override
    CARAPI OnWindowVisibilityChanged(
        /* [in] */ const Int32 visibility);

    //@Override
    CARAPI OnDraw(
        /* [in] */ const ICanvas* canvas);

    //@Override
    CARAPI SetLayoutParams(
        /* [in] */ const IViewGroupLayoutParams* layoutParams);

    //@Override
    CARAPI PerformLongClick(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ const IConfiguration* newConfig);

    //@Override
    CARAPI OnCreateInputConnection(
        /* [in] */ const IEditorInfo* outAttrs,
        /* [out] */ IInputConnection** connection);

    //@Override
    CARAPI OnKeyMultiple(
        /* [in] */ const Int32 keyCode,
        /* [in] */ const Int32 repeatCount,
        /* [in] */ const IKeyEvent* event
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ const Int32 keyCode,
        /* [in] */ const IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKeyUp(
        /* [in] */ const Int32 keyCode,
        /* [in] */ const IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnAttachedToWindow();

    //@Override
    CARAPI OnDetachedFromWindow();

    //@Override
    CARAPI OnVisibilityChanged(
        /* [in] */ const IView* changedView,
        /* [in] */ const Int32 visibility);

    //@Override
    CARAPI OnWindowFocusChanged(
        /* [in] */ const Boolean hasWindowFocus);

    //@Override
    CARAPI OnFocusChanged(
        /* [in] */ const Boolean focused,
        /* [in] */ const Int32 direction,
        /* [in] */ const IRect* previouslyFocusedRect);

    //@Override
    CARAPI SetFrame(
        /* [in] */ const Int32 left,
        /* [in] */ const Int32 top,
        /* [in] */ const Int32 right,
        /* [in] */ const Int32 bottom,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnSizeChanged(
        /* [in] */ const Int32 w,
        /* [in] */ const Int32 h,
        /* [in] */ const Int32 ow,
        /* [in] */ const Int32 oh);

    //@Override
    CARAPI OnScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

    //@Override
    CARAPI DispatchKeyEvent(
        /* [in] */ const IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ const IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnHoverEvent(
        /* [in] */ const IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnGenericMotionEvent(
        /* [in] */ const IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI RequestFocus(
        /* [in] */ const Int32 direction,
        /* [in] */ const IRect* previouslyFocusedRect,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnMeasure(
        /* [in] */ const Int32 widthMeasureSpec,
        /* [in] */ const Int32 heightMeasureSpec);

    //@Override
    CARAPI RequestChildRectangleOnScreen(
        /* [in] */ const IView* child,
        /* [in] */ const IRect* rect,
        /* [in] */ const Boolean immediate,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI SetBackgroundColor(
        /* [in] */ const Int32 color);

    //@Override
    CARAPI SetLayerType(
        /* [in] */ const Int32 layerType,
        /* [in] */ const IPaint* paint);

    // Remove from superclass
    CARAPI PreDispatchDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI OnStartTemporaryDetach();

    CARAPI OnFinishTemporaryDetach();

    // WebViewProvider.ScrollDelegate implementation ----------------------------------------------

    //@Override
    CARAPI ComputeHorizontalScrollRange(
        /* [in] */ Int32* range);

    //@Override
    CARAPI ComputeHorizontalScrollOffset(
        /* [in] */ Int32* offset);

    //@Override
    CARAPI ComputeVerticalScrollRange(
        /* [in] */ Int32* range);

    //@Override
    CARAPI ComputeVerticalScrollOffset(
        /* [in] */ Int32* offset);

    //@Override
    CARAPI ComputeVerticalScrollExtent(
        /* [in] */ Int32* extent);

    //@Override
    CARAPI ComputeScroll();

    // TODO(sgurun) this is only to have master-gpl compiling.
    CARAPI_(AutoPtr<IPrintDocumentAdapter>) CreatePrintDocumentAdapter();

    //@Override TODO(sgurun) commenting this out to have master-gpl compiling.
    CARAPI_(AutoPtr<IPrintDocumentAdapter>) CreatePrintDocumentAdapter(
        /* [in] */ const String& documentName);

    // Implements SmartClipProvider
    //@Override
    CARAPI ExtractSmartClipData(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // Implements SmartClipProvider
    //@Override
    CARAPI SetSmartClipResultHandler(
        /* [in] */ const IHandler* resultHandler);

public:
    // The WebView that this WebViewChromium is the provider for.
    AutoPtr<IWebView> mWebView;
    // Lets us access protected View-derived methods on the WebView instance we're backing.
    AutoPtr<IWebViewPrivateAccess> mWebViewPrivate;

private:
    CARAPI_(AutoPtr<IInterface>) RunBlockingFuture(
        /* [in] */ IFutureTask* task);

    // We have a 4 second timeout to try to detect deadlocks to detect and aid in debuggin
    // deadlocks.
    // Do not call this method while on the UI thread!
    CARAPI_(void) RunVoidTaskOnUiThreadBlocking(
        /* [in] */ IRunnable* r);

    CARAPI_(AutoPtr<IInterface>) RunOnUiThreadBlocking(
        /* [in] */ ICallable* c);

    // Wrap Context so that we can use resources from the webview resource apk.
    static CARAPI_(AutoPtr<IContext>) ResourcesContextWrapper(
        /* [in] */ const IContext* ctx);

    CARAPI_(void) InitForReal();

//    private RuntimeException createThreadException();

    CARAPI_(Boolean) CheckNeedsPost();

    //  Intentionally not static, as no need to check thread on static methods
    CARAPI_(void) CheckThread();

    static CARAPI_(String) FixupMimeType(
        /* [in] */ const String& mimeType);

    static CARAPI_(String) FixupData(
        /* [in] */ const String& data);

    static CARAPI_(String) FixupBase(
        /* [in] */ const String& url);

    static CARAPI_(String) FixupHistory(
        /* [in] */ const String& url);

    static CARAPI_(Boolean) IsBase64Encoded(
        /* [in] */ const String& encoding);

    CARAPI_(void) LoadUrlOnUiThread(
        /* [in] */ const LoadUrlParams* loadUrlParams);

    /**
     * Returns true if the supplied {@link WebChromeClient} supports fullscreen.
     *
     * <p>For fullscreen support, implementations of {@link WebChromeClient#onShowCustomView}
     * and {@link WebChromeClient#onHideCustomView()} are required.
     */
    CARAPI_(Boolean) DoesSupportFullscreen(
        /* [in] */ IWebChromeClient* client);

private:
    AutoPtr<WebViewChromiumRunQueue> mRunQueue;

    static const String TAG;
    // The client adapter class.
    AutoPtr<WebViewContentsClientAdapter> mContentsClientAdapter;

    // Variables for functionality provided by this adapter ---------------------------------------
    AutoPtr<ContentSettingsAdapter> mWebSettings;
    // The WebView wrapper for ContentViewCore and required browser compontents.
    AutoPtr<AwContents> mAwContents;
    // Non-null if this webview is using the GL accelerated draw path.
    AutoPtr<DrawGLFunctor> mGLfunctor;

    const AutoPtr<IWebViewHitTestResult> mHitTestResult;

    const Int32 mAppTargetSdkVersion;

    private WebViewChromiumFactoryProvider mFactory;

    static Boolean sRecordWholeDocumentEnabledByApi;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCHROMIUM_H__
