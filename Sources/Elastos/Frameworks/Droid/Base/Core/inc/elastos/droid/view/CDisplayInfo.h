
#ifndef __ELASTOS_DROID_VIEW_CDISPLAYINFO_H__
#define __ELASTOS_DROID_VIEW_CDISPLAYINFO_H__

#include "_Elastos_Droid_View_CDisplayInfo.h"
#include "elastos/droid/view/DisplayInfo.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDisplayInfo)
    , public DisplayInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CDISPLAYINFO_H__
