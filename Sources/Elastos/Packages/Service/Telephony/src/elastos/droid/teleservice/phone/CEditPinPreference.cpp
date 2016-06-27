
#include "elastos/droid/teleservice/phone/CEditPinPreference.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "R.h"

using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IEditText;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(CEditPinPreference, EditTextPreference, IPhoneEditPinPreference)

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
    SetDialogLayoutResource(Elastos::Droid::TeleService::R::layout::pref_dialog_editpin);

    AutoPtr<IView> dialog;
    EditTextPreference::OnCreateDialogView((IView**)&dialog);

    AutoPtr<IEditText> editText;
    GetEditText((IEditText**)&editText);
    ITextView::Probe(editText)->SetInputType(IInputType::TYPE_CLASS_NUMBER |
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
    view->FindViewById(Elastos::Droid::R::id::edit, (IView**)&_view);
    mShouldHideButtons = (_view == NULL);
    return NOERROR;
}

ECode CEditPinPreference::OnPrepareDialogBuilder(
    /* [in] */ IAlertDialogBuilder* builder)
{
    EditTextPreference::OnPrepareDialogBuilder(builder);

    // hide the buttons if we need to.
    if (mShouldHideButtons) {
        builder->SetPositiveButton((ICharSequence*)NULL, this);
        builder->SetNegativeButton((ICharSequence*)NULL, this);
    }
    return NOERROR;
}

ECode CEditPinPreference::OnDialogClosed(
    /* [in] */ Boolean positiveResult)
{
    EditTextPreference::OnDialogClosed(positiveResult);
    if (mPinListener != NULL) {
        mPinListener->OnPinEntered(this, positiveResult);
    }
    return NOERROR;
}

ECode CEditPinPreference::ShowPinDialog()
{
    return ShowDialog(NULL);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos