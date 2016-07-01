
#include "elastos/droid/app/admin/CDevicePolicyManagerHelper.h"
#include "elastos/droid/app/admin/CDevicePolicyManager.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Admin {

CAR_INTERFACE_IMPL(CDevicePolicyManagerHelper, Singleton, IDevicePolicyManagerHelper)

CAR_SINGLETON_IMPL(CDevicePolicyManagerHelper)

ECode CDevicePolicyManagerHelper::Create(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [out] */ IDevicePolicyManager** policymanager)
{
    VALIDATE_NOT_NULL(policymanager)

    return CDevicePolicyManager::Create(context, handler, policymanager);
}

} // namespace Admin
} // namespace App
} // namespace Droid
} // namespace Elastos