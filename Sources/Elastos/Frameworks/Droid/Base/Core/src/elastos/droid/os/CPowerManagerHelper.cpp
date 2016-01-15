
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CPowerManagerHelper.h"
#include "elastos/droid/os/CPowerManager.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CPowerManagerHelper, Singleton, IPowerManagerHelper)

CAR_SINGLETON_IMPL(CPowerManagerHelper)

ECode CPowerManagerHelper::UseTwilightAdjustmentFeature(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CPowerManager::UseTwilightAdjustmentFeature();
    return NOERROR;
}

ECode CPowerManagerHelper::ValidateWakeLockParameters(
    /* [in] */ Int32 levelAndFlags,
    /* [in] */ const String& tag)
{
    return CPowerManager::ValidateWakeLockParameters(levelAndFlags, tag);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
