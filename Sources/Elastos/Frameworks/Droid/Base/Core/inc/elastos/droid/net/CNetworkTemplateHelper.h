
#ifndef __ELASTOS_DROID_NET_CNETWORKTEMPLATEHELPER_H__
#define __ELASTOS_DROID_NET_CNETWORKTEMPLATEHELPER_H__

#include "_Elastos_Droid_Net_CNetworkTemplateHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Template definition used to generically match {@link NetworkIdentity},
 * usually when collecting statistics.
 *
 * @hide
 */
CarClass(CNetworkTemplateHelper)
    , public Singleton
    , public INetworkTemplateHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ForceAllNetworkTypes();

    CARAPI BuildTemplateMobileAll(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateMobile3gLower(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateMobile4g(
        /* [in] */ const String& subscriberId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateMobileWildcard(
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateWifiWildcard(
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateWifi(
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateWifi(
        /* [in] */ const String& networkId,
        /* [out] */ INetworkTemplate** result);

    CARAPI BuildTemplateEthernet(
        /* [out] */ INetworkTemplate** result);
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CNETWORKTEMPLATEHELPER_H__
