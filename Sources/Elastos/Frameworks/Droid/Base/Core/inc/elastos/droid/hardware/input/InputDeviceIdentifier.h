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

#ifndef __ELASTOS_DROID_HARDWARE_INPUT_INPUTDEVICEIDENTIFIER_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_INPUTDEVICEIDENTIFIER_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

/**
 * Describes a keyboard layout.
 *
 * @hide
 */
class InputDeviceIdentifier
    : public Object
    , public IInputDeviceIdentifier
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    InputDeviceIdentifier();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& descriptor,
        /* [in] */ Int32 vendorId,
        /* [in] */ Int32 productId);

    CARAPI GetDescriptor(
        /* [out] */ String* dsp);

    CARAPI GetVendorId(
        /* [out] */ Int32* id);

    CARAPI GetProductId(
        /* [out] */ Int32* id);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    String mDescriptor;
    Int32 mVendorId;
    Int32 mProductId;


    // public static final Parcelable.Creator<InputDeviceIdentifier> CREATOR =
    //         new Parcelable.Creator<InputDeviceIdentifier>() {

    //     @Override
    //     public InputDeviceIdentifier createFromParcel(Parcel source) {
    //         return new InputDeviceIdentifier(source);
    //     }

    //     @Override
    //     public InputDeviceIdentifier[] newArray(int size) {
    //         return new InputDeviceIdentifier[size];
    //     }

    // };
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_INPUTDEVICEIDENTIFIER_H__
