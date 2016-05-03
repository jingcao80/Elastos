#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/widget/DatePicker.h"
#include "elastos/droid/widget/CNumberPickerHelper.h"
#include "elastos/droid/widget/DatePickerCalendarDelegate.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/format/DateUtils.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/view/inputmethod/CInputMethodManager.h"
#include "elastos/droid/R.h"
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::View::InputMethod::CInputMethodManager;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::INumberPickerHelper;
using Elastos::Droid::Widget::CNumberPickerHelper;
using Elastos::Droid::Widget::DatePickerCalendarDelegate;
using Elastos::Core::CoreUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::IDateFormatSymbols;
using Elastos::Text::CDateFormatSymbols;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::Arrays;
using Libcore::ICU::IICUUtil;
using Libcore::ICU::CICUUtil;

namespace Elastos {
namespace Droid {
namespace Widget {

const String DatePicker::TAG("DatePicker");

const Int32 DatePicker::MODE_SPINNER = 1;
const Int32 DatePicker::MODE_CALENDAR = 2;

// ==================================================================
//                DatePicker::DatePickerOnChangeListener
// ==================================================================
CAR_INTERFACE_IMPL(DatePicker::DatePickerOnChangeListener, Object, INumberPickerOnValueChangeListener)

DatePicker::DatePickerOnChangeListener::DatePickerOnChangeListener(
    /* [in] */ DatePickerSpinnerDelegate* host)
    : mHost(host)
{
}

ECode DatePicker::DatePickerOnChangeListener::OnValueChange(
    /* [in] */ INumberPicker* picker,
    /* [in] */ Int32 oldVal,
    /* [in] */ Int32 newVal)
{
    mHost->UpdateInputState();
    Int64 millis = 0;
    mHost->mCurrentDate->GetTimeInMillis(&millis);
    mHost->mTempDate->SetTimeInMillis(millis);
    // take care of wrapping of days and months to update greater fields
    if (picker == mHost->mDaySpinner) {
        Int32 maxDayOfMonth = 0;
        mHost->mTempDate->GetActualMaximum(ICalendar::DAY_OF_MONTH, &maxDayOfMonth);
        if (oldVal == maxDayOfMonth && newVal == 1) {
            mHost->mTempDate->Add(ICalendar::DAY_OF_MONTH, 1);
        }
        else if (oldVal == 1 && newVal == maxDayOfMonth) {
            mHost->mTempDate->Add(ICalendar::DAY_OF_MONTH, -1);
        }
        else {
            mHost->mTempDate->Add(ICalendar::DAY_OF_MONTH, newVal - oldVal);
        }
    }
    else if (picker == mHost->mMonthSpinner) {
        if (oldVal == 11 && newVal == 0) {
            mHost->mTempDate->Add(ICalendar::MONTH, 1);
        }
        else if (oldVal == 0 && newVal == 11) {
            mHost->mTempDate->Add(ICalendar::MONTH, -1);
        }
        else {
            mHost->mTempDate->Add(ICalendar::MONTH, newVal - oldVal);
        }
    }
    else if (picker == mHost->mYearSpinner) {
        mHost->mTempDate->Set(ICalendar::YEAR, newVal);
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // now set the date to the adjusted one
    Int32 year = 0, month = 0, day = 0;
    mHost->mTempDate->Get(ICalendar::YEAR, &year);
    mHost->mTempDate->Get(ICalendar::MONTH, &month);
    mHost->mTempDate->Get(ICalendar::DAY_OF_MONTH, &day);
    mHost->SetDate(year, month, day);
    mHost->UpdateSpinners();
    mHost->UpdateCalendarView();
    mHost->NotifyDateChanged();
    return NOERROR;
}

// ==================================================================
//                DatePicker::DatePickerOnDateChangeListener
// ==================================================================
CAR_INTERFACE_IMPL(DatePicker::DatePickerOnDateChangeListener, Object, IOnDateChangeListener)

DatePicker::DatePickerOnDateChangeListener::DatePickerOnDateChangeListener(
    /* [in] */ DatePickerSpinnerDelegate* host)
    : mHost(host)
{
}

ECode DatePicker::DatePickerOnDateChangeListener::OnSelectedDayChange(
    /* [in] */ ICalendarView* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth)
{
    mHost->SetDate(year, month, dayOfMonth);
    mHost->UpdateSpinners();
    mHost->NotifyDateChanged();
    return NOERROR;
}

// ==================================================================
//                DatePicker::DatePickerSavedState
// ==================================================================

DatePicker::DatePickerSavedState::DatePickerSavedState(
    /* [in] */ IParcelable* superState,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    BaseSavedState::constructor(superState);
    mYear = year;
    mMonth = month;
    mDay = day;
}

ECode DatePicker::DatePickerSavedState::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mYear);
    in->ReadInt32(&mMonth);
    in->ReadInt32(&mDay);
    return NOERROR;
}

ECode DatePicker::DatePickerSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    BaseSavedState::WriteToParcel(dest);
    dest->WriteInt32(mYear);
    dest->WriteInt32(mMonth);
    dest->WriteInt32(mDay);
    return NOERROR;
}

ECode DatePicker::DatePickerSavedState::GetSuperState(
    /* [out] */ IParcelable** state)
{
    VALIDATE_NOT_NULL(state);
    AutoPtr<IParcelable> parcelable;
    AbsSavedState::GetSuperState((IParcelable**)&parcelable);
    *state = parcelable;
    REFCOUNT_ADD(*state);
    return NOERROR;
}

//===============================================================
// DatePicker::AbstractDatePickerDelegate::
//===============================================================
CAR_INTERFACE_IMPL(DatePicker::AbstractDatePickerDelegate, Object, IDatePickerDelegate)

DatePicker::AbstractDatePickerDelegate::AbstractDatePickerDelegate()
{}

ECode DatePicker::AbstractDatePickerDelegate::constructor(
    /* [in] */ IDatePicker* delegator,
    /* [in] */ IContext* context)
{
    mDelegator = delegator;
    mContext = context;

    // initialization based on locale
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> loc;
    hlp->GetDefault((ILocale**)&loc);
    SetCurrentLocale(loc);
    return NOERROR;
}

void DatePicker::AbstractDatePickerDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    Boolean bEqls = FALSE;
    if ((locale->Equals(mCurrentLocale, &bEqls), bEqls)) {
        return;
    }
    mCurrentLocale = locale;
}

