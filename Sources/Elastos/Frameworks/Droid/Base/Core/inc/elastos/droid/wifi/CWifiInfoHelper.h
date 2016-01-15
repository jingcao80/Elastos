
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIINFOHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIINFOHELPER_H__

#include "_Elastos_Droid_Wifi_CWifiInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::NetworkInfoDetailedState;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiInfoHelper)
    , public Singleton
    , public IWifiInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDetailedStateOf(
        /* [in] */ ISupplicantState* suppState,
        /* [out] */ NetworkInfoDetailedState* detailedState);

    CARAPI RemoveDoubleQuotes(
        /* [in] */ const String& str,
        /* [out] */ String* value);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_CWIFIINFOHELPER_H__
