
#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBCONFIRMACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBCONFIRMACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/internal/app/AlertActivity.h"
#include "elastos/droid/systemui/usb/UsbDisconnectedReceiver.h"

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Hardware::Usb::IUsbDevice;
using Elastos::Droid::Internal::App::AlertActivity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

class UsbConfirmActivity
    : public AlertActivity
    , public IDialogInterfaceOnClickListener
    , public ICompoundButtonOnCheckedChangeListener
{
public:
    CAR_INTERFACE_DECL()

    UsbConfirmActivity();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    // @Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

protected:
    // @Override
    CARAPI OnDestroy();

private:
    const static String TAG;

    AutoPtr<ICheckBox> mAlwaysUse;
    AutoPtr<ITextView> mClearDefaultHint;
    AutoPtr<IUsbDevice> mDevice;
    AutoPtr<IUsbAccessory> mAccessory;
    AutoPtr<IResolveInfo> mResolveInfo;
    Boolean mPermissionGranted;
    AutoPtr<UsbDisconnectedReceiver> mDisconnectedReceiver;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBCONFIRMACTIVITY_H__