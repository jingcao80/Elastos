
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CUserManagerHelper.h"
#include "elastos/droid/os/CUserManager.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CUserManagerHelper, Singleton, IUserManagerHelper)

CAR_SINGLETON_IMPL(CUserManagerHelper)

ECode CUserManagerHelper::Get(
    /* [in] */ IContext* ctx,
    /* [out] */ IUserManager** userManager)
{
    VALIDATE_NOT_NULL(userManager)
    AutoPtr<IUserManager> um = CUserManager::Get(ctx);
    *userManager = um;
    REFCOUNT_ADD(*userManager)
    return NOERROR;
}

ECode CUserManagerHelper::SupportsMultipleUsers(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CUserManager::SupportsMultipleUsers();
    return NOERROR;
}

ECode CUserManagerHelper::GetMaxSupportedUsers(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = CUserManager::GetMaxSupportedUsers();
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
