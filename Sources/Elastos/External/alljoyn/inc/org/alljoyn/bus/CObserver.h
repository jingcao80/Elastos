
#ifndef __ORG_ALLJOYN_BUS_COBSERVER_H__
#define __ORG_ALLJOYN_BUS_COBSERVER_H__

#include "_Org_Alljoyn_Bus_CObserver.h"
#include "org/alljoyn/bus/Observer.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CObserver)
    , public Observer
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_COBSERVER_H__
