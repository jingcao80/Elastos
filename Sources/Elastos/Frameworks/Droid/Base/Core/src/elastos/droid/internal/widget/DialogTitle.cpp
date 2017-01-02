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

#include "elastos/droid/internal/widget/DialogTitle.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_INTERFACE_IMPL(DialogTitle, TextView, IDialogTitle)

DialogTitle::DialogTitle()
{
}

DialogTitle::~DialogTitle()
{}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context)
{
    return TextView::constructor(context);
}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TextView::constructor(context, attrs);
}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return TextView::constructor(context, attrs, defStyleAttr);
}

ECode DialogTitle::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return TextView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode DialogTitle::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    AutoPtr<ILayout> layout;
    GetLayout((ILayout**)&layout);
    if (layout != NULL) {
        Int32 lineCount;
        layout->GetLineCount(&lineCount);

        if (lineCount > 0) {
            Int32 ellipsisCount;
            layout->GetEllipsisCount(lineCount - 1, &ellipsisCount);

            if (ellipsisCount > 0) {
                SetSingleLine(FALSE);
                SetMaxLines(2);

                AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TextAppearance);
                AutoPtr<ITypedArray> a;
                ASSERT_SUCCEEDED(mContext->ObtainStyledAttributes(
                        NULL, attrIds, R::attr::textAppearanceMedium,
                        R::style::TextAppearance_Medium, (ITypedArray**)&a));

                Int32 textSize;
                a->GetDimensionPixelSize(
                    R::styleable::TextAppearance_textSize, 0, &textSize);

                if (textSize != 0) {
                    // textSize is already expressed in pixels
                    SetTextSize(ITypedValue::COMPLEX_UNIT_PX, textSize);
                }
                a->Recycle();

                TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
            }
        }
    }

    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
