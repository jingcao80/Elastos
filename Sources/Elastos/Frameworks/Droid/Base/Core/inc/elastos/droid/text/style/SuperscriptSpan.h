#ifndef __ELASTOS_DROID_TEXT_STYLE_SuperscriptSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_SuperscriptSpan_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class SuperscriptSpan
    : public MetricAffectingSpan
    , public ISuperscriptSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    SuperscriptSpan();

    virtual ~SuperscriptSpan();

    CARAPI constructor();

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* tp);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* tp);
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_SuperscriptSpan_H__
