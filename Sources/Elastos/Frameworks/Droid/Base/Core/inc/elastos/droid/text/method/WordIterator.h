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

#ifndef __ELASTOS_DROID_TEXT_METHOD_WORDITERATOR_H__
#define __ELASTOS_DROID_TEXT_METHOD_WORDITERATOR_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Text::IBreakIterator;
using Elastos::Utility::ILocale;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * Walks through cursor positions at word boundaries. Internally uses
 * {@link BreakIterator#getWordInstance()}, and caches {@link CharSequence}
 * for performance reasons.
 *
 * Also provides methods to determine word boundaries.
 * {@hide}
 */
//public
class WordIterator
    : public Object
    , public IWordIterator
    , public ISelectionPositionIterator
{
public:
    /**
     * Constructs a WordIterator using the default locale.
     */
    WordIterator();

    virtual ~WordIterator();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Constructs a new WordIterator for the specified locale.
     * @param locale The locale to be used when analysing the text.
     */
    CARAPI constructor(
        /* [in] */ ILocale* locale);

    CARAPI/*CARAPI_(void)*/ SetCharSequence(
        /* [in] */ ICharSequence* charSequence,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /** {@inheritDoc} */
    CARAPI Preceding(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* ret);

    /** {@inheritDoc} */
    CARAPI Following(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* ret);

    /** If <code>offset</code> is within a word, returns the index of the first character of that
     * word, otherwise returns BreakIterator.DONE.
     *
     * The offsets that are considered to be part of a word are the indexes of its characters,
     * <i>as well as</i> the index of its last character plus one.
     * If offset is the index of a low surrogate character, BreakIterator.DONE will be returned.
     *
     * Valid range for offset is [0..textLength] (note the inclusive upper bound).
     * The returned value is within [0..offset] or BreakIterator.DONE.
     *
     * @throws IllegalArgumentException is offset is not valid.
     */
    CARAPI GetBeginning(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* ret);

    /** If <code>offset</code> is within a word, returns the index of the last character of that
     * word plus one, otherwise returns BreakIterator.DONE.
     *
     * The offsets that are considered to be part of a word are the indexes of its characters,
     * <i>as well as</i> the index of its last character plus one.
     * If offset is the index of a low surrogate character, BreakIterator.DONE will be returned.
     *
     * Valid range for offset is [0..textLength] (note the inclusive upper bound).
     * The returned value is within [offset..textLength] or BreakIterator.DONE.
     *
     * @throws IllegalArgumentException is offset is not valid.
     */
    CARAPI GetEnd(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* ret);

private:
    CARAPI_(Boolean) IsAfterLetterOrDigit(
        /* [in] */ Int32 shiftedOffset);

    CARAPI_(Boolean) IsOnLetterOrDigit(
        /* [in] */ Int32 shiftedOffset);

    CARAPI_(void) CheckOffsetIsValid(
        /* [in] */ Int32 shiftedOffset);

private:
    // Size of the window for the word iterator, should be greater than the longest word's length
    static const Int32 WINDOW_WIDTH;
    String mString;
    Int32 mOffsetShift;
    AutoPtr<IBreakIterator> mIterator;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_WORDITERATOR_H__
