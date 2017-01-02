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

#ifndef __ELASTOS_DROID_WEBKIT_WEBCHROMECLIENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBCHROMECLIENT_H__

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Webkit {

class ECO_PUBLIC WebChromeClient
    : public Object
    , public IWebChromeClient
{
public:
    /**
     * A callback interface used by the host application to notify
     * the current page that its custom view has been dismissed.
     */
    class CustomViewCallback : public Object
    {
    public:
        /**
         * Invoked when the host application dismisses the
         * custom view.
         */
        virtual CARAPI_(void) OnCustomViewHidden() = 0;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Tell the host application the current progress of loading a page.
     * @param view The WebView that initiated the callback.
     * @param newProgress Current page loading progress, represented by
     *                    an integer between 0 and 100.
     */
    CARAPI OnProgressChanged(
        /* [in] */ IWebView* view,
        /* [in] */ Int32 newProgress);

    /**
     * Notify the host application of a change in the document title.
     * @param view The WebView that initiated the callback.
     * @param title A String containing the new title of the document.
     */
    CARAPI OnReceivedTitle(
        /* [in] */ IWebView* view,
        /* [in] */ const String& title);

    /**
     * Notify the host application of a new favicon for the current page.
     * @param view The WebView that initiated the callback.
     * @param icon A Bitmap containing the favicon for the current page.
     */
    CARAPI OnReceivedIcon(
        /* [in] */ IWebView* view,
        /* [in] */ IBitmap* icon);

    /**
     * Notify the host application of the url for an apple-touch-icon.
     * @param view The WebView that initiated the callback.
     * @param url The icon url.
     * @param precomposed True if the url is for a precomposed touch icon.
     */
    CARAPI OnReceivedTouchIconUrl(
        /* [in] */ IWebView* view,
        /* [in] */ const String& url,
        /* [in] */ Boolean precomposed);

    /**
     * Notify the host application that the current page would
     * like to show a custom View.
     * @param view is the View object to be shown.
     * @param callback is the callback to be invoked if and when the view
     * is dismissed.
     */
    CARAPI OnShowCustomView(
        /* [in] */ IView* view,
        /* [in] */ IWebChromeClientCustomViewCallback* callback);

    /**
     * Notify the host application that the current page would
     * like to show a custom View in a particular orientation.
     * @param view is the View object to be shown.
     * @param requestedOrientation An orientation constant as used in
     * {@link ActivityInfo#screenOrientation ActivityInfo.screenOrientation}.
     * @param callback is the callback to be invoked if and when the view
     * is dismissed.
     */
    CARAPI OnShowCustomView(
        /* [in] */ IView* view,
        /* [in] */ Int32 requestedOrientation,
        /* [in] */ IWebChromeClientCustomViewCallback* callback);

    /**
     * Notify the host application that the current page would
     * like to hide its custom view.
     */
    CARAPI OnHideCustomView();

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
    CARAPI OnCreateWindow(
        /* [in] */ IWebView* view,
        /* [in] */ Boolean isDialog,
        /* [in] */ Boolean isUserGesture,
        /* [in] */ IMessage* resultMsg,
        /* [out] */ Boolean* result);

    /**
     * Request display and focus for this WebView. This may happen due to
     * another WebView opening a link in this WebView and requesting that this
     * WebView be displayed.
     * @param view The WebView that needs to be focused.
     */
    CARAPI OnRequestFocus(
        /* [in] */ IWebView* view);

    /**
     * Notify the host application to close the given WebView and remove it
     * from the view system if necessary. At this point, WebCore has stopped
     * any loading in this window and has removed any cross-scripting ability
     * in javascript.
     * @param window The WebView that needs to be closed.
     */
    CARAPI OnCloseWindow(
        /* [in] */ IWebView* window);

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
    CARAPI OnJsAlert(
        /* [in] */ IWebView* view,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ IJsResult* result,
        /* [out] */ Boolean* flag);

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
    CARAPI OnJsConfirm(
        /* [in] */ IWebView* view,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ IJsResult* result,
        /* [out] */ Boolean* flag);

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
    CARAPI OnJsPrompt(
        /* [in] */ IWebView* view,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ const String& defaultValue,
        /* [in] */ IJsPromptResult* result,
        /* [out] */ Boolean* flag);

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
    CARAPI OnJsBeforeUnload(
        /* [in] */ IWebView* view,
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ IJsResult* result,
        /* [out] */ Boolean* flag);

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
    CARAPI OnExceededDatabaseQuota(
        /* [in] */ const String& url,
        /* [in] */ const String& databaseIdentifier,
        /* [in] */ Int64 quota,
        /* [in] */ Int64 estimatedDatabaseSize,
        /* [in] */ Int64 totalQuota,
        /* [in] */ IWebStorageQuotaUpdater* quotaUpdater);

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
    CARAPI OnReachedMaxAppCacheSize(
        /* [in] */ Int64 requiredStorage,
        /* [in] */ Int64 quota,
        /* [in] */ IWebStorageQuotaUpdater* quotaUpdater);

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
    CARAPI OnGeolocationPermissionsShowPrompt(
        /* [in] */ const String& origin,
        /* [in] */ IGeolocationPermissionsCallback* callBack);

    /**
     * Notify the host application that a request for Geolocation permissions,
     * made with a previous call to
     * {@link #onGeolocationPermissionsShowPrompt(String,GeolocationPermissions.Callback) onGeolocationPermissionsShowPrompt()}
     * has been canceled. Any related UI should therefore be hidden.
     */
    CARAPI OnGeolocationPermissionsHidePrompt();

    /**
     * Notify the host application that web content is requesting permission to
     * access the specified resources and the permission currently isn't granted
     * or denied. The host application must invoke {@link PermissionRequest#grant(String[])}
     * or {@link PermissionRequest#deny()}.
     *
     * If this method isn't overridden, the permission is denied.
     *
     * @param request the PermissionRequest from current web content.
     */
    CARAPI OnPermissionRequest(
        /* [in] */ IPermissionRequest* request);

    /**
     * Notify the host application that the given permission request
     * has been canceled. Any related UI should therefore be hidden.
     *
     * @param request the PermissionRequest that needs be canceled.
     */
    CARAPI OnPermissionRequestCanceled(
        /* [in] */ IPermissionRequest* request);

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
    CARAPI OnJsTimeout(
        /* [out] */ Boolean* result);

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
    CARAPI OnConsoleMessage(
        /* [in] */ const String& message,
        /* [in] */ Int32 lineNumber,
        /* [in] */ const String& sourceID);

    /**
     * Report a JavaScript console message to the host application. The ChromeClient
     * should override this to process the log message as they see fit.
     * @param consoleMessage Object containing details of the console message.
     * @return true if the message is handled by the client.
     */
    CARAPI OnConsoleMessage(
        /* [in] */ IConsoleMessage* consoleMessage,
        /* [out] */ Boolean* result);

    /**
     * When not playing, video elements are represented by a 'poster' image. The
     * image to use can be specified by the poster attribute of the video tag in
     * HTML. If the attribute is absent, then a default poster will be used. This
     * method allows the ChromeClient to provide that default image.
     *
     * @return Bitmap The image to use as a default poster, or null if no such image is
     * available.
     */
    CARAPI GetDefaultVideoPoster(
        /* [out] */ IBitmap** bitmap);

    /**
     * When the user starts to playback a video element, it may take time for enough
     * data to be buffered before the first frames can be rendered. While this buffering
     * is taking place, the ChromeClient can use this function to provide a View to be
     * displayed. For example, the ChromeClient could show a spinner animation.
     *
     * @return View The View to be displayed whilst the video is loading.
     */
    CARAPI GetVideoLoadingProgressView(
        /* [out] */ IView** view);

    /** Obtains a list of all visited history items, used for link coloring
     */
    CARAPI GetVisitedHistory(
        /* [in] */ IValueCallback* callBack);

    /**
     * Tell the client to show a file chooser.
     *
     * This is called to handle HTML forms with 'file' input type, in response to the
     * user pressing the "Select File" button.
     * To cancel the request, call <code>filePathCallback.onReceiveValue(null)</code> and
     * return true.
     *
     * @param webView The WebView instance that is initiating the request.
     * @param filePathCallback Invoke this callback to supply the list of paths to files to upload,
     *                         or NULL to cancel. Must only be called if the
     *                         <code>showFileChooser</code> implementations returns true.
     * @param fileChooserParams Describes the mode of file chooser to be opened, and options to be
     *                          used with it.
     * @return true if filePathCallback will be invoked, false to use default handling.
     *
     * @see FileChooserParams
     */
    CARAPI OnShowFileChooser(
        /* [in] */ IWebView* webView,
        /* [in] */ IValueCallback* filePathCallback,
        /* [in] */ IWebChromeClientFileChooserParams* fileChooserParams,
        /* [out] */ Boolean* result);

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
    CARAPI OpenFileChooser(
        /* [in] */ IValueCallback* uploadFile,
        /* [in] */ const String& acceptType,
        /* [in] */ const String& capture);

    /**
     * Tell the client that the page being viewed has an autofillable
     * form and the user would like to set a profile up.
     * @param msg A Message to send once the user has successfully
     *      set up a profile and to inform the WebTextView it should
     *      now autofill using that new profile.
     * @hide
     */
    CARAPI SetupAutoFill(
        /* [in] */ IMessage* msg);

    CARAPI ToString(
        /* [out] */ String* info);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBCHROMECLIENT_H__
