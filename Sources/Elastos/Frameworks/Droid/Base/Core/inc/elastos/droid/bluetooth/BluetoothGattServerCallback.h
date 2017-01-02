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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVERCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVERCALLBACK_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * This abstract class is used to implement {@link BluetoothGattServer} callbacks.
  */
class BluetoothGattServerCallback
    : public Object
    , public IBluetoothGattServerCallback
{
public:
    /**
      * Callback indicating when a remote device has been connected or disconnected.
      *
      * @param device Remote device that has been connected or disconnected.
      * @param status Status of the connect or disconnect operation.
      * @param newState Returns the new connection state. Can be one of
      *                  {@link BluetoothProfile#STATE_DISCONNECTED} or
      *                  {@link BluetoothProfile#STATE_CONNECTED}
      */
    CARAPI OnConnectionStateChange(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 status,
        /* [in] */ Int32 newState);

    /**
      * Indicates whether a local service has been added successfully.
      *
      * @param status Returns {@link BluetoothGatt#GATT_SUCCESS} if the service
      *               was added successfully.
      * @param service The service that has been added
      */
    CARAPI OnServiceAdded(
        /* [in] */ Int32 status,
        /* [in] */ IBluetoothGattService* service);

    /**
      * A remote client has requested to read a local characteristic.
      *
      * <p>An application must call {@link BluetoothGattServer#sendResponse}
      * to complete the request.
      *
      * @param device The remote device that has requested the read operation
      * @param requestId The Id of the request
      * @param offset Offset into the value of the characteristic
      * @param characteristic Characteristic to be read
      */
    CARAPI OnCharacteristicReadRequest(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 requestId,
        /* [in] */ Int32 offset,
        /* [in] */ IBluetoothGattCharacteristic* characteristic);

    /**
      * A remote client has requested to write to a local characteristic.
      *
      * <p>An application must call {@link BluetoothGattServer#sendResponse}
      * to complete the request.
      *
      * @param device The remote device that has requested the write operation
      * @param requestId The Id of the request
      * @param characteristic Characteristic to be written to.
      * @param preparedWrite true, if this write operation should be queued for
      *                      later execution.
      * @param responseNeeded true, if the remote device requires a response
      * @param offset The offset given for the value
      * @param value The value the client wants to assign to the characteristic
      */
    CARAPI OnCharacteristicWriteRequest(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 requestId,
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [in] */ Boolean preparedWrite,
        /* [in] */ Boolean responseNeeded,
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * A remote client has requested to read a local descriptor.
      *
      * <p>An application must call {@link BluetoothGattServer#sendResponse}
      * to complete the request.
      *
      * @param device The remote device that has requested the read operation
      * @param requestId The Id of the request
      * @param offset Offset into the value of the characteristic
      * @param descriptor Descriptor to be read
      */
    CARAPI OnDescriptorReadRequest(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 requestId,
        /* [in] */ Int32 offset,
        /* [in] */ IBluetoothGattDescriptor* descriptor);

    /**
      * A remote client has requested to write to a local descriptor.
      *
      * <p>An application must call {@link BluetoothGattServer#sendResponse}
      * to complete the request.
      *
      * @param device The remote device that has requested the write operation
      * @param requestId The Id of the request
      * @param descriptor Descriptor to be written to.
      * @param preparedWrite true, if this write operation should be queued for
      *                      later execution.
      * @param responseNeeded true, if the remote device requires a response
      * @param offset The offset given for the value
      * @param value The value the client wants to assign to the descriptor
      */
    CARAPI OnDescriptorWriteRequest(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 requestId,
        /* [in] */ IBluetoothGattDescriptor* descriptor,
        /* [in] */ Boolean preparedWrite,
        /* [in] */ Boolean responseNeeded,
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Byte>* value);

    /**
      * Execute all pending write operations for this device.
      *
      * <p>An application must call {@link BluetoothGattServer#sendResponse}
      * to complete the request.
      *
      * @param device The remote device that has requested the write operations
      * @param requestId The Id of the request
      * @param execute Whether the pending writes should be executed (true) or
      *                cancelled (false)
      */
    CARAPI OnExecuteWrite(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 requestId,
        /* [in] */ Boolean execute);

    /**
      * Callback invoked when a notification or indication has been sent to
      * a remote device.
      *
      * <p>When multiple notifications are to be sent, an application must
      * wait for this callback to be received before sending additional
      * notifications.
      *
      * @param device The remote device the notification has been sent to
      * @param status {@link BluetoothGatt#GATT_SUCCESS} if the operation was successful
      */
    CARAPI OnNotificationSent(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 status);
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVERCALLBACK_H__
