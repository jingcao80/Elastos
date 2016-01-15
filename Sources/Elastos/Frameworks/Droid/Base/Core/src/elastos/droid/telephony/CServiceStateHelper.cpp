
#include "CServiceStateHelper.h"
#include "CServiceState.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {


ECode CServiceStateHelper::NewFromBundle(
    /* [in] */ IBundle* m,
    /* [out] */ IServiceState** res)
{
    return CServiceState::NewFromBundle(m, res);
}

ECode CServiceStateHelper::RilRadioTechnologyToString(
    /* [in] */ Int32 rt,
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CServiceState::RilRadioTechnologyToString(rt);
    return NOERROR;
}

ECode CServiceStateHelper::IsGsm(
    /* [in] */ Int32 radioTechnology,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CServiceState::IsGsm(radioTechnology);
    return NOERROR;
}

ECode CServiceStateHelper::IsCdma(
    /* [in] */ Int32 radioTechnology,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = CServiceState::IsCdma(radioTechnology);
    return NOERROR;
}

}
}
}

