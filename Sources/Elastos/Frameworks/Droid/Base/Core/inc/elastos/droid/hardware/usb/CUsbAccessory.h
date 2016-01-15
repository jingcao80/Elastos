
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBACCESSORY_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBACCESSORY_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbAccessory.h"
#include "elastos/droid/hardware/usb/UsbAccessory.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbAccessory)
    , public UsbAccessory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBACCESSORY_H__
