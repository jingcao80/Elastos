
#ifndef __CACTIVITYONE_H__
#define __CACTIVITYONE_H__

#include "elastos/droid/app/Activity.h"
#include "webkit/WebViewClient.h"
#include "webkit/WebChromeClient.h"
#include "_CActivityOne.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Webkit::IConsoleMessage;
using Elastos::Droid::Webkit::IDownloadListener;
using Elastos::Droid::Webkit::IGeolocationPermissionsCallback;
using Elastos::Droid::Webkit::IHttpAuthHandler;
using Elastos::Droid::Webkit::IJsResult;
using Elastos::Droid::Webkit::IJsPromptResult;
using Elastos::Droid::Webkit::ISslErrorHandler;
using Elastos::Droid::Webkit::IWebStorageQuotaUpdater;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Webkit::IWebViewClient;
using Elastos::Droid::Webkit::IWebChromeClient;
using Elastos::Droid::Webkit::IWebChromeClientCustomViewCallback;
using Elastos::Droid::Webkit::IWebResourceResponse;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::WebViewClient;
using Elastos::Droid::Webkit::WebChromeClient;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BrowserDemo {

class CActivityOne : public Activity
{
private:
    class InnerWebChromeClient
        : public ElLightRefBase
        , public WebChromeClient
        , public IWebChromeClient
    {
    public:
        InnerWebChromeClient(
            /* [in] */ CActivityOne* owner);

        CAR_INTERFACE_DECL();

        CARAPI OnProgressChanged(
            /* [in] */ IWebView* view,
            /* [in] */ Int32 newProgress);

        CARAPI OnReceivedTitle(
            /* [in] */ IWebView* view,
            /* [in] */ const String& title);

        CARAPI OnReceivedIcon(
            /* [in] */ IWebView* view,
            /* [in] */ IBitmap* icon);

        CARAPI OnReceivedTouchIconUrl(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ Boolean precomposed);

        CARAPI OnShowCustomView(
            /* [in] */ IView* view,
            /* [in] */ IWebChromeClientCustomViewCallback* callBack);

        CARAPI OnShowCustomView(
            /* [in] */ IView* view,
            /* [in] */ Int32 requestedOrientation,
            /* [in] */ IWebChromeClientCustomViewCallback* callBack);

        CARAPI OnHideCustomView();

        CARAPI OnCreateWindow(
            /* [in] */ IWebView* view,
            /* [in] */ Boolean isDialog,
            /* [in] */ Boolean isUserGesture,
            /* [in] */ IMessage* resultMsg,
            /* [out] */ Boolean* result);

        CARAPI OnRequestFocus(
            /* [in] */ IWebView* view);

        CARAPI OnCloseWindow(
            /* [in] */ IWebView* window);

        CARAPI OnJsAlert(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ IJsResult* result,
            /* [out] */ Boolean* flag);

        CARAPI OnJsConfirm(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ IJsResult* result,
            /* [out] */ Boolean* flag);

        CARAPI OnJsPrompt(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ const String& defaultValue,
            /* [in] */ IJsPromptResult* result,
            /* [out] */ Boolean* flag);

        CARAPI OnJsBeforeUnload(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ IJsResult* result,
            /* [out] */ Boolean* flag);

        CARAPI OnExceededDatabaseQuota(
            /* [in] */ const String& url,
            /* [in] */ const String& databaseIdentifier,
            /* [in] */ Int64 quota,
            /* [in] */ Int64 estimatedDatabaseSize,
            /* [in] */ Int64 totalQuota,
            /* [in] */ IWebStorageQuotaUpdater* quotaUpdater);

        CARAPI OnReachedMaxAppCacheSize(
            /* [in] */ Int64 requiredStorage,
            /* [in] */ Int64 quota,
            /* [in] */ IWebStorageQuotaUpdater* quotaUpdater);

        CARAPI OnGeolocationPermissionsShowPrompt(
            /* [in] */ const String& origin,
            /* [in] */ IGeolocationPermissionsCallback* callBack);

        CARAPI OnGeolocationPermissionsHidePrompt();

        CARAPI OnJsTimeout(
            /* [out] */ Boolean* result);

        CARAPI OnConsoleMessage(
            /* [in] */ const String& message,
            /* [in] */ Int32 lineNumber,
            /* [in] */ const String& sourceID);

        CARAPI OnConsoleMessage(
            /* [in] */ IConsoleMessage* consoleMessage,
            /* [out] */ Boolean* result);

        CARAPI GetDefaultVideoPoster(
            /* [out] */ IBitmap** bitmap);

        CARAPI GetVideoLoadingProgressView(
            /* [out] */ IView** view);

        CARAPI GetVisitedHistory(
            /* [in] */ IValueCallback* callBack);

        CARAPI OpenFileChooser(
            /* [in] */ IValueCallback* uploadFile,
            /* [in] */ const String& acceptType,
            /* [in] */ const String& capture);

        CARAPI SetupAutoFill(
            /* [in] */ IMessage* msg);

    private:
        CActivityOne* mOwner;
    };

    class InnerWebViewClient
        : public ElLightRefBase
        , public WebViewClient
        , public IWebViewClient
    {
    public:
        InnerWebViewClient(
            /* [in] */ CActivityOne* owner);

        CAR_INTERFACE_DECL();

        CARAPI ShouldOverrideUrlLoading(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [out] */ Boolean* result);

        CARAPI OnPageStarted(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ IBitmap* favicon);

        CARAPI OnPageFinished(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url);

        CARAPI OnLoadResource(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url);

        CARAPI ShouldInterceptRequest(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [out] */ IWebResourceResponse** wrr);

        CARAPI OnTooManyRedirects(
            /* [in] */ IWebView* view,
            /* [in] */ IMessage* cancelMsg,
            /* [in] */ IMessage* continueMsg);

        CARAPI OnReceivedError(
            /* [in] */ IWebView* view,
            /* [in] */ Int32 errorCode,
            /* [in] */ const String& description,
            /* [in] */ const String& failingUrl);

        CARAPI OnFormResubmission(
            /* [in] */ IWebView* view,
            /* [in] */ IMessage* dontResend,
            /* [in] */ IMessage* resend);

        CARAPI DoUpdateVisitedHistory(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ Boolean isReload);

        CARAPI OnReceivedSslError(
            /* [in] */ IWebView* view,
            /* [in] */ ISslErrorHandler* handler,
            /* [in] */ ISslError* error);

        CARAPI OnReceivedHttpAuthRequest(
            /* [in] */ IWebView* view,
            /* [in] */ IHttpAuthHandler* handler,
            /* [in] */ const String& host,
            /* [in] */ const String& realm);

        CARAPI ShouldOverrideKeyEvent(
            /* [in] */ IWebView* view,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean * pResult);

        CARAPI OnUnhandledKeyEvent(
            /* [in] */ IWebView* view,
            /* [in] */ IKeyEvent* event);

        CARAPI OnScaleChanged(
            /* [in] */ IWebView* view,
            /* [in] */ Float oldScale,
            /* [in] */ Float newScale);

        CARAPI OnReceivedLoginRequest(
            /* [in] */ IWebView* view,
            /* [in] */ const String& realm,
            /* [in] */ const String& account,
            /* [in] */ const String& args);

    private:
        CActivityOne* mOwner;
    };

    class Btn1OnClickListener
        : public ElLightRefBase
        , public IViewOnClickListener
    {
    public:
        Btn1OnClickListener(
            /* [in] */ CActivityOne* owner);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mOwner;
    };

    class Btn2OnClickListener
        : public ElLightRefBase
        , public IViewOnClickListener
    {
    public:
        Btn2OnClickListener(
            /* [in] */ CActivityOne* owner);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mOwner;
    };

    class Btn3OnClickListener
        : public ElLightRefBase
        , public IViewOnClickListener
    {
    public:
        Btn3OnClickListener(
            /* [in] */ CActivityOne* owner);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CActivityOne* mOwner;
    };

    class InnerDownloadListener
        : public ElLightRefBase
        , public IDownloadListener
    {
    public:
        InnerDownloadListener(
            /* [in] */ CActivityOne* owner);

        CAR_INTERFACE_DECL();

        CARAPI OnDownloadStart(
            /* [in] */ const String& url,
            /* [in] */ const String& userAgent,
            /* [in] */ const String& contentDisposition,
            /* [in] */ const String& mimetype,
            /* [in] */ Int64 contentLength);

    private:
        CActivityOne* mOwner;
    };

public:
    /** Called when the activity is first created. */
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    AutoPtr<IWebView> wv;
};

} // namespace BrowserDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CACTIVITYONE_H__
