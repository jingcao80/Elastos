#include "elastos/droid/settings/wifi/CWifiDialog.h"
#include "../R.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const Int32 CWifiDialog::BUTTON_SUBMIT = IDialogInterface::BUTTON_POSITIVE;
const Int32 CWifiDialog::BUTTON_FORGET = IDialogInterface::BUTTON_NEUTRAL;

CAR_INTERFACE_IMPL(CWifiDialog, AlertDialog, IWifiConfigUiBase);

CAR_OBJECT_IMPL(CWifiDialog)

CWifiDialog::CWifiDialog()
    : mEdit(FALSE)
    , mHideSubmitButton(FALSE)
{}

CWifiDialog::~CWifiDialog()
{}

ECode CWifiDialog::constructor(
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

ECode CWifiDialog::constructor(
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

ECode CWifiDialog::GetController(
    /* [out] */ IWifiConfigController** controller)
{
    VALIDATE_NOT_NULL(controller)
    *controller = (IWifiConfigController*)mController.Get();
    REFCOUNT_ADD(*controller);
    return NOERROR;
}

ECode CWifiDialog::OnCreate(
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

ECode CWifiDialog::IsEdit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEdit;
    return NOERROR;
}

ECode CWifiDialog::GetSubmitButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)

    return GetButton(BUTTON_SUBMIT, button);
}

ECode CWifiDialog::GetForgetButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)
    return GetButton(BUTTON_FORGET, button);
}

ECode CWifiDialog::GetCancelButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button)
    return GetButton(BUTTON_NEGATIVE, button);
}

ECode CWifiDialog::SetSubmitButton(
    /* [in] */ ICharSequence* text)
{
    return SetButton(BUTTON_SUBMIT, text, mListener);
}

ECode CWifiDialog::SetForgetButton(
    /* [in] */ ICharSequence* text)
{
    return SetButton(BUTTON_FORGET, text, mListener);
}

ECode CWifiDialog::SetCancelButton(
    /* [in] */ ICharSequence* text)
{
    return SetButton(BUTTON_NEGATIVE, text, mListener);
}

// the under method use to fix compile error
ECode CWifiDialog::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    return AlertDialog::GetContext(context);
}

ECode CWifiDialog::SetTitle(
    /* [in] */ Int32 id)
{
    return AlertDialog::SetTitle(id);
}

ECode CWifiDialog::SetTitle(
    /* [in] */ ICharSequence* title)
{
    return AlertDialog::SetTitle(title);
}

ECode CWifiDialog::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    return AlertDialog::GetLayoutInflater(inflater);
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
