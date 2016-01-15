
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASERECOGNITIONEXTRA_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASERECOGNITIONEXTRA_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerKeyphraseRecognitionExtra.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerKeyphraseRecognitionExtra)
    , public Object
    , public ISoundTriggerKeyphraseRecognitionExtra
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSoundTriggerKeyphraseRecognitionExtra();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ Int32 recognitionModes,
        /* [in] */ Int32 coarseConfidenceLevel,
        /* [in] */ ArrayOf<ISoundTriggerConfidenceLevel*>* confidenceLevels);

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

    // public static final Parcelable.Creator<KeyphraseRecognitionExtra> CREATOR
    //         = new Parcelable.Creator<KeyphraseRecognitionExtra>() {
    //     public KeyphraseRecognitionExtra createFromParcel(Parcel in) {
    //         return KeyphraseRecognitionExtra.fromParcel(in);
    //     }

    //     public KeyphraseRecognitionExtra[] newArray(int size) {
    //         return new KeyphraseRecognitionExtra[size];
    //     }
    // };

    CARAPI GetId(
        /* [out] */ Int32* id);

    CARAPI GetRecognitionModes(
        /* [out] */ Int32* modes);

    CARAPI GetCoarseConfidenceLevel(
        /* [out] */ Int32* level);

    CARAPI GetConfidenceLevels(
        /* [out, callee] */ ArrayOf<ISoundTriggerConfidenceLevel*>** levels);

private:
    static CARAPI FromParcel(
        /* [in] */ IParcel* source,
        /* [out] */ ISoundTriggerKeyphraseRecognitionExtra** result);

public:
    /** The keyphrase ID */
    Int32 mId;

    /** Recognition modes matched for this event */
    Int32 mRecognitionModes;

    /** Confidence level for mode RECOGNITION_MODE_VOICE_TRIGGER when user identification
     * is not performed */
    Int32 mCoarseConfidenceLevel;

    /** Confidence levels for all users recognized (KeyphraseRecognitionEvent) or to
     * be recognized (RecognitionConfig) */
    AutoPtr<ArrayOf<ISoundTriggerConfidenceLevel*> > mConfidenceLevels;
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERKEYPHRASERECOGNITIONEXTRA_H__
