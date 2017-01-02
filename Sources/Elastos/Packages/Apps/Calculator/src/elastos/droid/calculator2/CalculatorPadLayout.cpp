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

#include "elastos/droid/calculator2/CalculatorPadLayout.h"
#include "R.h"
#include <elastos/droid/R.h>
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Build.h>
#include <elastos/droid/view/ViewGroup.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Os::Build;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::View;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

CAR_INTERFACE_IMPL(CalculatorPadLayout, ViewGroup, ICalculatorPadLayout)

CalculatorPadLayout::CalculatorPadLayout()
    : mRowCount(0)
    , mColumnCount(0)
{}

CalculatorPadLayout::~CalculatorPadLayout()
{}

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
    ViewGroup::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(4);
    (*arr)[0] = Elastos::Droid::R::attr::rowCount;
    (*arr)[1] = Elastos::Droid::R::attr::columnCount;
    (*arr)[2] = defStyle;
    (*arr)[3] = 0;
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, arr, (ITypedArray**)&a);
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
    GetPaddingLeft(&paddingLeft);
    Int32 paddingRight;
    GetPaddingRight(&paddingRight);
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 paddingBottom;
    GetPaddingBottom(&paddingBottom);

    Boolean isRTL;
    if (Build::VERSION::SDK_INT >= 17) {
        Int32 ld;
        GetLayoutDirection(&ld);
        isRTL = ld == IView::LAYOUT_DIRECTION_RTL;
    }
    else {
        isRTL = FALSE;
    }

    Int32 columnWidth =
            Elastos::Core::Math::Round((Float) (right - left - paddingLeft - paddingRight)) / mColumnCount;
    Int32 rowHeight =
            Elastos::Core::Math::Round((Float) (bottom - top - paddingTop - paddingBottom)) / mRowCount;

    Int32 rowIndex = 0, columnIndex = 0;
    Int32 count;
    GetChildCount(&count);
    for (Int32 childIndex = 0; childIndex < count; ++childIndex) {
        AutoPtr<IView> childView;
        GetChildAt(childIndex, (IView**)&childView);
        Int32 visibility;
        if (childView->GetVisibility(&visibility), visibility == IView::GONE) {
            continue;
        }

        AutoPtr<IViewGroupLayoutParams> temp;
        childView->GetLayoutParams((IViewGroupLayoutParams**)&temp);
        AutoPtr<MarginLayoutParams> lp = (MarginLayoutParams*)temp.Get();
        Int32 childTop = paddingTop + lp->mTopMargin + rowIndex * rowHeight;
        Int32 childBottom = childTop - lp->mTopMargin - lp->mBottomMargin + rowHeight;
        Int32 childLeft = paddingLeft + lp->mLeftMargin +
                (isRTL ? (mColumnCount - 1) - columnIndex : columnIndex) * columnWidth;
        Int32 childRight = childLeft - lp->mLeftMargin - lp->mRightMargin + columnWidth;

        Int32 childWidth = childRight - childLeft;
        Int32 childHeight = childBottom - childTop;
        Int32 gmwidth, gmHeight;
        if ((childView->GetMeasuredWidth(&gmwidth), childWidth != gmwidth) ||
                (childView->GetMeasuredHeight(&gmHeight), childHeight != gmHeight)) {
            childView->Measure(
                    MeasureSpec::MakeMeasureSpec(childWidth, MeasureSpec::EXACTLY),
                    MeasureSpec::MakeMeasureSpec(childHeight, MeasureSpec::EXACTLY));
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
    GetContext((IContext**)&context);
    AutoPtr<MarginLayoutParams> mlp = new MarginLayoutParams();
    mlp->constructor(context, attrs);
    *result = IViewGroupLayoutParams::Probe(mlp);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CalculatorPadLayout::GenerateDefaultLayoutParams(
    /* [out] */ IViewGroupLayoutParams** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<MarginLayoutParams> mlp = new MarginLayoutParams();
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
    AutoPtr<MarginLayoutParams> vmp = new ViewGroup::MarginLayoutParams();
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
    *result = FALSE;
    if (IViewGroupLayoutParams::Probe(p)) {
        *result = TRUE;
    }
    return NOERROR;
}

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos