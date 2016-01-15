
#ifndef _ELASTOS_DROID_VIEW_CVIEWANIMATIONUTILSHELPER_H_
#define _ELASTOS_DROID_VIEW_CVIEWANIMATIONUTILSHELPER_H_

#include "_Elastos_Droid_View_CViewAnimationUtilsHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewAnimationUtilsHelper;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CViewAnimationUtilsHelper)
    , public Singleton
    , public IViewAnimationUtilsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateCircularReveal(
    	/* [in] */ IView* view,
        /* [in] */ Int32 centerX,
        /* [in] */ Int32 centerY,
        /* [in] */ Float startRadius,
        /* [in] */ Float endRadius,
        /* [out] */ IAnimator** result);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_VIEW_CVIEWANIMATIONUTILSHELPER_H_

