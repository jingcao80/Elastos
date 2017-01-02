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
