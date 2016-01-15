
#include "elastos/droid/webkit/native/content/browser/input/TwoFieldDatePickerDialog.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::EIID_IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

const String TwoFieldDatePickerDialog::YEAR("year");
const String TwoFieldDatePickerDialog::POSITION_IN_YEAR("position_in_year");

CAR_INTERFACE_IMPL_2(TwoFieldDatePickerDialog, TwoFieldDatePicker::OnMonthOrWeekChangedListener::Object, IAlertDialog, IDialogInterfaceOnClickListener);

TwoFieldDatePickerDialog::TwoFieldDatePickerDialog()
{
}

/**
 * @param context The context the dialog is to run in.
 * @param callBack How the parent is notified that the date is set.
 * @param year The initial year of the dialog.
 * @param weekOfYear The initial week of the dialog.
 */
TwoFieldDatePickerDialog::TwoFieldDatePickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ OnValueSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 positionInYear,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    Init(context, 0, callBack, year, positionInYear, minValue, maxValue);
}

/**
 * @param context The context the dialog is to run in.
 * @param theme the theme to apply to this dialog
 * @param callBack How the parent is notified that the date is set.
 * @param year The initial year of the dialog.
 * @param weekOfYear The initial week of the dialog.
 */
TwoFieldDatePickerDialog::TwoFieldDatePickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ OnValueSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 positionInYear,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    Init(context, theme, callBack, year, positionInYear, minValue, maxValue);
}


void TwoFieldDatePickerDialog::Init(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ OnValueSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 positionInYear,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    assert(0);
    // TODO
    // super(context, theme);

    mCallBack = callBack;

    AutoPtr<ICharSequence> setText;
    assert(0);
    // TODO
    // context->GetText(
    //         R::string::date_picker_dialog_set, (ICharSequence**)&setText);
    assert(0);
    // TODO
    // SetButton(BUTTON_POSITIVE, setText, this);
    AutoPtr<ICharSequence> cancelText;
    assert(0);
    // TODO
    // context->GetText(android::R::string::cancel, (ICharSequence**)&cancelText);
    assert(0);
    // TODO
    // SetButton(BUTTON_NEGATIVE, cancelText,
    //         (IDialogInterfaceOnClickListener*) NULL);
    SetIcon(0);

    mPicker = CreatePicker(context, minValue, maxValue);
    assert(0);
    // TODO
    // SetView(mPicker);
    mPicker->Init(year, positionInYear, this);
}

AutoPtr<TwoFieldDatePicker> TwoFieldDatePickerDialog::CreatePicker(
    /* [in] */ IContext* context,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    return NULL;
}

//@Override
ECode TwoFieldDatePickerDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    TryNotifyDateSet();
    return NOERROR;
}

/**
 * Notifies the listener, if such, that a date has been set.
 */
void TwoFieldDatePickerDialog::TryNotifyDateSet()
{
    if (mCallBack != NULL) {
        assert(0);
        // TODO
        // mPicker->ClearFocus();
        mCallBack->OnValueSet(mPicker->GetYear(), mPicker->GetPositionInYear());
    }
}

//@Override
ECode TwoFieldDatePickerDialog::OnMonthOrWeekChanged(
    /* [in] */ TwoFieldDatePicker* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 positionInYear)
{
    mPicker->Init(year, positionInYear, NULL);
    return NOERROR;
}

/**
 * Sets the current date.
 *
 * @param year The date week year.
 * @param weekOfYear The date week.
 */
void TwoFieldDatePickerDialog::UpdateDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 weekOfYear)
{
    mPicker->UpdateDate(year, weekOfYear);
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
