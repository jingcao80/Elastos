
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CCYCLEINTERPOLATOR_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CCYCLEINTERPOLATOR_H__

#include "_Elastos_Droid_View_Animation_CCycleInterpolator.h"
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

CarClass(CCycleInterpolator)
    , public Object
    , public ICycleInterpolator
    , public INativeInterpolatorFactory
    , public IInterpolator
    , public ITimeInterpolator
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CCycleInterpolator();

    ~CCycleInterpolator();

    CARAPI constructor(
        /* [in] */ Float cycles);

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
    Float mCycles;
};

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_CCYCLEINTERPOLATOR_H__
