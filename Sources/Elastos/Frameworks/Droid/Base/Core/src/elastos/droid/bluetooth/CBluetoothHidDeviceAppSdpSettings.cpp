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

#include "elastos/droid/bluetooth/CBluetoothHidDeviceAppSdpSettings.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothHidDeviceAppSdpSettings)

CAR_INTERFACE_IMPL_2(CBluetoothHidDeviceAppSdpSettings, Object, IBluetoothHidDeviceAppSdpSettings, IParcelable)

CBluetoothHidDeviceAppSdpSettings::CBluetoothHidDeviceAppSdpSettings()
{}

ECode CBluetoothHidDeviceAppSdpSettings::constructor()
{
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& description,
    /* [in] */ const String& provider,
    /* [in] */ Byte subclass,
    /* [in] */ ArrayOf<Byte>* descriptors)
{
    mName = name;
    mDescription = description;
    mProvider = provider;
    mSubclass = subclass;
    mDescriptors = descriptors->Clone();
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mName);
    source->ReadString(&mDescription);
    source->ReadString(&mProvider);
    source->ReadByte(&mSubclass);
    HANDLE tmp;
    source->ReadArrayOf(&tmp);
    mDescriptors = reinterpret_cast<ArrayOf<Byte>*>(tmp);
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mName);
    dest->WriteString(mDescription);
    dest->WriteString(mProvider);
    dest->WriteByte(mSubclass);
    dest->WriteArrayOf(reinterpret_cast<HANDLE>(mDescriptors.Get()));
    return NOERROR;
}

ECode CBluetoothHidDeviceAppSdpSettings::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos