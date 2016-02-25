
#include "elastos/droid/app/CAppOpsManagerHelper.h"
#include "elastos/droid/app/AppOpsManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CAppOpsManagerHelper, Singleton, IAppOpsManagerHelper)

CAR_SINGLETON_IMPL(CAppOpsManagerHelper)

ECode CAppOpsManagerHelper::StrOpToOp(
    /* [in] */ const String& op,
    /* [out] */ Int32* result)
{
    return AppOpsManager::StrOpToOp(op, result);
}

ECode CAppOpsManagerHelper::GetToken(
    /* [in] */ IIAppOpsService* service,
    /* [out] */ IBinder** binder)
{
    return AppOpsManager::GetToken(service, binder);
}

ECode CAppOpsManagerHelper::OpToSwitch(
    /* [in] */ Int32 op,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AppOpsManager::OpToSwitch(op);
    return NOERROR;
}

ECode CAppOpsManagerHelper::OpToName(
    /* [in] */ Int32 op,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AppOpsManager::OpToName(op);
    return NOERROR;
}

ECode CAppOpsManagerHelper::OpToPermission(
    /* [in] */ Int32 op,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AppOpsManager::OpToPermission(op);
    return NOERROR;
}

ECode CAppOpsManagerHelper::OpToRestriction(
    /* [in] */ Int32 op,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AppOpsManager::OpToRestriction(op);
    return NOERROR;
}

ECode CAppOpsManagerHelper::OpAllowSystemBypassRestriction(
    /* [in] */ Int32 op,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AppOpsManager::OpAllowSystemBypassRestriction(op);
    return NOERROR;
}

ECode CAppOpsManagerHelper::OpToDefaultMode(
    /* [in] */ Int32 op,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AppOpsManager::OpToDefaultMode(op);
    return NOERROR;
}

ECode CAppOpsManagerHelper::OpAllowsReset(
    /* [in] */ Int32 op,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = AppOpsManager::OpAllowsReset(op);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
