
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CWifiConfigurationPairwiseCipher.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

static AutoPtr< ArrayOf<String> > InitStrings()
{
    AutoPtr< ArrayOf<String> > strings = ArrayOf<String>::Alloc(3);
    (*strings)[0] = "NONE";
    (*strings)[1] = "TKIP";
    (*strings)[2] = "CCMP";
    return strings;
}

const AutoPtr< ArrayOf<String> > CWifiConfigurationPairwiseCipher::sStrings = InitStrings();

CAR_INTERFACE_IMPL(CWifiConfigurationPairwiseCipher, Singleton, IWifiConfigurationPairwiseCipher)

CAR_SINGLETON_IMPL(CWifiConfigurationPairwiseCipher)

ECode CWifiConfigurationPairwiseCipher::GetStrings(
    /* [out, callee] */ ArrayOf<String>** strings)
{
    VALIDATE_NOT_NULL(strings);
    *strings = sStrings;
    REFCOUNT_ADD(*strings);
    return NOERROR;
}

ECode CWifiConfigurationPairwiseCipher::ToString(
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
