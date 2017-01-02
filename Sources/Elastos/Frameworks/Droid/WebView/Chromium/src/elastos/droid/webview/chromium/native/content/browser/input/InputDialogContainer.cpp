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

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/InputDialogContainer.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/ChromeDatePickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateDialogNormalizer.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimeSuggestionListAdapter.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimeSuggestion.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/MonthPicker.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/MonthPickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/WeekPicker.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/WeekPickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ApiCompatibilityUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include "elastos/droid/R.h"
#include "elastos/core/Math.h"
#include "elastos/utility/Arrays.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::EIID_IDialog;
using Elastos::Droid::App::EIID_IAlertDialog;
using Elastos::Droid::App::EIID_IDatePickerDialogOnDateSetListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterface;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ApiCompatibilityUtils;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::CListView;
using Elastos::Utility::Arrays;
using Elastos::Utility::CGregorianCalendar;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::EIID_ICalendar;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

//==================================================================
//    InputDialogContainer::InnerAdapterViewOnItemClickListener
//==================================================================

CAR_INTERFACE_IMPL(InputDialogContainer::InnerAdapterViewOnItemClickListener, Object, IAdapterViewOnItemClickListener);

InputDialogContainer::InnerAdapterViewOnItemClickListener::InnerAdapterViewOnItemClickListener(
    /* [in] */ InputDialogContainer* owner,
    /* [in] */ Int32 dialogType,
    /* [in] */ Double dialogValue,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step,
    /* [in] */ DateTimeSuggestionListAdapter* adapter)
    : mOwner(owner)
    , mDialogType(dialogType)
    , mDialogValue(dialogValue)
    , mMin(min)
    , mMax(max)
    , mStep(step)
    , mAdapter(adapter)
{
}

ECode InputDialogContainer::InnerAdapterViewOnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Int32 count;
    mAdapter->GetCount(&count);
    if (position == count - 1) {
        mOwner->DismissDialog();
        mOwner->ShowPickerDialog(mDialogType, mDialogValue, mMin, mMax, mStep);
    }
    else {
        AutoPtr<IInterface> obj;
        mAdapter->GetItem(position, (IInterface**)&obj);
        DateTimeSuggestion* dts = (DateTimeSuggestion*)(IObject::Probe(obj));
        Double suggestionValue = dts->Value();
        mOwner->mInputActionDelegate->ReplaceDateTime(suggestionValue);
        mOwner->DismissDialog();
        mOwner->mDialogAlreadyDismissed = TRUE;
    }

    return NOERROR;
}

//=========================================================================
//        InputDialogContainer::InnerDialogInterfaceOnClickListener
//=========================================================================

CAR_INTERFACE_IMPL(InputDialogContainer::InnerDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener);

InputDialogContainer::InnerDialogInterfaceOnClickListener::InnerDialogInterfaceOnClickListener(
    /* [in] */ InputDialogContainer* owner)
    : mOwner(owner)
{
}

ECode InputDialogContainer::InnerDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mOwner->DismissDialog();
    return NOERROR;
}

//=========================================================================
//        InputDialogContainer::InnerDialogInterfaceOnDismissListener
//=========================================================================
CAR_INTERFACE_IMPL(InputDialogContainer::InnerDialogInterfaceOnDismissListener, Object, IDialogInterfaceOnDismissListener);

InputDialogContainer::InnerDialogInterfaceOnDismissListener::InnerDialogInterfaceOnDismissListener(
    /* [in] */ InputDialogContainer* owner)
    : mOwner(owner)
{
}

ECode InputDialogContainer::InnerDialogInterfaceOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    AutoPtr<IDialogInterface> dialogInterface = IDialogInterface::Probe(mOwner);
    if (dialogInterface.Get() == dialog && !mOwner->mDialogAlreadyDismissed) {
        mOwner->mDialogAlreadyDismissed = TRUE;
        mOwner->mInputActionDelegate->CancelDateTimeDialog();
    }

    return NOERROR;
}

//=============================================================================
//       InputDialogContainer::SetButtonDialogInterfaceOnClickListener
//=============================================================================
CAR_INTERFACE_IMPL(InputDialogContainer::SetButtonDialogInterfaceOnClickListener, Object, IDialogInterfaceOnClickListener);

InputDialogContainer::SetButtonDialogInterfaceOnClickListener::SetButtonDialogInterfaceOnClickListener(
    /* [in] */ InputDialogContainer* owner)
    : mOwner(owner)
{
}

ECode InputDialogContainer::SetButtonDialogInterfaceOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mOwner->mDialogAlreadyDismissed = TRUE;
    mOwner->mInputActionDelegate->ReplaceDateTime(Elastos::Core::Math::DOUBLE_NAN);
    return NOERROR;
}

//================================================================================================
//         InputDialogContainer::SetOnDismissListenerDialogInterfaceOnDismissListener
//================================================================================================
CAR_INTERFACE_IMPL(InputDialogContainer::SetOnDismissListenerDialogInterfaceOnDismissListener, Object, IDialogInterfaceOnDismissListener);

InputDialogContainer::SetOnDismissListenerDialogInterfaceOnDismissListener::SetOnDismissListenerDialogInterfaceOnDismissListener(
    /* [in] */ InputDialogContainer* owner)
    : mOwner(owner)
{
}

ECode InputDialogContainer::SetOnDismissListenerDialogInterfaceOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    if (!mOwner->mDialogAlreadyDismissed) {
        mOwner->mDialogAlreadyDismissed = TRUE;
        mOwner->mInputActionDelegate->CancelDateTimeDialog();
    }

    return NOERROR;
}

//==================================================================
//              InputDialogContainer::DateListener
//==================================================================
CAR_INTERFACE_IMPL(InputDialogContainer::DateListener, Object, IDatePickerDialogOnDateSetListener);

InputDialogContainer::DateListener::DateListener(
    /* [in] */ InputDialogContainer* owner,
    /* [in] */ Int32 dialogType)
    : mOwner(owner)
    , mDialogType(dialogType)
{
}

//@Override
ECode InputDialogContainer::DateListener::OnDateSet(
    /* [in] */ IDatePicker* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 monthDay)
{
    mOwner->SetFieldDateTimeValue(mDialogType, year, month, monthDay, 0, 0, 0, 0, 0);
    return NOERROR;
}

//==================================================================
//              InputDialogContainer::FullTimeListener
//==================================================================

InputDialogContainer::FullTimeListener::FullTimeListener(
    /* [in] */ InputDialogContainer* owner,
    /* [in] */ Int32 dialogType)
    : mOwner(owner)
    , mDialogType(dialogType)
{
}

//@Override
ECode InputDialogContainer::FullTimeListener::OnTimeSet(
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [in] */ Int32 milli)
{
    mOwner->SetFieldDateTimeValue(mDialogType, 0, 0, 0, hourOfDay, minute, second, milli, 0);
    return NOERROR;
}

//==================================================================
//             InputDialogContainer::DateTimeListener
//==================================================================

InputDialogContainer::DateTimeListener::DateTimeListener(
    /* [in] */ InputDialogContainer* owner,
    /* [in] */ Int32 dialogType)
    : mOwner(owner)
    , mLocal(dialogType == sTextInputTypeDateTimeLocal)
    , mDialogType(dialogType)
{
}

//@Override
ECode InputDialogContainer::DateTimeListener::OnDateTimeSet(
    /* [in] */ IDatePicker* dateView,
    /* [in] */ ITimePicker* timeView,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 monthDay,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    mOwner->SetFieldDateTimeValue(mDialogType, year, month, monthDay, hourOfDay, minute, 0, 0, 0);
    return NOERROR;
}

//==================================================================
//           InputDialogContainer::MonthOrWeekListener
//==================================================================

InputDialogContainer::MonthOrWeekListener::MonthOrWeekListener(
    /* [in] */ InputDialogContainer* owner,
    /* [in] */ Int32 dialogType)
    : mOwner(owner)
    , mDialogType(dialogType)
{
}

//@Override
void InputDialogContainer::MonthOrWeekListener::OnValueSet(
    /* [in] */ Int32 year,
    /* [in] */ Int32 positionInYear)
{
    if (mDialogType == sTextInputTypeMonth) {
        mOwner->SetFieldDateTimeValue(mDialogType, year, positionInYear, 0, 0, 0, 0, 0, 0);
    }
    else {
        mOwner->SetFieldDateTimeValue(mDialogType, year, 0, 0, 0, 0, 0, 0, positionInYear);
    }
}

//==================================================================
//                       InputDialogContainer
//==================================================================

Int32 InputDialogContainer::sTextInputTypeDate = 0;
Int32 InputDialogContainer::sTextInputTypeDateTime = 0;
Int32 InputDialogContainer::sTextInputTypeDateTimeLocal = 0;
Int32 InputDialogContainer::sTextInputTypeMonth = 0;
Int32 InputDialogContainer::sTextInputTypeTime = 0;
Int32 InputDialogContainer::sTextInputTypeWeek = 0;

InputDialogContainer::InputDialogContainer(
    /* [in] */ IContext* context,
    /* [in] */ InputActionDelegate* inputActionDelegate)
    : mContext(context)
    , mDialogAlreadyDismissed(FALSE)
    , mInputActionDelegate(inputActionDelegate)
{
}

void InputDialogContainer::InitializeInputTypes(
    /* [in] */ Int32 textInputTypeDate,
    /* [in] */ Int32 textInputTypeDateTime,
    /* [in] */ Int32 textInputTypeDateTimeLocal,
    /* [in] */ Int32 textInputTypeMonth,
    /* [in] */ Int32 textInputTypeTime,
    /* [in] */ Int32 textInputTypeWeek)
{
    sTextInputTypeDate = textInputTypeDate;
    sTextInputTypeDateTime = textInputTypeDateTime;
    sTextInputTypeDateTimeLocal = textInputTypeDateTimeLocal;
    sTextInputTypeMonth = textInputTypeMonth;
    sTextInputTypeTime = textInputTypeTime;
    sTextInputTypeWeek = textInputTypeWeek;
}

Boolean InputDialogContainer::IsDialogInputType(
    /* [in] */ Int32 type)
{
    return type == sTextInputTypeDate || type == sTextInputTypeTime
            || type == sTextInputTypeDateTime || type == sTextInputTypeDateTimeLocal
            || type == sTextInputTypeMonth || type == sTextInputTypeWeek;
}

void InputDialogContainer::ShowPickerDialog(
    /* [in] */ Int32 dialogType,
    /* [in] */ Double dialogValue,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step)
{
    AutoPtr<ICalendar> cal;
    // |dialogValue|, |min|, |max| mean different things depending on the |dialogType|.
    // For input type=month is the number of months since 1970.
    // For input type=time it is milliseconds since midnight.
    // For other types they are just milliseconds since 1970.
    // If |dialogValue| is NaN it means an empty value. We will show the current time.
    Boolean bIsNaN = Elastos::Core::Math::IsNaN(dialogValue);
    if (bIsNaN) {
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        helper->GetInstance((ICalendar**)&cal);
        cal->Set(ICalendar::MILLISECOND, 0);
    }
    else {
        if (dialogType == sTextInputTypeMonth) {
            cal = MonthPicker::CreateDateFromValue(dialogValue);
        }
        else if (dialogType == sTextInputTypeWeek) {
            cal = WeekPicker::CreateDateFromValue(dialogValue);
        }
        else {
            AutoPtr<ITimeZoneHelper> tzHelper;
            CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
            AutoPtr<ITimeZone> utcTZ;
            tzHelper->GetTimeZone(String("UTC"), (ITimeZone**)&utcTZ);

            AutoPtr<IGregorianCalendar> gregorianCalendar;
            CGregorianCalendar::New(utcTZ, (IGregorianCalendar**)&gregorianCalendar);
            // According to the HTML spec we only use the Gregorian calendar
            // so we ignore the Julian/Gregorian transition.
            AutoPtr<IDate> date;
            CDate::New(Elastos::Core::Math::INT64_MIN_VALUE, (IDate**)&date);
            gregorianCalendar->SetGregorianChange(date);
            ICalendar::Probe(gregorianCalendar)->SetTimeInMillis((Int64) dialogValue);
            cal =  ICalendar::Probe(gregorianCalendar);
        }
    }
    if (dialogType == sTextInputTypeDate) {
        Int32 year, month, dayOfMonth;
        cal->Get(ICalendar::YEAR, &year);
        cal->Get(ICalendar::MONTH, &month);
        cal->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
        ShowPickerDialog(dialogType,
                year,
                month,
                dayOfMonth,
                0, 0, 0, 0, 0, min, max, step);
    }
    else if (dialogType == sTextInputTypeTime) {
        Int32 hourOfDay, minute;
        cal->Get(ICalendar::HOUR_OF_DAY, &hourOfDay);
        cal->Get(ICalendar::MINUTE, &minute);
        ShowPickerDialog(dialogType, 0, 0, 0,
                hourOfDay,
                minute,
                0, 0, 0, min, max, step);
    }
    else if (dialogType == sTextInputTypeDateTime ||
            dialogType == sTextInputTypeDateTimeLocal) {
        Int32 year, month, dayOfMonth, hourOfDay, minute, second;
        cal->Get(ICalendar::YEAR, &year);
        cal->Get(ICalendar::MONTH, &month);
        cal->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
        cal->Get(ICalendar::HOUR_OF_DAY, &hourOfDay);
        cal->Get(ICalendar::MINUTE, &minute);
        cal->Get(ICalendar::SECOND, &second);
        Int32 millisecond;
        cal->Get(ICalendar::MILLISECOND, &millisecond);
        ShowPickerDialog(dialogType,
                year,
                month,
                dayOfMonth,
                hourOfDay,
                minute,
                second,
                millisecond,
                0, min, max, step);
    }
    else if (dialogType == sTextInputTypeMonth) {
        Int32 year, month;
        cal->Get(ICalendar::YEAR, &year);
        cal->Get(ICalendar::MONTH, &month);
        ShowPickerDialog(dialogType, year, month, 0,
                0, 0, 0, 0, 0, min, max, step);
    }
    else if (dialogType == sTextInputTypeWeek) {
        Int32 year = WeekPicker::GetISOWeekYearForDate(cal);
        Int32 week = WeekPicker::GetWeekForDate(cal);
        ShowPickerDialog(dialogType, year, 0, 0, 0, 0, 0, 0, week, min, max, step);
    }
}

// DateTimeSuggestion in suggestions
void InputDialogContainer::ShowSuggestionDialog(
    /* [in] */ Int32 dialogType,
    /* [in] */ Double dialogValue,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    AutoPtr<IListView> suggestionListView;
    CListView::New(mContext, (IListView**)&suggestionListView);
    AutoPtr<IList> list;
    Arrays::AsList(suggestions, (IList**)&list);
    AutoPtr<DateTimeSuggestionListAdapter> adapter =
        new DateTimeSuggestionListAdapter(mContext, list);
    IAdapterView::Probe(suggestionListView)->SetAdapter(adapter);
    AutoPtr<IAdapterViewOnItemClickListener> listener = new InnerAdapterViewOnItemClickListener(
           this, dialogType, dialogValue, min, max, step, adapter);
    IAdapterView::Probe(suggestionListView)->SetOnItemClickListener(listener);

    Int32 dialogTitleId = R::string::date_picker_dialog_title;
    if (dialogType == sTextInputTypeTime) {
        dialogTitleId = R::string::time_picker_dialog_title;
    }
    else if (dialogType == sTextInputTypeDateTime ||
            dialogType == sTextInputTypeDateTimeLocal) {
        dialogTitleId = R::string::date_time_picker_dialog_title;
    }
    else if (dialogType == sTextInputTypeMonth) {
        dialogTitleId = R::string::month_picker_dialog_title;
    }
    else if (dialogType == sTextInputTypeWeek) {
        dialogTitleId = R::string::week_picker_dialog_title;
    }

    AutoPtr<IAlertDialogBuilder> dialogBuilder;
    CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&dialogBuilder);
    dialogBuilder->SetTitle(dialogTitleId);
    dialogBuilder->SetView(IView::Probe(suggestionListView));
    AutoPtr<ICharSequence> text;
    mContext->GetText(Elastos::Droid::R::string::cancel, (ICharSequence**)&text);
    AutoPtr<IDialogInterfaceOnClickListener> clickListener =  new InnerDialogInterfaceOnClickListener(this);
    dialogBuilder->SetNegativeButton(text, clickListener);
    dialogBuilder->Create((IAlertDialog**)&mDialog);

    AutoPtr<InnerDialogInterfaceOnDismissListener> dismissListener = new InnerDialogInterfaceOnDismissListener(this);
    IDialog::Probe(mDialog)->SetOnDismissListener(dismissListener);
    mDialogAlreadyDismissed = FALSE;
    (IDialog::Probe(mDialog))->Show();
}

void InputDialogContainer::ShowDialog(
    /* [in] */ Int32 type,
    /* [in] */ Double value,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    // When the web page asks to show a dialog while there is one already open,
    // dismiss the old one.
    DismissDialog();
    if (suggestions == NULL) {
        ShowPickerDialog(type, value, min, max, step);
    }
    else {
        ShowSuggestionDialog(type, value, min, max, step, suggestions);
    }
}

void InputDialogContainer::ShowPickerDialog(
    /* [in] */ Int32 dialogType,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 monthDay,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [in] */ Int32 millis,
    /* [in] */ Int32 week,
    /* [in] */ Double min,
    /* [in] */ Double max,
    /* [in] */ Double step)
{
    if (IsDialogShowing()) {
        AutoPtr<IDialogInterface> dialog = IDialogInterface::Probe(mDialog);
        dialog->Dismiss();
    }

    Int32 stepTime = (Int32) step;

    if (dialogType == sTextInputTypeDate) {
        AutoPtr<DateListener> dateListener = new DateListener(this, dialogType);
        AutoPtr<ChromeDatePickerDialog> dialog = new ChromeDatePickerDialog(mContext,
                dateListener,
                year, month, monthDay);
        AutoPtr<IDatePicker> datepicker;
        dialog->GetDatePicker((IDatePicker**)&datepicker);
        DateDialogNormalizer::Normalize(datepicker, dialog,
                 year, month, monthDay,
                 0, 0,
                 (Int64) min, (Int64) max);

        AutoPtr<ICharSequence> text;
        mContext->GetText(R::string::date_picker_dialog_title, (ICharSequence**)&text);
        dialog->SetTitle(text);
        mDialog = IAlertDialog::Probe(dialog);
    }
    else if (dialogType == sTextInputTypeTime) {
        AutoPtr<FullTimeListener> listener = new FullTimeListener(this, dialogType);
        AutoPtr<IDateFormat> dateFormat;
        CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
        Boolean bFlag = FALSE;
        dateFormat->Is24HourFormat(mContext, &bFlag);
        mDialog = new MultiFieldTimePickerDialog(
             mContext, 0 /* theme */ ,
             hourOfDay, minute, second, millis,
             (Int32) min, (Int32) max, stepTime,
             bFlag,
             listener);
    }
    else if (dialogType == sTextInputTypeDateTime ||
            dialogType == sTextInputTypeDateTimeLocal) {
        AutoPtr<DateTimeListener> listener = new DateTimeListener(this, dialogType);
        AutoPtr<IDateFormat> dateFormat;
        CDateFormat::AcquireSingleton((IDateFormat**)&dateFormat);
        Boolean bFlag = FALSE;
        dateFormat->Is24HourFormat(mContext, &bFlag);
        mDialog = new DateTimePickerDialog(mContext,
                 listener,
                 year, month, monthDay,
                 hourOfDay, minute,
                 bFlag, min, max);
    }
    else if (dialogType == sTextInputTypeMonth) {
        AutoPtr<MonthOrWeekListener> listener = new MonthOrWeekListener(this, dialogType);
        mDialog = new MonthPickerDialog(mContext, listener,
                 year, month, min, max);
    }
    else if (dialogType == sTextInputTypeWeek) {
        AutoPtr<MonthOrWeekListener> listener = new MonthOrWeekListener(this, dialogType);
        mDialog = new WeekPickerDialog(mContext, listener,
                 year, week, min, max);
    }

    if (ApiCompatibilityUtils::DatePickerRequiresAccept()) {
        AutoPtr<ICharSequence> text;
        mContext->GetText(R::string::date_picker_dialog_set, (ICharSequence**)&text);
        mDialog->SetButton(IDialogInterface::BUTTON_POSITIVE,
                 text,
                 IDialogInterfaceOnClickListener::Probe(mDialog));
    }

    AutoPtr<ICharSequence> text1;
    mContext->GetText(Elastos::Droid::R::string::cancel, (ICharSequence**)&text1);
    mDialog->SetButton(IDialogInterface::BUTTON_NEGATIVE,
             text1,
             (IDialogInterfaceOnClickListener*) NULL);

    AutoPtr<ICharSequence> text2;
    mContext->GetText(R::string::date_picker_dialog_clear, (ICharSequence**)&text2);
    AutoPtr<IDialogInterfaceOnClickListener> clickListener = new SetButtonDialogInterfaceOnClickListener(this);
    mDialog->SetButton(IDialogInterface::BUTTON_NEUTRAL,
            text2,
            clickListener);

    AutoPtr<SetOnDismissListenerDialogInterfaceOnDismissListener> onDismissListener =
            new SetOnDismissListenerDialogInterfaceOnDismissListener(this);
    AutoPtr<IDialog> _dialog = IDialog::Probe(mDialog);
    _dialog->SetOnDismissListener(onDismissListener);

    mDialogAlreadyDismissed = FALSE;
    _dialog->Show();
}

Boolean InputDialogContainer::IsDialogShowing()
{
    Boolean bFlag = FALSE;
    AutoPtr<IDialog> dialog = IDialog::Probe(mDialog);
    return mDialog != NULL && (dialog->IsShowing(&bFlag), bFlag);
}

void InputDialogContainer::DismissDialog()
{
    if (IsDialogShowing()) {
        AutoPtr<IDialogInterface> dialog = IDialogInterface::Probe(mDialog);
        dialog->Dismiss();
    }
}

void InputDialogContainer::SetFieldDateTimeValue(
    /* [in] */ Int32 dialogType,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 monthDay,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Int32 second,
    /* [in] */ Int32 millis,
    /* [in] */ Int32 week)
{
    // Prevents more than one callback being sent to the native
    // side when the dialog triggers multiple events.
    if (mDialogAlreadyDismissed)
        return;

    mDialogAlreadyDismissed = TRUE;

    if (dialogType == sTextInputTypeMonth) {
        mInputActionDelegate->ReplaceDateTime((year - 1970) * 12 + month);
    }
    else if (dialogType == sTextInputTypeWeek) {
        Int64 timeMillis;
        WeekPicker::CreateDateFromWeek(year, week)->GetTimeInMillis(&timeMillis);
        mInputActionDelegate->ReplaceDateTime(timeMillis);
    }
    else if (dialogType == sTextInputTypeTime) {
        AutoPtr<ITimeUnitHelper> helper;
        CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
        AutoPtr<ITimeUnit> iHourOfDay;
        helper->GetHOURS((ITimeUnit**)&iHourOfDay);
        Int64 _hourOfDay;
        iHourOfDay->ToMillis(hourOfDay, &_hourOfDay);
        AutoPtr<ITimeUnit> iMinute;
        helper->GetMINUTES((ITimeUnit**)&iMinute);
        Int64 _minute;
        iMinute->ToMillis(minute, &_minute);
        AutoPtr<ITimeUnit> iSecond;
        helper->GetSECONDS((ITimeUnit**)&iSecond);
        Int64 _second;
        iSecond->ToMillis(second, &_second);
        mInputActionDelegate->ReplaceDateTime(_hourOfDay + _minute + _second + millis);
    }
    else {
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        AutoPtr<ICalendar> cal;
        AutoPtr<ITimeZoneHelper> timeZoneHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timeZoneHelper);
        AutoPtr<ITimeZone> timeZone;
        timeZoneHelper->GetTimeZone(String("UTC"), (ITimeZone**)&timeZone);
        helper->GetInstance(timeZone, (ICalendar**)&cal);
        cal->Clear();
        cal->Set(ICalendar::YEAR, year);
        cal->Set(ICalendar::MONTH, month);
        cal->Set(ICalendar::DAY_OF_MONTH, monthDay);
        cal->Set(ICalendar::HOUR_OF_DAY, hourOfDay);
        cal->Set(ICalendar::MINUTE, minute);
        cal->Set(ICalendar::SECOND, second);
        cal->Set(ICalendar::MILLISECOND, millis);
        Int64 timeInMillis;
        cal->GetTimeInMillis(&timeInMillis);
        mInputActionDelegate->ReplaceDateTime(timeInMillis);
    }
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
