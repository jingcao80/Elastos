
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBREQUEST_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBREQUEST_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbRequest.h"
#include "elastos/droid/hardware/usb/UsbRequest.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbRequest)
    , public UsbRequest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBREQUEST_H__
