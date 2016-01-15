
#include "elastos/droid/webkit/WebChromeClient.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(WebChromeClient, Object, IWebChromeClient);

ECode WebChromeClient::constructor()
{
    return NOERROR;
}

/**
 * Tell the host application the current progress of loading a page.
 * @param view The WebView that initiated the callback.
 * @param newProgress Current page loading progress, represented by
 *                    an integer between 0 and 100.
 */
ECode WebChromeClient::OnProgressChanged(
    /* [in] */ IWebView* view,
    /* [in] */ Int32 newProgress)
{
    return NOERROR;
}

/**
 * Notify the host application of a change in the document title.
 * @param view The WebView that initiated the callback.
 * @param title A String containing the new title of the document.
 */
ECode WebChromeClient::OnReceivedTitle(
    /* [in] */ IWebView* view,
    /* [in] */ const String& title)
{
    return NOERROR;
}

/**
 * Notify the host application of a new favicon for the current page.
 * @param view The WebView that initiated the callback.
 * @param icon A Bitmap containing the favicon for the current page.
 */
ECode WebChromeClient::OnReceivedIcon(
    /* [in] */ IWebView* view,
    /* [in] */ IBitmap* icon)
{
    return NOERROR;
}

/**
 * Notify the host application of the url for an apple-touch-icon.
 * @param view The WebView that initiated the callback.
 * @param url The icon url.
 * @param precomposed True if the url is for a precomposed touch icon.
 */
ECode WebChromeClient::OnReceivedTouchIconUrl(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ Boolean precomposed)
{
    return NOERROR;
}

/**
 * Notify the host application that the current page would
 * like to show a custom View.
 * @param view is the View object to be shown.
 * @param callback is the callback to be invoked if and when the view
 * is dismissed.
 */
ECode WebChromeClient::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ IWebChromeClientCustomViewCallback* callback)
{
    return NOERROR;
}

/**
 * Notify the host application that the current page would
 * like to show a custom View in a particular orientation.
 * @param view is the View object to be shown.
 * @param requestedOrientation An orientation constant as used in
 * {@link ActivityInfo#screenOrientation ActivityInfo.screenOrientation}.
 * @param callback is the callback to be invoked if and when the view
 * is dismissed.
 */
ECode WebChromeClient::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ Int32 requestedOrientation,
    /* [in] */ IWebChromeClientCustomViewCallback* callback)
{
    return NOERROR;
}

/**
 * Notify the host application that the current page would
 * like to hide its custom view.
 */
ECode WebChromeClient::OnHideCustomView()
{
    return NOERROR;
}

/**
 * Request the host application to create a new window. If the host
 * application chooses to honor this request, it should return true from
 * this method, create a new WebView to host the window, insert it into the
 * View system and send the supplied resultMsg message to its target with
 * the new WebView as an argument. If the host application chooses not to
 * honor the request, it should return false from this method. The default
 * implementation of this method does nothing and hence returns false.
 * @param view The WebView from which the request for a new window
 *             originated.
 * @param isDialog True if the new window should be a dialog, rather than
 *                 a full-size window.
 * @param isUserGesture True if the request was initiated by a user gesture,
 *                      such as the user clicking a link.
 * @param resultMsg The message to send when once a new WebView has been
 *                  created. resultMsg.obj is a
 *                  {@link WebView.WebViewTransport} object. This should be
 *                  used to transport the new WebView, by calling
 *                  {@link WebView.WebViewTransport#setWebView(WebView)
 *                  WebView.WebViewTransport.setWebView(WebView)}.
 * @return This method should return true if the host application will
 *         create a new window, in which case resultMsg should be sent to
 *         its target. Otherwise, this method should return false. Returning
 *         false from this method but also sending resultMsg will result in
 *         undefined behavior.
 */
