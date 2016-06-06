#ifndef __ELASTOS_APPS_DIALER_UTIL_ORIENTATIONUTIL_H__
#define __ELASTOS_APPS_DIALER_UTIL_ORIENTATIONUTIL_H__

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

/**
 * Static methods related to device orientation.
 */
class OrientationUtil
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
