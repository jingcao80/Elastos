
#include "elastos/droid/wifi/CScanResultHelper.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CScanResultHelper, Singleton, IScanResultHelper)

CAR_SINGLETON_IMPL(CScanResultHelper)

ECode CScanResultHelper::Is24GHz(
    /* [in] */ Int32 freq,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = freq > 2400 && freq < 2500;
    return NOERROR;
}

ECode CScanResultHelper::Is5GHz(
    /* [in] */ Int32 freq,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = freq > 4900 && freq < 5900;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
