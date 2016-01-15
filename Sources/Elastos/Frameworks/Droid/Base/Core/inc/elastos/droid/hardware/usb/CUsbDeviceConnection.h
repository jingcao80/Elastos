
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICECONNECTION_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICECONNECTION_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbDeviceConnection.h"
#include "elastos/droid/hardware/usb/UsbDeviceConnection.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbDeviceConnection)
    , public UsbDeviceConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICECONNECTION_H__
