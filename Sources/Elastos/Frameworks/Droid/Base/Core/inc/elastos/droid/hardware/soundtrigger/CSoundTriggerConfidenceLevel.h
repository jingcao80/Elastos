
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERCONFIDENCELEVEL_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERCONFIDENCELEVEL_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerConfidenceLevel.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerConfidenceLevel)
    , public Object
    , public ISoundTriggerConfidenceLevel
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSoundTriggerConfidenceLevel();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 confidenceLevel);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equal);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Parcelable.Creator<ConfidenceLevel> CREATOR
    //         = new Parcelable.Creator<ConfidenceLevel>() {
    //     public ConfidenceLevel createFromParcel(Parcel in) {
    //         return ConfidenceLevel.fromParcel(in);
    //     }

    //     public ConfidenceLevel[] newArray(int size) {
    //         return new ConfidenceLevel[size];
    //     }
    // };
    CARAPI GetUserId(
        /* [out] */ Int32* id);

    CARAPI GetConfidenceLevel(
        /* [out] */ Int32* level);

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerConfidenceLevel** result);

public:
    Int32 mUserId;
    Int32 mConfidenceLevel;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERCONFIDENCELEVEL_H__
