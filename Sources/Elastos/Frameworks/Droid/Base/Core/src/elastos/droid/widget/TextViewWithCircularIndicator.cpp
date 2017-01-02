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

#include "elastos/droid/widget/TextViewWithCircularIndicator.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/Typeface.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::Typeface;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 TextViewWithCircularIndicator::SELECTED_CIRCLE_ALPHA = 60;

CAR_INTERFACE_IMPL(TextViewWithCircularIndicator, TextView, ITextViewWithCircularIndicator)

TextViewWithCircularIndicator::TextViewWithCircularIndicator()
    : mCircleColor(0)
    , mDrawIndicator(FALSE)
{
    CPaint::New((IPaint**)&mCirclePaint);
}

TextViewWithCircularIndicator::~TextViewWithCircularIndicator()
{}

ECode TextViewWithCircularIndicator::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode TextViewWithCircularIndicator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode TextViewWithCircularIndicator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TextViewWithCircularIndicator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FAIL_RETURN(TextView::constructor(context, attrs))

    // Use Theme attributes if possible
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::DatePicker);
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    Int32 resId;
    a->GetResourceId(R::styleable::DatePicker_yearListItemTextAppearance, -1, &resId);
    if (resId != -1) {
        SetTextAppearance(context, resId);
    }

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    res->GetString(R::string::item_is_selected, &mItemIsSelectedText);

    a->Recycle();

    Init();
    return NOERROR;
}

void TextViewWithCircularIndicator::Init()
{
    AutoPtr<ITypeface> tf;
    mCirclePaint->GetTypeface((ITypeface**)&tf);

    AutoPtr<ITypeface> typeface;
    Typeface::Create(tf, ITypeface::BOLD, (ITypeface**)&typeface);

    mCirclePaint->SetTypeface(typeface);
    mCirclePaint->SetAntiAlias(TRUE);
    mCirclePaint->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER);
    mCirclePaint->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL);
}

ECode TextViewWithCircularIndicator::SetCircleColor(
    /* [in] */ Int32 color)
{
    if (color != mCircleColor) {
        mCircleColor = color;
        mCirclePaint->SetColor(mCircleColor);
        mCirclePaint->SetAlpha(SELECTED_CIRCLE_ALPHA);
        RequestLayout();
    }
    return NOERROR;
}

ECode TextViewWithCircularIndicator::SetDrawIndicator(
    /* [in] */ Boolean drawIndicator)
{
    mDrawIndicator = drawIndicator;
    return NOERROR;
}

void TextViewWithCircularIndicator::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    TextView::OnDraw(canvas);
    if (mDrawIndicator) {
        Int32 width;
        GetWidth(&width);
        Int32 height;
        GetHeight(&height);
        Int32 radius = Elastos::Core::Math::Min(width, height) / 2;
        canvas->DrawCircle(width / 2, height / 2, radius, mCirclePaint);
    }
}

ECode TextViewWithCircularIndicator::GetContentDescription(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    AutoPtr<ICharSequence> itemText;
    GetText((ICharSequence**)&itemText);

    if (mDrawIndicator) {
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, itemText);
        AutoPtr<ICharSequence> seq = CoreUtils::Convert(StringUtils::Format(mItemIsSelectedText, args));
        *text = seq;
        REFCOUNT_ADD(*text);
        return NOERROR;
    }
    else {
        *text = itemText;
        REFCOUNT_ADD(*text);
        return NOERROR;
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
