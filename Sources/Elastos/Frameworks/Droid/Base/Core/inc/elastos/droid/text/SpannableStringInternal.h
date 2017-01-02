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

#ifndef __ELASTOS_DROID_TEXT_SPANNABLESTRINGINTERNAL_H__
#define __ELASTOS_DROID_TEXT_SPANNABLESTRINGINTERNAL_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

class SpannableStringInternal
    : public Object
{
public:
    SpannableStringInternal();

    virtual ~SpannableStringInternal();

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    virtual CARAPI GetLength(
        /* [out] */ Int32* length);

    virtual CARAPI GetCharAt(
        /* [in] */ Int32 i,
        /* [out] */ Char32* ch);

    virtual CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetChars(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ArrayOf<Char32>* dest,
        /* [in] */ Int32 off);

    /* package */
    CARAPI SetSpan(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 flags);

    /* package */
    CARAPI RemoveSpan(
        /* [in] */ IInterface* what);

    CARAPI GetSpanStart(
        /* [in] */ IInterface* what,
        /* [out] */ Int32* result);

    CARAPI GetSpanEnd(
        /* [in] */ IInterface* what,
        /* [out] */ Int32* result);

    CARAPI GetSpanFlags(
        /* [in] */ IInterface* what,
        /* [out] */ Int32* result);

    CARAPI GetSpans(
        /* [in] */ Int32 queryStart,
        /* [in] */ Int32 queryEnd,
        /* [in] */ const InterfaceID& kind,
        /* [out, callee] */ ArrayOf<IInterface*>** objs);

    CARAPI NextSpanTransition(
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ const InterfaceID& kind,
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

protected:

    CARAPI_(Boolean) IsSpanEquals(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs);

private:
    CARAPI SendSpanAdded(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI SendSpanRemoved(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI SendSpanChanged(
        /* [in] */ IInterface* what,
        /* [in] */ Int32 s,
        /* [in] */ Int32 e,
        /* [in] */ Int32 st,
        /* [in] */ Int32 en);

    static CARAPI Region(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ String* str);

    CARAPI CheckRange(
        /* [in] */ const char* operation,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

public:
    /* package */
    static const AutoPtr< ArrayOf<IInterface*> > EMPTY;

private:
    String mText;
    AutoPtr< ArrayOf<IInterface*> > mSpans;
    AutoPtr< ArrayOf<Int32> > mSpanData;
    Int32 mSpanCount;

    static const Int32 START = 0;
    static const Int32 END = 1;
    static const Int32 FLAGS = 2;
    static const Int32 COLUMNS = 3;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_SPANNABLESTRINGINTERNAL_H__
