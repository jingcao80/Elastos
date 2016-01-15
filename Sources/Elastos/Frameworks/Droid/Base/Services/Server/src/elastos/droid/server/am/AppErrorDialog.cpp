
#include "elastos/droid/server/am/AppErrorDialog.h"

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

Int32 const AppErrorDialog::FORCE_QUIT = 0;
Int32 const AppErrorDialog::FORCE_QUIT_AND_REPORT = 1;
Int64 const AppErrorDialog::DISMISS_TIMEOUT = 1000 * 60 * 5;

AppErrorDialog::MyHandler::MyHandler(
    /* [in] */ AppErrorDialog* host)
    : HandlerBase(FALSE)
    , mHost(host)
{}

AppErrorDialog::MyHandler::~MyHandler()
{}

ECode AppErrorDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    {
        AutoLock lock(mHost->mService->mLock);
        if (mHost->mProc != NULL && mHost->mProc->mCrashDialog == mHost) {
            mHost->mProc->mCrashDialog = NULL;
        }
    }
    Int32 type;
    msg->GetWhat(&type);
    mHost->mResult->SetResult(type);

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
    if ((app->mPkgList.GetSize() == 1) && name!= NULL) {
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
    Dialog::GetWindow()->AddFlags(IWindowManagerLayoutParams::FLAG_SYSTEM_ERROR);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    Dialog::GetWindow()->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    String processName;
    app->mInfo->GetProcessName(&processName);
    AutoPtr<ICharSequence> titlecs;
    CString::New(String("Application Error: ") + processName, (ICharSequence**)&titlecs);
    attrs->SetTitle(titlecs);
    Int32 flags;
    attrs->GetPrivateFlags(&flags);
    attrs->SetPrivateFlags(flags | IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS);
    Dialog::GetWindow()->SetAttributes(attrs);
    if (app->mPersistent) {
        Dialog::GetWindow()->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR);
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
