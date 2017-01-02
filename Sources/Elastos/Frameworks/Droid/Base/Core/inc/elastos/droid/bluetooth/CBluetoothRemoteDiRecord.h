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

#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHREMOTEDIRECORD_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHREMOTEDIRECORD_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothRemoteDiRecord.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothRemoteDiRecord)
    , public Object
    , public IBluetoothRemoteDiRecord
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothRemoteDiRecord();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 vendorId,
        /* [in] */ Int32 vendorIdSource,
        /* [in] */ Int32 productId,
        /* [in] */ Int32 productVersion,
        /* [in] */ Int32 specificationId);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetVendorId(
        /* [out] */ Int32* id);

    CARAPI GetVendorIdSource(
        /* [out] */ Int32* idSource);

    CARAPI GetProductId(
        /* [out] */ Int32* id);

    CARAPI GetProductVersion(
        /* [out] */ Int32* vs);

    CARAPI GetSpecificationId(
        /* [out] */ Int32* id);

private:
    Int32 mVendorId;
    Int32 mVendorIdSource;
    Int32 mProductId;
    Int32 mProductVersion;
    Int32 mSpecificationId;
    Object mObject;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif