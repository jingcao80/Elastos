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

#ifndef  __ELASTOS_DROID_PHONE_BLUETOOTHMANAGER_H__
#define  __ELASTOS_DROID_PHONE_BLUETOOTHMANAGER_H__

#include "_Elastos.Droid.TeleService.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

class BluetoothManager
    : public Object
    , public IBluetoothManager
{
public:
    TO_STRING_IMPL("BluetoothManager")

    CAR_INTERFACE_DECL()

    BluetoothManager() {}

    CARAPI IsBluetoothHeadsetAudioOn(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothAvailable(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothAudioConnected(
        /* [out] */ Boolean* result);

    CARAPI IsBluetoothAudioConnectedOrPending(
        /* [out] */ Boolean* result);

    CARAPI ShowBluetoothIndication(
        /* [out] */ Boolean* result);

    CARAPI UpdateBluetoothIndication();

    CARAPI AddBluetoothIndicatorListener(
        /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener);

    CARAPI RemoveBluetoothIndicatorListener(
        /* [in] */ IBluetoothManagerBluetoothIndicatorListener* listener);

    CARAPI ConnectBluetoothAudio();

    CARAPI DisconnectBluetoothAudio();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_BLUETOOTHMANAGER_H__