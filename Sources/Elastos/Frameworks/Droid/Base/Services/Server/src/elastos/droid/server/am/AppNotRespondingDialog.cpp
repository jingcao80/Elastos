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

#include "elastos/droid/server/am/AppNotRespondingDialog.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/R.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::ISystem;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

String const AppNotRespondingDialog::TAG("AppNotRespondingDialog");
Int32 const AppNotRespondingDialog::FORCE_CLOSE = 1;
Int32 const AppNotRespondingDialog::WAIT = 2;
Int32 const AppNotRespondingDialog::WAIT_AND_REPORT = 3;

ECode AppNotRespondingDialog::constructor(
    /* [in] */ CActivityManagerService* service,
    /* [in] */ IContext* context,
    /* [in] */ ProcessRecord* app,
    /* [in] */ ActivityRecord* activity,
    /* [in] */ Boolean aboveSystem)
{
    FAIL_RETURN(BaseErrorDialog::Init(context))
    mService = service;
    mProc = app;
    mHandler = new MyHandler(this);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    SetCancelable(FALSE);

    Int32 resid;
    AutoPtr<ICharSequence> name1;
    AutoPtr<IPackageManager> pkgManager;
    context->GetPackageManager((IPackageManager**)&pkgManager);
    if (activity != NULL) {
        IPackageItemInfo::Probe(activity->mInfo)->LoadLabel(pkgManager, (ICharSequence**)&name1);
    }

    AutoPtr<ICharSequence> name2;
    pkgManager->GetApplicationLabel(app->mInfo, (ICharSequence**)&name2);
    Int32 size;
    if (((app->mPkgList->GetSize(&size), size) == 1) &&
            name2 != NULL) {
        if (name1 != NULL) {
            resid = R::string::anr_activity_application;
        }
        else {
            name1 = name2;
            name2 = NULL;
            CString::New(app->mProcessName, (ICharSequence**)&name2);
            resid = R::string::anr_application_process;
        }
    }
    else {
        if (name1 != NULL) {
            name2 = NULL;
            CString::New(app->mProcessName, (ICharSequence**)&name2);
            resid = R::string::anr_activity_process;
        }
        else {
            CString::New(app->mProcessName, (ICharSequence**)&name1);
            resid = R::string::anr_process;
        }
    }

    AutoPtr<ArrayOf<IInterface*> > arrayStr1 = ArrayOf<IInterface*>::Alloc(2);
    arrayStr1->Set(0, name1);
    arrayStr1->Set(1, name2);
    AutoPtr<ArrayOf<IInterface*> > arrayStr2 = ArrayOf<IInterface*>::Alloc(1);
    arrayStr2->Set(0, name1);
    String str1, str2;
    res->GetString(resid, arrayStr1, &str1);
    res->GetString(resid, arrayStr2, &str2);
    AutoPtr<ICharSequence> cs1, cs2;
    CString::New(str1, (ICharSequence**)&cs1);
    CString::New(str2, (ICharSequence**)&cs2);
    SetMessage(name2 != NULL ? cs1 : cs2);

    AutoPtr<ICharSequence> text1;
    res->GetText(R::string::force_close, (ICharSequence**)&text1);
    AutoPtr<IMessage> msg1;
    mHandler->ObtainMessage(FORCE_CLOSE, (IMessage**)&msg1);
    SetButton(IDialogInterface::BUTTON_POSITIVE, text1, msg1);

    AutoPtr<ICharSequence> text2;
    res->GetText(R::string::wait, (ICharSequence**)&text2);
    AutoPtr<IMessage> msg2;
    mHandler->ObtainMessage(WAIT, (IMessage**)&msg2);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, text2, msg2);

    if (app->mErrorReportReceiver != NULL) {
        AutoPtr<ICharSequence> text3;
        res->GetText(R::string::report, (ICharSequence**)&text3);
        AutoPtr<IMessage> msg3;
        mHandler->ObtainMessage(WAIT_AND_REPORT, (IMessage**)&msg3);
        SetButton(IDialogInterface::BUTTON_NEUTRAL, text3, msg3);
    }

    AutoPtr<ICharSequence> title;
    res->GetText(R::string::anr_title, (ICharSequence**)&title);
    SetTitle(title);

    AutoPtr<IWindow> window;
    Dialog::GetWindow((IWindow**)&window);
    if (aboveSystem) {
        window->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR);
    }
    String processName;
    app->mInfo->GetProcessName(&processName);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    window->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    AutoPtr<ICharSequence> titlecs;
    CString::New(String("Application Not Responding: ") + processName, (ICharSequence**)&titlecs);
    attrs->SetTitle(titlecs);
    attrs->SetPrivateFlags( IWindowManagerLayoutParams::PRIVATE_FLAG_SYSTEM_ERROR
            | IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS);
    window->SetAttributes(attrs);
    return NOERROR;
}

ECode AppNotRespondingDialog::OnStop()
{
    return NOERROR;
}

AppNotRespondingDialog::MyHandler::MyHandler(
    /* [in] */ AppNotRespondingDialog* host)
    : mHost(host)
{
    Handler::constructor();
}

AppNotRespondingDialog::MyHandler::~MyHandler()
{}

ECode AppNotRespondingDialog::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IIntent> appErrorIntent;
    Int32 type;
    msg->GetWhat(&type);
    switch (type) {
        case FORCE_CLOSE:
            // Kill the application.
            // todo:KillAppAtUsersRequest must be public and the function is public in android
            mHost->mService->KillAppAtUsersRequest(mHost->mProc, IDialog::Probe(mHost));
            break;
        case WAIT_AND_REPORT:
        case WAIT:
            // Continue waiting for the application.
            AutoLock lock(mHost->mService);
            AutoPtr<ProcessRecord> app = mHost->mProc;

            if (type == WAIT_AND_REPORT) {
                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 now;
                system->GetCurrentTimeMillis(&now);
                appErrorIntent = mHost->mService->CreateAppErrorIntentLocked(app, now, NULL);
            }

            app->mNotResponding = FALSE;
            app->mNotRespondingReport = NULL;
            if (app->mAnrDialog == mHost) {
                app->mAnrDialog = NULL;
            }
            break;
    }

    if (appErrorIntent != NULL) {
        //try {
        AutoPtr<IContext> context;
        mHost->GetContext((IContext**)&context);
        context->StartActivity(appErrorIntent);
        //} catch (ActivityNotFoundException e) {
        //    Slog.w(TAG, "bug report receiver dissappeared", e);
        //}
    }

    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
