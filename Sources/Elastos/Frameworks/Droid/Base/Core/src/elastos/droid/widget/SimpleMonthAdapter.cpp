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

#include "Elastos.Droid.View.h"
#include "elastos/droid/widget/SimpleMonthAdapter.h"
#include "elastos/droid/widget/CSimpleMonthView.h"

using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

const String SimpleMonthAdapter::TAG("SimpleMonthAdapter");

CAR_INTERFACE_IMPL_2(SimpleMonthAdapter, BaseAdapter, ISimpleMonthAdapter, IOnDayClickListener);
ECode SimpleMonthAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDatePickerController* controller)
{
    mContext = context;
    mController = controller;
    Init();
    AutoPtr<ICalendar> c;
    mController->GetSelectedDay((ICalendar**)&c);
    SetSelectedDay(c);
    return NOERROR;
}

ECode SimpleMonthAdapter::SetSelectedDay(
    /* [in] */ ICalendar* day)
{
    if (mSelectedDay.Get() != day) {
        mSelectedDay = day;
        NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode SimpleMonthAdapter::SetCalendarTextColor(
    /* [in] */ IColorStateList* colors)
{
    mCalendarTextColors = colors;
    return NOERROR;
}

void SimpleMonthAdapter::Init()
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance((ICalendar**)&mSelectedDay);
}

ECode SimpleMonthAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    Int32 max = 0, min = 0;
    Int32 diffYear = (mController->GetMaxYear(&max), max) - (mController->GetMinYear(&min), min);
    Int32 diffMonth = 1 + (mController->GetMaxMonth(&max), max) - (mController->GetMinMonth(&min), min)
            + 12 * diffYear;
    *count = diffMonth;
    return NOERROR;
}

ECode SimpleMonthAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    *item = NULL;
    return NOERROR;
}

ECode SimpleMonthAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = position;
    return NOERROR;
}

ECode SimpleMonthAdapter::HasStableIds(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = TRUE;
    return NOERROR;
}

ECode SimpleMonthAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    AutoPtr<ISimpleMonthView> v;
    AutoPtr<IHashMap> drawingParams;/*<String, Integer*/
    if (convertView != NULL) {
        v = ISimpleMonthView::Probe(convertView);
        // We store the drawing parameters in the view so it can be recycled
        AutoPtr<IInterface> obj;
        IView::Probe(v)->GetTag((IInterface**)&obj);
        drawingParams = IHashMap::Probe(obj);
    } else {
        CSimpleMonthView::New(mContext, (ISimpleMonthView**)&v);
        // Set up the new view
        AutoPtr<IAbsListViewLayoutParams> params;
        // CAbsListViewLayoutParams::New(AbsListView.LayoutParams.MATCH_PARENT, AbsListView.LayoutParams.MATCH_PARENT
        //         , (IAbsListViewLayoutParams**)&params);
        IView::Probe(v)->SetLayoutParams(IViewGroupLayoutParams::Probe(params));
        IView::Probe(v)->SetClickable(TRUE);
        v->SetOnDayClickListener(this);
        if (mCalendarTextColors != NULL) {
            v->SetTextColor(mCalendarTextColors);
        }
    }
    if (drawingParams == NULL) {
        CHashMap::New((IHashMap**)&drawingParams);
    } else {
        drawingParams->Clear();
    }

    Int32 value = 0;
    Int32 currentMonth = position + (mController->GetMinMonth(&value), value);
    Int32 month = currentMonth % 12;
    Int32 year = currentMonth / 12 + (mController->GetMinYear(&value), value);

    Int32 selectedDay = -1;
    if (IsSelectedDayInMonth(year, month)) {
        mSelectedDay->Get(ICalendar::DAY_OF_MONTH, &selectedDay);
    }

    // Invokes requestLayout() to ensure that the recycled view is set with the appropriate
    // height/number of weeks before being displayed.
    v->Reuse();

    Int32 enabledDayRangeStart = 0;
    if ((mController->GetMinMonth(&value), value) == month && (mController->GetMinYear(&value), value) == year) {
        mController->GetMinDay(&enabledDayRangeStart);
    } else {
        enabledDayRangeStart = 1;
    }

    Int32 enabledDayRangeEnd = 0;
    if ((mController->GetMaxMonth(&value), value) == month && (mController->GetMaxYear(&value), value) == year) {
        mController->GetMaxDay(&enabledDayRangeEnd);
    } else {
        enabledDayRangeEnd = 31;
    }

    mController->GetFirstDayOfWeek(&value);
    v->SetMonthParams(selectedDay, month, year, value, enabledDayRangeStart, enabledDayRangeEnd);
    IView::Probe(v)->Invalidate();

    *view = IView::Probe(v);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

Boolean SimpleMonthAdapter::IsSelectedDayInMonth(
    /* [in] */ Int32 year,
    /* [in] */ Int32 month)
{
    Int32 y = 0, m = 0;
    mSelectedDay->Get(ICalendar::YEAR, &y);
    mSelectedDay->Get(ICalendar::MONTH, &m);
    return y == year && m == month;
}

ECode SimpleMonthAdapter::OnDayClick(
    /* [in] */ ISimpleMonthView* view,
    /* [in] */ ICalendar* day)
{
    if (day != NULL) {
        OnDayTapped(day);
    }
    return NOERROR;
}

void SimpleMonthAdapter::OnDayTapped(
    /* [in] */ ICalendar* day)
{
    mController->TryVibrate();
    Int32 year = 0, month = 0, d = 0;
    day->Get(ICalendar::YEAR, &year);
    day->Get(ICalendar::MONTH, &month);
    day->Get(ICalendar::DAY_OF_MONTH, &d);
    mController->OnDayOfMonthSelected(year, month, d);
    SetSelectedDay(day);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
