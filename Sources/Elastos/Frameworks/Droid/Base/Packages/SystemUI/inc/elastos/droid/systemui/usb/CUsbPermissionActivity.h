#ifndef __ELASTOS_DROID_SYSTEMUI_USB_CUSBPERMISSIONACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_CUSBPERMISSIONACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Usb_CUsbPermissionActivity.h"
#include "elastos/droid/systemui/usb/UsbPermissionActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Usb{

CarClass(CUsbPermissionActivity)
    , public UsbPermissionActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_USB_CUSBPERMISSIONACTIVITY_H__
