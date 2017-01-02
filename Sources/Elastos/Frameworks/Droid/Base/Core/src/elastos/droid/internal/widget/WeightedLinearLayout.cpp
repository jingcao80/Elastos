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

#include "elastos/droid/internal/widget/WeightedLinearLayout.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Internal::Widget::EIID_IWeightedLinearLayout;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(WeightedLinearLayout, LinearLayout, IWeightedLinearLayout)

WeightedLinearLayout::WeightedLinearLayout()
    : mMajorWeightMin(0)
    , mMinorWeightMin(0)
    , mMajorWeightMax(0)
    , mMinorWeightMax(0)
{
}

ECode WeightedLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return LinearLayout::constructor(context);
}

ECode WeightedLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::WeightedLinearLayout);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    a->GetFloat(R::styleable::WeightedLinearLayout_majorWeightMin, 0.0f, &mMajorWeightMin);
    a->GetFloat(R::styleable::WeightedLinearLayout_minorWeightMin, 0.0f, &mMinorWeightMin);
    a->GetFloat(R::styleable::WeightedLinearLayout_majorWeightMax, 0.0f, &mMajorWeightMax);
    a->GetFloat(R::styleable::WeightedLinearLayout_minorWeightMax, 0.0f, &mMinorWeightMax);
    a->Recycle();
    return NOERROR;
}

ECode WeightedLinearLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<IDisplayMetrics> metrics;
    AutoPtr<IResources> res;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetResources((IResources**)&res);
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);

    Int32 screenWidth, screenHeight;
    metrics->GetWidthPixels(&screenWidth);
    metrics->GetHeightPixels(&screenHeight);
    Boolean isPortrait = screenWidth < screenHeight;

    Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 width = 0;
    GetMeasuredWidth(&width);
    Boolean measure = FALSE;

    widthMeasureSpec = MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY);

    Float widthWeightMin = isPortrait ? mMinorWeightMin : mMajorWeightMin;
    Float widthWeightMax = isPortrait ? mMinorWeightMax : mMajorWeightMax;
    if (widthMode == MeasureSpec::AT_MOST) {
        Int32 weightedMin = (Int32) (screenWidth * widthWeightMin);
        Int32 weightedMax = (Int32) (screenWidth * widthWeightMin);
        if (widthWeightMin > 0.0f && width < weightedMin) {
            widthMeasureSpec = MeasureSpec::MakeMeasureSpec(weightedMin, MeasureSpec::EXACTLY);
            measure = TRUE;
        }
        else if (widthWeightMax > 0.0f && width > weightedMax) {
            widthMeasureSpec = MeasureSpec::MakeMeasureSpec(weightedMax, MeasureSpec::EXACTLY);
            measure = TRUE;
        }
    }
    if (measure) {
        LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

