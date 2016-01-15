#ifndef __ELASTOS_DROID_TEXT_STYLE_RelativeSizeSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_RelativeSizeSpan_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class RelativeSizeSpan
    : public MetricAffectingSpan
    , public IRelativeSizeSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    RelativeSizeSpan();

    virtual ~RelativeSizeSpan();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Float proportion);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSizeChange(
        /* [out] */ Float* change);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* ds);

private:
    /*const*/ Float mProportion;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_RelativeSizeSpan_H__
