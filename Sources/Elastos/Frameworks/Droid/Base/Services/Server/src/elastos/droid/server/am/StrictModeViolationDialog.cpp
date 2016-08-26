#include "Elastos.Droid.Utility.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/server/am/StrictModeViolationDialog.h"
#include "elastos/droid/R.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

StrictModeViolationDialog::MyHandler::MyHandler(
    /* [in] */ StrictModeViolationDialog* owner)
    : mOwner(owner)
{
    Handler::constructor();
}

StrictModeViolationDialog::MyHandler::~MyHandler()
{}

ECode StrictModeViolationDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    {
        AutoLock lock(mOwner->mService);
        if (mOwner->mProc != NULL && mOwner->mProc->mCrashDialog == mOwner) {
            mOwner->mProc->mCrashDialog = NULL;
        }
    }
    Int32 what;
    msg->GetWhat(&what);
    mOwner->mResult->SetResult(what);

    // If this is a timeout we won't be automatically closed, so go
    // ahead and explicitly dismiss ourselves just in case.
    mOwner->Dismiss();
    return NOERROR;
}


const String StrictModeViolationDialog::TAG("StrictModeViolationDialog");
const Int32 StrictModeViolationDialog::ACTION_OK = 0;
const Int32 StrictModeViolationDialog::ACTION_OK_AND_REPORT = 1;
const Int64 StrictModeViolationDialog::DISMISS_TIMEOUT = 1000 * 60 * 1;

ECode StrictModeViolationDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CActivityManagerService* service,
    /* [in] */ AppErrorResult* result,
    /* [in] */ ProcessRecord* app)
{
    FAIL_RETURN(BaseErrorDialog::Init(context))
    mService = service;
    mResult = result;
    mProc = app;
    mHandler = new MyHandler(this);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<ICharSequence> name;
    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    AutoPtr<ICharSequence> label;
    pkgManager->GetApplicationLabel(app->mInfo, (ICharSequence**)&label);
    Int32 size;
    if (((app->mPkgList->GetSize(&size), size) == 1) &&
            (name = label) != NULL) {
        String processName;
        app->mInfo->GetProcessName(&processName);
        String resString;
        AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
        AutoPtr<ICharSequence> pNameCs;
        CString::New(processName, (ICharSequence**)&pNameCs);
        args->Set(0, (IInterface*)name.Get());
        args->Set(1, (IInterface*)pNameCs.Get());
        res->GetString(R::string::smv_application, args, &resString);
        assert(0);
        AutoPtr<ICharSequence> resCs;
        CString::New(resString,(ICharSequence**)&resCs);
        SetMessage(resCs);
    }
    else {
        res->GetString(R::string::smv_process, &(app->mProcessName));
        AutoPtr<ICharSequence> name;
        CString::New(app->mProcessName, (ICharSequence**)&name);
        SetMessage(name);
    }

    SetCancelable(FALSE);

    AutoPtr<ICharSequence> text;
    res->GetText(R::string::dlg_ok, (ICharSequence**)&text);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(ACTION_OK, (IMessage**)&msg);
    SetButton(IDialogInterface::BUTTON_POSITIVE, text, msg);

    if (app->mErrorReportReceiver != NULL) {
        AutoPtr<ICharSequence> text;
        res->GetText(R::string::report, (ICharSequence**)&text);
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(ACTION_OK_AND_REPORT, (IMessage**)&msg);
        msg->SetTarget(mHandler);
        SetButton(IDialogInterface::BUTTON_NEGATIVE, text, msg);
    }

    AutoPtr<ICharSequence> title;
    res->GetText(R::string::aerr_title, (ICharSequence**)&title);
    SetTitle(title);
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->AddFlags(IWindowManagerLayoutParams::PRIVATE_FLAG_SYSTEM_ERROR);
    String processName;
    app->mInfo->GetProcessName(&processName);
    AutoPtr<ICharSequence> cs;
    CString::New(String("Application Error: ") + processName, (ICharSequence**)&cs);
    window->SetTitle(cs);

    // After the timeout, pretend the user clicked the quit button
    msg = NULL;
    mHandler->ObtainMessage(ACTION_OK, (IMessage**)&msg);
    Boolean posted;
    mHandler->SendMessageDelayed(msg, DISMISS_TIMEOUT, &posted);

    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
