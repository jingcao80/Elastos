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
#include "elastos/droid/text/CBidiFormatter.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::View::IView;
using Elastos::Core::Character;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Text {

static String StringFromChar32(
    /* [in] */ Char32 ch)
{
    StringBuilder sb(4);
    sb.AppendChar(ch);
    return sb.ToString();
}

AutoPtr<ITextDirectionHeuristic> CBidiFormatter::DEFAULT_TEXT_DIRECTION_HEURISTIC = TextDirectionHeuristics::FIRSTSTRONG_LTR;

const Char32 CBidiFormatter::LRE = 0x202A;
const Char32 CBidiFormatter::RLE = 0x202B;
const Char32 CBidiFormatter::PDF = 0x202C;
const Char32 CBidiFormatter::LRM = 0x200E;
const Char32 CBidiFormatter::RLM = 0x200F;

const String CBidiFormatter::LRM_STRING = StringFromChar32(0x200E);//Character.toString(LRM);
const String CBidiFormatter::RLM_STRING = StringFromChar32(0x200F);//Character.toString(RLM);

const String CBidiFormatter::EMPTY_STRING("");

const Int32 CBidiFormatter::FLAG_STEREO_RESET = 2;
const Int32 CBidiFormatter::DEFAULT_FLAGS = 2;//FLAG_STEREO_RESET;

static AutoPtr<IBidiFormatter> InitBidiFormatter(
    /* [in] */ Boolean isRtlContext,
    /* [in] */ Int32 flags,
    /* [in] */ ITextDirectionHeuristic* heuristic)
{
    AutoPtr<IBidiFormatter> bf;
    CBidiFormatter::New(isRtlContext, flags, heuristic, (IBidiFormatter**)&bf);
    return bf;
}

const AutoPtr<IBidiFormatter> CBidiFormatter::DEFAULT_LTR_INSTANCE = InitBidiFormatter(
        FALSE /* LTR context */,
        DEFAULT_FLAGS,
        DEFAULT_TEXT_DIRECTION_HEURISTIC);

const AutoPtr<IBidiFormatter> CBidiFormatter::DEFAULT_RTL_INSTANCE = InitBidiFormatter(
        TRUE /* RTL context */,
        DEFAULT_FLAGS,
        DEFAULT_TEXT_DIRECTION_HEURISTIC);

const Int32 CBidiFormatter::DIR_LTR = -1;
const Int32 CBidiFormatter::DIR_UNKNOWN = 0;
const Int32 CBidiFormatter::DIR_RTL = +1;

const Int32 CBidiFormatter::DirectionalityEstimator::DIR_TYPE_CACHE_SIZE = 0x700;

AutoPtr< ArrayOf<Byte> > CBidiFormatter::DirectionalityEstimator::DIR_TYPE_CACHE;

Boolean CBidiFormatter::DirectionalityEstimator::Init()
{
    DIR_TYPE_CACHE = ArrayOf<Byte>::Alloc(DIR_TYPE_CACHE_SIZE);
    for (Int32 i = 0; i < DIR_TYPE_CACHE_SIZE; i++) {
        (*DIR_TYPE_CACHE)[i] = Character::GetDirectionality(i);
    }
    return TRUE;
}

Boolean CBidiFormatter::DirectionalityEstimator::InitStatic = Init();

//===============================================================================
//                  CBidiFormatter::DirectionalityEstimator
//===============================================================================

CBidiFormatter::DirectionalityEstimator::DirectionalityEstimator(
    /* [in] */ const String& text,
    /* [in] */ Boolean isHtml)
    : mIsHtml(FALSE)
    , mLength(0)
    , mCharIndex(0)
    , mLastChar(0)
{
    mText = text;
    mIsHtml = isHtml;
    mLength = text.GetLength();
}

CBidiFormatter::DirectionalityEstimator::~DirectionalityEstimator()
{}

Int32 CBidiFormatter::DirectionalityEstimator::GetEntryDir()
{
    // The reason for this method name, as opposed to getFirstStrongDir(), is that
    // "first strong" is a commonly used description of Unicode's estimation algorithm,
    // but the two must treat formatting characters quite differently. Thus, we are staying
    // away from both "first" and "last" in these method names to avoid confusion.
    mCharIndex = 0;
    Int32 embeddingLevel = 0;
    Int32 embeddingLevelDir = DIR_UNKNOWN;
    Int32 firstNonEmptyEmbeddingLevel = 0;
    while (mCharIndex < mLength && firstNonEmptyEmbeddingLevel == 0) {
        switch (DirTypeForward()) {
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING:
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE:
                ++embeddingLevel;
                embeddingLevelDir = DIR_LTR;
                break;
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING:
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE:
                ++embeddingLevel;
                embeddingLevelDir = DIR_RTL;
                break;
            case Character::DIRECTIONALITY_POP_DIRECTIONAL_FORMAT:
                --embeddingLevel;
                // To restore embeddingLevelDir to its previous value, we would need a
                // stack, which we want to avoid. Thus, at this point we do not know the
                // current embedding's directionality.
                embeddingLevelDir = DIR_UNKNOWN;
                break;
            case Character::DIRECTIONALITY_BOUNDARY_NEUTRAL:
                break;
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT:
                if (embeddingLevel == 0) {
                    return DIR_LTR;
                }
                firstNonEmptyEmbeddingLevel = embeddingLevel;
                break;
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT:
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC:
                if (embeddingLevel == 0) {
                    return DIR_RTL;
                }
                firstNonEmptyEmbeddingLevel = embeddingLevel;
                break;
            default:
                firstNonEmptyEmbeddingLevel = embeddingLevel;
                break;
        }
    }

    // We have either found a non-empty embedding or scanned the entire string finding
    // neither a non-empty embedding nor a strong character outside of an embedding.
    if (firstNonEmptyEmbeddingLevel == 0) {
        // We have not found a non-empty embedding. Thus, the string contains neither a
        // non-empty embedding nor a strong character outside of an embedding.
        return DIR_UNKNOWN;
    }

    // We have found a non-empty embedding.
    if (embeddingLevelDir != DIR_UNKNOWN) {
        // We know the directionality of the non-empty embedding.
        return embeddingLevelDir;
    }

    // We do not remember the directionality of the non-empty embedding we found. So, we go
    // backwards to find the start of the non-empty embedding and get its directionality.
    while (mCharIndex > 0) {
        switch (DirTypeBackward()) {
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING:
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE:
                if (firstNonEmptyEmbeddingLevel == embeddingLevel) {
                    return DIR_LTR;
                }
                --embeddingLevel;
                break;
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING:
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE:
                if (firstNonEmptyEmbeddingLevel == embeddingLevel) {
                    return DIR_RTL;
                }
                --embeddingLevel;
                break;
            case Character::DIRECTIONALITY_POP_DIRECTIONAL_FORMAT:
                ++embeddingLevel;
                break;
        }
    }
    // We should never get here.
    return DIR_UNKNOWN;
}

Int32 CBidiFormatter::DirectionalityEstimator::GetExitDir()
{
    // The reason for this method name, as opposed to getLastStrongDir(), is that "last
    // strong" sounds like the exact opposite of "first strong", which is a commonly used
    // description of Unicode's estimation algorithm (getUnicodeDir() above), but the two
    // must treat formatting characters quite differently. Thus, we are staying away from
    // both "first" and "last" in these method names to avoid confusion.
    mCharIndex = mLength;
    Int32 embeddingLevel = 0;
    Int32 lastNonEmptyEmbeddingLevel = 0;
    while (mCharIndex > 0) {
        switch (DirTypeBackward()) {
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT:
                if (embeddingLevel == 0) {
                    return DIR_LTR;
                }
                if (lastNonEmptyEmbeddingLevel == 0) {
                    lastNonEmptyEmbeddingLevel = embeddingLevel;
                }
                break;
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT_EMBEDDING:
            case Character::DIRECTIONALITY_LEFT_TO_RIGHT_OVERRIDE:
                if (lastNonEmptyEmbeddingLevel == embeddingLevel) {
                    return DIR_LTR;
                }
                --embeddingLevel;
                break;
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT:
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_ARABIC:
                if (embeddingLevel == 0) {
                    return DIR_RTL;
                }
                if (lastNonEmptyEmbeddingLevel == 0) {
                    lastNonEmptyEmbeddingLevel = embeddingLevel;
                }
                break;
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_EMBEDDING:
            case Character::DIRECTIONALITY_RIGHT_TO_LEFT_OVERRIDE:
                if (lastNonEmptyEmbeddingLevel == embeddingLevel) {
                    return DIR_RTL;
                }
                --embeddingLevel;
                break;
            case Character::DIRECTIONALITY_POP_DIRECTIONAL_FORMAT:
                ++embeddingLevel;
                break;
            case Character::DIRECTIONALITY_BOUNDARY_NEUTRAL:
                break;
            default:
                if (lastNonEmptyEmbeddingLevel == 0) {
                    lastNonEmptyEmbeddingLevel = embeddingLevel;
                }
                break;
        }
    }
    return DIR_UNKNOWN;
}

Byte CBidiFormatter::DirectionalityEstimator::GetCachedDirectionality(
    /* [in] */ Char32 c)
{
    return c < DIR_TYPE_CACHE_SIZE ? (*DIR_TYPE_CACHE)[c] : Character::GetDirectionality(c);
}

Byte CBidiFormatter::DirectionalityEstimator::DirTypeForward()
{
    mLastChar = mText.GetChar(mCharIndex);
    if (Character::IsHighSurrogate(mLastChar)) {
        assert(0 && "TODO");
        // Int32 codePoint = Character::CodePointAt(mText, mCharIndex);
        // mCharIndex += Character::CharCount(codePoint);
        // return Character::GetDirectionality(codePoint);
    }
    mCharIndex++;
    Byte dirType = GetCachedDirectionality(mLastChar);
    if (mIsHtml) {
        // Process tags and entities.
        if (mLastChar == '<') {
            dirType = SkipTagForward();
        }
        else if (mLastChar == '&') {
            dirType = SkipEntityForward();
        }
    }
    return dirType;
}

Byte CBidiFormatter::DirectionalityEstimator::DirTypeBackward()
{
    mLastChar = mText.GetChar(mCharIndex - 1);
    if (Character::IsLowSurrogate(mLastChar)) {
        assert(0 && "TODO");
        // Int32 codePoint = Character::CodePointBefore(mText, mCharIndex);
        // mCharIndex -= Character::CharCount(codePoint);
        // return Character::GetDirectionality(codePoint);
    }
    mCharIndex--;
    Byte dirType = GetCachedDirectionality(mLastChar);
    if (mIsHtml) {
        // Process tags and entities.
        if (mLastChar == '>') {
            dirType = SkipTagBackward();
        }
        else if (mLastChar == ';') {
            dirType = SkipEntityBackward();
        }
    }
    return dirType;
}

Byte CBidiFormatter::DirectionalityEstimator::SkipTagForward()
{
    Int32 initialCharIndex = mCharIndex;
    while (mCharIndex < mLength) {
        mLastChar = mText.GetChar(mCharIndex++);
        if (mLastChar == '>') {
            // The end of the tag.
            return Character::DIRECTIONALITY_WHITESPACE;
        }
        if (mLastChar == '"' || mLastChar == '\'') {
            // Skip over a quoted attribute value inside the tag.
            Char32 quote = mLastChar;
            while (mCharIndex < mLength && (mLastChar = mText.GetChar(mCharIndex++)) != quote) {}
        }
    }
    // The original '<' wasn't the start of a tag after all.
    mCharIndex = initialCharIndex;
    mLastChar = '<';
    return Character::DIRECTIONALITY_OTHER_NEUTRALS;
}

Byte CBidiFormatter::DirectionalityEstimator::SkipTagBackward()
{
    Int32 initialCharIndex = mCharIndex;
    while (mCharIndex > 0) {
        mLastChar = mText.GetChar(--mCharIndex);
        if (mLastChar == '<') {
            // The start of the tag.
            return Character::DIRECTIONALITY_WHITESPACE;
        }
        if (mLastChar == '>') {
            break;
        }
        if (mLastChar == '"' || mLastChar == '\'') {
            // Skip over a quoted attribute value inside the tag.
            Char32 quote = mLastChar;
            while (mCharIndex > 0 && (mLastChar = mText.GetChar(--mCharIndex)) != quote) {}
        }
    }
    // The original '>' wasn't the end of a tag after all.
    mCharIndex = initialCharIndex;
    mLastChar = '>';
    return Character::DIRECTIONALITY_OTHER_NEUTRALS;
}

Byte CBidiFormatter::DirectionalityEstimator::SkipEntityForward()
{
    while (mCharIndex < mLength && (mLastChar = mText.GetChar(mCharIndex++)) != ';') {}
    return Character::DIRECTIONALITY_WHITESPACE;
}

Byte CBidiFormatter::DirectionalityEstimator::SkipEntityBackward()
{
    Int32 initialCharIndex = mCharIndex;
    while (mCharIndex > 0) {
        mLastChar = mText.GetChar(--mCharIndex);
        if (mLastChar == '&') {
            return Character::DIRECTIONALITY_WHITESPACE;
        }
        if (mLastChar == ';') {
            break;
        }
    }
    mCharIndex = initialCharIndex;
    mLastChar = ';';
    return Character::DIRECTIONALITY_OTHER_NEUTRALS;
}

//===============================================================================
//                  CBidiFormatter
//===============================================================================

CAR_INTERFACE_IMPL(CBidiFormatter, Object, IBidiFormatter)

CAR_OBJECT_IMPL(CBidiFormatter)

CBidiFormatter::CBidiFormatter()
    : mIsRtlContext(FALSE)
    , mFlags(0)
{
}

CBidiFormatter::~CBidiFormatter()
{}

ECode CBidiFormatter::constructor(
    /* [in] */ Boolean isRtlContext,
    /* [in] */ Int32 flags,
    /* [in] */ ITextDirectionHeuristic* heuristic)
{
    mIsRtlContext = isRtlContext;
    mFlags = flags;
    mDefaultTextDirectionHeuristic = heuristic;
    return NOERROR;
}

ECode CBidiFormatter::IsRtlContext(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = mIsRtlContext;
    return NOERROR;
}

ECode CBidiFormatter::GetStereoReset(
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = (mFlags & FLAG_STEREO_RESET) != 0;
    return NOERROR;
}

ECode CBidiFormatter::MarkAfter(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)

    Boolean isRtl;
    heuristic->IsRtl(CoreUtils::Convert(str), 0, str.GetLength(), &isRtl);
    // getExitDir() is called only if needed (short-circuit).
    if (!mIsRtlContext && (isRtl || GetExitDir(str) == DIR_RTL)) {
        *ret = LRM_STRING;
        return NOERROR;
    }
    if (mIsRtlContext && (!isRtl || GetExitDir(str) == DIR_LTR)) {
        *ret = RLM_STRING;
        return NOERROR;
    }

    *ret = EMPTY_STRING;
    return NOERROR;
}

