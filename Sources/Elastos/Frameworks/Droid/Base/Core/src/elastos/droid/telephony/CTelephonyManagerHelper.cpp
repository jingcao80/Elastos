
#include "CTelephonyManagerHelper.h"
#include "CTelephonyManager.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

ECode CTelephonyManagerHelper::GetDefault(
    /* [out] */ ITelephonyManager** res)
{
    return CTelephonyManager::GetDefault(res);
}

ECode CTelephonyManagerHelper::From(
    /* [in] */ IContext* context,
    /* [out] */ ITelephonyManager** res)
{
    return CTelephonyManager::From(context, res);
}

ECode CTelephonyManagerHelper::GetPhoneType(
    /* [in] */ Int32 networkMode,
    /* [out] */ Int32* val)
{
    return CTelephonyManager::GetPhoneType(networkMode, val);
}

ECode CTelephonyManagerHelper::GetLteOnCdmaModeStatic(
    /* [out] */ Int32* val)
{
    return CTelephonyManager::GetLteOnCdmaModeStatic(val);
}

ECode CTelephonyManagerHelper::GetNetworkClass(
    /* [in] */ Int32 networkType,
    /* [out] */ Int32* val)
{
    VALIDATE_NOT_NULL(val);
    *val = CTelephonyManager::GetNetworkClass(networkType);
    return NOERROR;
}

ECode CTelephonyManagerHelper::GetNetworkTypeName(
    /* [in] */ Int32 type,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CTelephonyManager::GetNetworkTypeName(type);
    return NOERROR;
}

}
}
}

