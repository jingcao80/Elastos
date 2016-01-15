
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBMANAGER_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbManager.h"
#include "elastos/droid/hardware/usb/UsbManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbManager)
    , public UsbManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBMANAGER_H__
