#ifndef __ELASTOS_DROID_TEXT_STYLE_ScaleXSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_ScaleXSpan_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class ScaleXSpan
    : public MetricAffectingSpan
    , public IScaleXSpan
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    ScaleXSpan();

    virtual ~ScaleXSpan();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Float proportion);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetScaleX(
        /* [out] */ Float* x);

    //@Override
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);

    //@Override
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* ds);

private:
    Float mProportion;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_ScaleXSpan_H__
