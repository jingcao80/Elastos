#include "CActivityOne.h"
#include "R.h"
// #include "webkit/WebViewClient.h"
#include "webkit/WebChromeClient.h"
#include "webkit/WebSettingsClassic.h"

#include <elastos/core/Thread.h>
#include <Elastos.Droid.Core.h>

#include <cutils/log.h>


using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Core::Thread;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Webkit::IConsoleMessage;
using Elastos::Droid::Webkit::IWebChromeClientCustomViewCallback;
using Elastos::Droid::Webkit::IGeolocationPermissionsCallback;
using Elastos::Droid::Webkit::IHttpAuthHandler;
using Elastos::Droid::Webkit::IJsPromptResult;
using Elastos::Droid::Webkit::IJsResult;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::IWebSettings;
using Elastos::Droid::Webkit::IWebSettingsClassic;
using Elastos::Droid::Webkit::IWebViewClient;
using Elastos::Droid::Webkit::IWebResourceResponse;
using Elastos::Droid::Webkit::IWebStorageQuotaUpdater;
// using Elastos::Droid::Webkit::WebViewClient;
using Elastos::Droid::Webkit::WebChromeClient;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::CEditText;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::View::EIID_IViewOnKeyListener;

namespace Elastos {

namespace Droid {
namespace DevSamples {
namespace WebViewDemo {

CAR_INTERFACE_IMPL_LIGHT(CActivityOne::ButtonOnClickListener, IViewOnClickListener);
CAR_INTERFACE_IMPL_LIGHT(CActivityOne::DialogOnClickListener, IDialogInterfaceOnClickListener);
CAR_INTERFACE_IMPL_LIGHT(CActivityOne::ViewOnKeyListener, IViewOnKeyListener);

ECode CActivityOne::HandleMessage(
    /* [in] */ Int32 what)
{
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    Boolean bFlag = FALSE;
    thread->IsInterrupted(&bFlag);
    if (!bFlag)
    {
        switch (what)
        {
        case 0:
            mProgressDialog->Show();//show progress dialog
            break;

        case 1:
            mProgressDialog->Hide();
            break;
        }
    }

//    super.handleMessage(msg);

    return NOERROR;
}

void CActivityOne::SendMessage(
    /* [in] */ Int32 what)
{
    // if (handler == NULL) return;

    // ECode (STDCALL CActivityOne::*pHandlerFunc)(Int32 what);
    // pHandlerFunc = &CActivityOne::HandleMessage;

    // AutoPtr<IParcel> params;
    // CCallbackParcel::New((IParcel**)&params);
    // params->WriteInt32(what);
    // Boolean result;
    // handler->PostCppCallback((HANDLE)handler.Get(), *(HANDLE*)&pHandlerFunc,
    //       params, what, FALSE, &result);
}

//@Override
ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    AutoPtr<IView> view;
    FindViewById(R::id::myWv, (IView**)&view);
    ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);
    //IWebView::Probe(view)->LoadUrl(String("http://www.baidu.com/"));
    //IWebView::Probe(view)->LoadUrl(String("file:///data/temp/1.html"));
    mWebView = IWebView::Probe(view);
    AutoPtr<IWebSettingsClassic> tSettings;
    mWebView->GetSettings((IWebSettings**)&tSettings);
    tSettings->SetJavaScriptEnabled(true);
    tSettings->SetPluginsEnabled(true);
    tSettings->SetGeolocationEnabled(true);
    tSettings->SetDomStorageEnabled(true);
    //mWebView->LoadUrl(String("file:///data/temp/1.html"));
    ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);

    mClickListener = new CActivityOne::ButtonOnClickListener(this);
    //AutoPtr<IButton> gotoButton;
    AutoPtr<IView> gotoButton;
    FindViewById(R::id::goto_button1, (IView**)&gotoButton);
    gotoButton->SetOnClickListener(mClickListener);
    FindViewById(R::id::goto_button2, (IView**)&gotoButton);
    gotoButton->SetOnClickListener(mClickListener);
    FindViewById(R::id::goto_button3, (IView**)&gotoButton);
    gotoButton->SetOnClickListener(mClickListener);

    //AutoPtr<IButton> loadButton;
    AutoPtr<IView> loadButton;
    FindViewById(R::id::load_button, (IView**)&loadButton);
    loadButton->SetOnClickListener(mClickListener);

    //AutoPtr<IButton> exitButton;
    AutoPtr<IView> exitButton;
    FindViewById(R::id::exit_button, (IView**)&exitButton);
    exitButton->SetOnClickListener(mClickListener);

    mKeyListener = new CActivityOne::ViewOnKeyListener(this);
    AutoPtr<IView> dataEdit;
    FindViewById(R::id::data, (IView**)&dataEdit);
    dataEdit->SetOnKeyListener(mKeyListener);

    // return NOERROR;
    Init();
    // Loadurl(mWebView, String("http://weibo.cn/"));

    return NOERROR;
}

ECode CActivityOne::Init()
{
    // PRINT_ENTER_LEAVE("CWebViewActivity::Init");
    // // Progress
    // ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);
    // ECode ec = CProgressDialog::New(this, (IProgressDialog**)&pd);
    // PRINT_FILE_LINE_EX("pd = %p, ec = %p", pd.Get(), ec);
    // pd->SetProgressStyle(IProgressDialog::STYLE_SPINNER);
    // AutoPtr<ICharSequence> cs;
    // CStringWrapper::New(String("load data ..."), (ICharSequence**)&cs);
    // pd->SetMessage(cs);
    // ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);

#if 0
    // Show/Hide message
    handler=new Handler(){
        public void handleMessage(Message msg)
        {
            if (!Thread.currentThread().isInterrupted())
            {
                switch (msg.what)
                {
                case 0:
                    pd.show();//show progress dialog
                    break;
                case 1:
                    pd.hide();
                    break;
                }
            }

        super.handleMessage(msg);
        }
    };
#endif

    // CHandler::New((IHandler**)&handler);

    // ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);
    // // WebView
    // FindViewById(R::id::myWv, (IView**)&mWebView);
    // AutoPtr<IWebSettings> webSetting;
    // mWebView->GetSettings((IWebSettings**)&webSetting);
    // webSetting->SetJavaScriptEnabled(TRUE);
    // mWebView->SetScrollBarStyle(0);

    // ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);
#if 0
    mWebView->SetWebViewClient(new WebViewClient(){
        public boolean shouldOverrideUrlLoading(final WebView view, final String url) {
            loadurl(view,url);
            return true;
        }
    });
#endif

    // class InnerWebViewClient : public IWebViewClient
    //                          , public WebViewClient
    //                          , public ElRefBase
    // {
    // public:
    //     InnerWebViewClient(
    //         /* [in] */ CActivityOne* host) : mHost(host)
    //     {}

    //     CARAPI_(PInterface) Probe(
    //             /* [in]  */ REIID riid)
    //     {
    //         return NOERROR;
    //     }

    //     CARAPI_(UInt32) AddRef()
    //     {
    //         return ElRefBase::AddRef();
    //     }

    //     CARAPI_(UInt32) Release()
    //     {
    //         return ElRefBase::Release();
    //     }

    //     CARAPI GetInterfaceID(
    //         /* [in] */ IInterface *Object,
    //         /* [out] */ InterfaceID *IID)
    //     {
    //         return NOERROR;
    //     }

    //     CARAPI OnPageStarted(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ const String& url,
    //         /* [in] */ IBitmap* favicon)
    //     {
    //         return WebViewClient::OnPageStarted(view, url, favicon);
    //     }

    //     CARAPI OnPageFinished(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ const String& url)
    //     {
    //         return WebViewClient::OnPageFinished(view, url);
    //     }

    //     CARAPI OnLoadResource(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ const String& url)
    //     {
    //         return WebViewClient::OnLoadResource(view, url);
    //     }

    //     CARAPI ShouldInterceptRequest(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ const String& url,
    //         /* [out] */ IWebResourceResponse** wrr)
    //     {
    //         VALIDATE_NOT_NULL(wrr);
    //         AutoPtr<IWebResourceResponse> _wrr = WebViewClient::ShouldInterceptRequest(view, url);
    //         *wrr = _wrr.Get();
    //         REFCOUNT_ADD(*wrr);

    //         return NOERROR;
    //     }

    //     CARAPI OnReceivedError(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ Int32 errorCode,
    //         /* [in] */ const String& description,
    //         /* [in] */ const String& failingUrl)
    //     {
    //         return WebViewClient::OnReceivedError(view, errorCode, description, failingUrl);
    //     }

    //     CARAPI DoUpdateVisitedHistory(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ const String& url,
    //         /* [in] */ Boolean isReload)
    //     {
    //         return WebViewClient::DoUpdateVisitedHistory(view, url, isReload);
    //     }

    //     CARAPI OnReceivedHttpAuthRequest(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ IHttpAuthHandler* handler,
    //         /* [in] */ const String& host,
    //         /* [in] */ const String& realm)
    //     {
    //         return WebViewClient::OnReceivedHttpAuthRequest(view, handler, host, realm);
    //     }

    //     CARAPI ShouldOverrideKeyEvent(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ IKeyEvent* event,
    //         /* [out] */ Boolean * result)
    //     {
    //         Boolean b = WebViewClient::ShouldOverrideKeyEvent(view, event);
    //         if (result) *result = b;

    //         return NOERROR;
    //     }

    //     CARAPI OnUnhandledKeyEvent(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ IKeyEvent* event)
    //     {
    //         return WebViewClient::OnUnhandledKeyEvent(view, event);
    //     }

    //     CARAPI OnScaleChanged(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ Float oldScale,
    //         /* [in] */ Float newScale)
    //     {
    //         return WebViewClient::OnScaleChanged(view, oldScale, newScale);
    //     }

    //     CARAPI OnReceivedLoginRequest(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ const String& realm,
    //         /* [in] */ const String& account,
    //         /* [in] */ const String& args)
    //     {
    //         return WebViewClient::OnReceivedLoginRequest(view, realm, account, args);
    //     }

    //     CARAPI ShouldOverrideUrlLoading(
    //         /* [in] */ IWebView* view,
    //         /* [in] */ const String& url,
    //         /* [out] */ Boolean* result)
    //     {
    //         mHost->Loadurl(view, url);
    //         if (result) *result = TRUE;
    //     }

    // private:
    //     AutoPtr<CActivityOne> mHost;
    // };

    // AutoPtr<IWebViewClient> wc = new InnerWebViewClient(this);

    // mWebView->SetWebViewClient(wc);

#if 0
    mWebView.setWebChromeClient(new WebChromeClient(){
        public void onProgressChanged(WebView view,int progress){
            if(progress==100){
                handler.sendEmptyMessage(1);
            }
                super.onProgressChanged(view, progress);
        }
    });
#endif

    class InnerWebChromeClient : public IWebChromeClient
                               , public WebChromeClient
                               , public ElRefBase
    {
    public:
        InnerWebChromeClient(
           /* [in] */ CActivityOne* host) : mHost(host)
         {}

        CARAPI_(PInterface) Probe(
                /* [in]  */ REIID riid)
        {
            return NOERROR;
        }

        CARAPI_(UInt32) AddRef()
        {
            return ElRefBase::AddRef();
        }

        CARAPI_(UInt32) Release()
        {
            return ElRefBase::Release();
        }

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *Object,
            /* [out] */ InterfaceID *IID)
        {
            return NOERROR;
        }

        CARAPI OnProgressChanged(
            /* [in] */ IWebView* view,
            /* [in] */ Int32 progress)
        {
            if(progress==100){
//                handler.sendEmptyMessage(1);
                mHost->SendMessage(1);
            }

            WebChromeClient::OnProgressChanged(view, progress);

            return NOERROR;
        }

        CARAPI OnReceivedTitle(
            /* [in] */ IWebView* view,
            /* [in] */ const String& title)
        {
            return WebChromeClient::OnReceivedTitle(view, title);
        }

        CARAPI OnReceivedIcon(
            /* [in] */ IWebView* view,
            /* [in] */ IBitmap* icon)
        {
            return WebChromeClient::OnReceivedIcon(view, icon);
        }

        CARAPI OnReceivedTouchIconUrl(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ Boolean precomposed)
        {
            return WebChromeClient::OnReceivedTouchIconUrl(view, url, precomposed);
        }

        CARAPI OnShowCustomView(
            /* [in] */ IView* view,
            /* [in] */ IWebChromeClientCustomViewCallback* callBack)
        {
            return WebChromeClient::OnShowCustomView(view, callBack);
        }

        CARAPI OnShowCustomView(
            /* [in] */ IView* view,
            /* [in] */ Int32 requestedOrientation,
            /* [in] */ IWebChromeClientCustomViewCallback* callBack)
        {
            return WebChromeClient::OnShowCustomView(view, requestedOrientation, callBack);
        }

        CARAPI OnHideCustomView()
        {
            return WebChromeClient::OnHideCustomView();
        }

        CARAPI OnRequestFocus(
            /* [in] */ IWebView* view)
        {
            return WebChromeClient::OnRequestFocus(view);
        }

        CARAPI OnCloseWindow(
            /* [in] */ IWebView* window)
        {
            return WebChromeClient::OnCloseWindow(window);
        }

        CARAPI OnJsAlert(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ IJsResult* result,
            /* [out] */ Boolean* flag)
        {
            Boolean b = WebChromeClient::OnJsAlert(view, url, message, result);
            if (flag) *flag = b;
            return NOERROR;
        }

        CARAPI OnJsConfirm(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ IJsResult* result,
            /* [out] */ Boolean* flag)
         {
             Boolean b = WebChromeClient::OnJsConfirm(view, url, message, result);
             if (flag) *flag = b;
             return NOERROR;
         }

         CARAPI OnJsPrompt(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ const String& defaultValue,
            /* [in] */ IJsPromptResult* result,
            /* [out] */ Boolean* flag)
        {
            Boolean b = WebChromeClient::OnJsPrompt(view, url, message, defaultValue, result);
            if (flag) *flag = b;
            return NOERROR;
        }

         CARAPI OnJsBeforeUnload(
            /* [in] */ IWebView* view,
            /* [in] */ const String& url,
            /* [in] */ const String& message,
            /* [in] */ IJsResult* result,
            /* [out] */ Boolean* flag)
        {
            Boolean b = WebChromeClient::OnJsBeforeUnload(view, url, message, result);
            if (flag) *flag = b;
            return NOERROR;
        }

        CARAPI OnExceededDatabaseQuota(
            /* [in] */ const String& url,
            /* [in] */ const String& databaseIdentifier,
            /* [in] */ Int64 quota,
            /* [in] */ Int64 estimatedDatabaseSize,
            /* [in] */ Int64 totalQuota,
            /* [in] */ IWebStorageQuotaUpdater* quotaUpdater)
         {
             return WebChromeClient::OnExceededDatabaseQuota(url, databaseIdentifier,
                 quota, estimatedDatabaseSize, totalQuota, quotaUpdater);
         }

         CARAPI OnReachedMaxAppCacheSize(
             /* [in] */ Int64 requiredStorage,
             /* [in] */ Int64 quota,
             /* [in] */ IWebStorageQuotaUpdater* quotaUpdater)
        {
            return WebChromeClient::OnReachedMaxAppCacheSize(requiredStorage,
             quota, quotaUpdater);
        }

        CARAPI OnGeolocationPermissionsShowPrompt(
            /* [in] */ const String& origin,
            /* [in] */ IGeolocationPermissionsCallback* callBack)
        {
            return WebChromeClient::OnGeolocationPermissionsShowPrompt(origin, callBack);
        }

        CARAPI OnGeolocationPermissionsHidePrompt()
        {
            return WebChromeClient::OnGeolocationPermissionsHidePrompt();
        }

        CARAPI OnJsTimeout(
            /* [out] */ Boolean* result)
        {
            Boolean b = WebChromeClient::OnJsTimeout();
            if (result) *result = b;
            return NOERROR;
        }

        CARAPI OnConsoleMessage(
            /* [in] */ const String& message,
            /* [in] */ Int32 lineNumber,
            /* [in] */ const String& sourceID)
        {
            return WebChromeClient::OnConsoleMessage(message, lineNumber, sourceID);
        }

        CARAPI OnConsoleMessage(
            /* [in] */ IConsoleMessage* consoleMessage,
            /* [out] */ Boolean* result)
        {
            Boolean b = WebChromeClient::OnConsoleMessage(consoleMessage);
            if (result) *result = b;
            return NOERROR;
        }

        CARAPI GetDefaultVideoPoster(
             /* [out] */ IBitmap** bitmap)
        {
            VALIDATE_NOT_NULL(bitmap);
            AutoPtr<IBitmap> bm = WebChromeClient::GetDefaultVideoPoster();
            *bitmap = bm.Get();
            REFCOUNT_ADD(*bitmap);

            return NOERROR;
        }

        CARAPI GetVideoLoadingProgressView(
            /* [out] */ IView** view)
        {
            VALIDATE_NOT_NULL(view);
            AutoPtr<IView> _view = WebChromeClient::GetVideoLoadingProgressView();
            *view = _view.Get();
            REFCOUNT_ADD(*view);

            return NOERROR;
        }

        CARAPI GetVisitedHistory(
            /* [in] */ IValueCallback* callBack)
        {
            return WebChromeClient::GetVisitedHistory(callBack);
        }

        CARAPI OpenFileChooser(
            /* [in] */ IValueCallback* uploadFile,
            /* [in] */ const String& acceptType,
            /* [in] */ const String& capture)
        {
            return WebChromeClient::OpenFileChooser(uploadFile, acceptType, capture);
        }

        CARAPI OnCreateWindow(
            /* [in] */ IWebView* view,
            /* [in] */ Boolean isDialog,
            /* [in] */ Boolean isUserGesture,
            /* [in] */ IMessage* resultMsg,
            /* [out] */ Boolean* result)
        {
            return NOERROR;
        }

        CARAPI SetupAutoFill(
            /* [in] */ IMessage* msg)
        {
            return NOERROR;
        }

    private:
        AutoPtr<CActivityOne> mHost;
    };

    AutoPtr<IWebChromeClient> wcc = new InnerWebChromeClient(this);
    mWebView->SetWebChromeClient(wcc);

    return NOERROR;
}

ECode CActivityOne::OnKeyDown(
     Int32 keyCode,
     IKeyEvent* event,
     Boolean* result)
{
    Boolean b = FALSE;
    if ((keyCode == IKeyEvent::KEYCODE_BACK)
        && (mWebView->CanGoBack(&b), b)) {
        ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);
        mWebView->GoBack();

        if (result) *result = TRUE;

        return NOERROR;
    }else if(keyCode == IKeyEvent::KEYCODE_BACK){
        ALOGD("====== File: %s, Line: %d ======", __FILE__, __LINE__);
        ConfirmExit();
        if (result) *result = TRUE;

        return NOERROR;
    }

    //0x52 == 82 KEYCODE_MENU
    ALOGD("====== File: %s, Line: %d ======, keycode: 0x%x ", __FILE__, __LINE__, keyCode);
    return Activity::OnKeyDown(keyCode, event, result);
}

ECode CActivityOne::ConfirmExit()
{
    //Finish();
    //AlertDialog.Builder ad=new AlertDialog.Builder(main.this);
    AutoPtr<IAlertDialogBuilder> adb;
    CAlertDialogBuilder::New((IContext*)(this->Probe(EIID_IContext)), (IAlertDialogBuilder**)&adb);
    if (!adb)
    {
        ALOGD("====== File: %s, Line: %d ======, AlertDialogBuilder create fail!", __FILE__, __LINE__);
        return NOERROR;
    }
    AutoPtr<ICharSequence> cs;
    CStringWrapper::New(String("Exit!"), (ICharSequence**)&cs);
    adb->SetTitle(cs);

    cs = NULL;
    CStringWrapper::New(String("is Exit?"), (ICharSequence**)&cs);
    adb->SetMessage(cs);

    AutoPtr<DialogOnClickListener> listener = new CActivityOne::DialogOnClickListener(this);

    IDialogInterfaceOnClickListener* clickListener = (IDialogInterfaceOnClickListener*)(listener.Get());

    cs = NULL;
    CStringWrapper::New(String("确定"), (ICharSequence**)&cs);
    adb->SetPositiveButton(cs, clickListener);

    //ad.setPositiveButton("Yes", new DialogInterface.OnClickListener() {
    //    @Override
    //    public void onClick(DialogInterface dialog, int i) {
    //        // TODO Auto-generated method stub
    //        main.this.finish();

    //    }
    //});

    //ad.setNegativeButton("No",new DialogInterface.OnClickListener() {
    //    @Override
    //    public void onClick(DialogInterface dialog, int i) {

    //    }
    //});
    cs = NULL;
    CStringWrapper::New(String("cancel"), (ICharSequence**)&cs);
    adb->SetNegativeButton(cs, clickListener);

    AutoPtr<IAlertDialog> dlg;
    adb->Create((IAlertDialog**)&dlg);
    dlg->Show();

    //ad.show();
    return NOERROR;
}

