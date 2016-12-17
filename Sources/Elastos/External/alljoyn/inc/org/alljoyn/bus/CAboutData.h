
#ifndef __ORG_ALLJOYN_BUS_CABOUTDATA_H__
#define __ORG_ALLJOYN_BUS_CABOUTDATA_H__

#include "_Org_Alljoyn_Bus_CAboutData.h"
#include "org/alljoyn/bus/AboutData.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CAboutData)
    , public AboutData
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CABOUTDATA_H__
