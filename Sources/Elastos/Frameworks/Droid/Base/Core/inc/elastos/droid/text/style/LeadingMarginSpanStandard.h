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

#ifndef __ELASTOS_DROID_TEXT_STYLE_LEADINGMARGINSPAN_STANDARD_H__
#define __ELASTOS_DROID_TEXT_STYLE_LEADINGMARGINSPAN_STANDARD_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::LayoutAlignment;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class LeadingMarginSpanStandard
    : public Object
    , public ILeadingMarginSpanStandard
    , public ILeadingMarginSpan
    , public IParagraphStyle
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    LeadingMarginSpanStandard();

    virtual ~LeadingMarginSpanStandard();

    CARAPI constructor();

    /**
     * Constructor taking separate indents for the first and subsequent
     * lines.
     *
     * @param first the indent for the first line of the paragraph
     * @param rest the indent for the remaining lines of the paragraph
     */
    CARAPI constructor(
        /* [in] */ Int32 first,
        /* [in] */ Int32 rest);

    /**
     * Constructor taking an indent for all lines.
     * @param every the indent of each line
     */
    CARAPI constructor(
        /* [in] */ Int32 every);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetLeadingMargin(
        /* [in] */ Boolean first,
        /* [out] */ Int32* value);

    CARAPI DrawLeadingMargin(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p,
        /* [in] */ Int32 x,
        /* [in] */ Int32 dir,
        /* [in] */ Int32 top,
        /* [in] */ Int32 baseline,
        /* [in] */ Int32 bottom,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Boolean first,
        /* [in] */ ILayout* layout);

private:
    Int32 mFirst;
    Int32 mRest;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_LEADINGMARGINSPAN_STANDARD_H__
