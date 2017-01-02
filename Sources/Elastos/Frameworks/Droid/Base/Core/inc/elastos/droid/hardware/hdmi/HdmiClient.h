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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMICLIENT_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMICLIENT_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Hdmi::IHdmiDeviceInfo;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManagerVendorCommandListener;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiClient
    : public Object
    , public IHdmiClient
{
private:
    class MyListener
        : public Object
        , public IIHdmiVendorCommandListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ IHdmiControlManagerVendorCommandListener* listener);

        // @Override
        CARAPI OnReceived(
            /* [in] */ Int32 srcAddress,
            /* [in] */ ArrayOf<Byte>* params,
            /* [in] */ Boolean hasVendorId);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiControlManagerVendorCommandListener> mListener;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiClient();

    virtual ~HdmiClient() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IIHdmiControlService* service);

    /**
     * Returns the active source information.
     *
     * @return {@link HdmiDeviceInfo} object that describes the active source
     *         or active routing path
     */
    CARAPI GetActiveSource(
        /* [out] */ IHdmiDeviceInfo** source);

    /**
     * Send a key event to other logical device.
     *
     * @param keyCode key code to send. Defined in {@link android.view.KeyEvent}.
     * @param isPressed true if this is key press event
     */
    CARAPI SendKeyEvent(
        /* [in] */ Int32 keyCode,
        /* [in] */ Boolean isPressed);

    /**
     * Send vendor-specific command.
     *
     * @param targetAddress address of the target device
     * @param params vendor-specific parameter. For &lt;Vendor Command With ID&gt; do not
     *               include the first 3 bytes (vendor ID).
     * @param hasVendorId {@code true} if the command type will be &lt;Vendor Command With ID&gt;.
     *                    {@code false} if the command will be &lt;Vendor Command&gt;
     */
    CARAPI SendVendorCommand(
        /* [in] */ Int32 targetAddress,
        /* [in] */ ArrayOf<Byte>* params,
        /* [in] */ Boolean hasVendorId);

    /**
     * Add a listener used to receive incoming vendor-specific command.
     *
     * @param listener listener object
     */
    CARAPI AddVendorCommandListener(
        /* [in] */ IHdmiControlManagerVendorCommandListener* listener);

    virtual CARAPI GetDeviceType(
        /* [out] */ Int32* type) = 0;

private:
    static CARAPI_(AutoPtr<IIHdmiVendorCommandListener>) GetListenerWrapper(
        /* [in] */ IHdmiControlManagerVendorCommandListener* listener);

protected:
    AutoPtr<IIHdmiControlService> mService;

private:
    static const String TAG;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMICLIENT_H__
