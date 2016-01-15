
#ifndef __ELASTOS_DROID_HARDWARE_INPUT_TOUCHCALIBRATION_H__
#define __ELASTOS_DROID_HARDWARE_INPUT_TOUCHCALIBRATION_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Input::IKeyboardLayout;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

/**
 * Describes a keyboard layout.
 *
 * @hide
 */
class TouchCalibration
    : public Object
    , public ITouchCalibration
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create a new TouchCalibration initialized to the identity transformation.
     */
    TouchCalibration();

    /**
     * Create a new TouchCalibration from affine transformation paramters.
     * @param xScale   Influence of input x-axis value on output x-axis value.
     * @param xyMix    Influence of input y-axis value on output x-axis value.
     * @param xOffset  Constant offset to be applied to output x-axis value.
     * @param yXMix    Influence of input x-axis value on output y-axis value.
     * @param yScale   Influence of input y-axis value on output y-axis value.
     * @param yOffset  Constant offset to be applied to output y-axis value.
     */
    TouchCalibration(
        /* [in] */ Float xScale,
        /* [in] */ Float xyMix,
        /* [in] */ Float xOffset,
        /* [in] */ Float yxMix,
        /* [in] */ Float yScale,
        /* [in] */ Float yOffset);

    TouchCalibration(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetAffineTransform(
        /* [out, callee] */ ArrayOf<Float>** transform);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* another,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* code);

public:
    static const AutoPtr<ITouchCalibration> IDENTITY;// = new TouchCalibration();

    // public static final Parcelable.Creator<TouchCalibration> CREATOR
    //         = new Parcelable.Creator<TouchCalibration>() {
    //     public TouchCalibration createFromParcel(Parcel in) {
    //         return new TouchCalibration(in);
    //     }

    //     public TouchCalibration[] newArray(int size) {
    //         return new TouchCalibration[size];
    //     }
    // };

private:
    Float mXScale, mXYMix, mXOffset;
    Float mYXMix, mYScale, mYOffset;
};

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_INPUT_TOUCHCALIBRATION_H__
