
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/widget/DatePickerCalendarDelegate.h"
#include "elastos/droid/widget/CDatePickerCalendarDelegateSavedState.h"
#include "elastos/droid/widget/CDayPickerView.h"
#include "elastos/droid/widget/CYearPickerView.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/view/animation/CAlphaAnimation.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Widget::CDayPickerView;
using Elastos::Droid::Widget::CYearPickerView;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IFormat;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Widget {


// ==================================================================
//                DatePickerCalendarDelegate::InnerListener
// ==================================================================
CAR_INTERFACE_IMPL(DatePickerCalendarDelegate::InnerListener, Object, IViewOnClickListener)

DatePickerCalendarDelegate::InnerListener::InnerListener(
    /* [in] */ DatePickerCalendarDelegate* host)
    : mHost(host)
{}

ECode DatePickerCalendarDelegate::InnerListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->OnClick(v);
}

// ==================================================================
//                DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::
// ==================================================================

CAR_INTERFACE_IMPL(DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState, View::BaseSavedState, IDatePickerCalendarDelegateSavedState)

DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::DatePickerCalendarDelegateSavedState()
    : mSelectedYear(0)
    , mSelectedMonth(0)
    , mSelectedDay(0)
    , mMinDate(0)
    , mMaxDate(0)
    , mCurrentView(0)
    , mListPosition(0)
    , mListPositionOffset(0)
{}

DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::~DatePickerCalendarDelegateSavedState()
{}

ECode DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::constructor(
    /* [in] */ IParcelable* superState,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day,
    /* [in] */ Int64 minDate,
    /* [in] */ Int64 maxDate,
    /* [in] */ Int32 currentView,
    /* [in] */ Int32 listPosition,
    /* [in] */ Int32 listPositionOffset)
{
    Elastos::Droid::View::View::BaseSavedState::constructor(superState);
    mSelectedYear = year;
    mSelectedMonth = month;
    mSelectedDay = day;
    mMinDate = minDate;
    mMaxDate = maxDate;
    mCurrentView = currentView;
    mListPosition = listPosition;
    mListPositionOffset = listPositionOffset;
    return NOERROR;
}

ECode DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::constructor()
{
    return Elastos::Droid::View::View::BaseSavedState::constructor();
}

ECode DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Elastos::Droid::View::View::BaseSavedState::ReadFromParcel(source);
    source->ReadInt32(&mSelectedYear);
    source->ReadInt32(&mSelectedMonth);
    source->ReadInt32(&mSelectedDay);
    source->ReadInt64(&mMinDate);
    source->ReadInt64(&mMaxDate);
    source->ReadInt32(&mCurrentView);
    source->ReadInt32(&mListPosition);
    source->ReadInt32(&mListPositionOffset);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Elastos::Droid::View::View::BaseSavedState::WriteToParcel(dest);
    dest->WriteInt32(mSelectedYear);
    dest->WriteInt32(mSelectedMonth);
    dest->WriteInt32(mSelectedDay);
    dest->WriteInt64(mMinDate);
    dest->WriteInt64(mMaxDate);
    dest->WriteInt32(mCurrentView);
    dest->WriteInt32(mListPosition);
    dest->WriteInt32(mListPositionOffset);
    return NOERROR;
}

Int32 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetSelectedDay()
{
    return mSelectedDay;
}

Int32 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetSelectedMonth()
{
    return mSelectedMonth;
}

Int32 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetSelectedYear()
{
    return mSelectedYear;
}

Int64 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetMinDate()
{
    return mMinDate;
}

Int64 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetMaxDate()
{
    return mMaxDate;
}

Int32 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetCurrentView()
{
    return mCurrentView;
}

Int32 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetListPosition()
{
    return mListPosition;
}

Int32 DatePickerCalendarDelegate::DatePickerCalendarDelegateSavedState::GetListPositionOffset()
{
    return mListPositionOffset;
}

// ==================================================================
//                DatePickerCalendarDelegate::
// ==================================================================
const Int32 DatePickerCalendarDelegate::USE_LOCALE = 0;

const Int32 DatePickerCalendarDelegate::UNINITIALIZED = -1;
const Int32 DatePickerCalendarDelegate::MONTH_AND_DAY_VIEW = 0;
const Int32 DatePickerCalendarDelegate::YEAR_VIEW = 1;

const Int32 DatePickerCalendarDelegate::DEFAULT_START_YEAR = 1900;
const Int32 DatePickerCalendarDelegate::DEFAULT_END_YEAR = 2100;

const Int32 DatePickerCalendarDelegate::ANIMATION_DURATION = 300;

const Int32 DatePickerCalendarDelegate::MONTH_INDEX = 0;
const Int32 DatePickerCalendarDelegate::DAY_INDEX = 1;
const Int32 DatePickerCalendarDelegate::YEAR_INDEX = 2;

CAR_INTERFACE_IMPL_2(DatePickerCalendarDelegate, DatePicker::AbstractDatePickerDelegate, IDatePickerCalendarDelegate, IDatePickerController)

DatePickerCalendarDelegate::DatePickerCalendarDelegate()
    : mIsEnabled(TRUE)
    , mCurrentView(UNINITIALIZED)
    , mFirstDayOfWeek(USE_LOCALE)
{
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> locale;
    hlp->GetDefault((ILocale**)&locale);
    CSimpleDateFormat::New(String("y"), locale, (ISimpleDateFormat**)&mYearFormat);
    CSimpleDateFormat::New(String("d"), locale, (ISimpleDateFormat**)&mDayFormat);

    CHashSet::New((IHashSet**)&mListeners);
}

DatePickerCalendarDelegate::~DatePickerCalendarDelegate()
{}

ECode DatePickerCalendarDelegate::constructor(
    /* [in] */ IDatePicker* delegator,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    DatePicker::AbstractDatePickerDelegate::constructor(delegator, context);

    AutoPtr<InnerListener> listener = new InnerListener(this);
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> locale;
    hlp->GetDefault((ILocale**)&locale);

    mMinDate = GetCalendarForLocale(mMinDate, locale);
    mMaxDate = GetCalendarForLocale(mMaxDate, locale);
    mTempDate = GetCalendarForLocale(mMaxDate, locale);
    mCurrentDate = GetCalendarForLocale(mCurrentDate, locale);

    mMinDate->Set(DEFAULT_START_YEAR, ICalendar::JANUARY, 1);
    mMaxDate->Set(DEFAULT_END_YEAR, ICalendar::DECEMBER, 31);

    AutoPtr<IResources> res;
    IView::Probe(mDelegator)->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::DatePicker);
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(attrs,
            attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    AutoPtr<IInterface> pInf;
    mContext->GetSystemService(
            IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&pInf);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(pInf);
    Int32 layoutResourceId = 0;
    a->GetResourceId(
            R::styleable::DatePicker_internalLayout, R::layout::date_picker_holo, &layoutResourceId);
    AutoPtr<IView> mainView;
    inflater->Inflate(layoutResourceId, NULL, (IView**)&mainView);
    IViewGroup::Probe(mDelegator)->AddView(mainView);

    AutoPtr<IView> dwView;
    mainView->FindViewById(R::id::date_picker_header, (IView**)&dwView);
    mDayOfWeekView = ITextView::Probe(dwView);

    // Layout that contains the current date and day name header.
    AutoPtr<IView> v;
    mainView->FindViewById(
            R::id::day_picker_selector_layout, (IView**)&v);
    AutoPtr<ILinearLayout> dateLayout = ILinearLayout::Probe(v);
    v = NULL;
    mainView->FindViewById(
            R::id::date_picker_month_day_year_layout, (IView**)&v);
    mMonthDayYearLayout = ILinearLayout::Probe(v);
    v = NULL;
    mainView->FindViewById(
            R::id::date_picker_month_and_day_layout, (IView**)&v);
    mMonthAndDayLayout = ILinearLayout::Probe(v);
    v->SetOnClickListener(listener);
    v = NULL;
    mainView->FindViewById(R::id::date_picker_month, (IView**)&v);
    mHeaderMonthTextView = ITextView::Probe(v);
    v = NULL;
    mainView->FindViewById(R::id::date_picker_day, (IView**)&v);
    mHeaderDayOfMonthTextView = ITextView::Probe(v);
    v = NULL;
    mainView->FindViewById(R::id::date_picker_year, (IView**)&v);
    mHeaderYearTextView = ITextView::Probe(v);
    v->SetOnClickListener(listener);

    // Obtain default highlight color from the theme.
    Int32 defaultHighlightColor = 0;
    mHeaderYearTextView->GetHighlightColor(&defaultHighlightColor);

    // Use Theme attributes if possible
    Int32 dayOfWeekTextAppearanceResId = 0;
    a->GetResourceId(
            R::styleable::DatePicker_dayOfWeekTextAppearance, -1, &dayOfWeekTextAppearanceResId);
    if (dayOfWeekTextAppearanceResId != -1) {
        mDayOfWeekView->SetTextAppearance(context, dayOfWeekTextAppearanceResId);
    }

    AutoPtr<IDrawable> d;
    a->GetDrawable(R::styleable::DatePicker_dayOfWeekBackground, (IDrawable**)&d);
    IView::Probe(mDayOfWeekView)->SetBackground(d);

    d = NULL;
    a->GetDrawable(R::styleable::DatePicker_headerBackground, (IDrawable**)&d);
    IView::Probe(dateLayout)->SetBackground(d);

    Int32 headerSelectedTextColor = 0;
    a->GetColor(
            R::styleable::DatePicker_headerSelectedTextColor, defaultHighlightColor, &headerSelectedTextColor);
    Int32 monthTextAppearanceResId = 0;
    a->GetResourceId(
            R::styleable::DatePicker_headerMonthTextAppearance, -1, &monthTextAppearanceResId);
    if (monthTextAppearanceResId != -1) {
        mHeaderMonthTextView->SetTextAppearance(context, monthTextAppearanceResId);
    }
    AutoPtr<IColorStateList> cl;
    mHeaderMonthTextView->GetTextColors((IColorStateList**)&cl);
    mHeaderMonthTextView->SetTextColor(CColorStateList::AddFirstIfMissing(
            cl, R::attr::state_selected, headerSelectedTextColor));

    Int32 dayOfMonthTextAppearanceResId = 0;
    a->GetResourceId(
            R::styleable::DatePicker_headerDayOfMonthTextAppearance, -1, &dayOfMonthTextAppearanceResId);
    if (dayOfMonthTextAppearanceResId != -1) {
        mHeaderDayOfMonthTextView->SetTextAppearance(context, dayOfMonthTextAppearanceResId);
    }
    cl = NULL;
    mHeaderDayOfMonthTextView->GetTextColors((IColorStateList**)&cl);
    mHeaderDayOfMonthTextView->SetTextColor(CColorStateList::AddFirstIfMissing(
            cl, R::attr::state_selected, headerSelectedTextColor));

    Int32 yearTextAppearanceResId = 0;
    a->GetResourceId(
            R::styleable::DatePicker_headerYearTextAppearance, -1, &yearTextAppearanceResId);
    if (yearTextAppearanceResId != -1) {
        mHeaderYearTextView->SetTextAppearance(context, yearTextAppearanceResId);
    }
    cl = NULL;
    mHeaderYearTextView->GetTextColors((IColorStateList**)&cl);
    mHeaderYearTextView->SetTextColor(CColorStateList::AddFirstIfMissing(
            cl, R::attr::state_selected, headerSelectedTextColor));

    CDayPickerView::New(mContext, this, (IDayPickerView**)&mDayPickerView);
    CYearPickerView::New(mContext, (IYearPickerView**)&mYearPickerView);
    mYearPickerView->Init(this);

    Int32 yearSelectedCircleColor = 0;
    a->GetColor(R::styleable::DatePicker_yearListSelectorColor,
            defaultHighlightColor, &yearSelectedCircleColor);
    mYearPickerView->SetYearSelectedCircleColor(yearSelectedCircleColor);

    AutoPtr<IColorStateList> calendarTextColor;
    a->GetColorStateList(
            R::styleable::DatePicker_calendarTextColor, (IColorStateList**)&calendarTextColor);
    Int32 calendarSelectedTextColor = 0;
    a->GetColor(
            R::styleable::DatePicker_calendarSelectedTextColor, defaultHighlightColor, &calendarSelectedTextColor);
    ((CDayPickerView*)mDayPickerView.Get())->SetCalendarTextColor(CColorStateList::AddFirstIfMissing(
            calendarTextColor, R::attr::state_selected, calendarSelectedTextColor));

    res->GetString(R::string::day_picker_description, &mDayPickerDescription);
    res->GetString(R::string::select_day, &mSelectDay);
    res->GetString(R::string::year_picker_description, &mYearPickerDescription);
    res->GetString(R::string::select_year, &mSelectYear);

    v = NULL;
    mainView->FindViewById(R::id::animator, (IView**)&v);
    mAnimator = IAccessibleDateAnimator::Probe(v);
    IViewGroup::Probe(mAnimator)->AddView(IView::Probe(mDayPickerView));
    IViewGroup::Probe(mAnimator)->AddView(IView::Probe(mYearPickerView));
    Int64 mils = 0;
    mCurrentDate->GetTimeInMillis(&mils);
    mAnimator->SetDateMillis(mils);

    AutoPtr<IAnimation> animation;
    CAlphaAnimation::New(0.0f, 1.0f, (IAnimation**)&animation);
    animation->SetDuration(ANIMATION_DURATION);
    IViewAnimator::Probe(mAnimator)->SetInAnimation(animation);

    AutoPtr<IAnimation> animation2;
    CAlphaAnimation::New(1.0f, 0.0f, (IAnimation**)&animation2);
    animation2->SetDuration(ANIMATION_DURATION);
    IViewAnimator::Probe(mAnimator)->SetOutAnimation(animation2);

    UpdateDisplay(FALSE);
    SetCurrentView(MONTH_AND_DAY_VIEW);
    return NOERROR;
}

AutoPtr<ICalendar> DatePickerCalendarDelegate::GetCalendarForLocale(
    /* [in] */ ICalendar* oldCalendar,
    /* [in] */ ILocale* locale)
{
    AutoPtr<ICalendarHelper> hlp;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&hlp);
    if (oldCalendar == NULL) {
        AutoPtr<ICalendar> c;
        hlp->GetInstance(locale, (ICalendar**)&c);
        return c;
    }
    else {
        Int64 currentTimeMillis = 0;
        oldCalendar->GetTimeInMillis(&currentTimeMillis);
        AutoPtr<ICalendar> newCalendar;
        hlp->GetInstance(locale, (ICalendar**)&newCalendar);
        newCalendar->SetTimeInMillis(currentTimeMillis);
        return newCalendar;
    }
}

AutoPtr<ArrayOf<Int32> > DatePickerCalendarDelegate::GetMonthDayYearIndexes(
    /* [in] */ const String& pattern)
{
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(3);

    String filteredPattern;
    StringUtils::ReplaceAll(pattern, "'.*?'", "", &filteredPattern);

    Int32 dayIndex = filteredPattern.IndexOf('d');
    Int32 monthMIndex = filteredPattern.IndexOf("M");
    Int32 monthIndex = (monthMIndex != -1) ? monthMIndex : filteredPattern.IndexOf("L");
    Int32 yearIndex = filteredPattern.IndexOf("y");

    if (yearIndex < monthIndex) {
        (*result)[YEAR_INDEX] = 0;

        if (monthIndex < dayIndex) {
            (*result)[MONTH_INDEX] = 1;
            (*result)[DAY_INDEX] = 2;
        }
        else {
            (*result)[MONTH_INDEX] = 2;
            (*result)[DAY_INDEX] = 1;
        }
    }
    else {
        (*result)[YEAR_INDEX] = 2;

        if (monthIndex < dayIndex) {
            (*result)[MONTH_INDEX] = 0;
            (*result)[DAY_INDEX] = 1;
        }
        else {
            (*result)[MONTH_INDEX] = 1;
            (*result)[DAY_INDEX] = 0;
        }
    }
    return result;
}

void DatePickerCalendarDelegate::UpdateDisplay(
    /* [in] */ Boolean announce)
{
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> locale;
    hlp->GetDefault((ILocale**)&locale);
    if (mDayOfWeekView != NULL) {
        String str;
        mCurrentDate->GetDisplayName(ICalendar::DAY_OF_WEEK, ICalendar::LONG,
                locale, &str);
        mDayOfWeekView->SetText(CoreUtils::Convert(str));
    }

    // Compute indices of Month, Day and Year views
    using Elastos::Droid::Text::Format::IDateFormat;
    AutoPtr<IDateFormat> df;
    CDateFormat::AcquireSingleton((IDateFormat**)&df);
    String bestDateTimePattern;
    df->GetBestDateTimePattern(mCurrentLocale, String("yMMMd"), &bestDateTimePattern);
    AutoPtr<ArrayOf<Int32> > viewIndices = GetMonthDayYearIndexes(bestDateTimePattern);

    // Position the Year and MonthAndDay views within the header.
    IViewGroup* mdyl = IViewGroup::Probe(mMonthDayYearLayout);
    mdyl->RemoveAllViews();
    if ((*viewIndices)[YEAR_INDEX] == 0) {
        mdyl->AddView(IView::Probe(mHeaderYearTextView));
        mdyl->AddView(IView::Probe(mMonthAndDayLayout));
    }
    else {
        mdyl->AddView(IView::Probe(mMonthAndDayLayout));
        mdyl->AddView(IView::Probe(mHeaderYearTextView));
    }

    // Position Day and Month views within the MonthAndDay view.
    IViewGroup* madl = IViewGroup::Probe(mMonthAndDayLayout);
    madl->RemoveAllViews();
    if ((*viewIndices)[MONTH_INDEX] > (*viewIndices)[DAY_INDEX]) {
        madl->AddView(IView::Probe(mHeaderDayOfMonthTextView));
        madl->AddView(IView::Probe(mHeaderMonthTextView));
    }
    else {
        madl->AddView(IView::Probe(mHeaderMonthTextView));
        madl->AddView(IView::Probe(mHeaderDayOfMonthTextView));
    }

    String dn;
    mCurrentDate->GetDisplayName(ICalendar::MONTH, ICalendar::SHORT, locale, &dn);
    String dnup = dn.ToUpperCase(/*locale*/);
    mHeaderMonthTextView->SetText(CoreUtils::Convert(dnup));
    AutoPtr<IDate> dt;
    mCurrentDate->GetTime((IDate**)&dt);
    String dayF, yearF;
    Elastos::Text::IDateFormat::Probe(mDayFormat)->Format(dt, &dayF);
    Elastos::Text::IDateFormat::Probe(mYearFormat)->Format(dt, &yearF);
    mHeaderDayOfMonthTextView->SetText(CoreUtils::Convert(dayF));
    mHeaderYearTextView->SetText(CoreUtils::Convert(yearF));

    // Accessibility.
    Int64 millis = 0;
    mCurrentDate->GetTimeInMillis(&millis);
    mAnimator->SetDateMillis(millis);
    Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_NO_YEAR;
    AutoPtr<IDateUtils> du;
    CDateUtils::AcquireSingleton((IDateUtils**)&du);
    String monthAndDayText;
    du->FormatDateTime(mContext, millis, flags, &monthAndDayText);
    IView::Probe(mMonthAndDayLayout)->SetContentDescription(CoreUtils::Convert(monthAndDayText));

    if (announce) {
        flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_SHOW_YEAR;
        String fullDateText;
        du->FormatDateTime(mContext, millis, flags, &fullDateText);
        IView::Probe(mAnimator)->AnnounceForAccessibility(CoreUtils::Convert(fullDateText));
    }
    UpdatePickers();
}

