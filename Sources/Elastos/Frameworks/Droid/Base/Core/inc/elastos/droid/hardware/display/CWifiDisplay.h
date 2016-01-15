
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAY_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAY_H__

#include "_Elastos_Droid_Hardware_Display_CWifiDisplay.h"
#include "elastos/droid/hardware/display/WifiDisplay.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CarClass(CWifiDisplay) , public WifiDisplay
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAY_H__
