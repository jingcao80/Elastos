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

#include "elastos/droid/text/CSpannableString.h"

using Elastos::Core::EIID_ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_5(CSpannableString, SpannableStringInternal, ISpannableString, ICharSequence, IGetChars, ISpannable, ISpanned)

CAR_OBJECT_IMPL(CSpannableString)

ECode CSpannableString::constructor(
    /* [in] */ ICharSequence* source)
{
    VALIDATE_NOT_NULL(source);

    Int32 len;
    source->GetLength(&len);
    return SpannableStringInternal::constructor(source, 0, len);
}

ECode CSpannableString::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return SpannableStringInternal::constructor(source, start, end);
}

ECode CSpannableString::GetLength(
    /* [out] */ Int32* number)
{
    return SpannableStringInternal::GetLength(number);
}

ECode CSpannableString::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    return SpannableStringInternal::GetCharAt(index, c);
}

ECode CSpannableString::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    *csq = NULL;

    if (start < 0 || end < 0 || start > end) {
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    AutoPtr<ISpannableString> sub;
    CSpannableString::New(this, start, end, (ISpannableString**)&sub);
    *csq = ICharSequence::Probe(sub);
    REFCOUNT_ADD(*csq);
    return NOERROR;
}

ECode CSpannableString::ToString(
    /* [out] */ String* str)
{
    return SpannableStringInternal::ToString(str);
}

ECode CSpannableString::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    return SpannableStringInternal::GetChars(start, end, dest, destoff);
}

ECode CSpannableString::GetSpans(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& type,
    /* [out, callee] */ ArrayOf<IInterface*>** objs)
{
    return SpannableStringInternal::GetSpans(start, end, type, objs);
}

ECode CSpannableString::GetSpanStart(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* start)
{
    return SpannableStringInternal::GetSpanStart(tag, start);
}

ECode CSpannableString::GetSpanEnd(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* end)
{
    return SpannableStringInternal::GetSpanEnd(tag, end);
}

ECode CSpannableString::GetSpanFlags(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* flags)
{
    return SpannableStringInternal::GetSpanFlags(tag, flags);
}

ECode CSpannableString::NextSpanTransition(
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ const InterfaceID& type,
    /* [out] */ Int32* offset)
{
    return SpannableStringInternal::NextSpanTransition(start, limit, type, offset);
}

ECode CSpannableString::SetSpan(
    /* [in] */ IInterface* what,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 flags)
{
    return SpannableStringInternal::SetSpan(what, start, end, flags);
}

ECode CSpannableString::RemoveSpan(
    /* [in] */ IInterface* what)
{
    return SpannableStringInternal::RemoveSpan(what);
}

AutoPtr<ISpannableString> CSpannableString::ValueOf(
        /* [in] */ ICharSequence* source)
{
    AutoPtr<ISpannableString> ss = ISpannableString::Probe(source);
    if (ss != NULL) {
        return ss;
    }

    AutoPtr<CSpannableString> css;
    CSpannableString::NewByFriend(source, (CSpannableString**)&css);
    return css;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
