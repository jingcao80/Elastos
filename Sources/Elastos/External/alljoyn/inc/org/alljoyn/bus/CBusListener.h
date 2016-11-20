
#ifndef __ORG_ALLJOYN_BUS_CBUSLISTENER_H__
#define __ORG_ALLJOYN_BUS_CBUSLISTENER_H__

#include "_Org_Alljoyn_Bus_CBusListener.h"
#include "org/alljoyn/bus/BusListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CBusListener)
    , public BusListener
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CBUSLISTENER_H__
