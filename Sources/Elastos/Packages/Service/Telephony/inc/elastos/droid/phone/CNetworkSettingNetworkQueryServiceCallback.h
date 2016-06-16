#ifndef  __ELASTOS_DROID_PHONE_CNETWORKSETTINGNETWORKQUERYSERVICECALLBACK_H__
#define  __ELASTOS_DROID_PHONE_CNETWORKSETTINGNETWORKQUERYSERVICECALLBACK_H__

#include "elastos/droid/telephony/phone/CNetworkSetting.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CNetworkSettingNetworkQueryServiceCallback)
    , public CNetworkSetting::MyNetworkQueryServiceCallback
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CNETWORKSETTINGNETWORKQUERYSERVICECALLBACK_H__