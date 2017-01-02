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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMICONTROLMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMICONTROLMANAGER_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiControlManager
    : public Object
    , public IHdmiControlManager
{
private:
    class MyListener
        : public Object
        , public IIHdmiHotplugEventListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MyListener(
            /* [in] */ IHdmiControlManagerHotplugEventListener* listener);

        // @Override
        CARAPI OnReceived(
            /* [in] */ IHdmiHotplugEvent* event);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IHdmiControlManagerHotplugEventListener> mListener;
    };

public:
    CAR_INTERFACE_DECL()

    HdmiControlManager();

    virtual ~HdmiControlManager() {}

    CARAPI constructor();

    /**
     * @hide - hide this constructor because it has a parameter of type
     * IHdmiControlService, which is a system private class. The right way
     * to create an instance of this class is using the factory
     * Context.getSystemService.
     */
    CARAPI constructor(
        /* [in] */ IIHdmiControlService* service);

    CARAPI GetClient(
        /* [in] */ Int32 type,
        /* [out] */ IHdmiClient** client);

    /**
     * Gets an object that represents an HDMI-CEC logical device of type playback on the system.
     *
     * <p>Used to send HDMI control messages to other devices like TV or audio amplifier through
     * HDMI bus. It is also possible to communicate with other logical devices hosted in the same
     * system if the system is configured to host more than one type of HDMI-CEC logical devices.
     *
     * @return {@link HdmiPlaybackClient} instance. {@code null} on failure.
     */
    //@Nullable
    CARAPI GetPlaybackClient(
        /* [out] */ IHdmiPlaybackClient** client);

    /**
     * Gets an object that represents an HDMI-CEC logical device of type TV on the system.
     *
     * <p>Used to send HDMI control messages to other devices and manage them through
     * HDMI bus. It is also possible to communicate with other logical devices hosted in the same
     * system if the system is configured to host more than one type of HDMI-CEC logical devices.
     *
     * @return {@link HdmiTvClient} instance. {@code null} on failure.
     */
    //@Nullable
    CARAPI GetTvClient(
        /* [out] */ IHdmiTvClient** client);

    /**
     * Adds a listener to get informed of {@link HdmiHotplugEvent}.
     *
     * <p>To stop getting the notification,
     * use {@link #removeHotplugEventListener(HotplugEventListener)}.
     *
     * @param listener {@link HotplugEventListener} instance
     * @see HdmiControlManager#removeHotplugEventListener(HotplugEventListener)
     */
    CARAPI AddHotplugEventListener(
        /* [in] */ IHdmiControlManagerHotplugEventListener* listener);

    /**
     * Removes a listener to stop getting informed of {@link HdmiHotplugEvent}.
     *
     * @param listener {@link HotplugEventListener} instance to be removed
     */
    CARAPI RemoveHotplugEventListener(
        /* [in] */ IHdmiControlManagerHotplugEventListener* listener);

private:
    CARAPI_(Boolean) HasDeviceType(
        /* [in] */ ArrayOf<Int32>* types,
        /* [in] */ Int32 type);

    CARAPI_(AutoPtr<IIHdmiHotplugEventListener>) GetHotplugEventListenerWrapper(
        /* [in] */ IHdmiControlManagerHotplugEventListener* listener);

private:
    AutoPtr<IIHdmiControlService> mService;

    // True if we have a logical device of type playback hosted in the system.
    Boolean mHasPlaybackDevice;
    // True if we have a logical device of type TV hosted in the system.
    Boolean mHasTvDevice;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMICONTROLMANAGER_H__
