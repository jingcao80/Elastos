
#include "elastos/droid/server/am/AppWaitingForDebuggerDialog.h"

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

AppWaitingForDebuggerDialog::AppWaitingForDebuggerDialog(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IContext* context,
    /* [in] */ ProcessRecord* app)
    : mService(service)
    , mProc(app)
{
    BaseErrorDialog::Init(context);
    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    pkgManager->GetApplicationLabel(app->mInfo, (ICharSequence**)&mAppName);

    SetCancelable(FALSE);

    StringBuilder text;
    Int32 length;
    if (mAppName != NULL && (mAppName->GetLength(&length), length > 0)) {
        text.AppendCStr("Application ");
        text.AppendCharSequence(mAppName);
        text.AppendCStr(" (process ");
        text.AppendString(app->mProcessName);
        text.AppendCStr(")");
    } else {
        text.AppendCStr("Process ");
        text.AppendString(app->mProcessName);
    }

    text.AppendCStr(" is waiting for the debugger to attach.");

    AutoPtr<ICharSequence> textMsg;
    CString::New(text.ToString(), (ICharSequence**)&textMsg);
    SetMessage(textMsg);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(1, (IInterface*)app, (IMessage**)&msg);
    AutoPtr<ICharSequence> btMsg;
    CString::New(String("Force Close"), (ICharSequence**)&btMsg);
    SetButton(IDialogInterface::BUTTON_POSITIVE, btMsg, msg);
    AutoPtr<ICharSequence> title;
    CString::New(String("Waiting For Debugger"), (ICharSequence**)&title);
    SetTitle(title);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    Dialog::GetWindow()->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    String processName;
    app->mInfo->GetProcessName(&processName);
    AutoPtr<ICharSequence> attrsTitle;
    CString::New(String("Waiting For Debugger: ") + processName, (ICharSequence**)&attrsTitle);
    attrs->SetTitle(attrsTitle);
    Dialog::GetWindow()->SetAttributes(attrs);
}

void AppWaitingForDebuggerDialog::OnStop()
{}

ECode AppWaitingForDebuggerDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case 1:
            // Kill the application.
            mHost->mService->KillAppAtUsersRequest(mHost->mProc, mHost);
            break;
    }
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
