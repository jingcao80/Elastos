
#ifndef __ORG_ALLJOYN_BUS_CPROPERTY_CHANGED_EMITTER_H__
#define __ORG_ALLJOYN_BUS_CPROPERTY_CHANGED_EMITTER_H__

#include "_Org_Alljoyn_Bus_CPropertyChangedEmitter.h"
#include "org/alljoyn/bus/PropertyChangedEmitter.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CPropertyChangedEmitter)
    , public PropertyChangedEmitter
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CPROPERTY_CHANGED_EMITTER_H__
