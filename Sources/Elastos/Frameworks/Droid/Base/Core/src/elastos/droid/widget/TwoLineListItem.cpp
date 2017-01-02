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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/TwoLineListItem.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(TwoLineListItem, RelativeLayout, ITwoLineListItem);
ECode TwoLineListItem::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode TwoLineListItem::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode TwoLineListItem::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TwoLineListItem::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    RelativeLayout::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TwoLineListItem);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    a->Recycle();

    return NOERROR;
}

ECode TwoLineListItem::OnFinishInflate()
{
    RelativeLayout::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::text1, (IView**)&view);
    mText1 = ITextView::Probe(view);
    view = NULL;
    FindViewById(R::id::text2, (IView**)&view);
    mText2 = ITextView::Probe(view);
    return NOERROR;
}

ECode TwoLineListItem::GetText1(
    /* [out] */ ITextView** textView)
{
    VALIDATE_NOT_NULL(textView);
    *textView = mText1;
    REFCOUNT_ADD(*textView);
    return NOERROR;
}

ECode TwoLineListItem::GetText2(
    /* [out] */ ITextView** textView)
{
    VALIDATE_NOT_NULL(textView);
    *textView = mText2;
    REFCOUNT_ADD(*textView);
    return NOERROR;
}

ECode TwoLineListItem::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    RelativeLayout::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(String("CTwoLineListItem"), (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode TwoLineListItem::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    RelativeLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(String("CTwoLineListItem"), (ICharSequence**)&seq));
    info->SetClassName(seq);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
