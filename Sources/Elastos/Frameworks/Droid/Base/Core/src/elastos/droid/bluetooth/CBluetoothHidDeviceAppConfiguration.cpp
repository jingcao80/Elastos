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

#include "elastos/droid/bluetooth/CBluetoothHidDeviceAppConfiguration.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothHidDeviceAppConfiguration)

CAR_INTERFACE_IMPL_2(CBluetoothHidDeviceAppConfiguration, Object, IBluetoothHidDeviceAppConfiguration, IParcelable)

CBluetoothHidDeviceAppConfiguration::CBluetoothHidDeviceAppConfiguration()
{
    AutoPtr<IRandom> rdm;
    CRandom::New((IRandom**)&rdm);
    rdm->NextInt64(&mHash);
}

ECode CBluetoothHidDeviceAppConfiguration::constructor()
{
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::constructor(
    /* [in] */ Int64 hash)
{
    mHash = hash;
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt64(&mHash);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mHash);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppConfiguration::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    if (IBluetoothHidDeviceAppConfiguration::Probe(o) != NULL) {
        IBluetoothHidDeviceAppConfiguration* cfg = IBluetoothHidDeviceAppConfiguration::Probe(o);
        CBluetoothHidDeviceAppConfiguration* impl = (CBluetoothHidDeviceAppConfiguration*)cfg;
        *result = mHash == impl->mHash;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos