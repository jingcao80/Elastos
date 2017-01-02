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

#include "elastos/droid/server/telecom/ErrorDialogActivity.h"
#include "R.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/Log.h"
#include <elastos/droid/R.h>

using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// ErrorDialogActivity::ClickSubDialogInterfaceOnClickListener
//=============================================================================
CAR_INTERFACE_IMPL(ErrorDialogActivity::ClickSubDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

ErrorDialogActivity::ClickSubDialogInterfaceOnClickListener::ClickSubDialogInterfaceOnClickListener(
    /* [in] */ ErrorDialogActivity* host)
    : mHost(host)
{}

ECode ErrorDialogActivity::ClickSubDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->Finish();
}

//=============================================================================
// ErrorDialogActivity::CancelSubDialogInterfaceOnCancelListener
//=============================================================================
CAR_INTERFACE_IMPL(ErrorDialogActivity::CancelSubDialogInterfaceOnCancelListener, Object, IDialogInterfaceOnCancelListener)

ErrorDialogActivity::CancelSubDialogInterfaceOnCancelListener::CancelSubDialogInterfaceOnCancelListener(
    /* [in] */ ErrorDialogActivity* host)
    : mHost(host)
{}

ECode ErrorDialogActivity::CancelSubDialogInterfaceOnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->Finish();
}

//=============================================================================
// ErrorDialogActivity::PositiveSubDialogInterfaceOnClickListener
//=============================================================================
CAR_INTERFACE_IMPL(ErrorDialogActivity::PositiveSubDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

ErrorDialogActivity::PositiveSubDialogInterfaceOnClickListener::PositiveSubDialogInterfaceOnClickListener(
    /* [in] */ ErrorDialogActivity* host)
    : mHost(host)
{}

ECode ErrorDialogActivity::PositiveSubDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->Finish();
}

//=============================================================================
// ErrorDialogActivity::NegativeSubDialogInterfaceOnClickListener
//=============================================================================
CAR_INTERFACE_IMPL(ErrorDialogActivity::NegativeSubDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener)

ErrorDialogActivity::NegativeSubDialogInterfaceOnClickListener::NegativeSubDialogInterfaceOnClickListener(
    /* [in] */ ErrorDialogActivity* host)
    : mHost(host)
{}

ECode ErrorDialogActivity::NegativeSubDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->AddVoiceMailNumberPanel(dialog);
}

//=============================================================================
// ErrorDialogActivity::CancelShowSubDialogInterfaceOnCancelListener
//=============================================================================
CAR_INTERFACE_IMPL(ErrorDialogActivity::CancelShowSubDialogInterfaceOnCancelListener, Object, IDialogInterfaceOnCancelListener)

ErrorDialogActivity::CancelShowSubDialogInterfaceOnCancelListener::CancelShowSubDialogInterfaceOnCancelListener(
    /* [in] */ ErrorDialogActivity* host)
    : mHost(host)
{}

ECode ErrorDialogActivity::CancelShowSubDialogInterfaceOnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->Finish();
}


//=============================================================================
// ErrorDialogActivity
//=============================================================================
CAR_INTERFACE_IMPL(ErrorDialogActivity, Activity, IErrorDialogActivity)

const String ErrorDialogActivity::TAG("ErrorDialogActivity");

ECode ErrorDialogActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Boolean showVoicemailDialog;
    intent->GetBooleanExtra(
            SHOW_MISSING_VOICEMAIL_NO_DIALOG_EXTRA, FALSE, &showVoicemailDialog);
    if (showVoicemailDialog) {
        ShowMissingVoicemailErrorDialog();
    } else {
        Int32 error;
        intent->GetInt32Extra(ERROR_MESSAGE_ID_EXTRA, -1, &error);
        if (error == -1) {
            Log::W(TAG, "ErrorDialogActivity called with no error type extra.");
            Finish();
        } else {
            ShowGenericErrorDialog(error);
        }
    }
    return NOERROR;
}

ECode ErrorDialogActivity::ShowGenericErrorDialog(
    /* [in] */ Int32 resid)
{
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<ICharSequence> msg;
    res->GetText(resid, (ICharSequence**)&msg);
    AutoPtr<IDialogInterfaceOnClickListener> clickListener;
    AutoPtr<IDialogInterfaceOnCancelListener> cancelListener;
    clickListener = new ClickSubDialogInterfaceOnClickListener(this);
    cancelListener = new CancelSubDialogInterfaceOnCancelListener(this);
    AutoPtr<IAlertDialogBuilder> alertDialogBuilder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&alertDialogBuilder);
    alertDialogBuilder->SetMessage(msg);
    alertDialogBuilder->SetPositiveButton(Elastos::Droid::R::string::ok, clickListener);
    alertDialogBuilder->SetOnCancelListener(cancelListener);
    AutoPtr<IAlertDialog> errorDialog;
    alertDialogBuilder->Create((IAlertDialog**)&errorDialog);
    IDialog::Probe(errorDialog)->Show();
    return NOERROR;
}

ECode ErrorDialogActivity::ShowMissingVoicemailErrorDialog()
{
    AutoPtr<IAlertDialogBuilder> alertDialogBuilder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&alertDialogBuilder);
    alertDialogBuilder->SetTitle(R::string::no_vm_number);
    alertDialogBuilder->SetMessage(R::string::no_vm_number_msg);
    alertDialogBuilder->SetPositiveButton(Elastos::Droid::R::string::ok, new PositiveSubDialogInterfaceOnClickListener(this));
    alertDialogBuilder->SetNegativeButton(R::string::add_vm_number_str,
            new NegativeSubDialogInterfaceOnClickListener(this));
    alertDialogBuilder->SetOnCancelListener(new CancelShowSubDialogInterfaceOnCancelListener(this));
    AutoPtr<IAlertDialog> iNoUse;
    alertDialogBuilder->Show((IAlertDialog**)&iNoUse);
    return NOERROR;
}

ECode ErrorDialogActivity::AddVoiceMailNumberPanel(
    /* [in] */ IDialogInterface* dialog)
{
    if (dialog != NULL) {
        dialog->Dismiss();
    }
    // Navigate to the Voicemail setting in the Call Settings activity.
    AutoPtr<IIntent> intent;
    CIntent::New(ACTION_ADD_VOICEMAIL, (IIntent**)&intent);
    StartActivity(intent);
    Finish();
    return NOERROR;
}

ECode ErrorDialogActivity::Finish()
{
    Activity::Finish();
    // Don't show the return to previous task animation to avoid showing a black screen.
    // Just dismiss the dialog and undim the previous activity immediately.
    OverridePendingTransition(0, 0);
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
