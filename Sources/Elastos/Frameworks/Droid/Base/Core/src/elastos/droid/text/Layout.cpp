
#include "elastos/droid/text/Layout.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/TextLine.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/text/CSpannableString.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/CLayoutDirections.h"
#include "elastos/droid/text/MeasuredText.h"
#include "elastos/droid/text/AndroidBidi.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"
#include "elastos/droid/emoji/CEmojiFactoryHelper.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Emoji::IEmojiFactoryHelper;
using Elastos::Droid::Emoji::CEmojiFactoryHelper;
using Elastos::Droid::Graphics::CRect;
//using Elastos::Droid::Graphics::PathDirection;
using Elastos::Droid::Graphics::PathDirection_CW;
using Elastos::Droid::Text::MeasuredText;
using Elastos::Droid::Text::AndroidBidi;
using Elastos::Droid::Text::Style::EIID_IParagraphStyle;
using Elastos::Droid::Text::Style::EIID_IAlignmentSpan;
using Elastos::Droid::Text::Style::EIID_ILeadingMarginSpan;
using Elastos::Droid::Text::Style::EIID_ILeadingMarginSpan2;
using Elastos::Droid::Text::Style::EIID_ITabStopSpan;
using Elastos::Droid::Text::Style::EIID_ILineBackgroundSpan;
using Elastos::Droid::Text::Style::EIID_IReplacementSpan;
using Elastos::Droid::Text::Style::ITabStopSpan;
using Elastos::Droid::Text::Style::IAlignmentSpan;
using Elastos::Droid::Text::Style::ILeadingMarginSpan;
using Elastos::Droid::Text::Style::ILeadingMarginSpan2;
using Elastos::Droid::Text::Method::MetaKeyKeyListener;
using Elastos::Droid::Text::Method::IMetaKeyKeyListener;
using Elastos::Droid::Text::MeasuredText;
using Elastos::Droid::Text::AndroidBidi;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;

using Elastos::Core::CString;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Text {

Int32 Layout::MIN_EMOJI;
Int32 Layout::MAX_EMOJI;
Object Layout::sTempRectLock;

const Int32 Layout::TAB_INCREMENT;
const Char32 Layout::ELLIPSIS_NORMAL[] = { 0x2026 }; // this is "..."
const Char32 Layout::ELLIPSIS_TWO_DOTS[] = { 0x2025 }; // this is ".."

//========================================================================
//          Layout::Ellipsizer
//========================================================================
CAR_INTERFACE_IMPL_2(Layout::Ellipsizer, Object, IGetChars, ICharSequence)

Layout::Ellipsizer::Ellipsizer(
    /* [in] */ ICharSequence* s)
    : mText(s)
    , mLayout(NULL)
    , mWidth(0)
    , mMethod(TextUtilsTruncateAt_NONE)
{
}

ECode Layout::Ellipsizer::GetCharAt(
    /* [in] */ Int32 off,
    /* [out] */ Char32* ch)
{
    VALIDATE_NOT_NULL(ch);

    AutoPtr< ArrayOf<Char32> > buf = TextUtils::Obtain(1);
    GetChars(off, off + 1, buf, 0);
    *ch = (*buf)[0];

    TextUtils::Recycle(buf);
    return NOERROR;
}

ECode Layout::Ellipsizer::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    Int32 line1, line2;
    mLayout->GetLineForOffset(start, &line1);
    mLayout->GetLineForOffset(end, &line2);

    TextUtils::GetChars(mText, start, end, dest, destoff);

    for (Int32 i = line1; i <= line2; i++) {
        mLayout->Ellipsize(start, end, i, dest, destoff, mMethod);
    }

    return NOERROR;
}

ECode Layout::Ellipsizer::GetLength(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len);

    return mText->GetLength(len);
}

ECode Layout::Ellipsizer::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** subcsq)
{
    VALIDATE_NOT_NULL(subcsq);
    *subcsq = NULL;

    AutoPtr< ArrayOf<Char32> > s = ArrayOf<Char32>::Alloc(end - start);
    GetChars(start, end, s, 0);
    String str = String(*s);
    FAIL_RETURN(CString::New(str, subcsq))

    return NOERROR;
}

ECode Layout::Ellipsizer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    Int32 len;
    GetLength(&len);
    AutoPtr< ArrayOf<Char32> > s = ArrayOf<Char32>::Alloc(len);
    GetChars(0, len, s, 0);
    *str = String(*s);
    return NOERROR;
}

//========================================================================
//          Layout::TabStops
//========================================================================

Layout::TabStops::TabStops(
    /* [in] */ Int32 increment,
    /* [in] */ ArrayOf<IInterface*>* spans)
    : mNumStops(0)
    , mIncrement(0)
{
    Reset(increment, spans);
}

ECode Layout::TabStops::Reset(
    /* [in] */ Int32 increment,
    /* [in] */ ArrayOf<IInterface*>* spans)
{
    mIncrement = increment;

    Int32 ns = 0;
    if (spans != NULL) {
        AutoPtr< ArrayOf<Int32> > stops = mStops;

        Int32 length = spans->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if ((*spans)[i] == NULL) {
                continue;
            }

            ITabStopSpan* tss = (ITabStopSpan*)((*spans)[i]->Probe(EIID_ITabStopSpan));
            if (tss) {
                if (stops == NULL) {
                    stops = ArrayOf<Int32>::Alloc(10);
                }
                else if (ns == stops->GetLength()) {
                    AutoPtr< ArrayOf<Int32> > nstops = ArrayOf<Int32>::Alloc(ns * 2);
                    for (Int32 j = 0; j < ns; ++j) {
                        (*nstops)[j] = (*stops)[j];
                    }
                    stops = nstops;
                }

                Int32 temp = 0;
                tss->GetTabStop(&temp);
                (*stops)[ns++] = temp;
            }
        }

        if (ns > 1) {
            assert(0 && "TODO");
            // ArrayUtils::Sort(stops, 0, ns);
        }

        if (stops != mStops) {
            mStops = stops;
        }
    }

    mNumStops = ns;
    return NOERROR;
}

Float Layout::TabStops::NextTab(
    /* [in] */ Float h)
{
    Int32 ns = mNumStops;
    if (ns > 0) {
        AutoPtr< ArrayOf<Int32> > stops = mStops;
        for (Int32 i = 0; i < ns; ++i) {
            Int32 stop = (*stops)[i];
            if (stop > h) {
                return stop;
            }
        }
    }

    return NextDefaultStop(h, mIncrement);
}

Float Layout::TabStops::NextDefaultStop(
    /* [in] */ Float h,
    /* [in] */ Int32 inc)
{
    return ((Int32) ((h + inc) / inc)) * inc;
}

//========================================================================
//          Layout::SpannedEllipsizer
//========================================================================
CAR_INTERFACE_IMPL(Layout::SpannedEllipsizer, Ellipsizer, ISpanned)

Layout::SpannedEllipsizer::SpannedEllipsizer(
    /* [in] */ ICharSequence* display)
    : Ellipsizer(display)
{
    mSpanned = ISpanned::Probe(display);
}

ECode Layout::SpannedEllipsizer::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    return Ellipsizer::GetChars(start, end, dest, destoff);
}

ECode Layout::SpannedEllipsizer::GetLength(
    /* [out] */ Int32* len)
{
    return Ellipsizer::GetLength(len);
}

ECode Layout::SpannedEllipsizer::GetCharAt(
    /* [in] */ Int32 off,
    /* [out] */ Char32* ch)
{
    return Ellipsizer::GetCharAt(off, ch);
}

//ECode Layout::SpannedEllipsizer::ToString(
//    /* [in] */ Int32 off,
//    /* [out] */ Char32* ch)
//{
//    return Ellipsizer::ToString(off, ch);
//}

ECode Layout::SpannedEllipsizer::ToString(
    /* [out] */ String* str)
{
    return Ellipsizer::ToString(str);
}

ECode Layout::SpannedEllipsizer::GetSpans(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& type,
    /* [out, callee] */ ArrayOf<IInterface*>** objs)
{
    return mSpanned->GetSpans(start, end, type, objs);
}

ECode Layout::SpannedEllipsizer::GetSpanStart(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* start)
{
    return mSpanned->GetSpanStart(tag, start);
}

ECode Layout::SpannedEllipsizer::GetSpanEnd(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* end)
{
    return mSpanned->GetSpanEnd(tag, end);
}

ECode Layout::SpannedEllipsizer::GetSpanFlags(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* flags)
{
    return mSpanned->GetSpanFlags(tag, flags);
}

ECode Layout::SpannedEllipsizer::NextSpanTransition(
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ const InterfaceID& type,
    /* [out] */ Int32* offset)
{
    return mSpanned->NextSpanTransition(start, limit, type, offset);
}

ECode Layout::SpannedEllipsizer::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** subcsq)
{
    VALIDATE_NOT_NULL(subcsq);
    *subcsq = NULL;

    AutoPtr< ArrayOf<Char32> > s = TextUtils::Obtain(end - start);
    GetChars(start, end, s, 0);

    AutoPtr<ICharSequence> seq;
    String str = String(*s, 0, s->GetLength());
    FAIL_RETURN(CString::New(str, (ICharSequence**)&seq))

    AutoPtr<ISpannableString> ss;
    FAIL_RETURN(CSpannableString::New(seq, (ISpannableString**)&ss));
    TextUtils::CopySpansFrom(mSpanned, start, end, EIID_IInterface, ISpannable::Probe(ss), 0);
    *subcsq = ICharSequence::Probe(ss);
    REFCOUNT_ADD(*subcsq);
    return NOERROR;
}

//========================================================================
// Layout
//========================================================================

static AutoPtr<CRect> InitsTempRect()
{
    AutoPtr<CRect> tmp;
    CRect::NewByFriend((CRect**)&tmp);
    return tmp;
}

static AutoPtr<ILayoutDirections> InitDIRS_ALL_LEFT_TO_RIGHT()
{
    AutoPtr< ArrayOf<Int32> > val = ArrayOf<Int32>::Alloc(2);
    (*val)[0] = 0;
    (*val)[1] = ILayout::RUN_LENGTH_MASK;

    AutoPtr<ILayoutDirections> obj;
    CLayoutDirections::New(val, (ILayoutDirections**)&obj);
    return obj;
}

static AutoPtr<ILayoutDirections> InitDIRS_ALL_RIGHT_TO_LEFT()
{
    AutoPtr< ArrayOf<Int32> > val = ArrayOf<Int32>::Alloc(2);
    (*val)[0] = 0;
    (*val)[1] = ILayout::RUN_LENGTH_MASK | ILayout::RUN_RTL_FLAG;

    AutoPtr<ILayoutDirections> obj;
    CLayoutDirections::New(val, (ILayoutDirections**)&obj);
    return obj;
}


static AutoPtr<IEmojiFactory> InitEMOJI()
{
    AutoPtr<IEmojiFactoryHelper> efh;
    CEmojiFactoryHelper::AcquireSingleton((IEmojiFactoryHelper**)&efh);
    AutoPtr<IEmojiFactory> ef;
    // TODO:
    // efh->NewAvailableInstance((IEmojiFactory**)&ef);
    // if (ef != NULL) {
    //     ef->GetMinimumAndroidPua(&(Layout::MIN_EMOJI));
    //     ef->GetMaximumAndroidPua(&(Layout::MAX_EMOJI));
    // }
    // else {
    //     Layout::MIN_EMOJI = -1;
    //     Layout::MAX_EMOJI = -1;
    // }

    return ef;
}


// AutoPtr< ArrayOf<IParagraphStyle*> > Layout::NO_PARA_SPANS = ArrayOf<IParagraphStyle*>::Alloc(0);
AutoPtr< ArrayOf<IInterface*> > Layout::NO_PARA_SPANS = ArrayOf<IInterface*>::Alloc(0);
AutoPtr<IEmojiFactory> Layout::EMOJI_FACTORY = InitEMOJI();
AutoPtr<IRect> Layout::sTempRect = InitsTempRect();
AutoPtr<ILayoutDirections> Layout::DIRS_ALL_LEFT_TO_RIGHT = InitDIRS_ALL_LEFT_TO_RIGHT();
AutoPtr<ILayoutDirections> Layout::DIRS_ALL_RIGHT_TO_LEFT = InitDIRS_ALL_RIGHT_TO_LEFT();

CAR_INTERFACE_IMPL(Layout, Object, ILayout)

Layout::Layout()
    : mWidth(0)
    , mAlignment(ALIGN_NORMAL)
    , mSpacingMult(0)
    , mSpacingAdd(0)
    , mSpannedText(FALSE)
{
}

Layout::~Layout()
{
}

Float Layout::GetDesiredWidth(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint)
{
    assert(source != NULL);
    Int32 len = 0;
    source->GetLength(&len);
    return GetDesiredWidth(source, 0, len, paint);
}

Float Layout::GetDesiredWidth(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ITextPaint* paint)
{
    Float need = 0;

    Int32 next;
    for (Int32 i = start; i <= end; i = next) {
        next = TextUtils::IndexOf(source, '\n', i, end);

        if (next < 0)
            next = end;

        // note, omits trailing paragraph char
        Float w = MeasurePara(paint, source, i, next);

        if (w > need)
            need = w;

        next++;
    }

    return need;
}

ECode Layout::constructor(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingMult,
    /* [in] */ Float spacingAdd)
{
    AutoPtr<ITextDirectionHeuristic> tdh = TextDirectionHeuristics::FIRSTSTRONG_LTR;
    return constructor(text, paint, width, align, tdh,
            spacingMult, spacingAdd);
}

ECode Layout::constructor(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [in] */ Float spacingMult,
    /* [in] */ Float spacingAdd)
{
    if (width < 0) {
        Logger::E("Layout", "Layout: %d < 0", width);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Ensure paint doesn't have baselineShift set.
    // While normally we don't modify the paint the user passed in,
    // we were already doing this in Styled.drawUniformRun with both
    // baselineShift and bgColor.  We probably should reevaluate bgColor.
    if (paint != NULL) {
        paint->SetBgColor(0);
        paint->SetBaselineShift(0);
    }

    mText = text;
    mPaint = paint;
    mWidth = width;
    mAlignment = align;
    mSpacingMult = spacingMult;
    mSpacingAdd = spacingAdd;
    mTextDir = textDir;

    CTextPaint::New((ITextPaint**)&mWorkPaint);
    mSpannedText = ISpanned::Probe(text) != NULL;
    return NOERROR;
}

ECode Layout::ReplaceWith(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd)
{
    assert(width >= 0 && "Layout width < 0");
    if (width < 0) {
        Logger::E("Layout", "Layout: %d < 0", width);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mText = text;
    mPaint = paint;
    mWidth = width;
    mAlignment = align;
    mSpacingMult = spacingmult;
    mSpacingAdd = spacingadd;
    mSpannedText = ISpanned::Probe(text) != NULL;

    return NOERROR;
}

ECode Layout::Draw(
    /* [in] */ ICanvas* c)
{
    return Draw(c, NULL, NULL, 0);
}

ECode Layout::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPath* highlight,
    /* [in] */ IPaint* highlightPaint,
    /* [in] */ Int32 cursorOffsetVertical)
{
    Int64 lineRange;
    GetLineRangeForDraw(canvas, &lineRange);
    Int32 firstLine = TextUtils::UnpackRangeStartFromInt64(lineRange);
    Int32 lastLine = TextUtils::UnpackRangeEndFromInt64(lineRange);
    if (lastLine < 0) return NOERROR;

    DrawBackground(canvas, highlight, highlightPaint, cursorOffsetVertical,
            firstLine, lastLine);
    return DrawText(canvas, firstLine, lastLine);
}

ECode Layout::DrawText(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 firstLine,
    /* [in] */ Int32 lastLine)
{
    Int32 previousLineBottom, previousLineEnd;
    GetLineTop(firstLine, &previousLineBottom);
    GetLineStart(firstLine, &previousLineEnd);
    AutoPtr< ArrayOf<IInterface*> > spans = NO_PARA_SPANS;
    Int32 spanEnd = 0;
    AutoPtr<ITextPaint> paint = mPaint;
    IPaint* p = IPaint::Probe(mPaint);
    AutoPtr<ICharSequence> buf = mText;

    LayoutAlignment paraAlign = mAlignment;
    AutoPtr<TabStops> tabStops;
    Boolean tabStopsIsInitialized = FALSE;

    AutoPtr<TextLine> tl = TextLine::Obtain();

    // Draw the lines, one at a time.
    // The baseline is the top of the following line minus the current line's descent.
    for (Int32 i = firstLine; i <= lastLine; i++) {
        Int32 start = previousLineEnd;
        GetLineStart(i + 1, &previousLineEnd);
        Int32 end = GetLineVisibleEnd(i, start, previousLineEnd);

        Int32 ltop = previousLineBottom;
        Int32 lbottom, lineDescent;
        GetLineTop(i + 1, &lbottom);
        previousLineBottom = lbottom;
        GetLineDescent(i, &lineDescent);
        Int32 lbaseline = lbottom - lineDescent;

        Int32 dir;
        GetParagraphDirection(i, &dir);
        Int32 left = 0;
        Int32 right = mWidth;

        if (mSpannedText) {
            AutoPtr<ISpanned> sp = ISpanned::Probe(buf);
            Int32 textLength = 0;
            buf->GetLength(&textLength);
            Char32 ch;
            Boolean isFirstParaLine = (start == 0 || (buf->GetCharAt(start - 1, &ch), ch) == '\n');

            // New batch of paragraph styles, collect into spans array.
            // Compute the alignment, last alignment style wins.
            // Reset tabStops, we'll rebuild if we encounter a line with
            // tabs.
            // We expect paragraph spans to be relatively infrequent, use
            // spanEnd so that we can check less frequently.  Since
            // paragraph styles ought to apply to entire paragraphs, we can
            // just collect the ones present at the start of the paragraph.
            // If spanEnd is before the end of the paragraph, that's not
            // our problem.
            if (start >= spanEnd && (i == firstLine || isFirstParaLine)) {
                sp->NextSpanTransition(start, textLength, EIID_IParagraphStyle, &spanEnd);
                spans = GetParagraphSpans(sp, start, spanEnd, EIID_IParagraphStyle);

                paraAlign = mAlignment;
                for (Int32 n = spans->GetLength() - 1; n >= 0; n--) {
                    if ((*spans)[n] != NULL && (*spans)[n]->Probe(EIID_IAlignmentSpan)) {
                        IAlignmentSpan* as = (IAlignmentSpan*)((*spans)[n]->Probe(EIID_IAlignmentSpan));
                        as->GetAlignment(&paraAlign);
                        break;
                    }
                }

                tabStopsIsInitialized = FALSE;
            }

            // Draw all leading margin spans.  Adjust left or right according
            // to the paragraph direction of the line.
            Int32 length = spans.Get()->GetLength();
            // for (Int32 n = 0; n < length; n++) {
            //     if ((*spans)[n] != NULL && (*spans)[n]->Probe(EIID_ILeadingMarginSpan)) {
            //         AutoPtr<ILeadingMarginSpan> margin = (ILeadingMarginSpan*)((*spans)[n]->Probe(EIID_ILeadingMarginSpan));
            //         Boolean useFirstLineMargin = isFirstParaLine;
            //         if (margin != NULL && margin->Probe(EIID_ILeadingMarginSpan2)) {
            //             ILeadingMarginSpan2* margin2 = (ILeadingMarginSpan2*)(margin->Probe(EIID_ILeadingMarginSpan2));
            //             Int32 count;
            //             margin2->GetLeadingMarginLineCount(&count);
            //             Int32 tmp;
            //             sp->GetSpanStart(margin, &tmp);
            //             Int32 startLine = GetLineForOffset(tmp);
            //             useFirstLineMargin = i < startLine + count;
            //         }

            Boolean useFirstLineMargin = isFirstParaLine;
            for (Int32 n = 0; n < length; n++) {
                ILeadingMarginSpan2* lms2 = ILeadingMarginSpan2::Probe((*spans)[n]);
                if (lms2 != NULL) {
                    Int32 count, tmp;
                    lms2->GetLeadingMarginLineCount(&count);
                    sp->GetSpanStart((*spans)[n], &tmp);
                    Int32 startLine;
                    GetLineForOffset(tmp, &startLine);
                    // if there is more than one LeadingMarginSpan2, use
                    // the count that is greatest
                    if (i < startLine + count) {
                        useFirstLineMargin = true;
                        break;
                    }
                }
            }
            for (Int32 n = 0; n < length; n++) {
                ILeadingMarginSpan* margin = ILeadingMarginSpan::Probe((*spans)[n]);
                if (margin != NULL) {
                    if (dir == ILayout::DIR_RIGHT_TO_LEFT) {
                        margin->DrawLeadingMargin(canvas, p, right, dir, ltop,
                            lbaseline, lbottom, buf,
                            start, end, isFirstParaLine, this);

                        Int32 tmp;
                        margin->GetLeadingMargin(useFirstLineMargin, &tmp);
                        right -= tmp;
                    }
                    else {
                        margin->DrawLeadingMargin(canvas, p, left, dir, ltop,
                            lbaseline, lbottom, buf,
                            start, end, isFirstParaLine, this);
                        Int32 tmp;
                        margin->GetLeadingMargin(useFirstLineMargin, &tmp);
                        left += tmp;
                    }
                }
            }
        }

        Boolean hasTabOrEmoji;
        GetLineContainsTab(i, &hasTabOrEmoji);
        // Can't tell if we have tabs for sure, currently
        if (hasTabOrEmoji && !tabStopsIsInitialized) {
            if (tabStops == NULL) {
                tabStops = new TabStops(TAB_INCREMENT, (ArrayOf<IInterface*>*)(spans.Get()));
            } else {
                tabStops->Reset(TAB_INCREMENT, (ArrayOf<IInterface*>*)(spans.Get()));
            }
            tabStopsIsInitialized = TRUE;
        }

        // Determine whether the line aligns to normal, opposite, or center.
        LayoutAlignment align = paraAlign;
        if (align == ALIGN_LEFT) {
            align = (dir == ILayout::DIR_LEFT_TO_RIGHT) ?
                ALIGN_NORMAL : ALIGN_OPPOSITE;
        } else if (align == ALIGN_RIGHT) {
            align = (dir == ILayout::DIR_LEFT_TO_RIGHT) ?
                ALIGN_OPPOSITE : ALIGN_NORMAL;
        }

        Int32 x;
        if (align == ALIGN_NORMAL) {
            if (dir == ILayout::DIR_LEFT_TO_RIGHT) {
                x = left;
            } else {
                x = right;
            }
        } else {
            Int32 max = (Int32)GetLineExtent(i, tabStops, FALSE);
            if (align == ALIGN_OPPOSITE) {
                if (dir == ILayout::DIR_LEFT_TO_RIGHT) {
                    x = right - max;
                } else {
                    x = left - max;
                }
            } else { // Alignment.ALIGN_CENTER
                max = max & ~1;
                x = (right + left - max) >> 1;
            }
        }

        AutoPtr<ILayoutDirections> directions;
        GetLineDirections(i, (ILayoutDirections**)&directions);
        if (directions == DIRS_ALL_LEFT_TO_RIGHT && !mSpannedText && !hasTabOrEmoji) {
            // XXX: assumes there's nothing additional to be done
            canvas->DrawText(buf, start, end, x, lbaseline, p);
        }
        else {
            tl->Set(paint, buf, start, end, dir, directions, hasTabOrEmoji, tabStops);
            tl->Draw(canvas, x, ltop, lbaseline, lbottom);
        }
    }
    TextLine::Recycle(tl);

    return NOERROR;
}

/**
 * @hide
 */
ECode Layout::DrawBackground(
    /* [in] */ ICanvas* canvas,
    /* [in] */ IPath* highlight,
    /* [in] */ IPaint* highlightPaint,
    /* [in] */ Int32 cursorOffsetVertical,
    /* [in] */ Int32 firstLine,
    /* [in] */ Int32 lastLine)
{
    // First, draw LineBackgroundSpans.
    // LineBackgroundSpans know nothing about the alignment, margins, or
    // direction of the layout or line.  XXX: Should they?
    // They are evaluated at each line.
    if (mSpannedText) {
        if (mLineBackgroundSpans == NULL) {
            mLineBackgroundSpans = new SpanSet<ILineBackgroundSpan>(EIID_ILineBackgroundSpan);
        }

        AutoPtr<ISpanned> buffer = ISpanned::Probe(mText);
        ICharSequence* csq = ICharSequence::Probe(buffer);
        Int32 textLength;
        csq->GetLength(&textLength);
        mLineBackgroundSpans->Init(buffer, 0, textLength);

        if (mLineBackgroundSpans->mNumberOfSpans > 0) {
            Int32 previousLineBottom, previousLineEnd;
            GetLineTop(firstLine, &previousLineBottom);
            GetLineStart(firstLine, &previousLineEnd);
            AutoPtr< ArrayOf<IInterface*> > spans = NO_PARA_SPANS;
            Int32 spansLength = 0;
            AutoPtr<ITextPaint> paint = mPaint;
            IPaint* p = IPaint::Probe(mPaint);
            Int32 spanEnd = 0;
            Int32 width = mWidth;
            for (Int32 i = firstLine; i <= lastLine; i++) {
                Int32 start = previousLineEnd;
                Int32 end;
                GetLineStart(i + 1, &end);
                previousLineEnd = end;

                Int32 ltop = previousLineBottom;
                Int32 lbottom, descent;
                GetLineTop(i + 1, &lbottom);
                previousLineBottom = lbottom;
                GetLineDescent(i, &descent);
                Int32 lbaseline = lbottom - descent;

                if (start >= spanEnd) {
                    // These should be infrequent, so we'll use this so that
                    // we don't have to check as often.
                    spanEnd = mLineBackgroundSpans->GetNextTransition(start, textLength);
                    // All LineBackgroundSpans on a line contribute to its background.
                    spansLength = 0;
                    // Duplication of the logic of getParagraphSpans
                    if (start != end || start == 0) {
                        // Equivalent to a getSpans(start, end), but filling the 'spans' local
                        // array instead to reduce memory allocation
                        AutoPtr< ArrayOf<Int32> > spanStarts = mLineBackgroundSpans->mSpanStarts;
                        AutoPtr< ArrayOf<Int32> > spanEnds = mLineBackgroundSpans->mSpanEnds;
                        AutoPtr< ArrayOf<ILineBackgroundSpan*> > lbSpans = mLineBackgroundSpans->mSpans;
                        for (Int32 j = 0; j < mLineBackgroundSpans->mNumberOfSpans; j++) {
                            // equal test is valid since both intervals are not empty by
                            // construction
                            if ((*spanStarts)[j] >= end || (*spanEnds)[j] <= start) continue;
                            spans = GrowingArrayUtils::Append(spans, spansLength, (IInterface*)(*lbSpans)[i]);
                            spansLength++;
                        }
                    }
                }

                for (Int32 n = 0; n < spansLength; n++) {
                    AutoPtr<ILineBackgroundSpan> lineBackgroundSpan
                            = ILineBackgroundSpan::Probe((*spans)[n]);
                    lineBackgroundSpan->DrawBackground(canvas, p, 0, width,
                            ltop, lbaseline, lbottom, csq, start, end, i);
                }
            }
        }
        mLineBackgroundSpans->Recycle();
    }

    // There can be a highlight even without spans if we are drawing
    // a non-spanned transformation of a spanned editing buffer.
    if (highlight != NULL) {
        if (cursorOffsetVertical != 0) canvas->Translate(0, cursorOffsetVertical);
        canvas->DrawPath(highlight, highlightPaint);
        if (cursorOffsetVertical != 0) canvas->Translate(0, -cursorOffsetVertical);
    }

    return NOERROR;
}

ECode Layout::GetLineRangeForDraw(
    /* [in] */ ICanvas* canvas,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    VALIDATE_NOT_NULL(canvas)

    Int32 dtop, dbottom;

    {
        AutoLock lock(sTempRectLock);
        Boolean flag = FALSE;
        canvas->GetClipBounds(sTempRect, &flag);
        if (!flag) {
            // Negative range end used as a special flag
            return TextUtils::PackRangeInInt64(0, -1);
        }

        sTempRect->GetTop(&dtop);
        sTempRect->GetBottom(&dbottom);
    }

    Int32 top = Elastos::Core::Math::Max(dtop, 0);
    Int32 count, tt;
    GetLineCount(&count);
    GetLineTop(count, &tt);
    Int32 bottom = Elastos::Core::Math::Min(tt, dbottom);

    if (top >= bottom) return TextUtils::PackRangeInInt64(0, -1);
    Int32 tv, bv;
    GetLineForVertical(top, &tv);
    GetLineForVertical(bottom, &bv);
    *result = TextUtils::PackRangeInInt64(tv, bv);
    return NOERROR;
}

Int32 Layout::GetLineStartPos(
    /* [in] */ Int32 line,
    /* [in] */ Int32 left,
    /* [in] */ Int32 right)
{
    // Adjust the point at which to start rendering depending on the
    // alignment of the paragraph.
    LayoutAlignment align;
    GetParagraphAlignment(line, &align);
    Int32 dir;
    GetParagraphDirection(line, &dir);

    if (align == ALIGN_LEFT) {
        align = (dir == ILayout::DIR_LEFT_TO_RIGHT) ? ALIGN_NORMAL : ALIGN_OPPOSITE;
    } else if (align == ALIGN_RIGHT) {
        align = (dir == ILayout::DIR_LEFT_TO_RIGHT) ? ALIGN_OPPOSITE : ALIGN_NORMAL;
    }

    Int32 x;
    if (align == ALIGN_NORMAL) {
        if (dir == ILayout::DIR_LEFT_TO_RIGHT) {
            x = left;
        } else {
            x = right;
        }
    } else {
        AutoPtr<TabStops> tabStops;
        Boolean bval;
        GetLineContainsTab(line, &bval);
        if (mSpannedText && bval) {
            AutoPtr<ISpanned> spanned = ISpanned::Probe(mText);
            Int32 start;
            GetLineStart(line, &start);
            Int32 length;
            Int32 spanEnd;
            ICharSequence::Probe(spanned)->GetLength(&length);
            spanned->NextSpanTransition(start, length, EIID_ITabStopSpan, &spanEnd);
            AutoPtr< ArrayOf<IInterface*> > tabSpans = GetParagraphSpans(
                    spanned, start, spanEnd, EIID_ITabStopSpan);
            if (tabSpans->GetLength() > 0) {
                tabStops = new TabStops(TAB_INCREMENT, tabSpans);
            }
        }

        Int32 max = (Int32)GetLineExtent(line, tabStops, FALSE);
        if (align == ALIGN_OPPOSITE) {
            if (dir == ILayout::DIR_LEFT_TO_RIGHT) {
                x = right - max;
            } else {
                // max is negative here
                x = left - max;
            }
        } else { // Alignment.ALIGN_CENTER
            max = max & ~1;
            x = (left + right - max) >> 1;
        }
    }

    return x;
}

ECode Layout::GetText(
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq)
    *csq = mText;
    REFCOUNT_ADD(*csq)
    return NOERROR;
}

ECode Layout::GetPaint(
    /* [out] */ ITextPaint** paint)
{
    VALIDATE_NOT_NULL(paint)
    *paint = mPaint;
    REFCOUNT_ADD(*paint)
    return NOERROR;
}

ECode Layout::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mWidth;
    return NOERROR;
}

ECode Layout::GetEllipsizedWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mWidth;
    return NOERROR;
}

ECode Layout::IncreaseWidthTo(
    /* [in] */ Int32 wid)
{
   if (wid < mWidth) {
       Logger::E("Layout", "attempted to reduce Layout width");
       return E_RUNTIME_EXCEPTION;
   }

    mWidth = wid;
    return NOERROR;
}

ECode Layout::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    Int32 count;
    GetLineCount(&count);
    return GetLineTop(count, height);
}

ECode Layout::GetAlignment(
    /* [out] */ LayoutAlignment* align)
{
    VALIDATE_NOT_NULL(align)
    *align= mAlignment;
    return NOERROR;
}

ECode Layout::GetSpacingMultiplier(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSpacingMult;
    return NOERROR;
}

ECode Layout::GetSpacingAdd(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSpacingAdd;
    return NOERROR;
}

ECode Layout::GetTextDirectionHeuristic(
    /* [out] */ ITextDirectionHeuristic** dir)
{
    VALIDATE_NOT_NULL(dir)
    *dir = mTextDir;
    REFCOUNT_ADD(*dir)
    return NOERROR;
}

ECode Layout::GetLineBounds(
    /* [in] */ Int32 line,
    /* [in] */ IRect* bounds,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (bounds != NULL) {
        Int32 top;
        bounds->SetLeft(0);
        GetLineTop(line, &top);
        bounds->SetTop(top);
        bounds->SetRight(mWidth);   // ???
        GetLineTop(line + 1, &top);
        bounds->SetBottom(top);
    }

    return GetLineBaseline(line, result);
}

ECode Layout::IsLevelBoundary(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 line;
    GetLineForOffset(offset, &line);
    AutoPtr<ILayoutDirections> dirs;
    GetLineDirections(line, (ILayoutDirections**)&dirs);
    if (dirs == DIRS_ALL_LEFT_TO_RIGHT || dirs == DIRS_ALL_RIGHT_TO_LEFT) {
        return NOERROR;
    }

    AutoPtr< ArrayOf<Int32> > runs;
    dirs->GetDirections((ArrayOf<Int32>**)&runs);
    Int32 lineStart, lineEnd;
    GetLineStart(line, &lineStart);
    GetLineEnd(line, &lineEnd);
    if (offset == lineStart || offset == lineEnd) {
        Int32 dir;
        GetParagraphDirection(line, &dir);
        Int32 paraLevel = dir == 1 ? 0 : 1;
        Int32 runIndex = offset == lineStart ? 0 : runs->GetLength() - 2;
        *result = (((*runs)[runIndex + 1] /*>>>*/>> ILayout::RUN_LEVEL_SHIFT) & ILayout::RUN_LEVEL_MASK) != paraLevel;
        return NOERROR;
    }

    offset -= lineStart;
    for (Int32 i = 0; i < runs->GetLength(); i += 2) {
        if (offset == (*runs)[i]) {
            *result = TRUE;
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode Layout::IsRtlCharAt(
    /* [in] */ Int32 offset,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Int32 line;
    GetLineForOffset(offset, &line);
    AutoPtr<ILayoutDirections> dirs;
    GetLineDirections(line, (ILayoutDirections**)&dirs);
    if (dirs == DIRS_ALL_LEFT_TO_RIGHT) {
        return NOERROR;
    }

    if (dirs == DIRS_ALL_RIGHT_TO_LEFT) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr< ArrayOf<Int32> >runs;
    dirs->GetDirections((ArrayOf<Int32>**)&runs);
    Int32 lineStart;
    GetLineStart(line, &lineStart);
    for (Int32 i = 0; i < runs->GetLength(); i += 2) {
        Int32 start = lineStart + ((*runs)[i] & ILayout::RUN_LENGTH_MASK);
        Int32 limit = start + ((*runs)[i+1] & RUN_LENGTH_MASK);
        if (offset >= start && offset < limit) {
            Int32 level = ((*runs)[i+1] /*>>>*/>> ILayout::RUN_LEVEL_SHIFT) & ILayout::RUN_LEVEL_MASK;
            *result = ((level & 1) != 0);
            return NOERROR;
        }
    }
    // Should happen only if the offset is "out of bounds"
    return NOERROR;
}

Boolean Layout::PrimaryIsTrailingPrevious(
    /* [in] */ Int32 offset)
{
    Int32 line, lineStart, lineEnd;
    GetLineForOffset(offset, &line);
    GetLineStart(line, &lineStart);
    GetLineEnd(line, &lineEnd);
    AutoPtr< ArrayOf<Int32> > runs;
    AutoPtr<ILayoutDirections> dirs;
    GetLineDirections(line, (ILayoutDirections**)&dirs);
    dirs->GetDirections((ArrayOf<Int32>**)&runs);

    Int32 levelAt = -1;
    for (Int32 i = 0; i < runs->GetLength(); i += 2) {
        Int32 start = lineStart + (*runs)[i];
        Int32 limit = start + ((*runs)[i+1] & ILayout::RUN_LENGTH_MASK);
        if (limit > lineEnd) {
            limit = lineEnd;
        }

        if (offset >= start && offset < limit) {
            if (offset > start) {
                // Previous character is at same level, so don't use trailing.
                return FALSE;
            }

            levelAt = ((*runs)[i+1] /*>>>*/>> ILayout::RUN_LEVEL_SHIFT) & ILayout::RUN_LEVEL_MASK;
            break;
        }
    }

    if (levelAt == -1) {
        // Offset was limit of line.
        Int32 pd;
        GetParagraphDirection(line, &pd);
        levelAt = pd == 1 ? 0 : 1;
    }

    // At level boundary, check previous level.
    Int32 levelBefore = -1;
    if (offset == lineStart) {
        Int32 pd;
        GetParagraphDirection(line, &pd);
        levelBefore = pd == 1 ? 0 : 1;
    }
    else {
        offset -= 1;
        for (Int32 i = 0; i < runs->GetLength(); i += 2) {
            Int32 start = lineStart + (*runs)[i];
            Int32 limit = start + ((*runs)[i+1] & ILayout::RUN_LENGTH_MASK);
            if (limit > lineEnd) {
                limit = lineEnd;
            }
            if (offset >= start && offset < limit) {
                levelBefore = ((*runs)[i+1] /*>>>*/>> ILayout::RUN_LEVEL_SHIFT) & ILayout::RUN_LEVEL_MASK;
                break;
            }
        }
    }

    return levelBefore < levelAt;
}

ECode Layout::GetPrimaryHorizontal(
    /* [in] */ Int32 offset,
    /* [out] */ Float* result)
{
    return GetPrimaryHorizontal(offset, FALSE, result);
}

ECode Layout::GetPrimaryHorizontal(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean clamped,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean trailing = PrimaryIsTrailingPrevious(offset);
    *result = GetHorizontal(offset, trailing, clamped);
    return NOERROR;
}

ECode Layout::GetSecondaryHorizontal(
    /* [in] */ Int32 offset,
    /* [out] */ Float* result)
{
    return GetSecondaryHorizontal(offset, FALSE, result);
}

ECode Layout::GetSecondaryHorizontal(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean clamped,
    /* [out] */ Float* result)
{
    Boolean trailing = PrimaryIsTrailingPrevious(offset);
    *result = GetHorizontal(offset, !trailing, clamped);
    return NOERROR;
}

Float Layout::GetHorizontal(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean trailing,
    /* [in] */ Boolean clamped)
{
    Int32 line;
    GetLineForOffset(offset, &line);

    return GetHorizontal(offset, trailing, line, clamped);
}

Float Layout::GetHorizontal(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean trailing,
    /* [in] */ Int32 line,
    /* [in] */ Boolean clamped)
{
    Int32 start, end, dir;
    GetLineStart(line, &start);
    GetLineEnd(line, &end);
    GetParagraphDirection(line, &dir);
    Boolean hasTabOrEmoji;
    GetLineContainsTab(line, &hasTabOrEmoji);
    AutoPtr<ILayoutDirections> directions;
    GetLineDirections(line, (ILayoutDirections**)&directions);

    AutoPtr<TabStops> tabStops;
    if (hasTabOrEmoji && ISpanned::Probe(mText)) {
        // Just checking this line should be good enough, tabs should be
        // consistent across all lines in a paragraph.
        AutoPtr<ISpanned> spanned = ISpanned::Probe(mText);
        AutoPtr< ArrayOf<IInterface*> > tabs = GetParagraphSpans(
                spanned, start, end, EIID_ITabStopSpan);
        if (tabs->GetLength() > 0) {
            tabStops = new TabStops(TAB_INCREMENT, tabs); // XXX should reuse
        }
    }

    AutoPtr<TextLine> tl = TextLine::Obtain();
    tl->Set(mPaint, mText, start, end, dir, directions, hasTabOrEmoji, tabStops);
    Float wid = tl->Measure(offset - start, trailing, NULL);
    TextLine::Recycle(tl);

    if (clamped && wid > mWidth) {
        wid = mWidth;
    }

    Int32 left, right;
    GetParagraphLeft(line, &left);
    GetParagraphRight(line, &right);

    return GetLineStartPos(line, left, right) + wid;
}

ECode Layout::GetLineLeft(
    /* [in] */ Int32 line,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 dir;
    GetParagraphDirection(line, &dir);
    LayoutAlignment align;
    GetParagraphAlignment(line, &align);

    if (align == ALIGN_LEFT) {
        return NOERROR;
    }
    else if (align == ALIGN_NORMAL) {
        if (dir == ILayout::DIR_RIGHT_TO_LEFT) {
            Int32 right;
            GetParagraphRight(line, &right);
            Float fmax;
            GetLineMax(line, &fmax);
            *result = right - fmax;
            return NOERROR;
        }
        else {
            return NOERROR;
        }
    }
    else if (align == ALIGN_RIGHT) {
        Float fmax;
        GetLineMax(line, &fmax);
        return mWidth - fmax;
    }
    else if (align == ALIGN_OPPOSITE) {
        if (dir == ILayout::DIR_RIGHT_TO_LEFT) {
            return NOERROR;
        }
        else {
            Float fmax;
            GetLineMax(line, &fmax);
            return mWidth - fmax;
        }
    }
    else { /* align == Alignment.ALIGN_CENTER */
        Int32 left, right;
        Float fmax;
        GetParagraphLeft(line, &left);
        GetParagraphRight(line, &right);
        GetLineMax(line, &fmax);
        Int32 max = ((Int32)fmax) & ~1;

        *result = left + ((right - left) - max) / 2;
    }

    return NOERROR;
}

ECode Layout::GetLineRight(
    /* [in] */ Int32 line,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 dir;
    GetParagraphDirection(line, &dir);
    LayoutAlignment align;
    GetParagraphAlignment(line, &align);

    Int32 left;
    GetParagraphLeft(line, &left);
    Float max;
    GetLineMax(line, &max);

    if (align == ALIGN_LEFT) {
        *result = left + max;
        return NOERROR;
    }
    else if (align == ALIGN_NORMAL) {
        if (dir == ILayout::DIR_RIGHT_TO_LEFT) {
            return mWidth;
        }
        else {
            *result = left + max;
            return NOERROR;
        }
    }
    else if (align == ALIGN_RIGHT) {
        *result = mWidth;
        return NOERROR;
    }
    else if (align == ALIGN_OPPOSITE) {
        if (dir == ILayout::DIR_RIGHT_TO_LEFT) {
            *result = max;
            return NOERROR;
        }
        else {
            *result = mWidth;
            return NOERROR;
        }
    }
    else { /* align == Alignment.ALIGN_CENTER */
        Int32 right;
        GetParagraphRight(line, &right);
        Int32 imax = ((Int32) max) & ~1;

        *result = right - ((right - left) - imax) / 2;
    }

    return NOERROR;
}

ECode Layout::GetLineMax(
    /* [in] */ Int32 line,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Float margin = GetParagraphLeadingMargin(line);
    Float signedExtent = GetLineExtent(line, FALSE);
    *result = margin + (signedExtent >= 0 ? signedExtent : -signedExtent);
    return NOERROR;
}

ECode Layout::GetLineWidth(
    /* [in] */ Int32 line,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Float margin = GetParagraphLeadingMargin(line);
    Float signedExtent = GetLineExtent(line, TRUE);
    *result = margin + (signedExtent >= 0 ? signedExtent : -signedExtent);
    return NOERROR;
}

Float Layout::GetLineExtent(
    /* [in] */ Int32 line,
    /* [in] */ Boolean full)
{

    Int32 start, end, visibleEnd;
    GetLineStart(line, &start);
    GetLineEnd(line, &end);
    GetLineVisibleEnd(line, &visibleEnd);
    end = full ? end : visibleEnd;

    Boolean hasTabsOrEmoji;
    GetLineContainsTab(line, &hasTabsOrEmoji);
    AutoPtr<TabStops> tabStops;
    if (hasTabsOrEmoji && ISpanned::Probe(mText) != NULL) {
        // Just checking this line should be good enough, tabs should be
        // consistent across all lines in a paragraph.
        AutoPtr<ISpanned> spanned = ISpanned::Probe(mText);
        AutoPtr< ArrayOf<IInterface*> > tabs = GetParagraphSpans(
                spanned, start, end, EIID_ITabStopSpan);
        if (tabs->GetLength() > 0) {
            tabStops = new TabStops(TAB_INCREMENT, tabs); // XXX should reuse
        }
    }

    AutoPtr<ILayoutDirections> directions;
    GetLineDirections(line, (ILayoutDirections**)&directions);
    // Returned directions can actually be null
    if (directions == NULL) {
        return /*0f*/0.0;
    }

    Int32 dir;
    GetParagraphDirection(line, &dir);

    AutoPtr<TextLine> tl = TextLine::Obtain();
    tl->Set(mPaint, mText, start, end, dir, directions, hasTabsOrEmoji, tabStops);
    Float width = tl->Metrics(NULL);
    TextLine::Recycle(tl);

    return width;
}

Float Layout::GetLineExtent(
    /* [in] */ Int32 line,
    /* [in] */ TabStops* tabStops,
    /* [in] */ Boolean full)
{
    Int32 start,  end;
    GetLineStart(line, &start);
    if (full)
        GetLineEnd(line, &end);
    else
        GetLineVisibleEnd(line, &end);
    Boolean hasTabsOrEmoji;
    GetLineContainsTab(line, &hasTabsOrEmoji);
    AutoPtr<ILayoutDirections> directions;
    GetLineDirections(line, (ILayoutDirections**)&directions);
    Int32 dir;
    GetParagraphDirection(line, &dir);

    AutoPtr<TextLine> tl = TextLine::Obtain();
    tl->Set(mPaint, mText, start, end, dir, directions, hasTabsOrEmoji, tabStops);
    Float width = tl->Metrics(NULL);
    TextLine::Recycle(tl);

    return width;
}

ECode Layout::GetLineForVertical(
    /* [in] */ Int32 vertical,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 high;
    GetLineCount(&high);
    Int32 low = -1, guess, top;

    while (high - low > 1) {
        guess = (high + low) / 2;
        GetLineTop(guess, &top);
        if (top > vertical)
            high = guess;
        else
            low = guess;
    }

    if (low < 0)
        return NOERROR;

    *result = low;
    return NOERROR;
}

ECode Layout::GetLineForOffset(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 high;
    GetLineCount(&high);
    Int32 low = -1, guess;

    while (high - low > 1) {
        guess = (high + low) / 2;

        Int32 start;
        GetLineStart(guess, &start);
        if (start > offset) {
            high = guess;
        }
        else {
            low = guess;
        }
    }

    if (low < 0) {
        *result = 0;
    }
    else {
        *result = low;
    }
    return NOERROR;
}

ECode Layout::GetOffsetForHorizontal(
    /* [in] */ Int32 line,
    /* [in] */ Float horiz,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 max, min;
    GetLineEnd(line, &max);
    GetLineStart(line, &min);
    max = max - 1;
    AutoPtr<ILayoutDirections> dirs;
    GetLineDirections(line, (ILayoutDirections**)&dirs);

    Int32 count;
    GetLineCount(&count);
    if (line == count - 1)
        max++;

    Int32 best = min;
    Float ph;
    GetPrimaryHorizontal(best, &ph);
    Float bestdist = Elastos::Core::Math::Abs(ph - horiz);

    AutoPtr< ArrayOf<Int32> > directions;
    dirs->GetDirections((ArrayOf<Int32>**)&directions);
    for (Int32 i = 0; i < directions->GetLength(); i += 2) {
        Int32 here = min + (*directions)[i];
        Int32 there = here + ((*directions)[i+1] & ILayout::RUN_LENGTH_MASK);
        Int32 swap = ((*directions)[i+1] & ILayout::RUN_RTL_FLAG) != 0 ? -1 : 1;

        if (there > max)
            there = max;
        Int32 high = there - 1 + 1, low = here + 1 - 1, guess;

        while (high - low > 1) {
            guess = (high + low) / 2;
            Int32 adguess = GetOffsetAtStartOf(guess);
            GetPrimaryHorizontal(adguess, &ph);
            if (ph * swap >= horiz * swap)
                high = guess;
            else
                low = guess;
        }

        if (low < here + 1)
            low = here + 1;

        if (low < there) {
            low = GetOffsetAtStartOf(low);

            GetPrimaryHorizontal(low, &ph);
            Float dist = Elastos::Core::Math::Abs(ph - horiz);

            Int32 aft = TextUtils::GetOffsetAfter(mText, low);
            if (aft < there) {
                GetPrimaryHorizontal(aft, &ph);
                Float other = Elastos::Core::Math::Abs(ph - horiz);

                if (other < dist) {
                    dist = other;
                    low = aft;
                }
            }

            if (dist < bestdist) {
                bestdist = dist;
                best = low;
            }
        }

        GetPrimaryHorizontal(here, &ph);
        Float dist = Elastos::Core::Math::Abs(ph - horiz);

        if (dist < bestdist) {
            bestdist = dist;
            best = here;
        }
    }

    GetPrimaryHorizontal(max, &ph);
    Float dist = Elastos::Core::Math::Abs(ph - horiz);

    if (dist <= bestdist) {
        bestdist = dist;
        best = max;
    }

    return best;
}

ECode Layout::GetLineEnd(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    return GetLineStart(line + 1, result);
}

ECode Layout::GetLineVisibleEnd(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    Int32 start, startNext;
    GetLineStart(line, &start);
    GetLineStart(line + 1, &startNext);
    *result = GetLineVisibleEnd(line, start, startNext);
    return NOERROR;
}

Int32 Layout::GetLineVisibleEnd(
    /* [in] */ Int32 line,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<ICharSequence> text = mText;
    Int32 count;
    GetLineCount(&count);

    if (line == count - 1) {
        return end;
    }

    Char32 ch;
    for (; end > start; end--) {
        text->GetCharAt(end - 1, &ch);

        if (ch == '\n') {
            return end - 1;
        }

        if (ch != ' ' && ch != '\t') {
            break;
        }

    }

    return end;
}

ECode Layout::GetLineBottom(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    return GetLineTop(line + 1, result);
}

ECode Layout::GetLineBaseline(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // getLineTop(line+1) == getLineTop(line)
    Int32 top, descent;
    GetLineTop(line + 1, &top);
    GetLineDescent(line, &descent);
    *result = top - descent;
    return NOERROR;
}

ECode Layout::GetLineAscent(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // getLineTop(line+1) - getLineDescent(line) == getLineBaseLine(line)
    Int32 top, topNext, descent;
    GetLineTop(line, &top);
    GetLineTop(line + 1, &topNext);
    GetLineDescent(line, &descent);
    return top - (topNext - descent);
}

ECode Layout::GetOffsetToLeftOf(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetOffsetToLeftRightOf(offset, TRUE);
    return NOERROR;
}

ECode Layout::GetOffsetToRightOf(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetOffsetToLeftRightOf(offset, FALSE);
    return NOERROR;
}

Int32 Layout::GetOffsetToLeftRightOf(
    /* [in] */ Int32 caret,
    /* [in] */ Boolean toLeft)
{
    Int32 line, lineStart, lineEnd, lineDir;
    GetLineForOffset(caret, &line);
    GetLineStart(line, &lineStart);
    GetLineEnd(line, &lineEnd);
    GetParagraphDirection(line, &lineDir);

    Int32 count;
    Boolean lineChanged = FALSE;
    Boolean advance = toLeft == (lineDir == ILayout::DIR_RIGHT_TO_LEFT);
    // if walking off line, look at the line we're headed to
    if (advance) {
        if (caret == lineEnd) {
            GetLineCount(&count);
            if (line < count - 1) {
                lineChanged = TRUE;
                ++line;
            } else {
                return caret; // at very end, don't move
            }
        }
    } else {
        if (caret == lineStart) {
            if (line > 0) {
                lineChanged = TRUE;
                --line;
            } else {
                return caret; // at very start, don't move
            }
        }
    }

    if (lineChanged) {
        GetLineStart(line, &lineStart);
        GetLineEnd(line, &lineEnd);
        Int32 newDir;
        GetParagraphDirection(line, &newDir);
        if (newDir != lineDir) {
            // unusual case.  we want to walk onto the line, but it runs
            // in a different direction than this one, so we fake movement
            // in the opposite direction.
            toLeft = !toLeft;
            lineDir = newDir;
        }
    }

    AutoPtr<ILayoutDirections> directions;
    GetLineDirections(line, (ILayoutDirections**)&directions);

    AutoPtr<TextLine> tl = TextLine::Obtain();
    // XXX: we don't care about tabs
    tl->Set(mPaint, mText, lineStart, lineEnd, lineDir, directions, FALSE, NULL);
    caret = lineStart + tl->GetOffsetToLeftRightOf(caret - lineStart, toLeft);
    tl = TextLine::Recycle(tl);

    return caret;
}

Int32 Layout::GetOffsetAtStartOf(
    /* [in] */ Int32 offset)
{
    // XXX this probably should skip local reorderings and
    // zero-width characters, look at callers
    if (offset == 0)
        return 0;

    AutoPtr<ICharSequence> text = mText;
    Char32 c;
    text->GetCharAt(offset, &c);

    if (c >= 0xDC00/*'\uDC00'*/ && c <= 0xDFFF/*'\uDFFF'*/) {
        Char32 c1;
        text->GetCharAt(offset - 1, &c1);

        if (c1 >= 0xD800/*'\uD800'*/ && c1 <= 0xDBFF/*'\uDBFF'*/)
           offset -= 1;
    }

    if (mSpannedText) {
        AutoPtr<ISpanned> spanned = ISpanned::Probe(text);

        AutoPtr< ArrayOf<IInterface*> > spans;
        spanned->GetSpans(offset, offset, EIID_IReplacementSpan, (ArrayOf<IInterface*>**)&spans);

        Int32 start;
        Int32 end;
        for (Int32 i = 0; i < spans->GetLength(); i++) {
            spanned->GetSpanStart((*spans)[i], &start);
            spanned->GetSpanEnd((*spans)[i], &end);

            if (start < offset && end > offset)
                offset = start;
        }
    }

    return offset;
}

ECode Layout::ShouldClampCursor(
    /* [in] */ Int32 line,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // Only clamp cursor position in left-aligned displays.
    LayoutAlignment align;
    GetParagraphAlignment(line, &align);
    switch (align) {
        case Elastos::Droid::Text::ALIGN_LEFT: {
            *result = TRUE;
            return NOERROR;
        }
        case Elastos::Droid::Text::ALIGN_NORMAL: {
            Int32 dir;
            GetParagraphDirection(line, &dir);
            *result = dir > 0;
            return NOERROR;
        }
        default:
            return NOERROR;
    }

    return NOERROR;
}

ECode Layout::GetCursorPath(
    /* [in] */ Int32 point,
    /* [in] */ IPath* dest,
    /* [in] */ ICharSequence* editingBuffer)
{
    dest->Reset();

    Int32 line, top, bottom;
    GetLineForOffset(point, &line);
    GetLineTop(line, &top);
    GetLineTop(line + 1, &bottom);

    Boolean clamped;
    ShouldClampCursor(line, &clamped);
    Float horiz;
    GetPrimaryHorizontal(point, clamped, &horiz);
    Float h1 = horiz - 0.5f;
    Boolean bval;
    IsLevelBoundary(point, &bval);
    Float h2 = bval ? (GetSecondaryHorizontal(point, clamped, &horiz), horiz - 0.5f) : h1;

    Int32 caps = 0, fn = 0, dist = 0;
    Int32 data1, data2;
    caps = (MetaKeyKeyListener::GetMetaState(editingBuffer, IMetaKeyKeyListener::META_SHIFT_ON, &data1), data1)
        | (MetaKeyKeyListener::GetMetaState(editingBuffer, IMetaKeyKeyListener::META_SELECTING, &data2), data2);
    MetaKeyKeyListener::GetMetaState(editingBuffer, IMetaKeyKeyListener::META_ALT_ON, &fn);
    dist = 0;

    if (caps != 0 || fn != 0) {
        dist = (bottom - top) >> 2;

        if (fn != 0) {
            top += dist;
        }
        if (caps != 0) {
            bottom -= dist;
        }
    }

    if (h1 < 0.5f) {
        h1 = 0.5f;
    }
    if (h2 < 0.5f) {
        h2 = 0.5f;
    }

    if (Elastos::Core::Math::Compare(h1, h2) == 0) {
        dest->MoveTo(h1, top);
        dest->LineTo(h1, bottom);
    }
    else {
        dest->MoveTo(h1, top);
        dest->LineTo(h1, (top + bottom) >> 1);

        dest->MoveTo(h2, (top + bottom) >> 1);
        dest->LineTo(h2, bottom);
    }

    if (caps == 2) {
        dest->MoveTo(h2, bottom);
        dest->LineTo(h2 - dist, bottom + dist);
        dest->LineTo(h2, bottom);
        dest->LineTo(h2 + dist, bottom + dist);
    }
    else if (caps == 1) {
        dest->MoveTo(h2, bottom);
        dest->LineTo(h2 - dist, bottom + dist);

        dest->MoveTo(h2 - dist, bottom + dist - 0.5f);
        dest->LineTo(h2 + dist, bottom + dist - 0.5f);

        dest->MoveTo(h2 + dist, bottom + dist);
        dest->LineTo(h2, bottom);
    }

    if (fn == 2) {
        dest->MoveTo(h1, top);
        dest->LineTo(h1 - dist, top - dist);
        dest->LineTo(h1, top);
        dest->LineTo(h1 + dist, top - dist);
    }
    else if (fn == 1) {
        dest->MoveTo(h1, top);
        dest->LineTo(h1 - dist, top - dist);

        dest->MoveTo(h1 - dist, top - dist + 0.5f);
        dest->LineTo(h1 + dist, top - dist + 0.5f);

        dest->MoveTo(h1 + dist, top - dist);
        dest->LineTo(h1, top);
    }

    return NOERROR;
}

void Layout::AddSelection(
    /* [in] */ Int32 line,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 top,
    /* [in] */ Int32 bottom,
    /* [in] */ IPath* dest)
{
    Int32 lineStart, lineEnd;
    GetLineStart(line, &lineStart);
    GetLineEnd(line, &lineEnd);
    AutoPtr<ILayoutDirections> dirs;
    GetLineDirections(line, (ILayoutDirections**)&dirs);

    Char32 ch;
    if (lineEnd > lineStart && (mText->GetCharAt(lineEnd - 1, &ch), ch) == '\n')
        lineEnd--;

    AutoPtr< ArrayOf<Int32> > directions = NULL;
    dirs->GetDirections((ArrayOf<Int32>**)&directions);
    for (Int32 i = 0; i < directions->GetLength(); i += 2) {
        Int32 here = lineStart + (*directions)[i];
        Int32 there = here + ((*directions)[i+1] & ILayout::RUN_LENGTH_MASK);

        if (there > lineEnd)
            there = lineEnd;

        if (start <= there && end >= here) {
            Int32 st = Elastos::Core::Math::Max(start, here);
            Int32 en = Elastos::Core::Math::Min(end, there);

            if (st != en) {
                Float h1 = GetHorizontal(st, FALSE, line, FALSE /* not clamped */);
                Float h2 = GetHorizontal(en, TRUE, line, FALSE /* not clamped */);

                Float left = Elastos::Core::Math::Min(h1, h2);
                Float right = Elastos::Core::Math::Max(h1, h2);

                dest->AddRect(left, top, right, bottom, PathDirection_CW);
            }
        }
    }
}

ECode Layout::GetSelectionPath(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ IPath* dest)
{
    dest->Reset();

    if (start == end)
        return NOERROR;

    if (end < start) {
        Int32 temp = end;
        end = start;
        start = temp;
    }

    Int32 startline, endline, top, bottom;
    GetLineForOffset(start, &startline);
    GetLineForOffset(end, &endline);
    GetLineTop(startline, &top);
    GetLineBottom(endline, &bottom);

    if (startline == endline) {
        AddSelection(startline, start, end, top, bottom, dest);
    } else {
        Float width = mWidth;

        Int32 lb, le;
        GetLineEnd(startline, &le);
        GetLineEnd(startline, &lb);
        AddSelection(startline, start, le, top, lb, dest);

        Int32 dir;
        GetParagraphDirection(startline, &dir);
        if (dir == ILayout::DIR_RIGHT_TO_LEFT) {
            Float ll;
            GetLineLeft(startline, &ll);
            dest->AddRect(ll, top, 0, lb, PathDirection_CW);
        }
        else {
            Float lr;
            GetLineRight(startline, &lr);
            dest->AddRect(lr, top, width, lb, PathDirection_CW);
        }

        for (Int32 i = startline + 1; i < endline; i++) {
            GetLineTop(i, &top);
            GetLineBottom(i, &bottom);
            dest->AddRect(0, top, width, bottom, PathDirection_CW);
        }

        GetLineTop(endline, &top);
        GetLineBottom(endline, &bottom);

        Int32 ls;
        GetLineStart(endline, &ls);
        AddSelection(endline, ls, end, top, bottom, dest);

        GetParagraphDirection(endline, &dir);
        if (dir == ILayout::DIR_RIGHT_TO_LEFT) {
            Float lr;
            GetLineRight(endline, &lr);
            dest->AddRect(width, top, lr, bottom, PathDirection_CW);
        }
        else {
            Float ll;
            GetLineLeft(endline, &ll);
            dest->AddRect(0, top, ll, bottom, PathDirection_CW);
        }
    }

    return NOERROR;
}

ECode Layout::GetParagraphAlignment(
    /* [in] */ Int32 line,
    /* [out] */ LayoutAlignment* result)
{
    VALIDATE_NOT_NULL(result)

    LayoutAlignment align = mAlignment;

    if (mSpannedText) {
        AutoPtr<ISpanned> spanned = ISpanned::Probe(mText);
        if (spanned) {
            Int32 s, e;
            GetLineStart(line, &s);
            GetLineEnd(line, &e);
            AutoPtr< ArrayOf<IInterface*> > spans = GetParagraphSpans(spanned, s, e, EIID_IAlignmentSpan);
            Int32 spanLength = spans->GetLength();
            if (spanLength > 0) {
                IAlignmentSpan* as = IAlignmentSpan::Probe((*spans)[spanLength-1]);
                as->GetAlignment(&align);
            }
        }
    }

    *result = align;
    return NOERROR;
}

ECode Layout::GetParagraphLeft(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 left = 0;
    Int32 dir;
    GetParagraphDirection(line, &dir);
    if (dir == ILayout::DIR_RIGHT_TO_LEFT || !mSpannedText) {
        *result = left; // leading margin has no impact, or no styles
        return NOERROR;
    }

    *result = GetParagraphLeadingMargin(line);
    return NOERROR;
}

ECode Layout::GetParagraphRight(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 right = mWidth;
    Int32 dir;
    GetParagraphDirection(line, &dir);
    if (dir == ILayout::DIR_LEFT_TO_RIGHT || !mSpannedText) {
        *result = right; // leading margin has no impact, or no styles
        return NOERROR;
    }

    *result = right - GetParagraphLeadingMargin(line);
    return NOERROR;
}

Int32 Layout::GetParagraphLeadingMargin(
    /* [in] */ Int32 line)
{
    if (!mSpannedText) {
        return 0;
    }

    AutoPtr<ISpanned> spanned = ISpanned::Probe(mText);

    Int32 lineStart, lineEnd;
    GetLineStart(line, &lineStart);
    GetLineEnd(line, &lineEnd);
    Int32 spanEnd;
    spanned->NextSpanTransition(lineStart, lineEnd, EIID_ILeadingMarginSpan, &spanEnd);
    AutoPtr< ArrayOf<IInterface*> > spans = GetParagraphSpans(spanned, lineStart, spanEnd,
                                            EIID_ILeadingMarginSpan);
    if (spans->GetLength() == 0) {
        return 0; // no leading margin span;
    }

    Int32 margin = 0;

    Char32 ch;
    Boolean isFirstParaLine = (lineStart == 0)
        || (ICharSequence::Probe(spanned)->GetCharAt(lineStart - 1, &ch), ch) == '\n';
    Boolean useFirstLineMargin = isFirstParaLine;

    Int32 length = spans->GetLength();
    for (Int32 i = 0; i < length; i++) {
        ILeadingMarginSpan2* lms2 = ILeadingMarginSpan2::Probe((*spans)[i]);
        if (lms2 != NULL) {
            Int32 spStart, spanLine, count;
            spanned->GetSpanStart((*spans)[i], &spStart);
            GetLineForOffset(spStart, &spanLine);
            lms2->GetLeadingMarginLineCount(&count);
            // if there is more than one LeadingMarginSpan2, use the count that is greatest
            useFirstLineMargin |= line < spanLine + count;
        }
    }
    for (Int32 i = 0; i < length; i++) {
        ILeadingMarginSpan* span = ILeadingMarginSpan::Probe((*spans)[i]);

        Int32 tmp = 0;
        span->GetLeadingMargin(useFirstLineMargin, &tmp);
        margin += tmp;
    }

    return /*margin*/0;
}

Float Layout::MeasurePara(
    /* [in] */ ITextPaint* paint,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<MeasuredText> mt = MeasuredText::Obtain();
    AutoPtr<TextLine> tl = TextLine::Obtain();
    //try {
    AutoPtr<ITextDirectionHeuristic> ltr = TextDirectionHeuristics::LTR;
        mt->SetPara(text, start, end, ltr);
        AutoPtr<ILayoutDirections> directions;
        Int32 dir;
        if (mt->mEasy) {
            directions = DIRS_ALL_LEFT_TO_RIGHT;
            dir = ILayout::DIR_LEFT_TO_RIGHT;
        } else {
            directions = AndroidBidi::Directions(mt->mDir, mt->mLevels,
                0, mt->mChars, 0, mt->mLen);
            dir = mt->mDir;
        }

        AutoPtr< ArrayOf<Char32> > chars = mt->mChars;
        Int32 len = mt->mLen;
        Boolean hasTabs = FALSE;
        AutoPtr<TabStops> tabStops;

        // leading margins should be taken into account when measuring a paragraph
        Int32 margin = 0;
        ISpanned* spanned = ISpanned::Probe(text);
        if (spanned != NULL) {
            AutoPtr< ArrayOf<IInterface*> > spans = GetParagraphSpans(spanned, start, end, EIID_ILeadingMarginSpan);
            if (spans != NULL) {
                for (Int32 i = 0; i < spans->GetLength(); ++i) {
                    ILeadingMarginSpan* lms = ILeadingMarginSpan::Probe((*spans)[i]);
                    Int32 tmp = 0;
                    lms->GetLeadingMargin(TRUE, &tmp);
                    margin += tmp;
                }
            }
        }

        for (Int32 i = 0; i < len; ++i) {
            if ((*chars)[i] == '\t') {
                hasTabs = TRUE;
                if (text != NULL && text->Probe(EIID_ISpanned)) {
                    AutoPtr<ISpanned> spanned = (ISpanned*)(text->Probe(EIID_ISpanned));
                    Int32 spanEnd;
                    spanned->NextSpanTransition(start, end,
                            EIID_ITabStopSpan, &spanEnd);
                    AutoPtr< ArrayOf<IInterface*> > spans = GetParagraphSpans(spanned, start, spanEnd,
                            EIID_ITabStopSpan);
                    if (spans->GetLength() > 0) {
                        tabStops = new TabStops(TAB_INCREMENT, spans);
                    }
                }
                break;
            }
        }

        tl->Set(paint, text, start, end, dir, directions, hasTabs, tabStops);
        Float result =  margin + tl->Metrics(NULL);
    //} finally {
        TextLine::Recycle(tl);
        MeasuredText::Recycle(mt);
    //}

    return result;
}

Float Layout::NextTab(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Float h,
    /* [in] */ ArrayOf<IInterface*>* _tabs)
{
    Float nh = Elastos::Core::Math::FLOAT_MAX_VALUE;
    Boolean alltabs = FALSE;

    AutoPtr< ArrayOf<IInterface*> > tabs = _tabs;
    if (text != NULL && text->Probe(EIID_ISpanned)) {
        if (tabs == NULL) {
            AutoPtr<ISpanned> spanned = (ISpanned*)(text->Probe(EIID_ISpanned));
            tabs = GetParagraphSpans(spanned, start, end, EIID_ITabStopSpan);
            alltabs = TRUE;
        }

        Int32 length = tabs->GetLength();
        for (Int32 i = 0; i < length; i++) {
            if (!alltabs) {
                if ((*tabs)[i] == NULL || (*tabs)[i]->Probe(EIID_ITabStopSpan) == NULL)
                    continue;
            }

            Int32 where;
            ITabStopSpan* tss = (ITabStopSpan*)((*tabs)[i]->Probe(EIID_ITabStopSpan));
            tss->GetTabStop(&where);

            if (where < nh && where > h)
                nh = where;
        }

        if (nh != Elastos::Core::Math::FLOAT_MAX_VALUE)
            return nh;
    }

    return ((Int32) ((h + TAB_INCREMENT) / TAB_INCREMENT)) * TAB_INCREMENT;
}

Boolean Layout::IsSpanned()
{
    return mSpannedText;
}

AutoPtr< ArrayOf<IInterface*> > Layout::GetParagraphSpans(
    /* [in] */ ISpanned* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& type)
{
    AutoPtr< ArrayOf<IInterface*> > tmp;
    if (start == end && start > 0) {
        tmp = ArrayOf<IInterface*>::Alloc(0);
        return tmp;
//        return ArrayUtils::EmptyArray(type);
    }

    text->GetSpans(start, end, type, (ArrayOf<IInterface*>**)&tmp);
    return tmp;
}

Char32 Layout::GetEllipsisChar(
    /* [in] */ TextUtilsTruncateAt method)
{
    return (method == TextUtilsTruncateAt_END_SMALL) ?
            ELLIPSIS_TWO_DOTS[0] :
            ELLIPSIS_NORMAL[0];
}

void Layout::Ellipsize(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 line,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff,
    /* [in] */ TextUtilsTruncateAt method)
{
    Int32 ellipsisCount;
    GetEllipsisCount(line, &ellipsisCount);
    if (ellipsisCount == 0) {
        return;
    }

    Int32 ellipsisStart, lineStart;
    GetEllipsisStart(line, &ellipsisStart);
    GetLineStart(line, &lineStart);

    Char32 c;
    for (Int32 i = ellipsisStart; i < ellipsisStart + ellipsisCount; i++) {
        if (i == ellipsisStart) {
            c = GetEllipsisChar(method); // ellipsis
        }
        else {
            c = 0xFEFF;//'\uFEFF'; // 0-width space
        }

        Int32 a = i + lineStart;
        if (a >= start && a < end) {
            (*dest)[destoff + a - start] = c;
        }
    }
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
