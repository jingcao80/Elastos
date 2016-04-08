
#include <ext/frameworkext.h>
#include "CActivityOne.h"
#include "R.h"
#include "webkit/WebSettingsClassic.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

#include <cutils/log.h>

using Elastos::Core::CStringWrapper;
using Elastos::Core::ICharSequence;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Webkit::IConsoleMessage;
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
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::CEditText;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WebViewDemo {

CAR_INTERFACE_IMPL_LIGHT(CActivityOne::ButtonOnClickListener, IViewOnClickListener);
CAR_INTERFACE_IMPL_LIGHT(CActivityOne::DialogOnClickListener, IDialogInterfaceOnClickListener);
CAR_INTERFACE_IMPL_LIGHT(CActivityOne::ViewOnKeyListener, IViewOnKeyListener);

static const String WVD_TAG("WebViewDemo");

PInterface CActivityOne::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IObjectFactory) {
        return (IObjectFactory *)this;
    }
    else if (riid == EIID_ILogger) {
        return (ILogger *)this;
    }
    else return Activity::Probe(riid);
}

UInt32 CActivityOne::AddRef()
{
    return Activity::AddRef();
}

UInt32 CActivityOne::Release()
{
    return Activity::Release();
}

ECode CActivityOne::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(IObjectFactory *)this) {
        *iid = EIID_IObjectFactory;
        return NOERROR;
    }
    if (object == (IInterface*)(ILogger*)this) {
        *iid = EIID_ILogger;
        return NOERROR;
    }
    else return Activity::GetInterfaceID(object, iid);
}

ECode CActivityOne::GetClassID(
    /* [out] */ ClassID *pCLSID)
{
    VALIDATE_NOT_NULL(pCLSID);
    *pCLSID =  ECLSID_CActivityOne;
    return NOERROR;
}

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
            mDialog->Show();//show progress dialog
            break;

        case 1:
            mDialog->Hide();
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
    // handler->PostCppCallback((Handle32)handler.Get(), *(Handle32*)&pHandlerFunc,
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
    mWebView = IWebView::Probe(view);

    AutoPtr<IWebSettingsClassic> settings;
    mWebView->GetSettings((IWebSettings**)&settings);
    settings->SetJavaScriptEnabled(TRUE);
    settings->SetPluginsEnabled(TRUE);

    mClickListener = new ButtonOnClickListener(this);
    AutoPtr<IView> gotoButton;
    gotoButton = NULL;
    FindViewById(R::id::goto_button1, (IView**)&gotoButton);
    gotoButton->SetOnClickListener(mClickListener);

    AutoPtr<IView> exitButton;
    FindViewById(R::id::exit_button, (IView**)&exitButton);
    exitButton->SetOnClickListener(mClickListener);

    AutoPtr<IView> testButton;
    FindViewById(R::id::test_button, (IView**)&testButton);
    testButton->SetOnClickListener(mClickListener);

    mWebView->AddJavascriptInterface(this, String("CarObjectFactory"));
    mWebView->AddJavascriptInterface(this, String("CarLogger"));

    return NOERROR;
}

ECode CActivityOne::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    Boolean b = FALSE;
    if ((keyCode == IKeyEvent::KEYCODE_BACK)
        && (mWebView->CanGoBack(&b), b)) {
        mWebView->GoBack();
        *result = TRUE;
        return NOERROR;
    }
    else if(keyCode == IKeyEvent::KEYCODE_BACK) {
        ConfirmExit();
        *result = TRUE;
        return NOERROR;
    }

    return Activity::OnKeyDown(keyCode, event, result);
}

ECode CActivityOne::ConfirmExit()
{
    AutoPtr<IAlertDialogBuilder> adb;
    CAlertDialogBuilder::New((IContext*)(this->Probe(EIID_IContext)), (IAlertDialogBuilder**)&adb);
    if (!adb) {
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

    AutoPtr<IDialogInterfaceOnClickListener> clickListener = IDialogInterfaceOnClickListener::Probe(listener);

    cs = NULL;
    CStringWrapper::New(String("确定"), (ICharSequence**)&cs);
    adb->SetPositiveButton(cs, clickListener);

    cs = NULL;
    CStringWrapper::New(String("cancel"), (ICharSequence**)&cs);
    adb->SetNegativeButton(cs, clickListener);

    AutoPtr<IAlertDialog> dlg;
    adb->Create((IAlertDialog**)&dlg);
    dlg->Show();

    return NOERROR;
}

ECode CActivityOne::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    ALOGD("CActivityOne CActivityOne::OnCreateContextMenu");

    AutoPtr<ICharSequence> csq;
    CStringWrapper::New(String("ctxItem1"), (ICharSequence**)&csq);
    AutoPtr<IMenuItem> item;
    menu->Add(0, 1, 1, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem2"), (ICharSequence**)&csq);
    menu->Add(0, 2, 2, csq, (IMenuItem**)&item);

    csq = NULL; item = NULL;
    CStringWrapper::New(String("ctxItem3"), (ICharSequence**)&csq);
    menu->Add(0, 3, 3, csq, (IMenuItem**)&item);

    return NOERROR;
}

ECode CActivityOne::CreateInstance(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [out] */ IInterface** object)
{
    assert(object != NULL);

    AutoPtr<IModuleInfo> moduleInfo;
    ECode ec = _CReflector_AcquireModuleInfo(
            moduleName.string(), (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    AutoPtr<IClassInfo> classInfo;
    ec = moduleInfo->GetClassInfo(
            className.string(), (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        ALOGD("Acquire \"%s\" class info failed!\n", className.string());
        return ec;
    }

    AutoPtr<IInterface> testObject;
    ec = classInfo->CreateObject((IInterface**)&testObject);
    if (FAILED(ec)) {
        ALOGD("Create object failed!\n");
        return ec;
    }

    *object = testObject;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CActivityOne::Log(
    /* [in] */ const String& message)
{
    ALOGD("==== Log ==== message: %s ====", message.string());
    return NOERROR;
}

ECode CActivityOne::ButtonOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId = 0;
    v->GetId(&viewId);
    switch(viewId)
    {
        case R::id::goto_button1:
        {
            AutoPtr<IView> myUri;
            mHost->FindViewById(R::id::uri1, (IView**)&myUri);
            AutoPtr<IEditText> text = IEditText::Probe(myUri);
            AutoPtr<ICharSequence> ttt;
            text->GetText((ICharSequence**)&ttt);
            String uri;
            ttt->ToString(&uri);
            ALOGD("====== File: %s, Line: %d ======, uri: %s ", __FILE__, __LINE__, uri.string());
            mHost->mWebView->LoadUrl(uri);
            ALOGD("====== File: %s, Line: %d ======, ", __FILE__, __LINE__);
            break;
        }
        case R::id::test_button:
            break;
        case R::id::exit_button:
            mHost->Finish();
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
            this->mHost->Finish();
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

    *result = FALSE;

    return NOERROR;
}

} // namespace WebViewDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
