
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/WebViewClient.h"
//#include "elastos/droid/view/ViewRootImpl.h"

//using Elastos::Droid::View::ViewRootImpl;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IViewRootImpl;

namespace Elastos {
namespace Droid {
namespace Webkit {

// These ints must match up to the hidden values in EventHandler.
/** Generic error */
const Int32 WebViewClient::ERROR_UNKNOWN;
/** Server or proxy hostname lookup failed */
const Int32 WebViewClient::ERROR_HOST_LOOKUP;
/** Unsupported authentication scheme (not basic or digest) */
const Int32 WebViewClient::ERROR_UNSUPPORTED_AUTH_SCHEME;
/** User authentication failed on server */
const Int32 WebViewClient::ERROR_AUTHENTICATION;
/** User authentication failed on proxy */
const Int32 WebViewClient::ERROR_PROXY_AUTHENTICATION;
/** Failed to connect to the server */
const Int32 WebViewClient::ERROR_CONNECT;
/** Failed to read or write to the server */
const Int32 WebViewClient::ERROR_IO;
/** Connection timed out */
const Int32 WebViewClient::ERROR_TIMEOUT;
/** Too many redirects */
const Int32 WebViewClient::ERROR_REDIRECT_LOOP;
/** Unsupported URI scheme */
const Int32 WebViewClient::ERROR_UNSUPPORTED_SCHEME;
/** Failed to perform SSL handshake */
const Int32 WebViewClient::ERROR_FAILED_SSL_HANDSHAKE;
/** Malformed URL */
const Int32 WebViewClient::ERROR_BAD_URL;
/** Generic file error */
const Int32 WebViewClient::ERROR_FILE;
/** File not found */
const Int32 WebViewClient::ERROR_FILE_NOT_FOUND;
/** Too many requests during this load */
const Int32 WebViewClient::ERROR_TOO_MANY_REQUESTS;

CAR_INTERFACE_IMPL(WebViewClient, Object, IWebViewClient);

ECode WebViewClient::constructor()
{
    return NOERROR;
}

/**
 * Give the host application a chance to take over the control when a new
 * url is about to be loaded in the current WebView. If WebViewClient is not
 * provided, by default WebView will ask Activity Manager to choose the
 * proper handler for the url. If WebViewClient is provided, return true
 * means the host application handles the url, while return false means the
 * current WebView handles the url.
 *
 * @param view The WebView that is initiating the callback.
 * @param url The url to be loaded.
 * @return True if the host application wants to leave the current WebView
 *         and handle the url itself, otherwise return false.
 */
ECode WebViewClient::ShouldOverrideUrlLoading(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/**
 * Notify the host application that a page has started loading. This method
 * is called once for each main frame load so a page with iframes or
 * framesets will call onPageStarted one time for the main frame. This also
 * means that onPageStarted will not be called when the contents of an
 * embedded frame changes, i.e. clicking a link whose target is an iframe.
 *
 * @param view The WebView that is initiating the callback.
 * @param url The url to be loaded.
 * @param favicon The favicon for this page if it already exists in the
 *            database.
 */
ECode WebViewClient::OnPageStarted(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ IBitmap* favicon)
{
    return NOERROR;
}

/**
 * Notify the host application that a page has finished loading. This method
 * is called only for main frame. When onPageFinished() is called, the
 * rendering picture may not be updated yet. To get the notification for the
 * new Picture, use {@link WebView.PictureListener#onNewPicture}.
 *
 * @param view The WebView that is initiating the callback.
 * @param url The url of the page.
 */
ECode WebViewClient::OnPageFinished(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url)
{
    return NOERROR;
}

/**
 * Notify the host application that the WebView will load the resource
 * specified by the given url.
 *
 * @param view The WebView that is initiating the callback.
 * @param url The url of the resource the WebView will load.
 */
ECode WebViewClient::OnLoadResource(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url)
{
    return NOERROR;
}

/**
 * Notify the host application of a resource request and allow the
 * application to return the data.  If the return value is null, the WebView
 * will continue to load the resource as usual.  Otherwise, the return
 * response and data will be used.  NOTE: This method is called by the
 * network thread so clients should exercise caution when accessing private
 * data.
 *
 * @param view The {@link android.webkit.WebView} that is requesting the
 *             resource.
 * @param url The raw url of the resource.
 * @return A {@link android.webkit.WebResourceResponse} containing the
 *         response information or null if the WebView should load the
 *         resource itself.
 */
ECode WebViewClient::ShouldInterceptRequest(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [out] */ IWebResourceResponse** wrr)
{
    VALIDATE_NOT_NULL(wrr);
    *wrr = NULL;
    return NOERROR;
}

ECode WebViewClient::ShouldInterceptRequest(
    /* [in] */ IWebView* view,
    /* [in] */ IWebResourceRequest* request,
    /* [out] */ IWebResourceResponse** wrr)
{
    VALIDATE_NOT_NULL(request);
    AutoPtr<IUri> url;
    request->GetUrl((IUri**)&url);
    String str;
    assert(0);
    // TODO
    // url->ToString(&str);
    return ShouldInterceptRequest(view, str, wrr);
}

/**
 * Notify the host application that there have been an excessive number of
 * HTTP redirects. As the host application if it would like to continue
 * trying to load the resource. The default behavior is to send the cancel
 * message.
 *
 * @param view The WebView that is initiating the callback.
 * @param cancelMsg The message to send if the host wants to cancel
 * @param continueMsg The message to send if the host wants to continue
 * @deprecated This method is no longer called. When the WebView encounters
 *             a redirect loop, it will cancel the load.
 */
ECode WebViewClient::OnTooManyRedirects(
    /* [in] */ IWebView* view,
    /* [in] */ IMessage* cancelMsg,
    /* [in] */ IMessage* continueMsg)
{
    cancelMsg->SendToTarget();
    return NOERROR;
}

/**
 * Report an error to the host application. These errors are unrecoverable
 * (i.e. the main resource is unavailable). The errorCode parameter
 * corresponds to one of the ERROR_* constants.
 * @param view The WebView that is initiating the callback.
 * @param errorCode The error code corresponding to an ERROR_* value.
 * @param description A String describing the error.
 * @param failingUrl The url that failed to load.
 */
ECode WebViewClient::OnReceivedError(
    /* [in] */ IWebView* view,
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
    return NOERROR;
}

/**
 * As the host application if the browser should resend data as the
 * requested page was a result of a POST. The default is to not resend the
 * data.
 *
 * @param view The WebView that is initiating the callback.
 * @param dontResend The message to send if the browser should not resend
 * @param resend The message to send if the browser should resend data
 */
ECode WebViewClient::OnFormResubmission(
    /* [in] */ IWebView* view,
    /* [in] */ IMessage* dontResend,
    /* [in] */ IMessage* resend)
{
    return dontResend->SendToTarget();
}

/**
 * Notify the host application to update its visited links database.
 *
 * @param view The WebView that is initiating the callback.
 * @param url The url being visited.
 * @param isReload True if this url is being reloaded.
 */
ECode WebViewClient::DoUpdateVisitedHistory(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ Boolean isReload)
{
    return NOERROR;
}

/**
 * Notify the host application that an SSL error occurred while loading a
 * resource. The host application must call either handler.cancel() or
 * handler.proceed(). Note that the decision may be retained for use in
 * response to future SSL errors. The default behavior is to cancel the
 * load.
 *
 * @param view The WebView that is initiating the callback.
 * @param handler An SslErrorHandler object that will handle the user's
 *            response.
 * @param error The SSL error object.
 */
ECode WebViewClient::OnReceivedSslError(
    /* [in] */ IWebView* view,
    /* [in] */ ISslErrorHandler* handler,
    /* [in] */ ISslError* error)
{
    return handler->Cancel();
}

ECode WebViewClient::OnReceivedClientCertRequest(
    /* [in] */ IWebView* view,
    /* [in] */ IClientCertRequest* request)
{
    return request->Cancel();
}

/**
 * Notifies the host application that the WebView received an HTTP
 * authentication request. The host application can use the supplied
 * {@link HttpAuthHandler} to set the WebView's response to the request.
 * The default behavior is to cancel the request.
 *
 * @param view the WebView that is initiating the callback
 * @param handler the HttpAuthHandler used to set the WebView's response
 * @param host the host requiring authentication
 * @param realm the realm for which authentication is required
 * @see Webview#getHttpAuthUsernamePassword
 */
ECode WebViewClient::OnReceivedHttpAuthRequest(
    /* [in] */ IWebView* view,
    /* [in] */ IHttpAuthHandler* handler,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
    return handler->Cancel();
}

/**
 * Give the host application a chance to handle the key event synchronously.
 * e.g. menu shortcut key events need to be filtered this way. If return
 * true, WebView will not handle the key event. If return false, WebView
 * will always handle the key event, so none of the super in the view chain
 * will see the key event. The default behavior returns false.
 *
 * @param view The WebView that is initiating the callback.
 * @param event The key event.
 * @return True if the host application wants to handle the key event
 *         itself, otherwise return false
 */
ECode WebViewClient::ShouldOverrideKeyEvent(
    /* [in] */ IWebView* view,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/**
 * Notify the host application that a key was not handled by the WebView.
 * Except system keys, WebView always consumes the keys in the normal flow
 * or if shouldOverrideKeyEvent returns true. This is called asynchronously
 * from where the key is dispatched. It gives the host application a chance
 * to handle the unhandled key events.
 *
 * @param view The WebView that is initiating the callback.
 * @param event The key event.
 */
ECode WebViewClient::OnUnhandledKeyEvent(
    /* [in] */ IWebView* view,
    /* [in] */ IKeyEvent* event)
{
    OnUnhandledInputEventInternal(view, IInputEvent::Probe(event));
    return NOERROR;
}

ECode WebViewClient::OnUnhandledInputEvent(
    /* [in] */ IWebView* view,
    /* [in] */ IInputEvent* event)
{
    if (IKeyEvent::Probe(event) != NULL) {
        return OnUnhandledKeyEvent(view, IKeyEvent::Probe(event));
    }

    OnUnhandledInputEventInternal(view, event);

    return NOERROR;
}

/**
 * Notify the host application that the scale applied to the WebView has
 * changed.
 *
 * @param view he WebView that is initiating the callback.
 * @param oldScale The old scale factor
 * @param newScale The new scale factor
 */
ECode WebViewClient::OnScaleChanged(
    /* [in] */ IWebView* view,
    /* [in] */ Float oldScale,
    /* [in] */ Float newScale)
{
    return NOERROR;
}

/**
 * Notify the host application that a request to automatically log in the
 * user has been processed.
 * @param view The WebView requesting the login.
 * @param realm The account realm used to look up accounts.
 * @param account An optional account. If not null, the account should be
 *                checked against accounts on the device. If it is a valid
 *                account, it should be used to log in the user.
 * @param args Authenticator specific arguments used to log in the user.
 */
ECode WebViewClient::OnReceivedLoginRequest(
    /* [in] */ IWebView* view,
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
{
    return NOERROR;
}

ECode WebViewClient::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebViewClient";
    return NOERROR;
}

void WebViewClient::OnUnhandledInputEventInternal(
    /* [in] */ IWebView* view,
    /* [in] */ IInputEvent* event)
{
    AutoPtr<IViewRootImpl> root;
    assert(0);
    // TODO
    // view->GetViewRootImpl((IViewRootImpl**)&root);
    if (root != NULL) {
        root->DispatchUnhandledInputEvent(event);
    }
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
