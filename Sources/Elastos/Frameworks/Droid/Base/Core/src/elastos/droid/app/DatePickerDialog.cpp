
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Text.h"
#include "elastos/droid/app/DatePickerDialog.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::EIID_IDatePickerValidationCallback;
using Elastos::Droid::Widget::EIID_IDatePickerOnDateChangedListener;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;

namespace Elastos {
namespace Droid {
namespace App {

//=====================================================================
// DatePickerDialog::MyValidationCallback
//=====================================================================
CAR_INTERFACE_IMPL(DatePickerDialog::MyValidationCallback, Object, IDatePickerValidationCallback)

DatePickerDialog::MyValidationCallback::MyValidationCallback(
    /* [in] */ DatePickerDialog* host)
    : mHost(host)
{}

//@Override
ECode DatePickerDialog::MyValidationCallback::OnValidationChanged(
    /* [in] */ Boolean valid)
{
    AutoPtr<IButton> positive;
    mHost->GetButton(IDialogInterface::BUTTON_POSITIVE, (IButton**)&positive);
    if (positive != NULL) {
        IView::Probe(positive)->SetEnabled(valid);
    }
    return NOERROR;
}

//=====================================================================
// DatePickerDialog
//=====================================================================

const String DatePickerDialog::YEAR("year");
const String DatePickerDialog::MONTH("month");
const String DatePickerDialog::DAY("day");

CAR_INTERFACE_IMPL_3(DatePickerDialog, AlertDialog, IDatePickerDialog, \
    IDialogInterfaceOnClickListener, IDatePickerOnDateChangedListener)

DatePickerDialog::DatePickerDialog()
    : mTitleNeedsUpdate(TRUE)
{
}

DatePickerDialog::~DatePickerDialog()
{}

ECode DatePickerDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDatePickerDialogOnDateSetListener* listener,
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth)
{
    return constructor(context, 0, listener, year, monthOfYear, dayOfMonth);
}

ECode DatePickerDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ IDatePickerDialogOnDateSetListener* listener,
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth)
{
    AlertDialog::constructor(context, ResolveDialogTheme(context, theme));

    mValidationCallback = new MyValidationCallback(this);
    mDateSetListener = listener;

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance((ICalendar**)&mCalendar);

    AutoPtr<IContext> themeContext;
    GetContext((IContext**)&themeContext);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(themeContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::date_picker_dialog, NULL, (IView**)&view);
    SetView(view);
    String ok, cancel;
    themeContext->GetString(R::string::ok, &ok);
    themeContext->GetString(R::string::cancel, &cancel);
    SetButton(IDialogInterface::BUTTON_POSITIVE, CoreUtils::Convert(ok), this);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, CoreUtils::Convert(cancel), this);
    SetButtonPanelLayoutHint(IAlertDialog::LAYOUT_HINT_SIDE);

    view = NULL;
    view->FindViewById(R::id::datePicker, (IView**)&view);
    mDatePicker = IDatePicker::Probe(view);
    mDatePicker->Init(year, monthOfYear, dayOfMonth, this);
    mDatePicker->SetValidationCallback(mValidationCallback);
    return NOERROR;
}

Int32 DatePickerDialog::ResolveDialogTheme(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid)
{
    if (resid == 0) {
        AutoPtr<ITypedValue> outValue;
        CTypedValue::New((ITypedValue**)&outValue);
        AutoPtr<IResourcesTheme> theme;
        context->GetTheme((IResourcesTheme**)&theme);
        Boolean bval;
        theme->ResolveAttribute(R::attr::datePickerDialogTheme, outValue, TRUE, &bval);
        Int32 id;
        outValue->GetResourceId(&id);
        return id;
    }
    else {
        return resid;
    }
}

ECode DatePickerDialog::OnDateChanged(
    /* [in] */ IDatePicker* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    mDatePicker->Init(year, month, day, this);
    UpdateTitle(year, month, day);
    return NOERROR;
}

ECode DatePickerDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    switch (which) {
        case IDialogInterface::BUTTON_POSITIVE:
            if (mDateSetListener != NULL) {
                Int32 year, month, dayOfMonth;
                mDatePicker->GetYear(&year);
                mDatePicker->GetMonth(&month);
                mDatePicker->GetDayOfMonth(&dayOfMonth);
                mDateSetListener->OnDateSet(mDatePicker, year, month, dayOfMonth);
            }
            break;
        case IDialogInterface::BUTTON_NEGATIVE:
            Cancel();
            break;
    }
    return NOERROR;
}

ECode DatePickerDialog::GetDatePicker(
    /* [out] */ IDatePicker** picker)
{
    VALIDATE_NOT_NULL(picker)
    *picker = mDatePicker;
    REFCOUNT_ADD(*picker)
    return NOERROR;
}

ECode DatePickerDialog::UpdateDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth)
{
    return mDatePicker->UpdateDate(year, monthOfYear, dayOfMonth);
}

void DatePickerDialog::UpdateTitle(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    Boolean bval;
    mDatePicker->GetCalendarViewShown(&bval);
    if (!bval) {
        mCalendar->Set(ICalendar::YEAR, year);
        mCalendar->Set(ICalendar::MONTH, month);
        mCalendar->Set(ICalendar::DAY_OF_MONTH, day);
        Int64 ms;
        mCalendar->GetTimeInMillis(&ms);
        String title = DateUtils::FormatDateTime(mContext,
            ms,
            IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_SHOW_WEEKDAY
            | IDateUtils::FORMAT_SHOW_YEAR | IDateUtils::FORMAT_ABBREV_MONTH
            | IDateUtils::FORMAT_ABBREV_WEEKDAY);
        SetTitle(CoreUtils::Convert(title));
        mTitleNeedsUpdate = TRUE;
    }
    else {
        if (mTitleNeedsUpdate) {
            mTitleNeedsUpdate = FALSE;
            SetTitle(R::string::date_picker_dialog_title);
        }
    }
}

ECode DatePickerDialog::OnSaveInstanceState(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    Int32 year, month, dayOfMonth;
    mDatePicker->GetYear(&year);
    mDatePicker->GetMonth(&month);
    mDatePicker->GetDayOfMonth(&dayOfMonth);

    AutoPtr<IBundle> state;
    AlertDialog::OnSaveInstanceState((IBundle**)&state);
    state->PutInt32(YEAR, year);
    state->PutInt32(MONTH, month);
    state->PutInt32(DAY, dayOfMonth);
    *bundle = state;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

ECode DatePickerDialog::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    AlertDialog::OnRestoreInstanceState(savedInstanceState);
    Int32 year, month, dayOfMonth;
    savedInstanceState->GetInt32(YEAR, &year);
    savedInstanceState->GetInt32(MONTH, &month);
    savedInstanceState->GetInt32(DAY, &dayOfMonth);

    return mDatePicker->Init(year, month, dayOfMonth, this);
}

} // namespace App
} // namespace Droid
} // namespace Elastos