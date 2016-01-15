
#ifndef __ELASTOS_DROID_WIDGET_SIMPLEMONTHVIEW_H__
#define __ELASTOS_DROID_WIDGET_SIMPLEMONTHVIEW_H__

#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/widget/ExploreByTouchHelper.h"
#include "elastos/droid/view/View.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Widget::ExploreByTouchHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IAccessibilityDelegate;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IOnDayClickListener;
using Elastos::Droid::Widget::ISimpleMonthView;
using Elastos::Core::ICharSequence;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IFormatter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
  * A calendar-like view displaying a specified month and the appropriate selectable day numbers
  * within the specified month.
  */
class SimpleMonthView
    : public Elastos::Droid::View::View
    , public ISimpleMonthView
{
private:
    /**
      * Provides a virtual view hierarchy for interfacing with an accessibility
      * service.
      */
    class MonthViewTouchHelper
        : public ExploreByTouchHelper
    {
    public:
        MonthViewTouchHelper(
            /* [in] */ SimpleMonthView* owner);

        virtual CARAPI SetFocusedVirtualView(
            /* [in] */ Int32 virtualViewId);

        virtual CARAPI ClearFocusedVirtualView();

    protected:
        // @Override
        CARAPI_(Int32) GetVirtualViewAt(
            /* [in] */ Float x,
            /* [in] */ Float y);

        // @Override
        CARAPI_(void) GetVisibleVirtualViews(
            /* [in] */ IList* virtualViewIds);

        // @Override
        CARAPI_(void) OnPopulateEventForVirtualView(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ IAccessibilityEvent* event);

        // @Override
        CARAPI_(void) OnPopulateNodeForVirtualView(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ IAccessibilityNodeInfo* node);

        // @Override
        CARAPI_(Boolean) OnPerformActionForVirtualView(
            /* [in] */ Int32 virtualViewId,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments);

    private:
        /**
          * Calculates the bounding rectangle of a given time object.
          *
          * @param day The day to calculate bounds for
          * @param rect The rectangle in which to store the bounds
          */
        CARAPI GetItemBounds(
            /* [in] */ Int32 day,
            /* [in] */ IRect* rect);

        /**
          * Generates a description for a given time object. Since this
          * description will be spoken, the components are ordered by descending
          * specificity as DAY MONTH YEAR.
          *
          * @param day The day to generate a description for
          * @return A description of the time object
          */
        CARAPI_(AutoPtr<ICharSequence>) GetItemDescription(
            /* [in] */ Int32 day);

    private:
        SimpleMonthView* mOwner;
        static const String DATE_FORMAT;
        /*const*/ AutoPtr<IRect> mTempRect;
        /*const*/ AutoPtr<ICalendar> mTempCalendar;
    };

public:
    CAR_INTERFACE_DECL()

    SimpleMonthView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    virtual CARAPI SetTextColor(
        /* [in] */ IColorStateList* colors);

    // @Override
    CARAPI SetAccessibilityDelegate(
        /* [in] */ IAccessibilityDelegate* delegate);

    virtual CARAPI SetOnDayClickListener(
        /* [in] */ IOnDayClickListener* listener);

    // @Override
    CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    /**
      * Sets all the parameters for displaying this week. Parameters have a default value and
      * will only update if a new value is included, except for focus month, which will always
      * default to no focus month if no value is passed in. The only required parameter is the
      * week start.
      *
      * @param selectedDay the selected day of the month, or -1 for no selection.
      * @param month the month.
      * @param year the year.
      * @param weekStart which day the week should start on. {@link Calendar#SUNDAY} through
      *        {@link Calendar#SATURDAY}.
      * @param enabledDayStart the first enabled day.
      * @param enabledDayEnd the last enabled day.
      */
    virtual CARAPI SetMonthParams(
        /* [in] */ Int32 selectedDay,
        /* [in] */ Int32 month,
        /* [in] */ Int32 year,
        /* [in] */ Int32 weekStart,
        /* [in] */ Int32 enabledDayStart,
        /* [in] */ Int32 enabledDayEnd);

    virtual CARAPI Reuse();

    /**
      * @return The date that has accessibility focus, or {@code null} if no date
      *         has focus
      */
    virtual CARAPI GetAccessibilityFocus(
        /* [out] */ ICalendar** result);

    /**
      * Clears accessibility focus within the view. No-op if the view does not
      * contain accessibility focus.
      */
    virtual CARAPI ClearAccessibilityFocus();

    /**
      * Attempts to restore accessibility focus to the specified date.
      *
      * @param day The date which should receive focus
      * @return {@code false} if the date is not valid for this month view, or
      *         {@code true} if the date received focus
      */
    virtual CARAPI RestoreAccessibilityFocus(
        /* [in] */ ICalendar* day,
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    /**
      * Sets up the text and style properties for painting.
      */
    CARAPI InitView();

    static CARAPI_(Boolean) IsValidDayOfWeek(
        /* [in] */ Int32 day);

    static CARAPI_(Boolean) IsValidMonth(
        /* [in] */ Int32 month);

    static CARAPI_(Int32) GetDaysInMonth(
        /* [in] */ Int32 month,
        /* [in] */ Int32 year);

    CARAPI_(Int32) CalculateNumRows();

    CARAPI_(Boolean) SameDay(
        /* [in] */ Int32 day,
        /* [in] */ ITime* today);

    CARAPI_(String) GetMonthAndYearString();

    CARAPI_(void) DrawMonthTitle(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) DrawWeekDayLabels(
        /* [in] */ ICanvas* canvas);

    /**
      * Draws the month days.
      */
    CARAPI_(void) DrawDays(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Int32) FindDayOffset();

    /**
      * Calculates the day that the given x position is in, accounting for week
      * number. Returns the day or -1 if the position wasn't in a day.
      *
      * @param x The x position of the touch event
      * @return The day number, or -1 if the position wasn't in a day
      */
    CARAPI_(Int32) GetDayFromLocation(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
      * Called when the user clicks on a day. Handles callbacks to the
      * {@link OnDayClickListener} if one is set.
      *
      * @param day The day that was clicked
      */
    CARAPI_(void) OnDayClick(
        /* [in] */ Int32 day);

private:
    static const String TAG;
    static const Int32 DEFAULT_HEIGHT = 32;
    static const Int32 MIN_HEIGHT = 10;
    static const Int32 DEFAULT_SELECTED_DAY = -1;
    static const Int32 DEFAULT_WEEK_START = ICalendar::SUNDAY;
    static const Int32 DEFAULT_NUM_DAYS = 7;
    static const Int32 DEFAULT_NUM_ROWS = 6;
    static const Int32 MAX_NUM_ROWS = 6;
    static const Int32 SELECTED_CIRCLE_ALPHA = 60;
    static const Int32 DAY_SEPARATOR_WIDTH = 1;
    /*const*/ Int32 mMiniDayNumberTextSize;
    /*const*/ Int32 mMonthLabelTextSize;
    /*const*/ Int32 mMonthDayLabelTextSize;
    /*const*/ Int32 mMonthHeaderSize;
    /*const*/ Int32 mDaySelectedCircleSize;
    // used for scaling to the device density
    static Float mScale;
    /** Single-letter (when available) formatter for the day of week label. */
    AutoPtr<ISimpleDateFormat> mDayFormatter;
    // affects the padding on the sides of this view
    Int32 mPadding;
    String mDayOfWeekTypeface;
    String mMonthTitleTypeface;
    AutoPtr<IPaint> mDayNumberPaint;
    AutoPtr<IPaint> mDayNumberDisabledPaint;
    AutoPtr<IPaint> mDayNumberSelectedPaint;
    AutoPtr<IPaint> mMonthTitlePaint;
    AutoPtr<IPaint> mMonthDayLabelPaint;
    /*const*/ AutoPtr<IFormatter> mFormatter;
    /*const*/ AutoPtr<StringBuilder> mStringBuilder;
    Int32 mMonth;
    Int32 mYear;
    // Quick reference to the width of this view, matches parent
    Int32 mWidth;
    // The height this view should draw at in pixels, set by height param
    Int32 mRowHeight;
    // If this view contains the today
    Boolean mHasToday;
    // Which day is selected [0-6] or -1 if no day is selected
    Int32 mSelectedDay;
    // Which day is today [0-6] or -1 if no day is today
    Int32 mToday;
    // Which day of the week to start on [0-6]
    Int32 mWeekStart;
    // How many days to display
    Int32 mNumDays;
    // The number of days + a spot for week number if it is displayed
    Int32 mNumCells;
    Int32 mDayOfWeekStart;
    // First enabled day
    Int32 mEnabledDayStart;
    // Last enabled day
    Int32 mEnabledDayEnd;
    /*const*/ AutoPtr<ICalendar> mCalendar;
    /*const*/ AutoPtr<ICalendar> mDayLabelCalendar;
    /*const*/ AutoPtr<MonthViewTouchHelper> mTouchHelper;
    Int32 mNumRows;
    // Optional listener for handling day click actions
    AutoPtr<IOnDayClickListener> mOnDayClickListener;
    // Whether to prevent setting the accessibility delegate
    Boolean mLockAccessibilityDelegate;
    Int32 mNormalTextColor;
    Int32 mDisabledTextColor;
    Int32 mSelectedDayColor;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SIMPLEMONTHVIEW_H__