ECode CBidiFormatter::MarkBefore(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)

    Boolean isRtl;
    heuristic->IsRtl(CoreUtils::Convert(str), 0, str.GetLength(), &isRtl);
    // getEntryDir() is called only if needed (short-circuit).
    if (!mIsRtlContext && (isRtl || GetEntryDir(str) == DIR_RTL)) {
        *ret = LRM_STRING;
        return NOERROR;
    }
    if (mIsRtlContext && (!isRtl || GetEntryDir(str) == DIR_LTR)) {
        *ret = RLM_STRING;
        return NOERROR;
    }

    *ret = EMPTY_STRING;
    return NOERROR;
}

ECode CBidiFormatter::IsRtl(
    /* [in] */ const String& str,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)

    return mDefaultTextDirectionHeuristic->IsRtl(CoreUtils::Convert(str),
            0, str.GetLength(), ret);
}

ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [in] */ Boolean isolate,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)

    Boolean isRtl;
    heuristic->IsRtl(CoreUtils::Convert(str), 0, str.GetLength(), &isRtl);

    StringBuilder result;
    Boolean res;
    GetStereoReset(&res);
    if (res && isolate) {
        String beforeStr;
        MarkBefore(str,
                isRtl ? TextDirectionHeuristics::RTL : TextDirectionHeuristics::LTR, &beforeStr);
        result += beforeStr;
    }
    if (isRtl != mIsRtlContext) {
        result += isRtl ? RLE : LRE;
        result += str;
        result += PDF;
    }
    else {
        result += str;
    }
    if (isolate) {
        String afterStr;
        MarkAfter(str,
                isRtl ? TextDirectionHeuristics::RTL : TextDirectionHeuristics::LTR, &afterStr);
        result += afterStr;
    }
    *ret = result.ToString();
    return NOERROR;
}

ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [in] */ ITextDirectionHeuristic* heuristic,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)
    return UnicodeWrap(str, heuristic, TRUE /* isolate */, ret);
}

ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [in] */ Boolean isolate,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)
    return UnicodeWrap(str, mDefaultTextDirectionHeuristic, isolate, ret);
}

ECode CBidiFormatter::UnicodeWrap(
    /* [in] */ const String& str,
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)
    return UnicodeWrap(str, mDefaultTextDirectionHeuristic, TRUE /* isolate */, ret);
}

Boolean CBidiFormatter::IsRtlLocale(
    /* [in] */ ILocale* locale)
{
    return (TextUtils::GetLayoutDirectionFromLocale(locale) == IView::LAYOUT_DIRECTION_RTL);
}

Int32 CBidiFormatter::GetExitDir(
    /* [in] */ const String& str)
{
    AutoPtr<DirectionalityEstimator> de = new DirectionalityEstimator(str, FALSE /* mIsHtml */);
    return de->GetExitDir();
}

Int32 CBidiFormatter::GetEntryDir(
    /* [in] */ const String& str)
{
    AutoPtr<DirectionalityEstimator> de = new DirectionalityEstimator(str, FALSE /* mIsHtml */);
    return de->GetEntryDir();
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos