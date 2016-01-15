
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASESOUNDMODEL_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASESOUNDMODEL_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerKeyphraseSoundModel.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerSoundModel.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerKeyphraseSoundModel)
    , public SoundTriggerSoundModel
    , public ISoundTriggerKeyphraseSoundModel
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSoundTriggerKeyphraseSoundModel();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUUID* uuid,
        /* [in] */ IUUID* vendorUuid,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<ISoundTriggerKeyphrase*>* keyphrases);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    //@Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    // public static final Parcelable.Creator<KeyphraseSoundModel> CREATOR
    //         = new Parcelable.Creator<KeyphraseSoundModel>() {
    //     public KeyphraseSoundModel createFromParcel(Parcel in) {
    //         return KeyphraseSoundModel.fromParcel(in);
    //     }

    //     public KeyphraseSoundModel[] newArray(int size) {
    //         return new KeyphraseSoundModel[size];
    //     }
    // };

    CARAPI GetKeyphrases(
        /* [out, callee] */ ArrayOf<ISoundTriggerKeyphrase*>** keyphrases);

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerKeyphraseSoundModel** result);

public:
    /** Key phrases in this sound model */
    AutoPtr<ArrayOf<ISoundTriggerKeyphrase*> > mKeyphrases; // keyword phrases in model
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASESOUNDMODEL_H__
