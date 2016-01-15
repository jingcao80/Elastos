
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/widget/DayPickerView.h"
#include "elastos/droid/widget/SimpleMonthAdapter.h"
#include "elastos/droid/widget/SimpleMonthView.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringBuffer.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::Widget::SimpleMonthAdapter;
using Elastos::Droid::Widget::SimpleMonthView;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IStringBuffer;
using Elastos::Core::StringBuffer;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//                  DayPickerView::ScrollStateRunnable
//=====================================================================
CAR_INTERFACE_IMPL(DayPickerView::ScrollStateRunnable, Object, IRunnable)

DayPickerView::ScrollStateRunnable::ScrollStateRunnable(
    /* [in] */ DayPickerView* owner,
    /* [in] */ IView* view)
    : mOwner(owner)
    , mParent(view)
{
    // ==================before translated======================
    // mParent = view;
    assert(mOwner);
    assert(mParent);
}

ECode DayPickerView::ScrollStateRunnable::DoScrollStateChange(
    /* [in] */ AbsListView* view,
    /* [in] */ Int32 scrollState)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // mParent.removeCallbacks(this);
    // mNewState = scrollState;
    // mParent.postDelayed(this, SCROLL_CHANGE_DELAY);

    Boolean resTmp;
    mParent->RemoveCallbacks(this, &resTmp);
    mNewState = scrollState;
    mParent->PostDelayed(this, SCROLL_CHANGE_DELAY, &resTmp);
    return NOERROR;
}

ECode DayPickerView::ScrollStateRunnable::Run()
{
    // ==================before translated======================
    // mCurrentScrollState = mNewState;
    // if (Log.isLoggable(TAG, Log.DEBUG)) {
    //     Log.d(TAG,
    //             "new scroll state: " + mNewState + " old state: " + mPreviousScrollState);
    // }
    // // Fix the position after a scroll or a fling ends
    // if (mNewState == OnScrollListener.SCROLL_STATE_IDLE
    //         && mPreviousScrollState != OnScrollListener.SCROLL_STATE_IDLE
    //         && mPreviousScrollState != OnScrollListener.SCROLL_STATE_TOUCH_SCROLL) {
    //     mPreviousScrollState = mNewState;
    //     int i = 0;
    //     View child = getChildAt(i);
    //     while (child != null && child.getBottom() <= 0) {
    //         child = getChildAt(++i);
    //     }
    //     if (child == null) {
    //         // The view is no longer visible, just return
    //         return;
    //     }
    //     int firstPosition = getFirstVisiblePosition();
    //     int lastPosition = getLastVisiblePosition();
    //     boolean scroll = firstPosition != 0 && lastPosition != getCount() - 1;
    //     final int top = child.getTop();
    //     final int bottom = child.getBottom();
    //     final int midpoint = getHeight() / 2;
    //     if (scroll && top < LIST_TOP_OFFSET) {
    //         if (bottom > midpoint) {
    //             smoothScrollBy(top, GOTO_SCROLL_DURATION);
    //         } else {
    //             smoothScrollBy(bottom, GOTO_SCROLL_DURATION);
    //         }
    //     }
    // } else {
    //     mPreviousScrollState = mNewState;
    // }

    mOwner->mCurrentScrollState = mNewState;
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "new scroll state: %d old state: %d", mNewState, mOwner->mPreviousScrollState);
    }
    // Fix the position after a scroll or a fling ends
    if (mNewState == IAbsListViewOnScrollListener::SCROLL_STATE_IDLE
        && mOwner->mPreviousScrollState != IAbsListViewOnScrollListener::SCROLL_STATE_IDLE
        && mOwner->mPreviousScrollState != IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL) {
        mOwner->mPreviousScrollState = mNewState;
        int i = 0;
        AutoPtr<IView> child;
        mOwner->GetChildAt(i, (IView**)&child);
        Int32 bottom = 0;
        child->GetBottom(&bottom);
        while (child != NULL && bottom <= 0) {
            child = NULL;
            mOwner->GetChildAt(++i, (IView**)&child);
        }
        if (child == NULL) {
            // The view is no longer visible, just return
            return NOERROR;
        }

        Int32 firstPosition = 0, lastPosition = 0;
        mOwner->GetFirstVisiblePosition(&firstPosition);
        mOwner->GetLastVisiblePosition(&lastPosition);
        Int32 count = 0;
        mOwner->GetCount(&count);
        Boolean scroll = firstPosition != 0 && lastPosition != count - 1;
        Int32 top = 0, midpoint = 0;
        child->GetTop(&top);
        child->GetBottom(&bottom);
        mOwner->GetHeight(&midpoint);
        midpoint = midpoint / 2;
        if (scroll && top < LIST_TOP_OFFSET) {
            if (bottom > midpoint) {
                mOwner->SmoothScrollBy(top, GOTO_SCROLL_DURATION);
            }
            else {
                mOwner->SmoothScrollBy(bottom, GOTO_SCROLL_DURATION);
            }
        }
    }
    else {
        mOwner->mPreviousScrollState = mNewState;
    }
    return NOERROR;
}

//=====================================================================
//                    DayPickerView::InnerRunnable
//=====================================================================
CAR_INTERFACE_IMPL(DayPickerView::InnerRunnable, Object, IRunnable)

DayPickerView::InnerRunnable::InnerRunnable(
    /* [in] */ DayPickerView* owner,
    /* [in] */ Int32 position)
    : mOwner(owner)
    , mPosition(position)
{
    // ==================before translated======================
    // mOwner = owner;
    assert(mOwner);
}

ECode DayPickerView::InnerRunnable::Run()
{
    // ==================before translated======================
    // setSelection(position);

    mOwner->SetSelection(mPosition);
    return NOERROR;
}

//=====================================================================
//                            DayPickerView
//=====================================================================
const String DayPickerView::TAG("DayPickerView");
const Int32 DayPickerView::GOTO_SCROLL_DURATION;
const Int32 DayPickerView::SCROLL_CHANGE_DELAY;
const Int32 DayPickerView::LIST_TOP_OFFSET;

CAR_INTERFACE_IMPL_3(DayPickerView, ListView, IDayPickerView, IAbsListViewOnScrollListener, IOnDateChangedListener)

DayPickerView::DayPickerView()
    : mFriction(0.0f)
    , mCurrentMonthDisplayed(0)
    , mPreviousScrollState(0)
    , mCurrentScrollState(0)
    , mPerformingScroll(FALSE)
{
}

ECode DayPickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDatePickerController* controller)
{
    // ==================before translated======================
    // super(context);
    // init();
    // setController(controller);

    ListView::constructor(context);
    Init();
    SetController(controller);
    return NOERROR;
}

ECode DayPickerView::SetController(
    /* [in] */ IDatePickerController* controller)
{
    VALIDATE_NOT_NULL(controller);
    // ==================before translated======================
    // if (mController != null) {
    //     mController.unregisterOnDateChangedListener(this);
    // }
    // mController = controller;
    // mController.registerOnDateChangedListener(this);
    // setUpAdapter();
    // setAdapter(mAdapter);
    // onDateChanged();

    if (mController != NULL) {
        mController->UnregisterOnDateChangedListener(this);
    }
    mController = controller;
    mController->RegisterOnDateChangedListener(this);
    SetUpAdapter();
    SetAdapter(IAdapter::Probe(mAdapter));
    OnDateChanged();
    return NOERROR;
}

ECode DayPickerView::Init()
{
    // ==================before translated======================
    // setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
    // setDrawSelectorOnTop(false);
    //
    // setUpListView();

    AutoPtr<LayoutParams> layoutParams = new LayoutParams();
    layoutParams->constructor(IListPopupWindow::MATCH_PARENT, IListPopupWindow::MATCH_PARENT);
    SetLayoutParams(layoutParams);
    SetDrawSelectorOnTop(FALSE);
    SetUpListView();
    return NOERROR;
}

ECode DayPickerView::OnChange()
{
    // ==================before translated======================
    // setUpAdapter();
    // setAdapter(mAdapter);

    SetUpAdapter();
    SetAdapter(IAdapter::Probe(mAdapter));
    return NOERROR;
}

ECode DayPickerView::GoTo(
    /* [in] */ ICalendar* day,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean setSelected,
    /* [in] */ Boolean forceScroll,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(day);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    //
    // // Set the selected day
    // if (setSelected) {
    //     mSelectedDay.setTimeInMillis(day.getTimeInMillis());
    // }
    //
    // mTempDay.setTimeInMillis(day.getTimeInMillis());
    // final int position = getPositionFromDay(day);
    //
    // View child;
    // int i = 0;
    // int top = 0;
    // // Find a child that's completely in the view
    // do {
    //     child = getChildAt(i++);
    //     if (child == null) {
    //         break;
    //     }
    //     top = child.getTop();
    // } while (top < 0);
    //
    // // Compute the first and last position visible
    // int selectedPosition;
    // if (child != null) {
    //     selectedPosition = getPositionForView(child);
    // } else {
    //     selectedPosition = 0;
    // }
    //
    // if (setSelected) {
    //     mAdapter.setSelectedDay(mSelectedDay);
    // }
    //
    // // Check if the selected day is now outside of our visible range
    // // and if so scroll to the month that contains it
    // if (position != selectedPosition || forceScroll) {
    //     setMonthDisplayed(mTempDay);
    //     mPreviousScrollState = OnScrollListener.SCROLL_STATE_FLING;
    //     if (animate) {
    //         smoothScrollToPositionFromTop(
    //                 position, LIST_TOP_OFFSET, GOTO_SCROLL_DURATION);
    //         return true;
    //     } else {
    //         postSetSelection(position);
    //     }
    // } else if (setSelected) {
    //     setMonthDisplayed(mSelectedDay);
    // }
    // return false;

    // Set the selected day
    Int64 timeInMillis = 0;
    day->GetTimeInMillis(&timeInMillis);
    if (setSelected) {
        mSelectedDay->SetTimeInMillis(timeInMillis);
    }

    mTempDay->SetTimeInMillis(timeInMillis);
    Int32 position = GetPositionFromDay(day);

    AutoPtr<IView> child;
    Int32 i = 0;
    Int32 top = 0;
    // Find a child that's completely in the view
    do {
        GetChildAt(i++, (IView**)&child);
        if (child == NULL) {
            break;
        }
        child->GetTop(&top);
    } while (top < 0);

    // Compute the first and last position visible
    Int32 selectedPosition = 0;
    if (child != NULL) {
        GetPositionForView(child, &selectedPosition);
    }
    else {
        selectedPosition = 0;
    }

    if (setSelected) {
        mAdapter->SetSelectedDay(mSelectedDay);
    }

    // Check if the selected day is now outside of our visible range
    // and if so scroll to the month that contains it
    if (position != selectedPosition || forceScroll) {
        SetMonthDisplayed(mTempDay);
        mPreviousScrollState = IAbsListViewOnScrollListener::SCROLL_STATE_FLING;
        if (animate) {
            SmoothScrollToPositionFromTop(position, LIST_TOP_OFFSET, GOTO_SCROLL_DURATION);
            *result = TRUE;
            return NOERROR;
        }
        else {
            PostSetSelection(position);
        }
    }
    else if (setSelected) {
        SetMonthDisplayed(mSelectedDay);
    }
    *result = FALSE;
    return NOERROR;
}

ECode DayPickerView::PostSetSelection(
    /* [in] */ Int32 position)
{
    // ==================before translated======================
    // clearFocus();
    // post(new Runnable() {
    //
    //     @Override
    //     public void run() {
    //         setSelection(position);
    //     }
    // });
    // onScrollStateChanged(this, OnScrollListener.SCROLL_STATE_IDLE);

    ClearFocus();
    AutoPtr<IRunnable> runnable = new InnerRunnable(this, position);
    Boolean resTmp;
    Post(runnable, &resTmp);
    OnScrollStateChanged(this, IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
    return NOERROR;
}

ECode DayPickerView::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // SimpleMonthView child = (SimpleMonthView) view.getChildAt(0);
    // if (child == null) {
    //     return;
    // }
    //
    // mPreviousScrollState = mCurrentScrollState;

    AutoPtr<IView> viewTmp;
    (IViewGroup::Probe(view))->GetChildAt(0, (IView**)&viewTmp);
    SimpleMonthView* child = (SimpleMonthView*)viewTmp.Get();
    if (child == NULL) {
        return NOERROR;
    }
    mPreviousScrollState = mCurrentScrollState;
    return NOERROR;
}

ECode DayPickerView::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    VALIDATE_NOT_NULL(view);
    // ==================before translated======================
    // // use a post to prevent re-entering onScrollStateChanged before it
    // // exits
    // mScrollStateChangedRunnable.doScrollStateChange(view, scrollState);

    mScrollStateChangedRunnable->DoScrollStateChange((AbsListView*)view, scrollState);
    return NOERROR;
}

ECode DayPickerView::SetCalendarTextColor(
    /* [in] */ IColorStateList* colors)
{
    VALIDATE_NOT_NULL(colors);
    // ==================before translated======================
    // mAdapter.setCalendarTextColor(colors);

    mAdapter->SetCalendarTextColor(colors);
    return NOERROR;
}

ECode DayPickerView::GetMostVisiblePosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final int firstPosition = getFirstVisiblePosition();
    // final int height = getHeight();
    //
    // int maxDisplayedHeight = 0;
    // int mostVisibleIndex = 0;
    // int i=0;
    // int bottom = 0;
    // while (bottom < height) {
    //     View child = getChildAt(i);
    //     if (child == null) {
    //         break;
    //     }
    //     bottom = child.getBottom();
    //     int displayedHeight = Math.min(bottom, height) - Math.max(0, child.getTop());
    //     if (displayedHeight > maxDisplayedHeight) {
    //         mostVisibleIndex = i;
    //         maxDisplayedHeight = displayedHeight;
    //     }
    //     i++;
    // }
    // return firstPosition + mostVisibleIndex;

    Int32 firstPosition = 0, height = 0;
    GetFirstVisiblePosition(&firstPosition);
    GetHeight(&height);

    Int32 maxDisplayedHeight = 0;
    Int32 mostVisibleIndex = 0;
    Int32 i=0;
    Int32 top = 0;
    Int32 bottom = 0;
    Int32 displayedHeight = 0;
    while (bottom < height) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child == NULL) {
            break;
        }

        child->GetBottom(&bottom);
        child->GetTop(&top);
        displayedHeight = Elastos::Core::Math::Min(bottom, height) -
            Elastos::Core::Math::Max(0, top);
        if (displayedHeight > maxDisplayedHeight) {
            mostVisibleIndex = i;
            maxDisplayedHeight = displayedHeight;
        }
        ++i;
    }

    *result = firstPosition + mostVisibleIndex;
    return NOERROR;
}

ECode DayPickerView::OnDateChanged()
{
    // ==================before translated======================
    // goTo(mController.getSelectedDay(), false, true, true);

    AutoPtr<ICalendar> calendar;
    mController->GetSelectedDay((ICalendar**)&calendar);
    Boolean resTmp;
    GoTo(calendar, FALSE, TRUE, TRUE, &resTmp);
    return NOERROR;
}

ECode DayPickerView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // super.onInitializeAccessibilityEvent(event);
    // event.setItemCount(-1);

    ListView::OnInitializeAccessibilityEvent(event);
    (IAccessibilityRecord::Probe(event))->SetItemCount(-1);
    return NOERROR;
}

ECode DayPickerView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // super.onInitializeAccessibilityNodeInfo(info);
    // info.addAction(AccessibilityNodeInfo.ACTION_SCROLL_FORWARD);
    // info.addAction(AccessibilityNodeInfo.ACTION_SCROLL_BACKWARD);

    ListView::OnInitializeAccessibilityNodeInfo(info);
    info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
    info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
    return NOERROR;
}

Boolean DayPickerView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    // ==================before translated======================
    // if (action != AccessibilityNodeInfo.ACTION_SCROLL_FORWARD &&
    //         action != AccessibilityNodeInfo.ACTION_SCROLL_BACKWARD) {
    //     return super.performAccessibilityAction(action, arguments);
    // }
    //
    // // Figure out what month is showing.
    // int firstVisiblePosition = getFirstVisiblePosition();
    // int month = firstVisiblePosition % 12;
    // int year = firstVisiblePosition / 12 + mController.getMinYear();
    // Calendar day = Calendar.getInstance();
    // day.set(year, month, 1);
    //
    // // Scroll either forward or backward one month.
    // if (action == AccessibilityNodeInfo.ACTION_SCROLL_FORWARD) {
    //     day.add(Calendar.MONTH, 1);
    //     if (day.get(Calendar.MONTH) == 12) {
    //         day.set(Calendar.MONTH, 0);
    //         day.add(Calendar.YEAR, 1);
    //     }
    // } else if (action == AccessibilityNodeInfo.ACTION_SCROLL_BACKWARD) {
    //     View firstVisibleView = getChildAt(0);
    //     // If the view is fully visible, jump one month back. Otherwise, we'll just jump
    //     // to the first day of first visible month.
    //     if (firstVisibleView != null && firstVisibleView.getTop() >= -1) {
    //         // There's an off-by-one somewhere, so the top of the first visible item will
    //         // actually be -1 when it's at the exact top.
    //         day.add(Calendar.MONTH, -1);
    //         if (day.get(Calendar.MONTH) == -1) {
    //             day.set(Calendar.MONTH, 11);
    //             day.add(Calendar.YEAR, -1);
    //         }
    //     }
    // }
    //
    // // Go to that month.
    // announceForAccessibility(getMonthAndYearString(day));
    // goTo(day, true, false, true);
    // mPerformingScroll = true;
    // return true;

    if (action != IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD &&
            action != IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD) {
        Boolean resTmp = FALSE;
        ListView::PerformAccessibilityAction(action, arguments, &resTmp);
        return resTmp;
    }

    // Figure out what month is showing.
    Int32 firstVisiblePosition = 0;
    GetFirstVisiblePosition(&firstVisiblePosition);
    Int32 month = firstVisiblePosition % 12;
    Int32 minYear = 0;
    mController->GetMinYear(&minYear);
    Int32 year = firstVisiblePosition / 12 + minYear;

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> day;
    helper->GetInstance((ICalendar**)&day);
    day->Set(year, month, 1);

    // Scroll either forward or backward one month.
    if (action == IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD) {
        day->Add(ICalendar::MONTH, 1);
        Int32 monthTmp = 0;
        day->Get(ICalendar::MONTH, &monthTmp);
        if (monthTmp == 12) {
            day->Set(ICalendar::MONTH, 0);
            day->Add(ICalendar::YEAR, 1);
        }
    }
    else if (action == IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD) {
        AutoPtr<IView> firstVisibleView;
        GetChildAt(0, (IView**)&firstVisibleView);
        // If the view is fully visible, jump one month back. Otherwise, we'll just jump
        // to the first day of first visible month.
        Int32 topTmp = 0;
        firstVisibleView->GetTop(&topTmp);
        if (firstVisibleView != NULL && topTmp >= -1) {
            // There's an off-by-one somewhere, so the top of the first visible item will
            // actually be -1 when it's at the exact top.
            day->Add(ICalendar::MONTH, -1);
            Int32 monthTmp = 0;
            day->Get(ICalendar::MONTH, &monthTmp);
            if (monthTmp == -1) {
                day->Set(ICalendar::MONTH, 11);
                day->Add(ICalendar::YEAR, -1);
            }
        }
    }

    // Go to that month.
    String dayCalendar = GetMonthAndYearString(day);
    AutoPtr<ICharSequence> charSequence;
    CString::New(dayCalendar, (ICharSequence**)&charSequence);
    AnnounceForAccessibility(charSequence);
    Boolean resTmp;
    GoTo(day, TRUE, FALSE, TRUE, &resTmp);
    mPerformingScroll = TRUE;
    return TRUE;
}

void DayPickerView::SetUpAdapter()
{
    // ==================before translated======================
    // if (mAdapter == null) {
    //     mAdapter = new SimpleMonthAdapter(getContext(), mController);
    // } else {
    //     mAdapter.setSelectedDay(mSelectedDay);
    //     mAdapter.notifyDataSetChanged();
    // }
    // // refresh the view with the new parameters
    // mAdapter.notifyDataSetChanged();

    if (mAdapter == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        mAdapter = new SimpleMonthAdapter();
        ((SimpleMonthAdapter*)mAdapter.Get())->constructor(context, mController);
    }
    else {
        mAdapter->SetSelectedDay(mSelectedDay);
        ((SimpleMonthAdapter*)mAdapter.Get())->NotifyDataSetChanged();
    }
    // refresh the view with the new parameters
    ((SimpleMonthAdapter*)mAdapter.Get())->NotifyDataSetChanged();
}

void DayPickerView::SetUpListView()
{
    // ==================before translated======================
    // // Transparent background on scroll
    // setCacheColorHint(0);
    // // No dividers
    // setDivider(null);
    // // Items are clickable
    // setItemsCanFocus(true);
    // // The thumb gets in the way, so disable it
    // setFastScrollEnabled(false);
    // setVerticalScrollBarEnabled(false);
    // setOnScrollListener(this);
    // setFadingEdgeLength(0);
    // // Make the scrolling behavior nicer
    // setFriction(ViewConfiguration.getScrollFriction() * mFriction);

    // Transparent background on scroll
    SetCacheColorHint(0);
    // No dividers
    SetDivider(NULL);
    // Items are clickable
    SetItemsCanFocus(TRUE);
    // The thumb gets in the way, so disable it
    SetFastScrollEnabled(FALSE);
    SetVerticalScrollBarEnabled(FALSE);
    SetOnScrollListener(this);
    SetFadingEdgeLength(0);
    // Make the scrolling behavior nicer
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    Float scrollFriction = 0.0f;
    helper->GetScrollFriction(&scrollFriction);
    SetFriction(scrollFriction * mFriction);
}

void DayPickerView::SetMonthDisplayed(
    /* [in] */ ICalendar* date)
{
    // ==================before translated======================
    // if (mCurrentMonthDisplayed != date.get(Calendar.MONTH)) {
    //     mCurrentMonthDisplayed = date.get(Calendar.MONTH);
    //     invalidateViews();
    // }

    Int32 month = 0;
    date->Get(ICalendar::MONTH, &month);
    if (mCurrentMonthDisplayed != month) {
        mCurrentMonthDisplayed = month;
        InvalidateViews();
    }
}

ECode DayPickerView::LayoutChildren()
{
    // ==================before translated======================
    // final Calendar focusedDay = findAccessibilityFocus();
    // super.layoutChildren();
    // if (mPerformingScroll) {
    //     mPerformingScroll = false;
    // } else {
    //     restoreAccessibilityFocus(focusedDay);
    // }

    AutoPtr<ICalendar> focusedDay = FindAccessibilityFocus();
    ListView::LayoutChildren();
    if (mPerformingScroll) {
        mPerformingScroll = FALSE;
    }
    else {
        RestoreAccessibilityFocus(focusedDay);
    }
    return NOERROR;
}

void DayPickerView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    // ==================before translated======================
    // mYearFormat = new SimpleDateFormat("yyyy", Locale.getDefault());

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    CSimpleDateFormat::New(String("yyyy"), locale, (ISimpleDateFormat**)&mYearFormat);
}

Int32 DayPickerView::GetDiffMonths(
    /* [in] */ ICalendar* start,
    /* [in] */ ICalendar* end)
{
    // ==================before translated======================
    // final int diffYears = end.get(Calendar.YEAR) - start.get(Calendar.YEAR);
    // final int diffMonths = end.get(Calendar.MONTH) - start.get(Calendar.MONTH) + 12 * diffYears;
    // return diffMonths;

    Int32 diffYears = 0, diffMonths = 0;
    Int32 tmp1 = 0, tmp2 = 0;
    end->Get(ICalendar::YEAR, &tmp1);
    start->Get(ICalendar::YEAR, &tmp2);
    diffYears = tmp1 - tmp2;

    end->Get(ICalendar::MONTH, &tmp1);
    start->Get(ICalendar::MONTH, &tmp2);
    diffMonths = tmp1 - tmp2 + 12 * diffYears;
    return diffMonths;
}

Int32 DayPickerView::GetPositionFromDay(
    /* [in] */ ICalendar* day)
{
    // ==================before translated======================
    // final int diffMonthMax = getDiffMonths(mController.getMinDate(), mController.getMaxDate());
    // int diffMonth = getDiffMonths(mController.getMinDate(), day);
    //
    // if (diffMonth < 0 ) {
    //     diffMonth = 0;
    // } else if (diffMonth > diffMonthMax) {
    //     diffMonth = diffMonthMax;
    // }
    //
    // return diffMonth;

    AutoPtr<ICalendar> minDate;
    AutoPtr<ICalendar> maxDate;
    mController->GetMinDate((ICalendar**)&minDate);
    mController->GetMaxDate((ICalendar**)&maxDate);
    Int32 diffMonthMax = GetDiffMonths(minDate, maxDate);
    Int32 diffMonth = GetDiffMonths(minDate, day);

    if (diffMonth < 0 ) {
        diffMonth = 0;
    }
    else if (diffMonth > diffMonthMax) {
        diffMonth = diffMonthMax;
    }
    return diffMonth;
}

AutoPtr<ICalendar> DayPickerView::FindAccessibilityFocus()
{
    // ==================before translated======================
    // final int childCount = getChildCount();
    // for (int i = 0; i < childCount; i++) {
    //     final View child = getChildAt(i);
    //     if (child instanceof SimpleMonthView) {
    //         final Calendar focus = ((SimpleMonthView) child).getAccessibilityFocus();
    //         if (focus != null) {
    //             return focus;
    //         }
    //     }
    // }
    //
    // return null;

    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; ++i) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        ISimpleMonthView* monthView = ISimpleMonthView::Probe(child);
        if (monthView != NULL) {
            AutoPtr<ICalendar> focus;
            monthView->GetAccessibilityFocus((ICalendar**)&focus);
            if (focus != NULL) {
                return focus;
            }
        }
    }
    return NULL;
}

Boolean DayPickerView::RestoreAccessibilityFocus(
    /* [in] */ ICalendar* day)
{
    // ==================before translated======================
    // if (day == null) {
    //     return false;
    // }
    //
    // final int childCount = getChildCount();
    // for (int i = 0; i < childCount; i++) {
    //     final View child = getChildAt(i);
    //     if (child instanceof SimpleMonthView) {
    //         if (((SimpleMonthView) child).restoreAccessibilityFocus(day)) {
    //             return true;
    //         }
    //     }
    // }
    //
    // return false;

    if (day == NULL) {
        return FALSE;
    }

    Int32 childCount = 0;
    GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; ++i) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        ISimpleMonthView* monthView = ISimpleMonthView::Probe(child);
        if (monthView != NULL) {
            Boolean result = FALSE;
            monthView->RestoreAccessibilityFocus(day, &result);
            return result;
        }
    }
    return FALSE;
}

String DayPickerView::GetMonthAndYearString(
    /* [in] */ ICalendar* day)
{
    // ==================before translated======================
    // StringBuffer sbuf = new StringBuffer();
    // sbuf.append(day.getDisplayName(Calendar.MONTH, Calendar.LONG, Locale.getDefault()));
    // sbuf.append(" ");
    // sbuf.append(mYearFormat.format(day.getTime()));

    assert(0);
    AutoPtr<StringBuffer> sbuf = new StringBuffer();
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);

    String name;
    day->GetDisplayName(ICalendar::MONTH, ICalendar::LONG, locale, &name);
    sbuf->Append(name);
    sbuf->Append(" ");

    AutoPtr<IDate> dateTime;
    day->GetTime((IDate**)&dateTime);
    AutoPtr<IStringBuffer> yearFormat; // = mYearFormat.format(dateTime);
    sbuf->Append(yearFormat);
    String result;
    sbuf->ToString(&result);
    return result;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


