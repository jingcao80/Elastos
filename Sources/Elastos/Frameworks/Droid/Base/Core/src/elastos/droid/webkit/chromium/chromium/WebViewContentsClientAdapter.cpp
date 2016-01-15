
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

//===============================================================
//        WebViewContentsClientAdapter::NullWebViewClient
//===============================================================

//@Override
ECode WebViewContentsClientAdapter::NullWebViewClientShouldOverrideKeyEvent(
    /* [in] */ IWebView* view,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
#if 0
    // TODO: Investigate more and add a test case.
    // This is reflecting Clank's behavior.
    int keyCode = event.getKeyCode();
    return !ContentViewClient.shouldPropagateKey(keyCode);
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewContentsClientAdapter::NullWebViewClientShouldOverrideUrlLoading(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
#if 0
    Intent intent;
    // Perform generic parsing of the URI to turn it into an Intent.
    try {
        intent = Intent.parseUri(url, Intent.URI_INTENT_SCHEME);
    } catch (URISyntaxException ex) {
        Log.w(TAG, "Bad URI " + url + ": " + ex.getMessage());
        return false;
    }
    // Sanitize the Intent, ensuring web pages can not bypass browser
    // security (only access to BROWSABLE activities).
    intent.addCategory(Intent.CATEGORY_BROWSABLE);
    intent.setComponent(null);
    Intent selector = intent.getSelector();
    if (selector != null) {
        selector.addCategory(Intent.CATEGORY_BROWSABLE);
        selector.setComponent(null);
    }
    // Pass the package name as application ID so that the intent from the
    // same application can be opened in the same tab.
    intent.putExtra(Browser.EXTRA_APPLICATION_ID,
            view.getContext().getPackageName());
    try {
        view.getContext().startActivity(intent);
    } catch (ActivityNotFoundException ex) {
        Log.w(TAG, "No application can handle " + url);
        return false;
    }
    return true;
#endif
    return E_NOT_IMPLEMENTED;
}

//===============================================================
//      WebViewContentsClientAdapter::WebResourceRequestImpl
//===============================================================

private static class WebResourceRequestImpl implements WebResourceRequest {
    private final ShouldInterceptRequestParams mParams;

WebViewContentsClientAdapter::WebResourceRequestImpl::WebResourceRequestImpl(
    /* [in] */ ShouldInterceptRequestParams* params)
    : mParams(params)
{
}

//@Override
ECode WebViewContentsClientAdapter::WebResourceRequestImpl::GetUrl(
    /* [out] */ IUri** uri)
{
#if 0
    return Uri.parse(mParams.url);
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewContentsClientAdapter::WebResourceRequestImpl::IsForMainFrame(
    /* [out] */ Boolean* result)
{
#if 0
    return mParams.isMainFrame;
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewContentsClientAdapter::WebResourceRequestImpl::HasGesture(
    /* [out] */ Boolean* result)
{
#if 0
    return mParams.hasUserGesture;
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewContentsClientAdapter::WebResourceRequestImpl::GetMethod(
    /* [out] */ String* method)
{
#if 0
    return mParams.method;
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewContentsClientAdapter::WebResourceRequestImpl::GetRequestHeaders(
    /* [out] */ IMap** headers)
{
#if 0
    return mParams.requestHeaders;
#endif
    return E_NOT_IMPLEMENTED;
}

//========================================================================
//       WebViewContentsClientAdapter::JsPromptResultReceiverAdapter
//========================================================================

WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::JsPromptResultReceiverAdapter(
    /* [in] */ IJsPromptResultReceiver* receiver)
    : mChromePromptResultReceiver(receiver)
{
}

WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::JsPromptResultReceiverAdapter(
    /* [in] */ IJsResultReceiver* receiver)
    : mChromeResultReceiver(receiver)
{
}

AutoPtr<JsPromptResult> WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::GetPromptResult()
{
    return mPromptResult;
}

//@Override
ECode WebViewContentsClientAdapter::JsPromptResultReceiverAdapter::OnJsResultComplete(
    /* [in] */ IJsResult* result)
{
#if 0
    if (mChromePromptResultReceiver != null) {
        if (mPromptResult.getResult()) {
            mChromePromptResultReceiver.confirm(mPromptResult.getStringResult());
        } else {
            mChromePromptResultReceiver.cancel();
        }
    } else {
        if (mPromptResult.getResult()) {
            mChromeResultReceiver.confirm();
        } else {
            mChromeResultReceiver.cancel();
        }
    }
#endif
    return E_NOT_IMPLEMENTED;
}

//========================================================================
//         WebViewContentsClientAdapter::ClientCertRequestImpl
//========================================================================

WebViewContentsClientAdapter::ClientCertRequestImpl::ClientCertRequestImpl(
    /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal>* principals,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
    : mCallback(callback)
    , mKeyTypes(keyTypes)
    , mPrincipals(principals)
    , mHost(host)
    , mPort(port)
{
}

//@Override
ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetKeyTypes(
    /* [out] */ ArrayOf<String>** keyTypes)
{
    VALIDATE_NOT_NULL(keyTypes);
    // This is already a copy of native argument, so return directly.
    *keyTypes = mKeyTypes;
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetPrincipals(
    /* [out] */ ArrayOf<IPrincipal>** principals)
{
    VALIDATE_NOT_NULL(principals);
    // This is already a copy of native argument, so return directly.
    *principals = mPrincipals;
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host);
    *host = mHost;
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::ClientCertRequestImpl::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = mPort;
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::ClientCertRequestImpl::Proceed(
    /* [in] */ const IPrivateKey* privateKey,
    /* [in] */ const ArrayOf<X509Certificate>* chain)
{
    mCallback->Proceed(privateKey, chain);
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::ClientCertRequestImpl::Ignore()
{
    mCallback->Ignore();
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::ClientCertRequestImpl::Cancel()
{
    mCallback->Cancel();
    return NOERROR;
}

//========================================================================
//         WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter
//========================================================================

// TODO: Move to upstream.
private static class AwHttpAuthHandlerAdapter extends android.webkit.HttpAuthHandler {
    private AwHttpAuthHandler mAwHandler;

WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::AwHttpAuthHandlerAdapter(
    /* [in] */ AwHttpAuthHandler* awHandler)
    : mAwHandler(awHandler)
{
}

//@Override
ECode WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::Proceed(
    /* [in] */ const String& username,
    /* [in] */ const String& password)
{
    if (username == NULL) {
        username = "";
    }

    if (password == NULL) {
        password = "";
    }

    mAwHandler->Proceed(username, password);

    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::Cancel()
{
    mAwHandler->Cancel();
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::AwHttpAuthHandlerAdapter::UseHttpAuthUsernamePassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAwHandler->IsFirstAttempt();
    return NOERROR;
}

//========================================================================
//         WebViewContentsClientAdapter::PermissionRequestAdapter
//========================================================================

// TODO: Move the below definitions to AwPermissionRequest.
Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::BITMASK_RESOURCE_VIDEO_CAPTURE = 1 << 1;
Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::BITMASK_RESOURCE_AUDIO_CAPTURE = 1 << 2;
Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::BITMASK_RESOURCE_PROTECTED_MEDIA_ID = 1 << 3;

WebViewContentsClientAdapter::PermissionRequestAdapter::PermissionRequestAdapter(
    /* [in] */ AwPermissionRequest* awPermissionRequest)
{
    assert(awPermissionRequest != NULL);
    mAwPermissionRequest = awPermissionRequest;

}


Int64 WebViewContentsClientAdapter::PermissionRequestAdapter::ToAwPermissionResources(
    /* [in] */ ArrayOf<String>* resources)
{
#if 0
    long result = 0;
    for (String resource : resources) {
        if (resource.equals(PermissionRequest.RESOURCE_VIDEO_CAPTURE))
            result |= BITMASK_RESOURCE_VIDEO_CAPTURE;
        else if (resource.equals(PermissionRequest.RESOURCE_AUDIO_CAPTURE))
            result |= BITMASK_RESOURCE_AUDIO_CAPTURE;
        else if (resource.equals(PermissionRequest.RESOURCE_PROTECTED_MEDIA_ID))
            result |= BITMASK_RESOURCE_PROTECTED_MEDIA_ID;
    }
    return result;
#endif
    return -1;
}

AutoPtr< ArrayOf<String> > WebViewContentsClientAdapter::PermissionRequestAdapter::ToPermissionResources(
    /* [in] */ Int64 resources)
{
#if 0
    ArrayList<String> result = new ArrayList<String>();
    if ((resources & BITMASK_RESOURCE_VIDEO_CAPTURE) != 0)
        result.add(PermissionRequest.RESOURCE_VIDEO_CAPTURE);
    if ((resources & BITMASK_RESOURCE_AUDIO_CAPTURE) != 0)
        result.add(PermissionRequest.RESOURCE_AUDIO_CAPTURE);
    if ((resources & BITMASK_RESOURCE_PROTECTED_MEDIA_ID) != 0)
        result.add(PermissionRequest.RESOURCE_PROTECTED_MEDIA_ID);
    String[] resource_array = new String[result.size()];
    return result.toArray(resource_array);
#endif
    return NULL;
}

//@Override
ECode WebViewContentsClientAdapter::PermissionRequestAdapter::GetOrigin(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = mAwPermissionRequest->GetOrigin();
    return NOERROR;
}

//@Override
ECode WebViewContentsClientAdapter::PermissionRequestAdapter::GetResources(
    /* [out] */ ArrayOf<String>** res)
{
#if 0
    synchronized(this) {
        if (mResources == null) {
            mResources = toPermissionResources(mAwPermissionRequest.getResources());
        }
        return mResources;
    }
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewContentsClientAdapter::PermissionRequestAdapter::Grant(
    /* [in] */ ArrayOf<String>* resources)
{
#if 0
    long requestedResource = mAwPermissionRequest.getResources();
    if ((requestedResource & toAwPermissionResources(resources)) == requestedResource)
        mAwPermissionRequest.grant();
    else
        mAwPermissionRequest.deny();
#endif
    return E_NOT_IMPLEMENTED;
}

//@Override
ECode WebViewContentsClientAdapter::PermissionRequestAdapter::Deny()
{
    mAwPermissionRequest->Deny();
    return NOERROR;
}

//===============================================================
//                WebViewContentsClientAdapter
//===============================================================

// TAG is chosen for consistency with classic webview tracing.
const String WebViewContentsClientAdapter::TAG("WebViewCallback");
// Enables API callback tracing
const Boolean WebViewContentsClientAdapter::TRACE = android.webkit.DebugFlags.TRACE_CALLBACK;

const Int32 WebViewContentsClientAdapter::NEW_WEBVIEW_CREATED;

/**
 * Adapter constructor.
 *
 * @param webView the {@link WebView} instance that this adapter is serving.
 */
WebViewContentsClientAdapter::WebViewContentsClientAdapter(
    /* [in] */ IWebView* webView)
{
#if 0
    if (webView == null) {
        throw new IllegalArgumentException("webView can't be null");
    }

    mWebView = webView;
    setWebViewClient(null);

    mUiThreadHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            switch(msg.what) {
                case NEW_WEBVIEW_CREATED:
                    WebView.WebViewTransport t = (WebView.WebViewTransport) msg.obj;
                    WebView newWebView = t.getWebView();
                    if (newWebView == mWebView) {
                        throw new IllegalArgumentException(
                                "Parent WebView cannot host it's own popup window. Please " +
                                "use WebSettings.setSupportMultipleWindows(false)");
                    }

                    if (newWebView != null && newWebView.copyBackForwardList().getSize() != 0) {
                        throw new IllegalArgumentException(
                                "New WebView for popup window must not have been previously " +
                                "navigated.");
                    }

                    WebViewChromium.completeWindowCreation(mWebView, newWebView);
                    break;
                default:
                    throw new IllegalStateException();
            }
        }
    };
#endif
}



void WebViewContentsClientAdapter::SetWebViewClient(
    /* [in] */ IWebViewClient* client)
{
    if (client != NULL) {
        mWebViewClient = client;
    }
    else {
        mWebViewClient = new NullWebViewClient();
    }
}

void WebViewContentsClientAdapter::SetWebChromeClient(
    /* [in] */ IWebChromeClient* client)
{
    mWebChromeClient = client;
}

void WebViewContentsClientAdapter::SetDownloadListener(
    /* [in] */ DownloadListener* listener)
{
    mDownloadListener = listener;
}

void WebViewContentsClientAdapter::SetFindListener(
    /* [in] */ IWebViewFindListener* listener)
{
    mFindListener = listener;
}

void WebViewContentsClientAdapter::SetPictureListener(
    /* [in] */ IWebViewPictureListener* listener)
{
    mPictureListener = listener;
}

//--------------------------------------------------------------------------------------------
//                        Adapter for all the methods.
//--------------------------------------------------------------------------------------------

/**
 * @see AwContentsClient#getVisitedHistory
 */
//@Override
void WebViewContentsClientAdapter::GetVisitedHistory(
    /* [in] */ IValueCallback* callback)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "getVisitedHistory");
        mWebChromeClient.getVisitedHistory(callback);
    }
    TraceEvent.end();
#endif
}

/**
 * @see AwContentsClient#doUpdateVisiteHistory(String, boolean)
 */
//@Override
void WebViewContentsClientAdapter::DoUpdateVisitedHistory(
    /* [in] */ const String& url,
    /* [in] */ Boolean isReload)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "doUpdateVisitedHistory=" + url + " reload=" + isReload);
    mWebViewClient.doUpdateVisitedHistory(mWebView, url, isReload);
    TraceEvent.end();
#endif
}

/**
 * @see AwContentsClient#onProgressChanged(int)
 */
//@Override
void WebViewContentsClientAdapter::OnProgressChanged(
    /* [in] */ Int32 progress)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onProgressChanged=" + progress);
        mWebChromeClient.onProgressChanged(mWebView, progress);
    }
    TraceEvent.end();
#endif
}

/**
 * @see AwContentsClient#shouldInterceptRequest(java.lang.String)
 */
//@Override
AutoPtr<AwWebResourceResponse> WebViewContentsClientAdapter::ShouldInterceptRequest(
    /* [in] */ ShouldInterceptRequestParams* params)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "shouldInterceptRequest=" + params.url);
    WebResourceResponse response = mWebViewClient.shouldInterceptRequest(mWebView,
            new WebResourceRequestImpl(params));
    TraceEvent.end();
    if (response == null) return null;

    // AwWebResourceResponse should support null headers. b/16332774.
    Map<String, String> responseHeaders = response.getResponseHeaders();
    if (responseHeaders == null)
        responseHeaders = new HashMap<String, String>();

    return new AwWebResourceResponse(
            response.getMimeType(),
            response.getEncoding(),
            response.getData(),
            response.getStatusCode(),
            response.getReasonPhrase(),
            responseHeaders);
#endif
    return NULL;
}

/**
 * @see AwContentsClient#shouldOverrideUrlLoading(java.lang.String)
 */
//@Override
Boolean WebViewContentsClientAdapter::ShouldOverrideUrlLoading(
    /* [in] */ const String& url)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "shouldOverrideUrlLoading=" + url);
    boolean result = mWebViewClient.shouldOverrideUrlLoading(mWebView, url);
    TraceEvent.end();
    return result;
#endif
    return FALSE;
}

/**
 * @see AwContentsClient#onUnhandledKeyEvent(android.view.KeyEvent)
 */
//@Override
void WebViewContentsClientAdapter::OnUnhandledKeyEvent(
    /* [in] */ IKeyEvent event)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onUnhandledKeyEvent");
    mWebViewClient.onUnhandledKeyEvent(mWebView, event);
    TraceEvent.end();
#endif
}

/**
 * @see AwContentsClient#onConsoleMessage(android.webkit.ConsoleMessage)
 */
//@Override
Boolean WebViewContentsClientAdapter::OnConsoleMessage(
    /* [in] */ ConsoleMessage* consoleMessage)
{
#if 0
    TraceEvent.begin();
    boolean result;
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onConsoleMessage: " + consoleMessage.message());
        result = mWebChromeClient.onConsoleMessage(consoleMessage);
        String message = consoleMessage.message();
        if (result && message != null && message.startsWith("[blocked]")) {
            Log.e(TAG, "Blocked URL: " + message);
        }
    } else {
        result = false;
    }
    TraceEvent.end();
    return result;
#endif
    return FALSE;
}

/**
 * @see AwContentsClient#onFindResultReceived(int,int,boolean)
 */
//@Override
void WebViewContentsClientAdapter::OnFindResultReceived(
    /* [in] */ Int32 activeMatchOrdinal,
    /* [in] */ Int32 numberOfMatches,
    /* [in] */ Boolean isDoneCounting)
{
#if 0
    if (mFindListener == null) return;
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onFindResultReceived");
    mFindListener.onFindResultReceived(activeMatchOrdinal, numberOfMatches, isDoneCounting);
    TraceEvent.end();
#endif
}

