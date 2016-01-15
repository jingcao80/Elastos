
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBINTERFACE_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBINTERFACE_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbInterface.h"
#include "elastos/droid/hardware/usb/UsbInterface.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbInterface)
    , public UsbInterface
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBINTERFACE_H__
