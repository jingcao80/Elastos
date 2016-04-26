#ifndef  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERMODEL_H__
#define  __ELASTOS_DROID_LAUNCHER2_CLAUNCHERMODEL_H__

#include "_Elastos_Droid_Launcher2_CLauncherModel.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/LauncherModel.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Maintains in-memory state of the Launcher. It is expected that there should be only one
 * LauncherModel object held in a static. Also provide APIs for updating the database state
 * for the Launcher.
 */
CarClass(CLauncherModel)
    , public LauncherModel
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHERMODEL_H__