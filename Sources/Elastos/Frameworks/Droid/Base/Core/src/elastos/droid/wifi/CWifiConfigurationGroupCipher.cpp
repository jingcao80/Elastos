
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CWifiConfigurationGroupCipher.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > InitStrings()
{
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(4);
    (*strings)[0] = "WEP40";
    (*strings)[1] = "WEP104";
    (*strings)[2] = "TKIP";
    (*strings)[3] = "CCMP";
    return strings;
}

const AutoPtr< ArrayOf<String> > CWifiConfigurationGroupCipher::sStrings = InitStrings();

CAR_INTERFACE_IMPL(CWifiConfigurationGroupCipher, Singleton, IWifiConfigurationGroupCipher)

CAR_SINGLETON_IMPL(CWifiConfigurationGroupCipher)

ECode CWifiConfigurationGroupCipher::GetStrings(
    /* [out, callee] */ ArrayOf<String>** strings)
{
    VALIDATE_NOT_NULL(strings);
    *strings = sStrings;
    REFCOUNT_ADD(*strings);
    return NOERROR;
}

ECode CWifiConfigurationGroupCipher::ToString(
    /* [in] */ Int32 index,
    /* [out] */ String* strValue)
{
    VALIDATE_NOT_NULL(strValue);
    if (index < 0 || index > 3) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    *strValue = (*sStrings)[index];
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos