
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CANTICIPATEOVERSHOOTINTERPOLATOR_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CANTICIPATEOVERSHOOTINTERPOLATOR_H__

#include "_Elastos_Droid_View_Animation_CAnticipateOvershootInterpolator.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Internal::View::Animation::INativeInterpolatorFactory;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CarClass(CAnticipateOvershootInterpolator)
    , public Object
    , public IAnticipateOvershootInterpolator
    , public INativeInterpolatorFactory
    , public IInterpolator
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CAnticipateOvershootInterpolator();

    ~CAnticipateOvershootInterpolator();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Float tension);

    CARAPI constructor(
        /* [in] */ Float tension,
        /* [in] */ Float extraTension);

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
    Float mTension;
};

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_CANTICIPATEOVERSHOOTINTERPOLATOR_H__
