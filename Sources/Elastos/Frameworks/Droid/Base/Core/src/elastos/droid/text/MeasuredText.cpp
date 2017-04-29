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

#include "elastos/droid/text/MeasuredText.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/AndroidBidi.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/text/Layout.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::Style::IReplacementSpan;
using Elastos::Droid::Text::Style::EIID_IMetricAffectingSpan;
using Elastos::Droid::Text::Style::EIID_IReplacementSpan;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Text {

Object MeasuredText::sLock;
AutoPtr< ArrayOf<MeasuredText*> > MeasuredText::sCached;

Boolean MeasuredText::localLOGV = FALSE;

MeasuredText::MeasuredText()
    : mTextStart(0)
    , mDir(0)
    , mEasy(FALSE)
    , mLen(0)
    , mPos(0)
{
    CTextPaint::New((ITextPaint**)&mWorkPaint);
}

AutoPtr<MeasuredText> MeasuredText::Obtain()
{
    AutoPtr<MeasuredText> mt;
    {
        AutoLock lock(sLock);

        if (sCached == NULL) {
            sCached = ArrayOf<MeasuredText*>::Alloc(3);
        }
        for (Int32 i = sCached->GetLength(); --i >= 0;) {
            if ((*sCached)[i] != NULL) {
                mt = (*sCached)[i];
                sCached->Set(i, NULL);
                return mt;
            }
        }
    }
    mt = new MeasuredText();
    if (localLOGV) {
        Logger::V("MEAS", String("new: ")/* + mt*/ + String("\n"));
    }
    return mt;
}

AutoPtr<MeasuredText> MeasuredText::Recycle(
    /* [in] */ MeasuredText* mt)
{
    assert(mt != NULL);

    mt->mText = NULL;
    if (mt->mLen < 1000) {
        AutoLock lock(sLock);

        if (sCached == NULL) {
            sCached = ArrayOf<MeasuredText*>::Alloc(3);
        }
        for (Int32 i = 0; i < sCached->GetLength(); ++i) {
            if ((*sCached)[i] == NULL) {
                sCached->Set(i, mt);
                mt->mText = NULL;
                break;
            }
        }
    }
    return NULL;
}

void MeasuredText::SetPos(
    /* [in] */ Int32 pos)
{
    mPos = pos - mTextStart;
}

/**
 * Analyzes text for bidirectional runs.  Allocates working buffers.
 */
void MeasuredText::SetPara(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ITextDirectionHeuristic* textDir)
{
    mText = text;
    mTextStart = start;

    Int32 len = end - start;
    mLen = len;
    mPos = 0;

    if (mWidths == NULL || mWidths->GetLength() < len) {
        mWidths = ArrayUtils::NewUnpaddedFloatArray(len);
    }
    if (mChars == NULL || mChars->GetLength() < len) {
        mChars = ArrayUtils::NewUnpaddedChar32Array(len);
    }
    TextUtils::GetChars(text, start, end, mChars, 0);

    if (ISpanned::Probe(text)) {
        ISpanned* spanned = ISpanned::Probe(text);
        AutoPtr< ArrayOf<IInterface*> > spans; //IReplacementSpan

        spanned->GetSpans(start, end, EIID_IReplacementSpan, (ArrayOf<IInterface*> **)&spans);
        if (spans != NULL) {
            for (Int32 i = 0; i < spans->GetLength(); i++) {
                IReplacementSpan* rs = IReplacementSpan::Probe((*spans)[i]);
                Int32 startInPara, endInPara;
                spanned->GetSpanStart(rs, &startInPara);
                startInPara -= start;
                spanned->GetSpanEnd(rs, &endInPara);
                endInPara -= start;

                // The span interval may be larger and must be restricted to [start, end[
                if (startInPara < 0) startInPara = 0;
                if (endInPara > len) endInPara = len;
                for (Int32 j = startInPara; j < endInPara; j++) {
                    (*mChars)[j] = 0xFFFC/*'\uFFFC'*/; // object replacement character
                }
            }
        }
    }

    if ((textDir == TextDirectionHeuristics::LTR ||
            textDir == TextDirectionHeuristics::FIRSTSTRONG_LTR ||
            textDir == TextDirectionHeuristics::ANYRTL_LTR) &&
            TextUtils::DoesNotNeedBidi(mChars, 0, len)) {
        mDir = ILayout::DIR_LEFT_TO_RIGHT;
        mEasy = TRUE;
    }
    else {
        if (mLevels == NULL || mLevels->GetLength() < len) {
            mLevels = ArrayUtils::NewUnpaddedByteArray(len);
        }
        Int32 bidiRequest;
        if (textDir == TextDirectionHeuristics::LTR) {
            bidiRequest = ILayout::DIR_REQUEST_LTR;
        }
        else if (textDir == TextDirectionHeuristics::RTL) {
            bidiRequest = ILayout::DIR_REQUEST_RTL;
        }
        else if (textDir == TextDirectionHeuristics::FIRSTSTRONG_LTR) {
            bidiRequest = ILayout::DIR_REQUEST_DEFAULT_LTR;
        }
        else if (textDir == TextDirectionHeuristics::FIRSTSTRONG_RTL) {
            bidiRequest = ILayout::DIR_REQUEST_DEFAULT_RTL;
        }
        else {
            Boolean isRtl;
            textDir->IsRtl(mChars, 0, len, &isRtl);
            bidiRequest = isRtl ? ILayout::DIR_REQUEST_RTL : ILayout::DIR_REQUEST_LTR;
        }
        mDir = AndroidBidi::Bidi(bidiRequest, mChars, mLevels, len, FALSE);
        mEasy = FALSE;
    }
}

Float MeasuredText::AddStyleRun(
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 len,
    /* [in] */ IPaintFontMetricsInt* fm)
{
    assert(paint != NULL);
    IPaint* p = IPaint::Probe(paint);

    if (fm != NULL) {
        Int32 spacing;
        p->GetFontMetricsInt(fm, &spacing);
    }

    Int32 pos = mPos;
    mPos = pos + len;

    Float retValue;
    if (mEasy) {
        Boolean isRtl = (mDir != Layout::DIR_LEFT_TO_RIGHT);
        return (p->GetTextRunAdvances(mChars, pos, len, pos, len, isRtl, mWidths, pos, &retValue), retValue);
    }

    Float totalAdvance = 0;
    Int32 level = (*mLevels)[pos];
    for (Int32 q = pos, i = pos + 1, e = pos + len;; ++i) {
        if (i == e || (*mLevels)[i] != level) {
            Boolean isRtl = (level & 0x1) != 0;
            p->GetTextRunAdvances(mChars, q, i - q, q, i - q, isRtl, mWidths, q, &retValue);
            totalAdvance += retValue;
            if (i == e) {
                break;
            }
            q = i;
            level = (*mLevels)[i];
        }
    }
    return totalAdvance;
}

Float MeasuredText::AddStyleRun(
    /* [in] */ ITextPaint* paint,
    /* [in] */ ArrayOf<IInterface*>* spans,//IMetricAffectingSpan
    /* [in] */ Int32 len,
    /* [in] */ IPaintFontMetricsInt* fm)
{
    ITextPaint* workPaint = mWorkPaint.Get();
    workPaint->Set(paint);
    // XXX paint should not have a baseline shift, but...
    workPaint->SetBaselineShift(0);

    AutoPtr<IReplacementSpan> replacement;
    for (Int32 i = 0; i < spans->GetLength(); i++) {
        AutoPtr<IMetricAffectingSpan> span = IMetricAffectingSpan::Probe((*spans)[i]);
        if (IReplacementSpan::Probe(span)) {
            replacement = IReplacementSpan::Probe(span);
        } else {
            span->UpdateMeasureState(workPaint);
        }
    }

    IPaint* p = IPaint::Probe(workPaint);
    Float wid;
    if (replacement == NULL) {
        wid = AddStyleRun(workPaint, len, fm);
    }
    else {
        // Use original text.  Shouldn't matter.
        Int32 intWid;
        replacement->GetSize(p, mText, mTextStart + mPos,
                mTextStart + mPos + len, fm, &intWid);
        wid = intWid;
        AutoPtr< ArrayOf<Float> > w = mWidths;
        (*w)[mPos] = wid;
        for (Int32 i = mPos + 1, e = mPos + len; i < e; i++)
            (*w)[i] = 0;
        mPos += len;
    }

    if (fm != NULL) {
        Int32 baselineShift;
        workPaint->GetBaselineShift(&baselineShift);
        if (baselineShift < 0) {
            Int32 top, ascent;
            fm->GetTop(&top);
            fm->GetAscent(&ascent);
            fm->SetAscent(ascent + baselineShift);
            fm->SetTop(top + baselineShift);
        } else {
            Int32 bottom, descent;
            fm->GetBottom(&bottom);
            fm->GetDescent(&descent);
            fm->SetDescent(descent + baselineShift);
            fm->SetBottom(bottom + baselineShift);
        }
    }

    return wid;
}

Int32 MeasuredText::BreakText(
    /* [in] */ Int32 limit,
    /* [in] */ Boolean forwards,
    /* [in] */ Float width)
{
    AutoPtr< ArrayOf<Float> > w = mWidths;
    if (forwards) {
        Int32 i = 0;
        while (i < limit) {
            width -= (*w)[i];
            if (width < 0.0f) break;
            i++;
        }
        while (i > 0 && (*mChars)[i - 1] == ' ') i--;
        return i;
    }
    else {
        Int32 i = limit - 1;
        while (i >= 0) {
            width -= (*w)[i];
            if (width < 0.0f) break;
            i--;
        }
        while (i < limit - 1 && (*mChars)[i + 1] == ' ') i++;
        return limit - i - 1;
    }
}

Float MeasuredText::Measure(
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit)
{
    Float width = 0;
    AutoPtr< ArrayOf<Float> > w = mWidths;
    for (Int32 i = start; i < limit; ++i) {
        width += (*w)[i];
    }
    return width;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
