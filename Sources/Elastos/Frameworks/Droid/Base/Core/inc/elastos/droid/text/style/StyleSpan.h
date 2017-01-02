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

#ifndef __ELASTOS_DROID_TEXT_STYLE_STYLESPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_STYLESPAN_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::IParcelableSpan;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 *
 * Describes a style in a span.
 * Note that styles are cumulative -- if both bold and italic are set in
 * separate spans, or if the base style is bold and a span calls for italic,
 * you get bold italic.  You can't turn off a style from the base style.
 *
 */
class StyleSpan
    : public MetricAffectingSpan
    , public IStyleSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    StyleSpan();

    virtual ~StyleSpan();

    CARAPI constructor();

    /**
     *
     * @param style An integer constant describing the style for this span. Examples
     * include bold, italic, and normal. Values are constants defined
     * in {@link android.graphics.Typeface}.
     */
    CARAPI constructor(
        /* [in] */ Int32 style);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the style constant defined in {@link android.graphics.Typeface}.
     */
    CARAPI GetStyle(
        /* [out] */ Int32* style);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* paint);

private:
    static CARAPI Apply(
        /* [in] */ IPaint* paint,
        /* [in] */ Int32 style);

private:
    /*const*/ Int32 mStyle;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_STYLESPAN_H__
