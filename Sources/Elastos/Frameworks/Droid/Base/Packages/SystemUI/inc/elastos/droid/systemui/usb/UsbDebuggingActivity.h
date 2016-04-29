
#ifndef __ELASTOS_DROID_SYSTEMUI_USB_USBDEBUGGINGACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_USB_USBDEBUGGINGACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/internal/app/AlertActivity.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::App::AlertActivity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ICheckBox;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Usb {

class UsbDebuggingActivity
    : public AlertActivity
    , public IDialogInterfaceOnClickListener
{
private:
    class UsbDisconnectedReceiver
        : public BroadcastReceiver
    {
    public:
        UsbDisconnectedReceiver(
            /* [in] */ IActivity* activity,
            /* [in] */ UsbDebuggingActivity* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* content,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            *str = String("UsbDebuggingActivity.UsbDisconnectedReceiver");
            return NOERROR;
        }

    private:
        UsbDebuggingActivity* mHost;
        AutoPtr<IActivity> mActivity;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

protected:
    // @Override
    CARAPI OnStop();

private:
    const static String TAG;

    AutoPtr<ICheckBox> mAlwaysAllow;
    AutoPtr<UsbDisconnectedReceiver> mDisconnectedReceiver;
    String mKey;
};

} // namespace Usb
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_USB_USBDEBUGGINGACTIVITY_H__