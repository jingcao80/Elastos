#ifndef  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERCLOSESYSTEMDIALOGSINTENTRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERCLOSESYSTEMDIALOGSINTENTRECEIVER_H__

#include "_Elastos_Droid_Launcher2_CLauncherCloseSystemDialogsIntentReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/Launcher.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncherCloseSystemDialogsIntentReceiver)
    , public Launcher::CloseSystemDialogsIntentReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHERCLOSESYSTEMDIALOGSINTENTRECEIVER_H__