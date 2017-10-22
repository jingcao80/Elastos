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

#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CTextUtils.h"

namespace Elastos {
namespace Droid {
namespace Text {


CAR_INTERFACE_IMPL(CTextUtils, Singleton, ITextUtils)

CAR_SINGLETON_IMPL(CTextUtils)

ECode CTextUtils::GetChars(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    return TextUtils::GetChars(s, start, end, dest, destoff);
}

ECode CTextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IndexOf(s, ch);
    return NOERROR;
}

ECode CTextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 start,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IndexOf(s, ch, start);
    return NOERROR;
}

ECode CTextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IndexOf(s, ch, start, end);
    return NOERROR;
}

ECode CTextUtils::LastIndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::LastIndexOf(s, ch);
    return NOERROR;
}

ECode CTextUtils::LastIndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 start,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::LastIndexOf(s, ch, start);
    return NOERROR;
}

ECode CTextUtils::LastIndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ Char32 ch,
    /* [in] */ Int32 start,
    /* [in] */ Int32 last,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::LastIndexOf(s, ch, start, last);
    return NOERROR;
}


ECode CTextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ ICharSequence* needle,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IndexOf(s, needle);
    return NOERROR;
}

ECode CTextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ ICharSequence* needle,
    /* [in] */ Int32 start,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IndexOf(s, needle, start);
    return NOERROR;
}

ECode CTextUtils::IndexOf(
    /* [in] */ ICharSequence* s,
    /* [in] */ ICharSequence* needle,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IndexOf(s, needle, start, end);
    return NOERROR;
}

ECode CTextUtils::RegionMatches(
    /* [in] */ ICharSequence* one,
    /* [in] */ Int32 toffset,
    /* [in] */ ICharSequence* two,
    /* [in] */ Int32 ooffset,
    /* [in] */ Int32 len,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::RegionMatches(one, toffset, two, ooffset, len);
    return NOERROR;
}

ECode CTextUtils::Substring(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::Substring(source, start, end);
    return NOERROR;
}

ECode CTextUtils::Join(
    /* [in] */ ICharSequence* delimiter,
    /* [in] */ ArrayOf<IInterface*>* tokens,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::Join(delimiter, tokens);
    return NOERROR;
}

ECode CTextUtils::Join(
    /* [in] */ ICharSequence* delimiter,
    /* [in] */ IIterable* tokens,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::Join(delimiter, tokens);
    return NOERROR;
}

ECode CTextUtils::Split(
    /* [in] */ const String& text,
    /* [in] */ const String& expression,
    /* [out, callee] */ ArrayOf<String>** strs)
{
    VALIDATE_NOT_NULL(strs)
    AutoPtr<ArrayOf<String> > array = TextUtils::Split(text, expression);
    *strs = array;
    REFCOUNT_ADD(*strs)
    return NOERROR;
}

ECode CTextUtils::Split(
    /* [in] */ const String& text,
    /* [in] */ IPattern* pattern,
    /* [out, callee] */ ArrayOf<String>** strs)
{
    VALIDATE_NOT_NULL(strs)
    AutoPtr<ArrayOf<String> > array = TextUtils::Split(text, pattern);
    *strs = array;
    REFCOUNT_ADD(*strs)
    return NOERROR;
}

ECode CTextUtils::StringOrSpannedString(
    /* [in] */ ICharSequence* source,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::StringOrSpannedString(source);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::IsEmpty(
    /* [in] */ ICharSequence* str,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IsEmpty(str);
    return NOERROR;
}

ECode CTextUtils::IsEmpty(
    /* [in] */ const String& str,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IsEmpty(str);
    return NOERROR;
}

ECode CTextUtils::GetTrimmedLength(
    /* [in] */ ICharSequence* s,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::GetTrimmedLength(s);
    return NOERROR;
}

ECode CTextUtils::Equals(
    /* [in] */ ICharSequence* a,
    /* [in] */ ICharSequence* b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::Equals(a, b);
    return NOERROR;
}

ECode CTextUtils::Equals(
    /* [in] */ const String& a,
    /* [in] */ const String& b,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::Equals(a, b);
    return NOERROR;
}

ECode CTextUtils::GetReverse(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::GetReverse(source, start, end);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::WriteToParcel(
    /* [in] */ ICharSequence* cs,
    /* [in] */ IParcel* p)
{
    return TextUtils::WriteToParcel(cs, p);
}

ECode CTextUtils::Replace(
    /* [in] */ ICharSequence* temp,
    /* [in] */ ArrayOf<String>* sources,
    /* [in] */ ArrayOf<ICharSequence*>* destinations,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::Replace(temp, sources, destinations);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::ExpandTemplate(
    /* [in] */ ICharSequence* temp,
    /* [in] */ ArrayOf<ICharSequence*>* values,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::ExpandTemplate(temp, values);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::GetOffsetBefore(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::GetOffsetBefore(text, offset);
    return NOERROR;
}

ECode CTextUtils::GetOffsetAfter(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::GetOffsetAfter(text, offset);
    return NOERROR;
}

ECode CTextUtils::CopySpansFrom(
    /* [in] */ ISpanned* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& kind,
    /* [in] */ ISpannable* dest,
    /* [in] */ Int32 destoff)
{
    return TextUtils::CopySpansFrom(source, start, end, kind, dest, destoff);
}

ECode CTextUtils::Ellipsize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* p,
    /* [in] */ Float avail,
    /* [in] */ TextUtilsTruncateAt where,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::Ellipsize(text, p, avail, where);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::Ellipsize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Float avail,
    /* [in] */ TextUtilsTruncateAt where,
    /* [in] */ Boolean preserveLength,
    /* [in] */ ITextUtilsEllipsizeCallback* callback,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::Ellipsize(text, paint, avail, where, preserveLength, callback);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::CommaEllipsize(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* p,
    /* [in] */ Float avail,
    /* [in] */ const String& oneMore,
    /* [in] */ const String& more,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::CommaEllipsize(text, p, avail, oneMore, more);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::HtmlEncode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::HtmlEncode(s);
    return NOERROR;
}

ECode CTextUtils::Concat(
    /* [in] */ ArrayOf<ICharSequence*>* text,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICharSequence> csq = TextUtils::Concat(text);
    *result = csq;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTextUtils::IsGraphic(
    /* [in] */ ICharSequence* str,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IsGraphic(str);
    return NOERROR;
}

ECode CTextUtils::IsGraphic(
    /* [in] */ Char32 c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IsGraphic(c);
    return NOERROR;
}

ECode CTextUtils::IsDigitsOnly(
    /* [in] */ ICharSequence* str,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::IsDigitsOnly(str);
    return NOERROR;
}

ECode CTextUtils::GetCapsMode(
    /* [in] */ ICharSequence* cs,
    /* [in] */ Int32 off,
    /* [in] */ Int32 reqModes,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::GetCapsMode(cs, off, reqModes);
    return NOERROR;
}

ECode CTextUtils::GetLayoutDirectionFromLocale(
    /* [in] */ ILocale* locale,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TextUtils::GetLayoutDirectionFromLocale(locale);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