/**
 * @See AwContentsClient#onNewPicture(Picture)
 */
//@Override
void WebViewContentsClientAdapter::OnNewPicture(
    /* [in] */ IPicture* picture)
{
#if 0
    if (mPictureListener == null) return;
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onNewPicture");
    mPictureListener.onNewPicture(mWebView, picture);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnLoadResource(
    /* [in] */ const String& url)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onLoadResource=" + url);
    mWebViewClient.onLoadResource(mWebView, url);
    TraceEvent.end();
#endif
}

//@Override
Boolean WebViewContentsClientAdapter::OnCreateWindow(
    /* [in] */ Boolean isDialog,
    /* [in] */ Boolean isUserGesture)
{
#if 0
    Message m = mUiThreadHandler.obtainMessage(
            NEW_WEBVIEW_CREATED, mWebView.new WebViewTransport());
    TraceEvent.begin();
    boolean result;
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onCreateWindow");
        result = mWebChromeClient.onCreateWindow(mWebView, isDialog, isUserGesture, m);
    } else {
        result = false;
    }
    TraceEvent.end();
    return result;
#endif
    return FALSE;
}

/**
 * @see AwContentsClient#onCloseWindow()
 */
//@Override
void WebViewContentsClientAdapter::OnCloseWindow()
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onCloseWindow");
        mWebChromeClient.onCloseWindow(mWebView);
    }
    TraceEvent.end();
#endif
}

/**
 * @see AwContentsClient#onRequestFocus()
 */
//@Override
void WebViewContentsClientAdapter::OnRequestFocus()
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onRequestFocus");
        mWebChromeClient.onRequestFocus(mWebView);
    }
    TraceEvent.end();
#endif
}

/**
 * @see AwContentsClient#onReceivedTouchIconUrl(String url, boolean precomposed)
 */
//@Override
void WebViewContentsClientAdapter::OnReceivedTouchIconUrl(
    /* [in] */ const String& url,
    /* [in] */ Boolean precomposed)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onReceivedTouchIconUrl=" + url);
        mWebChromeClient.onReceivedTouchIconUrl(mWebView, url, precomposed);
    }
    TraceEvent.end();
#endif
}

/**
 * @see AwContentsClient#onReceivedIcon(Bitmap bitmap)
 */
//@Override
void WebViewContentsClientAdapter::OnReceivedIcon(
    /* [in] */ IBitmap* bitmap)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onReceivedIcon");
        mWebChromeClient.onReceivedIcon(mWebView, bitmap);
    }
    TraceEvent.end();
#endif
}

/**
 * @see ContentViewClient#onPageStarted(String)
 */
//@Override
void WebViewContentsClientAdapter::OnPageStarted(
    /* [in] */ const String& url)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onPageStarted=" + url);
    mWebViewClient.onPageStarted(mWebView, url, mWebView.getFavicon());
    TraceEvent.end();
#endif
}

/**
 * @see ContentViewClient#onPageFinished(String)
 */
//@Override
void WebViewContentsClientAdapter::OnPageFinished(
    /* [in] */ const String& url)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onPageFinished=" + url);
    mWebViewClient.onPageFinished(mWebView, url);
    TraceEvent.end();

    // See b/8208948
    // This fakes an onNewPicture callback after onPageFinished to allow
    // CTS tests to run in an un-flaky manner. This is required as the
    // path for sending Picture updates in Chromium are decoupled from the
    // page loading callbacks, i.e. the Chrome compositor may draw our
    // content and send the Picture before onPageStarted or onPageFinished
    // are invoked. The CTS harness discards any pictures it receives before
    // onPageStarted is invoked, so in the case we get the Picture before that and
    // no further updates after onPageStarted, we'll fail the test by timing
    // out waiting for a Picture.
    // To ensure backwards compatibility, we need to defer sending Picture updates
    // until onPageFinished has been invoked. This work is being done
    // upstream, and we can revert this hack when it lands.
    if (mPictureListener != null) {
        ThreadUtils.postOnUiThreadDelayed(new Runnable() {
            @Override
            public void run() {
                UnimplementedWebViewApi.invoke();
                if (mPictureListener != null) {
                    if (TRACE) Log.d(TAG, "onPageFinished-fake");
                    mPictureListener.onNewPicture(mWebView, new Picture());
                }
            }
        }, 100);
    }
#endif
}

/**
 * @see ContentViewClient#onReceivedError(int,String,String)
 */
//@Override
void WebViewContentsClientAdapter::OnReceivedError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
#if 0
    if (description == null || description.isEmpty()) {
        // ErrorStrings is @hidden, so we can't do this in AwContents.
        // Normally the net/ layer will set a valid description, but for synthesized callbacks
        // (like in the case for intercepted requests) AwContents will pass in null.
        description = ErrorStrings.getString(errorCode, mWebView.getContext());
    }
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onReceivedError=" + failingUrl);
    mWebViewClient.onReceivedError(mWebView, errorCode, description, failingUrl);
    TraceEvent.end();
#endif
}

/**
 * @see ContentViewClient#onReceivedTitle(String)
 */
//@Override
void WebViewContentsClientAdapter::OsnReceivedTitle(
    /* [in] */ const String& title)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onReceivedTitle");
        mWebChromeClient.onReceivedTitle(mWebView, title);
    }
    TraceEvent.end();
#endif
}


/**
 * @see ContentViewClient#shouldOverrideKeyEvent(KeyEvent)
 */
//@Override
Boolean WebViewContentsClientAdapter::ShouldOverrideKeyEvent(
    /* [in] */ IKeyEvent* event)
{
#if 0
    // The check below is reflecting Clank's behavior and is a workaround for http://b/7697782.
    // 1. The check for system key should be made in AwContents or ContentViewCore, before
    //    shouldOverrideKeyEvent() is called at all.
    // 2. shouldOverrideKeyEvent() should be called in onKeyDown/onKeyUp, not from
    //    dispatchKeyEvent().
    if (!ContentViewClient.shouldPropagateKey(event.getKeyCode())) return true;
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "shouldOverrideKeyEvent");
    boolean result = mWebViewClient.shouldOverrideKeyEvent(mWebView, event);
    TraceEvent.end();
    return result;
#endif

    return FALSE;
}


/**
 * @see ContentViewClient#onStartContentIntent(Context, String)
 * Callback when detecting a click on a content link.
 */
// TODO: Delete this method when removed from base class.
void WebViewContentsClientAdapter::OnStartContentIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& contentUrl)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "shouldOverrideUrlLoading=" + contentUrl);
    mWebViewClient.shouldOverrideUrlLoading(mWebView, contentUrl);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnGeolocationPermissionsShowPrompt(
    /* [in] */ const String& origin,
    /* [in] */ IGeolocationPermissionsCallback* callback)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onGeolocationPermissionsShowPrompt");
        mWebChromeClient.onGeolocationPermissionsShowPrompt(origin, callback);
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnGeolocationPermissionsHidePrompt()
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onGeolocationPermissionsHidePrompt");
        mWebChromeClient.onGeolocationPermissionsHidePrompt();
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnPermissionRequest(
    /* [in] */ AwPermissionRequest* permissionRequest)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onPermissionRequest");
        if (mOngoingPermissionRequests == null) {
            mOngoingPermissionRequests =
                new WeakHashMap<AwPermissionRequest, WeakReference<PermissionRequestAdapter>>();
        }
        PermissionRequestAdapter adapter = new PermissionRequestAdapter(permissionRequest);
        mOngoingPermissionRequests.put(
                permissionRequest, new WeakReference<PermissionRequestAdapter>(adapter));
        mWebChromeClient.onPermissionRequest(adapter);
    } else {
        // By default, we deny the permission.
        permissionRequest.deny();
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnPermissionRequestCanceled(
    /* [in] */ AwPermissionRequest* permissionRequest)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null && mOngoingPermissionRequests != null) {
        if (TRACE) Log.d(TAG, "onPermissionRequestCanceled");
        WeakReference<PermissionRequestAdapter> weakRef =
                mOngoingPermissionRequests.get(permissionRequest);
        // We don't hold strong reference to PermissionRequestAdpater and don't expect the
        // user only holds weak reference to it either, if so, user has no way to call
        // grant()/deny(), and no need to be notified the cancellation of request.
        if (weakRef != null) {
            PermissionRequestAdapter adapter = weakRef.get();
            if (adapter != null) mWebChromeClient.onPermissionRequestCanceled(adapter);
        }
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::HandleJsAlert(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResultReceiver receiver)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        final JsPromptResult res =
                new JsPromptResultReceiverAdapter(receiver).getPromptResult();
        if (TRACE) Log.d(TAG, "onJsAlert");
        if (!mWebChromeClient.onJsAlert(mWebView, url, message, res)) {
            new JsDialogHelper(res, JsDialogHelper.ALERT, null, message, url)
                    .showDialog(mWebView.getContext());
        }
    } else {
        receiver.cancel();
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::HandleJsBeforeUnload(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResultReceiver* receiver)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        final JsPromptResult res =
                new JsPromptResultReceiverAdapter(receiver).getPromptResult();
        if (TRACE) Log.d(TAG, "onJsBeforeUnload");
        if (!mWebChromeClient.onJsBeforeUnload(mWebView, url, message, res)) {
            new JsDialogHelper(res, JsDialogHelper.UNLOAD, null, message, url)
                    .showDialog(mWebView.getContext());
        }
    } else {
        receiver.cancel();
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::HandleJsConfirm(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResultReceiver* receiver)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        final JsPromptResult res =
                new JsPromptResultReceiverAdapter(receiver).getPromptResult();
        if (TRACE) Log.d(TAG, "onJsConfirm");
        if (!mWebChromeClient.onJsConfirm(mWebView, url, message, res)) {
            new JsDialogHelper(res, JsDialogHelper.CONFIRM, null, message, url)
                    .showDialog(mWebView.getContext());
        }
    } else {
        receiver.cancel();
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::HandleJsPrompt(
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ const String& defaultValue,
    /* [in] */ IJsPromptResultReceiver* receiver)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        final JsPromptResult res =
                new JsPromptResultReceiverAdapter(receiver).getPromptResult();
        if (TRACE) Log.d(TAG, "onJsPrompt");
        if (!mWebChromeClient.onJsPrompt(mWebView, url, message, defaultValue, res)) {
            new JsDialogHelper(res, JsDialogHelper.PROMPT, defaultValue, message, url)
                    .showDialog(mWebView.getContext());
        }
    } else {
        receiver.cancel();
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnReceivedHttpAuthRequest(
    /* [in] */ AwHttpAuthHandler* handler,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onReceivedHttpAuthRequest=" + host);
    mWebViewClient.onReceivedHttpAuthRequest(mWebView,
            new AwHttpAuthHandlerAdapter(handler), host, realm);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnReceivedSslError(
    /* [in] */ const IValueCallback* callback,
    /* [in] */ ISslError* error)
{
#if 0
    SslErrorHandler handler = new SslErrorHandler() {
        @Override
        public void proceed() {
            callback.onReceiveValue(true);
        }
        @Override
        public void cancel() {
            callback.onReceiveValue(false);
        }
    };
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onReceivedSslError");
    mWebViewClient.onReceivedSslError(mWebView, handler, error);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnReceivedClientCertRequest(
    /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal>* principals,
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
{
#if 0
    if (TRACE) Log.d(TAG, "onReceivedClientCertRequest");
    TraceEvent.begin();
    final ClientCertRequestImpl request = new ClientCertRequestImpl(callback,
        keyTypes, principals, host, port);
    mWebViewClient.onReceivedClientCertRequest(mWebView, request);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnReceivedLoginRequest(
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onReceivedLoginRequest=" + realm);
    mWebViewClient.onReceivedLoginRequest(mWebView, realm, account, args);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnFormResubmission(
    /* [in] */ IMessage* dontResend,
    /* [in] */ IMessage* resend)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, "onFormResubmission");
    mWebViewClient.onFormResubmission(mWebView, dontResend, resend);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnDownloadStart(
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 contentLength)
{
#if 0
    if (mDownloadListener != null) {
        TraceEvent.begin();
        if (TRACE) Log.d(TAG, "onDownloadStart");
        mDownloadListener.onDownloadStart(url,
                                          userAgent,
                                          contentDisposition,
                                          mimeType,
                                          contentLength);
        TraceEvent.end();
    }
#endif
}

//@Override
void WebViewContentsClientAdapter::ShowFileChooser(
    /* [in] */ const IValueCallback* uploadFileCallback,
    /* [in] */ const AwContentsClient::FileChooserParams* fileChooserParams)
{
#if 0
    if (mWebChromeClient == null) {
        uploadFileCallback.onReceiveValue(null);
        return;
    }
    TraceEvent.begin();
    FileChooserParamsAdapter adapter = new FileChooserParamsAdapter(
            fileChooserParams, mWebView.getContext());
    if (TRACE) Log.d(TAG, "showFileChooser");
    ValueCallback<Uri[]> callbackAdapter = new ValueCallback<Uri[]>() {
        private boolean mCompleted;
        @Override
        public void onReceiveValue(Uri[] uriList) {
            if (mCompleted) {
                throw new IllegalStateException("showFileChooser result was already called");
            }
            mCompleted = true;
            String s[] = null;
            if (uriList != null) {
                s = new String[uriList.length];
                for(int i = 0; i < uriList.length; i++) {
                    s[i] = uriList[i].toString();
                }
            }
            uploadFileCallback.onReceiveValue(s);
        }
    };
    if (mWebChromeClient.onShowFileChooser(mWebView, callbackAdapter, adapter)) {
        return;
    }
    if (mWebView.getContext().getApplicationInfo().targetSdkVersion >
            Build.VERSION_CODES.KITKAT) {
        uploadFileCallback.onReceiveValue(null);
        return;
    }
    ValueCallback<Uri> innerCallback = new ValueCallback<Uri>() {
        private boolean mCompleted;
        @Override
        public void onReceiveValue(Uri uri) {
            if (mCompleted) {
                throw new IllegalStateException("showFileChooser result was already called");
            }
            mCompleted = true;
            uploadFileCallback.onReceiveValue(
                    uri == null ? null : new String[] { uri.toString() });
        }
    };
    if (TRACE) Log.d(TAG, "openFileChooser");
    mWebChromeClient.openFileChooser(innerCallback, fileChooserParams.acceptTypes,
            fileChooserParams.capture ? "*" : "");
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnScaleChangedScaled(
    /* [in] */ Float oldScale,
    /* [in] */ Float newScale)
{
#if 0
    TraceEvent.begin();
    if (TRACE) Log.d(TAG, " onScaleChangedScaled");
    mWebViewClient.onScaleChanged(mWebView, oldScale, newScale);
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ ICustomViewCallback* cb)
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onShowCustomView");
        mWebChromeClient.onShowCustomView(view, cb);
    }
    TraceEvent.end();
#endif
}

//@Override
void WebViewContentsClientAdapter::OnHideCustomView()
{
#if 0
    TraceEvent.begin();
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "onHideCustomView");
        mWebChromeClient.onHideCustomView();
    }
    TraceEvent.end();
#endif
}

//@Override
AutoPtr<IView> WebViewContentsClientAdapter::GetVideoLoadingProgressView()
{
#if 0
    TraceEvent.begin();
    View result;
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "getVideoLoadingProgressView");
        result = mWebChromeClient.getVideoLoadingProgressView();
    } else {
        result = null;
    }
    TraceEvent.end();
    return result;
#endif
    return NULL;
}

//@Override
AutoPtr<IBitmap> WebViewContentsClientAdapter::GetDefaultVideoPoster()
{
#if 0
    TraceEvent.begin();
    Bitmap result = null;
    if (mWebChromeClient != null) {
        if (TRACE) Log.d(TAG, "getDefaultVideoPoster");
        result = mWebChromeClient.getDefaultVideoPoster();
    }
    if (result == null) {
        // The ic_media_video_poster icon is transparent so we need to draw it on a gray
        // background.
        Bitmap poster = BitmapFactory.decodeResource(
                mWebView.getContext().getResources(),
                R.drawable.ic_media_video_poster);
        result = Bitmap.createBitmap(poster.getWidth(), poster.getHeight(), poster.getConfig());
        result.eraseColor(Color.GRAY);
        Canvas canvas = new Canvas(result);
        canvas.drawBitmap(poster, 0f, 0f, null);
    }
    TraceEvent.end();
    return result;
#endif
    return NULL;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
