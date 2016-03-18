#ifndef __ELASTOS_DROID_SERVER_AM_CACTIVITYCONTAINER_H__
#define __ELASTOS_DROID_SERVER_AM_CACTIVITYCONTAINER_H__

#include "_Elastos_Droid_Server_Am_CActivityContainer.h"
#include "elastos/droid/server/am/ActivityContainer.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CarClass(CActivityContainer)
    , public ActivityContainer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CACTIVITYCONTAINER_H__
