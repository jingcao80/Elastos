
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERSOUNDMODELEVENT_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERSOUNDMODELEVENT_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerSoundModelEvent.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerSoundModelEvent)
    , public Object
    , public ISoundTriggerSoundModelEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSoundTriggerSoundModelEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 status,
        /* [in] */ Int32 soundModelHandle,
        /* [in] */ ArrayOf<Byte>* data);

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

    // public static final Parcelable.Creator<SoundModelEvent> CREATOR
    //         = new Parcelable.Creator<SoundModelEvent>() {
    //     public SoundModelEvent createFromParcel(Parcel in) {
    //         return SoundModelEvent.fromParcel(in);
    //     }

    //     public SoundModelEvent[] newArray(int size) {
    //         return new SoundModelEvent[size];
    //     }
    // };

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerSoundModelEvent** result);

public:
    /** Status e.g {@link #SOUNDMODEL_STATUS_UPDATED} */
    Int32 mStatus;

    /** The updated sound model handle */
    Int32 mSoundModelHandle;

    /** New sound model data */
    AutoPtr<ArrayOf<Byte> > mData;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERSOUNDMODELEVENT_H__
