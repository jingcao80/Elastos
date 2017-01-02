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
