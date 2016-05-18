
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_TRANSLATEANIMATION_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_TRANSLATEANIMATION_H__

#include "elastos/droid/view/animation/Animation.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ITypedValue;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

class TranslateAnimation
    : public Animation
    , public ITranslateAnimation
{
public:
    CAR_INTERFACE_DECL()

    TranslateAnimation();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ Float fromXDelta,
        /* [in] */ Float toXDelta,
        /* [in] */ Float fromYDelta,
        /* [in] */ Float toYDelta);

    CARAPI constructor(
        /* [in] */ Float fromXType,
        /* [in] */ Float fromXValue,
        /* [in] */ Float toXType,
        /* [in] */ Float toXValue,
        /* [in] */ Float fromYType,
        /* [in] */ Float fromYValue,
        /* [in] */ Float toYType,
        /* [in] */ Float toYValue);

    //@Override
    CARAPI Initialize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 parentWidth,
        /* [in] */ Int32 parentHeight);

protected:
    //@Override
    CARAPI_(void) ApplyTransformation(
        /* [in] */ Float interpolatedTime,
        /* [in] */ ITransformation* t);

    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI CloneImpl(
        /* [in] */ ITranslateAnimation* object);

private:
    Int32 mFromXType;
    Int32 mToXType;

    Int32 mFromYType;
    Int32 mToYType;

    Float mFromXValue;
    Float mToXValue;

    Float mFromYValue;
    Float mToYValue;

    Float mFromXDelta;
    Float mToXDelta;
    Float mFromYDelta;
    Float mToYDelta;
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_TRANSLATEANIMATION_H__
