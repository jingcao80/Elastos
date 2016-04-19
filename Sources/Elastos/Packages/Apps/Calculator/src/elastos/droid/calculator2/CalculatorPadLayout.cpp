#include "elastos/droid/calculator2/CalculatorPadLayout.h"
#include "R.h"
#include <elastos/droid/R.h>
#include "Elastos.Droid.View.h"
#include <elastos/droid/view/ViewGroup.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::View;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

CalculatorPadLayout::CalculatorPadLayout()
{}

CalculatorPadLayout::~CalculatorPadLayout()
{}

CAR_INTERFACE_IMPL(CalculatorPadLayout, ViewGroup, ICalculatorPadLayout)

ECode CalculatorPadLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CalculatorPadLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CalculatorPadLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(2);
    (*arr)[0] = Elastos::Droid::R::attr::rowCount;
    (*arr)[1] = Elastos::Droid::R::attr::columnCount;
    (*arr)[2] = defStyle;
    (*arr)[3] = 0;
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, arr.Get(), (ITypedArray**)&a);
    a->GetInt32(0, 1, &mRowCount);
    a->GetInt32(1, 1, &mColumnCount);

    return a->Recycle();
}

ECode CalculatorPadLayout::ShouldDelayChildPressedState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CalculatorPadLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 paddingLeft;
    IView::Probe(this)->GetPaddingLeft(&paddingLeft);
    Int32 paddingRight;
    IView::Probe(this)->GetPaddingRight(&paddingRight);
    Int32 paddingTop;
    IView::Probe(this)->GetPaddingTop(&paddingTop);
    Int32 paddingBottom;
    IView::Probe(this)->GetPaddingBottom(&paddingBottom);

    Int32 ld;
    IView::Probe(this)->GetLayoutDirection(&ld);
    Boolean isRTL = ld == IView::LAYOUT_DIRECTION_RTL;
    Int32 columnWidth =
            Elastos::Core::Math::Round((Float) (right - left - paddingLeft - paddingRight)) / mColumnCount;
    Int32 rowHeight =
            Elastos::Core::Math::Round((Float) (bottom - top - paddingTop - paddingBottom)) / mRowCount;

    Int32 rowIndex = 0, columnIndex = 0;
    Int32 count;
    IViewGroup::Probe(this)->GetChildCount(&count);
    AutoPtr<IView> childView;
    AutoPtr<IViewGroupLayoutParams> lp;
    for (Int32 childIndex = 0; childIndex < count; ++childIndex) {
        childView = NULL;
        IViewGroup::Probe(this)->GetChildAt(childIndex, (IView**)&childView);
        Int32 visibility;
        if (childView->GetVisibility(&visibility), visibility == IView::GONE) {
            continue;
        }

        lp = NULL;
        childView->GetLayoutParams((IViewGroupLayoutParams**)&lp);

        Int32 childTop = paddingTop + ((ViewGroup::MarginLayoutParams*)lp.Get())->mTopMargin + rowIndex * rowHeight;
        Int32 childBottom = childTop - ((ViewGroup::MarginLayoutParams*)lp.Get())->mTopMargin - ((ViewGroup::MarginLayoutParams*)lp.Get())->mBottomMargin + rowHeight;
        Int32 childLeft = paddingLeft + ((ViewGroup::MarginLayoutParams*)lp.Get())->mLeftMargin +
                (isRTL ? (mColumnCount - 1) - columnIndex : columnIndex) * columnWidth;
        Int32 childRight = childLeft - ((ViewGroup::MarginLayoutParams*)lp.Get())->mLeftMargin - ((ViewGroup::MarginLayoutParams*)lp.Get())->mRightMargin + columnWidth;

        Int32 childWidth = childRight - childLeft;
        Int32 childHeight = childBottom - childTop;
        Int32 gmwidth;
        childView->GetMeasuredWidth(&gmwidth);
        Int32 gmHeight;
        childView->GetMeasuredHeight(&gmHeight);
        if (childWidth != gmwidth ||
                childHeight != gmHeight) {
            childView->Measure(
                    View::MeasureSpec::MakeMeasureSpec(childWidth, View::MeasureSpec::EXACTLY),
                    View::MeasureSpec::MakeMeasureSpec(childHeight, View::MeasureSpec::EXACTLY));
        }
        childView->Layout(childLeft, childTop, childRight, childBottom);

        rowIndex = (rowIndex + (columnIndex + 1) / mColumnCount) % mRowCount;
        columnIndex = (columnIndex + 1) % mColumnCount;
    }
    return NOERROR;
}

ECode CalculatorPadLayout::GenerateLayoutParams(
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContext> context;
    IView::Probe(this)->GetContext((IContext**)&context);
    AutoPtr<ViewGroup::MarginLayoutParams> mlp;
    mlp = new ViewGroup::MarginLayoutParams();
    mlp->constructor(context.Get(), attrs);
    *result = IViewGroupLayoutParams::Probe(mlp);
    REFCOUNT_ADD(*result);
}

ECode CalculatorPadLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ViewGroup::MarginLayoutParams> mlp;
    mlp = new ViewGroup::MarginLayoutParams();
    mlp->constructor(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
    *result = IViewGroupLayoutParams::Probe(mlp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CalculatorPadLayout::GenerateLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ViewGroup::MarginLayoutParams> vmp;
    vmp = new ViewGroup::MarginLayoutParams();
    vmp->constructor(p);
    *result = IViewGroupLayoutParams::Probe(vmp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CalculatorPadLayout::CheckLayoutParams(
    /* [in] */ IViewGroupLayoutParams* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (IViewGroupLayoutParams::Probe(p)) {
        *result = TRUE;
    }
    return NOERROR;
}

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos