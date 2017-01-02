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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIOROUTESINFO_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOROUTESINFO_H__

#include "_Elastos_Droid_Media_CAudioRoutesInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Information available from AudioService about the current routes.
 * @hide
 */
CarClass(CAudioRoutesInfo)
    , public Object
    , public IAudioRoutesInfo
    , public IParcelable
{
public:
    CAudioRoutesInfo();

    virtual ~CAudioRoutesInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IAudioRoutesInfo* o);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* dest);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetBluetoothName(
        /* [out] */ ICharSequence** result);

    CARAPI SetBluetoothName(
        /* [in] */ ICharSequence* bluetoothName);

    CARAPI GetMainType(
        /* [out] */ Int32* result);

    CARAPI SetMainType(
        /* [in] */ Int32 mainType);

public:
    static const Int32 MAIN_SPEAKER;
    static const Int32 MAIN_HEADSET;
    static const Int32 MAIN_HEADPHONES;
    static const Int32 MAIN_DOCK_SPEAKERS;
    static const Int32 MAIN_HDMI;

private:
    AutoPtr<ICharSequence> mBluetoothName;
    Int32 mMainType;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOROUTESINFO_H__
