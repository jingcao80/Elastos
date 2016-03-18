#ifndef __ELASTOS_DROID_SYSTEMUI_USB_CSTORAGENOTIFICATION_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_CSTORAGENOTIFICATION_H__

#include "_Elastos_Droid_SystemUI_Usb_CStorageNotification.h"
#include "elastos/droid/systemui/usb/StorageNotification.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Usb{

CarClass(CStorageNotification)
    , public StorageNotification
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_USB_CSTORAGENOTIFICATION_H__
