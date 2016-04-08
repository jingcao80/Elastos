#ifndef  __ELASTOS_DROID_LAUNCHER2_CFOLDER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CFOLDER_H__

#include "_Elastos_Droid_Launcher2_CFolder.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/Folder.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CFolder)
    , public Folder
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CFOLDER_H__