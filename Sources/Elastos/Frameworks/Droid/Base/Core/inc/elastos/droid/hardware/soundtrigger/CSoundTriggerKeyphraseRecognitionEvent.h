
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASERECOGNITIONEVENT_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASERECOGNITIONEVENT_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerKeyphraseRecognitionEvent.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerRecognitionEvent.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerKeyphraseRecognitionEvent)
    , public SoundTriggerRecognitionEvent
    , public ISoundTriggerKeyphraseRecognitionEvent
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSoundTriggerKeyphraseRecognitionEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 status,
        /* [in] */ Int32 soundModelHandle,
        /* [in] */ Boolean captureAvailable,
        /* [in] */ Int32 captureSession,
        /* [in] */ Int32 captureDelayMs,
        /* [in] */ Int32 capturePreambleMs,
        /* [in] */ Boolean triggerInData,
        /* [in] */ IAudioFormat* captureFormat,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*>* keyphraseExtras);

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

    // public static final Parcelable.Creator<KeyphraseRecognitionEvent> CREATOR
    //         = new Parcelable.Creator<KeyphraseRecognitionEvent>() {
    //     public KeyphraseRecognitionEvent createFromParcel(Parcel in) {
    //         return KeyphraseRecognitionEvent.fromParcel(in);
    //     }

    //     public KeyphraseRecognitionEvent[] newArray(int size) {
    //         return new KeyphraseRecognitionEvent[size];
    //     }
    // };

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerKeyphraseRecognitionEvent** result);

public:
    /** Indicates if the key phrase is present in the buffered audio available for capture */
    AutoPtr<ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*> > mKeyphraseExtras;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASERECOGNITIONEVENT_H__
