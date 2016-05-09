
#include <Elastos.CoreLibrary.Utility.h>
#include "CActivityOne.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Widget::EIID_IDatePickerOnDateChangedListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace DevSamples {
namespace DatePickerDemo {

static const String DBG_TAG("CActivityOne");

//============================================================
//CActivityOne::DatePickerOnDateChangedListener
//============================================================

CAR_INTERFACE_IMPL(CActivityOne::DatePickerOnDateChangedListener, Object, IDatePickerOnDateChangedListener)

CActivityOne::DatePickerOnDateChangedListener::DatePickerOnDateChangedListener(
    /* [in] */ CActivityOne* host)
    : mHost(host)
{}

CActivityOne::DatePickerOnDateChangedListener::~DatePickerOnDateChangedListener()
{}

ECode CActivityOne::DatePickerOnDateChangedListener::OnDateChanged(
    /* [in] */ IDatePicker* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth)
{
    StringBuilder buider("");
    buider += "您选择的日期是：";
    buider += year;
    buider += "年";
    buider += monthOfYear + 1;
    buider += "月";
    buider += dayOfMonth;
    buider += "日。";

    return ITextView::Probe(mHost->mEditText)->SetText(buider.ToCharSequence());
}

//=======================================================================
// CActivityOne
//=======================================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(DBG_TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Logger::I(DBG_TAG, " >> OnCreate");
    SetContentView(R::layout::main);

    AutoPtr<IView> view = FindViewById(R::id::datePicker);
    mDatePicker = IDatePicker::Probe(view);
    // mDate->UpdateDate(2016, 5, 3);

    view = FindViewById(R::id::dateEt);
    mEditText = IEditText::Probe(view);

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance((ICalendar**)&calendar);

    Int32 year, monthOfYear, dayOfMonth;
    calendar->Get(ICalendar::YEAR, &year);
    calendar->Get(ICalendar::MONTH, &monthOfYear);
    calendar->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
    AutoPtr<IDatePickerOnDateChangedListener> listener = new DatePickerOnDateChangedListener(this);
    mDatePicker->Init(year, monthOfYear, dayOfMonth, listener);

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    Logger::I(DBG_TAG, " >> OnStart()");
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    Logger::I(DBG_TAG, " >> OnResume()");
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    Logger::I(DBG_TAG, " >> OnPause()");
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    Logger::I(DBG_TAG, " >> OnStop()");
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    Logger::I(DBG_TAG, " >> OnDestroy()");
    return Activity::OnDestroy();
}

} // namespace DatePickerDemo
} // namespace DevSamples
} // namespace Elastos
