
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIMANAGERHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIMANAGERHELPER_H__

#include "_Elastos_Droid_Wifi_CWifiManagerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiManagerHelper)
    , public Singleton
    , public IWifiManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CalculateSignalLevel(
        /* [in] */ Int32 rssi,
        /* [in] */ Int32 numLevels,
        /* [out] */ Int32* level);

    CARAPI CompareSignalLevel(
        /* [in] */ Int32 rssiA,
        /* [in] */ Int32 rssiB,
        /* [out] */ Int32* result);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_CWIFIMANAGERHELPER_H__
