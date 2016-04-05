#ifndef  __ELASTOS_DROID_LAUNCHER2_CCELLLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_CCELLLAYOUT_H__

#include "_Elastos_Droid_Launcher2_CCellLayout.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/CellLayout.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CCellLayout)
    , public CellLayout
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CCELLLAYOUT_H__