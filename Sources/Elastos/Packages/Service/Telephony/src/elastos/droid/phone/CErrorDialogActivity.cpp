
#include "elastos/droid/phone/CErrorDialogActivity.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CErrorDialogActivity::MyDialogInterfaceOnClickListener,
        Object, IDialogInterfaceOnClickListener)

ECode CErrorDialogActivity::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->Finish();
}

CAR_INTERFACE_IMPL(CErrorDialogActivity::MyDialogInterfaceOnClickListener2,
        Object, IDialogInterfaceOnClickListener)

ECode CErrorDialogActivity::MyDialogInterfaceOnClickListener2::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->DontAddVoiceMailNumber();
}

CAR_INTERFACE_IMPL(CErrorDialogActivity::MyDialogInterfaceOnClickListener3,
        Object, IDialogInterfaceOnClickListener)

ECode CErrorDialogActivity::MyDialogInterfaceOnClickListener3::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->AddVoiceMailNumberPanel(dialog);
}

CAR_INTERFACE_IMPL(CErrorDialogActivity::MyDialogInterfaceOnCancelListener,
        Object, IDialogInterfaceOnCancelListener)

ECode CErrorDialogActivity::MyDialogInterfaceOnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->Finish();
}

CAR_INTERFACE_IMPL(CErrorDialogActivity::MyDialogInterfaceOnCancelListener2,
        Object, IDialogInterfaceOnCancelListener)

ECode CErrorDialogActivity::MyDialogInterfaceOnCancelListener2::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->DontAddVoiceMailNumber();
}

const String CErrorDialogActivity::TAG("CErrorDialogActivity");// = ErrorDialogActivity.class.getSimpleName();

CAR_INTERFACE_IMPL(CErrorDialogActivity, Activity, IErrorDialogActivity)

CAR_OBJECT_IMPL(CErrorDialogActivity)

ECode CErrorDialogActivity::constructor()
{
    return Activity::constructor();
}

ECOde CErrorDialogActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Boolean showVoicemailDialog;
    intent->GetBooleanExtra(SHOW_MISSING_VOICEMAIL_NO_DIALOG_EXTRA, FALSE, &showVoicemailDialog);

    if (showVoicemailDialog) {
        ShowMissingVoicemailErrorDialog();
    }
    else {
        AutoPtr<IIntent> intent;
        GetIntent((IIntent**)&intent);
        Int32 error;
        intent->GetIntExtra(ERROR_MESSAGE_ID_EXTRA, -1, &error);
        if (error == -1) {
            Logger::E(TAG, "ErrorDialogActivity called with no error type extra.");
            Finish();
        }
        ShowGenericErrorDialog(error);
    }
}

void CErrorDialogActivity::ShowGenericErrorDialog(
    /* [in] */ Int32 resid)
{
    AutoPtr<IResources> resources;
    getResources((IResources**)&resources);
    AutoPtr<ICharSequence> msg;
    resources->GetText(resid, (ICharSequence**)&msg);

    AutoPtr<IDialogInterfaceOnClickListener> clickListener = new MyDialogInterfaceOnClickListener(this);

    AutoPtr<IDialogInterfaceOnCancelListener> cancelListener = new MyDialogInterfaceOnCancelListener(this);

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);
    builder->SetMessage(msg);
    builder->SetPositiveButton(R.string.ok, clickListener);
    builder->SetOnCancelListener(cancelListener);

    AutoPtr<IAlertDialog> errorDialog;
    builder->Create((IAlertDialog**)&errorDialog);
    errorDialog->Show();
}

void CErrorDialogActivity::ShowMissingVoicemailErrorDialog()
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, (IAlertDialogBuilder**)&builder);

    builder->SetTitle(R.string.no_vm_number);
    builder->SetMessage(R.string.no_vm_number_msg);

    AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener2(this);
    builder->SetPositiveButton(R.string.ok, listener);

    AutoPtr<IDialogInterfaceOnClickListener> listener2 = new MyDialogInterfaceOnClickListener3(this);
    builder->SetNegativeButton(R.string.add_vm_number_str, listener2);

    AutoPtr<MyDialogInterfaceOnCancelListener> listener3 = new MyDialogInterfaceOnCancelListener2(this);
    builder->SetOnCancelListener(listener3);

    builder->Show();
}

void CErrorDialogActivity::AddVoiceMailNumberPanel(
    /* [in] */ IDialogInterface* dialog)
{
    if (dialog != NULL) {
        dialog->Dismiss();
    }

    // navigate to the Voicemail setting in the Call Settings activity.
    AutoPtr<IIntent> intent;
    CIntent::New(ICallFeaturesSetting::ACTION_ADD_VOICEMAIL, (IIntent**)&intent);
    intent->SetClass(this, CallFeaturesSetting.class);
    StartActivity(intent);
    Finish();
}

void CErrorDialogActivity::DontAddVoiceMailNumber()
{
    Finish();
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos