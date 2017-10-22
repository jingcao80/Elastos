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

#ifndef __ELASTOS_DROID_WIDGET_DATEPICKER_H__
#define __ELASTOS_DROID_WIDGET_DATEPICKER_H__

#include <Elastos.CoreLibrary.h>
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IViewBaseSavedState;
using Elastos::Droid::View::View;
using Elastos::Droid::View::EIID_IViewBaseSavedState;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::INumberPickerOnValueChangeListener;
using Elastos::Text::IDateFormat;
using Elastos::Utility::ILocale;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget {

class DatePicker
    : public FrameLayout
    , public IDatePicker
{
public:
    /**
     * An abstract class which can be used as a start for DatePicker implementations
     */
    class AbstractDatePickerDelegate
        : public Object
        , public IDatePickerDelegate
    {
    public:
        CAR_INTERFACE_DECL()

        AbstractDatePickerDelegate();

        CARAPI constructor(
            /* [in] */ IDatePicker* delegator,
            /* [in] */ IContext* context);

        CARAPI SetValidationCallback(
            /* [in] */ IDatePickerValidationCallback* callback);

    protected:
        CARAPI_(void) SetCurrentLocale(
            /* [in] */ ILocale* locale);

        CARAPI_(void) OnValidationChanged(
            /* [in] */ Boolean valid);

    protected:
        // The delegator
        IDatePicker* mDelegator;

        // The context
        IContext* mContext;

        // The current locale
        AutoPtr<ILocale> mCurrentLocale;

        // Callbacks
        AutoPtr<IDatePickerOnDateChangedListener> mOnDateChangedListener;
        AutoPtr<IDatePickerValidationCallback> mValidationCallback;
    };

    class DatePickerSavedState
        : public View::BaseSavedState
        , public IDatePickerSavedState
    {
        friend class DatePicker;
    public:
        CAR_INTERFACE_DECL()

        DatePickerSavedState();

        ~DatePickerSavedState();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IParcelable* superState,
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 day);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* in);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

    private:
        Int32 mYear;
        Int32 mMonth;
        Int32 mDay;
    };

