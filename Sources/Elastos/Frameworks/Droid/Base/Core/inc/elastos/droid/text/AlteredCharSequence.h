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

#ifndef __ELASTOS_DROID_TEXT_ALTEREDCHARSEQUENCE_H__
#define __ELASTOS_DROID_TEXT_ALTEREDCHARSEQUENCE_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

// XXX should this really be in the public API at all?
/**
 * An AlteredCharSequence is a CharSequence that is largely mirrored from
 * another CharSequence, except that a specified range of characters are
 * mirrored from a different char array instead.
 */
class AlteredCharSequence
    : public Object
    , public IAlteredCharSequence
    , public IGetChars
    , public ICharSequence
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create an AlteredCharSequence whose text (and possibly spans)
     * are mirrored from <code>source</code>, except that the range of
     * offsets <code>substart</code> inclusive to <code>subend</code> exclusive
     * are mirrored instead from <code>sub</code>, beginning at offset 0.
     */
    static AutoPtr<IAlteredCharSequence> Make(
        /* [in] */ ICharSequence* source,
        /* [in] */ ArrayOf<Char32>* sub,
        /* [in] */ Int32 substart,
        /* [in] */ Int32 subend);

    AlteredCharSequence();

    virtual ~AlteredCharSequence();

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ ArrayOf<Char32>* sub,
        /* [in] */ Int32 substart,
        /* [in] */ Int32 subend);

    /* package */
    CARAPI_(void) Update(
        /* [in] */ ArrayOf<Char32>* sub,
        /* [in] */ Int32 substart,
        /* [in] */ Int32 subend);

public:
    CARAPI GetCharAt(
        /* [in] */ Int32 off,
        /* [out] */ Char32* c);

    CARAPI GetLength(
        /* [out] */ Int32* number);

    CARAPI SubSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [out] */ ICharSequence** csq);

    CARAPI GetChars(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ArrayOf<Char32>* dest,
        /* [in] */ Int32 off);

    CARAPI ToString(
            /* [out] */ String* str);

protected://private:
    Int32 mStart;
    Int32 mEnd;
    AutoPtr< ArrayOf<Char32> > mChars;
    AutoPtr<ICharSequence> mSource;
};

class AlteredSpanned
    : public AlteredCharSequence
    , public ISpanned
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ ICharSequence* source,
        /* [in] */ ArrayOf<Char32>* sub,
        /* [in] */ Int32 substart,
        /* [in] */ Int32 subend);

    CARAPI GetSpans(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ const InterfaceID& type,
        /*[out, callee]*/ ArrayOf<IInterface*>** objs);

    CARAPI GetSpanStart(
        /* [in] */ IInterface* tag,
        /* [out] */ Int32* start);

    CARAPI GetSpanEnd(
        /* [in] */ IInterface* tag,
        /* [out] */ Int32* end);

    CARAPI GetSpanFlags(
        /* [in] */ IInterface* tag,
        /* [out] */ Int32* flags);

    CARAPI NextSpanTransition(
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ const InterfaceID& type,
        /* [out] */ Int32* offset);

    CARAPI ToString(
            /* [out] */ String* str);

private:
    AutoPtr<ISpanned> mSpanned;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_ALTEREDCHARSEQUENCE_H__
