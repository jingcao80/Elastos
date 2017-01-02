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

#include "elastos/droid/widget/YearPickerView.h"
#include "elastos/droid/widget/CAbsListViewLayoutParams.h"
#include "elastos/droid/R.h"

using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::CAbsListViewLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::R;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget{

CAR_INTERFACE_IMPL_2(YearPickerView::InnerListener, Object, \
    IAdapterViewOnItemClickListener, IOnDateChangedListener)

YearPickerView::InnerListener::InnerListener(
    /* [in] */ YearPickerView* host)
    : mHost(host)
{}

ECode YearPickerView::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

ECode YearPickerView::InnerListener::OnDateChanged()
{
    return mHost->OnDateChanged();
}

///////////////////////////////////////////////////////////////
//              YearPickerView::YearAdapter
///////////////////////////////////////////////////////////////

YearPickerView::YearAdapter::YearAdapter()
    : mItemTextAppearanceResId(0)
{
}

ECode YearPickerView::YearAdapter::constructor(
    /* [in] */ YearPickerView* host,
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource)
{
    mHost = host;
    return ArrayAdapter::constructor(context, resource);
}

ECode YearPickerView::YearAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    AutoPtr<IView> tmp;
    ArrayAdapter::GetView(position, convertView, parent, (IView**)&tmp);
    AutoPtr<ITextViewWithCircularIndicator> v = ITextViewWithCircularIndicator::Probe(tmp);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    ITextView::Probe(v)->SetTextAppearance(context, mItemTextAppearanceResId);
    tmp->RequestLayout();
    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    Int32 year;
    IInteger32::Probe(obj)->GetValue(&year);
    AutoPtr<ICalendar> calendar;
    mHost->mController->GetSelectedDay((ICalendar**)&calendar);
    Int32 y;
    calendar->Get(ICalendar::YEAR, &y);
    Boolean selected = y == year;
    v->SetDrawIndicator(selected);
    if (selected) {
        v->SetCircleColor(mHost->mYearSelectedCircleColor);
    }

    *view = tmp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode YearPickerView::YearAdapter::SetItemTextAppearance(
    /* [in] */ Int32 resId)
{
    mItemTextAppearanceResId = resId;
    return NOERROR;
}

///////////////////////////////////////////////////////////////
//              YearPickerView::YearRunnable
///////////////////////////////////////////////////////////////

YearPickerView::YearRunnable::YearRunnable(
    /* [in] */ YearPickerView* host,
    /* [in] */ Int32 position,
    /* [in] */ Int32 offset)
    : mHost(host)
    , mPosition(position)
    , mOffset(offset)
{}

YearPickerView::YearRunnable::~YearRunnable()
{}

ECode YearPickerView::YearRunnable::Run()
{
    mHost->SetSelectionFromTop(mPosition, mOffset);
    mHost->RequestLayout();
    return NOERROR;
}

///////////////////////////////////////////////////////////////
//                  YearPickerView
///////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(YearPickerView, ListView, IYearPickerView)

YearPickerView::YearPickerView()
    : mViewSize(0)
    , mChildSize(0)
    , mSelectedPosition(-1)
    , mYearSelectedCircleColor(0)
{}

YearPickerView::~YearPickerView()
{}

ECode YearPickerView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode YearPickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::listViewStyle);
}

ECode YearPickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode YearPickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ListView::constructor(context, attrs, defStyleAttr, defStyleRes);

    mListener = new InnerListener(this);

    AutoPtr<IViewGroupLayoutParams> frame;
    CAbsListViewLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
        IViewGroupLayoutParams::WRAP_CONTENT, (IViewGroupLayoutParams**)&frame);
    SetLayoutParams(frame);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetDimensionPixelOffset(R::dimen::datepicker_view_animator_height, &mViewSize);
    res->GetDimensionPixelOffset(R::dimen::datepicker_year_label_height, &mChildSize);

    SetVerticalFadingEdgeEnabled(TRUE);
    SetFadingEdgeLength(mChildSize / 3);

    Int32 paddingTop;
    res->GetDimensionPixelSize(R::dimen::datepicker_year_picker_padding_top, &paddingTop);
    SetPadding(0, paddingTop, 0, 0);

    SetOnItemClickListener(mListener);
    SetDividerHeight(0);
    return NOERROR;
}

ECode YearPickerView::Init(
    /* [in] */ IDatePickerController* controller)
{
    mController = controller;
    mController->RegisterOnDateChangedListener(mListener);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mAdapter = new YearAdapter();
    mAdapter->constructor(this, ctx, R::layout::year_label_text_view);
    UpdateAdapterData();
    SetAdapter(mAdapter);

    OnDateChanged();
    return NOERROR;
}

ECode YearPickerView::SetYearSelectedCircleColor(
    /* [in] */ Int32 color)
{
    if (color != mYearSelectedCircleColor) {
        mYearSelectedCircleColor = color;
    }
    RequestLayout();
    return NOERROR;
}

ECode YearPickerView::GetYearSelectedCircleColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mYearSelectedCircleColor;
    return NOERROR;
}

ECode YearPickerView::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    mController->TryVibrate();
    if (position != mSelectedPosition) {
        mSelectedPosition = position;
        mAdapter->NotifyDataSetChanged();
    }
    AutoPtr<IInterface> obj;
    mAdapter->GetItem(position, (IInterface**)&obj);
    IInteger32* IntId = IInteger32::Probe(obj);
    Int32 itemId;
    IntId->GetValue(&itemId);
    mController->OnYearSelected(itemId);
    return NOERROR;
}

//package
ECode YearPickerView::SetItemTextAppearance(
    /* [in] */ Int32 resId)
{
    mAdapter->SetItemTextAppearance(resId);
    return NOERROR;
}

ECode YearPickerView::PostSetSelectionCentered(
    /* [in] */ Int32 position)
{
    PostSetSelectionFromTop(position, mViewSize / 2 - mChildSize / 2);
    return NOERROR;
}

ECode YearPickerView::PostSetSelectionFromTop(
    /* [in] */ Int32 position,
    /* [in] */ Int32 offset)
{
    AutoPtr<IRunnable> runnable = new YearRunnable(this, position, offset);
    Boolean res;
    return Post(runnable, &res);
}

ECode YearPickerView::GetFirstPositionOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    AutoPtr<IView> firstChild;
    GetChildAt(0, (IView**)&firstChild);
    if (firstChild == NULL) {
        *offset = 0;
        return NOERROR;
    }
    return firstChild->GetTop(offset);
}

ECode YearPickerView::OnDateChanged()
{
    UpdateAdapterData();
    mAdapter->NotifyDataSetChanged();
    AutoPtr<ICalendar> calendar;
    mController->GetSelectedDay((ICalendar**)&calendar);
    Int32 value, minYear;
    calendar->Get(ICalendar::YEAR, &value);
    mController->GetMinYear(&minYear);
    PostSetSelectionCentered(value - minYear);
    return NOERROR;
}

ECode YearPickerView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ListView::OnInitializeAccessibilityEvent(event);
    Int32 type;
    event->GetEventType(&type);
    if (type == IAccessibilityEvent::TYPE_VIEW_SCROLLED) {
        IAccessibilityRecord* record = IAccessibilityRecord::Probe(event);
        record->SetFromIndex(0);
        record->SetToIndex(0);
    }
    return NOERROR;
}

void YearPickerView::UpdateAdapterData()
{
    mAdapter->Clear();
    Int32 maxYear, year;
    mController->GetMaxYear(&maxYear);
    mController->GetMinYear(&year);
    for (; year <= maxYear; year++) {
        AutoPtr<IInteger32> id;
        CInteger32::New(year, (IInteger32**)&id);
        mAdapter->Add(id);
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos