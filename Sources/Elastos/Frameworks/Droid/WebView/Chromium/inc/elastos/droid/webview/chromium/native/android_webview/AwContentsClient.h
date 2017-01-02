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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSCLIENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Webkit.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/permission/AwPermissionRequest.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/WebContentsObserverElastos.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwWebResourceResponse.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/JsResultReceiver.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/JsPromptResultReceiver.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwHttpAuthHandler.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClientBridge.h"
#include <elastos/utility/etl/Map.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPicture;
using Elastos::Droid::Net::Http::ISslError;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Webkit::IConsoleMessage;
using Elastos::Droid::Webkit::IGeolocationPermissionsCallback;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::IWebChromeClientCustomViewCallback;

using Elastos::Utility::Etl::Map;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::Permission::AwPermissionRequest;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::WebContentsObserverElastos;
using Elastos::Security::IPrincipal;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {
class AwContentsClientCallbackHelper;
/**
 * Base-class that an AwContents embedder derives from to receive callbacks.
 * This extends ContentViewClient, as in many cases we want to pass-thru ContentViewCore
 * callbacks right to our embedder, and this setup facilities that.
 * For any other callbacks we need to make transformations of (e.g. adapt parameters
 * or perform filtering) we can provide final overrides for methods here, and then introduce
 * new abstract methods that the our own client must implement.
 * i.e.: all methods in this class should either be final, or abstract.
 */
class AwContentsClient
:public Object
{
public:
    class AwWebContentsObserver
    : public WebContentsObserverElastos
    {
    public:
        AwWebContentsObserver(
            /* [in] */ AwContentsClient* owner,
            /* [in] */ ContentViewCore* contentViewCore);

        //@Override
        CARAPI DidFinishLoad(
            /* [in] */ Int64 frameId,
            /* [in] */ const String& validatedUrl,
            /* [in] */ Boolean isMainFrame);

        //@Override
        CARAPI DidFailLoad(
            /* [in] */ Boolean isProvisionalLoad,
            /* [in] */ Boolean isMainFrame,
            /* [in] */ Int32 errorCode,
            /* [in] */ const String& description,
            /* [in] */ const String& failingUrl);

        //@Override
        CARAPI DidNavigateMainFrame(
            /* [in] */ const String& url,
            /* [in] */ const String& baseUrl,
            /* [in] */ Boolean isNavigationToDifferentPage,
            /* [in] */ Boolean isFragmentNavigation);

        //@Override
        CARAPI DidNavigateAnyFrame(
            /* [in] */ const String& url,
            /* [in] */ const String& baseUrl,
            /* [in] */ Boolean isReload);

    private:
        AwContentsClient* mOwner;
    };

    //--------------------------------------------------------------------------------------------
    //             WebView specific methods that map directly to WebViewClient / WebChromeClient
    //--------------------------------------------------------------------------------------------

    /**
     * Parameters for the {@link AwContentsClient#showFileChooser} method.
     */
    struct FileChooserParams
    :public Object
    {
        FileChooserParams()
            : mode(0)
            , capture(FALSE)
        {
        }

        Int32 mode;
        String acceptTypes;
        String title;
        String defaultFilename;
        Boolean capture;
    };

    /**
     * Parameters for the {@link AwContentsClient#shouldInterceptRequest} method.
     */
    struct ShouldInterceptRequestParams
    :public Object
    {
        ShouldInterceptRequestParams()
            : isMainFrame(FALSE)
            , hasUserGesture(FALSE)
        {
        }

        // Url of the request.
        String url;
        // Is this for the main frame or a child iframe?
        Boolean isMainFrame;
        // Was a gesture associated with the request? Don't trust can easily be spoofed.
        Boolean hasUserGesture;
        // Method used (GET/POST/OPTIONS)
        String method;
        // Headers that would have been sent to server.
        Map<String, String> requestHeaders;
    };

public:
    AwContentsClient();

    // Alllow injection of the callback thread, for testing.
    AwContentsClient(
        /* [in] */ ILooper* looper);

    virtual CARAPI InstallWebContentsObserver(
        /* [in] */ ContentViewCore* contentViewCore);

    virtual CARAPI_(AwContentsClientCallbackHelper*) GetCallbackHelper();

    virtual CARAPI_(Int32) GetCachedRendererBackgroundColor();

    virtual CARAPI_(Boolean) IsCachedRendererBackgroundColorValid();

    virtual CARAPI OnBackgroundColorChanged(
        /* [in] */ Int32 color);

    virtual CARAPI GetVisitedHistory(
        /* [in] */ IValueCallback* callback) = 0;

    virtual CARAPI DoUpdateVisitedHistory(
        /* [in] */ const String& url,
        /* [in] */ Boolean isReload) = 0;

    virtual CARAPI OnProgressChanged(
        /* [in] */ Int32 progress) = 0;

    virtual CARAPI_(AutoPtr<AwWebResourceResponse>) ShouldInterceptRequest(
        /* [in] */ ShouldInterceptRequestParams* params) = 0;

    virtual CARAPI_(Boolean) ShouldOverrideKeyEvent(
        /* [in] */ IKeyEvent* event) = 0;

    virtual CARAPI_(Boolean) ShouldOverrideUrlLoading(
        /* [in] */ const String& url) = 0;

    virtual CARAPI OnLoadResource(
        /* [in] */ const String& url) = 0;

    virtual CARAPI OnUnhandledKeyEvent(
        /* [in] */ IKeyEvent* event) = 0;

    virtual CARAPI_(Boolean) OnConsoleMessage(
        /* [in] */ IConsoleMessage* consoleMessage) = 0;

    virtual CARAPI OnReceivedHttpAuthRequest(
        /* [in] */ AwHttpAuthHandler* handler,
        /* [in] */ const String& host,
        /* [in] */ const String& realm) = 0;

    virtual CARAPI OnReceivedSslError(
        /* [in] */ IValueCallback* callback,
        /* [in] */ ISslError* error) = 0;

    // TODO(sgurun): Make abstract once this has rolled in downstream.
    virtual CARAPI OnReceivedClientCertRequest(
        /* [in] */ AwContentsClientBridge::ClientCertificateRequestCallback* callback,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal*>* principals,
        /* [in] */ const String& host,
        /* [in] */ Int32 port);

    virtual CARAPI OnReceivedLoginRequest(
        /* [in] */ const String& realm,
        /* [in] */ const String& account,
        /* [in] */ const String& args) = 0;

    virtual CARAPI OnFormResubmission(
        /* [in] */ IMessage* dontResend,
        /* [in] */ IMessage* resend) = 0;

    virtual CARAPI OnDownloadStart(
        /* [in] */ const String& url,
        /* [in] */ const String& userAgent,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [in] */ Int64 contentLength) = 0;

    // TODO(joth): Make abstract once this has rolled in downstream.
    virtual CARAPI ShowFileChooser(
        /* [in] */ IValueCallback* uploadFilePathsCallback,
        /* [in] */ FileChooserParams* fileChooserParams);

    virtual CARAPI OnGeolocationPermissionsShowPrompt(
        /* [in] */ const String& origin,
        /* [in] */ IGeolocationPermissionsCallback* callback) = 0;

    virtual CARAPI OnGeolocationPermissionsHidePrompt() = 0;

    // TODO(michaelbai): Change the abstract once merged
    virtual CARAPI OnPermissionRequest(
        /* [in] */ AwPermissionRequest* awPermissionRequest);

    // TODO(michaelbai): Change the abstract once merged
    virtual CARAPI OnPermissionRequestCanceled(
        /* [in] */ AwPermissionRequest* awPermissionRequest);

    virtual CARAPI OnScaleChangedScaled(
        /* [in] */ Float oldScale,
        /* [in] */ Float newScale) = 0;

    virtual CARAPI OnReceivedTouchIconUrl(
        /* [in] */ const String& url,
        /* [in] */ Boolean precomposed) = 0;

    virtual CARAPI OnReceivedIcon(
        /* [in] */ IBitmap* bitmap) = 0;

    virtual CARAPI OnReceivedTitle(
        /* [in] */ const String& title) = 0;

    virtual CARAPI OnPageStarted(
        /* [in] */ const String& url) = 0;

    virtual CARAPI OnPageFinished(
        /* [in] */ const String& url) = 0;

    virtual CARAPI OnReceivedError(
        /* [in] */ Int32 errorCode,
        /* [in] */ const String& description,
        /* [in] */ const String& failingUrl) = 0;

    // TODO (michaelbai): Remove this method once the same method remove from
    // WebViewContentsClientAdapter.
    virtual CARAPI OnShowCustomView(
        /* [in] */ IView* view,
        /* [in] */ Int32 requestedOrientation,
        /* [in] */ IWebChromeClientCustomViewCallback* callback);

    // TODO (michaelbai): This method should be abstract, having empty body here
    // makes the merge to the Android easy.
    virtual CARAPI OnShowCustomView(
        /* [in] */ IView* view,
        /* [in] */ IWebChromeClientCustomViewCallback* callback);

    virtual CARAPI OnHideCustomView() = 0;

    virtual CARAPI_(AutoPtr<IBitmap>) GetDefaultVideoPoster() = 0;

    //--------------------------------------------------------------------------------------------
    //                              Other WebView-specific methods
    //--------------------------------------------------------------------------------------------
    //
    virtual CARAPI OnFindResultReceived(
        /* [in] */ Int32 activeMatchOrdinal,
        /* [in] */ Int32 numberOfMatches,
        /* [in] */ Boolean isDoneCounting) = 0;

    /**
     * Called whenever there is a new content picture available.
     * @param picture New picture.
     */
    virtual CARAPI OnNewPicture(
        /* [in] */ IPicture* picture) = 0;

protected:
    virtual CARAPI HandleJsAlert(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ JsResultReceiver* receiver) = 0;

    virtual CARAPI_(void) HandleJsBeforeUnload(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ JsResultReceiver* receiver) = 0;

    virtual CARAPI_(void) HandleJsConfirm(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ JsResultReceiver* receiver) = 0;

    virtual CARAPI_(void) HandleJsPrompt(
        /* [in] */ const String& url,
        /* [in] */ const String& message,
        /* [in] */ const String& defaultValue,
        /* [in] */ JsPromptResultReceiver* receiver) = 0;

    virtual CARAPI_(Boolean) OnCreateWindow(
        /* [in] */ Boolean isDialog,
        /* [in] */ Boolean isUserGesture) = 0;

    virtual CARAPI OnCloseWindow() = 0;

    virtual CARAPI OnRequestFocus() = 0;

    virtual CARAPI_(AutoPtr<IView>) GetVideoLoadingProgressView() = 0;

private:
    CARAPI_(void) Init(
        /* [in] */ ILooper* looper);

    AwContentsClientCallbackHelper* mCallbackHelper;

    AutoPtr<AwWebContentsObserver> mWebContentsObserver;

    // Last background color reported from the renderer. Holds the sentinal value INVALID_COLOR
    // if not valid.
    Int32 mCachedRendererBackgroundColor;

    static const Int32 INVALID_COLOR = 0;

    friend class AwContentsClientBridge;
    friend class AwContentViewClient;
    friend class AwWebContentsDelegateAdapter;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWCONTENTSCLIENT_H__
