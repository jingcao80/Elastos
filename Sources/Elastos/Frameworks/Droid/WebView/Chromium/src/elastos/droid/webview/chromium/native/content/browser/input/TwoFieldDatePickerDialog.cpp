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

#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/TwoFieldDatePickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include "elastos/droid/R.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::EIID_IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

const String TwoFieldDatePickerDialog::YEAR("year");
const String TwoFieldDatePickerDialog::POSITION_IN_YEAR("position_in_year");

CAR_INTERFACE_IMPL(TwoFieldDatePickerDialog, AlertDialog, IDialogInterfaceOnClickListener);

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
    AlertDialog::constructor(context, theme);

    mCallBack = callBack;

    AutoPtr<ICharSequence> setText;
    context->GetText(
             R::string::date_picker_dialog_set, (ICharSequence**)&setText);
    SetButton(BUTTON_POSITIVE, setText, this);
    AutoPtr<ICharSequence> cancelText;
    context->GetText(Elastos::Droid::R::string::cancel, (ICharSequence**)&cancelText);
    SetButton(BUTTON_NEGATIVE, cancelText,
             (IDialogInterfaceOnClickListener*) NULL);
    SetIcon(0);

    mPicker = CreatePicker(context, minValue, maxValue);
    SetView(IView::Probe(mPicker));
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
        IView::Probe(mPicker)->ClearFocus();
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
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