ECode CActivityOne::Loadurl(
    /* [in] */ IWebView* view,
    /* [in] */ const String& url)
{
#if 0
    new Thread(){
        public void run(){
            handler.sendEmptyMessage(0);
            view.loadUrl(url);//ÔØÈëÍøÒ³
        }
    }.start();
#endif

//     class InnerThread : public Thread
//     {
//     public:
//         InnerThread(
//             /* [in] */ CActivityOne* host,
//             /* [in] */ IWebView* _view,
//             /* [in] */ const String& _url)
//             : view(_view)
//             , url(_url)
//             , mHost(host)
//         {}

//         CARAPI_(PInterface) Probe(
//                 /* [in]  */ REIID riid)
//         {
//             return NOERROR;
//         }

//         CARAPI Run()
//         {
// //            handler.sendEmptyMessage(0);
//             mHost->SendMessage(0);
//             view->LoadUrl(url);
//             return NOERROR;
//         };

//     private:
//         CARAPI_(Object*) GetSelfLock()
//         {
//             return &mMutex;
//         }

//     private:
//         Object mMutex;
//         AutoPtr<IWebView> view;
//         String url;
//         AutoPtr<CActivityOne> mHost;
//     };

//     InnerThread(this, view, url).Start();

    return NOERROR;
}

ECode CActivityOne::OnCreateContextMenu(
            /* [in] */ IContextMenu* menu,
                /* [in] */ IView* v,
                    /* [in] */ IContextMenuInfo* menuInfo)
{
    ALOGD("CActivityOne CActivityOne::OnCreateContextMenu");

    AutoPtr<IMenuItem> item = NULL;
    AutoPtr<ICharSequence> csq;
    CStringWrapper::New(String("ctxItem1"), (ICharSequence**)&csq);
    menu->Add(0, 1, 1, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem2"), (ICharSequence**)&csq);
    menu->Add(0, 2, 2, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem3"), (ICharSequence**)&csq);
    menu->Add(0, 3, 3, csq, (IMenuItem**)&item);

    return NOERROR;
}

ECode CActivityOne::ButtonOnClickListener::OnClick(
    IView* v)
{
    Int32 viewId = 0;

    ALOGD("====== File: %s, Line: %d ======, enter onclick", __FILE__, __LINE__);
    v->GetId(&viewId);
    switch(viewId)
    {
        case R::id::goto_button1:
            {
                ALOGD("====== File: %s, Line: %d ======, view: %s ", __FILE__, __LINE__, "goto_button1");
                AutoPtr<IView> myUri;
                mOwner->FindViewById(R::id::uri1, (IView**)&myUri);
                AutoPtr<IEditText> text = IEditText::Probe(myUri);
                String uri;
                AutoPtr<ICharSequence> ttt;
                text->GetText((ICharSequence**)&ttt);
                ttt->ToString(&uri);
                ALOGD("====== File: %s, Line: %d ======, uri: %s ", __FILE__, __LINE__, uri.string());
                mOwner->mWebView->LoadUrl(uri);
                ALOGD("====== File: %s, Line: %d ======, ", __FILE__, __LINE__);
                break;
            }
        case R::id::goto_button2:
            {
                ALOGD("====== File: %s, Line: %d ======, view: %s ", __FILE__, __LINE__, "goto_button2");
                AutoPtr<IView> myUri;
                mOwner->FindViewById(R::id::uri2, (IView**)&myUri);
                AutoPtr<IEditText> text = IEditText::Probe(myUri);
                String uri;
                AutoPtr<ICharSequence> ttt;
                text->GetText((ICharSequence**)&ttt);
                ttt->ToString(&uri);
                ALOGD("====== File: %s, Line: %d ======, uri: %s ", __FILE__, __LINE__, uri.string());
                mOwner->mWebView->LoadUrl(uri);
                ALOGD("====== File: %s, Line: %d ======, ", __FILE__, __LINE__);
                break;
            }
        case R::id::goto_button3:
            {
                ALOGD("====== File: %s, Line: %d ======, view: %s ", __FILE__, __LINE__, "goto_button3");
                AutoPtr<IView> myUri;
                mOwner->FindViewById(R::id::uri3, (IView**)&myUri);
                AutoPtr<IEditText> text = IEditText::Probe(myUri);
                String uri;
                AutoPtr<ICharSequence> ttt;
                text->GetText((ICharSequence**)&ttt);
                ttt->ToString(&uri);
                ALOGD("====== File: %s, Line: %d ======, uri: %s ", __FILE__, __LINE__, uri.string());
                mOwner->mWebView->LoadUrl(uri);
                ALOGD("====== File: %s, Line: %d ======, ", __FILE__, __LINE__);
                break;
            }
        case R::id::load_button:
            {
                AutoPtr<IView> myData;
                mOwner->FindViewById(R::id::data, (IView**)&myData);
                AutoPtr<IEditText> text = IEditText::Probe(myData);
                String data;
                AutoPtr<ICharSequence> ttt;
                text->GetText((ICharSequence**)&ttt);
                ttt->ToString(&data);
                ALOGD("====== File: %s, Line: %d ======, view: %s data: %s ", __FILE__, __LINE__, "load_button", data.string());
                //if (data.IsNullOrEmpty())
                data = String("<html><body bgcolor=\"#FFCC80\"><b>Hello World!</b></body></html>");
                String mimeType("text/html");
                String encoding("utf-8");
                ALOGD("====== File: %s, Line: %d ======, view: %s data: %s ", __FILE__, __LINE__, "load_button", data.string());
                mOwner->mWebView->LoadData(data, mimeType, encoding);
                break;
            }
        case R::id::exit_button:
            ALOGD("====== File: %s, Line: %d ======, view: %s ", __FILE__, __LINE__, "exit_button");
            mOwner->Finish();
            break;
        default:
            ALOGD("====== File: %s, Line: %d ======, viewID: %x ", __FILE__, __LINE__, viewId);
    }
    return NOERROR;
}

ECode CActivityOne::DialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    ALOGD("CActivityOne::DialogOnClickListener::OnClick");
    switch (which) {
        case IDialogInterface::BUTTON_POSITIVE:
            ALOGD("CActivityOne  点击了确定按钮");
            mOwner->Finish();
            break;
        case IDialogInterface::BUTTON_NEGATIVE:
            ALOGD("CActivityOne  点击了取消按钮");
            break;
        case IDialogInterface::BUTTON_NEUTRAL:
            ALOGD("CActivityOne  点击了中立按钮");
            break;
        default:
            break;
    }

    return NOERROR;
}

ECode CActivityOne::ViewOnKeyListener::OnKey(
    /* [in] */ IView * view,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    Int32 id;
    view->GetId(&id);

    Int32 action;
    event->GetAction(&action);
    Char32 label;
    event->GetDisplayLabel(&label);

    ALOGD("CActivityOne::MyListener::OnKey: view %08x, keyCode: %d, '%c'",
                id, keyCode, (char)label);

    if (action == IKeyEvent::ACTION_DOWN) {
            if (keyCode == IKeyEvent::KEYCODE_0) {
                        ALOGD("Key 0 is down.");
                            }
    }
    else if (action == IKeyEvent::ACTION_UP) {
    }

    if (result) {
            *result = FALSE;
    }

    return NOERROR;
}


} // namespace WebViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
