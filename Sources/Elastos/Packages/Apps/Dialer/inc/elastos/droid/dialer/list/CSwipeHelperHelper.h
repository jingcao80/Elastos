#ifndef __ELASTOS_DROID_DIALER_LIST_CSWIPEHELPERHELPER_H__
#define __ELASTOS_DROID_DIALER_LIST_CSWIPEHELPERHELPER_H__

#include "_Elastos_Droid_Dialer_List_CSwipeHelperHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CarClass(CSwipeHelperHelper)
    , public Singleton
    , public ISwipeHelperHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    // invalidate the view's own bounds all the way up the view hierarchy
    CARAPI InvalidateGlobalRegion(
        /* [in] */ IView* view);

    // invalidate a rectangle relative to the view's coordinate system all the way up the view
    // hierarchy
    CARAPI InvalidateGlobalRegion(
        /* [in] */ IView* view,
        /* [in] */ IRectF* childBounds);

    CARAPI SetSwipeable(
        /* [in] */ IView* view,
        /* [in] */ Boolean swipeable);

    CARAPI IsSwipeable(
        /* [in] */ IView* view,
        /* [out] */ Boolean* result);

    CARAPI GetIS_SWIPEABLE(
        /* [out] */ IInterface** obj);
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_CSWIPEHELPERHELPER_H__
