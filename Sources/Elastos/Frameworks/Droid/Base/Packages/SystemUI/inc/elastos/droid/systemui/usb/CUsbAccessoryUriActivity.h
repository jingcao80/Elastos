#ifndef __ELASTOS_DROID_SYSTEMUI_USB_CUSBACCESSORYURIACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_CUSBACCESSORYURIACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Usb_CUsbAccessoryUriActivity.h"
#include "elastos/droid/systemui/usb/UsbAccessoryUriActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Usb{

CarClass(CUsbAccessoryUriActivity)
    , public UsbAccessoryUriActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_USB_CUSBACCESSORYURIACTIVITY_H__
