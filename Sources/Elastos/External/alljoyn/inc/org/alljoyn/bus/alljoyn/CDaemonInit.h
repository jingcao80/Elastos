
#ifndef __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__
#define __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__

#include "_Org_Alljoyn_Bus_Alljoyn_CDaemonInit.h"
#include "org/alljoyn/bus/alljoyn/DaemonInit.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

CarClass(CDaemonInit)
    , public DaemonInit
    , public Singleton
{
public:
    CAR_SINGLETON_DECL();
};

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ALLJOYN_CDAEMONINIT_H__
