
#include "Elastos.Droid.App.h"
#include "elastos/droid/webkit/native/content/browser/input/ChromeDatePickerDialog.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

ChromeDatePickerDialog::ChromeDatePickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ IDatePickerDialogOnDateSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth)
    // TODO : DatePickerDialog(context, 0, callBack, year, monthOfYear, dayOfMonth)
    : mCallBack(callBack)
{
}

/**
 * The superclass DatePickerDialog has null for OnDateSetListener so we need to call the
 * listener manually.
 */
//@Override
ECode ChromeDatePickerDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    assert(0);
    // TODO
    // if (which == BUTTON_POSITIVE && mCallBack != NULL) {
    //     AutoPtr<IDatePicker> datePicker = GetDatePicker();
    //     datePicker->ClearFocus();
    //     Int32 year, month, dayOfMonth;
    //     datePicker->GetYear(&year);
    //     datePicker->GetMonth(&month);
    //     datePicker->GetDayOfMonth(&dayOfMonth);
    //     mCallBack->OnDateSet(datePicker, year,
    //             month, dayOfMonth);
    // }

    return E_NOT_IMPLEMENTED;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
