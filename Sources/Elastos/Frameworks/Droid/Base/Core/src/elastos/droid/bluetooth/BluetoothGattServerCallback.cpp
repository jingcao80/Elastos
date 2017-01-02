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

#include "elastos/droid/bluetooth/BluetoothGattServerCallback.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                     BluetoothGattServerCallback
//=====================================================================
ECode BluetoothGattServerCallback::OnConnectionStateChange(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 status,
    /* [in] */ Int32 newState)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnServiceAdded(
    /* [in] */ Int32 status,
    /* [in] */ IBluetoothGattService* service)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnCharacteristicReadRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 offset,
    /* [in] */ IBluetoothGattCharacteristic* characteristic)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnCharacteristicWriteRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Boolean preparedWrite,
    /* [in] */ Boolean responseNeeded,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Byte>* value)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnDescriptorReadRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 offset,
    /* [in] */ IBluetoothGattDescriptor* descriptor)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnDescriptorWriteRequest(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [in] */ Boolean preparedWrite,
    /* [in] */ Boolean responseNeeded,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Byte>* value)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnExecuteWrite(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ Boolean execute)
{
    return NOERROR;
}

ECode BluetoothGattServerCallback::OnNotificationSent(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 status)
{
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
