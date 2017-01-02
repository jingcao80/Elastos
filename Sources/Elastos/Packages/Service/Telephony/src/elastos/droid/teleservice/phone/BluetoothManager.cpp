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

#include "elastos/droid/teleservice/phone/BluetoothManager.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_INTERFACE_IMPL(BluetoothManager, Object, IBluetoothManager)

ECode BluetoothManager::IsBluetoothHeadsetAudioOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAudioConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAudioConnectedOrPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::ShowBluetoothIndication(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(NULL);

    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::UpdateBluetoothIndication()
{
    return NOERROR;
}

ECode BluetoothManager::AddBluetoothIndicatorListener(
    /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener)
{
    return NOERROR;
}

ECode BluetoothManager::RemoveBluetoothIndicatorListener(
    /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener)
{
    return NOERROR;
}

ECode BluetoothManager::ConnectBluetoothAudio()
{
    return NOERROR;
}

ECode BluetoothManager::DisconnectBluetoothAudio()
{
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos