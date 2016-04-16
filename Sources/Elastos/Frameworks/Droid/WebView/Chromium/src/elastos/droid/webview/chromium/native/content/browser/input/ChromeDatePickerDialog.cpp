
#include "Elastos.Droid.App.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/ChromeDatePickerDialog.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

ChromeDatePickerDialog::ChromeDatePickerDialog(
    /* [in] */ IContext* context,
    /* [in] */ IDatePickerDialogOnDateSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth)
    : mCallBack(callBack)
{
    DatePickerDialog::constructor(context, 0, callBack, year, monthOfYear, dayOfMonth);
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
    if (which == BUTTON_POSITIVE && mCallBack != NULL) {
        AutoPtr<IDatePicker> datePicker;
        GetDatePicker((IDatePicker**)&datePicker);
        IView::Probe(datePicker)->ClearFocus();
        Int32 year, month, dayOfMonth;
        datePicker->GetYear(&year);
        datePicker->GetMonth(&month);
        datePicker->GetDayOfMonth(&dayOfMonth);
        mCallBack->OnDateSet(datePicker, year,
                month, dayOfMonth);
    }

    return NOERROR;
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
