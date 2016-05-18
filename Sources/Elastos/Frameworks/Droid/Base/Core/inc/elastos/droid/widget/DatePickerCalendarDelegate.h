
#ifndef __ELASTOS_DROID_WIDGET_DATEPICKERCALENDARDELEGATE_H__
#define __ELASTOS_DROID_WIDGET_DATEPICKERCALENDARDELEGATE_H__

#include "elastos/droid/widget/DatePicker.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Internal::Widget::IAccessibleDateAnimator;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * A delegate for picking up a date (day / month / year).
 */
class DatePickerCalendarDelegate
    : public DatePicker::AbstractDatePickerDelegate
    , public IDatePickerCalendarDelegate
    , public IViewOnClickListener
    , public IDatePickerController
{
public:
    /**
     * Class for managing state storing/restoring.
     */
    class DatePickerCalendarDelegateSavedState
        : public Elastos::Droid::View::View::BaseSavedState
        , public IDatePickerCalendarDelegateSavedState
    {
    public:
        CAR_INTERFACE_DECL()

        DatePickerCalendarDelegateSavedState();

        ~DatePickerCalendarDelegateSavedState();

        /**
         * Constructor called from {@link DatePicker#onSaveInstanceState()}
         */
        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 day,
            /* [in] */ Int64 minDate,
            /* [in] */ Int64 maxDate,
            /* [in] */ Int32 currentView,
            /* [in] */ Int32 listPosition,
            /* [in] */ Int32 listPositionOffset);

        CARAPI constructor();

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        /**
         * Constructor called from {@link #CREATOR}
         */
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI_(Int32) GetSelectedDay();

        CARAPI_(Int32) GetSelectedMonth();

        CARAPI_(Int32) GetSelectedYear();

        CARAPI_(Int64) GetMinDate();

        CARAPI_(Int64) GetMaxDate();

        CARAPI_(Int32) GetCurrentView();

        CARAPI_(Int32) GetListPosition();

        CARAPI_(Int32) GetListPositionOffset();

    private:
        Int32 mSelectedYear;
        Int32 mSelectedMonth;
        Int32 mSelectedDay;
        Int64 mMinDate;
        Int64 mMaxDate;
        Int32 mCurrentView;
        Int32 mListPosition;
        Int32 mListPositionOffset;
    };

public:
    CAR_INTERFACE_DECL()

    DatePickerCalendarDelegate();

    ~DatePickerCalendarDelegate();

    CARAPI constructor(
        /* [in] */ IDatePicker* delegator,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI Init(
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ IDatePickerOnDateChangedListener* callBack);

    CARAPI UpdateDate(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 dayOfMonth);

    CARAPI GetYear(
        /* [out] */ Int32* year);

    CARAPI GetMonth(
        /* [out] */ Int32* month);

    CARAPI GetDayOfMonth(
        /* [out] */ Int32* dayOfMonth);

    CARAPI SetMinDate(
        /* [in] */ Int64 minDate);

    CARAPI GetMinDate(
        /* [out] */ ICalendar** calendar);

    CARAPI SetMaxDate(
        /* [in] */ Int64 maxDate);

    CARAPI GetMaxDate(
        /* [out] */ ICalendar** maxDate);

    CARAPI SetFirstDayOfWeek(
        /* [in] */ Int32 firstDayOfWeek);

    CARAPI GetFirstDayOfWeek(
        /* [out] */ Int32* firstDayOfWeek);

    CARAPI GetMinYear(
        /* [out] */ Int32* minYear);

    CARAPI GetMaxYear(
        /* [out] */ Int32* maxYear);

    CARAPI GetMinMonth(
        /* [out] */ Int32* minMonth);

    CARAPI GetMaxMonth(
        /* [out] */ Int32* maxMonth);

    CARAPI GetMinDay(
        /* [out] */ Int32* minDay);

    CARAPI GetMaxDay(
        /* [out] */ Int32* maxDay);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI GetCalendarView(
        /* [out] */ ICalendarView** view);

    CARAPI SetCalendarViewShown(
        /* [in] */ Boolean shown);

    CARAPI GetCalendarViewShown(
        /* [out] */ Boolean* shown);

    CARAPI SetSpinnersShown(
        /* [in] */ Boolean shown);

    CARAPI GetSpinnersShown(
        /* [out] */ Boolean* shown);

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnSaveInstanceState(
        /* [in] */ IParcelable* superState,
        /* [out] */ IParcelable** result);

    CARAPI OnRestoreInstanceState(
            /* [in] */ IParcelable* state);

    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* rst);

    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI OnYearSelected(
        /* [in] */ Int32 year);

    static CARAPI_(Int32) GetDaysInMonth(
        /* [in] */ Int32 month,
        /* [in] */ Int32 year);

    CARAPI OnDayOfMonthSelected(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    CARAPI RegisterOnDateChangedListener(
        /* [in] */ IOnDateChangedListener* listener);

    CARAPI UnregisterOnDateChangedListener(
        /* [in] */ IOnDateChangedListener* listener);

    CARAPI GetSelectedDay(
        /* [out] */ ICalendar** day);

    CARAPI TryVibrate();

    CARAPI OnClick(
        /* [in] */ IView* v);

private:
    /**
     * Gets a calendar for locale bootstrapped with the value of a given calendar.
     *
     * @param oldCalendar The old calendar.
     * @param locale The locale.
     */
    CARAPI_(AutoPtr<ICalendar>) GetCalendarForLocale(
        /* [in] */ ICalendar* oldCalendar,
        /* [in] */ ILocale* locale);

    /**
     * Compute the array representing the order of Month / Day / Year views in their layout.
     * Will be used for I18N purpose as the order of them depends on the Locale.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetMonthDayYearIndexes(
        /* [in] */ const String& pattern);

    CARAPI_(void) UpdateDisplay(
        /* [in] */ Boolean announce);

    CARAPI_(void) SetCurrentView(
        /* [in] */ Int32 viewIndex);

    // If the newly selected month / year does not contain the currently selected day number,
    // change the selected day number to the last day of the selected month or year.
    //      e.g. Switching from Mar to Apr when Mar 31 is selected -> Apr 30
    //      e.g. Switching from 2012 to 2013 when Feb 29, 2012 is selected -> Feb 28, 2013
    CARAPI_(void) AdjustDayInMonthIfNeeded(
        /* [in] */ Int32 month,
        /* [in] */ Int32 year);

    CARAPI_(void) UpdatePickers();

private:
    static const Int32 USE_LOCALE;

    static const Int32 UNINITIALIZED;
    static const Int32 MONTH_AND_DAY_VIEW;
    static const Int32 YEAR_VIEW;

    static const Int32 DEFAULT_START_YEAR;
    static const Int32 DEFAULT_END_YEAR;

    static const Int32 ANIMATION_DURATION;

    static const Int32 MONTH_INDEX;
    static const Int32 DAY_INDEX;
    static const Int32 YEAR_INDEX;

    AutoPtr<ISimpleDateFormat> mYearFormat;
    AutoPtr<ISimpleDateFormat> mDayFormat;

    AutoPtr<ITextView> mDayOfWeekView;

    /** Layout that contains the current month, day, and year. */
    AutoPtr<ILinearLayout> mMonthDayYearLayout;

    /** Clickable layout that contains the current day and year. */
    AutoPtr<ILinearLayout> mMonthAndDayLayout;

    AutoPtr<ITextView> mHeaderMonthTextView;
    AutoPtr<ITextView> mHeaderDayOfMonthTextView;
    AutoPtr<ITextView> mHeaderYearTextView;
    AutoPtr<IDayPickerView> mDayPickerView;
    AutoPtr<IYearPickerView> mYearPickerView;

    Boolean mIsEnabled;

    // Accessibility strings.
    String mDayPickerDescription;
    String mSelectDay;
    String mYearPickerDescription;
    String mSelectYear;

    AutoPtr<IAccessibleDateAnimator> mAnimator;

    AutoPtr<IDatePickerOnDateChangedListener> mDateChangedListener;

    Int32 mCurrentView;

    AutoPtr<ICalendar> mCurrentDate;
    AutoPtr<ICalendar> mTempDate;
    AutoPtr<ICalendar> mMinDate;
    AutoPtr<ICalendar> mMaxDate;

    Int32 mFirstDayOfWeek;

    AutoPtr<IHashSet> mListeners;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
