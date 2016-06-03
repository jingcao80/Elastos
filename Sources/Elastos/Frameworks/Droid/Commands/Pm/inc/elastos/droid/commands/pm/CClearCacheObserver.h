
#ifndef __ELASTOS_DROID_COMMANDS_PM_CCLEARCACHEOBSERVER_H__
#define __ELASTOS_DROID_COMMANDS_PM_CCLEARCACHEOBSERVER_H__

#include "_Elastos_Droid_Commands_Pm_CClearCacheObserver.h"
#include "Pm.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Pm {

CarClass(CClearCacheObserver)
    , public Pm::ClearCacheObserver
{
public:
    CAR_OBJECT_DECL()
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_PM_CCLEARCACHEOBSERVER_H__
