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

#ifndef __ELASTOS_DROID_BLUETOOTH_CBluetoothHidDeviceAppSdpSettings_H__
#define __ELASTOS_DROID_BLUETOOTH_CBluetoothHidDeviceAppSdpSettings_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHidDeviceAppSdpSettings.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHidDeviceAppSdpSettings)
    , public Object
    , public IBluetoothHidDeviceAppSdpSettings
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothHidDeviceAppSdpSettings();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& description,
        /* [in] */ const String& provider,
        /* [in] */ Byte subclass,
        /* [in] */ ArrayOf<Byte>* descriptors);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

public:
    String mName;
    String mDescription;
    String mProvider;
    Byte mSubclass;
    AutoPtr<ArrayOf<Byte> > mDescriptors;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif