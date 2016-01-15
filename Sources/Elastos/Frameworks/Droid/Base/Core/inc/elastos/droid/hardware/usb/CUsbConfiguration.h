
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBCONFIGURATION_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBCONFIGURATION_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbConfiguration.h"
#include "elastos/droid/hardware/usb/UsbConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbConfiguration)
    , public UsbConfiguration
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBACCESSORY_H__
