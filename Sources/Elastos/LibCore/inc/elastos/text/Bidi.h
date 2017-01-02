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

#ifndef __ELASTOS_TEXT_BIDI_H__
#define __ELASTOS_TEXT_BIDI_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Text::IAttributedCharacterIterator;;

namespace Elastos {
namespace Text {

/**
 * Implements the <a href="http://unicode.org/reports/tr9/">Unicode Bidirectional Algorithm</a>.
 *
 * <p>Use a {@code Bidi} object to get the information on the position reordering of a
 * bidirectional text, such as Arabic or Hebrew. The natural display ordering of
 * horizontal text in these languages is from right to left, while they order
 * numbers from left to right.
 *
 * <p>If the text contains multiple runs, the information of each run can be
 * obtained from the run index. The level of any particular run indicates the
 * direction of the text as well as the nesting level. Left-to-right runs have
 * even levels while right-to-left runs have odd levels.
 */
class Bidi
    : public Object
    , public IBidi
{
public:
    class BidiRun : public Object {
    public:
        BidiRun(
            /* [in] */ Int32 start,
            /* [in] */ Int32 limit,
            /* [in] */ Int32 level);

        CARAPI_(Int32) GetLevel();

        CARAPI_(Int32) GetLimit();

        CARAPI_(Int32) GetStart();

    private:
        Int32 mStart;
        Int32 mLimit;
        Int32 mLevel;
    };

public:
    CAR_INTERFACE_DECL()

    Bidi();

    virtual ~Bidi();

    /**
     * Creates a {@code Bidi} object from the {@code
     * AttributedCharacterIterator} of a paragraph text. The RUN_DIRECTION
     * attribute determines the base direction of the bidirectional text. If it
     * is not specified explicitly, the algorithm uses
     * DIRECTION_DEFAULT_LEFT_TO_RIGHT by default. The BIDI_EMBEDDING attribute
     * specifies the level of embedding for each character. Values between -1
     * and -62 denote overrides at the level's absolute value, values from 1 to
     * 62 indicate embeddings, and the 0 value indicates the level is calculated
     * by the algorithm automatically. For the character with no BIDI_EMBEDDING
     * attribute or with a improper attribute value, such as a {@code null}
     * value, the algorithm treats its embedding level as 0. The NUMERIC_SHAPING
     * attribute specifies the instance of NumericShaper used to convert
     * European digits to other decimal digits before performing the bidi
     * algorithm.
     *
     * @param paragraph
     *            the String containing the paragraph text to perform the
     *            algorithm.
     * @throws IllegalArgumentException if {@code paragraph == null}
     * @see java.awt.font.TextAttribute#BIDI_EMBEDDING
     * @see java.awt.font.TextAttribute#NUMERIC_SHAPING
     * @see java.awt.font.TextAttribute#RUN_DIRECTION
     */
    CARAPI constructor(
        /* [in] */ IAttributedCharacterIterator* paragraph);

    /**
     * Creates a {@code Bidi} object.
     *
     * @param text
     *            the char array of the paragraph text that is processed.
     * @param textStart
     *            the index in {@code text} of the start of the paragraph.
     * @param embeddings
     *            the embedding level array of the paragraph text, specifying
     *            the embedding level information for each character. Values
     *            between -1 and -61 denote overrides at the level's absolute
     *            value, values from 1 to 61 indicate embeddings, and the 0
     *            value indicates the level is calculated by the algorithm
     *            automatically.
     * @param embStart
     *            the index in {@code embeddings} of the start of the paragraph.
     * @param paragraphLength
     *            the length of the text to perform the algorithm.
     * @param flags
     *            indicates the base direction of the bidirectional text. It is
     *            expected that this will be one of the direction constant
     *            values defined in this class. An unknown value is treated as
     *            DIRECTION_DEFAULT_LEFT_TO_RIGHT.
     * @throws IllegalArgumentException
     *             if {@code textStart}, {@code embStart}, or {@code
     *             paragraphLength} is negative; if
     *             {@code text.length < textStart + paragraphLength} or
     *             {@code embeddings.length < embStart + paragraphLength}.
     * @see #DIRECTION_LEFT_TO_RIGHT
     * @see #DIRECTION_RIGHT_TO_LEFT
     * @see #DIRECTION_DEFAULT_RIGHT_TO_LEFT
     * @see #DIRECTION_DEFAULT_LEFT_TO_RIGHT
     */
     CARAPI constructor(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 textStart,
        /* [in] */ ArrayOf<Byte>* embeddings,
        /* [in] */ Int32 embStart,
        /* [in] */ Int32 paragraphLength,
        /* [in] */ Int32 flags);


    /**
     * Creates a {@code Bidi} object.
     *
     * @param paragraph
     *            the string containing the paragraph text to perform the
     *            algorithm on.
     * @param flags
     *            indicates the base direction of the bidirectional text. It is
     *            expected that this will be one of the direction constant
     *            values defined in this class. An unknown value is treated as
     *            DIRECTION_DEFAULT_LEFT_TO_RIGHT.
     * @see #DIRECTION_LEFT_TO_RIGHT
     * @see #DIRECTION_RIGHT_TO_LEFT
     * @see #DIRECTION_DEFAULT_RIGHT_TO_LEFT
     * @see #DIRECTION_DEFAULT_LEFT_TO_RIGHT
     */
    CARAPI constructor(
        /* [in] */ const String& paragraph,
        /* [in] */ Int32 flags);

    /* private constructor used by createLineBidi() */
    CARAPI constructor(
        /* [in] */ Int64 pBidi);

    /**
     * Returns whether the base level is from left to right.
     *
     * @return true if the base level is from left to right.
     */
    CARAPI BaseIsLeftToRight(
        /* [out] */ Boolean* isLeftToRight);

    /**
     * Creates a new {@code Bidi} object containing the information of one line
     * from this object.
     *
     * @param lineStart
     *            the start offset of the line.
     * @param lineLimit
     *            the limit of the line.
     * @return the new line Bidi object. In this new object, the indices will
     *         range from 0 to (limit - start - 1).
     * @throws IllegalArgumentException
     *             if {@code lineStart < 0}, {@code lineLimit < 0}, {@code
     *             lineStart > lineLimit} or if {@code lineStart} is greater
     *             than the length of this object's paragraph text.
     */
    CARAPI CreateLineBidi(
        /* [in] */ Int32 lineStart,
        /* [in] */ Int32 lineLimit,
        /* [out] */ IBidi** lineBidi);

    CARAPI GetBaseLevel(
        /* [out] */ Int32* baseLevel);

    CARAPI GetLength(
        /* [out] */ Int32* length);

    /**
     * Returns the level of a specified character.
     *
     * @param offset
     *            the offset of the character.
     * @return the level.
     */
    CARAPI GetLevelAt(
        /* [in] */ Int32 offset,
        /* [out] */ Int32* level);

    CARAPI GetRunCount(
        /* [out] */ Int32* runCount);

    CARAPI GetRunLevel(
        /* [in] */ Int32 run,
        /* [out] */ Int32* runLevel);

    CARAPI GetRunLimit(
        /* [in] */ Int32 run,
        /* [out] */ Int32* runLimit);

    CARAPI GetRunStart(
        /* [in] */ Int32 run,
        /* [out] */ Int32* runStart);

    CARAPI IsLeftToRight(
        /* [out] */ Boolean* isLefttoRight);

    CARAPI IsMixed(
        /* [out] */ Boolean* isMixed);

    CARAPI IsRightToLeft(
        /* [out] */ Boolean* isRightToLeft);

    static CARAPI ReorderVisually(
        /* [in] */ ArrayOf<Byte>* levels,
        /* [in] */ Int32 levelStart,
        /* [in] */ ArrayOf<IInterface*>* objects,
        /* [in] */ Int32 objectStart,
        /* [in] */ Int32 count);

    static CARAPI RequiresBidi(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [out] */ Boolean * result);

private:
    static CARAPI CreateUBiDi(
        /* [in] */ ArrayOf<Char32>* texts,
        /* [in] */ Int32 textStart,
        /* [in] */ ArrayOf<Byte>* embeddings,
        /* [in] */ Int32 embStart,
        /* [in] */ Int32 paragraphLength,
        /* [in] */ Int32 flags,
        /* [out] */ Int64* uBiDi);

    CARAPI_(void) ReadBidiInfo(
        /* [in] */ Int64 pBidi);

    CARAPI CreateEmptyLineBidi(
        /* [in] */ Int64 parent,
        /* [out] */ IBidi** lineBidi);

    static CARAPI_(Int64) Ubidi_open();

    static CARAPI_(void) Ubidi_close(
        /* [in] */ Int64 pBiDi);

    static CARAPI Ubidi_setPara(
        /* [in] */ Int64 pBiDi,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 length,
        /* [in] */ Int32 paraLevel,
        /* [in] */ ArrayOf<Byte>* embeddingLevels);

    static CARAPI Ubidi_setLine(
        /* [in] */ Int64 pParaBiDi,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [out] */ Int64* line);

    static CARAPI_(Int32) Ubidi_getDirection(
        /* [in] */ Int64 pBiDi);

    static CARAPI_(Int32) Ubidi_getLength(
        /* [in] */ Int64 pBiDi);

    static CARAPI_(Byte) Ubidi_getParaLevel(
        /* [in] */ Int64 pBiDi);

    static CARAPI Ubidi_getLevels(
        /* [in] */ Int64 pBiDi,
        /* [out] */ ArrayOf<Byte>** levels);

    static CARAPI Ubidi_countRuns(
        /* [in] */ Int64 pBiDi,
        /* [out] */ Int32* count);

    static CARAPI Ubidi_getRuns(
        /* [in] */ Int64 pBidi,
        /* [out, callee] */ ArrayOf<BidiRun*>** runs);

public:

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) Ubidi_reorderVisual(
        /* [in] */ ArrayOf<Byte>* levelBytes,
        /* [in] */ Int32 length);

private:
    Int32 mBaseLevel;

    Int32 mLength;

    AutoPtr<ArrayOf<Byte> > mOffsetLevel;

    AutoPtr<ArrayOf<BidiRun*> > mRuns;

    Int32 mDirection;

    Boolean mUnidirectional;

    // const static Int32 UBIDI_LEVEL_OVERRIDE;
    const static Int32 UBiDiDirection_UBIDI_LTR;
    const static Int32 UBiDiDirection_UBIDI_RTL;
    const static Int32 UBiDiDirection_UBIDI_MIXED;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_BIDI_H__
