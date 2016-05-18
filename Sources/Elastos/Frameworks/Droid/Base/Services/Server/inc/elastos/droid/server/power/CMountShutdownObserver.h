
#ifndef __ELASTOS_DROID_SERVER_POWER_CMOUNTSHUTDOWNOBSERVER_H__
#define __ELASTOS_DROID_SERVER_POWER_CMOUNTSHUTDOWNOBSERVER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_Power_CMountShutdownObserver.h"
#include "elastos/droid/server/power/ShutdownThread.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Power {

CarClass(CMountShutdownObserver)
    , public ShutdownThread::MountShutdownObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Power
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_POWER_CMOUNTSHUTDOWNOBSERVER_H__
