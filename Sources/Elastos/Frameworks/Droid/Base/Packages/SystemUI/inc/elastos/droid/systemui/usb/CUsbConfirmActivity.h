#ifndef __ELASTOS_DROID_SYSTEMUI_USB_CUSBCONFIRMACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_CUSBCONFIRMACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Usb_CUsbConfirmActivity.h"
#include "elastos/droid/systemui/usb/UsbConfirmActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Usb{

CarClass(CUsbConfirmActivity)
    , public UsbConfirmActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_USB_CUSBCONFIRMACTIVITY_H__
