
#include "elastos/droid/webkit/native/content/browser/input/WeekPickerDialog.h"
#include "elastos/droid/webkit/native/content/browser/input/WeekPicker.h"

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
 * @param weekOfYear The initial week of the dialog.
 */
WeekPickerDialog::WeekPickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ OnValueSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 weekOfYear,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    assert(0);
    // TODO
    // this(context, 0, callBack, year, weekOfYear, minValue, maxValue);
}

/**
 * @param context The context the dialog is to run in.
 * @param theme the theme to apply to this dialog
 * @param callBack How the parent is notified that the date is set.
 * @param year The initial year of the dialog.
 * @param weekOfYear The initial week of the dialog.
 */
WeekPickerDialog::WeekPickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ OnValueSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 weekOfYear,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    assert(0);
    // TODO
    // super(context, theme, callBack, year, weekOfYear, minValue, maxValue);
    // SetTitle(R.string.week_picker_dialog_title);
}

//@Override
AutoPtr<TwoFieldDatePicker> WeekPickerDialog::CreatePicker(
    /* [in] */ IContext* context,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    AutoPtr<TwoFieldDatePicker> wp;
    assert(0);
    // TODO
    // wp = new WeekPicker(context, minValue, maxValue);
    return wp;
}

/**
 * Gets the {@link WeekPicker} contained in this dialog.
 *
 * @return The calendar view.
 */
AutoPtr<WeekPicker> WeekPickerDialog::GetWeekPicker()
{
    assert(0);
    // TODO
    // return (WeekPicker*) mPicker;
    return NULL;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
