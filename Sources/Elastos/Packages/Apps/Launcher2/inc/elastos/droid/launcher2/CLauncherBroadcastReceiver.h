#ifndef  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERBROADCASTRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERBROADCASTRECEIVER_H__

#include "_Elastos_Droid_Launcher2_CLauncherBroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/Launcher.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncherBroadcastReceiver)
    , public Launcher::MyBroadcastReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHERBROADCASTRECEIVER_H__