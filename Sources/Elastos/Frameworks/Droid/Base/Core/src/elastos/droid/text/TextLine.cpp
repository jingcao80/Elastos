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

#include "elastos/droid/text/TextLine.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/core/Character.h>
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <Elastos.Droid.Emoji.h>

using Elastos::Core::AutoLock;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::PaintStyle;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Text::Style::EIID_IMetricAffectingSpan;
using Elastos::Droid::Text::Style::EIID_ICharacterStyle;
using Elastos::Droid::Text::Style::EIID_IReplacementSpan;

using Elastos::Core::Character;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Text {

const Boolean TextLine::DEBUG = FALSE;
AutoPtr< ArrayOf<TextLine*> > TextLine::sCached = ArrayOf<TextLine*>::Alloc(3);
const Int32 TextLine::TAB_INCREMENT;
Object TextLine::sCachedLock;

TextLine::TextLine()
    : mStart(0)
    , mLen(0)
    , mDir(0)
    , mHasTabs(FALSE)
    , mCharsValid(FALSE)
{
    CTextPaint::New((ITextPaint**)&mWorkPaint);
    mMetricAffectingSpanSpanSet = new SpanSet<IMetricAffectingSpan>(EIID_IMetricAffectingSpan);
    mCharacterStyleSpanSet = new SpanSet<ICharacterStyle>(EIID_ICharacterStyle);;
    mReplacementSpanSpanSet = new SpanSet<IReplacementSpan>(EIID_IReplacementSpan);
}

TextLine::~TextLine()
{
}

AutoPtr<TextLine> TextLine::Obtain()
{
    AutoPtr<TextLine> tl;
    {
      AutoLock lock(sCachedLock);
      Int32 length = sCached->GetLength();
      for (Int32 i = length; --i >= 0;) {
          if ((*sCached)[i] != NULL) {
              tl = (*sCached)[i];
              sCached->Set(i, NULL);
              return tl;
          }
      }
    }
    tl = new TextLine();
//    if (DEBUG) {
//        Log.v("TLINE", "new: " + tl);
//    }*
    return tl;
}

AutoPtr<TextLine> TextLine::Recycle(
    /* [in] */ TextLine* tl)
{
    assert(tl != NULL);

    tl->mText = NULL;
    tl->mPaint = NULL;
    tl->mDirections = NULL;

    if (tl->mMetricAffectingSpanSpanSet) {
        tl->mMetricAffectingSpanSpanSet->Recycle();
    }
    if (tl->mCharacterStyleSpanSet) {
        tl->mCharacterStyleSpanSet->Recycle();
    }
    if (tl->mReplacementSpanSpanSet) {
        tl->mReplacementSpanSpanSet->Recycle();
    }

    {    AutoLock syncLock(sCachedLock);
        for (Int32 i = 0; i < sCached->GetLength(); ++i) {
            if ((*sCached)[i] == NULL) {
                sCached->Set(i, tl);
                break;
            }
        }
    }
    return NULL;
}

ECode TextLine::Set(
    /* [in] */ ITextPaint* paint,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ Int32 dir,
    /* [in] */ ILayoutDirections* directions,
    /* [in] */ Boolean hasTabs,
    /* [in] */ Layout::TabStops* tabStops)
{
    mPaint = paint;
    mText = text;
    mStart = start;
    mLen = limit - start;
    mDir = dir;
    mDirections = directions;
    if (mDirections == NULL) {
        Logger::E("TextLine", "Directions cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHasTabs = hasTabs;
    mSpanned = NULL;

    Boolean hasReplacement = FALSE;
    if (ISpanned::Probe(text) != NULL) {
        mSpanned = ISpanned::Probe(text);
        mReplacementSpanSpanSet->Init(mSpanned, start, limit);
        hasReplacement = mReplacementSpanSpanSet->mNumberOfSpans > 0;
    }

    mCharsValid = hasReplacement || hasTabs || directions != Layout::DIRS_ALL_LEFT_TO_RIGHT;

    if (mCharsValid) {
        if (mChars == NULL || mChars->GetLength() < mLen) {
            mChars = ArrayUtils::NewUnpaddedChar32Array(mLen);
        }
        TextUtils::GetChars(text, start, limit, mChars, 0);
        if (hasReplacement) {
            // Handle these all at once so we don't have to do it as we go.
            // Replace the first character of each replacement run with the
            // object-replacement character and the remainder with zero width
            // non-break space aka BOM.  Cursor movement code skips these
            // zero-width characters.
            AutoPtr< ArrayOf<Char32> > chars = mChars;
            for (Int32 i = start, inext = 0; i < limit; i = inext) {
                inext = mReplacementSpanSpanSet->GetNextTransition(i, limit);
                if (mReplacementSpanSpanSet->HasSpansIntersecting(i, inext)) {
                    // transition into a span
                    (*chars)[i - start] = 0xfffc/*'\ufffc'*/;
                    for (Int32 j = i - start + 1, e = inext - start; j < e; ++j) {
                        (*chars)[j] = 0xfeff/*'\ufeff'*/; // used as ZWNBS, marks positions to skip
                    }
                }
            }
        }
    }
    mTabs = tabStops;
    return NOERROR;
}

void TextLine::Draw(
    /* [in] */ ICanvas* c,
    /* [in] */ Float x,
    /* [in] */ Int32 top,
    /* [in] */ Int32 y,
    /* [in] */ Int32 bottom)
{
    if (!mHasTabs) {
        if (mDirections == Layout::DIRS_ALL_LEFT_TO_RIGHT) {
            DrawRun(c, 0, mLen, FALSE, x, top, y, bottom, FALSE);
            return;
        }
        if (mDirections == Layout::DIRS_ALL_RIGHT_TO_LEFT) {
            DrawRun(c, 0, mLen, TRUE, x, top, y, bottom, FALSE);
            return;
        }
    }

    Float h = 0;
    AutoPtr< ArrayOf<Int32> > runs;
    mDirections->GetDirections((ArrayOf<Int32>**)&runs);
    AutoPtr<IRectF> emojiRect;

    Int32 len = runs->GetLength();
    Int32 lastRunIndex = len - 2;
    for (Int32 i = 0; i < len; i += 2) {
        Int32 runStart = (*runs)[i];
        Int32 runLimit = runStart + ((*runs)[i+1] & ILayout::RUN_LENGTH_MASK);
        if (runLimit > mLen) {
            runLimit = mLen;
        }
        Boolean runIsRtl = ((*runs)[i+1] & ILayout::RUN_RTL_FLAG) != 0;

        Int32 segstart = runStart;
        for (Int32 j = mHasTabs ? runStart : runLimit; j <= runLimit; j++) {
            Int32 codept = 0;
            AutoPtr<IBitmap> bm;

            if (mHasTabs && j < runLimit) {
                codept = (*(mChars.Get()))[j];
                if (codept >= 0xd800 && codept < 0xdc00 && j + 1 < runLimit) {
                    assert(0 && "TODO");
//                    codept = Character::CodePointAt(mChars, j);
                    if (codept >= Layout::MIN_EMOJI && codept <= Layout::MAX_EMOJI) {
                        Layout::EMOJI_FACTORY->GetBitmapFromAndroidPua(codept, (IBitmap**)&bm);
                    }
                    else if (codept > 0xffff) {
                        ++j;
                        continue;
                    }
                }
            }

            if (j == runLimit || codept == '\t' || bm != NULL) {
                h += DrawRun(c, segstart, j, runIsRtl, x+h, top, y, bottom,
                        i != lastRunIndex || j != mLen);

                if (codept == '\t') {
                    h = mDir * NextTab(h * mDir);
                }
                else if (bm != NULL) {
                    Float bmAscent = Ascent(j);
                    Int32 bitmapHeight;
                    bm->GetHeight(&bitmapHeight);
                    Float scale = -bmAscent / bitmapHeight;
                    Int32 temp;
                    bm->GetWidth(&temp);
                    Float width = temp * scale;

                    if (emojiRect == NULL) {
                        CRectF::New((IRectF**)&emojiRect);
                    }
                    emojiRect->Set(x + h, y + bmAscent, x + h + width, y);
                    c->DrawBitmap(bm, NULL, emojiRect, IPaint::Probe(mPaint));
                    h += width;
                    j++;
                }
                segstart = j + 1;
            }
        }
    }
}

Float TextLine::Metrics(
    /* [in] */ IPaintFontMetricsInt* fmi)
{
    return Measure(mLen, FALSE, fmi);
}

Float TextLine::Measure(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean trailing,
    /* [in] */ IPaintFontMetricsInt* fmi)
{
    Int32 target = trailing ? offset - 1 : offset;
    if (target < 0) {
        return 0;
    }

    Float h = 0;

    if (!mHasTabs) {
        if (mDirections == Layout::DIRS_ALL_LEFT_TO_RIGHT) {
            return MeasureRun(0, offset, mLen, FALSE, fmi);
        }
        if (mDirections == Layout::DIRS_ALL_RIGHT_TO_LEFT) {
            return MeasureRun(0, offset, mLen, TRUE, fmi);
        }
    }

    AutoPtr< ArrayOf<Char32> > chars = mChars;
    AutoPtr< ArrayOf<Int32> > runs;
    mDirections->GetDirections((ArrayOf<Int32>**)&runs);
    Int32 len = runs->GetLength();
    for (Int32 i = 0; i < len; i += 2) {
        Int32 runStart = (*runs)[i];
        Int32 runLimit = runStart + ((*runs)[i+1] & ILayout::RUN_LENGTH_MASK);
        if (runLimit > mLen) {
            runLimit = mLen;
        }
        Boolean runIsRtl = ((*runs)[i+1] & ILayout::RUN_RTL_FLAG) != 0;

        Int32 segstart = runStart;
        for (Int32 j = mHasTabs ? runStart : runLimit; j <= runLimit; j++) {
            Int32 codept = 0;
            AutoPtr<IBitmap> bm;

            if (mHasTabs && j < runLimit) {
                codept = (*chars)[j];
                if (codept >= 0xd800 && codept < 0xdc00 && j + 1 < runLimit) {
                    if (codept >= Layout::MIN_EMOJI && codept <= Layout::MAX_EMOJI) {
                        Layout::EMOJI_FACTORY->GetBitmapFromAndroidPua(codept, (IBitmap**)&bm);
                    }
                    else if (codept > 0xffff) {
                        ++j;
                        continue;
                    }
                }
            }

            if (j == runLimit || codept == '\t' || bm != NULL) {
                Boolean inSegment = target >= segstart && target < j;

                Boolean advance = (mDir == ILayout::DIR_RIGHT_TO_LEFT) == runIsRtl;
                if (inSegment && advance) {
                    return h += MeasureRun(segstart, offset, j, runIsRtl, fmi);
                }

                Float w = MeasureRun(segstart, j, j, runIsRtl, fmi);
                h += advance ? w : -w;

                if (inSegment) {
                    return h += MeasureRun(segstart, offset, j, runIsRtl, NULL);
                }

                if (codept == '\t') {
                    if (offset == j) {
                        return h;
                    }
                    h = mDir * NextTab(h * mDir);
                    if (target == j) {
                        return h;
                    }
                }

                if (bm != NULL) {
                    Float bmAscent = Ascent(j);
                    Int32 width, height;
                    bm->GetWidth(&width);
                    bm->GetHeight(&height);
                    Float wid = width * -bmAscent / height;
                    h += mDir * wid;
                    j++;
                }

                segstart = j + 1;
            }
        }
    }

    return h;
}

Int32 TextLine::GetOffsetToLeftRightOf(
    /* [in] */ Int32 cursor,
    /* [in] */ Boolean toLeft)
{
    // 1) The caret marks the leading edge of a character. The character
    // logically before it might be on a different level, and the active caret
    // position is on the character at the lower level. If that character
    // was the previous character, the caret is on its trailing edge.
    // 2) Take this character/edge and move it in the indicated direction.
    // This gives you a new character and a new edge.
    // 3) This position is between two visually adjacent characters.  One of
    // these might be at a lower level.  The active position is on the
    // character at the lower level.
    // 4) If the active position is on the trailing edge of the character,
    // the new caret position is the following logical character, else it
    // is the character.

    Int32 lineStart = 0;
    Int32 lineEnd = mLen;
    Boolean paraIsRtl = mDir == -1;
    AutoPtr< ArrayOf<Int32> >runs;
    mDirections->GetDirections((ArrayOf<Int32>**)&runs);

    Int32 runIndex, runLevel = 0, runStart = lineStart, runLimit = lineEnd, newCaret = -1;
    Boolean trailing = FALSE;

    Int32 runsLen = runs.Get()->GetLength();
    if (cursor == lineStart) {
        runIndex = -2;
    } else if (cursor == lineEnd) {
        runIndex = runsLen;
    } else {
      // First, get information about the run containing the character with
      // the active caret.
      for (runIndex = 0; runIndex < runsLen; runIndex += 2) {
        runStart = lineStart + (*runs)[runIndex];
        if (cursor >= runStart) {
          runLimit = runStart + ((*runs)[runIndex+1] & ILayout::RUN_LENGTH_MASK);
          if (runLimit > lineEnd) {
              runLimit = lineEnd;
          }
          if (cursor < runLimit) {
            UInt32 ui = (UInt32)(*runs)[runIndex+1];
            runLevel = (ui >> ILayout::RUN_LEVEL_SHIFT) &
                ILayout::RUN_LEVEL_MASK;
            if (cursor == runStart) {
              // The caret is on a run boundary, see if we should
              // use the position on the trailing edge of the previous
              // logical character instead.
              Int32 prevRunIndex, prevRunLevel, prevRunStart, prevRunLimit;
              Int32 pos = cursor - 1;
              for (prevRunIndex = 0; prevRunIndex < runs.Get()->GetLength(); prevRunIndex += 2) {
                prevRunStart = lineStart + (*runs)[prevRunIndex];
                if (pos >= prevRunStart) {
                  prevRunLimit = prevRunStart +
                      ((*runs)[prevRunIndex+1] & ILayout::RUN_LENGTH_MASK);
                  if (prevRunLimit > lineEnd) {
                      prevRunLimit = lineEnd;
                  }
                  if (pos < prevRunLimit) {
                    UInt32 ui = (UInt32)(*runs)[prevRunIndex+1];
                    prevRunLevel = (ui >> ILayout::RUN_LEVEL_SHIFT)
                        & ILayout::RUN_LEVEL_MASK;
                    if (prevRunLevel < runLevel) {
                      // Start from logically previous character.
                      runIndex = prevRunIndex;
                      runLevel = prevRunLevel;
                      runStart = prevRunStart;
                      runLimit = prevRunLimit;
                      trailing = true;
                      break;
                    }
                  }
                }
              }
            }
            break;
          }
        }
      }

      // caret might be == lineEnd.  This is generally a space or paragraph
      // separator and has an associated run, but might be the end of
      // text, in which case it doesn't.  If that happens, we ran off the
      // end of the run list, and runIndex == runs.length.  In this case,
      // we are at a run boundary so we skip the below test.
      if (runIndex != runsLen) {
          Boolean runIsRtl = (runLevel & 0x1) != 0;
          Boolean advance = toLeft == runIsRtl;
          if (cursor != (advance ? runLimit : runStart) || advance != trailing) {
              // Moving within or into the run, so we can move logically.
              newCaret = GetOffsetBeforeAfter(runIndex, runStart, runLimit,
                      runIsRtl, cursor, advance);
              // If the new position is internal to the run, we're at the strong
              // position already so we're finished.
              if (newCaret != (advance ? runLimit : runStart)) {
                  return newCaret;
              }
          }
      }
    }

    // If newCaret is -1, we're starting at a run boundary and crossing
    // into another run. Otherwise we've arrived at a run boundary, and
    // need to figure out which character to attach to.  Note we might
    // need to run this twice, if we cross a run boundary and end up at
    // another run boundary.
    while (TRUE) {
      Boolean advance = toLeft == paraIsRtl;
      Int32 otherRunIndex = runIndex + (advance ? 2 : -2);
      if (otherRunIndex >= 0 && otherRunIndex < runs.Get()->GetLength()) {
        Int32 otherRunStart = lineStart + (*runs)[otherRunIndex];
        Int32 otherRunLimit = otherRunStart +
        ((*runs)[otherRunIndex+1] & ILayout::RUN_LENGTH_MASK);
        if (otherRunLimit > lineEnd) {
            otherRunLimit = lineEnd;
        }
        UInt32 ui = (UInt32)(*runs)[otherRunIndex+1];
        Int32 otherRunLevel = (ui >> ILayout::RUN_LEVEL_SHIFT) &
            ILayout::RUN_LEVEL_MASK;
        Boolean otherRunIsRtl = (otherRunLevel & 1) != 0;

        advance = toLeft == otherRunIsRtl;
        if (newCaret == -1) {
            newCaret = GetOffsetBeforeAfter(otherRunIndex, otherRunStart,
                    otherRunLimit, otherRunIsRtl,
                    advance ? otherRunStart : otherRunLimit, advance);
            if (newCaret == (advance ? otherRunLimit : otherRunStart)) {
                // Crossed and ended up at a new boundary,
                // repeat a second and final time.
                runIndex = otherRunIndex;
                runLevel = otherRunLevel;
                continue;
            }
            break;
        }

        // The new caret is at a boundary.
        if (otherRunLevel < runLevel) {
          // The strong character is in the other run.
          newCaret = advance ? otherRunStart : otherRunLimit;
        }
        break;
      }

      if (newCaret == -1) {
          // We're walking off the end of the line.  The paragraph
          // level is always equal to or lower than any internal level, so
          // the boundaries get the strong caret.
          newCaret = advance ? mLen + 1 : -1;
          break;
      }

      // Else we've arrived at the end of the line.  That's a strong position.
      // We might have arrived here by crossing over a run with no internal
      // breaks and dropping out of the above loop before advancing one final
      // time, so reset the caret.
      // Note, we use '<=' below to handle a situation where the only run
      // on the line is a counter-directional run.  If we're not advancing,
      // we can end up at the 'lineEnd' position but the caret we want is at
      // the lineStart.
      if (newCaret <= lineEnd) {
          newCaret = advance ? lineEnd : lineStart;
      }
      break;
    }

    return newCaret;
}

void TextLine::UpdateMetrics(
    /* [in] */ IPaintFontMetricsInt* fmi,
    /* [in] */ Int32 previousTop,
    /* [in] */ Int32 previousAscent,
    /* [in] */ Int32 previousDescent,
    /* [in] */ Int32 previousBottom,
    /* [in] */ Int32 previousLeading)
{
    assert(fmi != NULL);
    using Elastos::Core::Math;

    Int32 top, ascent, descent, bottom, leading;

    fmi->GetTop(&top);
    fmi->GetAscent(&ascent);
    fmi->GetDescent(&descent);
    fmi->GetBottom(&bottom);
    fmi->GetLeading(&leading);

    fmi->SetTop(Min(top, previousTop));
    fmi->SetAscent(Min(ascent, previousAscent));
    fmi->SetDescent(Max(descent, previousDescent));
    fmi->SetBottom(Max(bottom, previousBottom));
    fmi->SetLeading(Max(leading, previousLeading));
}

Float TextLine::Ascent(
    /* [in] */ Int32 pos)
{
    assert(0 && "TODO");
/*
    if (mSpanned == NULL) {
        Float temp;
        mPaint->Ascent(&temp);
        return temp;
    }

    pos += mStart;
    AutoPtr< ArrayOf<IInterface> > spans;//IMetricAffectingSpan
    mSpanned->GetSpans(pos, pos + 1, ERIID_IMetricAffectingSpan,
             (ArrayOf<IInterface>**)&spans);
    if (spans->GetLength() == 0) {
        Float temp;
        mPaint->Ascent(&temp);
        return temp;
    }

    AutoPtr<ITextPaint> wp = mWorkPaint;
    wp->set(mPaint);
    //for (MetricAffectingSpan span : spans) {
    Int32 length = spans->GetLength();
    for (Int32 i = 0; i < length; i++) {
        IMetricAffectingSpan::Probe((*spans)[i])->UpdateMeasureState(wp);
    }

    Float temp;
    wp->Ascent(&temp);
    return temp;*/

    return 0.0f; //temp
}

Float TextLine::NextTab(
    /* [in] */ Float h)
{
    if (mTabs != NULL) {
        return mTabs->NextTab(h);
    }
    return Layout::TabStops::NextDefaultStop(h, TAB_INCREMENT);
}

Float TextLine::DrawRun(
    /* [in] */ ICanvas* c,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ Boolean runIsRtl,
    /* [in] */ Float x,
    /* [in] */ Int32 top,
    /* [in] */ Int32 y,
    /* [in] */ Int32 bottom,
    /* [in] */ Boolean needWidth)
{
    if ((mDir == ILayout::DIR_LEFT_TO_RIGHT) == runIsRtl) {
        Float w = -MeasureRun(start, limit, limit, runIsRtl, NULL);
        HandleRun(start, limit, limit, runIsRtl, c, x + w, top,
                y, bottom, NULL, FALSE);
        return w;
    }

    return HandleRun(start, limit, limit, runIsRtl, c, x, top,
            y, bottom, NULL, needWidth);
}

Float TextLine::MeasureRun(
    /* [in] */ Int32 start,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 limit,
    /* [in] */ Boolean runIsRtl,
    /* [in] */ IPaintFontMetricsInt* fmi)
{
    return HandleRun(start, offset, limit, runIsRtl, NULL, 0, 0, 0, 0, fmi, TRUE);
}

Int32 TextLine::GetOffsetBeforeAfter(
    /* [in] */ Int32 runIndex,
    /* [in] */ Int32 runStart,
    /* [in] */ Int32 runLimit,
    /* [in] */ Boolean runIsRtl,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean after)
{
    if (runIndex < 0 || offset == (after ? mLen : 0)) {
        // Walking off end of line.  Since we don't know
        // what cursor positions are available on other lines, we can't
        // return accurate values.  These are a guess.
        if (after) {
            return TextUtils::GetOffsetAfter(mText, offset + mStart) - mStart;
        }
        return TextUtils::GetOffsetBefore(mText, offset + mStart) - mStart;
    }

    AutoPtr<ITextPaint> wp = mWorkPaint;
    IPaint* p = IPaint::Probe(wp);
    wp->Set(mPaint);

    Int32 spanStart = runStart;
    Int32 spanLimit;
    if (mSpanned == NULL) {
        spanLimit = runLimit;
    } else {
        Int32 target = after ? offset + 1 : offset;
        Int32 limit = mStart + runLimit;
        Int32 temp;
        while (TRUE) {
            mSpanned->NextSpanTransition(mStart + spanStart, limit,
                    EIID_IMetricAffectingSpan, &temp);
            spanLimit = temp - mStart;
            if (spanLimit >= target) {
                break;
            }
            spanStart = spanLimit;
        }

        AutoPtr< ArrayOf<IInterface*> > spans;
        mSpanned->GetSpans(mStart + spanStart,
                mStart + spanLimit, EIID_IMetricAffectingSpan, (ArrayOf<IInterface*>**)&spans);
        spans = TextUtils::RemoveEmptySpans(spans, mSpanned, EIID_IMetricAffectingSpan);

        Int32 spansLen = spans->GetLength();
        if (spansLen > 0) {
            AutoPtr<IReplacementSpan> replacement;
            for (Int32 j = 0; j < spansLen; j++) {
                AutoPtr<IMetricAffectingSpan> span = (IMetricAffectingSpan*)((*spans)[j]->Probe(EIID_IMetricAffectingSpan));
                if (span->Probe(EIID_IReplacementSpan)) {
                    replacement = IReplacementSpan::Probe(span);
                } else {
                    span->UpdateMeasureState(wp);
                }
            }

            if (replacement != NULL) {
                // If we have a replacement span, we're moving either to
                // the start or end of this span.
                return after ? spanLimit : spanStart;
            }
        }
    }

    Int32 dir = runIsRtl ? IPaint::DIRECTION_RTL : IPaint::DIRECTION_LTR;
    Int32 cursorOpt = after ? IPaint::CURSOR_AFTER : IPaint::CURSOR_BEFORE;
    Int32 temp;
    if (mCharsValid) {
        p->GetTextRunCursor(mChars, spanStart, spanLimit - spanStart,
                dir, offset, cursorOpt, &temp);
        return temp;
    } else {
        p->GetTextRunCursor(mText, mStart + spanStart,
                mStart + spanLimit, dir, mStart + offset, cursorOpt, &temp);
        return temp - mStart;
    }
}

/**
 * @param wp
 */
void TextLine::ExpandMetricsFromPaint(
    /* [in] */ IPaintFontMetricsInt* fmi,
    /* [in] */ ITextPaint* wp)
{
    assert(fmi != NULL);
    assert(wp != NULL);

    Int32 previousTop;
    Int32 previousAscent;
    Int32 previousDescent;
    Int32 previousBottom;
    Int32 previousLeading;

    fmi->GetTop(&previousTop);
    fmi->GetAscent(&previousAscent);
    fmi->GetDescent(&previousDescent);
    fmi->GetBottom(&previousBottom);
    fmi->GetLeading(&previousLeading);

    Int32 spacing = 0;
    IPaint::Probe(wp)->GetFontMetricsInt(fmi, &spacing);

    UpdateMetrics(fmi, previousTop, previousAscent, previousDescent, previousBottom,
            previousLeading);
}

Float TextLine::HandleText(
    /* [in] */ ITextPaint* wp,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Boolean runIsRtl,
    /* [in] */ ICanvas* c,
    /* [in] */ Float x,
    /* [in] */ Int32 top,
    /* [in] */ Int32 y,
    /* [in] */ Int32 bottom,
    /* [in] */ IPaintFontMetricsInt* fmi,
    /* [in] */ Boolean needWidth)
{
    // Get metrics first (even for empty strings or "0" width runs)
    if (fmi != NULL) {
        ExpandMetricsFromPaint(fmi, wp);
    }

    Int32 runLen = end - start;
    // No need to do anything if the run width is "0"
    if (runLen == 0) {
        return 0;
    }

    Float ret = 0;

    IPaint* p = IPaint::Probe(wp);
    Int32 contextLen = contextEnd - contextStart;
    Int32 bgColor, underlineColor;
    if (needWidth || (c != NULL && ((wp->GetBgColor(&bgColor), bgColor != 0) ||
            (wp->GetUnderlineColor(&underlineColor), underlineColor != 0) || runIsRtl))) {
        if (mCharsValid) {
            p->GetTextRunAdvances(mChars, start, runLen,
                    contextStart, contextLen, runIsRtl, NULL, 0, &ret);
        } else {
            Int32 delta = mStart;
            p->GetTextRunAdvances(mText, delta + start,
                    delta + end, delta + contextStart, delta + contextEnd,
                    runIsRtl, NULL, 0, &ret);
        }
    }

    if (c != NULL) {
        if (runIsRtl) {
            x -= ret;
        }

        if (wp->GetBgColor(&bgColor), bgColor != 0) {
            Int32 previousColor;
            p->GetColor(&previousColor);
            PaintStyle previousStyle;
            p->GetStyle(&previousStyle);

            p->SetColor(bgColor);
            p->SetStyle(PaintStyle_FILL);
            c->DrawRect(x, top, x + ret, bottom, p);

            p->SetStyle(previousStyle);
            p->SetColor(previousColor);
        }

        Int32 underlineColor;
        if (wp->GetUnderlineColor(&underlineColor), underlineColor != 0) {
            // kStdUnderline_Offset = 1/9, defined in SkTextFormatParams.h
            Int32 baselineShift;
            wp->GetBaselineShift(&baselineShift);
            Float textSize;
            p->GetTextSize(&textSize);
            Float underlineTop = y + baselineShift + (1.0f / 9.0f) * textSize;

            Int32 previousColor;
            p->GetColor(&previousColor);
            PaintStyle previousStyle;
            p->GetStyle(&previousStyle);
            Boolean previousAntiAlias;
            p->IsAntiAlias(&previousAntiAlias);

            p->SetStyle(PaintStyle_FILL);
            p->SetAntiAlias(TRUE);

            Int32 underlineColor;
            wp->GetUnderlineColor(&underlineColor);
            p->SetColor(underlineColor);
            Float underlineThickness;
            wp->GetUnderlineThickness(&underlineThickness);
            c->DrawRect(x, underlineTop, x + ret, underlineTop + underlineThickness, p);

            p->SetStyle(previousStyle);
            p->SetColor(previousColor);
            p->SetAntiAlias(previousAntiAlias);
        }

        Int32 baselineShift;
        wp->GetBaselineShift(&baselineShift);
        DrawTextRun(c, wp, start, end, contextStart, contextEnd, runIsRtl,
                x, y + baselineShift);
    }

    return runIsRtl ? -ret : ret;
}

Float TextLine::HandleReplacement(
    /* [in] */ IReplacementSpan* replacement,
    /* [in] */ ITextPaint* wp,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ Boolean runIsRtl,
    /* [in] */ ICanvas* c,
    /* [in] */ Float x,
    /* [in] */ Int32 top,
    /* [in] */ Int32 y,
    /* [in] */ Int32 bottom,
    /* [in] */ IPaintFontMetricsInt* fmi,
    /* [in] */ Boolean needWidth)
{
    assert(replacement != NULL);

    IPaint* p = IPaint::Probe(wp);
    Float ret = 0;

    Int32 textStart = mStart + start;
    Int32 textLimit = mStart + limit;

    if (needWidth || (c != NULL && runIsRtl)) {
        Int32 previousTop = 0;
        Int32 previousAscent = 0;
        Int32 previousDescent = 0;
        Int32 previousBottom = 0;
        Int32 previousLeading = 0;

        Boolean needUpdateMetrics = (fmi != NULL);

        if (needUpdateMetrics) {
            fmi->GetTop(&previousTop);
            fmi->GetAscent(&previousAscent);
            fmi->GetDescent(&previousDescent);
            fmi->GetBottom(&previousBottom);
            fmi->GetLeading(&previousLeading);
        }

        Int32 v;
        replacement->GetSize(p, mText, textStart, textLimit, fmi, &v);
        ret = (Float)v;

        if (needUpdateMetrics) {
            UpdateMetrics(fmi, previousTop, previousAscent, previousDescent, previousBottom,
                    previousLeading);
        }
    }

    if (c != NULL) {
        if (runIsRtl) {
            x -= ret;
        }
        replacement->Draw(c, mText, textStart, textLimit,
                x, top, y, bottom, p);
    }

    return runIsRtl ? -ret : ret;
}

Float TextLine::HandleRun(
    /* [in] */ Int32 start,
    /* [in] */ Int32 measureLimit,
    /* [in] */ Int32 limit,
    /* [in] */ Boolean runIsRtl,
    /* [in] */ ICanvas* c,
    /* [in] */ Float x,
    /* [in] */ Int32 top,
    /* [in] */ Int32 y,
    /* [in] */ Int32 bottom,
    /* [in] */ IPaintFontMetricsInt* fmi,
    /* [in] */ Boolean needWidth)
{
    // Case of an empty line, make sure we update fmi according to mPaint
    if (start == measureLimit) {
        AutoPtr<ITextPaint> wp = mWorkPaint;
        wp->Set(mPaint);
        if (fmi != NULL) {
            ExpandMetricsFromPaint(fmi, wp);
        }
        return 0;
    }

    if (mSpanned == NULL) {
        AutoPtr<ITextPaint> wp = mWorkPaint;
        wp->Set(mPaint);
        Int32 mlimit = measureLimit;
        return HandleText(wp, start, mlimit, start, limit, runIsRtl, c, x, top,
                y, bottom, fmi, needWidth || mlimit < measureLimit);
    }

    mMetricAffectingSpanSpanSet->Init(mSpanned, mStart + start, mStart + limit);
    mCharacterStyleSpanSet->Init(mSpanned, mStart + start, mStart + limit);

    // Shaping needs to take into account context up to metric boundaries,
    // but rendering needs to take into account character style boundaries.
    // So we iterate through metric runs to get metric bounds,
    // then within each metric run iterate through character style runs
    // for the run bounds.
    Float originalX = x;
    for (Int32 i = start, inext; i < measureLimit; i = inext) {
        AutoPtr<ITextPaint> wp = mWorkPaint;
        wp->Set(mPaint);

        inext = mMetricAffectingSpanSpanSet->GetNextTransition(mStart + i, mStart + limit) -
                mStart;
        Int32 mlimit = Elastos::Core::Math::Min(inext, measureLimit);

        AutoPtr<IReplacementSpan> replacement;

        for (Int32 j = 0; j < mMetricAffectingSpanSpanSet->mNumberOfSpans; j++) {
            // Both intervals [spanStarts..spanEnds] and [mStart + i..mStart + mlimit] are NOT
            // empty by construction. This special case in getSpans() explains the >= & <= tests
            if (((*mMetricAffectingSpanSpanSet->mSpanStarts)[j] >= mStart + mlimit) ||
                    ((*mMetricAffectingSpanSpanSet->mSpanEnds)[j] <= mStart + i)) continue;
            AutoPtr<IMetricAffectingSpan> span = (*mMetricAffectingSpanSpanSet->mSpans)[j];
            if (IReplacementSpan::Probe(span) != NULL) {
                replacement = IReplacementSpan::Probe(span);
            } else {
                // We might have a replacement that uses the draw
                // state, otherwise measure state would suffice.
                ICharacterStyle::Probe(span)->UpdateDrawState(wp);
            }
        }

        if (replacement != NULL) {
            x += HandleReplacement(replacement, wp, i, mlimit, runIsRtl, c, x, top, y,
                    bottom, fmi, needWidth || mlimit < measureLimit);
            continue;
        }

        for (Int32 j = i, jnext; j < mlimit; j = jnext) {
            jnext = mCharacterStyleSpanSet->GetNextTransition(mStart + j, mStart + mlimit) -
                    mStart;

            wp->Set(mPaint);
            for (Int32 k = 0; k < mCharacterStyleSpanSet->mNumberOfSpans; k++) {
                // Intentionally using >= and <= as explained above
                if (((*mCharacterStyleSpanSet->mSpanStarts)[k] >= mStart + jnext) ||
                        ((*mCharacterStyleSpanSet->mSpanEnds)[k] <= mStart + j)) continue;

                (*mCharacterStyleSpanSet->mSpans)[k]->UpdateDrawState(wp);
            }

            x += HandleText(wp, j, jnext, i, inext, runIsRtl, c, x,
                    top, y, bottom, fmi, needWidth || jnext < measureLimit);
        }
    }

    return x - originalX;
}

void TextLine::DrawTextRun(
    /* [in] */ ICanvas* c,
    /* [in] */ ITextPaint* wp,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 contextStart,
    /* [in] */ Int32 contextEnd,
    /* [in] */ Boolean runIsRtl,
    /* [in] */ Float x,
    /* [in] */ Int32 y)
{
    assert(c != NULL);
    if (mCharsValid) {
        Int32 count = end - start;
        Int32 contextCount = contextEnd - contextStart;
        c->DrawTextRun(mChars, start, count, contextStart, contextCount,
                x, y, runIsRtl, IPaint::Probe(wp));
    } else {
        Int32 delta = mStart;
        c->DrawTextRun(mText, delta + start, delta + end,
                delta + contextStart, delta + contextEnd, x, y, runIsRtl, IPaint::Probe(wp));
    }
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
