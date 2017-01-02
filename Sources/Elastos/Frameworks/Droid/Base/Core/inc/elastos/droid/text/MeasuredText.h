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

#ifndef __ELASTOS_DROID_TEXT_MEASUREDTEXT_H__
#define __ELASTOS_DROID_TEXT_MEASUREDTEXT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Text::Style::IMetricAffectingSpan;

namespace Elastos {
namespace Droid {
namespace Text {

class MeasuredText
    : public Object
{
public:
    static CARAPI_(AutoPtr<MeasuredText>) Obtain();

    static CARAPI_(AutoPtr<MeasuredText>) Recycle(
        /* [in] */ MeasuredText* mt);

    MeasuredText();

    CARAPI_(void) SetPos(
        /* [in] */ Int32 pos);

    /**
     * Analyzes text for bidirectional runs.  Allocates working buffers.
     */
    CARAPI_(void) SetPara(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ITextDirectionHeuristic* textDir);

    CARAPI_(Float) AddStyleRun(
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 len,
        /* [in] */ IPaintFontMetricsInt* fm);

    CARAPI_(Float) AddStyleRun(
        /* [in] */ ITextPaint* paint,
        /* [in] */ ArrayOf<IInterface*>* spans, // IMetricAffectingSpan
        /* [in] */ Int32 len,
        /* [in] */ IPaintFontMetricsInt* fm);

    CARAPI_(Int32) BreakText(
        /* [in] */ Int32 limit,
        /* [in] */ Boolean forwards,
        /* [in] */ Float width);

    CARAPI_(Float) Measure(
        /* [in] */ Int32 start,
        /* [in] */ Int32 limit);

public:
    AutoPtr<ICharSequence> mText;
    Int32 mTextStart;
    AutoPtr< ArrayOf<Float> > mWidths;
    AutoPtr< ArrayOf<Char32> > mChars;
    AutoPtr< ArrayOf<Byte> > mLevels;
    Int32 mDir;
    Boolean mEasy;
    Int32 mLen;

private:
    static Object sLock;
    static AutoPtr< ArrayOf<MeasuredText*> > sCached;

    static Boolean localLOGV;
    Int32 mPos;
    AutoPtr<ITextPaint> mWorkPaint;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos


DEFINE_CONVERSION_FOR(Elastos::Droid::Text::MeasuredText, IInterface)


#endif //__ELASTOS_DROID_TEXT_MEASUREDTEXT_H__
