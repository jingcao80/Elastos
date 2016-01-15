
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICE_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICE_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbDevice.h"
#include "elastos/droid/hardware/usb/UsbDevice.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbDevice)
    , public UsbDevice
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICE_H__
