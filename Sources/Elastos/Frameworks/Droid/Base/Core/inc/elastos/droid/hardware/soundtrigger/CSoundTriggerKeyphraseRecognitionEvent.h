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
