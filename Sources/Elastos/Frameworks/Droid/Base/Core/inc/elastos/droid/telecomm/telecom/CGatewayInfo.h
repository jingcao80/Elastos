#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CGATEWAYINFO_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CGATEWAYINFO_H__

#include "_Elastos_Droid_Telecomm_Telecom_CGatewayInfo.h"
#include "elastos/droid/telecomm/telecom/GatewayInfo.h"

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

CarClass(CGatewayInfo)
    , public GatewayInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CGATEWAYINFO_H__