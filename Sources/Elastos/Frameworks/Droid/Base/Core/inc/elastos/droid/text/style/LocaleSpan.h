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
