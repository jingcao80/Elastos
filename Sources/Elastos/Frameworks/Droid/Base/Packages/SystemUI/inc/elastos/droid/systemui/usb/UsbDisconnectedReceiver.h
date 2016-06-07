
#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBDISCONNECTEDRECEIVER_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBDISCONNECTEDRECEIVER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Hardware::Usb::IUsbDevice;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

// This class is used to close UsbPermissionsActivity and UsbResolverActivity
// if their device/accessory is disconnected while the dialog is still open
class UsbDisconnectedReceiver
    : public BroadcastReceiver
{
public:
    TO_STRING_IMPL("UsbDisconnectedReceiver")

    UsbDisconnectedReceiver(
        /* [in] */ IActivity* activity,
        /* [in] */ IUsbDevice* device);

    UsbDisconnectedReceiver(
        /* [in] */ IActivity* activity,
        /* [in] */ IUsbAccessory* accessory);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    AutoPtr<IActivity> mActivity;
    AutoPtr<IUsbDevice> mDevice;
    AutoPtr<IUsbAccessory> mAccessory;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBDISCONNECTEDRECEIVER_H__