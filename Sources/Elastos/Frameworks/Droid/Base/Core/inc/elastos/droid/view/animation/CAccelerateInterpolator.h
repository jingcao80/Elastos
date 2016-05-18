
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CACCELERATEINTERPOLATOR_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CACCELERATEINTERPOLATOR_H__

#include "_Elastos_Droid_View_Animation_CAccelerateInterpolator.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Internal::View::Animation::INativeInterpolatorFactory;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CarClass(CAccelerateInterpolator)
    , public Object
    , public IAccelerateInterpolator
    , public INativeInterpolatorFactory
    , public IInterpolator
    , public ITimeInterpolator
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccelerateInterpolator();

    ~CAccelerateInterpolator();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Float factor);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /** @hide */
    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetInterpolation(
        /* [in] */ Float input,
        /* [out] */ Float* output);

    /** @hide */
    // @Override
    CARAPI CreateNativeInterpolator(
        /* [out] */ Int64* interpolator);

    // @Override
    CARAPI HasNativeInterpolator(
        /* [out] */ Boolean* res);

private:
    Float mFactor;
    Double mDoubleFactor;
};

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_CACCELERATEINTERPOLATOR_H__
