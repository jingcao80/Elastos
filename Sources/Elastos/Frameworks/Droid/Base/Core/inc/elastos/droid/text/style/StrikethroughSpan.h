#ifndef __ELASTOS_DROID_TEXT_STYLE_StrikethroughSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_StrikethroughSpan_H__

#include "elastos/droid/text/style/CharacterStyle.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class StrikethroughSpan
    : public CharacterStyle
    , public IStrikethroughSpan
    , public IUpdateAppearance
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    StrikethroughSpan();

    virtual ~StrikethroughSpan();

    CARAPI constructor();

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_StrikethroughSpan_H__
