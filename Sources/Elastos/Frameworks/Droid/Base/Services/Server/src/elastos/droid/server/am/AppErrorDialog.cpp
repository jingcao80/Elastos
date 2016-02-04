
#include "elastos/droid/R.h"
#include "elastos/droid/server/am/AppErrorDialog.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::R;
using Elastos::Droid::View::IWindow;

using Elastos::Core::AutoLock;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

Int32 const AppErrorDialog::FORCE_QUIT = 0;
Int32 const AppErrorDialog::FORCE_QUIT_AND_REPORT = 1;
Int64 const AppErrorDialog::DISMISS_TIMEOUT = 1000 * 60 * 5;

AppErrorDialog::MyHandler::MyHandler(
    /* [in] */ AppErrorDialog* host)
    : Handler(FALSE)
    , mHost(host)
{}

AppErrorDialog::MyHandler::~MyHandler()
{}

ECode AppErrorDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    {
        AutoLock lock(mHost->mService);
        if (mHost->mProc != NULL && mHost->mProc->mCrashDialog == mHost) {
            mHost->mProc->mCrashDialog = NULL;
        }
    }
    Int32 type;
    msg->GetWhat(&type);
    mHost->mResult->SetResult(type);

    // Make sure we don't have time timeout still hanging around.
    RemoveMessages(FORCE_QUIT);

    // If this is a timeout we won't be automatically closed, so go
    // ahead and explicitly dismiss ourselves just in case.
    mHost->Dismiss();
    return NOERROR;
}


AppErrorDialog::AppErrorDialog(
    /* [in] */ IContext* context,
    /* [in] */ CActivityManagerService* service,
    /* [in] */ AppErrorResult* result,
    /* [in] */ ProcessRecord* app)
    : mService(service)
    , mResult(result)
    , mProc(app)
{
    BaseErrorDialog::Init(context);
    mHandler = new MyHandler(this);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<ICharSequence> name;

    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    pkgManager->GetApplicationLabel(app->mInfo, (ICharSequence**)&name);
    Int32 size;
    if (((app->mPkgList->GetSize(&size), size) == 1) && name!= NULL) {
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        args->Set(0, (IInterface*)name.Get());
        String pName;
        app->mInfo->GetProcessName(&pName);
        AutoPtr<ICharSequence> pNamecs;
        CString::New(pName, (ICharSequence**)&pNamecs);
        args->Set(1, (IInterface*)pNamecs.Get());
        String resStr;
        res->GetString(R::string::aerr_application, args, &resStr);
        AutoPtr<ICharSequence> resCs;
        CString::New(resStr, (ICharSequence**)&resCs);
        SetMessage(resCs);
    }
    else {
        CString::New(app->mProcessName, (ICharSequence**)&name);
        String s;
        AutoPtr<ArrayOf<IInterface*> > nameArray = ArrayOf<IInterface*>::Alloc(1);
        nameArray->Set(0, (IInterface*)name.Get());
        res->GetString(R::string::aerr_process,
                nameArray, &s);
        AutoPtr<ICharSequence> cs;
        CString::New(s, (ICharSequence**)&cs);
        SetMessage(cs);
    }

    SetCancelable(FALSE);

    AutoPtr<ICharSequence> text;
    res->GetText(R::string::force_close, (ICharSequence**)&text);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(FORCE_QUIT, (IMessage**)&msg);
    SetButton(IDialogInterface::BUTTON_POSITIVE, text, msg);

    text = NULL;
    res->GetText(R::string::report, (ICharSequence**)&text);
    msg = NULL;
    mHandler->ObtainMessage(FORCE_QUIT_AND_REPORT, (IMessage**)&msg);
    if (app->mErrorReportReceiver != NULL) {
        SetButton(IDialogInterface::BUTTON_NEGATIVE, text, msg);
    }

    AutoPtr<ICharSequence> title;
    res->GetText(R::string::aerr_title, (ICharSequence**)&title);
    SetTitle(title);
    AutoPtr<IWindow> window;
    Dialog::GetWindow((IWindow**)&window);
    //window->AddFlags(IViewGroupLayoutParams::FLAG_SYSTEM_ERROR);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    String processName;
    app->mInfo->GetProcessName(&processName);
    AutoPtr<ICharSequence> titlecs;
    CString::New(String("Application Error: ") + processName, (ICharSequence**)&titlecs);
    attrs->SetTitle(titlecs);
    Int32 flags;
    attrs->GetPrivateFlags(&flags);
    attrs->SetPrivateFlags(flags
            | IWindowManagerLayoutParams::PRIVATE_FLAG_SYSTEM_ERROR
            | IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS);
    window->SetAttributes(attrs);
    if (app->mPersistent) {
        window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR);
    }

    // After the timeout, pretend the user clicked the quit button
    msg = NULL;
    mHandler->ObtainMessage(FORCE_QUIT, (IMessage**)&msg);
    Boolean sended;
    mHandler->SendMessageDelayed(msg, DISMISS_TIMEOUT, &sended);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
