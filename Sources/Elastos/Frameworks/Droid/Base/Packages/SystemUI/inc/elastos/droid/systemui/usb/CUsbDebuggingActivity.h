#ifndef __ELASTOS_DROID_SYSTEMUI_USB_CUSBDEBUGGINGACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_CUSBDEBUGGINGACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Usb_CUsbDebuggingActivity.h"
#include "elastos/droid/systemui/usb/UsbDebuggingActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Usb{

CarClass(CUsbDebuggingActivity)
    , public UsbDebuggingActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_USB_CUSBDEBUGGINGACTIVITY_H__
