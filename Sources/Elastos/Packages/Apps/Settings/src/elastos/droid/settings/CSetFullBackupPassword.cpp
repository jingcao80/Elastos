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

#include "elastos/droid/settings/CSetFullBackupPassword.h"
#include "elastos/droid/os/ServiceManager.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String CSetFullBackupPassword::TAG("CSetFullBackupPassword");

//===============================================================================
//                  CSetFullBackupPassword::MyOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CSetFullBackupPassword::MyOnClickListener, Object, IViewOnClickListener)

CSetFullBackupPassword::MyOnClickListener::MyOnClickListener(
    /* [in] */ CSetFullBackupPassword* host)
    : mHost(host)
{}

ECode CSetFullBackupPassword::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    if (v == IView::Probe(mHost->mSet)) {
        AutoPtr<ICharSequence> cs;
        mHost->mCurrentPw->GetText((ICharSequence**)&cs);
        const String curPw = Object::ToString(cs);
        cs = NULL;
        mHost->mNewPw->GetText((ICharSequence**)&cs);
        const String newPw = Object::ToString(cs);
        cs = NULL;
        mHost->mConfirmNewPw->GetText((ICharSequence**)&cs);
        const String confirmPw = Object::ToString(cs);

        AutoPtr<IToastHelper> helper;
        CToastHelper::AcquireSingleton((IToastHelper**)&helper);
        AutoPtr<IToast> toast;
        if (!newPw.Equals(confirmPw)) {
            // Mismatch between new pw and its confirmation re-entry
            Logger::I(TAG, "password mismatch");
            helper->MakeText(mHost,
                    R::string::local_backup_password_toast_confirmation_mismatch,
                    IToast::LENGTH_LONG, (IToast**)&toast);
            toast->Show();
            return NOERROR;
        }

        // TODO: should we distinguish cases of has/hasn't set a pw before?
        toast = NULL;
        if (mHost->SetBackupPassword(curPw, newPw)) {
            // success
            Logger::I(TAG, "password set successfully");

            helper->MakeText(mHost,
                    R::string::local_backup_password_toast_success,
                    IToast::LENGTH_LONG, (IToast**)&toast);
            toast->Show();
            mHost->Finish();
        }
        else {
            // failure -- bad existing pw, usually
            Logger::I(TAG, "failure; password mismatch?");

            helper->MakeText(mHost,
                    R::string::local_backup_password_toast_validation_failure,
                    IToast::LENGTH_LONG, (IToast**)&toast);
            toast->Show();
        }
    }
    else if (v == IView::Probe(mHost->mCancel)) {
        mHost->Finish();
    }
    else {
        Logger::W(TAG, "Click on unknown view");
    }
    return NOERROR;
}

//===============================================================================
//                  CSetFullBackupPassword
//===============================================================================

CAR_OBJECT_IMPL(CSetFullBackupPassword)

CSetFullBackupPassword::CSetFullBackupPassword()
{}

CSetFullBackupPassword::~CSetFullBackupPassword()
{}

ECode CSetFullBackupPassword::constructor()
{
    mButtonListener = new MyOnClickListener(this);
    return Activity::constructor();
}

ECode CSetFullBackupPassword::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Activity::OnCreate(icicle);

    mBackupManager = IIBackupManager::Probe(ServiceManager::GetService(String("backup")));

    SetContentView(R::layout::set_backup_pw);

    AutoPtr<IView> tmp;
    FindViewById(R::id::current_backup_pw, (IView**)&tmp);
    mCurrentPw = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::new_backup_pw, (IView**)&tmp);
    mNewPw = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::confirm_new_backup_pw, (IView**)&tmp);
    mConfirmNewPw = ITextView::Probe(tmp);

    tmp = NULL;
    FindViewById(R::id::backup_pw_cancel_button, (IView**)&tmp);
    mCancel = IButton::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::backup_pw_set_button, (IView**)&tmp);
    mSet = IButton::Probe(tmp);

    IView::Probe(mCancel)->SetOnClickListener(mButtonListener);
    IView::Probe(mSet)->SetOnClickListener(mButtonListener);
    return NOERROR;
}

Boolean CSetFullBackupPassword::SetBackupPassword(
    /* [in] */ const String& currentPw,
    /* [in] */ const String& newPw)
{
    Boolean res;
    ECode ec = mBackupManager->SetBackupPassword(currentPw, newPw, &res);
    if (ec == (ECode) E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Unable to communicate with backup manager");
        return FALSE;
    }
    return res;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos