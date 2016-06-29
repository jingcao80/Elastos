
#include "elastos/apps/dialer/util/COrientationUtil.h"
#include "elastos/apps/dialer/util/OrientationUtil.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Util {

CAR_INTERFACE_IMPL(COrientationUtil, Singleton, IOrientationUtil);

CAR_SINGLETON_IMPL(COrientationUtil);

ECode COrientationUtil::IsLandscape(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = OrientationUtil::IsLandscape(context);
    return NOERROR;
}

} // Util
} // Dialer
} // Apps
} // Elastos
