
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Emoji.h>
#include "elastos/droid/text/StaticLayout.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/AndroidBidi.h"
#include "elastos/droid/graphics/CPaintFontMetricsInt.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/logging/Slogger.h>
#include <unistd.h>
#include "unicode/locid.h"
#include "unicode/brkiter.h"
#include "utils/misc.h"
#include "utils/Log.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::CPaintFontMetricsInt;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Elastos::Droid::Text::Style::EIID_ILineHeightSpan;
using Elastos::Droid::Text::Style::EIID_ILeadingMarginSpan;
using Elastos::Droid::Text::Style::EIID_ILeadingMarginSpan2;
using Elastos::Droid::Text::Style::ILineHeightSpanWithDensity;
using Elastos::Droid::Text::Style::ILeadingMarginSpan;
using Elastos::Droid::Text::Style::ILeadingMarginSpan2;
using Elastos::Droid::Text::Style::EIID_IMetricAffectingSpan;
using Elastos::Droid::Text::Style::IMetricAffectingSpan;
using Elastos::Droid::Text::Style::EIID_ITabStopSpan;
using Elastos::Core::Character;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Text::IBreakIterator;
using Elastos::Utility::Etl::Vector;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Text {

const String StaticLayout::TAG("StaticLayout");
const Int32 StaticLayout::COLUMNS_NORMAL;
const Int32 StaticLayout::COLUMNS_ELLIPSIZE;
const Int32 StaticLayout::START;
const Int32 StaticLayout::DIR;
const Int32 StaticLayout::TAB;
const Int32 StaticLayout::TOP;
const Int32 StaticLayout::DESCENT;
const Int32 StaticLayout::ELLIPSIS_START;
const Int32 StaticLayout::ELLIPSIS_COUNT;

const Int32 StaticLayout::START_MASK;
const Int32 StaticLayout::DIR_SHIFT;
const Int32 StaticLayout::TAB_MASK;

const Int32 StaticLayout::TAB_INCREMENT;

const Char32 StaticLayout::CHAR_NEW_LINE;
const Char32 StaticLayout::CHAR_TAB;
const Char32 StaticLayout::CHAR_SPACE;
const Char32 StaticLayout::CHAR_ZWSP;

const Double StaticLayout::EXTRA_ROUNDING;

const Int32 StaticLayout::CHAR_FIRST_HIGH_SURROGATE;
const Int32 StaticLayout::CHAR_LAST_LOW_SURROGATE;

CAR_INTERFACE_IMPL(StaticLayout, Layout, IStaticLayout)

StaticLayout::StaticLayout()
    : mLineCount(0)
    , mTopPadding(0)
    , mBottomPadding(0)
    , mColumns(0)
    , mEllipsizedWidth(0)
    , mMaximumVisibleLineCount(Elastos::Core::Math::INT32_MAX_VALUE)
{
    CPaintFontMetricsInt::New((IPaintFontMetricsInt**)&mFontMetricsInt);
}

ECode StaticLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad)
{
    VALIDATE_NOT_NULL(source)
    Int32 length = 0;
    source->GetLength(&length);
    return constructor(source, 0, length, paint, width, align,
         spacingmult, spacingadd, includepad);
}

ECode StaticLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad)
{
    VALIDATE_NOT_NULL(source)
    Int32 length = 0;
    source->GetLength(&length);
    return constructor(source, 0, length, paint, width, align, textDir,
        spacingmult, spacingadd, includepad);
}

ECode StaticLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 bufstart,
    /* [in] */ Int32 bufend,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad)
{
    return constructor(source, bufstart, bufend, paint, outerwidth, align,
         spacingmult, spacingadd, includepad,
         TextUtilsTruncateAt_NONE, 0);
}

ECode StaticLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 bufstart,
    /* [in] */ Int32 bufend,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad)
{
    return constructor(source, bufstart, bufend, paint, outerwidth, align, textDir,
        spacingmult, spacingadd, includepad,
        TextUtilsTruncateAt_NONE, 0, Elastos::Core::Math::INT32_MAX_VALUE);
}

ECode StaticLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 bufstart,
    /* [in] */ Int32 bufend,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Int32 ellipsizedWidth)
{
    AutoPtr<ITextDirectionHeuristic> ltr = TextDirectionHeuristics::FIRSTSTRONG_LTR;
    return constructor(source, bufstart, bufend, paint, outerwidth, align,
            ltr,
            spacingmult, spacingadd, includepad, ellipsize,
            ellipsizedWidth, Elastos::Core::Math::INT32_MAX_VALUE);
}

/**
 * @hide
 */
ECode StaticLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 bufstart,
    /* [in] */ Int32 bufend,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Int32 ellipsizedWidth,
    /* [in] */ Int32 maxLines)
{
    VALIDATE_NOT_NULL(source)
    AutoPtr<ICharSequence> tempCS;
    if (source->Probe(EIID_ISpanned)) {
        tempCS = new SpannedEllipsizer(source);
    }
    else {
        tempCS = new Ellipsizer(source);
    }
    AutoPtr<ICharSequence> initCS;
    if (ellipsize == TextUtilsTruncateAt_NONE) {
        initCS = source;
    } else {
        initCS = tempCS;
    }

    FAIL_RETURN(Layout::constructor(initCS, paint, outerwidth, align,
        textDir, spacingmult, spacingadd))

    /*
     * This is annoying, but we can't refer to the layout until
     * superclass construction is finished, and the superclass
     * constructor wants the reference to the display text.
     *
     * This will break if the superclass constructor ever actually
     * cares about the content instead of just holding the reference.
     */
    if (ellipsize != TextUtilsTruncateAt_NONE) {
        AutoPtr<ICharSequence> csq;
        GetText((ICharSequence**)&csq);
        AutoPtr<Ellipsizer> e = (Ellipsizer*)csq.Get();
        e->mLayout = this;
        e->mWidth = ellipsizedWidth;
        e->mMethod = ellipsize;
        mEllipsizedWidth = ellipsizedWidth;

        mColumns = COLUMNS_ELLIPSIZE;
    } else {
        mColumns = COLUMNS_NORMAL;
        mEllipsizedWidth = outerwidth;
    }

    ILayoutDirections* obj = NULL;
    mLineDirections = ArrayUtils::NewUnpaddedArray(2 * mColumns, obj);
    mLines = ArrayOf<Int32>::Alloc(mLineDirections->GetLength());
    mMaximumVisibleLineCount = maxLines;

    mMeasured = MeasuredText::Obtain();

    Generate(source, bufstart, bufend, paint, outerwidth, textDir, spacingmult,
         spacingadd, includepad, includepad, ellipsizedWidth,
         ellipsize);

    mMeasured = MeasuredText::Recycle(mMeasured);
    mFontMetricsInt = NULL;
    return NOERROR;
}

/* package */
ECode StaticLayout::constructor(
    /* [in] */ ICharSequence* text)
{
    FAIL_RETURN(Layout::constructor(text, NULL, 0, ALIGN_NONE, 0.0f, 0.0f))

    mColumns = COLUMNS_ELLIPSIZE;
    ILayoutDirections* obj = NULL;
    mLineDirections = ArrayUtils::NewUnpaddedArray(2 * mColumns, obj);
    mLines = ArrayOf<Int32>::Alloc(mLineDirections->GetLength());
    // FIXME This is never recycled
    mMeasured = MeasuredText::Obtain();
    return NOERROR;
}

/* package */
ECode StaticLayout::Generate(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 bufStart,
    /* [in] */ Int32 bufEnd,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerWidth,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad,
    /* [in] */ Boolean trackpad,
    /* [in] */ Float ellipsizedWidth,
    /* [in] */ TextUtilsTruncateAt ellipsize)
{
    IPaint* p = IPaint::Probe(paint);
    AutoPtr< ArrayOf<Int32> > breakOpp;
    AutoPtr<ILocale> locale;
    p->GetTextLocale((ILocale**)&locale);
    String localeLanguageTag;
    locale->ToLanguageTag(&localeLanguageTag);

    mLineCount = 0;

    Int32 v = 0;
    Boolean needMultiply = (spacingmult != 1 || spacingadd != 0);

    AutoPtr<IPaintFontMetricsInt> fm = mFontMetricsInt;
    AutoPtr< ArrayOf<Int32> > chooseHtv;

    AutoPtr<MeasuredText> measured = mMeasured;
    AutoPtr<ISpanned> spanned;
    if (ISpanned::Probe(source))
        spanned = ISpanned::Probe(source);

    Int32 paraEnd;
    for (Int32 paraStart = bufStart; paraStart <= bufEnd; paraStart = paraEnd) {
        paraEnd = TextUtils::IndexOf(source, CHAR_NEW_LINE, paraStart, bufEnd);
        if (paraEnd < 0)
            paraEnd = bufEnd;
        else
            paraEnd++;

        Int32 firstWidthLineLimit = mLineCount + 1;
        Int32 firstWidth = outerWidth;
        Int32 restWidth = outerWidth;

        AutoPtr< ArrayOf<IInterface*> > chooseHt; // ILineHeightSpan

        if (spanned != NULL) {
            AutoPtr< ArrayOf<IInterface*> > sp = GetParagraphSpans(spanned, paraStart, paraEnd,
                    EIID_ILeadingMarginSpan);
            Int32 tempWidth;
            for (Int32 i = 0; i < sp->GetLength(); i++) {
                AutoPtr<ILeadingMarginSpan> lms = ILeadingMarginSpan::Probe((*sp)[i]);
                lms->GetLeadingMargin(TRUE, &tempWidth);
                firstWidth -= tempWidth;
                lms->GetLeadingMargin(FALSE, &tempWidth);
                restWidth -= tempWidth;

                // LeadingMarginSpan2 is odd.  The count affects all
                // leading margin spans, not just this particular one
                ILeadingMarginSpan2* lms2 = ILeadingMarginSpan2::Probe(lms);
                if (lms2) {
                    Int32 temp = 0;
                    spanned->GetSpanStart(lms2, &temp);
                    Int32 lmsFirstLine;
                    GetLineForOffset(temp, &lmsFirstLine);
                    lms2->GetLeadingMarginLineCount(&temp);
                    firstWidthLineLimit = Elastos::Core::Math::Max(firstWidthLineLimit, lmsFirstLine + temp);
                }
            }

            chooseHt = GetParagraphSpans(spanned, paraStart, paraEnd, EIID_ILineHeightSpan);

            Int32 length = chooseHt->GetLength();
            if (length != 0) {
                if (chooseHtv == NULL ||
                    chooseHtv->GetLength() < length) {
                    chooseHtv = ArrayUtils::NewUnpaddedInt32Array(chooseHt->GetLength());
                }

                for (Int32 i = 0; i < length; i++) {
                    AutoPtr<ILineHeightSpan> lhs = ILineHeightSpan::Probe((*chooseHt)[i]);
                    Int32 o = 0;
                    spanned->GetSpanStart(lhs, &o);

                    if (o < paraStart) {
                        // starts in this layout, before the
                        // current paragraph

                        Int32 lfo;
                        GetLineForOffset(o, &lfo);
                        GetLineTop(lfo, &(*chooseHtv)[i]);
                    } else {
                        // starts in this paragraph

                        (*chooseHtv)[i] = v;
                    }
                }
            }
        }

        measured->SetPara(source, paraStart, paraEnd, textDir);

        AutoPtr< ArrayOf<Char32> > chs = measured->mChars;
        AutoPtr< ArrayOf<Float> > widths = measured->mWidths;
        AutoPtr< ArrayOf<Byte> > chdirs = measured->mLevels;
        Int32 dir = measured->mDir;
        Boolean easy = measured->mEasy;

        breakOpp = nLineBreakOpportunities(localeLanguageTag, chs, paraEnd - paraStart, breakOpp);
        int breakOppIndex = 0;

        Int32 width = firstWidth;

        Float w = 0;
        // here is the offset of the starting character of the line we are currently measuring
        Int32 here = paraStart;

        // ok is a character offset located after a word separator (space, tab, number...) where
        // we would prefer to cut the current line. Equals to here when no such break was found.
        Int32 ok = paraStart;
        Float okWidth = w;
        Int32 okAscent = 0, okDescent = 0, okTop = 0, okBottom = 0;

        // fit is a character offset such that the [here, fit[ range fits in the allowed width.
        // We will cut the line there if no ok position is found.
        Int32 fit = paraStart;
        Float fitWidth = w;
        Int32 fitAscent = 0, fitDescent = 0, fitTop = 0, fitBottom = 0;
        // same as fitWidth but not including any trailing whitespace
        Float fitWidthGraphing = w;

        Boolean hasTabOrEmoji = FALSE;
        Boolean hasTab = FALSE;
        AutoPtr<TabStops> tabStops;

        for (Int32 spanStart = paraStart, spanEnd; spanStart < paraEnd; spanStart = spanEnd) {
            if (spanned == NULL) {
                spanEnd = paraEnd;
                Int32 spanLen = spanEnd - spanStart;
                measured->AddStyleRun(paint, spanLen, fm);
            }
            else {
                spanned->NextSpanTransition(spanStart, paraEnd, EIID_IMetricAffectingSpan, &spanEnd);
                Int32 spanLen = spanEnd - spanStart;
                AutoPtr< ArrayOf<IInterface*> > spans; //IMetricAffectingSpan
                spanned->GetSpans(spanStart, spanEnd, EIID_IMetricAffectingSpan,
                           (ArrayOf<IInterface*>**)&spans);
                spans = TextUtils::RemoveEmptySpans(spans, spanned, EIID_IMetricAffectingSpan);
                measured->AddStyleRun(paint, spans, spanLen, fm);
            }

            Int32 fmTop;
            Int32 fmBottom;
            Int32 fmAscent;
            Int32 fmDescent;
            assert(fm != NULL);
            fm->GetTop(&fmTop);
            fm->GetBottom(&fmBottom);
            fm->GetAscent(&fmAscent);
            fm->GetDescent(&fmDescent);

            for (Int32 j = spanStart; j < spanEnd; j++) {
                Char32 c = (*chs)[j - paraStart];

                if (c == CHAR_NEW_LINE) {
                    // intentionally left empty
                }
                else if (c == CHAR_TAB) {
                    if (hasTab == FALSE) {
                        hasTab = TRUE;
                        hasTabOrEmoji = TRUE;
                        if (spanned != NULL) {
                            // First tab this para, check for tabstops
                            AutoPtr< ArrayOf<IInterface*> > spans = GetParagraphSpans(spanned, paraStart,
                                    paraEnd, EIID_ITabStopSpan);//ITabStopSpan*
                            if (spans->GetLength() > 0) {
                                tabStops = new TabStops(TAB_INCREMENT, spans);
                            }
                        }
                    }
                    if (tabStops != NULL) {
                        w = tabStops->NextTab(w);
                    } else {
                        w = tabStops->NextDefaultStop(w, TAB_INCREMENT);
                    }
                }
                else if (c >= CHAR_FIRST_HIGH_SURROGATE && c <= CHAR_LAST_LOW_SURROGATE
                        && j + 1 < spanEnd) {
                    assert(0 && "TODO");
                    Int32 emoji = 0;
//                    Int32 emoji = Character.codePointAt(chs, j - paraStart);
                    if (emoji >= MIN_EMOJI && emoji <= MAX_EMOJI) {
                        AutoPtr<IBitmap> bm;
                        EMOJI_FACTORY->GetBitmapFromAndroidPua(emoji, (IBitmap**)&bm);
                        if (bm != NULL) {
                            AutoPtr<IPaint> whichPaint;
                            if (spanned == NULL) {
                                whichPaint = p;
                            } else {
                                whichPaint = IPaint::Probe(mWorkPaint);
                            }

                            Int32 width, height;
                            bm->GetWidth(&width);
                            bm->GetHeight(&height);
                            Float ascent;
                            whichPaint->Ascent(&ascent);
                            Float wid = width * -ascent / height;

                            w += wid;
                            hasTabOrEmoji = TRUE;
                            j++;
                        } else {
                            w += (*widths)[j - paraStart];
                        }

                    } else {
                        w += (*widths)[j - paraStart];
                    }
                } else {
                    w += (*widths)[j - paraStart];
                }

                Boolean isSpaceOrTab = c == CHAR_SPACE || c == CHAR_TAB || c == CHAR_ZWSP;

                if (w <= width || isSpaceOrTab) {
                    fitWidth = w;
                    fit = j + 1;

                    if (fmTop < fitTop)
                        fitTop = fmTop;
                    if (fmAscent < fitAscent)
                        fitAscent = fmAscent;
                    if (fmDescent > fitDescent)
                        fitDescent = fmDescent;
                    if (fmBottom > fitBottom)
                        fitBottom = fmBottom;

                    while ((*breakOpp)[breakOppIndex] != -1
                            && (*breakOpp)[breakOppIndex] < j - paraStart + 1) {
                        breakOppIndex++;
                    }
                    Boolean isLineBreak = breakOppIndex < breakOpp->GetLength() &&
                            (*breakOpp)[breakOppIndex] == j - paraStart + 1;

                    if (isLineBreak) {
                        okWidth = fitWidthGraphing;
                        ok = j + 1;

                        if (fitTop < okTop)
                            okTop = fitTop;
                        if (fitAscent < okAscent)
                            okAscent = fitAscent;
                        if (fitDescent > okDescent)
                            okDescent = fitDescent;
                        if (fitBottom > okBottom)
                            okBottom = fitBottom;
                    }
                } else {
                    Boolean moreChars;
                    Int32 endPos;
                    Int32 above, below, top, bottom;
                    Float currentTextWidth;

                    if (ok != here) {
                        endPos = ok;
                        above = okAscent;
                        below = okDescent;
                        top = okTop;
                        bottom = okBottom;
                        currentTextWidth = okWidth;
                        moreChars = (j + 1 < spanEnd);
                    } else if (fit != here) {
                        endPos = fit;
                        above = fitAscent;
                        below = fitDescent;
                        top = fitTop;
                        bottom = fitBottom;
                        currentTextWidth = fitWidth;
                        moreChars = (j + 1 < spanEnd);
                    } else {
                        // must make progress, so take next character
                        endPos = here + 1;
                        // but to deal properly with clusters
                        // take all zero width characters following that
                        while (endPos < spanEnd && (*widths)[endPos - paraStart] == 0) {
                            endPos++;
                        }
                        above = fmAscent;
                        below = fmDescent;
                        top = fmTop;
                        bottom = fmBottom;
                        currentTextWidth = (*widths)[here - paraStart];
                        moreChars = (endPos < spanEnd);
                    }

                    v = Out(source, here, endPos,
                            above, below, top, bottom,
                            v, spacingmult, spacingadd, chooseHt, chooseHtv, fm, hasTabOrEmoji,
                            needMultiply, chdirs, dir, easy, bufEnd, includepad, trackpad,
                            chs, widths, paraStart, ellipsize, ellipsizedWidth,
                            currentTextWidth, paint, moreChars);

                    here = endPos;
                    j = here - 1; // restart j-span loop from here, compensating for the j++
                    ok = fit = here;
                    w = 0;
                    fitWidthGraphing = w;
                    fitAscent = fitDescent = fitTop = fitBottom = 0;
                    okAscent = okDescent = okTop = okBottom = 0;

                    if (--firstWidthLineLimit <= 0) {
                        width = restWidth;
                    }

                    if (here < spanStart) {
                        // The text was cut before the beginning of the current span range.
                        // Exit the span loop, and get spanStart to start over from here.
                        measured->SetPos(here);
                        spanEnd = here;
                        break;
                    }

                    if (mLineCount >= mMaximumVisibleLineCount) {
                        return NOERROR;
                    }
                }
            }
        }

        if (paraEnd != here && mLineCount < mMaximumVisibleLineCount) {
            if ((fitTop | fitBottom | fitDescent | fitAscent) == 0) {
                Int32 spacing;
                p->GetFontMetricsInt(fm, &spacing);

                fm->GetTop(&fitTop);
                fm->GetBottom(&fitBottom);
                fm->GetAscent(&fitAscent);
                fm->GetDescent(&fitDescent);
            }

            // Log.e("text", "output rest " + here + " to " + end);

            v = Out(source,
                    here, paraEnd, fitAscent, fitDescent,
                    fitTop, fitBottom,
                    v,
                    spacingmult, spacingadd, chooseHt,
                    chooseHtv, fm, hasTabOrEmoji,
                    needMultiply, chdirs, dir, easy, bufEnd,
                    includepad, trackpad, chs,
                    widths, paraStart, ellipsize,
                    ellipsizedWidth, w, paint, paraEnd != bufEnd);
        }

        paraStart = paraEnd;

        if (paraEnd == bufEnd)
            break;
    }

    Char32 charAt = 0;
    source->GetCharAt(bufEnd - 1, &charAt);
    if ((bufEnd == bufStart || charAt == CHAR_NEW_LINE) &&
            mLineCount < mMaximumVisibleLineCount) {
        // Log.e("text", "output last " + bufEnd);

        Int32 spacing;
        p->GetFontMetricsInt(fm, &spacing);

        Int32 ascent, descent, top, bottom;
        fm->GetAscent(&ascent);
        fm->GetDescent(&descent);
        fm->GetTop(&top);
        fm->GetBottom(&bottom);

        v = Out(source,
                bufEnd, bufEnd, ascent, descent,
                top, bottom,
                v,
                spacingmult, spacingadd, NULL,
                NULL, fm, FALSE,
                needMultiply, measured->mLevels, measured->mDir, measured->mEasy, bufEnd,
                includepad, trackpad, NULL,
                NULL, bufStart, ellipsize,
                ellipsizedWidth, 0, paint, FALSE);
    }

    return NOERROR;
}

ECode StaticLayout::GetLineForVertical(
    /* [in] */ Int32 vertical,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 high = mLineCount;
    Int32 low = -1;
    Int32 guess;
    AutoPtr< ArrayOf<Int32> > lines = mLines;
    while (high - low > 1) {
        guess = (high + low) >> 1;
        if ((*lines)[mColumns * guess + TOP] > vertical){
            high = guess;
        } else {
            low = guess;
        }
    }

    if (low < 0) {
        *result = 0;
    } else {
        *result = low;
    }

    return NOERROR;
}

//@Override
ECode StaticLayout::GetLineCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLineCount;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetLineTop(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(mLines != NULL);
    Int32 top = (*mLines)[mColumns * line + TOP];
    if (mMaximumVisibleLineCount > 0 && line >= mMaximumVisibleLineCount
        && line != mLineCount) {
        Int32 padding;
        GetBottomPadding(&padding);
        top += padding;
    }
    *result = top;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetLineDescent(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 descent = (*mLines)[mColumns * line + DESCENT];
    if (mMaximumVisibleLineCount > 0 && line >= mMaximumVisibleLineCount - 1
        && line != mLineCount) {
        Int32 padding;
        GetBottomPadding(&padding);
        descent += padding;
    }
    *result = descent;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetLineStart(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (*mLines)[mColumns * line + START] & START_MASK;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetParagraphDirection(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (*mLines)[mColumns * line + DIR] >> DIR_SHIFT;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetLineContainsTab(
    /* [in] */ Int32 line,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((*mLines)[mColumns * line + TAB] & TAB_MASK) != 0;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetLineDirections(
    /* [in] */ Int32 line,
    /* [out] */ ILayoutDirections** result)
{
    VALIDATE_NOT_NULL(result)
    assert(line >= 0);
    AutoPtr<ILayoutDirections> ld;
    if (mLineDirections != NULL && line >= 0 && line < mLineDirections->GetLength()) {
        ld = (*mLineDirections)[line];
    }
    *result = ld;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//@Override
ECode StaticLayout::GetTopPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTopPadding;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetBottomPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBottomPadding;
    return NOERROR;
}

//@Override
ECode StaticLayout::GetEllipsisCount(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mColumns < COLUMNS_ELLIPSIZE) {
        *result = 0;
        return NOERROR;
    }

    *result = (*mLines)[mColumns * line + ELLIPSIS_COUNT];
    return NOERROR;
}

//@Override
ECode StaticLayout::GetEllipsisStart(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (mColumns < COLUMNS_ELLIPSIZE) {
        *result = 0;
        return NOERROR;
    }

    *result = (*mLines)[mColumns * line + ELLIPSIS_START];
    return NOERROR;
}

//@Override
ECode StaticLayout::GetEllipsizedWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEllipsizedWidth;
    return NOERROR;
}

ECode StaticLayout::Prepare()
{
    mMeasured = MeasuredText::Obtain();
    return NOERROR;
}

ECode StaticLayout::Finish()
{
    mMeasured = MeasuredText::Recycle(mMeasured);
    return NOERROR;
}

Int32 StaticLayout::Out(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 above,
    /* [in] */ Int32 below,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 v,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ ArrayOf<IInterface*>* chooseHt,//ILineHeightSpan
    /* [in] */ ArrayOf<Int32>* chooseHtv,
    /* [in] */ IPaintFontMetricsInt* fm,
    /* [in] */ Boolean hasTabOrEmoji,
    /* [in] */ Boolean needMultiply,
    /* [in] */ ArrayOf<Byte>* chdirs,
    /* [in] */ Int32 dir,
    /* [in] */ Boolean easy,
    /* [in] */ Int32 bufEnd,
    /* [in] */ Boolean includePad,
    /* [in] */ Boolean trackPad,
    /* [in] */ ArrayOf<Char32>* chs,
    /* [in] */ ArrayOf<Float>* widths,
    /* [in] */ Int32 widthStart,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Float ellipsisWidth,
    /* [in] */ Float textWidth,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Boolean moreChars)
{
    Int32 j = mLineCount;
    Int32 off = j * mColumns;
    Int32 want = off + mColumns + TOP;
    AutoPtr< ArrayOf<Int32> > lines = mLines;

    if (want >= lines->GetLength()) {
        ILayoutDirections* obj = NULL;
        AutoPtr< ArrayOf<ILayoutDirections*> > grow2 = ArrayUtils::NewUnpaddedArray(GrowingArrayUtils::GrowSize(want), obj);
        grow2->Copy(mLineDirections);
        mLineDirections = grow2;

        AutoPtr< ArrayOf<Int32> > grow = ArrayOf<Int32>::Alloc(grow2->GetLength());
        grow->Copy(lines);
        mLines = grow;
        lines = grow;
    }

    if (chooseHt != NULL) {
        fm->SetAscent(above);
        fm->SetDescent(below);
        fm->SetTop(top);
        fm->SetBottom(bottom);

        for (Int32 i = 0; i < chooseHt->GetLength(); i++) {
            ILineHeightSpan* obj = ILineHeightSpan::Probe((*chooseHt)[i]);
            assert(obj != NULL);
            ILineHeightSpanWithDensity* lhsd = ILineHeightSpanWithDensity::Probe(obj);
            if (lhsd) {
                lhsd->ChooseHeight(text, start, end, (*chooseHtv)[i], v, fm, paint);
            } else {
                obj->ChooseHeight(text, start, end, (*chooseHtv)[i], v, fm);
            }
        }

        fm->GetAscent(&above);
        fm->GetDescent(&below);
        fm->GetTop(&top);
        fm->GetBottom(&bottom);
    }

    Boolean firstLine = (j == 0);
    Boolean currentLineIsTheLastVisibleOne = (j + 1 == mMaximumVisibleLineCount);
    Boolean lastLine = currentLineIsTheLastVisibleOne || (end == bufEnd);
    if (firstLine) {
        if (trackPad) {
            mTopPadding = top - above;
        }

        if (includePad) {
            above = top;
        }
    }

    Int32 extra;

    if (lastLine) {
        if (trackPad) {
            mBottomPadding = bottom - below;
        }

        if (includePad) {
            below = bottom;
        }
    }

    if (needMultiply && !lastLine) {
        Double ex = (below - above) * (spacingmult - 1) + spacingadd;
        if (ex >= 0) {
            extra = (Int32)(ex + EXTRA_ROUNDING);
        } else {
            extra = -(Int32)(-ex + EXTRA_ROUNDING);
        }
    } else {
        extra = 0;
    }

    (*lines)[off + START] = start;
    (*lines)[off + TOP] = v;
    (*lines)[off + DESCENT] = below + extra;

    v += (below - above) + extra;
    (*lines)[off + mColumns + START] = end;
    (*lines)[off + mColumns + TOP] = v;

    if (hasTabOrEmoji)
        (*lines)[off + TAB] |= TAB_MASK;

    (*lines)[off + DIR] |= dir << DIR_SHIFT;

    // easy means all chars < the first RTL, so no emoji, no nothing
    // XXX a run with no text or all spaces is easy but might be an empty
    // RTL paragraph.  Make sure easy is false if this is the case.
    AutoPtr<ILayoutDirections> linedirs;
    if (easy) {
        linedirs = DIRS_ALL_LEFT_TO_RIGHT;
    } else {
        linedirs = AndroidBidi::Directions(dir, chdirs, start - widthStart, chs,
                start - widthStart, end - start);
    }
    mLineDirections->Set(j, linedirs);

    if (ellipsize != TextUtilsTruncateAt_NONE) {
        // If there is only one line, then do any type of ellipsis except when it is MARQUEE
        // if there are multiple lines, just allow END ellipsis on the last line
        Boolean forceEllipsis = moreChars && (mLineCount + 1 == mMaximumVisibleLineCount);

        Boolean doEllipsis =
                    (((mMaximumVisibleLineCount == 1 && moreChars) || (firstLine && !moreChars)) &&
                            ellipsize != TextUtilsTruncateAt_MARQUEE) ||
                    (!firstLine && (currentLineIsTheLastVisibleOne || !moreChars) &&
                            ellipsize == TextUtilsTruncateAt_END);
        if (doEllipsis) {
            CalculateEllipsis(start, end, widths, widthStart,
                    ellipsisWidth, ellipsize, j,
                    textWidth, paint, forceEllipsis);
        }
    }

    mLineCount++;
    return v;
}

void StaticLayout::CalculateEllipsis(
    /* [in] */ Int32 lineStart,
    /* [in] */ Int32 lineEnd,
    /* [in] */ ArrayOf<Float>* widths,
    /* [in] */ Int32 widthStart,
    /* [in] */ Float avail,
    /* [in] */ TextUtilsTruncateAt where,
    /* [in] */ Int32 line,
    /* [in] */ Float textWidth,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Boolean forceEllipsis)
{
    assert(paint != NULL);

    if (textWidth <= avail && !forceEllipsis) {
        // Everything fits!
        (*mLines)[mColumns * line + ELLIPSIS_START] = 0;
        (*mLines)[mColumns * line + ELLIPSIS_COUNT] = 0;
        return;
    }

    IPaint* p = IPaint::Probe(paint);
    Float ellipsisWidth = 0.0f;
    AutoPtr< ArrayOf<Char32> > text = ArrayOf<Char32>::Alloc(1);
    (*text)[0] = (where == TextUtilsTruncateAt_END_SMALL) ?
            ELLIPSIS_TWO_DOTS[0] : ELLIPSIS_NORMAL[0];
    p->MeasureText(text, 0, 1, &ellipsisWidth);
    Int32 ellipsisStart = 0;
    Int32 ellipsisCount = 0;
    Int32 len = lineEnd - lineStart;

    // We only support start ellipsis on a single line
    if (where == TextUtilsTruncateAt_START) {
        if (mMaximumVisibleLineCount == 1) {
            Float sum = 0;
            Int32 i;

            for (i = len; i > 0; i--) {
                assert(widths != NULL);
                Float w = (*widths)[i - 1 + lineStart - widthStart];

                if (w + sum + ellipsisWidth > avail) {
                    break;
                }

                sum += w;
            }

            ellipsisStart = 0;
            ellipsisCount = i;
        } else {
//            if (Log.isLoggable(TAG, Log.WARN)) {
//                Log.w(TAG, "Start Ellipsis only supported with one line");
//            }
        }
    } else if (where == TextUtilsTruncateAt_END || where == TextUtilsTruncateAt_MARQUEE ||
            where == TextUtilsTruncateAt_END_SMALL) {
        Float sum = 0;
        Int32 i;

        for (i = 0; i < len; i++) {
            Float w = (*widths)[i + lineStart - widthStart];

            if (w + sum + ellipsisWidth > avail) {
                break;
            }

            sum += w;
        }

        ellipsisStart = i;
        ellipsisCount = len - i;
        if (forceEllipsis && ellipsisCount == 0 && len > 0) {
            ellipsisStart = len - 1;
            ellipsisCount = 1;
        }
    } else {
        // where = TextUtilsTruncateAt_MIDDLE We only support middle ellipsis on a single line
        if (mMaximumVisibleLineCount == 1) {
            Float lsum = 0, rsum = 0;
            Int32 left = 0, right = len;

            Float ravail = (avail - ellipsisWidth) / 2;
            for (right = len; right >= 0; right--) {
                assert(widths != NULL);
                Float w = (*widths)[right - 1 + lineStart - widthStart];

                if (w + rsum > ravail) {
                    break;
                }

                rsum += w;
            }

            Float lavail = avail - ellipsisWidth - rsum;
            for (left = 0; left < right; left++) {
                Float w = (*widths)[left + lineStart - widthStart];

                if (w + lsum > lavail) {
                    break;
                }

                lsum += w;
            }

            ellipsisStart = left;
            ellipsisCount = right - left;
        } else {
//            if (Log.isLoggable(TAG, Log.WARN)) {
//                Log.w(TAG, "Middle Ellipsis only supported with one line");
//            }
        }
    }

    (*mLines)[mColumns * line + ELLIPSIS_START] = ellipsisStart;
    (*mLines)[mColumns * line + ELLIPSIS_COUNT] = ellipsisCount;
}


class ScopedIcuLocale {
public:
    ScopedIcuLocale(
        /* [in] */ const String& localeName)
    {
        mLocale.setToBogus();

        assert(localeName.IsNull() == FALSE && "javaLocaleName == null");
        // if (javaLocaleName.IsNull()) {
        //     jniThrowNullPointerException(mEnv, "javaLocaleName == null");
        //     return;
        // }

        mLocale = Locale::createFromName(localeName.string());
    }

    ~ScopedIcuLocale() {
    }

    bool valid() const {
        return !mLocale.isBogus();
    }

    Locale& locale() {
        return mLocale;
    }

private:
    Locale mLocale;

    // Disallow copy and assignment.
    ScopedIcuLocale(const ScopedIcuLocale&);
    void operator=(const ScopedIcuLocale&);
};

class ScopedBreakIterator {
public:
    ScopedBreakIterator(
        /* [in] */ BreakIterator* breakIterator,
        /* [in] */ ArrayOf<Char32>* inputText,
        /* [in] */ Int32 length)
        : mBreakIterator(breakIterator)
    {
        UErrorCode status = U_ZERO_ERROR;
        mChars = String(*inputText).GetChar16s();
        mUText = utext_openUChars(NULL, (const UChar*)mChars->GetPayload(), length, &status);
        if (mUText == NULL) {
            return;
        }

        mBreakIterator->setText(mUText, status);
    }

    inline BreakIterator* operator->() {
        return mBreakIterator;
    }

    ~ScopedBreakIterator() {
        utext_close(mUText);
        delete mBreakIterator;
    }
private:
    BreakIterator* mBreakIterator;
    AutoPtr<ArrayOf<Char16> > mChars;
    UText* mUText;

    // disable copying and assignment
    ScopedBreakIterator(const ScopedBreakIterator&);
    void operator=(const ScopedBreakIterator&);
};

AutoPtr<ArrayOf<Int32> > StaticLayout::nLineBreakOpportunities(
    /* [in] */ const String& locale,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Int32>* recycle)
{
    AutoPtr<ArrayOf<Int32> > ret;

    Vector<Int32> breaks;

    ScopedIcuLocale icuLocale(locale);
    if (icuLocale.valid()) {
        UErrorCode status = U_ZERO_ERROR;
        BreakIterator* it = BreakIterator::createLineInstance(icuLocale.locale(), status);
        if (!U_SUCCESS(status) || it == NULL) {
            if (it) {
                delete it;
            }
        }
        else {
            ScopedBreakIterator breakIterator(it, text, length);
            for (int loc = breakIterator->first(); loc != IBreakIterator::DONE;
                loc = breakIterator->next()) {
                breaks.PushBack(loc);
            }
        }
    }

    breaks.PushBack(-1); // sentinel terminal value

    if (recycle != NULL && recycle->GetLength() >= (Int32)(breaks.GetSize())) {
        ret = recycle;
    }
    else {
        ret = ArrayOf<Int32>::Alloc(breaks.GetSize());
    }

    if (ret != NULL) {
        Vector<Int32>::Iterator it;
        Int32 i = 0;
        for (it = breaks.Begin(); it != breaks.End(); ++it) {
            ret->Set(i++, *it);
        }
    }

    return ret;
}


} // namespace Text
} // namepsace Droid
} // namespace Elastos
