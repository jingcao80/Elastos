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

#include "elastos/droid/server/am/AppWaitingForDebuggerDialog.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ECode AppWaitingForDebuggerDialog::constructor(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IContext* context,
    /* [in] */ ProcessRecord* app)
{
    FAIL_RETURN(BaseErrorDialog::Init(context))
    mService = service;
    mProc = app;
    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    pkgManager->GetApplicationLabel(app->mInfo, (ICharSequence**)&mAppName);

    SetCancelable(FALSE);

    StringBuilder text;
    Int32 length;
    if (mAppName != NULL && (mAppName->GetLength(&length), length > 0)) {
        text.Append("Application ");
        text.Append(mAppName);
        text.Append(" (process ");
        text.Append(app->mProcessName);
        text.Append(")");
    } else {
        text.Append("Process ");
        text.Append(app->mProcessName);
    }

    text.Append(" is waiting for the debugger to attach.");

    AutoPtr<ICharSequence> textMsg;
    CString::New(text.ToString(), (ICharSequence**)&textMsg);
    SetMessage(textMsg);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(1, TO_IINTERFACE(app), (IMessage**)&msg);
    AutoPtr<ICharSequence> btMsg;
    CString::New(String("Force Close"), (ICharSequence**)&btMsg);
    SetButton(IDialogInterface::BUTTON_POSITIVE, btMsg, msg);
    AutoPtr<ICharSequence> title;
    CString::New(String("Waiting For Debugger"), (ICharSequence**)&title);
    SetTitle(title);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    AutoPtr<IWindow> window;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    String processName;
    app->mInfo->GetProcessName(&processName);
    AutoPtr<ICharSequence> attrsTitle;
    CString::New(String("Waiting For Debugger: ") + processName, (ICharSequence**)&attrsTitle);
    attrs->SetTitle(attrsTitle);
    window->SetAttributes(attrs);

    return NOERROR;
}

ECode AppWaitingForDebuggerDialog::OnStop()
{
    return NOERROR;
}

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
