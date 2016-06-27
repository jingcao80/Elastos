#ifndef  __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICENETWORKQUERYSERVICE_H__
#define  __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICENETWORKQUERYSERVICE_H__

#include "_Elastos_Droid_TeleService_Phone_CNetworkQueryServiceNetworkQueryService.h"
#include "elastos/droid/teleservice/phone/CNetworkQueryService.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::TeleService::Phone::CNetworkQueryService;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CNetworkQueryServiceNetworkQueryService)
    , public CNetworkQueryService::MyNetworkQueryService
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICENETWORKQUERYSERVICE_H__