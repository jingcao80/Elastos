
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAYSESSIONINFO_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAYSESSIONINFO_H__

#include "_Elastos_Droid_Hardware_Display_CWifiDisplaySessionInfo.h"
#include "elastos/droid/hardware/display/WifiDisplaySessionInfo.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CarClass(CWifiDisplaySessionInfo) , public WifiDisplaySessionInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Display
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_DISPLAY_CWIFIDISPLAYSESSIONINFO_H__
