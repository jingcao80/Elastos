
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CACCELERATEDECELERATEINTERPOLATOR_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CACCELERATEDECELERATEINTERPOLATOR_H__

#include "_Elastos_Droid_View_Animation_CAccelerateDecelerateInterpolator.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::View::Animation::INativeInterpolatorFactory;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CarClass(CAccelerateDecelerateInterpolator)
    , public Object
    , public IAccelerateDecelerateInterpolator
    , public INativeInterpolatorFactory
    , public IInterpolator
    , public ITimeInterpolator
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccelerateDecelerateInterpolator();

    ~CAccelerateDecelerateInterpolator();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
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
};

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_CACCELERATEDECELERATEINTERPOLATOR_H__