ECode DatePicker::AbstractDatePickerDelegate::SetValidationCallback(
    /* [in] */ IDatePickerValidationCallback* callback)
{
    mValidationCallback = callback;
    return NOERROR;
}

void DatePicker::AbstractDatePickerDelegate::OnValidationChanged(
    /* [in] */ Boolean valid)
{
    if (mValidationCallback != NULL) {
        mValidationCallback->OnValidationChanged(valid);
    }
}

//===============================================================
// DatePicker::DatePickerSpinnerDelegate::
//===============================================================

const String DatePicker::DatePickerSpinnerDelegate::DATE_FORMAT("MM/dd/yyyy");

const Int32 DatePicker::DatePickerSpinnerDelegate::DEFAULT_START_YEAR = 1900;

const Int32 DatePicker::DatePickerSpinnerDelegate::DEFAULT_END_YEAR = 2100;

const Boolean DatePicker::DatePickerSpinnerDelegate::DEFAULT_CALENDAR_VIEW_SHOWN = TRUE;

const Boolean DatePicker::DatePickerSpinnerDelegate::DEFAULT_SPINNERS_SHOWN = TRUE;

const Boolean DatePicker::DatePickerSpinnerDelegate::DEFAULT_ENABLED_STATE = TRUE;

DatePicker::DatePickerSpinnerDelegate::DatePickerSpinnerDelegate()
    : mIsEnabled(DEFAULT_ENABLED_STATE)
{
    CSimpleDateFormat::New(DATE_FORMAT, (IDateFormat**)&mDateFormat);
}

ECode DatePicker::DatePickerSpinnerDelegate::constructor(
    /* [in] */ IDatePicker* delegator,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(AbstractDatePickerDelegate::constructor(delegator, context))

    mDelegator = delegator;
    mContext = context;

    // initialization based on locale
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> loc;
    hlp->GetDefault((ILocale**)&loc);
    SetCurrentLocale(loc);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::DatePicker),
            ArraySize(R::styleable::DatePicker));
    AutoPtr<ITypedArray> attributesArray;
    context->ObtainStyledAttributes(attrs,
            attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&attributesArray);
    Boolean spinnersShown = FALSE;
    attributesArray->GetBoolean(R::styleable::DatePicker_spinnersShown,
            DEFAULT_SPINNERS_SHOWN, &spinnersShown);
    Boolean calendarViewShown = FALSE;
    attributesArray->GetBoolean(
            R::styleable::DatePicker_calendarViewShown, DEFAULT_CALENDAR_VIEW_SHOWN, &calendarViewShown);
    Int32 startYear = 0;
    attributesArray->GetInt32(R::styleable::DatePicker_startYear,
            DEFAULT_START_YEAR, &startYear);
    Int32 endYear = 0;
    attributesArray->GetInt32(R::styleable::DatePicker_endYear, DEFAULT_END_YEAR, &endYear);
    String minDate;
    attributesArray->GetString(R::styleable::DatePicker_minDate, &minDate);
    String maxDate;
    attributesArray->GetString(R::styleable::DatePicker_maxDate, &maxDate);
    Int32 layoutResourceId = 0;
    attributesArray->GetResourceId(
            R::styleable::DatePicker_legacyLayout, R::layout::date_picker_legacy, &layoutResourceId);
    attributesArray->Recycle();

    AutoPtr<IInterface> inf;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&inf);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(inf);
    AutoPtr<IView> vRes;
    inflater->Inflate(layoutResourceId, IViewGroup::Probe(mDelegator), TRUE, (IView**)&vRes);

    AutoPtr<DatePickerOnChangeListener> onChangeListener = new DatePickerOnChangeListener(this);

    AutoPtr<IView> vPic;
    IView::Probe(mDelegator)->FindViewById(R::id::pickers, (IView**)&vPic);
    mSpinners = ILinearLayout::Probe(vPic);

    // calendar view day-picker
    AutoPtr<IView> vView;
    IView::Probe(mDelegator)->FindViewById(R::id::calendar_view, (IView**)&vView);
    mCalendarView = ICalendarView::Probe(vView);
    AutoPtr<DatePickerOnDateChangeListener> p = new DatePickerOnDateChangeListener(this);
    mCalendarView->SetOnDateChangeListener(p);

    // day
    AutoPtr<IView> vD;
    IView::Probe(mDelegator)->FindViewById(R::id::day, (IView**)&vD);
    mDaySpinner = INumberPicker::Probe(vD);
    AutoPtr<INumberPickerHelper> hlper;
    CNumberPickerHelper::AcquireSingleton((INumberPickerHelper**)&hlper);
    AutoPtr<INumberPickerFormatter> fm;
    hlper->GetTwoDigitFormatter((INumberPickerFormatter**)&fm);
    mDaySpinner->SetFormatter(fm);
    mDaySpinner->SetOnLongPressUpdateInterval(100);
    mDaySpinner->SetOnValueChangedListener(onChangeListener);
    AutoPtr<IView> vInput;
    IView::Probe(mDaySpinner)->FindViewById(R::id::numberpicker_input, (IView**)&vInput);
    mDaySpinnerInput = IEditText::Probe(vInput);

    // month
    AutoPtr<IView> vMonth;
    IView::Probe(mDelegator)->FindViewById(R::id::month, (IView**)&vMonth);
    mMonthSpinner = INumberPicker::Probe(vMonth);
    mMonthSpinner->SetMinValue(0);
    mMonthSpinner->SetMaxValue(mNumberOfMonths - 1);
    mMonthSpinner->SetDisplayedValues(mShortMonths);
    mMonthSpinner->SetOnLongPressUpdateInterval(200);
    mMonthSpinner->SetOnValueChangedListener(onChangeListener);
    AutoPtr<IView> vMIn;
    IView::Probe(mMonthSpinner)->FindViewById(R::id::numberpicker_input, (IView**)&vMIn);
    mMonthSpinnerInput = IEditText::Probe(vMIn);

    // year
    AutoPtr<IView> v;
    IView::Probe(mDelegator)->FindViewById(R::id::year, (IView**)&v);
    mYearSpinner = INumberPicker::Probe(v);
    mYearSpinner->SetOnLongPressUpdateInterval(100);
    mYearSpinner->SetOnValueChangedListener(onChangeListener);
    AutoPtr<IView> vYIn;
    IView::Probe(mYearSpinner)->FindViewById(R::id::numberpicker_input, (IView**)&vYIn);
    mYearSpinnerInput = IEditText::Probe(vYIn);

    // show only what the user required but make sure we
    // show something and the spinners have higher priority
    if (!spinnersShown && !calendarViewShown) {
        SetSpinnersShown(TRUE);
    }
    else {
        SetSpinnersShown(spinnersShown);
        SetCalendarViewShown(calendarViewShown);
    }

    // set the min date giving priority of the minDate over startYear
    mTempDate->Clear();
    if (!TextUtils::IsEmpty(minDate)) {
        if (!ParseDate(minDate, mTempDate)) {
            mTempDate->Set(startYear, 0, 1);
        }
    }
    else {
        mTempDate->Set(startYear, 0, 1);
    }
    Int64 minMils = 0;
    mTempDate->GetTimeInMillis(&minMils);
    SetMinDate(minMils);

    // set the max date giving priority of the maxDate over endYear
    mTempDate->Clear();
    if (!TextUtils::IsEmpty(maxDate)) {
        if (!ParseDate(maxDate, mTempDate)) {
            mTempDate->Set(endYear, 11, 31);
        }
    }
    else {
        mTempDate->Set(endYear, 11, 31);
    }
    Int64 maxMils = 0;
    mTempDate->GetTimeInMillis(&maxMils);
    SetMaxDate(maxMils);

    // initialize to current date
    AutoPtr<ISystem> sys;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 millis = 0;
    sys->GetCurrentTimeMillis(&millis);
    mCurrentDate->SetTimeInMillis(millis);
    Int32 y = 0, m = 0, dm = 0;
    mCurrentDate->Get(ICalendar::YEAR, &y);
    mCurrentDate->Get(ICalendar::MONTH, &m);
    mCurrentDate->Get(ICalendar::DAY_OF_MONTH, &dm);
    Init(y, m, dm, NULL);

    // re-order the number spinners to match the current date format
    ReorderSpinners();

    // accessibility
    SetContentDescriptions();

    // If not explicitly specified this view is important for accessibility.
    Int32 acc = 0;
    IView::Probe(mDelegator)->GetImportantForAccessibility(&acc);
    if (acc == IMPORTANT_FOR_ACCESSIBILITY_AUTO) {
        IView::Probe(mDelegator)->SetImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_YES);
    }
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::Init(
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ IDatePickerOnDateChangedListener* onDateChangedListener)
{
    SetDate(year, monthOfYear, dayOfMonth);
    UpdateSpinners();
    UpdateCalendarView();
    mOnDateChangedListener = onDateChangedListener;
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::UpdateDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth)
{
    if(!IsNewDate(year, month, dayOfMonth)){
        return NOERROR;
    }
    SetDate(year, month, dayOfMonth);
    UpdateSpinners();
    UpdateCalendarView();
    NotifyDateChanged();
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::GetYear(
    /* [out] */ Int32* year)
{
    VALIDATE_NOT_NULL(year)
    return mCurrentDate->Get(ICalendar::YEAR, year);
}

ECode DatePicker::DatePickerSpinnerDelegate::GetMonth(
    /* [out] */ Int32* month)
{
    VALIDATE_NOT_NULL(month)
    return mCurrentDate->Get(ICalendar::MONTH, month);
}

ECode DatePicker::DatePickerSpinnerDelegate::GetDayOfMonth(
    /* [out] */ Int32* dayOfMonth)
{
    VALIDATE_NOT_NULL(dayOfMonth)
    return mCurrentDate->Get(ICalendar::DAY_OF_MONTH, dayOfMonth);
}

ECode DatePicker::DatePickerSpinnerDelegate::SetFirstDayOfWeek(
    /* [in] */ Int32 firstDayOfWeek)
{
    return mCalendarView->SetFirstDayOfWeek(firstDayOfWeek);
}

ECode DatePicker::DatePickerSpinnerDelegate::GetFirstDayOfWeek(
    /* [out] */ Int32* firstDayOfWeek)
{
    VALIDATE_NOT_NULL(firstDayOfWeek)
    return mCalendarView->GetFirstDayOfWeek(firstDayOfWeek);
}

ECode DatePicker::DatePickerSpinnerDelegate::SetMinDate(
    /* [in] */ Int64 minDate)
{
    mTempDate->SetTimeInMillis(minDate);

    Int32 tempDateYear = 0, minDateYear = 0, tempDateDay = 0, minDateDay = 0;
    mTempDate->Get(ICalendar::YEAR, &tempDateYear);
    mTempDate->Get(ICalendar::DAY_OF_YEAR, &tempDateDay);
    mMinDate->Get(ICalendar::YEAR, &minDateYear);
    mMinDate->Get(ICalendar::DAY_OF_YEAR, &minDateDay);
    if((tempDateYear == minDateYear) && (tempDateDay != minDateDay)) {
        return NOERROR;
    }

    mMinDate->SetTimeInMillis(minDate);
    mCalendarView->SetMinDate(minDate);

    Boolean result = FALSE;
    mCurrentDate->IsBefore(mMinDate, &result);
    if(result) {
        Int64 t = 0;
        mMinDate->GetTimeInMillis(&t);
        mCurrentDate->SetTimeInMillis(t);
        UpdateCalendarView();
    }
    UpdateSpinners();
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::GetMinDate(
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar)
    Int64 mD = 0;
    mCalendarView->GetMinDate(&mD);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> minDate;
    helper->GetInstance((ICalendar**)&minDate);
    minDate->SetTimeInMillis(mD);
    *calendar = minDate;
    REFCOUNT_ADD(*calendar)
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::SetMaxDate(
    /* [in] */ Int64 maxDate)
{
    mTempDate->SetTimeInMillis(maxDate);

    Int32 tempDateYear = 0, maxDateYear = 0, tempDateDay = 0, maxDateDay = 0;
    mTempDate->Get(ICalendar::YEAR, &tempDateYear);
    mTempDate->Get(ICalendar::DAY_OF_YEAR, &tempDateDay);
    mMaxDate->Get(ICalendar::YEAR, &maxDateYear);
    mMaxDate->Get(ICalendar::DAY_OF_YEAR, &maxDateDay);
    if((tempDateYear == maxDateYear) && (tempDateDay != maxDateDay)) {
        return NOERROR;
    }
    mMaxDate->SetTimeInMillis(maxDate);
    mCalendarView->SetMaxDate(maxDate);

    Boolean result = FALSE;
    mCurrentDate->IsAfter(mMaxDate, &result);
    if(result) {
        Int64 t = 0;
        mMaxDate->GetTimeInMillis(&t);
        mCurrentDate->SetTimeInMillis(t);
        UpdateCalendarView();
    }
    UpdateSpinners();
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::GetMaxDate(
    /* [out] */ ICalendar** maxDate)
{
    VALIDATE_NOT_NULL(maxDate)
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> _maxDate;
    helper->GetInstance((ICalendar**)&_maxDate);
    Int64 maxD = 0;
    mCalendarView->GetMaxDate(&maxD);
    _maxDate->SetTimeInMillis(maxD);
    *maxDate = _maxDate;
    REFCOUNT_ADD(*maxDate)
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::SetEnabled(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mDaySpinner)->SetEnabled(enabled);
    IView::Probe(mMonthSpinner)->SetEnabled(enabled);
    IView::Probe(mYearSpinner)->SetEnabled(enabled);
    IView::Probe(mCalendarView)->SetEnabled(enabled);
    mIsEnabled = enabled;
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = mIsEnabled;
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::GetCalendarView (
    /* [out] */ ICalendarView** view)
{
    VALIDATE_NOT_NULL(view)
    *view = mCalendarView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::SetCalendarViewShown(
    /* [in] */ Boolean shown)
{
    IView::Probe(mCalendarView)->SetVisibility(shown ? VISIBLE : GONE);
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::GetCalendarViewShown(
    /* [out] */ Boolean* shown)
{
    VALIDATE_NOT_NULL(shown)
    Int32 vis = 0;
    IView::Probe(mCalendarView)->GetVisibility(&vis);
    *shown = (vis == IView::VISIBLE);
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::SetSpinnersShown(
    /* [in] */ Boolean shown)
{
    return IView::Probe(mSpinners)->SetVisibility(shown ? VISIBLE : GONE);
}

ECode DatePicker::DatePickerSpinnerDelegate::GetSpinnersShown(
    /* [out] */ Boolean* shown)
{
    VALIDATE_NOT_NULL(shown)
    return IView::Probe(mSpinners)->IsShown(shown);
}

ECode DatePicker::DatePickerSpinnerDelegate::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<CConfiguration> cc = (CConfiguration*)newConfig;
    return SetCurrentLocale(cc->mLocale);
}

ECode DatePicker::DatePickerSpinnerDelegate::OnSaveInstanceState(
    /* [in] */ IParcelable* superState,
    /* [out] */ IParcelable** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 y = 0, m = 0, dm = 0;
    GetYear(&y);
    GetMonth(&m);
    GetDayOfMonth(&dm);
    AutoPtr<DatePickerSavedState> ss = new DatePickerSavedState(superState, y, m, dm);
    *result = IParcelable::Probe(ss);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<DatePickerSavedState> ss = (DatePickerSavedState*)IViewBaseSavedState::Probe(state);
    SetDate(ss->mYear, ss->mMonth, ss->mDay);
    UpdateSpinners();
    UpdateCalendarView();
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    OnPopulateAccessibilityEvent(event);
    *rst = TRUE;
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    const Int32 flags = IDateUtils::FORMAT_SHOW_TIME | IDateUtils::FORMAT_SHOW_YEAR;
    Int64 timeInMillis = 0;
    mCurrentDate->GetTimeInMillis(&timeInMillis);
    String selectedDateUtterance = DateUtils::FormatDateTime(mContext, timeInMillis, flags);
    AutoPtr<IList> container;
    IAccessibilityRecord::Probe(event)->GetText((IList**)&container);
    container->Add(CoreUtils::Convert(selectedDateUtterance));
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert("CDatePicker"));
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    info->SetClassName(CoreUtils::Convert("CDatePicker"));
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    AbstractDatePickerDelegate::SetCurrentLocale(locale);

    mTempDate = GetCalendarForLocale(mTempDate, locale);
    mMinDate = GetCalendarForLocale(mMinDate, locale);
    mMaxDate = GetCalendarForLocale(mMaxDate, locale);
    mCurrentDate = GetCalendarForLocale(mCurrentDate, locale);

    mTempDate->GetActualMaximum(ICalendar::MONTH, &mNumberOfMonths);
    mNumberOfMonths += 1;
    AutoPtr<IDateFormatSymbols> sym;
    CDateFormatSymbols::New((IDateFormatSymbols**)&sym);
    sym->GetShortMonths((ArrayOf<String>**)&mShortMonths);

    if (UsingNumericMonths()) {
        // We're in a locale where a date should either be all-numeric, or all-text.
        // All-text would require custom NumberPicker formatters for day and year.
        mShortMonths = ArrayOf<String>::Alloc(mNumberOfMonths);
        for(Int32 i = 0; i < mNumberOfMonths; i++) {
            AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
            arr->Set(0, CoreUtils::Convert(i + 1));

            (*mShortMonths)[i] = StringUtils::Format(String("%d"), arr);
        }
    }
    return NOERROR;
}

Boolean DatePicker::DatePickerSpinnerDelegate::UsingNumericMonths()
{
    return Character::IsDigit(((*mShortMonths)[ICalendar::JANUARY]).GetChar(0));
}

AutoPtr<ICalendar> DatePicker::DatePickerSpinnerDelegate::GetCalendarForLocale(
    /* [in] */ ICalendar* oldCalendar,
    /* [in] */ ILocale* locale)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    if(oldCalendar == NULL) {
        AutoPtr<ICalendar> calendar;
        helper->GetInstance(locale, (ICalendar**)&calendar);
        return calendar;
    }
    else {
        Int64 currentTimeMillis = 0;
        oldCalendar->GetTimeInMillis(&currentTimeMillis);
        AutoPtr<ICalendar> newCalendar;
        helper->GetInstance(locale, (ICalendar**)&newCalendar);
        newCalendar->SetTimeInMillis(currentTimeMillis);
        return newCalendar;
    }
}

ECode DatePicker::DatePickerSpinnerDelegate::ReorderSpinners()
{
    IViewGroup::Probe(mSpinners)->RemoveAllViews();
    // We use numeric spinners for year and day, but textual months. Ask icu4c what
    // order the user's locale uses for that combination. http://b/7207103.
    using Elastos::Droid::Text::Format::IDateFormat;
    AutoPtr<IDateFormat> df;
    CDateFormat::AcquireSingleton((IDateFormat**)&df);
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> loc;
    hlp->GetDefault((ILocale**)&loc);
    String pattern;
    df->GetBestDateTimePattern(loc, String("yyyyMMMdd"), &pattern);
    AutoPtr<IICUUtil> icuHelper;
    CICUUtil::AcquireSingleton((IICUUtil**)&icuHelper);
    AutoPtr<ArrayOf<Char32> > order;
    icuHelper->GetDateFormatOrder(pattern, (ArrayOf<Char32>**)&order);
    const Int32 spinnerCount = order->GetLength();
    for (Int32 i = 0; i < spinnerCount; i++) {
        switch ((Int32)((*order)[i])) {
            case 'd':
                IViewGroup::Probe(mSpinners)->AddView(IView::Probe(mDaySpinner));
                SetImeOptions(mDaySpinner, spinnerCount, i);
                break;
            case 'M':
                IViewGroup::Probe(mSpinners)->AddView(IView::Probe(mMonthSpinner));
                SetImeOptions(mMonthSpinner, spinnerCount, i);
                break;
            case 'y':
                IViewGroup::Probe(mSpinners)->AddView(IView::Probe(mYearSpinner));
                SetImeOptions(mYearSpinner, spinnerCount, i);
                break;
            default:
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
                break;
        }
    }
    return NOERROR;
}

Boolean DatePicker::DatePickerSpinnerDelegate::ParseDate(
    /* [in] */ const String& date,
    /* [in] */ ICalendar* outDate)
{
    AutoPtr<IDate> toDate;
    mDateFormat->Parse(date, (IDate**)&toDate);
    outDate->SetTime(toDate);
    return TRUE;
}

Boolean DatePicker::DatePickerSpinnerDelegate::IsNewDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth)
{
    Int32 newYear = 0, newMonth = 0, newDayOfMonth = 0;
    mCurrentDate->Get(ICalendar::YEAR, &newYear);
    mCurrentDate->Get(ICalendar::MONTH, &newMonth);
    mCurrentDate->Get(ICalendar::DAY_OF_MONTH, &newDayOfMonth);
    return (newYear != year || newMonth != month || newDayOfMonth != dayOfMonth);
}

ECode DatePicker::DatePickerSpinnerDelegate::SetDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth)
{
    mCurrentDate->Set(year, month, dayOfMonth);
    Boolean beforeMin = FALSE, afterMax = FALSE;
    mCurrentDate->IsBefore(mMinDate, &beforeMin);
    mCurrentDate->IsAfter(mMaxDate, &afterMax);
    if (beforeMin) {
        Int64 min = 0;
        mMinDate->GetTimeInMillis(&min);
        mCurrentDate->SetTimeInMillis(min);
    }
    else if (afterMax) {
        Int64 max = 0;
        mMaxDate->GetTimeInMillis(&max);
        mCurrentDate->SetTimeInMillis(max);
    }
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::UpdateSpinners()
{
    // set the spinner ranges respecting the min and max dates
    Boolean min = FALSE, max = FALSE;
    mCurrentDate->Equals(mMinDate, &min);
    mCurrentDate->Equals(mMaxDate, &max);
    Int32 minDayValue = 0, maxDayValue = 0, minMonthValue = 0, maxMonthValue = 0;
    if (min) {
        mCurrentDate->Get(ICalendar::DAY_OF_MONTH, &minDayValue);
        mCurrentDate->GetActualMaximum(ICalendar::DAY_OF_MONTH, &maxDayValue);
        mDaySpinner->SetMinValue(minDayValue);
        mDaySpinner->SetMaxValue(maxDayValue);
        mDaySpinner->SetWrapSelectorWheel(FALSE);
        mMonthSpinner->SetDisplayedValues(NULL);
        mCurrentDate->Get(ICalendar::MONTH, &minMonthValue);
        mCurrentDate->GetActualMaximum(ICalendar::MONTH, &maxMonthValue);
        mMonthSpinner->SetMinValue(minMonthValue);
        mMonthSpinner->SetMaxValue(maxMonthValue);
        mMonthSpinner->SetWrapSelectorWheel(FALSE);
    }
    else if (max) {
        mCurrentDate->GetActualMinimum(ICalendar::DAY_OF_MONTH, &minDayValue);
        mCurrentDate->Get(ICalendar::DAY_OF_MONTH, &maxDayValue);
        mDaySpinner->SetMinValue(minDayValue);
        mDaySpinner->SetMaxValue(maxDayValue);
        mDaySpinner->SetWrapSelectorWheel(FALSE);
        mMonthSpinner->SetDisplayedValues(NULL);
        mCurrentDate->GetActualMinimum(ICalendar::MONTH, &minMonthValue);
        mCurrentDate->Get(ICalendar::MONTH, &maxMonthValue);
        mMonthSpinner->SetMinValue(minMonthValue);
        mMonthSpinner->SetMaxValue(maxMonthValue);
        mMonthSpinner->SetWrapSelectorWheel(FALSE);
    }
    else {
        mDaySpinner->SetMaxValue(1);
        mCurrentDate->GetActualMaximum(ICalendar::DAY_OF_MONTH, &maxDayValue);
        mDaySpinner->SetMaxValue(maxDayValue);
        mDaySpinner->SetWrapSelectorWheel(TRUE);
        mMonthSpinner->SetDisplayedValues(NULL);
        mMonthSpinner->SetMinValue(0);
        mMonthSpinner->SetMaxValue(11);
        mMonthSpinner->SetWrapSelectorWheel(TRUE);
    }

    // make sure the month names are a zero based array
    // with the months in the month spinner
    Int32 start = 0, end = 0;
    mMonthSpinner->GetMinValue(&start);
    mMonthSpinner->GetMaxValue(&end);
    end += 1;
    AutoPtr<ArrayOf<String> > displayedVls;
    Arrays::CopyOfRange(mShortMonths, start, end, (ArrayOf<String>**)&displayedVls);
    mMonthSpinner->SetDisplayedValues(displayedVls);

    // year spinner range does not change based on the current date
    Int32 y = 0;
    mMinDate->Get(ICalendar::YEAR, &y);
    mYearSpinner->SetMinValue(y);
    Int32 maxY = 0;
    mMaxDate->Get(ICalendar::YEAR, &maxY);
    mYearSpinner->SetMaxValue(maxY);
    mYearSpinner->SetWrapSelectorWheel(FALSE);

    // set the spinner values
    Int32 yearValue = 0, monthValue = 0, dayVelue = 0;
    mCurrentDate->Get(ICalendar::YEAR, &yearValue);
    mCurrentDate->Get(ICalendar::MONTH, &monthValue);
    mCurrentDate->Get(ICalendar::DAY_OF_MONTH, &dayVelue);
    mYearSpinner->SetValue(yearValue);
    mMonthSpinner->SetValue(monthValue);
    mDaySpinner->SetValue(dayVelue);

    if (UsingNumericMonths()) {
        ITextView::Probe(mMonthSpinnerInput)->SetRawInputType(IInputType::TYPE_CLASS_NUMBER);
    }

    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::UpdateCalendarView()
{
    Int64 timeInMillis = 0;
    mCurrentDate->GetTimeInMillis(&timeInMillis);
    mCalendarView->SetDate(timeInMillis, FALSE, FALSE);
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::NotifyDateChanged()
{
    IView::Probe(mDelegator)->SendAccessibilityEvent(IAccessibilityEvent::TYPE_VIEW_SELECTED);
    if(mOnDateChangedListener != NULL) {
        Int32 y = 0, m = 0, dm = 0;
        GetYear(&y);
        GetMonth(&m);
        GetDayOfMonth(&dm);
        mOnDateChangedListener->OnDateChanged(mDelegator, y, m, dm);
    }

    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::SetImeOptions(
    /* [in] */ INumberPicker* spinner,
    /* [in] */ Int32 spinnerCount,
    /* [in] */ Int32 spinnerIndex)
{
    Int32 imeOptions = 0;
    if(spinnerIndex < spinnerCount - 1) {
        imeOptions = IEditorInfo::IME_ACTION_NEXT;
    }
    else {
        imeOptions = IEditorInfo::IME_ACTION_DONE;
    }
    AutoPtr<IView> view;
    IView::Probe(spinner)->FindViewById(R::id::numberpicker_input, (IView**)&view);
    AutoPtr<ITextView> input = ITextView::Probe(view);
    if(input != NULL) {
        input->SetImeOptions(imeOptions);
    }
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::SetContentDescriptions()
{
    // Day
    AutoPtr<IView> view = IView::Probe(mDaySpinner);
    TrySetContentDescription(view, R::id::increment, R::string::date_picker_increment_day_button);
    TrySetContentDescription(view, R::id::decrement, R::string::date_picker_decrement_day_button);

    // Month
    AutoPtr<IView> view1 = IView::Probe(mMonthSpinner);
    TrySetContentDescription(view1, R::id::increment, R::string::date_picker_increment_month_button);
    TrySetContentDescription(view1, R::id::decrement, R::string::date_picker_decrement_month_button);

    // Year
    AutoPtr<IView> view2 = IView::Probe(mYearSpinner);
    TrySetContentDescription(view2, R::id::increment, R::string::date_picker_increment_year_button);
    TrySetContentDescription(view2, R::id::decrement, R::string::date_picker_decrement_year_button);

    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::TrySetContentDescription(
    /* [in] */ IView* root,
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 contDescResId)
{
    AutoPtr<IView> target;
    root->FindViewById(viewId, (IView**)&target);
    if(target != NULL) {
        String res;
        mContext->GetString(contDescResId, &res);
        target->SetContentDescription(CoreUtils::Convert(res));
    }
    return NOERROR;
}

ECode DatePicker::DatePickerSpinnerDelegate::UpdateInputState()
{
    // Make sure that if the user changes the value and the IME is active
    // for one of the inputs if this widget, the IME is closed. If the user
    // changed the value via the IME and there is a next input the IME will
    // be shown, otherwise the user chose another means of changing the
    // value and having the IME up makes no sense.
    AutoPtr<IInputMethodManager> inputMethodManager = CInputMethodManager::PeekInstance();
    if (inputMethodManager != NULL) {
        Boolean yearActive = FALSE, monthActive = FALSE, dayActive = FALSE;
        inputMethodManager->IsActive(IView::Probe(mYearSpinnerInput), &yearActive);
        inputMethodManager->IsActive(IView::Probe(mMonthSpinnerInput), &monthActive);
        inputMethodManager->IsActive(IView::Probe(mDaySpinnerInput), &dayActive);
        AutoPtr<IBinder> bind;
        IView::Probe(mDelegator)->GetWindowToken((IBinder**)&bind);
        Boolean res = FALSE;
        if (yearActive) {
            IView::Probe(mYearSpinnerInput)->ClearFocus();
            inputMethodManager->HideSoftInputFromWindow(bind, 0, &res);
        }
        else if (monthActive) {
            IView::Probe(mMonthSpinnerInput)->ClearFocus();
            inputMethodManager->HideSoftInputFromWindow(bind, 0, &res);
        }
        else if (dayActive) {
            IView::Probe(mDaySpinnerInput)->ClearFocus();
            inputMethodManager->HideSoftInputFromWindow(bind, 0, &res);
        }
    }
    return NOERROR;
}

//===============================================================
// DatePicker::
//===============================================================
CAR_INTERFACE_IMPL(DatePicker, FrameLayout, IDatePicker)

DatePicker::DatePicker()
{
}

ECode DatePicker::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode DatePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::datePickerStyle);
}

ECode DatePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode DatePicker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::DatePicker),
            ArraySize(R::styleable::DatePicker));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    Int32 mode = 0;
    a->GetInt32(R::styleable::DatePicker_datePickerMode, MODE_SPINNER, &mode);
    Int32 firstDayOfWeek = 0;
    a->GetInt32(R::styleable::DatePicker_firstDayOfWeek, 0, &firstDayOfWeek);
    a->Recycle();

    switch (mode) {
        case MODE_CALENDAR:
            mDelegate = CreateCalendarUIDelegate(context, attrs, defStyleAttr, defStyleRes);
            break;
        case MODE_SPINNER:
        default:
            mDelegate = CreateSpinnerUIDelegate(context, attrs, defStyleAttr, defStyleRes);
            break;
    }

    if (firstDayOfWeek != 0) {
        SetFirstDayOfWeek(firstDayOfWeek);
    }
    return NOERROR;
}

AutoPtr<IDatePickerDelegate> DatePicker::CreateSpinnerUIDelegate(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AutoPtr<DatePickerSpinnerDelegate> res = new DatePickerSpinnerDelegate();
    res->constructor(this, context, attrs, defStyleAttr, defStyleRes);
    return IDatePickerDelegate::Probe(res);
}

AutoPtr<IDatePickerDelegate> DatePicker::CreateCalendarUIDelegate(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AutoPtr<DatePickerCalendarDelegate> res = new DatePickerCalendarDelegate();
    res->constructor(this, context, attrs, defStyleAttr, defStyleRes);
    return IDatePickerDelegate::Probe(res);
}

ECode DatePicker::Init(
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ IDatePickerOnDateChangedListener* onDateChangedListener)
{
    mDelegate->Init(year, monthOfYear, dayOfMonth, onDateChangedListener);
    return NOERROR;
}

ECode DatePicker::UpdateDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth)
{
    mDelegate->UpdateDate(year, month, dayOfMonth);
    return NOERROR;
}

ECode DatePicker::GetYear(
    /* [out] */ Int32* year)
{
    VALIDATE_NOT_NULL(year)
    return mDelegate->GetYear(year);
}

ECode DatePicker::GetMonth(
    /* [out] */ Int32* month)
{
    VALIDATE_NOT_NULL(month)
    return mDelegate->GetMonth(month);
}

ECode DatePicker::GetDayOfMonth(
    /* [out] */ Int32* dayOfMonth)
{
    VALIDATE_NOT_NULL(dayOfMonth)
    return mDelegate->GetDayOfMonth(dayOfMonth);
}

ECode DatePicker::GetMinDate(
    /* [out] */ Int64* calendar)
{
    VALIDATE_NOT_NULL(calendar)
    AutoPtr<ICalendar> cl;
    mDelegate->GetMinDate((ICalendar**)&cl);
    return cl->GetTimeInMillis(calendar);
}

ECode DatePicker::SetMinDate(
    /* [in] */ Int64 minDate)
{
    mDelegate->SetMinDate(minDate);
    return NOERROR;
}

ECode DatePicker::GetMaxDate(
    /* [out] */ Int64* maxDate)
{
    VALIDATE_NOT_NULL(maxDate)
    AutoPtr<ICalendar> cl;
    mDelegate->GetMaxDate((ICalendar**)&cl);
    return cl->GetTimeInMillis(maxDate);
}

ECode DatePicker::SetMaxDate(
    /* [in] */ Int64 maxDate)
{
    return mDelegate->SetMaxDate(maxDate);
}

ECode DatePicker::SetValidationCallback(
    /* [in] */ IDatePickerValidationCallback* callback)
{
    return mDelegate->SetValidationCallback(callback);
}

ECode DatePicker::SetEnabled(
    /* [in] */ Boolean enabled)
{
    Boolean isEbl = FALSE;
    mDelegate->IsEnabled(&isEbl);
    if (isEbl == enabled) {
        return NOERROR;
    }
    FrameLayout::SetEnabled(enabled);
    mDelegate->SetEnabled(enabled);
    return NOERROR;
}

ECode DatePicker::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    return mDelegate->IsEnabled(enabled);
}

ECode DatePicker::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    return mDelegate->DispatchPopulateAccessibilityEvent(event, rst);
}

ECode DatePicker::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnPopulateAccessibilityEvent(event);
    return mDelegate->OnPopulateAccessibilityEvent(event);
}

ECode DatePicker::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    return mDelegate->OnInitializeAccessibilityEvent(event);
}

ECode DatePicker::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    return mDelegate->OnInitializeAccessibilityNodeInfo(info);
}

void DatePicker::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);
    mDelegate->OnConfigurationChanged(newConfig);
}

ECode DatePicker::SetFirstDayOfWeek(
    /* [in] */ Int32 firstDayOfWeek)
{
    if (firstDayOfWeek < ICalendar::SUNDAY || firstDayOfWeek > ICalendar::SATURDAY) {
        // throw new IllegalArgumentException("firstDayOfWeek must be between 1 and 7");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mDelegate->SetFirstDayOfWeek(firstDayOfWeek);
}

ECode DatePicker::GetFirstDayOfWeek(
    /* [out] */ Int32* firstDayOfWeek)
{
    VALIDATE_NOT_NULL(firstDayOfWeek)
    return mDelegate->GetFirstDayOfWeek(firstDayOfWeek);
}

ECode DatePicker::GetCalendarViewShown(
    /* [out] */ Boolean* shown)
{
    VALIDATE_NOT_NULL(shown)
    return mDelegate->GetCalendarViewShown(shown);
}

ECode DatePicker::GetCalendarView(
    /* [out] */ ICalendarView** view)
{
    VALIDATE_NOT_NULL(view)
    return mDelegate->GetCalendarView(view);
}

ECode DatePicker::SetCalendarViewShown(
    /* [in] */ Boolean shown)
{
    return mDelegate->SetCalendarViewShown(shown);
}

ECode DatePicker::GetSpinnersShown(
    /* [out] */ Boolean* shown)
{
    VALIDATE_NOT_NULL(shown)
    return mDelegate->GetSpinnersShown(shown);
}

ECode DatePicker::SetSpinnersShown(
    /* [in] */ Boolean shown)
{
    return mDelegate->SetSpinnersShown(shown);
}

ECode DatePicker::DispatchRestoreInstanceState(
    /* [in] */ ISparseArray* container)
{
    return DispatchThawSelfOnly(container);
}

AutoPtr<IParcelable> DatePicker::OnSaveInstanceState()
{
    AutoPtr<IParcelable> superState = FrameLayout::OnSaveInstanceState();
    AutoPtr<IParcelable> res;
    mDelegate->OnSaveInstanceState(superState, (IParcelable**)&res);
    return res;
}

void DatePicker::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<BaseSavedState> ss = (BaseSavedState*) state;
    AutoPtr<IParcelable> pcl;
    ss->GetSuperState((IParcelable**)&pcl);
    FrameLayout::OnRestoreInstanceState(pcl);
    mDelegate->OnRestoreInstanceState(ss);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
