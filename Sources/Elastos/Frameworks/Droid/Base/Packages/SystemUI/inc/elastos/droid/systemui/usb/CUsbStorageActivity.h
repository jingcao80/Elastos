#ifndef __ELASTOS_DROID_SYSTEMUI_USB_CUSBSTORAGEACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_CUSBSTORAGEACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Usb_CUsbStorageActivity.h"
#include "elastos/droid/systemui/usb/UsbStorageActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Usb{

CarClass(CUsbStorageActivity)
    , public UsbStorageActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_USB_CUSBSTORAGEACTIVITY_H__
