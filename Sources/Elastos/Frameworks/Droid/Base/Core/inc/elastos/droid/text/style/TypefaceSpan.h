#ifndef __ELASTOS_DROID_TEXT_STYLE_TypefaceSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_TypefaceSpan_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * Changes the typeface family of the text to which the span is attached.
 */
class TypefaceSpan
    : public MetricAffectingSpan
    , public ITypefaceSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    TypefaceSpan();

    virtual ~TypefaceSpan();

    CARAPI constructor();

    /**
     * @param family The font family for this typeface.  Examples include
     * "monospace", "serif", and "sans-serif".
     */
    CARAPI constructor(
        /* [in] */ const String& family);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Returns the font family name.
     */
    CARAPI GetFamily(
        /* [out] */ String* family);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* paint);

private:
    static CARAPI Apply(
        /* [in] */ IPaint* paint,
        /* [in] */ const String& family);

private:
    /*const*/ String mFamily;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_TypefaceSpan_H__
