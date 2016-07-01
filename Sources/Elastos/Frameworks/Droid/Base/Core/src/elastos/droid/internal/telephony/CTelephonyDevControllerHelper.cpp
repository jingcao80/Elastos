
#include "elastos/droid/internal/telephony/CTelephonyDevControllerHelper.h"
#include "elastos/droid/internal/telephony/TelephonyDevController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CTelephonyDevControllerHelper, Singleton, ITelephonyDevControllerHelper)

CAR_SINGLETON_IMPL(CTelephonyDevControllerHelper)

ECode CTelephonyDevControllerHelper::Create(
    /* [out] */ ITelephonyDevController** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITelephonyDevController> res = TelephonyDevController::Create();
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTelephonyDevControllerHelper::GetInstance(
    /* [out] */ ITelephonyDevController** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITelephonyDevController> res = TelephonyDevController::GetInstance();
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CTelephonyDevControllerHelper::RegisterRIL(
    /* [in] */ ICommandsInterface* cmdsIf)
{
    return TelephonyDevController::RegisterRIL(cmdsIf);
}

ECode CTelephonyDevControllerHelper::UnregisterRIL(
    /* [in] */ ICommandsInterface* cmdsIf)
{
    return TelephonyDevController::UnregisterRIL(cmdsIf);
}

ECode CTelephonyDevControllerHelper::GetModemCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 res = TelephonyDevController::GetModemCount();
    *result = res;
    return NOERROR;
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony