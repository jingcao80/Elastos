#ifndef __ELASTOS_DROID_TEXT_STYLE_UnderlineSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_UnderlineSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class UnderlineSpan
    : public CharacterStyle
    , public IUnderlineSpan
    , public IUpdateAppearance
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    UnderlineSpan();

    virtual ~UnderlineSpan();

    CARAPI constructor();

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_UnderlineSpan_H__
