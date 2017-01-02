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
