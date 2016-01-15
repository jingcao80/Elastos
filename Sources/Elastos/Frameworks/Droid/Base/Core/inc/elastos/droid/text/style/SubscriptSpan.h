#ifndef __ELASTOS_DROID_TEXT_STYLE_SubscriptSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_SubscriptSpan_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class SubscriptSpan
    : public MetricAffectingSpan
    , public ISubscriptSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    SubscriptSpan();

    virtual ~SubscriptSpan();

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

#endif // __ELASTOS_DROID_TEXT_STYLE_SubscriptSpan_H__
