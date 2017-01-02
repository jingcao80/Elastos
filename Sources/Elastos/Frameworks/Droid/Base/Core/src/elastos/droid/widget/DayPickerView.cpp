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

#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/widget/DayPickerView.h"
#include "elastos/droid/widget/SimpleMonthAdapter.h"
#include "elastos/droid/widget/SimpleMonthView.h"
#include "elastos/droid/widget/CAbsListViewLayoutParams.h"
#include "elastos/droid/widget/CSimpleMonthAdapter.h"
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
//                  DayPickerView::InnerListener
//=====================================================================
CAR_INTERFACE_IMPL_2(DayPickerView::InnerListener, Object, \
    IAbsListViewOnScrollListener, IOnDateChangedListener)

DayPickerView::InnerListener::InnerListener(
    /* [in] */ DayPickerView* host)
    : mHost(host)
{
}

ECode DayPickerView::InnerListener::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    return mHost->OnScroll(view, firstVisibleItem, visibleItemCount, totalItemCount);
}

// @Override
ECode DayPickerView::InnerListener::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    return mHost->OnScrollStateChanged(view, scrollState);
}

// @Override
ECode DayPickerView::InnerListener::OnDateChanged()
{
    return mHost->OnDateChanged();
}

//=====================================================================
//                  DayPickerView::ScrollStateRunnable
//=====================================================================
DayPickerView::ScrollStateRunnable::ScrollStateRunnable(
    /* [in] */ DayPickerView* owner,
    /* [in] */ IView* view)
    : mOwner(owner)
    , mNewState(0)
    , mParent(view)
{
    assert(mOwner);
    assert(mParent);
}

DayPickerView::ScrollStateRunnable::~ScrollStateRunnable()
{}

ECode DayPickerView::ScrollStateRunnable::DoScrollStateChange(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    VALIDATE_NOT_NULL(view);

    Boolean resTmp;
    mParent->RemoveCallbacks(this, &resTmp);
    mNewState = scrollState;
    mParent->PostDelayed(this, SCROLL_CHANGE_DELAY, &resTmp);
    return NOERROR;
}

ECode DayPickerView::ScrollStateRunnable::Run()
{
    mOwner->mCurrentScrollState = mNewState;
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "new scroll state: %d old state: %d", mNewState, mOwner->mPreviousScrollState);
    }
    // Fix the position after a scroll or a fling ends
    if (mNewState == IAbsListViewOnScrollListener::SCROLL_STATE_IDLE
        && mOwner->mPreviousScrollState != IAbsListViewOnScrollListener::SCROLL_STATE_IDLE
        && mOwner->mPreviousScrollState != IAbsListViewOnScrollListener::SCROLL_STATE_TOUCH_SCROLL) {
        mOwner->mPreviousScrollState = mNewState;
        Int32 i = 0;
        AutoPtr<IView> child;
        mOwner->GetChildAt(i, (IView**)&child);
        Int32 bottom = 0;
        while (child != NULL && (child->GetBottom(&bottom), bottom) <= 0) {
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
DayPickerView::InnerRunnable::InnerRunnable(
    /* [in] */ DayPickerView* owner,
    /* [in] */ Int32 position)
    : mOwner(owner)
    , mPosition(position)
{
    assert(mOwner);
}

DayPickerView::InnerRunnable::~InnerRunnable()
{}

ECode DayPickerView::InnerRunnable::Run()
{
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

CAR_INTERFACE_IMPL(DayPickerView, ListView, IDayPickerView)

DayPickerView::DayPickerView()
    : mFriction(1.0f)
    , mCurrentMonthDisplayed(0)
    , mPreviousScrollState(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE)
    , mCurrentScrollState(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE)
    , mPerformingScroll(FALSE)
{
}

ECode DayPickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDatePickerController* controller)
{
    ListView::constructor(context);

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    CSimpleDateFormat::New(String("yyyy"), locale, (ISimpleDateFormat**)&mYearFormat);

    AutoPtr<ICalendarHelper> calHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calHelper);
    calHelper->GetInstance((ICalendar**)&mSelectedDay);
    calHelper->GetInstance((ICalendar**)&mTempDay);

    mScrollStateChangedRunnable = new ScrollStateRunnable(this, this);
    mListener = new InnerListener(this);

    Init();

    SetController(controller);
    return NOERROR;
}

ECode DayPickerView::SetController(
    /* [in] */ IDatePickerController* controller)
{
    if (mController != NULL) {
        mController->UnregisterOnDateChangedListener(mListener);
    }
    mController = controller;
    mController->RegisterOnDateChangedListener(mListener);
    SetUpAdapter();
    SetAdapter(IAdapter::Probe(mAdapter));
    OnDateChanged();
    return NOERROR;
}

ECode DayPickerView::Init()
{
    AutoPtr<IViewGroupLayoutParams> vglp;
    CAbsListViewLayoutParams::New(IListPopupWindow::MATCH_PARENT,
            IListPopupWindow::MATCH_PARENT, (IViewGroupLayoutParams**)&vglp);
    SetLayoutParams(vglp);
    SetDrawSelectorOnTop(FALSE);

    SetUpListView();
    return NOERROR;
}

ECode DayPickerView::OnChange()
{
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
        child = NULL;
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

    AutoPtr<IView> viewTmp;
    (IViewGroup::Probe(view))->GetChildAt(0, (IView**)&viewTmp);
    ISimpleMonthView* child = ISimpleMonthView::Probe(viewTmp);
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

    mScrollStateChangedRunnable->DoScrollStateChange(view, scrollState);
    return NOERROR;
}

ECode DayPickerView::SetCalendarTextColor(
    /* [in] */ IColorStateList* colors)
{
    VALIDATE_NOT_NULL(colors);

    mAdapter->SetCalendarTextColor(colors);
    return NOERROR;
}

ECode DayPickerView::GetMostVisiblePosition(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 firstPosition = 0, height = 0;
    GetFirstVisiblePosition(&firstPosition);
    GetHeight(&height);

    Int32 maxDisplayedHeight = 0;
    Int32 mostVisibleIndex = 0;
    Int32 i = 0;
    Int32 top = 0;
    Int32 bottom = 0;
    Int32 displayedHeight = 0;
    using Elastos::Core::Math;
    while (bottom < height) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        if (child == NULL) {
            break;
        }

        child->GetBottom(&bottom);
        child->GetTop(&top);
        displayedHeight = Math::Min(bottom, height) - Math::Max(0, top);
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

    ListView::OnInitializeAccessibilityEvent(event);
    (IAccessibilityRecord::Probe(event))->SetItemCount(-1);
    return NOERROR;
}

ECode DayPickerView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);

    ListView::OnInitializeAccessibilityNodeInfo(info);
    info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD);
    info->AddAction(IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD);
    return NOERROR;
}

ECode DayPickerView::PerformAccessibilityAction(
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    if (action != IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD &&
            action != IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD) {
        return ListView::PerformAccessibilityAction(action, arguments, res);
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
    *res = TRUE;
    return NOERROR;
}

void DayPickerView::SetUpAdapter()
{
    if (mAdapter == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CSimpleMonthAdapter::New(context, mController, (ISimpleMonthAdapter**)&mAdapter);
    }
    else {
        mAdapter->SetSelectedDay(mSelectedDay);
        IBaseAdapter::Probe(mAdapter)->NotifyDataSetChanged();
    }
    // refresh the view with the new parameters
    IBaseAdapter::Probe(mAdapter)->NotifyDataSetChanged();
}

void DayPickerView::SetUpListView()
{
    // Transparent background on scroll
    SetCacheColorHint(0);
    // No dividers
    SetDivider(NULL);
    // Items are clickable
    SetItemsCanFocus(TRUE);
    // The thumb gets in the way, so disable it
    SetFastScrollEnabled(FALSE);
    SetVerticalScrollBarEnabled(FALSE);
    SetOnScrollListener(mListener);
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
    Int32 month = 0;
    date->Get(ICalendar::MONTH, &month);
    if (mCurrentMonthDisplayed != month) {
        mCurrentMonthDisplayed = month;
        InvalidateViews();
    }
}

ECode DayPickerView::LayoutChildren()
{
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

ECode DayPickerView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    mYearFormat = NULL;
    return CSimpleDateFormat::New(String("yyyy"), locale, (ISimpleDateFormat**)&mYearFormat);
}

Int32 DayPickerView::GetDiffMonths(
    /* [in] */ ICalendar* start,
    /* [in] */ ICalendar* end)
{
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
    String str;
    Elastos::Text::IDateFormat::Probe(mYearFormat)->Format(dateTime, &str);
    sbuf->Append(str);
    String result;
    sbuf->ToString(&result);
    return result;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
