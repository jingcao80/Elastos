
#include "elastos/droid/phone/CEditPinPreference.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CEditPinPreference, EditTextPreference, IEditPinPreference)

CAR_OBJECT_IMPL(CEditPinPreference)

CEditPinPreference::CEditPinPreference()
    : mShouldHideButtons(FALSE)
{
}

ECode CEditPinPreference::SetOnPinEnteredListener(
    /* [in] */ IEditPinPreferenceOnPinEnteredListener* listener)
{
    mPinListener = listener;
    return NOERROR;
}

ECode CEditPinPreference::OnCreateDialogView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    // set the dialog layout
    SetDialogLayoutResource(R.layout.pref_dialog_editpin);

    AutoPtr<IView> dialog;
    EditTextPreference::OnCreateDialogView((IView**)&dialog);

    AutoPtr<IEditText> editText;
    GetEditText((IEditText**)&editText);
    editText->SetInputType(IInputType::TYPE_CLASS_NUMBER |
        IInputType::TYPE_NUMBER_VARIATION_PASSWORD);

    *view = dialog;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CEditPinPreference::OnBindDialogView(
    /* [in] */ IView* view)
{
    EditTextPreference::OnBindDialogView(view);

    // If the layout does not contain an edittext, hide the buttons.
    AutoPtr<IView> _view;
    view->FindViewById(android.R.id.edit, (IView**)&_view);
    mShouldHideButtons = (_view == NULL);
    return NOERROR;
}

ECode CEditPinPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    EditTextPreference::OnPrepareDialogBuilder(builder);

    // hide the buttons if we need to.
    if (mShouldHideButtons) {
        builder->SetPositiveButton(NULL, this);
        builder->SetNegativeButton(NULL, this);
    }
}

ECode CEditPinPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    EditTextPreference::OnDialogClosed(positiveResult);
    if (mPinListener != NULL) {
        mPinListener->OnPinEntered(this, positiveResult);
    }
}

ECode CEditPinPreference::ShowPinDialog()
{
    return ShowDialog(NULL);
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos