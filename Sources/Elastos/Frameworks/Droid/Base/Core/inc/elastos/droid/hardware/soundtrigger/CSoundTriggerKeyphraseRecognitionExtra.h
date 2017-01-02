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
