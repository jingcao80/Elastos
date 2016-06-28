#ifndef  __ELASTOS_DROID_LAUNCHER2_CDRAGLAYERLAYOUTPARAMS_H__
#define  __ELASTOS_DROID_LAUNCHER2_CDRAGLAYERLAYOUTPARAMS_H__

#include "_Elastos_Droid_Launcher2_CDragLayerLayoutParams.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/DragLayer.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CDragLayerLayoutParams)
    , public DragLayer::DragLayerLayoutParams
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CDRAGLAYERLAYOUTPARAMS_H__