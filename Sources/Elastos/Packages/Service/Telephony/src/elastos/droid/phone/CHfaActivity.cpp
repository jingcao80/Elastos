
#include "elastos/droid/phone/CHfaActivity.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CHfaActivity::MyDialogInterfaceOnClickListener, Object,
        IDialogInterfaceOnClickListener)

ECode CHfaActivity::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* di,
    /* [in] */ Int32 which)
{
    return mHost->OnUserSkip();
}

CAR_INTERFACE_IMPL(CHfaActivity::MyDialogInterfaceOnClickListener2, Object,
        IDialogInterfaceOnClickListener)

ECode CHfaActivity::MyDialogInterfaceOnClickListener2::OnClick(
    /* [in] */ IDialogInterface* di,
    /* [in] */ Int32 which)
{
    di->Dismiss();
    return mHost->OnUserSkip();
}

CAR_INTERFACE_IMPL(CHfaActivity::MyDialogInterfaceOnClickListener3, Object,
        IDialogInterfaceOnClickListener)

ECode CHfaActivity::MyDialogInterfaceOnClickListener3::OnClick(
    /* [in] */ IDialogInterface* di,
    /* [in] */ Int32 which)
{
    di->Dismiss();
    return mHost->StartProvisioning();
}

CAR_INTERFACE_IMPL(CHfaActivity::MyHfaLogicHfaLogicCallback, Object,
        IHfaLogicHfaLogicCallback)

ECode CHfaActivity::MyHfaLogicHfaLogicCallback::OnSuccess()
{
    return mHost->OnHfaSuccess();
}

ECode CHfaActivity::MyHfaLogicHfaLogicCallback::OnError(
    /* [in] */ const String& error)
{
    return mHost->OnHfaError(error);
}

const String CHfaActivity::TAG("CHfaActivity");// = HfaActivity.class.getSimpleName();

CAR_INTERFACE_IMPL(CHfaActivity, Activity, IHfaActivity)

CAR_OBJECT_IMPL(CHfaActivity)

ECode CHfaActivity::constructor()
{
    return Activity::constructor();
}

ECode CHfaActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Logger::I(TAG, "onCreate");

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);

    AutoPtr<IParcelable> value;
    intent->GetParcelableExtra(IOtaUtils::EXTRA_OTASP_RESULT_CODE_PENDING_INTENT, (IParcelable**)&value);
    AutoPtr<IPendingIntent> otaResponseIntent = IPendingIntent::Probe(value);

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    AutoPtr<IHfaLogicHfaLogicCallback> cb = new MyHfaLogicHfaLogicCallback(this);

    CHfaLogic::New(ctx, cb, otaResponseIntent, (HfaLogic**)&mHfaLogic);

    return StartProvisioning();
}

ECode CHfaActivity::OnDestroy()
{
    Activity::OnDestroy();

    Logger::I(TAG, "onDestroy");

    Boolean res;
    if (mDialog != NULL && (mDialog->IsShowing(&res), res)) {
        mDialog->Dismiss();
        mDialog = NULL;
    }
    return NOERROR;
}

void CHfaActivity::StartProvisioning()
{
    BuildAndShowDialog();
    mHfaLogic->Start();
}

void CHfaActivity::BuildAndShowDialog()
{
    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, IAlertDialog::THEME_DEVICE_DEFAULT_LIGHT, (IAlertDialogBuilder**)&builder);
    builder->SetTitle(R.string.ota_hfa_activation_title);
    builder->SetMessage(R.string.ota_hfa_activation_dialog_message);

    AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener(this);
    builder->setPositiveButton(R.string.ota_skip_activation_dialog_skip_label, listener);

    builder->Create((IAlertDialog**)&mDialog);

    // Do not allow user to dismiss dialog unless they are clicking "skip"
    mDialog->SetCanceledOnTouchOutside(FALSE);
    mDialog->SetCancelable(FALSE);

    Logger::I(TAG, "showing dialog");
    mDialog->Show();
}

void CHfaActivity::OnHfaError(
    /* [in] */ const String& errorMsg)
{
    mDialog->Dismiss();

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, IAlertDialog::THEME_DEVICE_DEFAULT_LIGHT, (IAlertDialogBuilder**)&builder);
    builder->SetMessage(errorMsg);

    AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener2(this);
    builder->SetPositiveButton(R.string.ota_skip_activation_dialog_skip_label, listener);

    AutoPtr<IDialogInterfaceOnClickListener> listener2 = new MyDialogInterfaceOnClickListener3(this);
    builder->SetNegativeButton(R.string.ota_try_again, listener2);

    AutoPtr<IAlertDialog> errorDialog;
    builder->Create((IAlertDialog**)&errorDialog);
    errorDialog->Show();
}

void CHfaActivity::OnHfaSuccess()
{
    Finish();
}

void CHfaActivity::OnUserSkip()
{
    Finish();
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos