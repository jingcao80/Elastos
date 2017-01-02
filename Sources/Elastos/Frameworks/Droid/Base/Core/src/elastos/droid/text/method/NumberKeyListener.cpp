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

#include "Elastos.Droid.View.h"
#include "elastos/droid/text/method/NumberKeyListener.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/Selection.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_INTERFACE_IMPL_2(NumberKeyListener, BaseKeyListener, INumberKeyListener, IInputFilter)

NumberKeyListener::NumberKeyListener()
{}

NumberKeyListener::~NumberKeyListener()
{}

Int32 NumberKeyListener::Lookup(
    /* [in] */ IKeyEvent* event,
    /* [in] */ ISpannable* content)
{
    Int32 metaState;
    MetaKeyKeyListener::GetMetaState(ICharSequence::Probe(content), event, &metaState);
    AutoPtr< ArrayOf<Char32> > accepedChars = GetAcceptedChars();
    Char32 c;
    event->GetMatch(accepedChars.Get(), metaState, &c);
    return (Int32)c;
}

ECode NumberKeyListener::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** ret)
{
    VALIDATE_NOT_NULL(ret)

    AutoPtr< ArrayOf<Char32> > accept = GetAcceptedChars();

    Int32 i;
    for (i = start; i < end; i++) {
        Char32 c;
        if (!Ok(accept, (source->GetCharAt(i, &c), c))) {
            break;
        }
    }

    if (i == end) {
        // It was all OK.
        *ret = NULL;
        return NOERROR;
    }

    if (end - start == 1) {
        // It was not OK, and there is only one char, so nothing remains.
        AutoPtr<ICharSequence> cs;
        CString::New(String(""), (ICharSequence**)&cs);
        *ret = cs;
        REFCOUNT_ADD(*ret);
        return NOERROR;
    }

    AutoPtr<ISpannableStringBuilder> filtered;
    CSpannableStringBuilder::New(source, start, end, (ISpannableStringBuilder**)&filtered);
    i -= start;
    end -= start;

    Int32 UNUSED(len) = end - start;
    // Only count down to i because the chars before that were all OK.
    for (Int32 j = end - 1; j >= i; j--) {
        Char32 c;
        if (!Ok(accept, (source->GetCharAt(i, &c), c))) {
            IEditable::Probe(filtered)->Delete(j, j + 1);
        }
    }

    *ret = ICharSequence::Probe(filtered);
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

Boolean NumberKeyListener::Ok(
    /* [in] */ ArrayOf<Char32>* accept,
    /* [in] */ Char32 c)
{
    for (Int32 i = accept->GetLength() - 1; i >= 0; i--) {
        if ((*accept)[i] == c) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode NumberKeyListener::OnKeyDown(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    Int32 selStart, selEnd;

    {
        Int32 a = Selection::GetSelectionStart(ICharSequence::Probe(content));
        Int32 b = Selection::GetSelectionEnd(ICharSequence::Probe(content));

        selStart = Elastos::Core::Math::Min(a, b);
        selEnd = Elastos::Core::Math::Max(a, b);
    }

    if (selStart < 0 || selEnd < 0) {
        selStart = selEnd = 0;
        Selection::SetSelection(ISpannable::Probe(content), 0);
    }

    Int32 i = event != NULL ? Lookup(event, ISpannable::Probe(content)) : 0;
    Int32 repeatCount = event != NULL ? event->GetRepeatCount(&repeatCount) : 0;
    if (repeatCount == 0) {
        if (i != 0) {
            if (selStart != selEnd) {
                Selection::SetSelection(ISpannable::Probe(content), selEnd);
            }
            String str("");
            str.Append((Char32)i);
            AutoPtr<ICharSequence> cs;
            CString::New(str, (ICharSequence**)&cs);
            content->Replace(selStart, selEnd, cs);

            AdjustMetaAfterKeypress(ISpannable::Probe(content));
            *ret = TRUE;
            return NOERROR;
        }
    }
    else if (i == '0' && repeatCount == 1) {
        // Pretty hackish, it replaces the 0 with the +
        Char32 c;
        if (selStart == selEnd && selEnd > 0 && (ICharSequence::Probe(content)->GetCharAt(selStart - 1, &c), c) == '0') {

            AutoPtr<ICharSequence> cs;
            CString::New(String("+"), (ICharSequence**)&cs);
            content->Replace(selStart - 1, selEnd, cs);
            AdjustMetaAfterKeypress(ISpannable::Probe(content));
            *ret = TRUE;
            return NOERROR;
        }
    }

    AdjustMetaAfterKeypress(ISpannable::Probe(content));
    Boolean flag(FALSE);
    BaseKeyListener::OnKeyDown(view, content, keyCode, event, &flag);
    *ret = flag;
    return NOERROR;
}


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos
