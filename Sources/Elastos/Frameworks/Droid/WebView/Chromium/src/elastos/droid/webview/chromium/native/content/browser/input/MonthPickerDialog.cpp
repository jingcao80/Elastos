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
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/MonthPickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/MonthPicker.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"

using Elastos::Droid::Webkit::Webview::Chromium::Content::R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
    SetTitle(R::string::month_picker_dialog_title);
}

//@Override
AutoPtr<TwoFieldDatePicker> MonthPickerDialog::CreatePicker(
    /* [in] */ IContext* context,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    AutoPtr<TwoFieldDatePicker> picker = new MonthPicker(context, minValue, maxValue);
    return picker;
}

/**
 * Gets the {@link MonthPicker} contained in this dialog.
 *
 * @return The calendar view.
 */
AutoPtr<MonthPicker> MonthPickerDialog::GetMonthPicker()
{
    return (MonthPicker*) mPicker.Get();
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