private:
    /**
     * A delegate implementing the basic DatePicker
     */
    class DatePickerSpinnerDelegate
        : public AbstractDatePickerDelegate
    {
    public:
        DatePickerSpinnerDelegate();

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
            /* [in] */ IDatePickerOnDateChangedListener* onDateChangedListener);

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

        CARAPI SetFirstDayOfWeek(
            /* [in] */ Int32 firstDayOfWeek);

        CARAPI GetFirstDayOfWeek(
            /* [out] */ Int32* firstDayOfWeek);

        CARAPI SetMinDate(
            /* [in] */ Int64 minDate);

        CARAPI GetMinDate(
            /* [out] */ ICalendar** calendar);

        CARAPI SetMaxDate(
            /* [in] */ Int64 maxDate);

        CARAPI GetMaxDate(
            /* [out] */ ICalendar** maxDate);

        CARAPI SetEnabled(
            /* [in] */ Boolean enabled);

        CARAPI IsEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI GetCalendarView (
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

        /**
         * Sets the current locale.
         *
         * @param locale The current locale.
         */
        CARAPI SetCurrentLocale(
            /* [in] */ ILocale* locale);

        /**
         * Tests whether the current locale is one where there are no real month names,
         * such as Chinese, Japanese, or Korean locales.
         */
        CARAPI_(Boolean) UsingNumericMonths();

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
         * Reorders the spinners according to the date format that is
         * explicitly set by the user and if no such is set fall back
         * to the current locale's default format.
         */
        CARAPI ReorderSpinners();

        /**
         * Parses the given <code>date</code> and in case of success sets the result
         * to the <code>outDate</code>.
         *
         * @return True if the date was parsed.
         */
        CARAPI_(Boolean) ParseDate(
            /* [in] */ const String& date,
            /* [in] */ ICalendar* outDate);

        CARAPI_(Boolean) IsNewDate(
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 dayOfMonth);

        CARAPI SetDate(
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 dayOfMonth);

        CARAPI UpdateSpinners();

        /**
         * Updates the calendar view with the current date.
         */
        CARAPI UpdateCalendarView();

        /**
         * Notifies the listener, if such, for a change in the selected date.
         */
        CARAPI NotifyDateChanged();

        /**
         * Sets the IME options for a spinner based on its ordering.
         *
         * @param spinner The spinner.
         * @param spinnerCount The total spinner count.
         * @param spinnerIndex The index of the given spinner.
         */
        CARAPI SetImeOptions(
            /* [in] */ INumberPicker* spinner,
            /* [in] */ Int32 spinnerCount,
            /* [in] */ Int32 spinnerIndex);

        CARAPI SetContentDescriptions();

        CARAPI TrySetContentDescription(
            /* [in] */ IView* root,
            /* [in] */ Int32 viewId,
            /* [in] */ Int32 contDescResId);

        CARAPI UpdateInputState();

    public:
        static const String DATE_FORMAT;

        static const Int32 DEFAULT_START_YEAR;

        static const Int32 DEFAULT_END_YEAR;

        static const Boolean DEFAULT_CALENDAR_VIEW_SHOWN;

        static const Boolean DEFAULT_SPINNERS_SHOWN;

        static const Boolean DEFAULT_ENABLED_STATE;

        AutoPtr<ILinearLayout> mSpinners;

        AutoPtr<INumberPicker> mDaySpinner;

        AutoPtr<INumberPicker> mMonthSpinner;

        AutoPtr<INumberPicker> mYearSpinner;

        AutoPtr<IEditText> mDaySpinnerInput;

        AutoPtr<IEditText> mMonthSpinnerInput;

        AutoPtr<IEditText> mYearSpinnerInput;

        AutoPtr<ICalendarView> mCalendarView;

        AutoPtr<ArrayOf<String> > mShortMonths;

        AutoPtr<IDateFormat> mDateFormat;

        Int32 mNumberOfMonths;

        AutoPtr<ICalendar> mTempDate;

        AutoPtr<ICalendar> mMinDate;

        AutoPtr<ICalendar> mMaxDate;

        AutoPtr<ICalendar> mCurrentDate;

        Boolean mIsEnabled;
    };

    class DatePickerOnChangeListener
        : public Object
        , public INumberPickerOnValueChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        DatePickerOnChangeListener(
            /* [in] */ DatePickerSpinnerDelegate* host);

        CARAPI OnValueChange(
            /* [in] */ INumberPicker* picker,
            /* [in] */ Int32 oldVal,
            /* [in] */ Int32 newVal);

    private:
        DatePickerSpinnerDelegate* mHost;
    };

    class DatePickerOnDateChangeListener //CalendarView
        : public Object
        , public IOnDateChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        DatePickerOnDateChangeListener(
            /* [in] */ DatePickerSpinnerDelegate* host);

        CARAPI OnSelectedDayChange(
            /* [in] */ ICalendarView* view,
            /* [in] */ Int32 year,
            /* [in] */ Int32 month,
            /* [in] */ Int32 dayOfMonth);

    private:
        DatePickerSpinnerDelegate* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DatePicker();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI Init(
        /* [in] */ Int32 year,
        /* [in] */ Int32 monthOfYear,
        /* [in] */ Int32 dayOfMonth,
        /* [in] */ IDatePickerOnDateChangedListener* onDateChangedListener);

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

    CARAPI GetMinDate(
        /* [out] */ Int64* calendar);

    CARAPI SetMinDate(
        /* [in] */ Int64 minDate);

    CARAPI GetMaxDate(
        /* [out] */ Int64* maxDate);

    CARAPI SetMaxDate(
        /* [in] */ Int64 maxDate);

    CARAPI SetValidationCallback(
        /* [in] */ IDatePickerValidationCallback* cb);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* rst);

    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    CARAPI SetFirstDayOfWeek(
        /* [in] */ Int32 firstDayOfWeek);

    CARAPI GetFirstDayOfWeek(
        /* [out] */ Int32* firstDayOfWeek);

    CARAPI GetCalendarViewShown(
        /* [out] */ Boolean* shown);

    CARAPI GetCalendarView (
        /* [out] */ ICalendarView** view);

    CARAPI SetCalendarViewShown(
        /* [in] */ Boolean shown);

    CARAPI GetSpinnersShown(
        /* [out] */ Boolean* shown);

    CARAPI SetSpinnersShown(
        /* [in] */ Boolean shown);

protected:
    virtual CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    /*protected void dispatchRestoreInstanceState(SparseArray<IParcelable> container);*/
    virtual CARAPI DispatchRestoreInstanceState(
            /* [in] */ ISparseArray* container);

    virtual CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    virtual CARAPI OnRestoreInstanceState(
            /* [in] */ IParcelable* state);

private:
    CARAPI_(AutoPtr<IDatePickerDelegate>) CreateSpinnerUIDelegate(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI_(AutoPtr<IDatePickerDelegate>) CreateCalendarUIDelegate(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

private:
    static const String TAG;

    static const Int32 MODE_SPINNER;
    static const Int32 MODE_CALENDAR;

    AutoPtr<IDatePickerDelegate> mDelegate;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
