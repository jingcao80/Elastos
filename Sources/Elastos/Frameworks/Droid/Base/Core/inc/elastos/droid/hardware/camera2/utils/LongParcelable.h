
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_LONGPARCELABLE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_LONGPARCELABLE_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class LongParcelable
    : public Object
    , public ILongParcelable
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    LongParcelable() {}

    virtual ~LongParcelable() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 number);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetNumber(
        /* [out] */ Int64* number);

    CARAPI SetNumber(
        /* [in] */ Int64 number);

    // public static final Parcelable.Creator<LongParcelable> CREATOR =
    //         new Parcelable.Creator<LongParcelable>() {
    //     @Override
    //     public LongParcelable createFromParcel(Parcel in) {
    //         return new LongParcelable(in);
    //     }

    //     @Override
    //     public LongParcelable[] newArray(int size) {
    //         return new LongParcelable[size];
    //     }
    // };

private:
    CARAPI constructor(
        /* [in] */ IParcel* _in);

private:
    Int64 mNumber;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_LONGPARCELABLE_H__
