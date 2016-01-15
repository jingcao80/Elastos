
#ifndef __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICEHELPER_H__
#define __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICEHELPER_H__

#include "_Elastos_Droid_Hardware_Usb_CUsbDeviceHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Usb {

CarClass(CUsbDeviceHelper)
    , public Singleton
    , public IUsbDeviceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDeviceId(
        /* [in] */ const String& name,
        /* [out] */ Int32* id);

    CARAPI GetDeviceName(
        /* [in] */ Int32 id,
        /* [out] */ String* name);
};

} // namespace Usb
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_USB_CUSBDEVICEHELPER_H__