void DatePickerCalendarDelegate::SetCurrentView(
    /* [in] */ Int32 viewIndex)
{
    Int64 millis = 0;
    mCurrentDate->GetTimeInMillis(&millis);

    switch (viewIndex) {
        case MONTH_AND_DAY_VIEW: {
            IOnDateChangedListener::Probe(mDayPickerView)->OnDateChanged();
            if (mCurrentView != viewIndex) {
                IView::Probe(mMonthAndDayLayout)->SetSelected(TRUE);
                IView::Probe(mHeaderYearTextView)->SetSelected(FALSE);
                IViewAnimator::Probe(mAnimator)->SetDisplayedChild(MONTH_AND_DAY_VIEW);
                mCurrentView = viewIndex;
            }

            Int32 flags = IDateUtils::FORMAT_SHOW_DATE;
            AutoPtr<IDateUtils> du;
            CDateUtils::AcquireSingleton((IDateUtils**)&du);
            String dayString;
            du->FormatDateTime(mContext, millis, flags, &dayString);
            String dsp = mDayPickerDescription + ": " + dayString;
            IView* animator = IView::Probe(mAnimator);
            animator->SetContentDescription(CoreUtils::Convert(dsp));
            animator->AnnounceForAccessibility(CoreUtils::Convert(mSelectDay));
            break;
        }
        case YEAR_VIEW: {
            IOnDateChangedListener::Probe(mYearPickerView)->OnDateChanged();
            if (mCurrentView != viewIndex) {
                IView::Probe(mMonthAndDayLayout)->SetSelected(FALSE);
                IView::Probe(mHeaderYearTextView)->SetSelected(TRUE);
                IViewAnimator::Probe(mAnimator)->SetDisplayedChild(YEAR_VIEW);
                mCurrentView = viewIndex;
            }

            String yearString;
            IFormat::Probe(mYearFormat)->Format(CoreUtils::Convert(millis), &yearString);
            String yDSP = mYearPickerDescription + ": " + yearString;
            IView* animator = IView::Probe(mAnimator);
            animator->SetContentDescription(CoreUtils::Convert(yDSP));
            animator->AnnounceForAccessibility(CoreUtils::Convert(mSelectYear));
            break;
        }
    }
}

ECode DatePickerCalendarDelegate::Init(
    /* [in] */ Int32 year,
    /* [in] */ Int32 monthOfYear,
    /* [in] */ Int32 dayOfMonth,
    /* [in] */ IDatePickerOnDateChangedListener* callBack)
{
    mDateChangedListener = callBack;
    mCurrentDate->Set(ICalendar::YEAR, year);
    mCurrentDate->Set(ICalendar::MONTH, monthOfYear);
    mCurrentDate->Set(ICalendar::DAY_OF_MONTH, dayOfMonth);
    UpdateDisplay(FALSE);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::UpdateDate(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 dayOfMonth)
{
    mCurrentDate->Set(ICalendar::YEAR, year);
    mCurrentDate->Set(ICalendar::MONTH, month);
    mCurrentDate->Set(ICalendar::DAY_OF_MONTH, dayOfMonth);
    if (mDateChangedListener != NULL) {
        mDateChangedListener->OnDateChanged(mDelegator, year, month, dayOfMonth);
    }
    UpdateDisplay(FALSE);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetYear(
    /* [out] */ Int32* year)
{
    VALIDATE_NOT_NULL(year)
    return mCurrentDate->Get(ICalendar::YEAR, year);
}

ECode DatePickerCalendarDelegate::GetMonth(
    /* [out] */ Int32* month)
{
    VALIDATE_NOT_NULL(month)
    return mCurrentDate->Get(ICalendar::MONTH, month);
}

ECode DatePickerCalendarDelegate::GetDayOfMonth(
    /* [out] */ Int32* dayOfMonth)
{
    VALIDATE_NOT_NULL(dayOfMonth)
    return mCurrentDate->Get(ICalendar::DAY_OF_MONTH, dayOfMonth);
}

ECode DatePickerCalendarDelegate::SetMinDate(
    /* [in] */ Int64 minDate)
{
    mTempDate->SetTimeInMillis(minDate);
    Int32 tmpY = 0, minY = 0, tmpD = 0, minD = 0;
    mTempDate->Get(ICalendar::YEAR, &tmpY);
    mMinDate->Get(ICalendar::YEAR, &minY);
    mTempDate->Get(ICalendar::DAY_OF_YEAR, &tmpD);
    mMinDate->Get(ICalendar::DAY_OF_YEAR, &minD);
    if (tmpY == minY && tmpD != minD) {
        return NOERROR;
    }
    Boolean bBf = FALSE;
    if ((mCurrentDate->IsBefore(mTempDate, &bBf), bBf)) {
        mCurrentDate->SetTimeInMillis(minDate);
        UpdatePickers();
        UpdateDisplay(FALSE);
    }
    mMinDate->SetTimeInMillis(minDate);
    AutoPtr<ICalendar> cl;
    GetSelectedDay((ICalendar**)&cl);
    Boolean b = FALSE;
    mDayPickerView->GoTo(cl, FALSE, TRUE, TRUE, &b);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetMinDate(
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar)
    *calendar = mMinDate;
    REFCOUNT_ADD(*calendar)
    return NOERROR;
}

ECode DatePickerCalendarDelegate::SetMaxDate(
    /* [in] */ Int64 maxDate)
{
    mTempDate->SetTimeInMillis(maxDate);
    Int32 tmpY = 0, maxY = 0, tmpD = 0, maxD = 0;
    mTempDate->Get(ICalendar::YEAR, &tmpY);
    mMaxDate->Get(ICalendar::YEAR, &maxY);
    mTempDate->Get(ICalendar::DAY_OF_YEAR, &tmpD);
    mMaxDate->Get(ICalendar::DAY_OF_YEAR, &maxD);
    if (tmpY == maxY && tmpD != maxD) {
        return NOERROR;
    }
    Boolean bAf = FALSE;
    if ((mCurrentDate->IsAfter(mTempDate, &bAf), bAf)) {
        mCurrentDate->SetTimeInMillis(maxDate);
        UpdatePickers();
        UpdateDisplay(FALSE);
    }
    mMaxDate->SetTimeInMillis(maxDate);
    AutoPtr<ICalendar> cl;
    GetSelectedDay((ICalendar**)&cl);
    Boolean b = FALSE;
    mDayPickerView->GoTo(cl, FALSE, TRUE, TRUE, &b);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetMaxDate(
    /* [out] */ ICalendar** maxDate)
{
    VALIDATE_NOT_NULL(maxDate)
    *maxDate = mMaxDate;
    REFCOUNT_ADD(*maxDate)
    return NOERROR;
}

ECode DatePickerCalendarDelegate::SetFirstDayOfWeek(
    /* [in] */ Int32 firstDayOfWeek)
{
    mFirstDayOfWeek = firstDayOfWeek;
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetFirstDayOfWeek(
    /* [out] */ Int32* firstDayOfWeek)
{
    VALIDATE_NOT_NULL(firstDayOfWeek)
    if (mFirstDayOfWeek != USE_LOCALE) {
        *firstDayOfWeek = mFirstDayOfWeek;
        return NOERROR;
    }
    return mCurrentDate->GetFirstDayOfWeek(firstDayOfWeek);
}

ECode DatePickerCalendarDelegate::GetMinYear(
    /* [out] */ Int32* minYear)
{
    VALIDATE_NOT_NULL(minYear)
    return mMinDate->Get(ICalendar::YEAR, minYear);
}

ECode DatePickerCalendarDelegate::GetMaxYear(
    /* [out] */ Int32* maxYear)
{
    VALIDATE_NOT_NULL(maxYear)
    return mMaxDate->Get(ICalendar::YEAR, maxYear);
}

ECode DatePickerCalendarDelegate::GetMinMonth(
    /* [out] */ Int32* minMonth)
{
    VALIDATE_NOT_NULL(minMonth)
    return mMinDate->Get(ICalendar::MONTH, minMonth);
}

ECode DatePickerCalendarDelegate::GetMaxMonth(
    /* [out] */ Int32* maxMonth)
{
    VALIDATE_NOT_NULL(maxMonth)
    return mMaxDate->Get(ICalendar::MONTH, maxMonth);
}

ECode DatePickerCalendarDelegate::GetMinDay(
    /* [out] */ Int32* minDay)
{
    VALIDATE_NOT_NULL(minDay)
    return mMinDate->Get(ICalendar::DAY_OF_MONTH, minDay);
}

ECode DatePickerCalendarDelegate::GetMaxDay(
    /* [out] */ Int32* maxDay)
{
    VALIDATE_NOT_NULL(maxDay)
    return mMaxDate->Get(ICalendar::DAY_OF_MONTH, maxDay);
}

ECode DatePickerCalendarDelegate::SetEnabled(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mMonthAndDayLayout)->SetEnabled(enabled);
    IView::Probe(mHeaderYearTextView)->SetEnabled(enabled);
    IView::Probe(mAnimator)->SetEnabled(enabled);
    mIsEnabled = enabled;
    return NOERROR;
}

ECode DatePickerCalendarDelegate::IsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = mIsEnabled;
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetCalendarView(
    /* [out] */ ICalendarView** view)
{
    VALIDATE_NOT_NULL(view)
    // throw new UnsupportedOperationException(
    //         "CalendarView does not exists for the new DatePicker");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode DatePickerCalendarDelegate::SetCalendarViewShown(
    /* [in] */ Boolean shown)
{
    // No-op for compatibility with the old DatePicker.
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetCalendarViewShown(
    /* [out] */ Boolean* shown)
{
    VALIDATE_NOT_NULL(shown)
    *shown = FALSE;
    return NOERROR;
}

ECode DatePickerCalendarDelegate::SetSpinnersShown(
    /* [in] */ Boolean shown)
{
    // No-op for compatibility with the old DatePicker.
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetSpinnersShown(
    /* [out] */ Boolean* shown)
{
    VALIDATE_NOT_NULL(shown)
    *shown = FALSE;
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<CConfiguration> cfg = (CConfiguration*)newConfig;
    CSimpleDateFormat::New(String("y"), cfg->mLocale, (ISimpleDateFormat**)&mYearFormat);
    CSimpleDateFormat::New(String("d"), cfg->mLocale, (ISimpleDateFormat**)&mDayFormat);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnSaveInstanceState(
    /* [in] */ IParcelable* superState,
    /* [out] */ IParcelable** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 year = 0;
    mCurrentDate->Get(ICalendar::YEAR, &year);
    Int32 month = 0;
    mCurrentDate->Get(ICalendar::MONTH, &month);
    Int32 day = 0;
    mCurrentDate->Get(ICalendar::DAY_OF_MONTH, &day);

    Int32 listPosition = -1;
    Int32 listPositionOffset = -1;

    if (mCurrentView == MONTH_AND_DAY_VIEW) {
        mDayPickerView->GetMostVisiblePosition(&listPosition);
    }
    else if (mCurrentView == YEAR_VIEW) {
        IAdapterView::Probe(mYearPickerView)->GetFirstVisiblePosition(&listPosition);
        mYearPickerView->GetFirstPositionOffset(&listPositionOffset);
    }

    Int64 minMils = 0, maxMils = 0;
    mMinDate->GetTimeInMillis(&minMils);
    mMaxDate->GetTimeInMillis(&maxMils);
    AutoPtr<IDatePickerCalendarDelegateSavedState> res;
    CDatePickerCalendarDelegateSavedState::New(superState, year, month, day, minMils,
            maxMils, mCurrentView, listPosition, listPositionOffset,
            (IDatePickerCalendarDelegateSavedState**)&res);
    *result = IParcelable::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    AutoPtr<DatePickerCalendarDelegateSavedState> ss = (DatePickerCalendarDelegateSavedState*) state;

    mCurrentDate->Set(ss->GetSelectedYear(), ss->GetSelectedMonth(), ss->GetSelectedDay());
    mCurrentView = ss->GetCurrentView();
    mMinDate->SetTimeInMillis(ss->GetMinDate());
    mMaxDate->SetTimeInMillis(ss->GetMaxDate());

    UpdateDisplay(FALSE);
    SetCurrentView(mCurrentView);

    Int32 listPosition = ss->GetListPosition();
    if (listPosition != -1) {
        if (mCurrentView == MONTH_AND_DAY_VIEW) {
            mDayPickerView->PostSetSelection(listPosition);
        }
        else if (mCurrentView == YEAR_VIEW) {
            mYearPickerView->PostSetSelectionFromTop(listPosition, ss->GetListPositionOffset());
        }
    }
    return NOERROR;
}

ECode DatePickerCalendarDelegate::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    OnPopulateAccessibilityEvent(event);
    *rst = TRUE;
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AutoPtr<IDate> d;
    mCurrentDate->GetTime((IDate**)&d);
    String str;
    d->ToString(&str);

    AutoPtr<IList> l;
    IAccessibilityRecord::Probe(event)->GetText((IList**)&l);
    l->Add(CoreUtils::Convert(str));
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    IAccessibilityRecord::Probe(event)->SetClassName(CoreUtils::Convert("CDatePickerCalendarDelegate"));
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    info->SetClassName(CoreUtils::Convert("CDatePickerCalendarDelegate"));
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnYearSelected(
    /* [in] */ Int32 year)
{
    Int32 m = 0;
    mCurrentDate->Get(ICalendar::MONTH, &m);
    AdjustDayInMonthIfNeeded(m, year);
    mCurrentDate->Set(ICalendar::YEAR, year);
    UpdatePickers();
    SetCurrentView(MONTH_AND_DAY_VIEW);
    UpdateDisplay(TRUE);
    return NOERROR;
}

void DatePickerCalendarDelegate::AdjustDayInMonthIfNeeded(
    /* [in] */ Int32 month,
    /* [in] */ Int32 year)
{
    Int32 day = 0;
    mCurrentDate->Get(ICalendar::DAY_OF_MONTH, &day);
    Int32 daysInMonth = 0;
    daysInMonth = GetDaysInMonth(month, year);
    if (day > daysInMonth) {
        mCurrentDate->Set(ICalendar::DAY_OF_MONTH, daysInMonth);
    }
}

Int32 DatePickerCalendarDelegate::GetDaysInMonth(
    /* [in] */ Int32 month,
    /* [in] */ Int32 year)
{
    switch (month) {
        case ICalendar::JANUARY:
        case ICalendar::MARCH:
        case ICalendar::MAY:
        case ICalendar::JULY:
        case ICalendar::AUGUST:
        case ICalendar::OCTOBER:
        case ICalendar::DECEMBER:
            return 31;
        case ICalendar::APRIL:
        case ICalendar::JUNE:
        case ICalendar::SEPTEMBER:
        case ICalendar::NOVEMBER:
            return 30;
        case ICalendar::FEBRUARY:
            return (year % 4 == 0) ? 29 : 28;
        default:
            return -1;
        //    throw new IllegalArgumentException("Invalid Month");
    }
}

ECode DatePickerCalendarDelegate::OnDayOfMonthSelected(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    mCurrentDate->Set(ICalendar::YEAR, year);
    mCurrentDate->Set(ICalendar::MONTH, month);
    mCurrentDate->Set(ICalendar::DAY_OF_MONTH, day);
    UpdatePickers();
    UpdateDisplay(TRUE);
    return NOERROR;
}

void DatePickerCalendarDelegate::UpdatePickers()
{
    AutoPtr<IIterator> it;
    mListeners->GetIterator((IIterator**)&it);
    Boolean b = FALSE;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IOnDateChangedListener> listener = IOnDateChangedListener::Probe(p);
        listener->OnDateChanged();
    }
}

ECode DatePickerCalendarDelegate::RegisterOnDateChangedListener(
    /* [in] */ IOnDateChangedListener* listener)
{
    mListeners->Add(listener);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::UnregisterOnDateChangedListener(
    /* [in] */ IOnDateChangedListener* listener)
{
    mListeners->Remove(listener);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::GetSelectedDay(
    /* [out] */ ICalendar** day)
{
    VALIDATE_NOT_NULL(day)
    *day = mCurrentDate;
    REFCOUNT_ADD(*day)
    return NOERROR;
}

ECode DatePickerCalendarDelegate::TryVibrate()
{
    Boolean b = FALSE;
    IView::Probe(mDelegator)->PerformHapticFeedback(IHapticFeedbackConstants::CALENDAR_DATE, &b);
    return NOERROR;
}

ECode DatePickerCalendarDelegate::OnClick(
    /* [in] */ IView* v)
{
    TryVibrate();
    Int32 id = 0;
    v->GetId(&id);
    if (id == R::id::date_picker_year) {
        SetCurrentView(YEAR_VIEW);
    }
    else if (id == R::id::date_picker_month_and_day_layout) {
        SetCurrentView(MONTH_AND_DAY_VIEW);
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
