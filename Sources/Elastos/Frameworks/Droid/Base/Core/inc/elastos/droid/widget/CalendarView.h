#ifndef __ELASTOS_DROID_WIDGET_CALENDARVIEW_H__
#define __ELASTOS_DROID_WIDGET_CALENDARVIEW_H__

#include "elastos/droid/database/DataSetObserver.h"
#include "elastos/droid/view/GestureDetector.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::GestureDetector;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Text::IDateFormat;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * This class is a calendar widget for displaying and selecting dates. The range
 * of dates supported by this calendar is configurable. A user can select a date
 * by taping on it and can scroll and fling the calendar to a desired date.
 *
 * @attr ref android.R.styleable#CalendarView_showWeekNumber
 * @attr ref android.R.styleable#CalendarView_firstDayOfWeek
 * @attr ref android.R.styleable#CalendarView_minDate
 * @attr ref android.R.styleable#CalendarView_maxDate
 * @attr ref android.R.styleable#CalendarView_shownWeekCount
 * @attr ref android.R.styleable#CalendarView_selectedWeekBackgroundColor
 * @attr ref android.R.styleable#CalendarView_focusedMonthDateColor
 * @attr ref android.R.styleable#CalendarView_unfocusedMonthDateColor
 * @attr ref android.R.styleable#CalendarView_weekNumberColor
 * @attr ref android.R.styleable#CalendarView_weekSeparatorLineColor
 * @attr ref android.R.styleable#CalendarView_selectedDateVerticalBar
 * @attr ref android.R.styleable#CalendarView_weekDayTextAppearance
 * @attr ref android.R.styleable#CalendarView_dateTextAppearance
 */
class CalendarView
    : public FrameLayout
    , public ICalendarView
{
public:
    /**
     * An abstract class which can be used as a start for CalendarView implementations
     */
    class AbstractCalendarViewDelegate
        : public Object
        , public ICalendarViewDelegate
    {
    public:
        CAR_INTERFACE_DECL()

        AbstractCalendarViewDelegate(
            /* [in] */ ICalendarView* delegator,
            /* [in] */ IContext* context);

        CARAPI_(void) SetCurrentLocale(
            /* [in] */ ILocale* locale);

    protected:
        // The delegator
        AutoPtr<ICalendarView> mDelegator;

        // The context
        AutoPtr<IContext> mContext;

        // The current locale
        AutoPtr<ILocale> mCurrentLocale;
    };

private:
    /**
     * A delegate implementing the legacy CalendarView
     */
    class LegacyCalendarViewDelegate
        : public AbstractCalendarViewDelegate
    {
    private:
        /**
         * Command responsible for acting upon scroll state changes.
         */
        class ScrollStateRunnable
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            ScrollStateRunnable(
                /* [in] */ LegacyCalendarViewDelegate* host);

            /**
             * Sets up the runnable with a short delay in case the scroll state
             * immediately changes again.
             *
             * @param view The list view that changed state
             * @param scrollState The new state it changed to
             */
            CARAPI_(void) DoScrollStateChange(
                /* [in] */ IAbsListView* view,
                /* [in] */ Int32 scrollState);

            CARAPI Run();

        private:
            AutoPtr<IAbsListView> mView;

            Int32 mNewState;

            LegacyCalendarViewDelegate* mHost;
        };

        /**
         * <p>
         * This is a specialized adapter for creating a list of weeks with
         * selectable days. It can be configured to display the week number, start
         * the week on a given day, show a reduced number of days, or display an
         * arbitrary number of weeks at a time.
         * </p>
         */
        class WeeksAdapter
            : public BaseAdapter
            , public IViewOnTouchListener
        {
        public:
            /**
             * This is here so we can identify single tap events and set the
             * selected day correctly
             */
            class CalendarGestureListener
                : public GestureDetector::SimpleOnGestureListener
            {
            public:
                CARAPI OnSingleTapUp(
                    /* [in] */ IMotionEvent* e,
                    /* [out] */ Boolean* res);
            };

        public:
            CAR_INTERFACE_DECL()

            WeeksAdapter(
                /* [in] */ IContext* context,
                /* [in] */ LegacyCalendarViewDelegate* host);

            /**
             * Updates the selected day and related parameters.
             *
             * @param selectedDay The time to highlight
             */
            CARAPI_(void) SetSelectedDay(
                /* [in] */ ICalendar* selectedDay);

            /**
             * @return The selected day of month.
             */
            CARAPI_(AutoPtr<ICalendar>) GetSelectedDay();

            CARAPI GetCount(
                /* [out] */ Int32* result);

            CARAPI GetItem(
                /* [in] */ Int32 position,
                /* [out] */ IInterface** result);

            CARAPI GetItemId(
                /* [in] */ Int32 position,
                /* [out] */ Int64* result);

            CARAPI GetView(
                /* [in] */ Int32 position,
                /* [in] */ IView* convertView,
                /* [in] */ IViewGroup* parent,
                /* [out] */ IView** result);

            /**
             * Changes which month is in focus and updates the view.
             *
             * @param month The month to show as in focus [0-11]
             */
            CARAPI_(void) SetFocusMonth(
                /* [in] */ Int32 month);

            CARAPI OnTouch(
                /* [in] */ IView* v,
                /* [in] */ IMotionEvent* event,
                /* [out] */ Boolean* result);

            /**
             * Set up the gesture detector and selected time
             */
            CARAPI_(void) Init();

            /**
             * Maintains the same hour/min/sec but moves the day to the tapped day.
             *
             * @param day The day that was tapped
             */
            CARAPI_(void) OnDateTapped(
                /* [in] */ ICalendar* day);

        public:
            Int32 mSelectedWeek;

            AutoPtr<IGestureDetector> mGestureDetector;

            Int32 mFocusedMonth;

            AutoPtr<ICalendar> mSelectedDate;

            Int32 mTotalWeekCount;

            LegacyCalendarViewDelegate* mHost;
        };

        /**
         * <p>
         * This is a dynamic view for drawing a single week. It can be configured to
         * display the week number, start the week on a given day, or show a reduced
         * number of days. It is intended for use as a single view within a
         * ListView. See {@link WeeksAdapter} for usage.
         * </p>
         */
        class WeekView
            : public View
        {
        public:
            WeekView(
                /* [in] */ IContext* context,
                /* [in] */ LegacyCalendarViewDelegate* host);

            /**
             * Initializes this week view.
             *
             * @param weekNumber The number of the week this view represents. The
             *            week number is a zero based index of the weeks since
             *            {@link CalendarView#getMinDate()}.
             * @param selectedWeekDay The selected day of the week from 0 to 6, -1 if no
             *            selected day.
             * @param focusedMonth The month that is currently in focus i.e.
             *            highlighted.
             */
            CARAPI_(void) Init(
                /* [in] */ Int32 weekNumber,
                /* [in] */ Int32 selectedWeekDay,
                /* [in] */ Int32 focusedMonth);

            /**
             * Returns the month of the first day in this week.
             *
             * @return The month the first day of this view is in.
             */
            CARAPI_(Int32) GetMonthOfFirstWeekDay();

            /**
             * Returns the month of the last day in this week
             *
             * @return The month the last day of this view is in
             */
            CARAPI_(Int32) GetMonthOfLastWeekDay();

            /**
             * Returns the first day in this view.
             *
             * @return The first day in the view.
             */
            CARAPI_(AutoPtr<ICalendar>) GetFirstDay();

            /**
             * Calculates the day that the given x position is in, accounting for
             * week number.
             *
             * @param x The x position of the touch event.
             * @return True if a day was found for the given location.
             */
            CARAPI_(Boolean) GetDayFromLocation(
                /* [in] */ float x,
                /* [in] */ ICalendar* outCalendar);

            CARAPI_(void) OnDraw(
                /* [in] */ ICanvas* canvas);

            CARAPI_(void) OnSizeChanged(
                /* [in] */ Int32 w,
                /* [in] */ Int32 h,
                /* [in] */ Int32 oldw,
                /* [in] */ Int32 oldh);

            CARAPI OnMeasure(
                /* [in] */ Int32 widthMeasureSpec,
                /* [in] */ Int32 heightMeasureSpec);

        private:
            /**
             * Initialize the paint instances.
             */
            CARAPI_(void) InitilaizePaints();

            /**
             * This draws the selection highlight if a day is selected in this week.
             *
             * @param canvas The canvas to draw on
             */
            CARAPI_(void) DrawBackground(
                /* [in] */ ICanvas* canvas);

            /**
             * Draws the week and month day numbers for this week.
             *
             * @param canvas The canvas to draw on
             */
            CARAPI_(void) DrawWeekNumbersAndDates(
                /* [in] */ ICanvas* canvas);

            /**
             * Draws a horizontal line for separating the weeks.
             *
             * @param canvas The canvas to draw on.
             */
            CARAPI_(void) DrawWeekSeparators(
                /* [in] */ ICanvas* canvas);

            /**
             * Draws the selected date bars if this week has a selected day.
             *
             * @param canvas The canvas to draw on
             */
            CARAPI_(void) DrawSelectedDateVerticalBars(
                /* [in] */ ICanvas* canvas);

            /**
             * This calculates the positions for the selected day lines.
             */
            CARAPI_(void) UpdateSelectionPositions();

        public:
            AutoPtr<IRect> mTempRect;

            AutoPtr<IPaint> mDrawPaint;

            AutoPtr<IPaint> mMonthNumDrawPaint;

            // Cache the number strings so we don't have to recompute them each time
            AutoPtr<ArrayOf<String> > mDayNumbers;

            // Quick lookup for checking which days are in the focus month
            AutoPtr<ArrayOf<Boolean> > mFocusDay;

            // Whether this view has a focused day.
            Boolean mHasFocusedDay;

            // Whether this view has only focused days.
            Boolean mHasUnfocusedDay;

            // The first day displayed by this item
            AutoPtr<ICalendar> mFirstDay;

            // The month of the first day in this week
            Int32 mMonthOfFirstWeekDay;

            // The month of the last day in this week
            Int32 mLastWeekDayMonth;

            // The position of this week, equivalent to weeks since the week of Jan
            // 1st, 1900
            Int32 mWeek;

            // Quick reference to the width of this view, matches parent
            Int32 mWidth;

            // The height this view should draw at in pixels, set by height param
            Int32 mHeight;

            // If this view contains the selected day
            Boolean mHasSelectedDay;

            // Which day is selected [0-6] or -1 if no day is selected
            Int32 mSelectedDay;

            // The number of days + a spot for week number if it is displayed
            Int32 mNumCells;

            // The left edge of the selected day
            Int32 mSelectedLeft;

            // The right edge of the selected day
            Int32 mSelectedRight;

            LegacyCalendarViewDelegate* mHost;
        };

        class _DataSetObserver
            : public DataSetObserver
        {
        public:
            _DataSetObserver(
                /* [in] */ LegacyCalendarViewDelegate* host);

            CARAPI OnChanged();

            CARAPI OnInvalidated();

        public:
            LegacyCalendarViewDelegate* mHost;
        };

        class OnScrollListener
            : public Object
            , public IAbsListViewOnScrollListener
        {
        public:
            CAR_INTERFACE_DECL()

            OnScrollListener(
                /* [in] */ LegacyCalendarViewDelegate* owner);

            CARAPI OnScrollStateChanged(
                /* [in] */ IAbsListView* view,
                /* [in] */ Int32 scrollState);

            CARAPI OnScroll(
                /* [in] */ IAbsListView* view,
                /* [in] */ Int32 firstVisibleItem,
                /* [in] */ Int32 visibleItemCount,
                /* [in] */ Int32 totalItemCount);

        public:
            LegacyCalendarViewDelegate* mOwner;
        };

    public:
        LegacyCalendarViewDelegate(
            /* [in] */ ICalendarView* delegator,
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 defStyleAttr,
            /* [in] */ Int32 defStyleRes);

        CARAPI SetShownWeekCount(
            /* [in] */ Int32 count);

        CARAPI GetShownWeekCount(
            /* [out] */ Int32* result);

        CARAPI SetSelectedWeekBackgroundColor(
            /* [in] */ Int32 color);

        CARAPI GetSelectedWeekBackgroundColor(
            /* [out] */ Int32* result);

        CARAPI SetFocusedMonthDateColor(
            /* [in] */ Int32 color);

        CARAPI GetFocusedMonthDateColor(
            /* [out] */ Int32* result);

        CARAPI SetUnfocusedMonthDateColor(
            /* [in] */ Int32 color);

        CARAPI GetUnfocusedMonthDateColor(
            /* [out] */ Int32* result);

        CARAPI SetWeekNumberColor(
            /* [in] */ Int32 color);

        CARAPI GetWeekNumberColor(
            /* [out] */ Int32* result);

        CARAPI SetWeekSeparatorLineColor(
            /* [in] */ Int32 color);

        CARAPI GetWeekSeparatorLineColor(
            /* [out] */ Int32* result);

        CARAPI SetSelectedDateVerticalBar(
            /* [in] */ Int32 resourceId);

        CARAPI SetSelectedDateVerticalBar(
            /* [in] */ IDrawable* drawable);

        CARAPI GetSelectedDateVerticalBar(
            /* [out] */ IDrawable** result);

        CARAPI SetWeekDayTextAppearance(
            /* [in] */ Int32 resourceId);

        CARAPI GetWeekDayTextAppearance(
            /* [out] */ Int32* result);

        CARAPI SetDateTextAppearance(
            /* [in] */ Int32 resourceId);

        CARAPI GetDateTextAppearance(
            /* [out] */ Int32* result);

        CARAPI SetEnabled(
            /* [in] */ Boolean enabled);

        CARAPI IsEnabled(
            /* [out] */ Boolean* result);

        CARAPI SetMinDate(
            /* [in] */ Int64 minDate);

        CARAPI GetMinDate(
            /* [out] */ Int64* result);

        CARAPI SetMaxDate(
            /* [in] */ Int64 maxDate);

        CARAPI GetMaxDate(
            /* [out] */ Int64* result);

        CARAPI SetShowWeekNumber(
            /* [in] */ Boolean showWeekNumber);

        CARAPI GetShowWeekNumber(
            /* [out] */ Boolean* result);

        CARAPI SetFirstDayOfWeek(
            /* [in] */ Int32 firstDayOfWeek);

        CARAPI GetFirstDayOfWeek(
            /* [out] */ Int32* result);

        CARAPI SetDate(
            /* [in] */ Int64 date);

        CARAPI SetDate(
            /* [in] */ Int64 date,
            /* [in] */ Boolean animate,
            /* [in] */ Boolean center);

        CARAPI GetDate(
            /* [out] */ Int64* result);

        CARAPI SetOnDateChangeListener(
            /* [in] */ IOnDateChangeListener* listener);

        CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* newConfig);

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

    private:
        CARAPI_(void) UpdateDateTextSize();

        /**
         * Invalidates all week views.
         */
        CARAPI_(void) InvalidateAllWeekViews();

        /**
         * Gets a calendar for locale bootstrapped with the value of a given calendar.
         *
         * @param oldCalendar The old calendar.
         * @param locale The locale.
         */
        static CARAPI_(AutoPtr<ICalendar>) GetCalendarForLocale(
            /* [in] */ ICalendar* oldCalendar,
            /* [in] */ ILocale* locale);

        /**
         * @return True if the <code>firstDate</code> is the same as the <code>
         * secondDate</code>.
         */
        static CARAPI_(Boolean) IsSameDate(
            /* [in] */ ICalendar* firstDate,
            /* [in] */ ICalendar* secondDate);

        /**
         * Creates a new adapter if necessary and sets up its parameters.
         */
        CARAPI_(void) SetUpAdapter();

        /**
         * Sets up the strings to be used by the header.
         */
        CARAPI_(void) SetUpHeader();

        /**
         * Sets all the required fields for the list view.
         */
        CARAPI_(void) SetUpListView();

        /**
         * This moves to the specified time in the view. If the time is not already
         * in range it will move the list so that the first of the month containing
         * the time is at the top of the view. If the new time is already in view
         * the list will not be scrolled unless forceScroll is true. This time may
         * optionally be highlighted as selected as well.
         *
         * @param date The time to move to.
         * @param animate Whether to scroll to the given time or just redraw at the
         *            new location.
         * @param setSelected Whether to set the given time as selected.
         * @param forceScroll Whether to recenter even if the time is already
         *            visible.
         *
         * @throws IllegalArgumentException of the provided date is before the
         *        range start of after the range end.
         */
        CARAPI_(void) GoTo(
            /* [in] */ ICalendar* date,
            /* [in] */ Boolean animate,
            /* [in] */ Boolean setSelected,
            /* [in] */ Boolean forceScroll);

        /**
         * Parses the given <code>date</code> and in case of success sets
         * the result to the <code>outDate</code>.
         *
         * @return True if the date was parsed.
         */
        CARAPI_(Boolean) ParseDate(
            /* [in] */ const String& date,
            /* [in] */ ICalendar* outDate);

        /**
         * Called when a <code>view</code> transitions to a new <code>scrollState
         * </code>.
         */
        CARAPI_(void) OnScrollStateChanged(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 scrollState);

        /**
         * Updates the title and selected month if the <code>view</code> has moved to a new
         * month.
         */
        CARAPI_(void) OnScroll(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 firstVisibleItem,
            /* [in] */ Int32 visibleItemCount,
            /* [in] */ Int32 totalItemCount);

        /**
         * Sets the month displayed at the top of this view based on time. Override
         * to add custom events when the title is changed.
         *
         * @param calendar A day in the new focus month.
         */
        CARAPI_(void) SetMonthDisplayed(
            /* [in] */ ICalendar* calendar);

        /**
         * @return Returns the number of weeks between the current <code>date</code>
         *         and the <code>mMinDate</code>.
         */
        CARAPI_(Int32) GetWeeksSinceMinDate(
            /* [in] */ ICalendar* date);

    private:
        /**
         * Default value whether to show week number.
         */
        static const Boolean DEFAULT_SHOW_WEEK_NUMBER;

        /**
         * The number of milliseconds in a day.e
         */
        static const Int64 MILLIS_IN_DAY;

        /**
         * The number of day in a week.
         */
        static const Int32 DAYS_PER_WEEK;

        /**
         * The number of milliseconds in a week.
         */
        static const Int64 MILLIS_IN_WEEK;

        /**
         * Affects when the month selection will change while scrolling upe
         */
        static const Int32 SCROLL_HYST_WEEKS;

        /**
         * How Int64 the GoTo fling animation should last.
         */
        static const Int32 GOTO_SCROLL_DURATION;

        /**
         * The duration of the adjustment upon a user scroll in milliseconds.
         */
        static const Int32 ADJUSTMENT_SCROLL_DURATION;

        /**
         * How Int64 to wait after receiving an onScrollStateChanged notification
         * before acting on it.
         */
        static const Int32 SCROLL_CHANGE_DELAY;

        /**
         * String for parsing dates.
         */
        static const String DATE_FORMAT;

        /**
         * The default minimal date.
         */
        static const String DEFAULT_MIN_DATE;

        /**
         * The default maximal date.
         */
        static const String DEFAULT_MAX_DATE;

        static const Int32 DEFAULT_SHOWN_WEEK_COUNT;

        static const Int32 DEFAULT_DATE_TEXT_SIZE;

        static const Int32 UNSCALED_SELECTED_DATE_VERTICAL_BAR_WIDTH;

        static const Int32 UNSCALED_WEEK_MIN_VISIBLE_HEIGHT;

        static const Int32 UNSCALED_LIST_SCROLL_TOP_OFFSET;

        static const Int32 UNSCALED_BOTTOM_BUFFER;

        static const Int32 UNSCALED_WEEK_SEPARATOR_LINE_WIDTH;

        static const Int32 DEFAULT_WEEK_DAY_TEXT_APPEARANCE_RES_ID;

        Int32 mWeekSeperatorLineWidth;

        Int32 mDateTextSize;

        AutoPtr<IDrawable> mSelectedDateVerticalBar;

        Int32 mSelectedDateVerticalBarWidth;

        Int32 mSelectedWeekBackgroundColor;

        Int32 mFocusedMonthDateColor;

        Int32 mUnfocusedMonthDateColor;

        Int32 mWeekSeparatorLineColor;

        Int32 mWeekNumberColor;

        Int32 mWeekDayTextAppearanceResId;

        Int32 mDateTextAppearanceResId;

        /**
         * The top offset of the weeks list.
         */
        Int32 mListScrollTopOffset;

        /**
         * The visible height of a week view.
         */
        Int32 mWeekMinVisibleHeight;

        /**
         * The visible height of a week view.
         */
        Int32 mBottomBuffer;

        /**
         * The number of shown weeks.
         */
        Int32 mShownWeekCount;

        /**
         * Flag whether to show the week number.
         */
        Boolean mShowWeekNumber;

        /**
         * The number of day per week to be shown.
         */
        Int32 mDaysPerWeek;

        /**
         * The friction of the week list while flinging.
         */
        Float mFriction;

        /**
         * Scale for adjusting velocity of the week list while flinging.
         */
        Float mVelocityScale;

        /**
         * The adapter for the weeks list.
         */
        AutoPtr<WeeksAdapter> mAdapter;

        /**
         * The weeks list.
         */
        AutoPtr<IListView> mListView;

        /**
         * The name of the month to display.
         */
        AutoPtr<ITextView> mMonthName;

        /**
         * The header with week day names.
         */
        AutoPtr<IViewGroup> mDayNamesHeader;

        /**
         * Cached labels for the week names header.
         */
        AutoPtr<ArrayOf<String> > mDayLabels;

        /**
         * The first day of the week.
         */
        Int32 mFirstDayOfWeek;

        /**
         * Which month should be displayed/highlighted [0-11].
         */
        Int32 mCurrentMonthDisplayed;

        /**
         * Used for tracking during a scroll.
         */
        Int64 mPreviousScrollPosition;

        /**
         * Used for tracking which direction the view is scrolling.
         */
        Boolean mIsScrollingUp;

        /**
         * The previous scroll state of the weeks ListView.
         */
        Int32 mPreviousScrollState;

        /**
         * The current scroll state of the weeks ListView.
         */
        Int32 mCurrentScrollState;

        /**
         * Listener for changes in the selected day.
         */
        AutoPtr<IOnDateChangeListener> mOnDateChangeListener;

        /**
         * Command for adjusting the position after a scroll/fling.
         */
        AutoPtr<ScrollStateRunnable> mScrollStateChangedRunnable;

        /**
         * Temporary instance to avoid multiple instantiations.
         */
        AutoPtr<ICalendar> mTempDate;

        /**
         * The first day of the focused month.
         */
        AutoPtr<ICalendar> mFirstDayOfMonth;

        /**
         * The start date of the range supported by this picker.
         */
        AutoPtr<ICalendar> mMinDate;

        /**
         * The end date of the range supported by this picker.
         */
        AutoPtr<ICalendar> mMaxDate;

        /**
         * Date format for parsing dates.
         */
        AutoPtr<IDateFormat> mDateFormat;
    };

public:
    CAR_INTERFACE_DECL()

    CalendarView();

    ~CalendarView();

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

    /**
     * Sets the number of weeks to be shown.
     *
     * @param count The shown week count.
     *
     * @attr ref android.R.styleable#CalendarView_shownWeekCount
     */
    CARAPI SetShownWeekCount(
        /* [in] */ Int32 count);

    /**
     * Gets the number of weeks to be shown.
     *
     * @return The shown week count.
     *
     * @attr ref android.R.styleable#CalendarView_shownWeekCount
     */
    CARAPI GetShownWeekCount(
        /* [out] */ Int32* result);

    /**
     * Sets the background color for the selected week.
     *
     * @param color The week background color.
     *
     * @attr ref android.R.styleable#CalendarView_selectedWeekBackgroundColor
     */
    CARAPI SetSelectedWeekBackgroundColor(
        /* [in] */ Int32 color);

    /**
     * Gets the background color for the selected week.
     *
     * @return The week background color.
     *
     * @attr ref android.R.styleable#CalendarView_selectedWeekBackgroundColor
     */
    CARAPI GetSelectedWeekBackgroundColor(
        /* [out] */ Int32* result);

    /**
     * Sets the color for the dates of the focused month.
     *
     * @param color The focused month date color.
     *
     * @attr ref android.R.styleable#CalendarView_focusedMonthDateColor
     */
    CARAPI SetFocusedMonthDateColor(
        /* [in] */ Int32 color);

    /**
     * Gets the color for the dates in the focused month.
     *
     * @return The focused month date color.
     *
     * @attr ref android.R.styleable#CalendarView_focusedMonthDateColor
     */
    CARAPI GetFocusedMonthDateColor(
        /* [out] */ Int32* result);

    /**
     * Sets the color for the dates of a not focused month.
     *
     * @param color A not focused month date color.
     *
     * @attr ref android.R.styleable#CalendarView_unfocusedMonthDateColor
     */
    CARAPI SetUnfocusedMonthDateColor(
        /* [in] */ Int32 color);

    /**
     * Gets the color for the dates in a not focused month.
     *
     * @return A not focused month date color.
     *
     * @attr ref android.R.styleable#CalendarView_unfocusedMonthDateColor
     */
    CARAPI GetUnfocusedMonthDateColor(
        /* [out] */ Int32* result);

    /**
     * Sets the color for the week numbers.
     *
     * @param color The week number color.
     *
     * @attr ref android.R.styleable#CalendarView_weekNumberColor
     */
    CARAPI SetWeekNumberColor(
        /* [in] */ Int32 color);

    /**
     * Gets the color for the week numbers.
     *
     * @return The week number color.
     *
     * @attr ref android.R.styleable#CalendarView_weekNumberColor
     */
    CARAPI GetWeekNumberColor(
        /* [out] */ Int32* result);

    /**
     * Sets the color for the separator line between weeks.
     *
     * @param color The week separator color.
     *
     * @attr ref android.R.styleable#CalendarView_weekSeparatorLineColor
     */
    CARAPI SetWeekSeparatorLineColor(
        /* [in] */ Int32 color);

    /**
     * Gets the color for the separator line between weeks.
     *
     * @return The week separator color.
     *
     * @attr ref android.R.styleable#CalendarView_weekSeparatorLineColor
     */
    CARAPI GetWeekSeparatorLineColor(
        /* [out] */ Int32* result);

    /**
     * Sets the drawable for the vertical bar shown at the beginning and at
     * the end of the selected date.
     *
     * @param resourceId The vertical bar drawable resource id.
     *
     * @attr ref android.R.styleable#CalendarView_selectedDateVerticalBar
     */
    CARAPI SetSelectedDateVerticalBar(
        /* [in] */ Int32 resourceId);

    /**
     * Sets the drawable for the vertical bar shown at the beginning and at
     * the end of the selected date.
     *
     * @param drawable The vertical bar drawable.
     *
     * @attr ref android.R.styleable#CalendarView_selectedDateVerticalBar
     */
    CARAPI SetSelectedDateVerticalBar(
        /* [in] */ IDrawable* drawable);

    /**
     * Gets the drawable for the vertical bar shown at the beginning and at
     * the end of the selected date.
     *
     * @return The vertical bar drawable.
     */
    CARAPI GetSelectedDateVerticalBar(
        /* [out] */ IDrawable** result);

    /**
     * Sets the text appearance for the week day abbreviation of the calendar header.
     *
     * @param resourceId The text appearance resource id.
     *
     * @attr ref android.R.styleable#CalendarView_weekDayTextAppearance
     */
    CARAPI SetWeekDayTextAppearance(
        /* [in] */ Int32 resourceId);

    /**
     * Gets the text appearance for the week day abbreviation of the calendar header.
     *
     * @return The text appearance resource id.
     *
     * @attr ref android.R.styleable#CalendarView_weekDayTextAppearance
     */
    CARAPI GetWeekDayTextAppearance(
        /* [out] */ Int32* result);

    /**
     * Sets the text appearance for the calendar dates.
     *
     * @param resourceId The text appearance resource id.
     *
     * @attr ref android.R.styleable#CalendarView_dateTextAppearance
     */
    CARAPI SetDateTextAppearance(
        /* [in] */ Int32 resourceId);

    /**
     * Gets the text appearance for the calendar dates.
     *
     * @return The text appearance resource id.
     *
     * @attr ref android.R.styleable#CalendarView_dateTextAppearance
     */
    CARAPI GetDateTextAppearance(
        /* [out] */ Int32* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    /**
     * Gets the minimal date supported by this {@link CalendarView} in milliseconds
     * since January 1, 1970 00:00:00 in {@link TimeZone#getDefault()} time
     * zone.
     * <p>
     * Note: The default minimal date is 01/01/1900.
     * <p>
     *
     * @return The minimal supported date.
     *
     * @attr ref android.R.styleable#CalendarView_minDate
     */
    CARAPI GetMinDate(
        /* [out] */ Int64* result);

    /**
     * Sets the minimal date supported by this {@link CalendarView} in milliseconds
     * since January 1, 1970 00:00:00 in {@link TimeZone#getDefault()} time
     * zone.
     *
     * @param minDate The minimal supported date.
     *
     * @attr ref android.R.styleable#CalendarView_minDate
     */
    CARAPI SetMinDate(
        /* [in] */ Int64 minDate);

    /**
     * Gets the maximal date supported by this {@link CalendarView} in milliseconds
     * since January 1, 1970 00:00:00 in {@link TimeZone#getDefault()} time
     * zone.
     * <p>
     * Note: The default maximal date is 01/01/2100.
     * <p>
     *
     * @return The maximal supported date.
     *
     * @attr ref android.R.styleable#CalendarView_maxDate
     */
    CARAPI GetMaxDate(
        /* [out] */ Int64* result);

    /**
     * Sets the maximal date supported by this {@link CalendarView} in milliseconds
     * since January 1, 1970 00:00:00 in {@link TimeZone#getDefault()} time
     * zone.
     *
     * @param maxDate The maximal supported date.
     *
     * @attr ref android.R.styleable#CalendarView_maxDate
     */
    CARAPI SetMaxDate(
        /* [in] */ Int64 maxDate);

    /**
     * Sets whether to show the week number.
     *
     * @param showWeekNumber True to show the week number.
     *
     * @attr ref android.R.styleable#CalendarView_showWeekNumber
     */
    CARAPI SetShowWeekNumber(
        /* [in] */ Boolean showWeekNumber);

    /**
     * Gets whether to show the week number.
     *
     * @return True if showing the week number.
     *
     * @attr ref android.R.styleable#CalendarView_showWeekNumber
     */
    CARAPI GetShowWeekNumber(
        /* [out] */ Boolean* result);

    /**
     * Gets the first day of week.
     *
     * @return The first day of the week conforming to the {@link CalendarView}
     *         APIs.
     * @see Calendar#MONDAY
     * @see Calendar#TUESDAY
     * @see Calendar#WEDNESDAY
     * @see Calendar#THURSDAY
     * @see Calendar#FRIDAY
     * @see Calendar#SATURDAY
     * @see Calendar#SUNDAY
     *
     * @attr ref android.R.styleable#CalendarView_firstDayOfWeek
     */
    CARAPI GetFirstDayOfWeek(
        /* [out] */ Int32* result);

    /**
     * Sets the first day of week.
     *
     * @param firstDayOfWeek The first day of the week conforming to the
     *            {@link CalendarView} APIs.
     * @see Calendar#MONDAY
     * @see Calendar#TUESDAY
     * @see Calendar#WEDNESDAY
     * @see Calendar#THURSDAY
     * @see Calendar#FRIDAY
     * @see Calendar#SATURDAY
     * @see Calendar#SUNDAY
     *
     * @attr ref android.R.styleable#CalendarView_firstDayOfWeek
     */
    CARAPI SetFirstDayOfWeek(
        /* [in] */ Int32 firstDayOfWeek);

    /**
     * Sets the listener to be notified upon selected date change.
     *
     * @param listener The listener to be notified.
     */
    CARAPI SetOnDateChangeListener(
        /* [in] */ IOnDateChangeListener* listener);

    /**
     * Gets the selected date in milliseconds since January 1, 1970 00:00:00 in
     * {@link TimeZone#getDefault()} time zone.
     *
     * @return The selected date.
     */
    CARAPI GetDate(
        /* [out] */ Int64* result);

    /**
     * Sets the selected date in milliseconds since January 1, 1970 00:00:00 in
     * {@link TimeZone#getDefault()} time zone.
     *
     * @param date The selected date.
     *
     * @throws IllegalArgumentException of the provided date is before the
     *        minimal or after the maximal date.
     *
     * @see #setDate(long, boolean, boolean)
     * @see #setMinDate(long)
     * @see #setMaxDate(long)
     */
    CARAPI SetDate(
        /* [in] */ Int64 date);

    /**
     * Sets the selected date in milliseconds since January 1, 1970 00:00:00 in
     * {@link TimeZone#getDefault()} time zone.
     *
     * @param date The date.
     * @param animate Whether to animate the scroll to the current date.
     * @param center Whether to center the current date even if it is already visible.
     *
     * @throws IllegalArgumentException of the provided date is before the
     *        minimal or after the maximal date.
     *
     * @see #setMinDate(long)
     * @see #setMaxDate(long)
     */
    CARAPI SetDate(
        /* [in] */ Int64 date,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean center);

    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    /**
     * Tag for logging.
     */
    static const String TAG;

    AutoPtr<ICalendarViewDelegate> mDelegate;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif  //__ELASTOS_DROID_WIDGET_CALENDARVIEW_H__
