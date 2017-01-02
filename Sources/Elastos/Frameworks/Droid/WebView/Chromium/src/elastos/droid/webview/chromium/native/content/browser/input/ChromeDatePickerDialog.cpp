//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
