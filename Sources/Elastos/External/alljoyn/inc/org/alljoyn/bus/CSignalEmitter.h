
#ifndef __ORG_ALLJOYN_BUS_CSIGNALEMITTER_H__
#define __ORG_ALLJOYN_BUS_CSIGNALEMITTER_H__

#include "_Org_Alljoyn_Bus_CSignalEmitter.h"
#include "org/alljoyn/bus/SignalEmitter.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CarClass(CSignalEmitter)
    , public SignalEmitter
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_CSIGNALEMITTER_H__
