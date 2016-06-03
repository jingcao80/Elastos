
#ifndef __ELASTOS_DROID_COMMANDS_PM_CPM_H__
#define __ELASTOS_DROID_COMMANDS_PM_CPM_H__

#include "_Elastos_Droid_Commands_Pm_CPm.h"
#include "Pm.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Pm {

CarClass(CPm)
    , public Pm
{
public:
    CAR_SINGLETON_DECL();
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_PM_CPM_H__
