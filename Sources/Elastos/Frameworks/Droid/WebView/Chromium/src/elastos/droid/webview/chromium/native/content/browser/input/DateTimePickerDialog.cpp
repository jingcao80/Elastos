
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateTimePickerDialog.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/DateDialogNormalizer.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"
#include "elastos/droid/R.h"

using Elastos::Droid::App::EIID_IAlertDialog;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;
using Elastos::Droid::Widget::EIID_ITimePickerOnTimeChangedListener;
using Elastos::Droid::Widget::EIID_IDatePickerOnDateChangedListener;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Input {

CAR_INTERFACE_IMPL_3(DateTimePickerDialog::InnerListener, Object,
    IDialogInterfaceOnClickListener, IDatePickerOnDateChangedListener,
    ITimePickerOnTimeChangedListener);

DateTimePickerDialog::InnerListener::InnerListener(
    /* [in] */ DateTimePickerDialog* host)
    : mHost(host)
{}

ECode DateTimePickerDialog::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

ECode DateTimePickerDialog::InnerListener::OnDateChanged(
    /* [in] */ IDatePicker* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    return mHost->OnDateChanged(view, year, month, day);
}

ECode DateTimePickerDialog::InnerListener::OnTimeChanged(
    /* [in] */ ITimePicker* view,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    return mHost->OnTimeChanged(view, hourOfDay, minute);
}


DateTimePickerDialog::DateTimePickerDialog()
    : mMinTimeMillis(0)
    , mMaxTimeMillis(0)
{}

/**
 * @param context The context the dialog is to run in.
 * @param callBack How the parent is notified that the date is set.
 * @param year The initial year of the dialog.
 * @param monthOfYear The initial month of the dialog.
 * @param dayOfMonth The initial day of the dialog.
 */
ECode DateTimePickerDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ DateTimePickerDialog::OnDateTimeSetListener* callBack,
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Boolean is24HourView,
    /* [in] */ Double min,
    /* [in] */ Double max)
{
    AlertDialog::constructor(context, 0);

    mCallBack = callBack;
    mMinTimeMillis = (Int64)min;
    mMaxTimeMillis = (Int64)max;
    mListener = new InnerListener(this);

    AutoPtr<ICharSequence> setCS;
    context->GetText(R::string::date_picker_dialog_set, (ICharSequence**)&setCS);
     SetButton(IDialogInterface::BUTTON_POSITIVE, setCS, this);
    AutoPtr<ICharSequence> cancelCS;
    context->GetText(Elastos::Droid::R::string::cancel, (ICharSequence**)&cancelCS);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, cancelCS,
             (IMessage*)NULL);
    SetIcon(0);
    AutoPtr<ICharSequence> titleCS;
    context->GetText(R::string::date_time_picker_dialog_title, (ICharSequence**)&titleCS);
    SetTitle(titleCS);

    AutoPtr<ILayoutInflater> inflater;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj);
    inflater = ILayoutInflater::Probe(obj);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::date_time_picker_dialog, NULL, (IView**)&view);
    SetView(view);
    AutoPtr<IView> tmpView;
    view->FindViewById(R::id::date_picker, (IView**)&tmpView);
    mDatePicker = IDatePicker::Probe(tmpView);
    DateDialogNormalizer::Normalize(mDatePicker, this,
        year, monthOfYear, dayOfMonth, hourOfDay, minute, mMinTimeMillis, mMaxTimeMillis);

    tmpView = NULL;
    view->FindViewById(R::id::time_picker, (IView**)&tmpView);
    mTimePicker = ITimePicker::Probe(tmpView);
    mTimePicker->SetIs24HourView(is24HourView);
    mTimePicker->SetCurrentHour(hourOfDay);
    mTimePicker->SetCurrentMinute(minute);
    mTimePicker->SetOnTimeChangedListener(mListener);

    Int32 currentHour, currentMinute;
    mTimePicker->GetCurrentHour(&currentHour);
    mTimePicker->GetCurrentMinute(&currentMinute);
    return OnTimeChanged(mTimePicker, currentHour, currentMinute);
}

//@Override
ECode DateTimePickerDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return TryNotifyDateTimeSet();
}

ECode DateTimePickerDialog::TryNotifyDateTimeSet()
{
    if (mCallBack != NULL) {
        AutoPtr<IView> view = IView::Probe(mDatePicker);
        view->ClearFocus();
        Int32 year, month, dayOfMonth, currentHour, currentMinute;
        mDatePicker->GetYear(&year);
        mDatePicker->GetMonth(&month);
        mDatePicker->GetDayOfMonth(&dayOfMonth);
        mTimePicker->GetCurrentHour(&currentHour);
        mTimePicker->GetCurrentMinute(&currentMinute);
        mCallBack->OnDateTimeSet(mDatePicker, mTimePicker, year,
            month, dayOfMonth, currentHour, currentMinute);
    }

    return NOERROR;
}

//@Override
ECode DateTimePickerDialog::OnDateChanged(
    /* [in] */ IDatePicker* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    // Signal a time change so the max/min checks can be applied.
    if (mTimePicker != NULL) {
        Int32 hour, minute;
        mTimePicker->GetCurrentHour(&hour);
        mTimePicker->GetCurrentMinute(&minute);
        OnTimeChanged(mTimePicker, hour, minute);
    }

    return NOERROR;
}

//@Override
ECode DateTimePickerDialog::OnTimeChanged(
    /* [in] */ ITimePicker* view,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    AutoPtr<ITime> time;
    CTime::New((ITime**)&time);
    Int32 currentMinute, currentHour, dayOfMonth, month, year;
    mTimePicker->GetCurrentMinute(&currentMinute);
    mTimePicker->GetCurrentHour(&currentHour);
    mDatePicker->GetDayOfMonth(&dayOfMonth);
    mDatePicker->GetMonth(&month);
    mDatePicker->GetYear(&year);
    time->Set(0, currentMinute,
            currentHour, dayOfMonth,
            month, year);

    Int64 millis;
    time->ToMillis(TRUE, &millis);
    if (millis < mMinTimeMillis) {
        time->Set(mMinTimeMillis);
    }
    else if (millis > mMaxTimeMillis) {
        time->Set(mMaxTimeMillis);
    }

    Int32 tHour, tMinute;
    time->GetHour(&tHour);
    time->GetMinute(&tMinute);
    mTimePicker->SetCurrentHour(tHour);
    mTimePicker->SetCurrentMinute(tMinute);

    return NOERROR;
}

/**
 * Sets the current date.
 *
 * @param year The date year.
 * @param monthOfYear The date month.
 * @param dayOfMonth The date day of month.
 */
void DateTimePickerDialog::UpdateDateTime(
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minuteOfHour)
{
    mDatePicker->UpdateDate(year, monthOfYear, dayOfMonth);
    mTimePicker->SetCurrentHour(hourOfDay);
    mTimePicker->SetCurrentMinute(minuteOfHour);
}

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
