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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/WeekPickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/WeekPicker.h"
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
    TwoFieldDatePickerDialog(context, 0, callBack, year, weekOfYear, minValue, maxValue);
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
    TwoFieldDatePickerDialog(context, theme, callBack, year, weekOfYear, minValue, maxValue);
    SetTitle(R::string::week_picker_dialog_title);
}

//@Override
AutoPtr<TwoFieldDatePicker> WeekPickerDialog::CreatePicker(
    /* [in] */ IContext* context,
    /* [in] */ Double minValue,
    /* [in] */ Double maxValue)
{
    AutoPtr<TwoFieldDatePicker> wp;
    wp = new WeekPicker(context, minValue, maxValue);
    return wp;
}

/**
 * Gets the {@link WeekPicker} contained in this dialog.
 *
 * @return The calendar view.
 */
AutoPtr<WeekPicker> WeekPickerDialog::GetWeekPicker()
{
    return (WeekPicker*) mPicker.Get();
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
