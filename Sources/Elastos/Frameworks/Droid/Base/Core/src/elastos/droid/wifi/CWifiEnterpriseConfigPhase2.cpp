
#include "elastos/droid/wifi/CWifiEnterpriseConfigPhase2.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > strings_init()
{
    const char* array[] = { IWifiEnterpriseConfig::EMPTY_VALUE, "PAP", "MSCHAP",
                "MSCHAPV2", "GTC" };
    Int32 length = sizeof(array)/sizeof(array[0]);
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        (*strings)[i] = array[i];
    }

    return strings;
}

const String CWifiEnterpriseConfigPhase2::PREFIX("auth=");

CAR_INTERFACE_IMPL(CWifiEnterpriseConfigPhase2, Singleton, IWifiEnterpriseConfigEap)

CAR_SINGLETON_IMPL(CWifiEnterpriseConfigPhase2)

CWifiEnterpriseConfigPhase2::CWifiEnterpriseConfigPhase2()
    : mStrings(strings_init())
{
}

ECode CWifiEnterpriseConfigPhase2::GetStrings(
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
