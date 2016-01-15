
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONGROUPCIPHER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONGROUPCIPHER_H__

#include "_Elastos_Droid_Wifi_CWifiConfigurationGroupCipher.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiConfigurationGroupCipher)
    , public Singleton
    , public IWifiConfigurationGroupCipher
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetStrings(
        /* [out, callee] */ ArrayOf<String>** strings);

    CARAPI ToString(
        /* [in] */ Int32 index,
        /* [out] */ String* strValue);

public:
    static const AutoPtr< ArrayOf<String> > sStrings;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONGROUPCIPHER_H__
