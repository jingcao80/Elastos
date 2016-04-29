
#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBRESOLVERACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBRESOLVERACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/app/ResolverActivity.h"
#include "elastos/droid/systemui/usb/UsbDisconnectedReceiver.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Internal::App::ResolverActivity;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

/* Activity for choosing an application for a USB device or accessory */
class UsbResolverActivity
    : public ResolverActivity
{
public:
    CARAPI constructor();

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnIntentSelected(
        /* [in] */ IResolveInfo* ri,
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean alwaysCheck);

public:
    const static String TAG;
    const static String EXTRA_RESOLVE_INFOS;

private:
    AutoPtr<IUsbDevice> mDevice;
    AutoPtr<IUsbAccessory> mAccessory;
    AutoPtr<UsbDisconnectedReceiver> mDisconnectedReceiver;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBRESOLVERACTIVITY_H__