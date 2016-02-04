
#ifndef __ELASTOS_DROID_SERVER_AM_CAPPTASKIMPL_H__
#define __ELASTOS_DROID_SERVER_AM_CAPPTASKIMPL_H__

#include "_Elastos_Droid_Server_Am_CAppTaskImpl.h"
#include <elastos/droid/server/am/CActivityManagerService.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CAppTaskImpl)
    , public CActivityManagerService::AppTaskImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_CAPPTASKIMPL_H__
