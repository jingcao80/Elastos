#ifndef  __ELASTOS_DROI_LAUNCHER2_CLAUNCHERAPPLICATION_H__
#define  __ELASTOS_DROI_LAUNCHER2_CLAUNCHERAPPLICATION_H__

#include "_Elastos_Droid_Launcher2_CLauncherApplication.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/LauncherApplication.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * We use a custom tab view to process our own focus traversals.
 */
CarClass(CLauncherApplication)
    , public LauncherApplication
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROI_LAUNCHER2_CLAUNCHERAPPLICATION_H__