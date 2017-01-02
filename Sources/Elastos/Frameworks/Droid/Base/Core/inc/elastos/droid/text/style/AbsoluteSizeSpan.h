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

#ifndef __ELASTOS_DROID_TEXT_STYLE_AbsoluteSizeSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_AbsoluteSizeSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/text/style/MetricAffectingSpan.h"

using Elastos::Droid::Text::ITextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class AbsoluteSizeSpan
    : public MetricAffectingSpan
    , public IAbsoluteSizeSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    AbsoluteSizeSpan();

    virtual ~AbsoluteSizeSpan();

    CARAPI constructor();

    /**
     * Set the text size to <code>size</code> physical pixels.
     */
    CARAPI constructor(
        /* [in] */ Int32 size);

    /**
     * Set the text size to <code>size</code> physical pixels,
     * or to <code>size</code> device-independent pixels if
     * <code>dip</code> is true.
     */
    CARAPI constructor(
        /* [in] */ Int32 size,
        /* [in] */ Boolean dip);


    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI GetDip(
        /* [out] */ Boolean* dip);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* ds);

private:
    /*const*/ Int32 mSize;
    Boolean mDip;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_AbsoluteSizeSpan_H__
