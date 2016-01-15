#ifndef __ELASTOS_DROID_TEXT_STYLE_ForegroundColorSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_ForegroundColorSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class ForegroundColorSpan
    : public CharacterStyle
    , public IForegroundColorSpan
    , public IUpdateAppearance
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ForegroundColorSpan();

    virtual ~ForegroundColorSpan();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 color);

    ForegroundColorSpan(
        /* [in] */ IParcel* src);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetForegroundColor(
        /* [out] */ Int32* color);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

private:
    /*const*/ Int32 mColor;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_ForegroundColorSpan_H__
