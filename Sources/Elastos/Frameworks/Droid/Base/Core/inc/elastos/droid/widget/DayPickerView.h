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

#ifndef __ELASTOS_DROID_WIDGET_DAYPICKERVIEW_H__
#define __ELASTOS_DROID_WIDGET_DAYPICKERVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/widget/ListView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IDatePickerController;
using Elastos::Droid::Widget::IOnDateChangedListener;
using Elastos::Droid::Widget::ISimpleMonthAdapter;
using Elastos::Droid::Widget::ListView;
using Elastos::Core::IRunnable;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
  * This displays a list of months in a calendar format with selectable days.
  */
class DayPickerView
    : public ListView
    , public IDayPickerView
{
protected:
    class ScrollStateRunnable
        : public Runnable
    {
    public:
        ScrollStateRunnable(
            /* [in] */ DayPickerView* owner,
            /* [in] */ IView* view);

        ~ScrollStateRunnable();
        /**
          * Sets up the runnable with a short delay in case the scroll state
          * immediately changes again.
          *
          * @param view The list view that changed state
          * @param scrollState The new state it changed to
          */
        virtual CARAPI DoScrollStateChange(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 scrollState);

        // @Override
        CARAPI Run();

    private:
        DayPickerView* mOwner;
        Int32 mNewState;
        AutoPtr<IView> mParent;
    };

private:
    class InnerRunnable
        : public Runnable
    {
    public:
        InnerRunnable(
            /* [in] */ DayPickerView* owner,
            /* [in] */ Int32 position);

        ~InnerRunnable();

        // @Override
        CARAPI Run();

    private:
        DayPickerView* mOwner;
        Int32 mPosition;
    };

    class InnerListener
        : public Object
        , public IAbsListViewOnScrollListener
        , public IOnDateChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
          /* [in] */ DayPickerView* host);

        CARAPI OnScroll(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 firstVisibleItem,
            /* [in] */ Int32 visibleItemCount,
            /* [in] */ Int32 totalItemCount);

        // @Override
        CARAPI OnScrollStateChanged(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 scrollState);

        // @Override
        CARAPI OnDateChanged();

    private:
        DayPickerView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DayPickerView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDatePickerController* controller);

    virtual CARAPI SetController(
        /* [in] */ IDatePickerController* controller);

    virtual CARAPI Init();

    virtual CARAPI OnChange();

    /**
      * This moves to the specified time in the view. If the time is not already
      * in range it will move the list so that the first of the month containing
      * the time is at the top of the view. If the new time is already in view
      * the list will not be scrolled unless forceScroll is true. This time may
      * optionally be highlighted as selected as well.
      *
      * @param day The day to move to
      * @param animate Whether to scroll to the given time or just redraw at the
      *            new location
      * @param setSelected Whether to set the given time as selected
      * @param forceScroll Whether to recenter even if the time is already
      *            visible
      * @return Whether or not the view animated to the new location
      */
    virtual CARAPI GoTo(
        /* [in] */ ICalendar* day,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean setSelected,
        /* [in] */ Boolean forceScroll,
        /* [out] */ Boolean* result);

    virtual CARAPI PostSetSelection(
        /* [in] */ Int32 position);

    /**
      * Updates the title and selected month if the view has moved to a new
      * month.
      */
    // @Override
    CARAPI OnScroll(
        /* [in] */ IAbsListView* view,
        /* [in] */ Int32 firstVisibleItem,
        /* [in] */ Int32 visibleItemCount,
        /* [in] */ Int32 totalItemCount);

    // @Override
    CARAPI OnScrollStateChanged(
        /* [in] */ IAbsListView* view,
        /* [in] */ Int32 scrollState);

    virtual CARAPI SetCalendarTextColor(
        /* [in] */ IColorStateList* colors);

    /**
      * Gets the position of the view that is most prominently displayed within the list view.
      */
    virtual CARAPI GetMostVisiblePosition(
        /* [out] */ Int32* result);

    // @Override
    CARAPI OnDateChanged();

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
      * Necessary for accessibility, to ensure we support "scrolling" forward and backward
      * in the month list.
      */
    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
      * When scroll forward/backward events are received, announce the newly scrolled-to month.
      */
    // @Override
    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [out] */ Boolean* res);

protected:
    /**
      * Creates a new adapter if necessary and sets up its parameters. Override
      * this method to provide a custom adapter.
      */
    virtual CARAPI_(void) SetUpAdapter();

    /*
      * Sets all the required fields for the list view. Override this method to
      * set a different list view behavior.
      */
    virtual CARAPI_(void) SetUpListView();

    /**
      * Sets the month displayed at the top of this view based on time. Override
      * to add custom events when the title is changed.
      */
    virtual CARAPI_(void) SetMonthDisplayed(
        /* [in] */ ICalendar* date);

    // @Override
    CARAPI LayoutChildren();

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(Int32) GetDiffMonths(
        /* [in] */ ICalendar* start,
        /* [in] */ ICalendar* end);

    CARAPI_(Int32) GetPositionFromDay(
        /* [in] */ ICalendar* day);

    /**
      * Attempts to return the date that has accessibility focus.
      *
      * @return The date that has accessibility focus, or {@code null} if no date
      *         has focus.
      */
    CARAPI_(AutoPtr<ICalendar>) FindAccessibilityFocus();

    /**
      * Attempts to restore accessibility focus to a given date. No-op if
      * {@code day} is {@code null}.
      *
      * @param day The date that should receive accessibility focus
      * @return {@code true} if focus was restored
      */
    CARAPI_(Boolean) RestoreAccessibilityFocus(
        /* [in] */ ICalendar* day);

    CARAPI_(String) GetMonthAndYearString(
        /* [in] */ ICalendar* day);

private:
    static const String TAG;
    static const Int32 GOTO_SCROLL_DURATION = 250;
    static const Int32 SCROLL_CHANGE_DELAY = 40;
    static const Int32 LIST_TOP_OFFSET = -1;
    AutoPtr<ISimpleDateFormat> mYearFormat;
    Float mFriction;
    AutoPtr<ICalendar> mSelectedDay;
    AutoPtr<ISimpleMonthAdapter> mAdapter;
    AutoPtr<ICalendar> mTempDay;
    Int32 mCurrentMonthDisplayed;
    Int32 mPreviousScrollState;
    Int32 mCurrentScrollState;
    AutoPtr<IDatePickerController> mController;
    Boolean mPerformingScroll;
    AutoPtr<ScrollStateRunnable> mScrollStateChangedRunnable;
    AutoPtr<InnerListener> mListener;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_DAYPICKERVIEW_H__

