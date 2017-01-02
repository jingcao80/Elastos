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

#ifndef __ELASTOS_DROID_HARDWARE_HDMI_HDMIPORTINFO_H__
#define __ELASTOS_DROID_HARDWARE_HDMI_HDMIPORTINFO_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

class HdmiPortInfo
    : public Object
    , public IHdmiPortInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    HdmiPortInfo();

    virtual ~HdmiPortInfo() {}

    CARAPI constructor();

    /**
     * Constructor.
     *
     * @param id identifier assigned to each port. 1 for HDMI port 1
     * @param type HDMI port input/output type
     * @param address physical address of the port
     * @param cec {@code true} if HDMI-CEC is supported on the port
     * @param mhl {@code true} if MHL is supported on the port
     * @param arc {@code true} if audio return channel is supported on the port
     */
    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ Int32 type,
        /* [in] */ Int32 address,
        /* [in] */ Boolean cec,
        /* [in] */ Boolean mhl,
        /* [in] */ Boolean arc);

    /**
     * Returns the port id.
     *
     * @return port id
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * Returns the port type.
     *
     * @return port type
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * Returns the port address.
     *
     * @return port address
     */
    CARAPI GetAddress(
        /* [out] */ Int32* address);

    /**
     * Returns {@code true} if the port supports HDMI-CEC signaling.
     *
     * @return {@code true} if the port supports HDMI-CEC signaling.
     */
    CARAPI IsCecSupported(
        /* [out] */ Boolean* result);

    /**
     * Returns {@code true} if the port supports MHL signaling.
     *
     * @return {@code true} if the port supports MHL signaling.
     */
    CARAPI IsMhlSupported(
        /* [out] */ Boolean* result);

    /**
     * Returns {@code true} if the port supports audio return channel.
     *
     * @return {@code true} if the port supports audio return channel
     */
    CARAPI IsArcSupported(
        /* [out] */ Boolean* result);

    /**
     * Serialize this object into a {@link Parcel}.
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

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // /**
    //  * A helper class to deserialize {@link HdmiPortInfo} for a parcel.
    //  */
    // public static final Parcelable.Creator<HdmiPortInfo> CREATOR =
    //         new Parcelable.Creator<HdmiPortInfo>() {
    //             @Override
    //             public HdmiPortInfo createFromParcel(Parcel source) {
    //                 int id = source.readInt();
    //                 int type = source.readInt();
    //                 int address = source.readInt();
    //                 boolean cec = (source.readInt() == 1);
    //                 boolean arc = (source.readInt() == 1);
    //                 boolean mhl = (source.readInt() == 1);
    //                 return new HdmiPortInfo(id, type, address, cec, arc, mhl);
    //             }

    //             @Override
    //             public HdmiPortInfo[] newArray(int size) {
    //                 return new HdmiPortInfo[size];
    //             }
    //         };

private:
    Int32 mId;
    Int32 mType;
    Int32 mAddress;
    Boolean mCecSupported;
    Boolean mArcSupported;
    Boolean mMhlSupported;
};

} //Hdmi
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_HDMI_HDMIPORTINFO_H__
