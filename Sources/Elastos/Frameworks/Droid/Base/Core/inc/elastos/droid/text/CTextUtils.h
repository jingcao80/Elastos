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

#ifndef __ELASTOS_DROID_TEXT_CTEXTUTILS_H__
#define __ELASTOS_DROID_TEXT_CTEXTUTILS_H__

#include "_Elastos_Droid_Text_CTextUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::ICharSequence;
using Elastos::Utility::ILocale;
using Elastos::Utility::IIterable;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Text {

CarClass(CTextUtils)
    , public Singleton
    , public ITextUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetChars(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ArrayOf<Char32>* dest,
        /* [in] */ Int32 destoff);

    CARAPI IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [out] */ Int32* result);

    CARAPI IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 start,
        /* [out] */ Int32* result);

    CARAPI IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Int32* result);

    CARAPI LastIndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [out] */ Int32* result);

    CARAPI LastIndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 start,
        /* [out] */ Int32* result);

    CARAPI LastIndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ Char32 ch,
        /* [in] */ Int32 start,
        /* [in] */ Int32 last,
        /* [out] */ Int32* result);

    CARAPI IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ ICharSequence* needle,
        /* [out] */ Int32* result);

    CARAPI IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ ICharSequence* needle,
        /* [in] */ Int32 start,
        /* [out] */ Int32* result);

    CARAPI IndexOf(
        /* [in] */ ICharSequence* s,
        /* [in] */ ICharSequence* needle,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ Int32* result);

    CARAPI RegionMatches(
        /* [in] */ ICharSequence* one,
        /* [in] */ Int32 toffset,
        /* [in] */ ICharSequence* two,
        /* [in] */ Int32 ooffset,
        /* [in] */ Int32 len,
        /* [out] */ Boolean* result);

    CARAPI Substring(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ String* result);

    CARAPI Join(
        /* [in] */ ICharSequence* delimiter,
        /* [in] */ ArrayOf<IInterface*>* tokens,
        /* [out] */ String* result);

    CARAPI Join(
        /* [in] */ ICharSequence* delimiter,
        /* [in] */ IIterable* tokens,
        /* [out] */ String* result);

    CARAPI Split(
        /* [in] */ const String& text,
        /* [in] */ const String& expression,
        /* [out, callee] */ ArrayOf<String>** strs);

    CARAPI Split(
        /* [in] */ const String& text,
        /* [in] */ IPattern* pattern,
        /* [out, callee] */ ArrayOf<String>** strs);

    CARAPI StringOrSpannedString(
        /* [in] */ ICharSequence* source,
        /* [out] */ ICharSequence** result);

    CARAPI IsEmpty(
        /* [in] */ ICharSequence* str,
        /* [out] */ Boolean* result);

    CARAPI IsEmpty(
        /* [in] */ const String& str,
        /* [out] */ Boolean* result);

    CARAPI GetTrimmedLength(
        /* [in] */ ICharSequence* s,
        /* [out] */ Int32* result);

    using Object::Equals;

    CARAPI Equals(
        /* [in] */ ICharSequence* a,
        /* [in] */ ICharSequence* b,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ const String& a,
        /* [in] */ const String& b,
        /* [out] */ Boolean* result);

    CARAPI  GetReverse(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** result);

    CARAPI WriteToParcel(
        /* [in] */ ICharSequence* cs,
        /* [in] */ IParcel* p);

    CARAPI Replace(
        /* [in] */ ICharSequence* temp,
        /* [in] */ ArrayOf<String>* sources,
        /* [in] */ ArrayOf<ICharSequence*>* destinations,
        /* [out] */ ICharSequence** result);

    CARAPI ExpandTemplate(
        /* [in] */ ICharSequence* temp,
        /* [in] */ ArrayOf<ICharSequence*>* values,
        /* [out] */ ICharSequence** result);

    CARAPI GetOffsetBefore(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    CARAPI GetOffsetAfter(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    CARAPI CopySpansFrom(
        /* [in] */ ISpanned* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ const InterfaceID& kind,
        /* [in] */ ISpannable* dest,
        /* [in] */ Int32 destoff);

    CARAPI Ellipsize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* p,
        /* [in] */ Float avail,
        /* [in] */ TextUtilsTruncateAt where,
        /* [out] */ ICharSequence** result);

    CARAPI Ellipsize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Float avail,
        /* [in] */ TextUtilsTruncateAt where,
        /* [in] */ Boolean preserveLength,
        /* [in] */ ITextUtilsEllipsizeCallback* callback,
        /* [out] */ ICharSequence** result);

    CARAPI CommaEllipsize(
        /* [in] */ ICharSequence* text,
        /* [in] */ ITextPaint* p,
        /* [in] */ Float avail,
        /* [in] */ const String& oneMore,
        /* [in] */ const String& more,
        /* [out] */ ICharSequence** result);

    CARAPI HtmlEncode(
        /* [in] */ const String& s,
        /* [out] */ String* result);

    CARAPI Concat(
        /* [in] */ ArrayOf<ICharSequence*>* text,
        /* [out] */ ICharSequence** result);

    CARAPI IsGraphic(
        /* [in] */ ICharSequence* str,
        /* [out] */ Boolean* result);

    CARAPI IsGraphic(
        /* [in] */ Char32 c,
        /* [out] */ Boolean* result);

    CARAPI IsDigitsOnly(
        /* [in] */ ICharSequence* str,
        /* [out] */ Boolean* result);

    CARAPI GetCapsMode(
        /* [in] */ ICharSequence* cs,
        /* [in] */ Int32 off,
        /* [in] */ Int32 reqModes,
        /* [out] */ Int32* result);

    CARAPI GetLayoutDirectionFromLocale(
        /* [in] */ ILocale* locale,
        /* [out] */ Int32* result);
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_CTEXTUTILS_H__
