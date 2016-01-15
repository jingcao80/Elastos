
#ifndef __ELASTOS_DROID_VIEW_CINPUTDEVICE_H__
#define __ELASTOS_DROID_VIEW_CINPUTDEVICE_H__

#include "_Elastos_Droid_View_CInputDevice.h"
#include "elastos/droid/view/InputDevice.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CInputDevice)
    , public InputDevice
{
public:
    CAR_OBJECT_DECL()
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CINPUTDEVICE_H__
