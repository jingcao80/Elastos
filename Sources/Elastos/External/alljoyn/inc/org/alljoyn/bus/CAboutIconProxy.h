
#ifndef __ORG_ALLJOYN_BUS_CABOUTICONPROXY_H__
#define __ORG_ALLJOYN_BUS_CABOUTICONPROXY_H__

#include "_Org_Alljoyn_Bus_CAboutIconProxy.h"
#include "org/alljoyn/bus/AboutIconProxy.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CAboutIconProxy)
    , public AboutIconProxy
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CABOUTICONPROXY_H__
