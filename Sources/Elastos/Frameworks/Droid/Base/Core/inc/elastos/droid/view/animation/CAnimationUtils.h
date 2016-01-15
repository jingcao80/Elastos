
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CANIMATIONUTILS_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CANIMATIONUTILS_H__

#include "_Elastos_Droid_View_Animation_CAnimationUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CarClass(CAnimationUtils)
    , public Singleton
    , public IAnimationUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CurrentAnimationTimeMillis(
        /* [out] */ Int64* time);

    CARAPI LoadAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ IAnimation** animation);

    CARAPI LoadLayoutAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ ILayoutAnimationController** controller);

    CARAPI MakeInAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Boolean fromLeft,
        /* [out] */ IAnimation** animation);

    CARAPI MakeOutAnimation(
        /* [in] */ IContext* context,
        /* [in] */ Boolean toRight,
        /* [out] */ IAnimation** animation);

    CARAPI MakeInChildBottomAnimation(
        /* [in] */ IContext* context,
        /* [out] */ IAnimation** animation);

    CARAPI LoadInterpolator(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ IInterpolator** interpolator);

    CARAPI LoadInterpolator(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Int32 id,
        /* [out] */ IInterpolator** interpolator);
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_CANIMATIONUTILS_H__
