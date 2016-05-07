
#include "elastos/droid/wifi/CWifiInfoHelper.h"
#include "elastos/droid/wifi/CWifiInfo.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiInfoHelper, Singleton, IWifiInfoHelper)

CAR_SINGLETON_IMPL(CWifiInfoHelper)

ECode CWifiInfoHelper::GetDetailedStateOf(
    /* [in] */ ISupplicantState* suppState,
    /* [out] */ NetworkInfoDetailedState* detailedState)
{
    VALIDATE_NOT_NULL(detailedState);
    SupplicantState ss;
    suppState->Get(&ss);
    *detailedState = CWifiInfo::GetDetailedStateOf(ss);
    return NOERROR;
}

ECode CWifiInfoHelper::RemoveDoubleQuotes(
    /* [in] */ const String& str,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    CWifiInfo::RemoveDoubleQuotes(str, value);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
