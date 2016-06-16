#ifndef  __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICENETWORKQUERYSERVICE_H__
#define  __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICENETWORKQUERYSERVICE_H__

#include "elastos/droid/telephony/phone/CNetworkQueryService.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CarClass(CNetworkQueryServiceNetworkQueryService)
    , public CNetworkQueryService::MyNetworkQueryService
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CNETWORKQUERYSERVICENETWORKQUERYSERVICE_H__