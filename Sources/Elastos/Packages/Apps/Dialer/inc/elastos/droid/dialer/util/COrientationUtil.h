#ifndef __ELASTOS_DROID_DIALER_UTIL_CORIENTATIONUTIL_H__
#define __ELASTOS_DROID_DIALER_UTIL_CORIENTATIONUTIL_H__

#include "_Elastos_Droid_Dialer_Util_COrientationUtil.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Util {

CarClass(COrientationUtil)
    , public Singleton
    , public IOrientationUtil
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * @return if the context is in landscape orientation.
     */
    CARAPI IsLandscape(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* result);
};

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_CORIENTATIONUTIL_H__
