#include "elastos/droid/internal/policy/impl/CEnableAccessibilityControllerHelper.h"
#include "elastos/droid/internal/policy/impl/EnableAccessibilityController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CAR_SINGLETON_IMPL(CEnableAccessibilityControllerHelper)
CAR_INTERFACE_IMPL(CEnableAccessibilityControllerHelper, Singleton, IEnableAccessibilityControllerHelper)

ECode CEnableAccessibilityControllerHelper::CanEnableAccessibilityViaGesture(
    /* [in] */ IContext* context,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = EnableAccessibilityController::CanEnableAccessibilityViaGesture(context);
    return NOERROR;
}


} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
