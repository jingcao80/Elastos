
#include "elastos/droid/dialer/util/COrientationUtil.h"
#include "elastos/droid/dialer/util/OrientationUtil.h"

namespace Elastos {
namespace Droid {
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
} // Droid
} // Elastos
