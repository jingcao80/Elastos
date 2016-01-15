
#ifndef __ELASTOS_DROID_VIEW_CPHYSICALDISPLAYINFO_H__
#define __ELASTOS_DROID_VIEW_CPHYSICALDISPLAYINFO_H__

#include "_Elastos_Droid_View_CPhysicalDisplayInfo.h"
#include "elastos/droid/view/PhysicalDisplayInfo.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CPhysicalDisplayInfo)
    , public PhysicalDisplayInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CPHYSICALDISPLAYINFO_H__