ECode WebChromeClient::OnCreateWindow(
    /* [in] */ IWebView* view,
    /* [in] */ Boolean isDialog,
    /* [in] */ Boolean isUserGesture,
    /* [in] */ IMessage* resultMsg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/**
 * Request display and focus for this WebView. This may happen due to
 * another WebView opening a link in this WebView and requesting that this
 * WebView be displayed.
 * @param view The WebView that needs to be focused.
 */
ECode WebChromeClient::OnRequestFocus(
    /* [in] */ IWebView* view)
{
    return NOERROR;
}

/**
 * Notify the host application to close the given WebView and remove it
 * from the view system if necessary. At this point, WebCore has stopped
 * any loading in this window and has removed any cross-scripting ability
 * in javascript.
 * @param window The WebView that needs to be closed.
 */
ECode WebChromeClient::OnCloseWindow(
    /* [in] */ IWebView* window)
{
    return NOERROR;
}

/**
 * Tell the client to display a javascript alert dialog.  If the client
 * returns true, WebView will assume that the client will handle the
 * dialog.  If the client returns false, it will continue execution.
 * @param view The WebView that initiated the callback.
 * @param url The url of the page requesting the dialog.
 * @param message Message to be displayed in the window.
 * @param result A JsResult to confirm that the user hit enter.
 * @return boolean Whether the client will handle the alert dialog.
 */
ECode WebChromeClient::OnJsAlert(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResult* result,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    *flag = FALSE;
    return NOERROR;
}

/**
 * Tell the client to display a confirm dialog to the user. If the client
 * returns true, WebView will assume that the client will handle the
 * confirm dialog and call the appropriate JsResult method. If the
 * client returns false, a default value of false will be returned to
 * javascript. The default behavior is to return false.
 * @param view The WebView that initiated the callback.
 * @param url The url of the page requesting the dialog.
 * @param message Message to be displayed in the window.
 * @param result A JsResult used to send the user's response to
 *               javascript.
 * @return boolean Whether the client will handle the confirm dialog.
 */
ECode WebChromeClient::OnJsConfirm(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResult* result,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    *flag = FALSE;
    return NOERROR;
}

/**
 * Tell the client to display a prompt dialog to the user. If the client
 * returns true, WebView will assume that the client will handle the
 * prompt dialog and call the appropriate JsPromptResult method. If the
 * client returns false, a default value of false will be returned to to
 * javascript. The default behavior is to return false.
 * @param view The WebView that initiated the callback.
 * @param url The url of the page requesting the dialog.
 * @param message Message to be displayed in the window.
 * @param defaultValue The default value displayed in the prompt dialog.
 * @param result A JsPromptResult used to send the user's reponse to
 *               javascript.
 * @return boolean Whether the client will handle the prompt dialog.
 */
ECode WebChromeClient::OnJsPrompt(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ const String& defaultValue,
    /* [in] */ IJsPromptResult* result,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    *flag = FALSE;
    return NOERROR;
}

/**
 * Tell the client to display a dialog to confirm navigation away from the
 * current page. This is the result of the onbeforeunload javascript event.
 * If the client returns true, WebView will assume that the client will
 * handle the confirm dialog and call the appropriate JsResult method. If
 * the client returns false, a default value of true will be returned to
 * javascript to accept navigation away from the current page. The default
 * behavior is to return false. Setting the JsResult to true will navigate
 * away from the current page, false will cancel the navigation.
 * @param view The WebView that initiated the callback.
 * @param url The url of the page requesting the dialog.
 * @param message Message to be displayed in the window.
 * @param result A JsResult used to send the user's response to
 *               javascript.
 * @return boolean Whether the client will handle the confirm dialog.
 */
ECode WebChromeClient::OnJsBeforeUnload(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResult* result,
    /* [out] */ Boolean* flag)
{
    VALIDATE_NOT_NULL(flag);
    *flag = FALSE;
    return NOERROR;
}

/**
* Tell the client that the quota has been exceeded for the Web SQL Database
* API for a particular origin and request a new quota. The client must
* respond by invoking the
* {@link WebStorage.QuotaUpdater#updateQuota(long) updateQuota(long)}
* method of the supplied {@link WebStorage.QuotaUpdater} instance. The
* minimum value that can be set for the new quota is the current quota. The
* default implementation responds with the current quota, so the quota will
* not be increased.
* @param url The URL of the page that triggered the notification
* @param databaseIdentifier The identifier of the database where the quota
*                           was exceeded.
* @param quota The quota for the origin, in bytes
* @param estimatedDatabaseSize The estimated size of the offending
*                              database, in bytes
* @param totalQuota The total quota for all origins, in bytes
* @param quotaUpdater An instance of {@link WebStorage.QuotaUpdater} which
*                     must be used to inform the WebView of the new quota.
*/
// Note that the callback must always be executed at some point to ensure
// that the sleeping WebCore thread is woken up.
ECode WebChromeClient::OnExceededDatabaseQuota(
    /* [in] */ const String& url,
    /* [in] */ const String& databaseIdentifier,
    /* [in] */ Int64 quota,
    /* [in] */ Int64 estimatedDatabaseSize,
    /* [in] */ Int64 totalQuota,
    /* [in] */ IWebStorageQuotaUpdater* quotaUpdater)
{
    // This default implementation passes the current quota back to WebCore.
    // WebCore will interpret this that new quota was declined.
    return quotaUpdater->UpdateQuota(quota);
}

/**
* Notify the host application that the Application Cache has reached the
* maximum size. The client must respond by invoking the
* {@link WebStorage.QuotaUpdater#updateQuota(long) updateQuota(long)}
* method of the supplied {@link WebStorage.QuotaUpdater} instance. The
* minimum value that can be set for the new quota is the current quota. The
* default implementation responds with the current quota, so the quota will
* not be increased.
* @param requiredStorage The amount of storage required by the Application
*                        Cache operation that triggered this notification,
*                        in bytes.
* @param quota the current maximum Application Cache size, in bytes
* @param quotaUpdater An instance of {@link WebStorage.QuotaUpdater} which
*                     must be used to inform the WebView of the new quota.
*/
// Note that the callback must always be executed at some point to ensure
// that the sleeping WebCore thread is woken up.
ECode WebChromeClient::OnReachedMaxAppCacheSize(
    /* [in] */ Int64 requiredStorage,
    /* [in] */ Int64 quota,
    /* [in] */ IWebStorageQuotaUpdater* quotaUpdater)
{
    return quotaUpdater->UpdateQuota(quota);
}

/**
 * Notify the host application that web content from the specified origin
 * is attempting to use the Geolocation API, but no permission state is
 * currently set for that origin. The host application should invoke the
 * specified callback with the desired permission state. See
 * {@link GeolocationPermissions} for details.
 * @param origin The origin of the web content attempting to use the
 *               Geolocation API.
 * @param callback The callback to use to set the permission state for the
 *                 origin.
 */
ECode WebChromeClient::OnGeolocationPermissionsShowPrompt(
    /* [in] */ const String& origin,
    /* [in] */ IGeolocationPermissionsCallback* callBack)
{
    return NOERROR;
}

/**
 * Notify the host application that a request for Geolocation permissions,
 * made with a previous call to
 * {@link #onGeolocationPermissionsShowPrompt(String,GeolocationPermissions.Callback) onGeolocationPermissionsShowPrompt()}
 * has been canceled. Any related UI should therefore be hidden.
 */
ECode WebChromeClient::OnGeolocationPermissionsHidePrompt()
{
    return NOERROR;
}

ECode WebChromeClient::OnPermissionRequest(
    /* [in] */ IPermissionRequest* request)
{
    return request->Deny();
}

/**
 * Notify the host application that the given permission request
 * has been canceled. Any related UI should therefore be hidden.
 *
 * @param request the PermissionRequest that needs be canceled.
 */
ECode WebChromeClient::OnPermissionRequestCanceled(
    /* [in] */ IPermissionRequest* request)
{
    return NOERROR;
}

/**
 * Tell the client that a JavaScript execution timeout has occured. And the
 * client may decide whether or not to interrupt the execution. If the
 * client returns true, the JavaScript will be interrupted. If the client
 * returns false, the execution will continue. Note that in the case of
 * continuing execution, the timeout counter will be reset, and the callback
 * will continue to occur if the script does not finish at the next check
 * point.
 * @return boolean Whether the JavaScript execution should be interrupted.
 * @deprecated This method is no longer supported and will not be invoked.
 */
// This method was only called when using the JSC javascript engine. V8 became
// the default JS engine with Froyo and support for building with JSC was
// removed in b/5495373. V8 does not have a mechanism for making a callback such
// as this.
ECode WebChromeClient::OnJsTimeout(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

/**
 * Report a JavaScript error message to the host application. The ChromeClient
 * should override this to process the log message as they see fit.
 * @param message The error message to report.
 * @param lineNumber The line number of the error.
 * @param sourceID The name of the source file that caused the error.
 * @deprecated Use {@link #onConsoleMessage(ConsoleMessage) onConsoleMessage(ConsoleMessage)}
 *      instead.
 */
//@Deprecated
ECode WebChromeClient::OnConsoleMessage(
    /* [in] */ const String& message,
    /* [in] */ Int32 lineNumber,
    /* [in] */ const String& sourceID)
{
    return NOERROR;
}

/**
 * Report a JavaScript console message to the host application. The ChromeClient
 * should override this to process the log message as they see fit.
 * @param consoleMessage Object containing details of the console message.
 * @return true if the message is handled by the client.
 */
ECode WebChromeClient::OnConsoleMessage(
    /* [in] */ IConsoleMessage* consoleMessage,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    assert(consoleMessage);

    // Call the old version of this function for backwards compatability.
    String message;
    Int32 lineNumber;
    String sourceID;
    consoleMessage->Message(&message);
    consoleMessage->LineNumber(&lineNumber);
    consoleMessage->SourceId(&sourceID);
    OnConsoleMessage(message, lineNumber, sourceID);

    *result = FALSE;

    return NOERROR;
}

/**
 * When not playing, video elements are represented by a 'poster' image. The
 * image to use can be specified by the poster attribute of the video tag in
 * HTML. If the attribute is absent, then a default poster will be used. This
 * method allows the ChromeClient to provide that default image.
 *
 * @return Bitmap The image to use as a default poster, or null if no such image is
 * available.
 */
ECode WebChromeClient::GetDefaultVideoPoster(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;
    return NOERROR;
}

/**
 * When the user starts to playback a video element, it may take time for enough
 * data to be buffered before the first frames can be rendered. While this buffering
 * is taking place, the ChromeClient can use this function to provide a View to be
 * displayed. For example, the ChromeClient could show a spinner animation.
 *
 * @return View The View to be displayed whilst the video is loading.
 */
ECode WebChromeClient::GetVideoLoadingProgressView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = NULL;
    return NOERROR;
}

/** Obtains a list of all visited history items, used for link coloring
 */
ECode WebChromeClient::GetVisitedHistory(
    /* [in] */ IValueCallback* callBack)
{
    return NOERROR;
}

ECode WebChromeClient::OnShowFileChooser(
    /* [in] */ IWebView* webView,
    /* [in] */ IValueCallback* filePathCallback,
    /* [in] */ IWebChromeClientFileChooserParams* fileChooserParams,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

/**
 * Tell the client to open a file chooser.
 * @param uploadFile A ValueCallback to set the URI of the file to upload.
 *      onReceiveValue must be called to wake up the thread.a
 * @param acceptType The value of the 'accept' attribute of the input tag
 *         associated with this file picker.
 * @param capture The value of the 'capture' attribute of the input tag
 *         associated with this file picker.
 * @hide
 */
ECode WebChromeClient::OpenFileChooser(
    /* [in] */ IValueCallback* uploadFile,
    /* [in] */ const String& acceptType,
    /* [in] */ const String& capture)
{
//        uploadFile->OnReceiveValue(NULL);
    return NOERROR;
}

/**
 * Tell the client that the page being viewed has an autofillable
 * form and the user would like to set a profile up.
 * @param msg A Message to send once the user has successfully
 *      set up a profile and to inform the WebTextView it should
 *      now autofill using that new profile.
 * @hide
 */
ECode WebChromeClient::SetupAutoFill(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}

ECode WebChromeClient::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = "WebChromeClient";
    return NOERROR;
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos