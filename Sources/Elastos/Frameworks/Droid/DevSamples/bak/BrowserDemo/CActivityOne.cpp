
#include "CActivityOne.h"
#include "R.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Webkit.h"
#include <elastos/utility/logging/Slogger.h>
// #include "elastos/droid/net/WebAddress.h"

using BrowserDemo::R;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Webkit::CURLUtil;
using Elastos::Droid::Webkit::IURLUtil;
using Elastos::Droid::Webkit::EIID_IWebViewClient;
using Elastos::Droid::Webkit::EIID_IWebChromeClient;
//using Elastos::Droid::Webkit::IWebSettingsClassic;
using Elastos::Droid::Webkit::IWebSettings;
using Elastos::Droid::Webkit::EIID_IDownloadListener;
using Elastos::Net::IURI;
using Elastos::Utility::Logging::Slogger;
// using Elastos::Droid::Net::WebAddress;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace BrowserDemo {

 void DownloadProviderStart(
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimetype,
    /* [in] */ Int64 contentLength)
{
    //String contentDisposition = "attachment; filename=/"dancing-skeleton.3gp/"";
    //String mimetype = "video/3GPP";

    AutoPtr<IURLUtil> URLUtil;
    CURLUtil::AcquireSingleton((IURLUtil**)&URLUtil);
    String filename;
    URLUtil->GuessFileName(url, contentDisposition, mimetype, &filename);

    AutoPtr<IURI> uri;

    //try {
        // Undo the percent-encoding that KURL may have done.
        AutoPtr< ArrayOf<Byte> > urlBytes = url.GetBytes();
        AutoPtr< ArrayOf<Byte> > urlDecode;
        URLUtil->Decode(urlBytes, (ArrayOf<Byte>**)&urlDecode);
        //String newUrl = new String(URLUtil.decode(url.getBytes()));
        String newUrl((char const*)urlDecode->GetPayload());
        // Parse the url into pieces
//        WebAddress w(newUrl);
        String frag;
        String query;
        String path;
#if 0
        // Break the path into path, query, and fragment
        if (path.length() > 0) {
            // Strip the fragment
            int idx = path.lastIndexOf('#');
            if (idx != -1) {
                frag = path.substring(idx + 1);
                path = path.substring(0, idx);
            }
            idx = path.lastIndexOf('?');
            if (idx != -1) {
                query = path.substring(idx + 1);
                path = path.substring(0, idx);
            }
        }
        uri = new URI(w.mScheme, w.mAuthInfo, w.mHost, w.mPort, path,
                query, frag);
    //} catch (Exception e) {
        //Log.e(LOGTAG, "Could not parse url for download: " + url, e);
        return;
    }

    ContentValues values = new ContentValues();
    values.put("uri", uri.toString());
    values.put("useragent", "Mozilla/5.0 (linux; U; Android 1.5; en-us; SDK Build/CUPCAKE) AppleWebKit/528.5+ (KHTML, like Gecko) Version/3.1.2 Mobile Safari/525.20.1");
    values.put("notificationpackage", getPackageName());
    values.put("notificationclass", "HelloWorld");
    values.put("visibility", 1);
    values.put("mimetype", mimetype);
    values.put("hint", filename);
    values.put("description", uri.getHost());
    values.put("total_bytes", 1349528);
    values.put("destination", 1);



    //这些参数参考：DownloadProvider工程中的：Helpers.java
    //public static DownloadFileInfo generateSaveFile(
    //        Context context,
    //      String url,
    //        String hint,
    //        String contentDisposition,
    //        String contentLocation,
    //        String mimeType,
    //        int destination,
    //        int contentLength) throws FileNotFoundException {
    //以及：  framework里的Downloads.java;


    ContentResolver mResolver = getContentResolver();
    mResolver.insert(Uri.parse("content://downloads/download"), values);
#endif
}

//===============================================================
//             CActivityOne::InnerWebChromeClient
//===============================================================

CActivityOne::InnerWebChromeClient::InnerWebChromeClient(
    /* [in] */ CActivityOne* owner)
    : mOwner(owner)
{
}

//CAR_INTERFACE_IMPL_LIGHT(CActivityOne::InnerWebChromeClient, IWebChromeClient);

ECode CActivityOne::InnerWebChromeClient::OnProgressChanged(
    /* [in] */ IWebView* view,
    /* [in] */ Int32 newProgress)
{
    return mOwner->SetProgress(newProgress*100);
}

ECode CActivityOne::InnerWebChromeClient::OnReceivedTitle(
    /* [in] */ IWebView* view,
    /* [in] */ const String& title)
{
    return WebChromeClient::OnReceivedTitle(view, title);
}

ECode CActivityOne::InnerWebChromeClient::OnReceivedIcon(
    /* [in] */ IWebView* view,
    /* [in] */ IBitmap* icon)
{
    return WebChromeClient::OnReceivedIcon(view, icon);
}

ECode CActivityOne::InnerWebChromeClient::OnReceivedTouchIconUrl(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ Boolean precomposed)
{
    return WebChromeClient::OnReceivedTouchIconUrl(view, url, precomposed);
}

ECode CActivityOne::InnerWebChromeClient::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ IWebChromeClientCustomViewCallback* callBack)
{
    return WebChromeClient::OnShowCustomView(view, callBack);
}

ECode CActivityOne::InnerWebChromeClient::OnShowCustomView(
    /* [in] */ IView* view,
    /* [in] */ Int32 requestedOrientation,
    /* [in] */ IWebChromeClientCustomViewCallback* callBack)
{
    return WebChromeClient::OnShowCustomView(view, requestedOrientation, callBack);
}

ECode CActivityOne::InnerWebChromeClient::OnHideCustomView()
{
    return WebChromeClient::OnHideCustomView();
}

ECode CActivityOne::InnerWebChromeClient::OnCreateWindow(
    /* [in] */ IWebView* view,
    /* [in] */ Boolean isDialog,
    /* [in] */ Boolean isUserGesture,
    /* [in] */ IMessage* resultMsg,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CActivityOne::InnerWebChromeClient::OnRequestFocus(
    /* [in] */ IWebView* view)
{
    return WebChromeClient::OnRequestFocus(view);
}

ECode CActivityOne::InnerWebChromeClient::OnCloseWindow(
    /* [in] */ IWebView* window)
{
    return WebChromeClient::OnCloseWindow(window);
}

ECode CActivityOne::InnerWebChromeClient::OnJsAlert(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResult* result,
    /* [out] */ Boolean* flag)
{
    return WebChromeClient::OnJsAlert(view, url, message, result, flag);
}

ECode CActivityOne::InnerWebChromeClient::OnJsConfirm(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResult* result,
    /* [out] */ Boolean* flag)
{
    return WebChromeClient::OnJsConfirm(view, url, message, result, flag);
}

ECode CActivityOne::InnerWebChromeClient::OnJsPrompt(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ const String& defaultValue,
    /* [in] */ IJsPromptResult * result,
    /* [out] */ Boolean* flag)
{
    return WebChromeClient::OnJsPrompt(view, url, message, defaultValue, result, flag);
}

ECode CActivityOne::InnerWebChromeClient::OnJsBeforeUnload(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ const String& message,
    /* [in] */ IJsResult* result,
    /* [out] */ Boolean* flag)
{
    return WebChromeClient::OnJsBeforeUnload(view, url, message, result, flag);
}

ECode CActivityOne::InnerWebChromeClient::OnExceededDatabaseQuota(
    /* [in] */ const String& url,
    /* [in] */ const String& databaseIdentifier,
    /* [in] */ Int64 quota,
    /* [in] */ Int64 estimatedDatabaseSize,
    /* [in] */ Int64 totalQuota,
    /* [in] */ IWebStorageQuotaUpdater* quotaUpdater)
{
    return WebChromeClient::OnExceededDatabaseQuota(url, databaseIdentifier, quota, estimatedDatabaseSize,
            totalQuota, quotaUpdater);
}

ECode CActivityOne::InnerWebChromeClient::OnReachedMaxAppCacheSize(
    /* [in] */ Int64 requiredStorage,
    /* [in] */ Int64 quota,
    /* [in] */ IWebStorageQuotaUpdater* quotaUpdater)
{
    return WebChromeClient::OnReachedMaxAppCacheSize(requiredStorage, quota, quotaUpdater);
}

ECode CActivityOne::InnerWebChromeClient::OnGeolocationPermissionsShowPrompt(
    /* [in] */ const String& origin,
    /* [in] */ IGeolocationPermissionsCallback* callBack)
{
    return WebChromeClient::OnGeolocationPermissionsShowPrompt(origin, callBack);
}

ECode CActivityOne::InnerWebChromeClient::OnGeolocationPermissionsHidePrompt()
{
    return WebChromeClient::OnGeolocationPermissionsHidePrompt();
}

ECode CActivityOne::InnerWebChromeClient::OnJsTimeout(
    /* [out] */ Boolean* result)
{
    return WebChromeClient::OnJsTimeout(result);
}

ECode CActivityOne::InnerWebChromeClient::OnConsoleMessage(
    /* [in] */ const String& message,
    /* [in] */ Int32 lineNumber,
    /* [in] */ const String& sourceID)
{
    return WebChromeClient::OnConsoleMessage(message, lineNumber, sourceID);
}

ECode CActivityOne::InnerWebChromeClient::OnConsoleMessage(
    /* [in] */ IConsoleMessage* consoleMessage,
    /* [out] */ Boolean* result)
{
    return WebChromeClient::OnConsoleMessage(consoleMessage, result);
}

ECode CActivityOne::InnerWebChromeClient::GetDefaultVideoPoster(
    /* [out] */ IBitmap** bitmap)
{
    return WebChromeClient::GetDefaultVideoPoster(bitmap);
}

ECode CActivityOne::InnerWebChromeClient::GetVideoLoadingProgressView(
    /* [out] */ IView** view)
{
    return WebChromeClient::GetVideoLoadingProgressView(view);
}

ECode CActivityOne::InnerWebChromeClient::GetVisitedHistory(
    /* [in] */ IValueCallback* callBack)
{
    return WebChromeClient::GetVisitedHistory(callBack);
}

ECode CActivityOne::InnerWebChromeClient::OpenFileChooser(
    /* [in] */ IValueCallback* uploadFile,
    /* [in] */ const String& acceptType,
    /* [in] */ const String& capture)
{
    return WebChromeClient::OpenFileChooser(uploadFile, acceptType, capture);
}

ECode CActivityOne::InnerWebChromeClient::SetupAutoFill(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}

//===============================================================
//              CActivityOne::InnerWebViewClient
//===============================================================

CActivityOne::InnerWebViewClient::InnerWebViewClient(
    /* [in] */ CActivityOne* owner)
    : mOwner(owner)
{
}

//CAR_INTERFACE_IMPL_LIGHT(CActivityOne::InnerWebViewClient, IWebViewClient);

ECode CActivityOne::InnerWebViewClient::ShouldOverrideUrlLoading(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    //Slogger::D("BrowserDemo", "ShouldOverrideUrlLoading,URL:%s", url.string());
    return WebViewClient::ShouldOverrideUrlLoading(view, url, result);
}

ECode CActivityOne::InnerWebViewClient::OnPageStarted(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ IBitmap* favicon)
{
    WebViewClient::OnPageStarted(view, url, favicon);
    return NOERROR;
}

ECode CActivityOne::InnerWebViewClient::OnPageFinished(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url)
{
    WebViewClient::OnPageFinished(view, url);
    return NOERROR;
}

ECode CActivityOne::InnerWebViewClient::OnLoadResource(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url)
{
    WebViewClient::OnLoadResource(view, url);
    return NOERROR;
}

ECode CActivityOne::InnerWebViewClient::ShouldInterceptRequest(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [out] */ IWebResourceResponse** wrr)
{
    return WebViewClient::ShouldInterceptRequest(view, url, wrr);
}

ECode CActivityOne::InnerWebViewClient::OnTooManyRedirects(
    /* [in] */ IWebView* view,
    /* [in] */ IMessage* cancelMsg,
    /* [in] */ IMessage* continueMsg)
{
    return WebViewClient::OnTooManyRedirects(view, cancelMsg, continueMsg);
}

ECode CActivityOne::InnerWebViewClient::OnReceivedError(
    /* [in] */ IWebView* view,
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
    AutoPtr<IToastHelper> toastHelper;
    CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
    String text("sorry");
    text += description;
    AutoPtr<ICharSequence> textCS;
    CString::New(text, (ICharSequence**)&textCS);
    AutoPtr<IToast> toast;
    toastHelper->MakeText(mOwner, textCS, IToast::LENGTH_SHORT, (IToast**)&toast);
    return toast->Show();
}

ECode CActivityOne::InnerWebViewClient::OnFormResubmission(
    /* [in] */ IWebView* view,
    /* [in] */ IMessage* dontResend,
    /* [in] */ IMessage* resend)
{
    return WebViewClient::OnFormResubmission(view, dontResend, resend);
}

ECode CActivityOne::InnerWebViewClient::DoUpdateVisitedHistory(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url,
    /* [in] */ Boolean isReload)
{
    WebViewClient::DoUpdateVisitedHistory(view, url, isReload);
    return NOERROR;
}

ECode CActivityOne::InnerWebViewClient::OnReceivedSslError(
    /* [in] */ IWebView* view,
    /* [in] */ ISslErrorHandler* handler,
    /* [in] */ ISslError* error)
{
    return WebViewClient::OnReceivedSslError(view, handler, error);
}

ECode CActivityOne::InnerWebViewClient::OnReceivedHttpAuthRequest(
    /* [in] */ IWebView* view,
    /* [in] */ IHttpAuthHandler* handler,
    /* [in] */ const String& host,
    /* [in] */ const String& realm)
{
    WebViewClient::OnReceivedHttpAuthRequest(view, handler, host, realm);
    return NOERROR;
}

ECode CActivityOne::InnerWebViewClient::ShouldOverrideKeyEvent(
    /* [in] */ IWebView* view,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    return WebViewClient::ShouldOverrideKeyEvent(view, event, result);
}

ECode CActivityOne::InnerWebViewClient::OnUnhandledKeyEvent(
    /* [in] */ IWebView* view,
    /* [in] */ IKeyEvent* event)
{
    WebViewClient::OnUnhandledKeyEvent(view, event);
    return NOERROR;
}

ECode CActivityOne::InnerWebViewClient::OnScaleChanged(
    /* [in] */ IWebView* view,
    /* [in] */ Float oldScale,
    /* [in] */ Float newScale)
{
    WebViewClient::OnScaleChanged(view, oldScale, newScale);
    return NOERROR;
}

ECode CActivityOne::InnerWebViewClient::OnReceivedLoginRequest(
    /* [in] */ IWebView* view,
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
{
    WebViewClient::OnReceivedLoginRequest(view, realm, account, args);
    return NOERROR;
}

//===============================================================
//              CActivityOne::Btn1OnClickListener
//===============================================================

CActivityOne::Btn1OnClickListener::Btn1OnClickListener(
    /* [in] */ CActivityOne* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(CActivityOne::Btn1OnClickListener, Object, IViewOnClickListener);

ECode CActivityOne::Btn1OnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean goBack = FALSE;
    if (mOwner->wv->CanGoBack(&goBack), goBack){
        mOwner->wv->GoBack();
    }
    else {
        AutoPtr<IToastHelper> toastHelper;
        CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
        AutoPtr<ICharSequence> textCS;
        CString::New(String("sorry, do not GoBack!"), (ICharSequence**)&textCS);
        AutoPtr<IToast> toast;
        toastHelper->MakeText(mOwner, textCS, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    return NOERROR;
}

//===============================================================
//              CActivityOne::Btn2OnClickListener
//===============================================================

CActivityOne::Btn2OnClickListener::Btn2OnClickListener(
    /* [in] */ CActivityOne* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(CActivityOne::Btn2OnClickListener, Object, IViewOnClickListener);

ECode CActivityOne::Btn2OnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Boolean goForward = FALSE;
    if (mOwner->wv->CanGoForward(&goForward), goForward){
        mOwner->wv->GoForward();
    }
    else {
        AutoPtr<IToastHelper> toastHelper;
        CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
        AutoPtr<ICharSequence> textCS;
        CString::New(String("sorry, do not GoForward!"), (ICharSequence**)&textCS);
        AutoPtr<IToast> toast;
        toastHelper->MakeText(mOwner, textCS, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
    }
    return NOERROR;
}

//===============================================================
//              CActivityOne::Btn3OnClickListener
//===============================================================

CActivityOne::Btn3OnClickListener::Btn3OnClickListener(
    /* [in] */ CActivityOne* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(CActivityOne::Btn3OnClickListener, Object, IViewOnClickListener);

ECode CActivityOne::Btn3OnClickListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IEditText> et = IEditText::Probe(mOwner->FindViewById(R::id::et));
    String url;
    AutoPtr<ICharSequence> textCS;
    ITextView::Probe(et)->GetText((ICharSequence**)&textCS);
    String text;
    textCS->ToString(&text);
    url = text.Trim();
    AutoPtr<IURLUtil> urlUtil;
    CURLUtil::AcquireSingleton((IURLUtil**)&urlUtil);
    Boolean result = FALSE;
    //if (urlUtil->IsNetworkUrl(url, &result), result){
    if (urlUtil->IsValidUrl(url, &result), result){
        mOwner->wv->LoadUrl(url);
    }
    else {
        AutoPtr<IToastHelper> toastHelper;
        CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
        AutoPtr<ICharSequence> tipText;
        CString::New(String("sorry, url is Error!"), (ICharSequence**)&tipText);
        AutoPtr<IToast> toast;
        toastHelper->MakeText(mOwner, tipText, IToast::LENGTH_SHORT, (IToast**)&toast);
        toast->Show();
        IView::Probe(et)->RequestFocus(&result);
    }
    return NOERROR;
}

//===============================================================
//              CActivityOne::InnerDownloadListener
//===============================================================

CActivityOne::InnerDownloadListener::InnerDownloadListener(
    /* [in] */ CActivityOne* owner)
    : mOwner(owner)
{
}

CAR_INTERFACE_IMPL(CActivityOne::InnerDownloadListener, Object, IDownloadListener);

ECode CActivityOne::InnerDownloadListener::OnDownloadStart(
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimetype,
    /* [in] */ Int64 contentLength)
{
    return NOERROR;
}

//===============================================================
//                        CActivityOne
//===============================================================

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Boolean result = FALSE;
    GetWindow()->RequestFeature(IWindow::FEATURE_PROGRESS, &result);
    SetContentView(R::layout::main);

    wv = IWebView::Probe(FindViewById(R::id::wv));
    AutoPtr<IWebChromeClient> wcc = new InnerWebChromeClient(this);
    wv->SetWebChromeClient(wcc);

    AutoPtr<IWebViewClient> wvc = new InnerWebViewClient(this);
    wv->SetWebViewClient(wvc);

    AutoPtr<IDownloadListener> downloadListener = new InnerDownloadListener(this);
    wv->SetDownloadListener(downloadListener);

    AutoPtr<IWebSettings> tSettings;
    wv->GetSettings((IWebSettings**)&tSettings);
    tSettings->SetJavaScriptEnabled(true);
    tSettings->SetPluginsEnabled(true);
    tSettings->SetGeolocationEnabled(true);
    tSettings->SetDomStorageEnabled(true);

    AutoPtr<IButton> btn1 = IButton::Probe(FindViewById(R::id::btn1));
    AutoPtr<IButton> btn2 = IButton::Probe(FindViewById(R::id::btn2));
    AutoPtr<IButton> btn3 = IButton::Probe(FindViewById(R::id::btn3));

    AutoPtr<IViewOnClickListener> listener1 = new Btn1OnClickListener(this);
    IView::Probe(btn1)->SetOnClickListener(listener1);

    AutoPtr<IViewOnClickListener> listener2 = new Btn2OnClickListener(this);
    IView::Probe(btn2)->SetOnClickListener(listener2);

    AutoPtr<IViewOnClickListener> listener3 = new Btn3OnClickListener(this);
    IView::Probe(btn3)->SetOnClickListener(listener3);
    return NOERROR;
}

} // namespace BrowserDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
