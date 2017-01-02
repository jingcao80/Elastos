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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVER_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Public API for the Bluetooth GATT Profile server role.
  *
  * <p>This class provides Bluetooth GATT server role functionality,
  * allowing applications to create Bluetooth Smart services and
  * characteristics.
  *
  * <p>BluetoothGattServer is a proxy object for controlling the Bluetooth Service
  * via IPC.  Use {@link BluetoothManager#openGattServer} to get an instance
  * of this class.
  */
class BluetoothGattServer
    : public Object
    , public IBluetoothGattServer
    , public IBluetoothProfile
{
public:
    class BluetoothGattServerCallbackStub
        : public Object
        , public IIBluetoothGattServerCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        BluetoothGattServerCallbackStub();

        CARAPI constructor(
            /* [in] */ IBluetoothGattServer* owner);

        /**
          * Application interface registered - app is ready to go
          * @hide
          */
        CARAPI OnServerRegistered(
            /* [in] */ Int32 status,
            /* [in] */ Int32 serverIf);

        /**
          * Callback reporting an LE scan result.
          * @hide
          */
        CARAPI OnScanResult(
            /* [in] */ const String& address,
            /* [in] */ Int32 rssi,
            /* [in] */ ArrayOf<Byte>* advData);

        /**
          * Server connection state changed
          * @hide
          */
        CARAPI OnServerConnectionState(
            /* [in] */ Int32 status,
            /* [in] */ Int32 serverIf,
            /* [in] */ Boolean connected,
            /* [in] */ const String& address);

        /**
          * Service has been added
          * @hide
          */
        CARAPI OnServiceAdded(
            /* [in] */ Int32 status,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcId);

        /**
          * Remote client characteristic read request.
          * @hide
          */
        CARAPI OnCharacteristicReadRequest(
            /* [in] */ const String& address,
            /* [in] */ Int32 transId,
            /* [in] */ Int32 offset,
            /* [in] */ Boolean isLong,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcId,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charId);

        /**
          * Remote client descriptor read request.
          * @hide
          */
        CARAPI OnDescriptorReadRequest(
            /* [in] */ const String& address,
            /* [in] */ Int32 transId,
            /* [in] */ Int32 offset,
            /* [in] */ Boolean isLong,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcId,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charId,
            /* [in] */ IParcelUuid* descrId);

        /**
          * Remote client characteristic write request.
          * @hide
          */
        CARAPI OnCharacteristicWriteRequest(
            /* [in] */ const String& address,
            /* [in] */ Int32 transId,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [in] */ Boolean isPrep,
            /* [in] */ Boolean needRsp,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcId,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charId,
            /* [in] */ ArrayOf<Byte>* value);

        /**
          * Remote client descriptor write request.
          * @hide
          */
        CARAPI OnDescriptorWriteRequest(
            /* [in] */ const String& address,
            /* [in] */ Int32 transId,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [in] */ Boolean isPrep,
            /* [in] */ Boolean needRsp,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcId,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charId,
            /* [in] */ IParcelUuid* descrId,
            /* [in] */ ArrayOf<Byte>* value);

        /**
          * Execute pending writes.
          * @hide
          */
        CARAPI OnExecuteWrite(
            /* [in] */ const String& address,
            /* [in] */ Int32 transId,
            /* [in] */ Boolean execWrite);

        /**
          * A notification/indication has been sent.
          * @hide
          */
        CARAPI OnNotificationSent(
            /* [in] */ const String& address,
            /* [in] */ Int32 status);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info)
            return Object::ToString(info);
        }
    private:
        BluetoothGattServer* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    BluetoothGattServer();

    /**
      * Create a BluetoothGattServer proxy object.
      */
    /*package*/
    BluetoothGattServer(
        /* [in] */ IContext* context,
        /* [in] */ IIBluetoothGatt* iGatt,
        /* [in] */ Int32 transport);

    /**
      * Close this GATT server instance.
      *
      * Application should call this method as early as possible after it is done with
      * this GATT server.
      */
    CARAPI Close();

    /**
      * Register an application callback to start using GattServer.
      *
      * <p>This is an asynchronous call. The callback is used to notify
      * success or failure if the function returns true.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param callback GATT callback handler that will receive asynchronous
      *                 callbacks.
      * @return true, the callback will be called to notify success or failure,
      *         false on immediate error
      */
    /*package*/
    CARAPI RegisterCallback(
        /* [in] */ IBluetoothGattServerCallback* callback,
        /* [out] */ Boolean* result);

    /**
      * Returns a service by UUID, instance and type.
      * @hide
      */
    /*package*/
    CARAPI GetService(
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instanceId,
        /* [in] */ Int32 type,
        /* [out] */ IBluetoothGattService** result);

    /**
      * Initiate a connection to a Bluetooth GATT capable device.
      *
      * <p>The connection may not be established right away, but will be
      * completed when the remote device is available. A
      * {@link BluetoothGattServerCallback#onConnectionStateChange} callback will be
      * invoked when the connection state changes as a result of this function.
      *
      * <p>The autoConnect paramter determines whether to actively connect to
      * the remote device, or rather passively scan and finalize the connection
      * when the remote device is in range/available. Generally, the first ever
      * connection to a device should be direct (autoConnect set to false) and
      * subsequent connections to known devices should be invoked with the
      * autoConnect parameter set to true.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param autoConnect Whether to directly connect to the remote device (false)
      *                    or to automatically connect as soon as the remote
      *                    device becomes available (true).
      * @return true, if the connection attempt was initiated successfully
      */
    CARAPI Connect(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Boolean autoConnect,
        /* [out] */ Boolean* result);

    /**
      * Disconnects an established connection, or cancels a connection attempt
      * currently in progress.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param device Remote device
      */
    CARAPI CancelConnection(
        /* [in] */ IBluetoothDevice* device);

    /**
      * Send a response to a read or write request to a remote device.
      *
      * <p>This function must be invoked in when a remote read/write request
      * is received by one of these callback methods:
      *
      * <ul>
      *      <li>{@link BluetoothGattServerCallback#onCharacteristicReadRequest}
      *      <li>{@link BluetoothGattServerCallback#onCharacteristicWriteRequest}
      *      <li>{@link BluetoothGattServerCallback#onDescriptorReadRequest}
      *      <li>{@link BluetoothGattServerCallback#onDescriptorWriteRequest}
      * </ul>
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param device The remote device to send this response to
      * @param requestId The ID of the request that was received with the callback
      * @param status The status of the request to be sent to the remote devices
      * @param offset Value offset for partial read/write response
      * @param value The value of the attribute that was read/written (optional)
      */
    CARAPI SendResponse(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 requestId,
        /* [in] */ Int32 status,
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<Byte>* value,
        /* [out] */ Boolean* result);

    /**
      * Send a notification or indication that a local characteristic has been
      * updated.
      *
      * <p>A notification or indication is sent to the remote device to signal
      * that the characteristic has been updated. This function should be invoked
      * for every client that requests notifications/indications by writing
      * to the "Client Configuration" descriptor for the given characteristic.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param device The remote device to receive the notification/indication
      * @param characteristic The local characteristic that has been updated
      * @param confirm true to request confirmation from the client (indication),
      *                false to send a notification
      * @throws IllegalArgumentException
      * @return true, if the notification has been triggered successfully
      */
    CARAPI NotifyCharacteristicChanged(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [in] */ Boolean confirm,
        /* [out] */ Boolean* result);

    /**
      * Add a service to the list of services to be hosted.
      *
      * <p>Once a service has been addded to the the list, the service and its
      * included characteristics will be provided by the local device.
      *
      * <p>If the local device has already exposed services when this function
      * is called, a service update notification will be sent to all clients.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param service Service to be added to the list of services provided
      *                by this device.
      * @return true, if the service has been added successfully
      */
    CARAPI AddService(
        /* [in] */ IBluetoothGattService* service,
        /* [out] */ Boolean* result);

    /**
      * Removes a service from the list of services to be provided.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param service Service to be removed.
      * @return true, if the service has been removed
      */
    CARAPI RemoveService(
        /* [in] */ IBluetoothGattService* service,
        /* [out] */ Boolean* result);

    /**
      * Remove all services from the list of provided services.
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      */
    CARAPI ClearServices();

    /**
      * Returns a list of GATT services offered by this device.
      *
      * <p>An application must call {@link #addService} to add a serice to the
      * list of services offered by this device.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return List of services. Returns an empty list
      *         if no services have been added yet.
      */
    CARAPI GetServices(
        /* [out] */ IList** result);// BluetoothGattService

    /**
      * Returns a {@link BluetoothGattService} from the list of services offered
      * by this device.
      *
      * <p>If multiple instances of the same service (as identified by UUID)
      * exist, the first instance of the service is returned.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param uuid UUID of the requested service
      * @return BluetoothGattService if supported, or null if the requested
      *         service is not offered by this device.
      */
    CARAPI GetService(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothGattService** result);

    /**
      * Not supported - please use {@link BluetoothManager#getConnectedDevices(int)}
      * with {@link BluetoothProfile#GATT} as argument
      *
      * @throws UnsupportedOperationException
      */
    // @Override
    CARAPI GetConnectionState(
        /* [in] */ IBluetoothDevice* device,
        /* [out] */ Int32* state);

    /**
      * Not supported - please use {@link BluetoothManager#getConnectedDevices(int)}
      * with {@link BluetoothProfile#GATT} as argument
      *
      * @throws UnsupportedOperationException
      */
    // @Override
    CARAPI GetConnectedDevices(
        /* [out] */ IList** devices);// IBluetoothDevice

    /**
      * Not supported - please use
      * {@link BluetoothManager#getDevicesMatchingConnectionStates(int, int[])}
      * with {@link BluetoothProfile#GATT} as first argument
      *
      * @throws UnsupportedOperationException
      */
    // @Override
    CARAPI GetDevicesMatchingConnectionStates(
        /* [in] */ ArrayOf<Int32>* states,
        /* [out] */ IList** devices);// IBluetoothDevice

private:
    /**
      * Unregister the current application and callbacks.
      */
    CARAPI_(void) UnregisterCallback();

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    AutoPtr<IContext> mContext;
    AutoPtr<IBluetoothAdapter> mAdapter;
    AutoPtr<IIBluetoothGatt> mService;
    AutoPtr<IBluetoothGattServerCallback> mCallback;
    AutoPtr<Object> mServerIfLock;
    Int32 mServerIf;
    Int32 mTransport;
    AutoPtr<IList> mServices;// BluetoothGattService
    static const Int32 CALLBACK_REG_TIMEOUT = 10000;
    /**
      * Bluetooth GATT interface callbacks
      */
    AutoPtr<IIBluetoothGattServerCallback> mBluetoothGattServerCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATTSERVER_H__
