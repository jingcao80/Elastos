#ifndef  __ELASTOS_DROID_LAUNCHER2_CUNINSTALLSHORTCUTRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CUNINSTALLSHORTCUTRECEIVER_H__

#include "_Elastos_Droid_Launcher2_CUninstallShortcutReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/UninstallShortcutReceiver.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CUninstallShortcutReceiver)
    , public UninstallShortcutReceiver
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CUNINSTALLSHORTCUTRECEIVER_H__