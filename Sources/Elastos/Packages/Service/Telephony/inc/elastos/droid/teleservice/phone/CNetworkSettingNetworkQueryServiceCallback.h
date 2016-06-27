#ifndef  __ELASTOS_DROID_PHONE_CNETWORKSETTINGNETWORKQUERYSERVICECALLBACK_H__
#define  __ELASTOS_DROID_PHONE_CNETWORKSETTINGNETWORKQUERYSERVICECALLBACK_H__

#include "_Elastos_Droid_TeleService_Phone_CNetworkSettingNetworkQueryServiceCallback.h"
#include "elastos/droid/teleservice/phone/CNetworkSetting.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::TeleService::Phone::CNetworkSetting;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CNetworkSettingNetworkQueryServiceCallback)
    , public CNetworkSetting::MyNetworkQueryServiceCallback
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CNETWORKSETTINGNETWORKQUERYSERVICECALLBACK_H__