
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGEAP_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGEAP_H__

#include "_Elastos_Droid_Wifi_CWifiEnterpriseConfigEap.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiEnterpriseConfigEap)
    , public Singleton
    , public IWifiEnterpriseConfigEap
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CWifiEnterpriseConfigEap();

    /** @hide */
    CARAPI GetStrings(
        /* [out, callee] */ ArrayOf<String>** result);

private:
    AutoPtr< ArrayOf<String> > mStrings;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFIENTERPRISECONFIGEAP_H__
