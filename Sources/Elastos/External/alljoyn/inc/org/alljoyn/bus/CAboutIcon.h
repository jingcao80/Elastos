
#ifndef __ORG_ALLJOYN_BUS_CABOUTICON_H__
#define __ORG_ALLJOYN_BUS_CABOUTICON_H__

#include "_Org_Alljoyn_Bus_CAboutIcon.h"
#include "org/alljoyn/bus/AboutIcon.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CAboutIcon)
    , public AboutIcon
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CABOUTICON_H__
