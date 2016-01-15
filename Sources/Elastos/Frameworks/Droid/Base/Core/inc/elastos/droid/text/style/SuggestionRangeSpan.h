#ifndef __ELASTOS_DROID_TEXT_STYLE_SuggestionRangeSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_SuggestionRangeSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

/**
 * A SuggestionRangeSpan is used to show which part of an EditText is affected by a suggestion
 * popup window.
 *
 * @hide
 */
class SuggestionRangeSpan
    : public CharacterStyle
    , public ISuggestionRangeSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    SuggestionRangeSpan();

    virtual ~SuggestionRangeSpan();

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI SetBackgroundColor(
        /* [in] */ Int32 backgroundColor);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* tp);

private:
    Int32 mBackgroundColor;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_SuggestionRangeSpan_H__
