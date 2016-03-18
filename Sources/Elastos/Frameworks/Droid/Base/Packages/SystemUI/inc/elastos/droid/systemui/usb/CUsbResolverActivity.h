#ifndef __ELASTOS_DROID_SYSTEMUI_USB_CUSBRESOLVERACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_CUSBRESOLVERACTIVITY_H__

#include "_Elastos_Droid_SystemUI_Usb_CUsbResolverActivity.h"
#include "elastos/droid/systemui/usb/UsbResolverActivity.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Usb{

CarClass(CUsbResolverActivity)
    , public UsbResolverActivity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_USB_CUSBRESOLVERACTIVITY_H__
