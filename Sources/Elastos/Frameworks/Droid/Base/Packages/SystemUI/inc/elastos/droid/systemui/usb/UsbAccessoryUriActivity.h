
#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBACCESSORYURIACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBACCESSORYURIACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/app/AlertActivity.h"

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Internal::App::AlertActivity;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

/**
 * If the attached USB accessory has a URL associated with it, and that URL is valid,
 * show this dialog to the user to allow them to optionally visit that URL for more
 * information or software downloads.
 * Otherwise (no valid URL) this activity does nothing at all, finishing immediately.
 */
class UsbAccessoryUriActivity
    : public AlertActivity
{
private:
    class InnerListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ UsbAccessoryUriActivity* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    private:
        UsbAccessoryUriActivity* mHost;
    };
public:

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    const static String TAG;

    AutoPtr<IUsbAccessory> mAccessory;
    AutoPtr<IUri> mUri;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBACCESSORYURIACTIVITY_H__