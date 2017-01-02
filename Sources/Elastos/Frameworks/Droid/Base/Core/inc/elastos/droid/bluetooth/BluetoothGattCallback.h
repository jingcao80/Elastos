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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCALLBACK_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * This abstract class is used to implement {@link BluetoothGatt} callbacks.
  */
class BluetoothGattCallback
    : public Object
    , public IBluetoothGattCallback
{
public:
    CAR_INTERFACE_DECL()

    BluetoothGattCallback();

    /**
      * Callback indicating when GATT client has connected/disconnected to/from a remote
      * GATT server.
      *
      * @param gatt GATT client
      * @param status Status of the connect or disconnect operation.
      *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
      * @param newState Returns the new connection state. Can be one of
      *                  {@link BluetoothProfile#STATE_DISCONNECTED} or
      *                  {@link BluetoothProfile#STATE_CONNECTED}
      */
    CARAPI OnConnectionStateChange(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ Int32 status,
        /* [in] */ Int32 newState);

    /**
      * Callback invoked when the list of remote services, characteristics and descriptors
      * for the remote device have been updated, ie new services have been discovered.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#discoverServices}
      * @param status {@link BluetoothGatt#GATT_SUCCESS} if the remote device
      *               has been explored successfully.
      */
    CARAPI OnServicesDiscovered(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ Int32 status);

    /**
      * Callback reporting the result of a characteristic read operation.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#readCharacteristic}
      * @param characteristic Characteristic that was read from the associated
      *                       remote device.
      * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
      *               was completed successfully.
      */
    CARAPI OnCharacteristicRead(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [in] */ Int32 status);

    /**
      * Callback indicating the result of a characteristic write operation.
      *
      * <p>If this callback is invoked while a reliable write transaction is
      * in progress, the value of the characteristic represents the value
      * reported by the remote device. An application should compare this
      * value to the desired value to be written. If the values don't match,
      * the application must abort the reliable write transaction.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#writeCharacteristic}
      * @param characteristic Characteristic that was written to the associated
      *                       remote device.
      * @param status The result of the write operation
      *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
      */
    CARAPI OnCharacteristicWrite(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [in] */ Int32 status);

    /**
      * Callback triggered as a result of a remote characteristic notification.
      *
      * @param gatt GATT client the characteristic is associated with
      * @param characteristic Characteristic that has been updated as a result
      *                       of a remote notification event.
      */
    CARAPI OnCharacteristicChanged(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ IBluetoothGattCharacteristic* characteristic);

    /**
      * Callback reporting the result of a descriptor read operation.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#readDescriptor}
      * @param descriptor Descriptor that was read from the associated
      *                   remote device.
      * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
      *               was completed successfully
      */
    CARAPI OnDescriptorRead(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ IBluetoothGattDescriptor* descriptor,
        /* [in] */ Int32 status);

    /**
      * Callback indicating the result of a descriptor write operation.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#writeDescriptor}
      * @param descriptor Descriptor that was writte to the associated
      *                   remote device.
      * @param status The result of the write operation
      *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
      */
    CARAPI OnDescriptorWrite(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ IBluetoothGattDescriptor* descriptor,
        /* [in] */ Int32 status);

    /**
      * Callback invoked when a reliable write transaction has been completed.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#executeReliableWrite}
      * @param status {@link BluetoothGatt#GATT_SUCCESS} if the reliable write
      *               transaction was executed successfully
      */
    CARAPI OnReliableWriteCompleted(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ Int32 status);

    /**
      * Callback reporting the RSSI for a remote device connection.
      *
      * This callback is triggered in response to the
      * {@link BluetoothGatt#readRemoteRssi} function.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#readRemoteRssi}
      * @param rssi The RSSI value for the remote device
      * @param status {@link BluetoothGatt#GATT_SUCCESS} if the RSSI was read successfully
      */
    CARAPI OnReadRemoteRssi(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ Int32 rssi,
        /* [in] */ Int32 status);

    /**
      * Callback indicating the MTU for a given device connection has changed.
      *
      * This callback is triggered in response to the
      * {@link BluetoothGatt#requestMtu} function, or in response to a connection
      * event.
      *
      * @param gatt GATT client invoked {@link BluetoothGatt#requestMtu}
      * @param mtu The new MTU size
      * @param status {@link BluetoothGatt#GATT_SUCCESS} if the MTU has been changed successfully
      */
    CARAPI OnMtuChanged(
        /* [in] */ IBluetoothGatt* gatt,
        /* [in] */ Int32 mtu,
        /* [in] */ Int32 status);
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTCALLBACK_H__
