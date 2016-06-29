#ifndef __ELASTOS_APPS_DIALER_UTIL_ORIENTATIONUTIL_H__
#define __ELASTOS_APPS_DIALER_UTIL_ORIENTATIONUTIL_H__

#include <elastos/core/Object.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

/**
 * Static methods related to device orientation.
 */
class OrientationUtil
    : public Object
{
public:
    /**
     * @return if the context is in landscape orientation.
     */
    static CARAPI_(Boolean) IsLandscape(
        /* [in] */ IContext* context);
}

} // Util
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_UTIL_ORIENTATIONUTIL_H__
