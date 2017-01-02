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

#include "elastos/droid/bluetooth/BluetoothActivityEnergyInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                     BluetoothActivityEnergyInfo
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothActivityEnergyInfo, Object, IBluetoothActivityEnergyInfo, IParcelable);

BluetoothActivityEnergyInfo::BluetoothActivityEnergyInfo()
{
}

ECode BluetoothActivityEnergyInfo::constructor()
{
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::constructor(
    /* [in] */ Int32 stackState,
    /* [in] */ Int32 txTime,
    /* [in] */ Int32 rxTime,
    /* [in] */ Int32 idleTime,
    /* [in] */ Int32 energyUsed)
{
    mBluetoothStackState = stackState;
    mControllerTxTimeMs = txTime;
    mControllerRxTimeMs = rxTime;
    mControllerIdleTimeMs = idleTime;
    mControllerEnergyUsed = energyUsed;

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&timestamp);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("BluetoothActivityEnergyInfo{")
        + String(" timestamp=") + StringUtils::ToString(timestamp)
        + String(" mBluetoothStackState=") + StringUtils::ToString(mBluetoothStackState)
        + String(" mControllerTxTimeMs=") + StringUtils::ToString(mControllerTxTimeMs)
        + String(" mControllerRxTimeMs=") + StringUtils::ToString(mControllerRxTimeMs)
        + String(" mControllerIdleTimeMs=") + StringUtils::ToString(mControllerIdleTimeMs)
        + String(" mControllerEnergyUsed=") + StringUtils::ToString(mControllerEnergyUsed)
        + String(" }");
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mBluetoothStackState);
    out->WriteInt32(mControllerTxTimeMs);
    out->WriteInt32(mControllerRxTimeMs);
    out->WriteInt32(mControllerIdleTimeMs);
    out->WriteInt32(mControllerEnergyUsed);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mBluetoothStackState);
    source->ReadInt32(&mControllerTxTimeMs);
    source->ReadInt32(&mControllerRxTimeMs);
    source->ReadInt32(&mControllerIdleTimeMs);
    source->ReadInt32(&mControllerEnergyUsed);
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetBluetoothStackState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBluetoothStackState;
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerTxTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerTxTimeMs;
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerRxTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerRxTimeMs;
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerIdleTimeMillis(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerIdleTimeMs;
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetControllerEnergyUsed(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mControllerEnergyUsed;
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::GetTimeStamp(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = timestamp;
    return NOERROR;
}

ECode BluetoothActivityEnergyInfo::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 txTimeMillis;
    Int32 rxTimeMillis;
    Int32 idleTimeMillis;
    *result = (((GetControllerTxTimeMillis(&txTimeMillis), txTimeMillis) !=0) ||
            ((GetControllerRxTimeMillis(&rxTimeMillis), rxTimeMillis) !=0) ||
            ((GetControllerIdleTimeMillis(&idleTimeMillis), idleTimeMillis) !=0));
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
