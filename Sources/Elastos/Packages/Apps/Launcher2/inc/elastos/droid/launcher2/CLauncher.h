#ifndef  __ELASTOS_DROID_LAUNCHER2_CLAUNCHER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CLAUNCHER_H__

#include "_Elastos_Droid_Launcher2_CLauncher.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/Launcher.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncher)
    , public Launcher
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHER_H__