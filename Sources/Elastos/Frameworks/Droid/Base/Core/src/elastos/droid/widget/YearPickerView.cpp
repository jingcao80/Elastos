#include "elastos/droid/widget/YearPickerView.h"
#include "elastos/droid/widget/CAbsListViewLayoutParams.h"

using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::Widget::CAbsListViewLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget{

///////////////////////////////////////////////////////////////
//              YearPickerView::YearAdapter
///////////////////////////////////////////////////////////////

YearPickerView::YearAdapter::YearAdapter()
    : ArrayAdapter()
    , mItemTextAppearanceResId(0)
{
}

ECode YearPickerView::YearAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resource)
{
    ArrayAdapter::constructor(context, resource);
    return NOERROR;
}

ECode YearPickerView::YearAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    /*TextViewWithCircularIndicator v = (TextViewWithCircularIndicator)
            super.getView(position, convertView, parent);
    v.setTextAppearance(getContext(), mItemTextAppearanceResId);
    v.requestLayout();
    int year = getItem(position);
    boolean selected = mController.getSelectedDay().get(Calendar.YEAR) == year;
    v.setDrawIndicator(selected);
    if (selected) {
        v.setCircleColor(mYearSelectedCircleColor);
    }
    return v;*/
    return NOERROR;
}

ECode YearPickerView::YearAdapter::SetItemTextAppearance(
    /* [in] */ Int32 resId)
{
    mItemTextAppearanceResId = resId;
    return NOERROR;
}

///////////////////////////////////////////////////////////////
//                  YearPickerView
///////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL_3(YearPickerView, ListView, IYearPickerView, IAdapterViewOnItemClickListener, IOnDateChangedListener)

YearPickerView::YearPickerView()
    : ListView()
    , mViewSize(0)
    , mChildSize(0)
    , mSelectedPosition(-1)
    , mYearSelectedCircleColor(0)
{
}

ECode YearPickerView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ListView::constructor(context, attrs, defStyleAttr, defStyleRes);

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

    SetOnItemClickListener(this);
    SetDividerHeight(0);
    return NOERROR;
}

ECode YearPickerView::Init(
    /* [in] */ IDatePickerController* controller)
{
    mController = controller;
    mController->RegisterOnDateChangedListener((IOnDateChangedListener*)this->Probe(EIID_IOnDateChangedListener));
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mAdapter = new YearAdapter();
    mAdapter->constructor(ctx, R::layout::year_label_text_view);
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

    return NOERROR;
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
        IAccessibilityRecord::Probe(event)->SetFromIndex(0);
        IAccessibilityRecord::Probe(event)->SetToIndex(0);
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