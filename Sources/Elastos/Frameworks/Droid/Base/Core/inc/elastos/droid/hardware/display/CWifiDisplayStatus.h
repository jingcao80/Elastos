
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAY_STATUS__H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAY_STATUS__H__

#include "_Elastos_Droid_Hardware_Display_CWifiDisplayStatus.h"
#include "elastos/droid/hardware/display/WifiDisplayStatus.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CarClass(CWifiDisplayStatus) , public WifiDisplayStatus
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAY_STATUS__H__
