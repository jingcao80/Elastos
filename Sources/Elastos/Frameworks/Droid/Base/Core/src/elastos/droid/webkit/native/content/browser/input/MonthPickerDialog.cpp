
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/native/content/browser/input/MonthPickerDialog.h"
#include "elastos/droid/webkit/native/content/browser/input/MonthPicker.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * @param context The context the dialog is to run in.
 * @param callBack How the parent is notified that the date is set.
 * @param year The initial year of the dialog.
 * @param monthOfYear The initial month of the dialog.
 */
MonthPickerDialog::MonthPickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ OnValueSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Double minMonth,
    /* [in] */ Double maxMonth)
    : TwoFieldDatePickerDialog(context, callBack, year, monthOfYear, minMonth, maxMonth)
{
    assert(0);
    // TODO
    // SetTitle(R::string::month_picker_dialog_title);
}

//@Override
AutoPtr<TwoFieldDatePicker> MonthPickerDialog::CreatePicker(
    /* [in] */ IContext* context,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    AutoPtr<TwoFieldDatePicker> picker;// TODO = new MonthPicker(context, minValue, maxValue);
    return picker;
}

/**
 * Gets the {@link MonthPicker} contained in this dialog.
 *
 * @return The calendar view.
 */
AutoPtr<MonthPicker> MonthPickerDialog::GetMonthPicker()
{
    assert(0);
    // TODO
    // return (MonthPicker*) mPicker;
    return NULL;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
