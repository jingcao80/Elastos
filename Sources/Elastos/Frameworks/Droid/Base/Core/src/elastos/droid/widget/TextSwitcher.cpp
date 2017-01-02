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

#include "elastos/droid/widget/TextSwitcher.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;

using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(TextSwitcher, ViewSwitcher, ITextSwitcher)

ECode TextSwitcher::constructor(
    /* [in] */ IContext* context)
{
    return ViewSwitcher::constructor(context);
}

ECode TextSwitcher::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewSwitcher::constructor(context, attrs);
}

ECode TextSwitcher::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    if (child->Probe(EIID_ITextView) == NULL) {
//        throw new IllegalArgumentException(
//                "TextSwitcher children must be instances of TextView");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return ViewSwitcher::AddView(child, index, params);
}

ECode TextSwitcher::SetText(
    /* [in] */ ICharSequence* text)
{
    AutoPtr<IView> v;
    GetNextView((IView**)&v);
    AutoPtr<ITextView> tv = ITextView::Probe(v);
    if(tv == NULL)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    tv->SetText(text);
    ShowNext();
    return NOERROR;
}

ECode TextSwitcher::SetCurrentText(
    /* [in] */ ICharSequence* text)
{
    AutoPtr<IView> v;
    GetCurrentView((IView**)&v);
    AutoPtr<ITextView> tv = ITextView::Probe(v);
    if(tv == NULL)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    tv->SetText(text);
    return NOERROR;
}

ECode TextSwitcher::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewSwitcher::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("CTextSwitcher"));
    return IAccessibilityRecord::Probe(event)->SetClassName(seq);
}

ECode TextSwitcher::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewSwitcher::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(String("CTextSwitcher"));
    return info->SetClassName(seq);
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
