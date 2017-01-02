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

#ifndef __ELASTOS_DROID_TEXT_TEXTLINE_H__
#define __ELASTOS_DROID_TEXT_TEXTLINE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/Layout.h"
#include "elastos/droid/text/SpanSet.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Text::Style::IMetricAffectingSpan;
using Elastos::Droid::Text::Style::ICharacterStyle;
using Elastos::Droid::Text::Style::IReplacementSpan;

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Represents a line of styled text, for measuring in visual order and
 * for rendering.
 *
 * <p>Get a new instance using obtain(), and when finished with it, return it
 * to the pool using recycle().
 *
 * <p>Call set to prepare the instance for use, then either draw, measure,
 * metrics, or caretToLeftRightOf.
 *
 * @hide
 */
class TextLine
    : public Object
{
public:
    TextLine();

    virtual ~TextLine();

    /**
     * Returns a new TextLine from the shared pool.
     *
     * @return an uninitialized TextLine
     */
    static CARAPI_(AutoPtr<TextLine>) Obtain();

    /**
     * Puts a TextLine back into the shared pool. Do not use this TextLine once
     * it has been returned.
     * @param tl the textLine
     * @return null, as a convenience from clearing references to the provided
     * TextLine
     */
    static CARAPI_(AutoPtr<TextLine>) Recycle(
        /* [in] */ TextLine* tl);

    /**
     * Initializes a TextLine and prepares it for use.
     *
     * @param paint the base paint for the line
     * @param text the text, can be Styled
     * @param start the start of the line relative to the text
     * @param limit the limit of the line relative to the text
     * @param dir the paragraph direction of this line
     * @param directions the directions information of this line
     * @param hasTabs true if the line might contain tabs or emoji
     * @param tabStops the tabStops. Can be null.
     */
    virtual CARAPI Set(
        /* [in] */ ITextPaint* paint,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ Int32 dir,
        /* [in] */ ILayoutDirections* directions,
        /* [in] */ Boolean hasTabs,
        /* [in] */ Layout::TabStops* tabStops);

    /**
     * Renders the TextLine.
     *
     * @param c the canvas to render on
     * @param x the leading margin position
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     */
    virtual CARAPI_(void) Draw(
        /* [in] */ ICanvas* c,
        /* [in] */ Float x,
        /* [in] */ Int32 top,
        /* [in] */ Int32 y,
        /* [in] */ Int32 bottom);

    /**
     * Returns metrics information for the entire line.
     *
     * @param fmi receives font metrics information, can be null
     * @return the signed width of the line
     */
    virtual CARAPI_(Float) Metrics(
        /* [in] */ IPaintFontMetricsInt* fmi);

    /**
     * Returns information about a position on the line.
     *
     * @param offset the line-relative character offset, between 0 and the
     * line length, inclusive
     * @param trailing true to measure the trailing edge of the character
     * before offset, false to measure the leading edge of the character
     * at offset.
     * @param fmi receives metrics information about the requested
     * character, can be null.
     * @return the signed offset from the leading margin to the requested
     * character edge.
     */
    virtual CARAPI_(Float) Measure(
        /* [in] */ Int32 offset,
        /* [in] */ Boolean trailing,
        /* [in] */ IPaintFontMetricsInt* fmi);

    /**
     * Walk the cursor through this line, skipping conjuncts and
     * zero-width characters.
     *
     * <p>This function cannot properly walk the cursor off the ends of the line
     * since it does not know about any shaping on the previous/following line
     * that might affect the cursor position. Callers must either avoid these
     * situations or handle the result specially.
     *
     * @param cursor the starting position of the cursor, between 0 and the
     * length of the line, inclusive
     * @param toLeft true if the caret is moving to the left.
     * @return the new offset.  If it is less than 0 or greater than the length
     * of the line, the previous/following line should be examined to get the
     * actual offset.
     */
    virtual CARAPI_(Int32) GetOffsetToLeftRightOf(
        /* [in] */ Int32 cursor,
        /* [in] */ Boolean toLeft);

    static CARAPI_(void) UpdateMetrics(
        /* [in] */ IPaintFontMetricsInt* fmi,
        /* [in] */ Int32 previousTop,
        /* [in] */ Int32 previousAscent,
        /* [in] */ Int32 previousDescent,
        /* [in] */ Int32 previousBottom,
        /* [in] */ Int32 previousLeading);

    /**
     * Returns the ascent of the text at start.  This is used for scaling
     * emoji.
     *
     * @param pos the line-relative position
     * @return the ascent of the text at start
     */
    virtual CARAPI_(Float) Ascent(
        /* [in] */ Int32 pos);

    /**
     * Returns the next tab position.
     *
     * @param h the (unsigned) offset from the leading margin
     * @return the (unsigned) tab position after this offset
     */
    virtual CARAPI_(Float) NextTab(
        /* [in] */ Float h);

private:
    /**
     * Draws a unidirectional (but possibly multi-styled) run of text.
     *
     *
     * @param c the canvas to draw on
     * @param start the line-relative start
     * @param limit the line-relative limit
     * @param runIsRtl true if the run is right-to-left
     * @param x the position of the run that is closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param needWidth true if the width value is required.
     * @return the signed width of the run, based on the paragraph direction.
     * Only valid if needWidth is true.
     */
    CARAPI_(Float) DrawRun(
        /* [in] */ ICanvas* c,
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit,
        /* [in] */ Boolean runIsRtl,
        /* [in] */ Float x,
        /* [in] */ Int32 top,
        /* [in] */ Int32 y,
        /* [in] */ Int32 bottom,
        /* [in] */ Boolean needWidth);

    /**
     * Measures a unidirectional (but possibly multi-styled) run of text.
     *
     *
     * @param start the line-relative start of the run
     * @param offset the offset to measure to, between start and limit inclusive
     * @param limit the line-relative limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param fmi receives metrics information about the requested
     * run, can be null.
     * @return the signed width from the start of the run to the leading edge
     * of the character at offset, based on the run (not paragraph) direction
     */
    CARAPI_(Float) MeasureRun(
        /* [in] */ Int32 start,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 limit,
        /* [in] */ Boolean runIsRtl,
        /* [in] */ IPaintFontMetricsInt* fmi);

    /**
     * Returns the next valid offset within this directional run, skipping
     * conjuncts and zero-width characters.  This should not be called to walk
     * off the end of the line, since the returned values might not be valid
     * on neighboring lines.  If the returned offset is less than zero or
     * greater than the line length, the offset should be recomputed on the
     * preceding or following line, respectively.
     *
     * @param runIndex the run index
     * @param runStart the start of the run
     * @param runLimit the limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param offset the offset
     * @param after true if the new offset should logically follow the provided
     * offset
     * @return the new offset
     */
    CARAPI_(Int32) GetOffsetBeforeAfter(
        /* [in] */ Int32 runIndex,
        /* [in] */ Int32 runStart,
        /* [in] */ Int32 runLimit,
        /* [in] */ Boolean runIsRtl,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean after);

    /**
     * @param wp
     */
    static CARAPI_(void) ExpandMetricsFromPaint(
        /* [in] */ IPaintFontMetricsInt* fmi,
        /* [in] */ ITextPaint* wp);

    /**
     * Utility function for measuring and rendering text.  The text must
     * not include a tab or emoji.
     *
     * @param wp the working paint
     * @param start the start of the text
     * @param end the end of the text
     * @param runIsRtl true if the run is right-to-left
     * @param c the canvas, can be null if rendering is not needed
     * @param x the edge of the run closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param fmi receives metrics information, can be null
     * @param needWidth true if the width of the run is needed
     * @return the signed width of the run based on the run direction; only
     * valid if needWidth is true
     */
    CARAPI_(Float) HandleText(
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
        /* [in] */ Boolean needWidth);
    /**
     * Utility function for measuring and rendering a replacement.
     *
     *
     * @param replacement the replacement
     * @param wp the work paint
     * @param start the start of the run
     * @param limit the limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param c the canvas, can be null if not rendering
     * @param x the edge of the replacement closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param fmi receives metrics information, can be null
     * @param needWidth true if the width of the replacement is needed
     * @return the signed width of the run based on the run direction; only
     * valid if needWidth is true
     */
    CARAPI_(Float) HandleReplacement(
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
        /* [in] */ Boolean needWidth);

    /**
     * Utility function for handling a unidirectional run.  The run must not
     * contain tabs or emoji but can contain styles.
     *
     *
     * @param start the line-relative start of the run
     * @param measureLimit the offset to measure to, between start and limit inclusive
     * @param limit the limit of the run
     * @param runIsRtl true if the run is right-to-left
     * @param c the canvas, can be null
     * @param x the end of the run closest to the leading margin
     * @param top the top of the line
     * @param y the baseline
     * @param bottom the bottom of the line
     * @param fmi receives metrics information, can be null
     * @param needWidth true if the width is required
     * @return the signed width of the run based on the run direction; only
     * valid if needWidth is true
     */
    CARAPI_(Float) HandleRun(
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
        /* [in] */ Boolean needWidth);

    /**
     * Render a text run with the set-up paint.
     *
     * @param c the canvas
     * @param wp the paint used to render the text
     * @param start the start of the run
     * @param end the end of the run
     * @param contextStart the start of context for the run
     * @param contextEnd the end of the context for the run
     * @param runIsRtl true if the run is right-to-left
     * @param x the x position of the left edge of the run
     * @param y the baseline of the run
     */
    CARAPI_(void) DrawTextRun(
        /* [in] */ ICanvas* c,
        /* [in] */ ITextPaint* wp,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 contextStart,
        /* [in] */ Int32 contextEnd,
        /* [in] */ Boolean runIsRtl,
        /* [in] */ Float x,
        /* [in] */ Int32 y);

private:
    static const Boolean DEBUG;

    AutoPtr<ITextPaint> mPaint;
    AutoPtr<ICharSequence> mText;
    Int32 mStart;
    Int32 mLen;
    Int32 mDir;
    AutoPtr<ILayoutDirections> mDirections;
    Boolean mHasTabs;
    AutoPtr<Layout::TabStops> mTabs;
    AutoPtr< ArrayOf<Char32> > mChars;
    Boolean mCharsValid;
    AutoPtr<ISpanned> mSpanned;
    AutoPtr<ITextPaint> mWorkPaint;
    AutoPtr< SpanSet<IMetricAffectingSpan> > mMetricAffectingSpanSpanSet;
    AutoPtr< SpanSet<ICharacterStyle> > mCharacterStyleSpanSet;
    AutoPtr< SpanSet<IReplacementSpan> > mReplacementSpanSpanSet;

    static AutoPtr< ArrayOf<TextLine*> > sCached;
    static Object sCachedLock;

    static const Int32 TAB_INCREMENT = 20;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Text::TextLine, IInterface)

#endif //__ELASTOS_DROID_TEXT_TEXTLINE_H__
