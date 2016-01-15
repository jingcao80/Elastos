
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGPHASE2_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGPHASE2_H__

#include "_Elastos_Droid_Wifi_CWifiEnterpriseConfigPhase2.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiEnterpriseConfigPhase2)
    , public Singleton
    , public IWifiEnterpriseConfigPhase2
{
    friend class CWifiEnterpriseConfig;
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CWifiEnterpriseConfigPhase2();

    /** @hide */
    CARAPI GetStrings(
        /* [out, callee] */ ArrayOf<String>** result);

private:
    static const String PREFIX;
    AutoPtr< ArrayOf<String> > mStrings;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGPHASE2_H__
