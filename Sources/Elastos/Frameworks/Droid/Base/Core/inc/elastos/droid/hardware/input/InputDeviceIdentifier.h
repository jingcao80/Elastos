
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
