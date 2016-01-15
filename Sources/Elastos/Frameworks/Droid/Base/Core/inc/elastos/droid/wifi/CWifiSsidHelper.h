
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISSIDHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISSIDHELPER_H__

#include "_Elastos_Droid_Wifi_CWifiSsidHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiSsidHelper)
    , public Singleton
    , public IWifiSsidHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateFromAsciiEncoded(
        /* [in] */ const String& asciiEncoded,
        /* [out] */ IWifiSsid** wifiSsid);

    CARAPI CreateFromHex(
        /* [in] */ const String& hexStr,
        /* [out] */ IWifiSsid** wifiSsid);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CWIFISSIDHELPER_H__
