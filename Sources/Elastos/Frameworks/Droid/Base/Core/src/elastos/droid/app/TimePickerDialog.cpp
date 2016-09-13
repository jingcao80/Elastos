
#include "elastos/droid/app/TimePickerDialog.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::EIID_ITimePickerOnTimeChangedListener;
using Elastos::Droid::Widget::EIID_ITimePickerValidationCallback;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace App {

//========================================================================================
// TimePickerDialog::InnerListener
//========================================================================================
CAR_INTERFACE_IMPL_3(TimePickerDialog::InnerListener, Object, ITimePickerValidationCallback, \
    IDialogInterfaceOnClickListener, ITimePickerOnTimeChangedListener)

TimePickerDialog::InnerListener::InnerListener(
    /* [in] */ TimePickerDialog* host)
    : mHost(host)
{}

//@Override
ECode TimePickerDialog::InnerListener::OnValidationChanged(
    /* [in] */ Boolean valid)
{
    AutoPtr<IButton> positive;
    mHost->GetButton(IDialogInterface::BUTTON_POSITIVE, (IButton**)&positive);
    if (positive != NULL) {
        IView::Probe(positive)->SetEnabled(valid);
    }
    return NOERROR;
}

ECode TimePickerDialog::InnerListener::OnTimeChanged(
    /* [in] */ ITimePicker* view,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    return mHost->OnTimeChanged(view, hourOfDay, minute);
}

ECode TimePickerDialog::InnerListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return mHost->OnClick(dialog, which);
}

//========================================================================================
// TimePickerDialog
//========================================================================================
const String TimePickerDialog::HOUR("hour");
const String TimePickerDialog::MINUTE("minute");
const String TimePickerDialog::IS_24_HOUR("is24hour");

CAR_INTERFACE_IMPL(TimePickerDialog, AlertDialog, ITimePickerDialog)

TimePickerDialog::TimePickerDialog()
    : mInitialHourOfDay(0)
    , mInitialMinute(0)
    , mIs24HourView(FALSE)
{}

TimePickerDialog::~TimePickerDialog()
{}

ECode TimePickerDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ITimePickerDialogOnTimeSetListener* callBack,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Boolean is24HourView)
{
    return constructor(context, 0, callBack, hourOfDay, minute, is24HourView);
}

ECode TimePickerDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ ITimePickerDialogOnTimeSetListener* callBack,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute,
    /* [in] */ Boolean is24HourView)
{
    AlertDialog::constructor(context, ResolveDialogTheme(context, theme));

    mListener = new InnerListener(this);

    mTimeSetCallback = callBack;
    mInitialHourOfDay = hourOfDay;
    mInitialMinute = minute;
    mIs24HourView = is24HourView;

    AutoPtr<IContext> themeContext;
    GetContext((IContext**)&themeContext);
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(themeContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::time_picker_dialog, NULL, (IView**)&view);
    SetView(view);

    String ok, cancel;
    themeContext->GetString(R::string::ok, &ok);
    themeContext->GetString(R::string::cancel, &cancel);
    SetButton(IDialogInterface::BUTTON_POSITIVE, CoreUtils::Convert(ok), mListener);
    SetButton(IDialogInterface::BUTTON_NEGATIVE, CoreUtils::Convert(cancel), mListener);
    SetButtonPanelLayoutHint(IAlertDialog::LAYOUT_HINT_SIDE);

    AutoPtr<IView> tmp;
    view->FindViewById(R::id::timePicker, (IView**)&tmp);
    mTimePicker = ITimePicker::Probe(tmp);
    mTimePicker->SetIs24HourView(mIs24HourView);
    mTimePicker->SetCurrentHour(mInitialHourOfDay);
    mTimePicker->SetCurrentMinute(mInitialMinute);
    mTimePicker->SetOnTimeChangedListener(mListener);
    mTimePicker->SetValidationCallback(mListener);
    return NOERROR;
}

Int32 TimePickerDialog::ResolveDialogTheme(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid)
{
    if (resid == 0) {
        AutoPtr<ITypedValue> outValue;
        CTypedValue::New((ITypedValue**)&outValue);
        AutoPtr<IResourcesTheme> theme;
        context->GetTheme((IResourcesTheme**)&theme);
        Boolean bval;
        theme->ResolveAttribute(R::attr::timePickerDialogTheme, outValue, TRUE, &bval);
        Int32 id;
        outValue->GetResourceId(&id);
        return id;
    }
    else {
        return resid;
    }
}

ECode TimePickerDialog::OnTimeChanged(
    /* [in] */ ITimePicker* view,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    /* do nothing */
    return NOERROR;
}

ECode TimePickerDialog::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    switch (which) {
    case IDialogInterface::BUTTON_POSITIVE:
        if (mTimeSetCallback != NULL) {
            Int32 hour, minute;
            mTimePicker->GetCurrentHour(&hour);
            mTimePicker->GetCurrentMinute(&minute);
            mTimeSetCallback->OnTimeSet(mTimePicker, hour, minute);
        }
        break;
    }
    return NOERROR;
}

ECode TimePickerDialog::UpdateTime(
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minuteOfHour)
{
    mTimePicker->SetCurrentHour(hourOfDay);
    mTimePicker->SetCurrentMinute(minuteOfHour);
    return NOERROR;
}

ECode TimePickerDialog::OnSaveInstanceState(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBundle> state;
    AlertDialog::OnSaveInstanceState((IBundle**)&state);
    Int32 hour, minute;
    Boolean bval;
    mTimePicker->GetCurrentHour(&hour);
    mTimePicker->GetCurrentMinute(&minute);
    state->PutInt32(HOUR, hour);
    state->PutInt32(MINUTE, minute);
    mTimePicker->Is24HourView(&bval);
    state->PutBoolean(IS_24_HOUR, bval);
    *result = state;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TimePickerDialog::OnRestoreInstanceState(
    /* [in] */ IBundle* savedInstanceState)
{
    AlertDialog::OnRestoreInstanceState(savedInstanceState);
    Int32 hour, minute;
    savedInstanceState->GetInt32(HOUR, &hour);
    savedInstanceState->GetInt32(MINUTE, &minute);
    Boolean bval;
    savedInstanceState->GetBoolean(IS_24_HOUR, &bval);
    mTimePicker->SetIs24HourView(bval);
    mTimePicker->SetCurrentHour(hour);
    mTimePicker->SetCurrentMinute(minute);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
