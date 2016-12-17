
#ifndef __ORG_ALLJOYN_BUS_CABOUTICONOBJ_H__
#define __ORG_ALLJOYN_BUS_CABOUTICONOBJ_H__

#include "_Org_Alljoyn_Bus_CAboutIconObj.h"
#include "org/alljoyn/bus/AboutIconObj.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CAboutIconObj)
    , public AboutIconObj
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CABOUTICONOBJ_H__
