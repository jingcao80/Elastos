#ifndef __ELASTOS_DROID_TELECOM_CGATEWAYINFO_H__
#define __ELASTOS_DROID_TELECOM_CGATEWAYINFO_H__

#include "_Elastos_Droid_Telecom_CGatewayInfo.h"
#include "elastos/droid/telecom/GatewayInfo.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CGatewayInfo)
    , public GatewayInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CGATEWAYINFO_H__