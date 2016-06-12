#include "elastos/droid/settings/wifi/WifiDialog.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const Int32 WifiDialog::BUTTON_SUBMIT = IDialogInterface::BUTTON_POSITIVE;
const Int32 WifiDialog::BUTTON_FORGET = IDialogInterface::BUTTON_NEUTRAL;

CAR_INTERFACE_IMPL(WifiDialog, AlertDialog, IWifiConfigUiBase);

WifiDialog::WifiDialog()
    : mEdit(FALSE)
    , mHideSubmitButton(FALSE)
{}

WifiDialog::~WifiDialog()
{}

ECode WifiDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDialogInterfaceOnClickListener* listener,
    /* [in] */ IAccessPoint* accessPoint,
    /* [in] */ Boolean edit,
    /* [in] */ Boolean hideSubmitButton)
{
    constructor(context, listener, accessPoint, edit);
    mHideSubmitButton = hideSubmitButton;
    return NOERROR;
}

ECode WifiDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDialogInterfaceOnClickListener* listener,
    /* [in] */ IAccessPoint* accessPoint,
    /* [in] */ Boolean edit)
{
    AlertDialog::constructor(context);
    mEdit = edit;
    mListener = listener;
    mAccessPoint = accessPoint;
    mHideSubmitButton = FALSE;
    return NOERROR;
}

ECode WifiDialog::GetController(
    /* [out] */ IWifiConfigController** controller)
{
    VALIDATE_NOT_NULL(controller)
    *controller = (IWifiConfigController*)mController.Get();
    REFCOUNT_ADD(*controller);
    return NOERROR;
}

ECode WifiDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AutoPtr<ILayoutInflater> inflate;
    AlertDialog::GetLayoutInflater((ILayoutInflater**)&inflate);
    inflate->Inflate(R::layout::wifi_dialog, NULL, (IView**)&mView);
    SetView(mView);
    SetInverseBackgroundForced(TRUE);
    mController = new WifiConfigController((IWifiConfigUiBase*)this, mView, mAccessPoint, mEdit);
    AlertDialog::OnCreate(savedInstanceState);

    if (mHideSubmitButton) {
        mController->HideSubmitButton();
        return NOERROR;
    }
    else {
        /* During creation, the submit button can be unavailable to determine
         * visibility. Right after creation, update button visibility */
        mController->EnableSubmitIfAppropriate();
        return NOERROR;
    }
}

ECode WifiDialog::IsEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEdit;
    return NOERROR;
}

ECode WifiDialog::GetSubmitButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)

    return GetButton(BUTTON_SUBMIT, button);
}

ECode WifiDialog::GetForgetButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)
    return GetButton(BUTTON_FORGET, button);
}

ECode WifiDialog::GetCancelButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)
    return GetButton(BUTTON_NEGATIVE, button);
}

ECode WifiDialog::SetSubmitButton(
    /* [in] */ ICharSequence* text)
{
    return SetButton(BUTTON_SUBMIT, text, mListener);
}

ECode WifiDialog::SetForgetButton(
    /* [in] */ ICharSequence* text)
{
    return SetButton(BUTTON_FORGET, text, mListener);
}

ECode WifiDialog::SetCancelButton(
    /* [in] */ ICharSequence* text)
{
    return SetButton(BUTTON_NEGATIVE, text, mListener);
}

// the under method use to fix compile error
ECode WifiDialog::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    assert(0 && "TODO");
    return NOERROR;
}

ECode WifiDialog::SetTitle(
    /* [in] */ Int32 id)
{
    Logger::D("WifiDialog", "Ignoring setTitle");
    return NOERROR;
}

ECode WifiDialog::SetTitle(
    /* [in] */ ICharSequence* title)
{
    Logger::D("WifiDialog", "Ignoring setTitle");
    return NOERROR;
}

ECode WifiDialog::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    assert(0 && "TODO");
    return NOERROR;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
