
#include "elastos/droid/teleservice/phone/CHfaActivity.h"
#include "elastos/droid/teleservice/phone/HfaLogic.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Core::StringBuilder;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CHfaActivity::MyDialogInterfaceOnClickListener, Object,
        IDialogInterfaceOnClickListener)

ECode CHfaActivity::MyDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* di,
    /* [in] */ Int32 which)
{
    mHost->OnUserSkip();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CHfaActivity::MyDialogInterfaceOnClickListener2, Object,
        IDialogInterfaceOnClickListener)

ECode CHfaActivity::MyDialogInterfaceOnClickListener2::OnClick(
    /* [in] */ IDialogInterface* di,
    /* [in] */ Int32 which)
{
    di->Dismiss();
    mHost->OnUserSkip();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CHfaActivity::MyDialogInterfaceOnClickListener3, Object,
        IDialogInterfaceOnClickListener)

ECode CHfaActivity::MyDialogInterfaceOnClickListener3::OnClick(
    /* [in] */ IDialogInterface* di,
    /* [in] */ Int32 which)
{
    di->Dismiss();
    mHost->StartProvisioning();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CHfaActivity::MyHfaLogicHfaLogicCallback, Object,
        IHfaLogicHfaLogicCallback)

ECode CHfaActivity::MyHfaLogicHfaLogicCallback::OnSuccess()
{
    mHost->OnHfaSuccess();
    return NOERROR;
}

ECode CHfaActivity::MyHfaLogicHfaLogicCallback::OnError(
    /* [in] */ const String& error)
{
    mHost->OnHfaError(error);
    return NOERROR;
}

const String CHfaActivity::TAG("CHfaActivity");// = HfaActivity.class.getSimpleName();

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

    mHfaLogic = new HfaLogic(ctx, cb, otaResponseIntent);

    StartProvisioning();
    return NOERROR;
}

ECode CHfaActivity::OnDestroy()
{
    Activity::OnDestroy();

    Logger::I(TAG, "onDestroy");

    Boolean res;
    if (mDialog != NULL && (IDialog::Probe(mDialog)->IsShowing(&res), res)) {
        IDialogInterface::Probe(mDialog)->Dismiss();
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
    builder->SetTitle(Elastos::Droid::TeleService::R::string::ota_hfa_activation_title);
    builder->SetMessage(Elastos::Droid::TeleService::R::string::ota_hfa_activation_dialog_message);

    AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener(this);
    builder->SetPositiveButton(Elastos::Droid::TeleService::R::string::ota_skip_activation_dialog_skip_label,
            listener);

    builder->Create((IAlertDialog**)&mDialog);

    // Do not allow user to dismiss dialog unless they are clicking "skip"
    IDialog::Probe(mDialog)->SetCanceledOnTouchOutside(FALSE);
    IDialog::Probe(mDialog)->SetCancelable(FALSE);

    Logger::I(TAG, "showing dialog");
    IDialog::Probe(mDialog)->Show();
}

void CHfaActivity::OnHfaError(
    /* [in] */ const String& errorMsg)
{
    IDialogInterface::Probe(mDialog)->Dismiss();

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(this, IAlertDialog::THEME_DEVICE_DEFAULT_LIGHT, (IAlertDialogBuilder**)&builder);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(errorMsg);
    builder->SetMessage(cchar);

    AutoPtr<IDialogInterfaceOnClickListener> listener = new MyDialogInterfaceOnClickListener2(this);
    builder->SetPositiveButton(Elastos::Droid::TeleService::R::string::ota_skip_activation_dialog_skip_label,
            listener);

    AutoPtr<IDialogInterfaceOnClickListener> listener2 = new MyDialogInterfaceOnClickListener3(this);
    builder->SetNegativeButton(Elastos::Droid::TeleService::R::string::ota_try_again,
            listener2);

    AutoPtr<IAlertDialog> errorDialog;
    builder->Create((IAlertDialog**)&errorDialog);
    IDialog::Probe(errorDialog)->Show();
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos