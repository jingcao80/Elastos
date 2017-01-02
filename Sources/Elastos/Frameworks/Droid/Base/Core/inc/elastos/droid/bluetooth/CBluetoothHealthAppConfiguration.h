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

#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHAPPCONFIGURATION_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHAPPCONFIGURATION_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Bluetooth_CBluetoothHealthAppConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHealthAppConfiguration)
    , public Object
    , public IBluetoothHealthAppConfiguration
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothHealthAppConfiguration();

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equal);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI ToString(
        /* [out] */ String* s);

    CARAPI GetDataType(
        /* [out] */ Int32* type);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetRole(
        /* [out] */ Int32* role);

    CARAPI GetChannelType(
        /* [out] */ Int32* type);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 dataType);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 dataType,
        /* [in] */ Int32 role,
        /* [in] */ Int32 channelType);

private:
    String mName;
    Int32 mDataType;
    Int32 mRole;
    Int32 mChannelType;
};

}
}
}

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHAPPCONFIGURATION_H__
