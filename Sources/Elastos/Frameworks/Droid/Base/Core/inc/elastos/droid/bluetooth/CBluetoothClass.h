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

#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHCLASS_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHCLASS_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothClass.h"
#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothClass)
    , public Object
    , public IBluetoothClass
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothClass();

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI HasService(
        /* [in] */ Int32 service,
        /* [out] */ Boolean* hasService);

    CARAPI GetMajorDeviceClass(
        /* [out] */ Int32* deviceClass);

    CARAPI GetDeviceClass(
        /* [out] */ Int32* deviceClass);

    CARAPI DoesClassMatch(
        /* [in] */ Int32 profile,
        /* [out] */ Boolean* isMatch);

    CARAPI GetClass(
        /* [out] */ Int32* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 classInt);

private:
    Int32 mClass;
};

}
}
}

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHCLASS_H__
