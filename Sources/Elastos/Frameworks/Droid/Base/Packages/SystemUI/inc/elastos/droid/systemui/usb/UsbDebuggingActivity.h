//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
{
private:
    class InnerListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ UsbDebuggingActivity* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    private:
        UsbDebuggingActivity* mHost;
    };

    class UsbDisconnectedReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("UsbDebuggingActivity::UsbDisconnectedReceiver")

        UsbDisconnectedReceiver(
            /* [in] */ IActivity* activity,
            /* [in] */ UsbDebuggingActivity* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* content,
            /* [in] */ IIntent* intent);

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