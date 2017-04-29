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

#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/CBoringLayoutMetrics.h"
#include "elastos/droid/text/BoringLayout.h"
#include "elastos/droid/text/CBoringLayout.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/TextLine.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Text::Style::EIID_IParagraphStyle;
using Elastos::Core::IString;

namespace Elastos {
namespace Droid {
namespace Text {

const Char32 BoringLayout::FIRST_RIGHT_TO_LEFT = 0x0590;//'\u0590';

CAR_INTERFACE_IMPL_2(BoringLayout, Layout, IBoringLayout, ITextUtilsEllipsizeCallback)

AutoPtr<IBoringLayout> BoringLayout::Make(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad)
{
    AutoPtr<IBoringLayout> layout;
    ASSERT_SUCCEEDED(CBoringLayout::New(source, paint, outerwidth, align,
        spacingmult, spacingadd, metrics, includepad, (IBoringLayout**)&layout));

    return layout;
}

AutoPtr<IBoringLayout> BoringLayout::Make(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Int32 ellipsizedWidth)
{
    AutoPtr<IBoringLayout> layout;
    ASSERT_SUCCEEDED(CBoringLayout::New(source, paint, outerwidth, align,
        spacingmult, spacingadd, metrics, includepad, ellipsize,
        ellipsizedWidth, (IBoringLayout**)&layout));

    return layout;
}

ECode BoringLayout::ReplaceOrMake(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad,
    /* [out] */ IBoringLayout** layout)
{
    VALIDATE_NOT_NULL(layout)

    ReplaceWith(source, paint, outerwidth, align, spacingmult,
        spacingadd);

    mEllipsizedWidth = outerwidth;
    mEllipsizedStart = 0;
    mEllipsizedCount = 0;

    Init(source, paint, outerwidth, align, spacingmult, spacingadd,
        metrics, includepad, TRUE);
    *layout = this;
    REFCOUNT_ADD(*layout);
    return NOERROR;
}

ECode BoringLayout::ReplaceOrMake(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Int32 ellipsizedWidth,
    /* [out] */ IBoringLayout** layout)
{
    VALIDATE_NOT_NULL(layout)

    Boolean trust = FALSE;

    if (ellipsize == TextUtilsTruncateAt_NONE || ellipsize == TextUtilsTruncateAt_MARQUEE) {
        ReplaceWith(source, paint, outerwidth, align, spacingmult, spacingadd);

        mEllipsizedWidth = outerwidth;
        mEllipsizedStart = 0;
        mEllipsizedCount = 0;
        trust = TRUE;
    } else {
       ReplaceWith(TextUtils::Ellipsize(source, paint, ellipsizedWidth,
            ellipsize, TRUE, this),
            paint, outerwidth, align, spacingmult,
            spacingadd);

        mEllipsizedWidth = ellipsizedWidth;
        trust = FALSE;
    }

    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    Init(text, paint, outerwidth, align, spacingmult, spacingadd,
         metrics, includepad, trust);

    *layout = this;
    REFCOUNT_ADD(*layout);
    return NOERROR;
}

BoringLayout::BoringLayout()
    : mTopPadding(0)
    , mBottomPadding(0)
    , mMax(0)
    , mEllipsizedWidth(0)
    , mEllipsizedStart(0)
    , mEllipsizedCount(0)
{}

ECode BoringLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad)
{
    FAIL_RETURN(Layout::constructor(source, paint, outerwidth, align, spacingmult, spacingadd))
    mEllipsizedWidth = outerwidth;
    mEllipsizedStart = 0;
    mEllipsizedCount = 0;
    return Init(source, paint, outerwidth, align, spacingmult, spacingadd,
        metrics, includepad, TRUE);
}

ECode BoringLayout::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Int32 ellipsizedWidth)
{
    /*
     * It is silly to have to call super() and then replaceWith(),
     * but we can't use "this" for the callback until the call to
     * super() finishes.
     */
    FAIL_RETURN(Layout::constructor(source, paint, outerwidth, align, spacingmult, spacingadd))

    Boolean trust = FALSE;

    if (ellipsize == TextUtilsTruncateAt_NONE || ellipsize == TextUtilsTruncateAt_MARQUEE) {
        mEllipsizedWidth = outerwidth;
        mEllipsizedStart = 0;
        mEllipsizedCount = 0;
        trust = TRUE;
    }
    else {
        ReplaceWith(TextUtils::Ellipsize(source, paint, ellipsizedWidth,
                     ellipsize, TRUE, this),
                   paint, outerwidth, align, spacingmult,
                   spacingadd);

        mEllipsizedWidth = ellipsizedWidth;
        trust = FALSE;
    }

    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    return Init(text, paint, outerwidth, align, spacingmult, spacingadd,
         metrics, includepad, trust);
}

ECode BoringLayout::Init(
    /* [in] */ ICharSequence* source,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 outerwidth,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ IBoringLayoutMetrics* metrics,
    /* [in] */ Boolean includepad,
    /* [in] */ Boolean trustWidth)
{
    VALIDATE_NOT_NULL(source)
    Int32 spacing;

    if (IString::Probe(source) != NULL && align == Elastos::Droid::Text::ALIGN_NORMAL) {
        source->ToString(&mDirect);
    }
    else {
        mDirect = NULL;
    }

    mPaint = IPaint::Probe(paint);

    Int32 mtbottom, mttop, mtdescent, mtascent;
    Int32 mtwidth;
    metrics->GetBottom(&mtbottom);
    metrics->GetTop(&mttop);
    metrics->GetDescent(&mtdescent);
    metrics->GetAscent(&mtascent);
    metrics->GetWidth(&mtwidth);
    if (includepad) {
        spacing = mtbottom - mttop;
    } else {
        spacing = mtdescent - mtascent;
    }

    mBottom = spacing;

    if (includepad) {
        mDesc = spacing + mttop;
    } else {
        mDesc = spacing + mtascent;
    }

    if (trustWidth) {
        mMax = mtwidth;
    } else {
        /*
         * If we have ellipsized, we have to actually calculate the
         * width because the width that was passed in was for the
         * full text, not the ellipsized form.
         */
        AutoPtr<TextLine> line = TextLine::Obtain();
        Int32 length = 0;
        source->GetLength(&length);
        line->Set(paint, source, 0, length, ILayout::DIR_LEFT_TO_RIGHT,
            Layout::DIRS_ALL_LEFT_TO_RIGHT, FALSE, NULL);
        mMax = (Int32) Elastos::Core::Math::Ceil(line->Metrics(NULL));
        TextLine::Recycle(line);
    }

    if (includepad) {
        mTopPadding = mttop - mtascent;
        mBottomPadding = mtbottom - mtdescent;
    }
    return NOERROR;
}

AutoPtr<IBoringLayoutMetrics> BoringLayout::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint)
{
    return IsBoring(text, paint, TextDirectionHeuristics::FIRSTSTRONG_LTR, NULL);
}

AutoPtr<IBoringLayoutMetrics> BoringLayout::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ ITextDirectionHeuristic* textDir)
{
    return IsBoring(text, paint, textDir, NULL);
}

AutoPtr<IBoringLayoutMetrics> BoringLayout::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ IBoringLayoutMetrics* metrics)
{
    return IsBoring(text, paint, TextDirectionHeuristics::FIRSTSTRONG_LTR, metrics);
}

AutoPtr<IBoringLayoutMetrics> BoringLayout::IsBoring(
    /* [in] */ ICharSequence* text,
    /* [in] */ ITextPaint* paint,
    /* [in] */ ITextDirectionHeuristic* textDir,
    /* [in] */ IBoringLayoutMetrics* metrics)
{
    assert(text != NULL);
    AutoPtr< ArrayOf<Char32> > temp = TextUtils::Obtain(500);
    Int32 length;
    text->GetLength(&length);
    Boolean boring = TRUE;

    for (Int32 i = 0; i < length; i += 500) {
        Int32 j = i + 500;

        if (j > length)
            j = length;

        TextUtils::GetChars(text, i, j, temp, 0);

        Int32 n = j - i;

        for (Int32 a = 0; a < n; a++) {
            Char32 c = (*temp)[a];

            if (c == '\n' || c == '\t' || c >= FIRST_RIGHT_TO_LEFT) {
                boring = FALSE;
                goto outer;
            }
        }

        Boolean bFlag = FALSE;
        if (textDir != NULL && (textDir->IsRtl(temp, 0, n, &bFlag), bFlag)) {
           boring = FALSE;
           goto outer;
        }
    }

outer:
    TextUtils::Recycle(temp);

    if (boring && ISpanned::Probe(text) != NULL) {
        ISpanned* sp = ISpanned::Probe(text);
        AutoPtr<ArrayOf<IInterface*> > styles;
        sp->GetSpans(0, length, EIID_IParagraphStyle, (ArrayOf<IInterface*>**)&styles);
        if (styles && styles->GetLength() > 0) {
            boring = FALSE;
        }
    }

    if (boring) {
        AutoPtr<IBoringLayoutMetrics> fm = metrics;
        if (fm == NULL) {
            CBoringLayoutMetrics::New((IBoringLayoutMetrics**)&fm);
        }

        AutoPtr<TextLine> line = TextLine::Obtain();
        line->Set(paint, text, 0, length, ILayout::DIR_LEFT_TO_RIGHT,
                Layout::DIRS_ALL_LEFT_TO_RIGHT, FALSE, NULL);
        Int32 wid = (Int32) Elastos::Core::Math::Ceil(line->Metrics(IPaintFontMetricsInt::Probe(fm)));
        fm->SetWidth(wid);
        TextLine::Recycle(line);

        return fm;
    } else {
        return NULL;
    }
}

ECode BoringLayout::GetHeight(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBottom;
    return NOERROR;
}

ECode BoringLayout::GetLineCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 1;
    return NOERROR;
}

ECode BoringLayout::GetLineTop(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (line != 0) {
        *result = mBottom;
    }
    return NOERROR;
}

ECode BoringLayout::GetLineDescent(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDesc;
    return NOERROR;
}

ECode BoringLayout::GetLineStart(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (line != 0) {
        AutoPtr<ICharSequence> csq;
        GetText((ICharSequence**)&csq);
        csq->GetLength(result);
    }
    return NOERROR;
}

ECode BoringLayout::GetParagraphDirection(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ILayout::DIR_LEFT_TO_RIGHT;
    return NOERROR;
}

ECode BoringLayout::GetLineContainsTab(
    /* [in] */ Int32 line,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode BoringLayout::GetLineMax(
    /* [in] */ Int32 line,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMax;
    return NOERROR;
}

ECode BoringLayout::GetLineDirections(
    /* [in] */ Int32 line,
    /* [out] */ ILayoutDirections** result)
{
    VALIDATE_NOT_NULL(result)
    *result = Layout::DIRS_ALL_LEFT_TO_RIGHT;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode BoringLayout::GetTopPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTopPadding;
    return NOERROR;
}

ECode BoringLayout::GetBottomPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBottomPadding;
    return NOERROR;
}

ECode BoringLayout::GetEllipsisCount(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEllipsizedCount;
    return NOERROR;
}

ECode BoringLayout::GetEllipsisStart(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEllipsizedStart;
    return NOERROR;
}

ECode BoringLayout::GetEllipsizedWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEllipsizedWidth;
    return NOERROR;
}

ECode BoringLayout::Draw(
    /* [in] */ ICanvas* c,
    /* [in] */ IPath* highlight,
    /* [in] */ IPaint* highlightpaint,
    /* [in] */ Int32 cursorOffset)
{
    VALIDATE_NOT_NULL(c)

    if (!mDirect.IsNull() && highlight == NULL) {
        c->DrawText(mDirect, 0, mBottom - mDesc, mPaint);
    }
    else {
        Layout::Draw(c, highlight, highlightpaint, cursorOffset);
    }

    return NOERROR;
}

ECode BoringLayout::Ellipsized(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    mEllipsizedStart = start;
    mEllipsizedCount = end - start;

    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
