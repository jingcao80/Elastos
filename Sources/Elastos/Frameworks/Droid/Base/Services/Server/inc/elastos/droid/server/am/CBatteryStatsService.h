#ifndef __ELASTOS_DROID_SERVER_AM_CBATTERYSTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_AM_CBATTERYSTATSSERVICE_H__

#include "_Elastos_Droid_Server_Am_CBatteryStatsService.h"
#include "elastos/droid/server/am/BatteryStatsService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CBatteryStatsService)
    , public BatteryStatsService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CBATTERYSTATSSERVICE_H__
