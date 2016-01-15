
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CWifiConfigurationStatus.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > InitStrings()
{
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(3);
    (*strings)[0] = "current";
    (*strings)[1] = "disabled";
    (*strings)[2] = "enabled";
    return strings;
}

const AutoPtr< ArrayOf<String> > CWifiConfigurationStatus::sStrings = InitStrings();

CAR_INTERFACE_IMPL(CWifiConfigurationStatus, Singleton, IWifiConfigurationStatus)

CAR_SINGLETON_IMPL(CWifiConfigurationStatus)

ECode CWifiConfigurationStatus::GetStrings(
    /* [out, callee] */ ArrayOf<String>** strings)
{
    VALIDATE_NOT_NULL(strings);
    *strings = sStrings;
    REFCOUNT_ADD(*strings);
    return NOERROR;
}

ECode CWifiConfigurationStatus::ToString(
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
