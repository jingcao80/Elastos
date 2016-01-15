
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CWifiConfigurationAuthAlgorithm.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > InitStrings()
{
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(3);
    (*strings)[0] = "OPEN";
    (*strings)[1] = "SHARED";
    (*strings)[2] = "LEAP";
    return strings;
}

const AutoPtr< ArrayOf<String> > CWifiConfigurationAuthAlgorithm::sStrings = InitStrings();

CAR_INTERFACE_IMPL(CWifiConfigurationAuthAlgorithm, Singleton, IWifiConfigurationAuthAlgorithm)

CAR_SINGLETON_IMPL(CWifiConfigurationAuthAlgorithm)

ECode CWifiConfigurationAuthAlgorithm::GetStrings(
    /* [out, callee] */ ArrayOf<String>** strings)
{
    VALIDATE_NOT_NULL(strings);
    *strings = sStrings;
    REFCOUNT_ADD(*strings);
    return NOERROR;
}

ECode CWifiConfigurationAuthAlgorithm::ToString(
    /* [in] */ Int32 index,
    /* [out] */ String* strValue)
{
    VALIDATE_NOT_NULL(strValue);
    if (index < 0 || index > 2) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    *strValue = (*sStrings)[index];
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
