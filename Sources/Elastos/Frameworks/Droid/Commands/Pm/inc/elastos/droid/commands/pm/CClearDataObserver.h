
#ifndef __ELASTOS_DROID_COMMANDS_PM_CCLEARDATAOBSERVER_H__
#define __ELASTOS_DROID_COMMANDS_PM_CCLEARDATAOBSERVER_H__

#include "_Elastos_Droid_Commands_Pm_CClearDataObserver.h"
#include "Pm.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Pm {

CarClass(CClearDataObserver)
    , public Pm::ClearDataObserver
{
public:
    CAR_OBJECT_DECL()
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_PM_CCLEARDATAOBSERVER_H__
