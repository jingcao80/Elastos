
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBENDPOINT_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBENDPOINT_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbEndpoint.h"
#include "elastos/droid/hardware/usb/UsbEndpoint.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbEndpoint)
    , public UsbEndpoint
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBENDPOINT_H__
