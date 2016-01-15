
#ifndef __ELASTOS_DROID_VIEW_CDISPLAY_H__
#define __ELASTOS_DROID_VIEW_CDISPLAY_H__

#include "_Elastos_Droid_View_CDisplay.h"
#include "elastos/droid/view/Display.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CDisplay)
    , public Display
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CDISPLAY_H__
