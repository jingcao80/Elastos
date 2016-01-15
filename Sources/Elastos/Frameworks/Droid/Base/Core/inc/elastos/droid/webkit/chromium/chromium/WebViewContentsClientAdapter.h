
#ifndef __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCONTENTSCLIENTADAPTER_H__
#define __ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCONTENTSCLIENTADAPTER_H__

// import android.content.ActivityNotFoundException;
// import android.content.Context;
// import android.content.Intent;
// import android.graphics.Bitmap;
// import android.graphics.BitmapFactory;
// import android.graphics.Canvas;
// import android.graphics.Color;
// import android.graphics.Picture;
// import android.net.http.ErrorStrings;
// import android.net.http.SslError;
// import android.net.Uri;
// import android.os.Build;
// import android.os.Handler;
// import android.os.Looper;
// import android.os.Message;
// import android.provider.Browser;
// import android.util.Log;
// import android.view.KeyEvent;
// import android.view.View;
// import android.webkit.ClientCertRequest;
// import android.webkit.ConsoleMessage;
// import android.webkit.DownloadListener;
// import android.webkit.GeolocationPermissions;
// import android.webkit.JsDialogHelper;
// import android.webkit.JsPromptResult;
// import android.webkit.JsResult;
// import android.webkit.PermissionRequest;
// import android.webkit.SslErrorHandler;
// import android.webkit.ValueCallback;
// import android.webkit.WebChromeClient;
// import android.webkit.WebChromeClient.CustomViewCallback;
// import android.webkit.WebResourceResponse;
// import android.webkit.WebResourceRequest;
// import android.webkit.WebView;
// import android.webkit.WebViewClient;

// import org.chromium.android_webview.AwContentsClient;
// import org.chromium.android_webview.AwContentsClientBridge;
// import org.chromium.android_webview.AwHttpAuthHandler;
// import org.chromium.android_webview.AwWebResourceResponse;
// import org.chromium.android_webview.JsPromptResultReceiver;
// import org.chromium.android_webview.JsResultReceiver;
// import org.chromium.android_webview.permission.AwPermissionRequest;
// import org.chromium.base.ThreadUtils;
// import org.chromium.base.TraceEvent;
// import org.chromium.content.browser.ContentView;
// import org.chromium.content.browser.ContentViewClient;

// import java.lang.ref.WeakReference;
// import java.net.URISyntaxException;
// import java.security.Principal;
// import java.security.PrivateKey;
// import java.security.cert.X509Certificate;
// import java.util.ArrayList;
// import java.util.HashMap;
// import java.util.Map;
// import java.util.WeakHashMap;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/**
 * An adapter class that forwards the callbacks from {@link ContentViewClient}
 * to the appropriate {@link WebViewClient} or {@link WebChromeClient}.
 *
 * An instance of this class is associated with one {@link WebViewChromium}
 * instance. A WebViewChromium is a WebView implementation provider (that is
 * android.webkit.WebView delegates all functionality to it) and has exactly
 * one corresponding {@link ContentView} instance.
 *
 * A {@link ContentViewClient} may be shared between multiple {@link ContentView}s,
 * and hence multiple WebViews. Many WebViewClient methods pass the source
 * WebView as an argument. This means that we either need to pass the
 * corresponding ContentView to the corresponding ContentViewClient methods,
 * or use an instance of ContentViewClientAdapter per WebViewChromium, to
 * allow the source WebView to be injected by ContentViewClientAdapter. We
 * choose the latter, because it makes for a cleaner design.
 */
class WebViewContentsClientAdapter
    : public AwContentsClient
{
public:
    // WebViewClassic is coded in such a way that even if a null WebViewClient is set,
    // certain actions take place.
    // We choose to replicate this behavior by using a NullWebViewClient implementation (also known
    // as the Null Object pattern) rather than duplicating the WebViewClassic approach in
    // ContentView.
    class NullWebViewClient
        : public WebViewClient
    {
    public:
        //@Override
        CARAPI ShouldOverrideKeyEvent(
            /* [in] */ IWebView* view,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI ShouldOverrideUrlLoading(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [out] */ Boolean* result);
    };

    // TODO: Move to the upstream once the PermissionRequest is part of SDK.
    class PermissionRequestAdapter
        : public PermissionRequest
    {
    public:
        public PermissionRequestAdapter(AwPermissionRequest awPermissionRequest);

        public static long toAwPermissionResources(String[] resources);

        //@Override
        CARAPI GetOrigin(
            /* [out] */ IUri** uri);

        //@Override
        CARAPI GetResources(
            /* [out] */ ArrayOf<String>** res);

        //@Override
        CARAPI Grant(
            /* [in] */ ArrayOf<String>* resources);

        //@Override
        CARAPI Deny();

    private:
        static CARAPI_(AutoPtr< ArrayOf<String> > ToPermissionResources(
            /* [in] */ Int64 resources);

    private:
        // TODO: Move the below definitions to AwPermissionRequest.
        static Int64 BITMASK_RESOURCE_VIDEO_CAPTURE;
        static Int64 BITMASK_RESOURCE_AUDIO_CAPTURE;
        static Int64 BITMASK_RESOURCE_PROTECTED_MEDIA_ID;

        AutoPtr<AwPermissionRequest> mAwPermissionRequest;
        AutoPtr< ArrayOf<String> > mResources;
    };

private:
    class WebResourceRequestImpl
        : public WebResourceRequest
    {
    public:
        WebResourceRequestImpl(
            /* [in] */ ShouldInterceptRequestParams* params);

        //@Override
        CARAPI GetUrl(
            /* [out] */ IUri** uri);

        //@Override
        CARAPI IsForMainFrame(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI HasGesture(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI getMethod(
            /* [out] */ String* method);

        //@Override
        CARAPI GetRequestHeaders(
            /* [out] */ IMap** headers);

    private:
        const AutoPtr<ShouldInterceptRequestParams> mParams;
    };

    class JsPromptResultReceiverAdapter
        : public IJsResultResultReceiver
    {
    public:
        JsPromptResultReceiverAdapter(
            /* [in] */ IJsPromptResultReceiver* receiver);

        JsPromptResultReceiverAdapter(
            /* [in] */ JsResultReceiver* receiver);

        CARAPI_(AutoPtr<IJsPromptResult>) GetPromptResult();

        //@Override
        CARAPI OnJsResultComplete(
            /* [in] */ IJsResult* result);

    private:
        AutoPtr<IJsPromptResultReceiver> mChromePromptResultReceiver;
        AutoPtr<IJsResultReceiver> mChromeResultReceiver;
        // We hold onto the JsPromptResult here, just to avoid the need to downcast
        // in onJsResultComplete.
        const AutoPtr<IJsPromptResult> mPromptResult;
    };

    class ClientCertRequestImpl
        : public ClientCertRequest
    {
    public:
        ClientCertRequestImpl(
            /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
            /* [in] */ ArrayOf<String>* keyTypes,
            /* [in] */ ArrayOf<IPrincipal>* principals,
            /* [in] */ const String& host,
            /* [in] */ Int32 port);

        //@Override
        CARAPI GetKeyTypes(
            /* [out] */ ArrayOf<String>** keyTypes);

        //@Override
        CARAPI GetPrincipals(
            /* [out] */ ArrayOf<IPrincipal>** principals);

        //@Override
        CARAPI GetHost(
            /* [out] */ String* host);

        //@Override
        CARAPI GetPort(
            /* [out] */ Int32* port);

        //@Override
        CARAPI Proceed(
            /* [in] */ const IPrivateKey* privateKey,
            /* [in] */ const ArrayOf<X509Certificate>* chain);

        //@Override
        CARAPI Ignore();

        //@Override
        CARAPI Cancel();

    public:
        const AutoPtr<AwContentsClientBridge::ClientCertificateRequestCallback> mCallback;
        const AutoPtr< ArrayOf<String> > mKeyTypes;
        const AutoPtr< ArrayOf<IPrincipal> > mPrincipals;
        const String mHost;
        const Int32 mPort;
    };

    // TODO: Move to upstream.
    class AwHttpAuthHandlerAdapter
        : public HttpAuthHandler
    {
    public:
        AwHttpAuthHandlerAdapter(
            /* [in] */ AwHttpAuthHandler* awHandler);

        //@Override
        CARAPI Proceed(
            /* [in] */ const String& username,
            /* [in] */ const String& password);

        //@Override
        CARAPI Cancel();

        //@Override
        CARAPI UseHttpAuthUsernamePassword(
            /* [out] */ Boolean* result);

    private:
        AutoPtr<AwHttpAuthHandler> mAwHandler;
    };

public:
    /**
     * Adapter constructor.
     *
     * @param webView the {@link WebView} instance that this adapter is serving.
     */
    WebViewContentsClientAdapter(
        /* [in] */ IWebView* webView);

    CARAPI_(void) SetWebViewClient(
        /* [in] */ IWebViewClient* client);

    CARAPI_(void) SetWebChromeClient(
        /* [in] */ IWebChromeClient* client);

    CARAPI_(void) SetDownloadListener(
        /* [in] */ DownloadListener* listener);

    CARAPI_(void) SetFindListener(
        /* [in] */ IWebViewFindListener* listener);

    CARAPI_(void) SetPictureListener(
        /* [in] */ IWebViewPictureListener* listener);

    //--------------------------------------------------------------------------------------------
    //                        Adapter for all the methods.
    //--------------------------------------------------------------------------------------------

    /**
     * @see AwContentsClient#getVisitedHistory
     */
    //@Override
    CARAPI_(void) GetVisitedHistory(
        /* [in] */ IValueCallback* callback);

    /**
     * @see AwContentsClient#doUpdateVisiteHistory(String, boolean)
     */
    //@Override
    CARAPI_(void) DoUpdateVisitedHistory(
        /* [in] */ const String& url,
        /* [in] */ Boolean isReload);

    /**
     * @see AwContentsClient#onProgressChanged(int)
     */
    //@Override
    CARAPI_(void) OnProgressChanged(
        /* [in] */ Int32 progress);

    /**
     * @see AwContentsClient#shouldInterceptRequest(java.lang.String)
     */
    //@Override
    CARAPI_(AwWebResourceResponse) ShouldInterceptRequest(
        /* [in] */ ShouldInterceptRequestParams* params);

    /**
     * @see AwContentsClient#shouldOverrideUrlLoading(java.lang.String)
     */
    //@Override
    CARAPI_(Boolean) ShouldOverrideUrlLoading(
        /* [in] */ const String& url);

    /**
     * @see AwContentsClient#onUnhandledKeyEvent(android.view.KeyEvent)
     */
    //@Override
    CARAPI_(void) OnUnhandledKeyEvent(
        /* [in] */ IKeyEvent* event);

    /**
     * @see AwContentsClient#onConsoleMessage(android.webkit.ConsoleMessage)
     */
    //@Override
    CARAPI_(Boolean) OnConsoleMessage(
        /* [in] */ ConsoleMessage consoleMessage);

    /**
     * @see AwContentsClient#onFindResultReceived(int,int,boolean)
     */
    //@Override
    CARAPI_(void) OnFindResultReceived(
        /* [in] */ Int32 activeMatchOrdinal,
        /* [in] */ Int32 numberOfMatches,
        /* [in] */ Boolean isDoneCounting);

    /**
     * @See AwContentsClient#onNewPicture(Picture)
     */
    //@Override
    CARAPI_(void) OnNewPicture(
        /* [in] */ IPicture* picture);

    //@Override
    CARAPI_(void) OnLoadResource(
        /* [in] */ const String& url);

    //@Override
    CARAPI_(Boolean) OnCreateWindow(
        /* [in] */ Boolean isDialog,
        /* [in] */ Boolean isUserGesture);

    /**
     * @see AwContentsClient#onCloseWindow()
     */
    //@Override
    CARAPI_(void) OnCloseWindow();

    /**
     * @see AwContentsClient#onRequestFocus()
     */
    //@Override
    CARAPI_(void) OnRequestFocus();

    /**
     * @see AwContentsClient#onReceivedTouchIconUrl(String url, boolean precomposed)
     */
    //@Override
    CARAPI_(void) OnReceivedTouchIconUrl(
        /* [in] */ const String& url,
        /* [in] */ Boolean precomposed);

    /**
     * @see AwContentsClient#onReceivedIcon(Bitmap bitmap)
     */
    //@Override
    CARAPI_(void) OnReceivedIcon(
        /* [in] */ IBitmap* bitmap);

    /**
     * @see ContentViewClient#onPageStarted(String)
     */
    //@Override
    CARAPI_(void) OnPageStarted(
        /* [in] */ const String& url);

    /**
     * @see ContentViewClient#onPageFinished(String)
     */
    //@Override
    CARAPI_(void) OnPageFinished(
        /* [in] */ const String& url);

    /**
     * @see ContentViewClient#onReceivedError(int,String,String)
     */
    //@Override
    CARAPI_(void) OnReceivedError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& description,
        /* [in] */ const String& failingUrl);

    /**
     * @see ContentViewClient#onReceivedTitle(String)
     */
    //@Override
    CARAPI_(void) OnReceivedTitle(
        /* [in] */ const String& title);

    /**
     * @see ContentViewClient#shouldOverrideKeyEvent(KeyEvent)
     */
    //@Override
    CARAPI_(Boolean) ShouldOverrideKeyEvent(
        /* [in] */ IKeyEvent* event);

    /**
     * @see ContentViewClient#onStartContentIntent(Context, String)
     * Callback when detecting a click on a content link.
     */
    // TODO: Delete this method when removed from base class.
    CARAPI_(void) OnStartContentIntent(
        /* [in] */ IContext* context,
        /* [in] */ const String& contentUrl);

    //@Override
    CARAPI_(void) OnGeolocationPermissionsShowPrompt(
        /* [in] */ const String& origin,
        /* [in] */ IGeolocationPermissionsCallback* callback);

    //@Override
    CARAPI_(void) OnGeolocationPermissionsHidePrompt();

    //@Override
    CARAPI_(void) OnPermissionRequest(
        /* [in] */ AwPermissionRequest* permissionRequest);

    //@Override
    CARAPI_(void) OnPermissionRequestCanceled(
        /* [in] */ AwPermissionRequest* permissionRequest);

    //@Override
    CARAPI_(void) HandleJsAlert(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ IJsResultReceiver* receiver);

    //@Override
    CARAPI_(void) HandleJsBeforeUnload(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ IJsResultReceiver* receiver);

    //@Override
    CARAPI_(void) HandleJsConfirm(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ IJsResultReceiver* receiver);

    //@Override
    CARAPI_(void) HandleJsPrompt(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ const String& defaultValue,
        /* [in] */ IJsPromptResultReceiver* receiver);

    //@Override
    CARAPI_(void) OnReceivedHttpAuthRequest(
        /* [in] */ AwHttpAuthHandler* handler,
        /* [in] */ const String& host,
        /* [in] */ const String& realm);

    //@Override
    CARAPI_(void) OnReceivedSslError(
        /* [in] */ const IValueCallback* callback,
        /* [in] */ ISslError* error);

    //@Override
    CARAPI_(void) OnReceivedClientCertRequest(
        /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal>* principals,
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    //@Override
    CARAPI_(void) OnReceivedLoginRequest(
        /* [in] */ const String& realm,
        /* [in] */ const String& account,
        /* [in] */ const String& args);

    //@Override
    CARAPI_(void) OnFormResubmission(
        /* [in] */ IMessage* dontResend,
        /* [in] */ IMessage* resend);

    //@Override
    CARAPI_(void) OnDownloadStart(
        /* [in] */ const String& url,
        /* [in] */ const String& userAgent,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [in] */ Int64 contentLength);

    //@Override
    CARAPI_(void) ShowFileChooser(
        /* [in] */ const IValueCallback* uploadFileCallback,
        /* [in] */ const AwContentsClient::FileChooserParams* fileChooserParams);

    //@Override
    CARAPI_(void) OnScaleChangedScaled(
        /* [in] */ Float oldScale,
        /* [in] */ Float newScale);

    //@Override
    CARAPI_(void) OnShowCustomView(
        /* [in] */ IView* view,
        /* [in] */ ICustomViewCallback* cb);

    //@Override
    CARAPI_(void) OnHideCustomView();

    //@Override
    CARAPI_(AutoPtr<IBitmap>) GetDefaultVideoPoster();

protected:
    //@Override
    CARAPI_(AutoPtr<IView>) GetVideoLoadingProgressView();

private:
    // TAG is chosen for consistency with classic webview tracing.
    static const String;
    // Enables API callback tracing
    static const Boolean TRACE = android.webkit.DebugFlags.TRACE_CALLBACK;
    // The WebView instance that this adapter is serving.
    const AutoPtr<IWebView> mWebView;
    // The WebViewClient instance that was passed to WebView.setWebViewClient().
    AutoPtr<IWebViewClient> mWebViewClient;
    // The WebChromeClient instance that was passed to WebView.setContentViewClient().
    AutoPtr<IWebChromeClient> mWebChromeClient;
    // The listener receiving find-in-page API results.
    AutoPtr<IWebViewFindListener> mFindListener;
    // The listener receiving notifications of screen updates.
    AutoPtr<IWebViewPictureListener> mPictureListener;

    AutoPtr<DownloadListener> mDownloadListener;

    AutoPtr<IHandler> mUiThreadHandler;

    static const Int32 NEW_WEBVIEW_CREATED = 100;

    WeakHashMap<AwPermissionRequest, WeakReference<PermissionRequestAdapter>>
            mOngoingPermissionRequests;
};

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_CHROMIUM_CHROMIUM_WEBVIEWCONTENTSCLIENTADAPTER_H__
