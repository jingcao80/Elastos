#ifndef __ELASTOS_DROID_TEXT_STYLE_BackgroundColorSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_BackgroundColorSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class BackgroundColorSpan
    : public CharacterStyle
    , public IBackgroundColorSpan
    , public IUpdateAppearance
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    BackgroundColorSpan();

    virtual ~BackgroundColorSpan();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 color);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetBackgroundColor(
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

#endif // __ELASTOS_DROID_TEXT_STYLE_BackgroundColorSpan_H__
