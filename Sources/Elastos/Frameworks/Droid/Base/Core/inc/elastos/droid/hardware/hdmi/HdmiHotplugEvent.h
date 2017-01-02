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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMIHOTPLUGEVENT_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMIHOTPLUGEVENT_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiHotplugEvent
    : public Object
    , public IHdmiHotplugEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    HdmiHotplugEvent();

    virtual ~HdmiHotplugEvent() {}

    CARAPI constructor();

    /**
     * Constructor.
     *
     * <p>Marked as hidden so only system can create the instance.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 port,
        /* [in] */ Boolean connected);

    /**
     * Return the port number for which the event occurred.
     *
     * @return port number
     */
    CARAPI GetPort(
        /* [out] */ Int32* port);

    /**
     * Return the connection status associated with this event
     *
     * @return true if the device gets connected; otherwise false
     */
    CARAPI IsConnected(
        /* [out] */ Boolean* port);

    /**
     * Flatten this object in to a Parcel.
     *
     * @param dest The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     *        May be 0 or {@link Parcelable#PARCELABLE_WRITE_RETURN_VALUE}.
     */
    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // public static final Parcelable.Creator<HdmiHotplugEvent> CREATOR
    //         = new Parcelable.Creator<HdmiHotplugEvent>() {
    //    /**
    //     * Rebuild a {@link HdmiHotplugEvent} previously stored with
    //     * {@link Parcelable#writeToParcel(Parcel, int)}.
    //     *
    //     * @param p {@link HdmiHotplugEvent} object to read the Rating from
    //     * @return a new {@link HdmiHotplugEvent} created from the data in the parcel
    //     */
    //     public HdmiHotplugEvent createFromParcel(Parcel p) {
    //         int port = p.readInt();
    //         boolean connected = p.readByte() == 1;
    //         return new HdmiHotplugEvent(port, connected);
    //     }
    //     public HdmiHotplugEvent[] newArray(int size) {
    //         return new HdmiHotplugEvent[size];
    //     }
    // };

private:
    Int32 mPort;
    Boolean mConnected;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMIHOTPLUGEVENT_H__
