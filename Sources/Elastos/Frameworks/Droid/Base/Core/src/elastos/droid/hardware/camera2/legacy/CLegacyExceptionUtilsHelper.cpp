
#include "elastos/droid/hardware/camera2/legacy/CLegacyExceptionUtilsHelper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyExceptionUtils.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CLegacyExceptionUtilsHelper, Singleton, ILegacyExceptionUtilsHelper)

CAR_SINGLETON_IMPL(CLegacyExceptionUtilsHelper)

ECode CLegacyExceptionUtilsHelper::ThrowOnError(
    /* [in] */ Int32 errorFlag,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    return LegacyExceptionUtils::ThrowOnError(errorFlag, result);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
