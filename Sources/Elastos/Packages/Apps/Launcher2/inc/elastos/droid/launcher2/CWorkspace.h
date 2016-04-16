
#ifndef  __ELASTOS_DROID_LAUNCHER2_CWORKSPACE_H__
#define  __ELASTOS_DROID_LAUNCHER2_CWORKSPACE_H__

#include "_Elastos_Droid_Launcher2_CWorkspace.h"
#include "elastos/droid/launcher2/Workspace.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CWorkspace)
    , public Workspace
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CWORKSPACE_H__
