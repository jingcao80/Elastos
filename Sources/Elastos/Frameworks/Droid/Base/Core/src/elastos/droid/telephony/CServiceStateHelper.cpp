
#include "elastos/droid/telephony/CServiceStateHelper.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CServiceStateHelper, Singleton, IServiceStateHelper)

CAR_SINGLETON_IMPL(CServiceStateHelper)

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
    return CServiceState::RilRadioTechnologyToString(rt, res);
}

ECode CServiceStateHelper::IsGsm(
    /* [in] */ Int32 radioTechnology,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return CServiceState::IsGsm(radioTechnology, res);
}

ECode CServiceStateHelper::IsCdma(
    /* [in] */ Int32 radioTechnology,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return CServiceState::IsCdma(radioTechnology, res);
}

} //namespace Elastos
} //namespace Droid
} //namespace Telephony
