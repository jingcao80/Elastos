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

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/text/method/QwertyKeyListener.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/widget/MultiAutoCompleteTextView.h"
#include "elastos/droid/text/CSpannableString.h"

#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::Method::QwertyKeyListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;

using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Widget {

const String MultiAutoCompleteTextView::MULTIAUTOCOMPLETETEXTVIEW_NAME = String("MultiAutoCompleteTextView");

CAR_INTERFACE_IMPL(MultiAutoCompleteTextView, AutoCompleteTextView, IMultiAutoCompleteTextView)

MultiAutoCompleteTextView::MultiAutoCompleteTextView()
{}

ECode MultiAutoCompleteTextView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode MultiAutoCompleteTextView::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::autoCompleteTextViewStyle);
}

ECode MultiAutoCompleteTextView::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode MultiAutoCompleteTextView::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes)
{
    return AutoCompleteTextView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode MultiAutoCompleteTextView::SetTokenizer(
    /* [in] */ ITokenizer* t)
{
    mTokenizer = t;
    return NOERROR;
}

ECode MultiAutoCompleteTextView::EnoughToFilter(
        /* [out] */ Boolean* enough)
{
    AutoPtr<ICharSequence> seq;
    GetText((ICharSequence**)&seq);
    AutoPtr<IEditable> text = IEditable::Probe(seq);

    Int32 end;
    GetSelectionEnd(&end);
    if(end < 0 || mTokenizer == NULL) {
        *enough = FALSE;
        return NOERROR;
    }

    Int32 start = 0;
    mTokenizer->FindTokenStart(seq, end, &start);

    Int32 threshold;
    GetThreshold(&threshold);
    if(end - start >= threshold) {
        *enough = TRUE;
        return NOERROR;
    } else {
        *enough = FALSE;
        return NOERROR;
    }
}

ECode MultiAutoCompleteTextView::PerformValidation()
{
    AutoPtr<IValidator> v;
    GetValidator((IValidator**)&v);
    if(v == NULL || mTokenizer == NULL) {
        return NOERROR;
    }
    AutoPtr<ICharSequence> seq;
    GetText((ICharSequence**)&seq);
    AutoPtr<IEditable> text = IEditable::Probe(seq);
    Int32 i = 0;
    seq->GetLength(&i);

    while(i > 0) {
        Int32 start = 0;
        mTokenizer->FindTokenStart (seq, i, &start);
        Int32 end = 0;
        mTokenizer->FindTokenEnd(seq, start, &end);

        AutoPtr<ICharSequence> sub;
        seq->SubSequence(start, end, (ICharSequence**)&sub);

        Boolean isValid = false;
        v->IsValid(sub, &isValid);
        if(TextUtils::IsEmpty(sub)) {
            AutoPtr<ICharSequence> csqnull = CoreUtils::Convert(String(""));
            text->Replace(start, i, csqnull);
        } else if (!isValid) {
            AutoPtr<ICharSequence> csq;
            v->FixText(sub, (ICharSequence**)&csq);
            AutoPtr<ICharSequence> terCsq;
            mTokenizer->TerminateToken(csq, (ICharSequence**)&terCsq);
            text->Replace(start, i, terCsq);
        }
        i = start;
    }
    return NOERROR;
}

ECode MultiAutoCompleteTextView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AutoCompleteTextView::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(MULTIAUTOCOMPLETETEXTVIEW_NAME);
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode MultiAutoCompleteTextView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AutoCompleteTextView::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq = CoreUtils::Convert(MULTIAUTOCOMPLETETEXTVIEW_NAME);
    info->SetClassName(seq);
    return NOERROR;
}

void MultiAutoCompleteTextView::PerformFiltering(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 keyCode)
{
    Boolean enough;
    if(EnoughToFilter(&enough), enough) {
        Int32 end;
        GetSelectionEnd(&end);
        Int32 start = 0;
        mTokenizer->FindTokenStart(text, end, &start);

        PerformFiltering(text, start, end, keyCode);
    } else {
        DismissDropDown();

        AutoPtr<IFilter> f = GetFilter();
        if(f) {
            f->DoFilter(NULL);
        }
    }
}

void MultiAutoCompleteTextView::PerformFiltering(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 keyCode)
{
    AutoPtr<ICharSequence> sub;
    text->SubSequence(start, end, (ICharSequence**)&sub);
    GetFilter()->DoFilter(sub, this);
}

void MultiAutoCompleteTextView::ReplaceText(
    /* [in] */ ICharSequence* text)
{
    ClearComposingText();
    Int32 end;
    GetSelectionEnd(&end);
    Int32 start = 0;
    AutoPtr<ICharSequence> chars;
    GetText((ICharSequence**)&chars);
    mTokenizer->FindTokenStart(chars, end, &start);

    AutoPtr<IEditable> editable = IEditable::Probe(chars);
    String original = TextUtils::Substring(chars, start, end);
    QwertyKeyListener::MarkAsReplaced(ISpannable::Probe(chars), start, end, original);
    AutoPtr<ICharSequence> seq;
    mTokenizer->TerminateToken(text, (ICharSequence**)&seq);
    editable->Replace(start, end, seq);
}

void MultiAutoCompleteTextView::FinishInit()
{
    //do nothing;
}

//=========================================================================
//             MultiAutoCompleteTextView::CommaTokenizer
//=========================================================================
CAR_INTERFACE_IMPL(MultiAutoCompleteTextView::CommaTokenizer, Object, ITokenizer)

MultiAutoCompleteTextView::CommaTokenizer::CommaTokenizer()
{}

ECode MultiAutoCompleteTextView::CommaTokenizer::FindTokenStart(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 cursor,
    /* [out] */ Int32* start)
{
    Int32 i = cursor;
    Char32 c = 0;
    text->GetCharAt(i - 1, &c);
    while (i > 0 && c != ',') {
        i--;
        text->GetCharAt(i - 1, &c);
    }
    Char32 ci = 0;
    text->GetCharAt(i, &ci);
    while (i < cursor && ci == ' ') {
        i++;
        text->GetCharAt(i, &ci);
    }
    *start = i;
    return NOERROR;
}

ECode MultiAutoCompleteTextView::CommaTokenizer::FindTokenEnd(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 cursor,
    /* [out] */ Int32* end)
{
    Int32 i = cursor;
    Int32 len = 0;
    text->GetLength(&len);
    while (i < len) {
        Char32 c = 0;
        text->GetCharAt(i, &c);
        if (c == ',') {
            *end = i;
        } else {
            i++;
        }
    }
    *end = len;
    return NOERROR;
}

ECode MultiAutoCompleteTextView::CommaTokenizer::TerminateToken(
    /* [in] */ ICharSequence* text,
    /* [out] */ ICharSequence** res)
{
    Int32 i = 0;
    text->GetLength(&i);
    Char32 c = 0;
    text->GetCharAt(i - 1, &c);
    while (i > 0 && c == ' ') {
        i--;
        text->GetCharAt(i - 1, &c);
    }
    if (i > 0 && c == ',') {
        *res = text;
        // is right?
        REFCOUNT_ADD(*res);
    } else {
        if (ISpanned::Probe(text)) {
            String str;
            text->ToString(&str);
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(str + String(","));
            AutoPtr<ISpannable> sp;
            CSpannableString::New(csq, (ISpannable**)&sp);
            Int32 len = 0;
            text->GetLength(&len);
            TextUtils::CopySpansFrom(ISpanned::Probe(text), 0, len, EIID_IInterface, sp, 0);
            *res = ICharSequence::Probe(sp.Get());
            REFCOUNT_ADD(*res);
        } else {
            String str;
            text->ToString(&str);
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(str + String(","));
            *res = csq;
            REFCOUNT_ADD(*res);
        }
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos

