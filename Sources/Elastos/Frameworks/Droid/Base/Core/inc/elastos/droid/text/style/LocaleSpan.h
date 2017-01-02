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

#ifndef __ELASTOS_DROID_TEXT_STYLE_LOCALESPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_LOCALESPAN_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

using Elastos::Utility::ILocale;
using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * Changes the {@link Locale} of the text to which the span is attached.
 */
class LocaleSpan
    : public MetricAffectingSpan
    , public ILocaleSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    LocaleSpan();

    virtual ~LocaleSpan();

    CARAPI constructor();

    /**
     * Creates a LocaleSpan.
     * @param locale The {@link Locale} of the text to which the span is
     * attached.
     */
    CARAPI constructor(
        /* [in] */ ILocale* locale);

    //@Override
    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    //@Override

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the {@link Locale}.
     *
     * @return The {@link Locale} for this span.
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* paint);

private:
    static CARAPI Apply(
        /* [in] */ IPaint* paint,
        /* [in] */ ILocale* locale);

private:
    AutoPtr<ILocale> mLocale;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_LOCALESPAN_H__
