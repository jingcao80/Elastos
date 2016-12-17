
#ifndef __ORG_ALLJOYN_BUS_CABOUTPROXY_H__
#define __ORG_ALLJOYN_BUS_CABOUTPROXY_H__

#include "_Org_Alljoyn_Bus_CAboutProxy.h"
#include "org/alljoyn/bus/AboutProxy.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CAboutProxy)
    , public AboutProxy
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CABOUTPROXY_H__
