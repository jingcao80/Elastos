
#include "elastos/droid/wifi/CWifiEnterpriseConfigEap.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > strings_init()
{
    const char* array[] = { "PEAP", "TLS", "TTLS", "PWD", "SIM", "AKA" };
    Int32 length = sizeof(array)/sizeof(array[0]);
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        (*strings)[i] = array[i];
    }

    return strings;
}

CAR_INTERFACE_IMPL(CWifiEnterpriseConfigEap, Singleton, IWifiEnterpriseConfigEap)

CAR_SINGLETON_IMPL(CWifiEnterpriseConfigEap)

CWifiEnterpriseConfigEap::CWifiEnterpriseConfigEap()
    : mStrings(strings_init())
{
}

ECode CWifiEnterpriseConfigEap::GetStrings(
    /* [out, callee] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStrings;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
