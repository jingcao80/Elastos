#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATT_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATT_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/bluetooth/BluetoothGattCallbackWrapper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Utility::IList;
using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Public API for the Bluetooth GATT Profile.
  *
  * <p>This class provides Bluetooth GATT functionality to enable communication
  * with Bluetooth Smart or Smart Ready devices.
  *
  * <p>To connect to a remote peripheral device, create a {@link BluetoothGattCallback}
  * and call {@link BluetoothDevice#connectGatt} to get a instance of this class.
  * GATT capable devices can be discovered using the Bluetooth device discovery or BLE
  * scan process.
  */
class BluetoothGatt
    : public Object
    , public IBluetoothGatt
    , public IBluetoothProfile
{
public:
    class InnerBluetoothGattCallbackWrapper
        : public BluetoothGattCallbackWrapper
    {
    public:
        InnerBluetoothGattCallbackWrapper();

        CARAPI constructor(
            /* [in] */ IBluetoothGatt* owner);

        /**
          * Application interface registered - app is ready to go
          * @hide
          */
        CARAPI OnClientRegistered(
            /* [in] */ Int32 status,
            /* [in] */ Int32 clientIf);

        /**
         * Client connection state changed
         * @hide
         */
        CARAPI OnClientConnectionState(
            /* [in] */ Int32 status,
            /* [in] */ Int32 clientIf,
            /* [in] */ Boolean connected,
            /* [in] */ const String& address);

        /**
         * A new GATT service has been discovered.
         * The service is added to the internal list and the search
         * continues.
         * @hide
         */
        CARAPI OnGetService(
            /* [in] */ const String& address,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid);

        /**
         * An included service has been found durig GATT discovery.
         * The included service is added to the respective parent.
         * @hide
         */
        CARAPI OnGetIncludedService(
            /* [in] */ const String& address,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 inclSrvcType,
            /* [in] */ Int32 inclSrvcInstId,
            /* [in] */ IParcelUuid* inclSrvcUuid);

        /**
         * A new GATT characteristic has been discovered.
         * Add the new characteristic to the relevant service and continue
         * the remote device inspection.
         * @hide
         */
        CARAPI OnGetCharacteristic(
            /* [in] */ const String& address,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charUuid,
            /* [in] */ Int32 charProps);

        /**
         * A new GATT descriptor has been discovered.
         * Finally, add the descriptor to the related characteristic.
         * This should conclude the remote device update.
         * @hide
         */
        CARAPI OnGetDescriptor(
            /* [in] */ const String& address,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charUuid,
            /* [in] */ Int32 descrInstId,
            /* [in] */ IParcelUuid* descUuid);

        /**
         * Remote search has been completed.
         * The internal object structure should now reflect the state
         * of the remote device database. Let the application know that
         * we are done at this point.
         * @hide
         */
        CARAPI OnSearchComplete(
            /* [in] */ const String& address,
            /* [in] */ Int32 status);

        /**
          * Remote characteristic has been read.
          * Updates the internal value.
          * @hide
          */
        CARAPI OnCharacteristicRead(
            /* [in] */ const String& address,
            /* [in] */ Int32 status,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charUuid,
            /* [in] */ ArrayOf<Byte>* value);

        /**
          * Characteristic has been written to the remote device.
          * Let the app know how we did...
          * @hide
          */
        CARAPI OnCharacteristicWrite(
            /* [in] */ const String& address,
            /* [in] */ Int32 status,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charUuid);

        /**
          * Remote characteristic has been updated.
          * Updates the internal value.
          * @hide
          */
        CARAPI OnNotify(
            /* [in] */ const String& address,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charUuid,
            /* [in] */ ArrayOf<Byte>* value);

        /**
          * Descriptor has been read.
          * @hide
          */
        CARAPI OnDescriptorRead(
            /* [in] */ const String& address,
            /* [in] */ Int32 status,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charUuid,
            /* [in] */ Int32 descrInstId,
            /* [in] */ IParcelUuid* descrUuid,
            /* [in] */ ArrayOf<Byte>* value);

        /**
          * Descriptor write operation complete.
          * @hide
          */
        CARAPI OnDescriptorWrite(
            /* [in] */ const String& address,
            /* [in] */ Int32 status,
            /* [in] */ Int32 srvcType,
            /* [in] */ Int32 srvcInstId,
            /* [in] */ IParcelUuid* srvcUuid,
            /* [in] */ Int32 charInstId,
            /* [in] */ IParcelUuid* charUuid,
            /* [in] */ Int32 descrInstId,
            /* [in] */ IParcelUuid* descrUuid);

        /**
          * Prepared write transaction completed (or aborted)
          * @hide
          */
        CARAPI OnExecuteWrite(
            /* [in] */ const String& address,
            /* [in] */ Int32 status);

        /**
          * Remote device RSSI has been read
          * @hide
          */
        CARAPI OnReadRemoteRssi(
            /* [in] */ const String& address,
            /* [in] */ Int32 rssi,
            /* [in] */ Int32 status);

        /**
          * Callback invoked when the MTU for a given connection changes
          * @hide
          */
        CARAPI OnConfigureMTU(
            /* [in] */ const String& address,
            /* [in] */ Int32 mtu,
            /* [in] */ Int32 status);

    private:
        BluetoothGatt* mOwner;
    };

public:
    CAR_INTERFACE_DECL();

    BluetoothGatt();

    BluetoothGatt(
        /* [in] */ IContext* context,
        /* [in] */ IIBluetoothGatt* iGatt,
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 transport);

    /**
      * Close this Bluetooth GATT client.
      *
      * Application should call this method as early as possible after it is done with
      * this GATT client.
      */
    CARAPI Close();

    /**
      * Returns a service by UUID, instance and type.
      * @hide
      */
    /*package*/
    CARAPI GetService(
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ IUUID* uuid,
        /* [in] */ Int32 instanceId,
        /* [in] */ Int32 type,
        /* [out] */ IBluetoothGattService** result);

    /**
      * Initiate a connection to a Bluetooth GATT capable device.
      *
      * <p>The connection may not be established right away, but will be
      * completed when the remote device is available. A
      * {@link BluetoothGattCallback#onConnectionStateChange} callback will be
      * invoked when the connection state changes as a result of this function.
      *
      * <p>The autoConnect parameter determines whether to actively connect to
      * the remote device, or rather passively scan and finalize the connection
      * when the remote device is in range/available. Generally, the first ever
      * connection to a device should be direct (autoConnect set to false) and
      * subsequent connections to known devices should be invoked with the
      * autoConnect parameter set to true.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param device Remote device to connect to
      * @param autoConnect Whether to directly connect to the remote device (false)
      *                    or to automatically connect as soon as the remote
      *                    device becomes available (true).
      * @return true, if the connection attempt was initiated successfully
      */
    /*package*/
    CARAPI Connect(
        /* [in] */ Boolean autoConnect,
        /* [in] */ IBluetoothGattCallback* callback,
        /* [out] */ Boolean* result);

    /**
      * Disconnects an established connection, or cancels a connection attempt
      * currently in progress.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      */
    CARAPI Disconnect();

    /**
      * Connect back to remote device.
      *
      * <p>This method is used to re-connect to a remote device after the
      * connection has been dropped. If the device is not in range, the
      * re-connection will be triggered once the device is back in range.
      *
      * @return true, if the connection attempt was initiated successfully
      */
    CARAPI Connect(
        /* [out] */ Boolean* result);

    /**
      * Return the remote bluetooth device this GATT client targets to
      *
      * @return remote bluetooth device
      */
    CARAPI GetDevice(
        /* [out] */ IBluetoothDevice** result);

    /**
      * Discovers services offered by a remote device as well as their
      * characteristics and descriptors.
      *
      * <p>This is an asynchronous operation. Once service discovery is completed,
      * the {@link BluetoothGattCallback#onServicesDiscovered} callback is
      * triggered. If the discovery was successful, the remote services can be
      * retrieved using the {@link #getServices} function.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return true, if the remote service discovery has been started
      */
    CARAPI DiscoverServices(
        /* [out] */ Boolean* result);

    /**
      * Returns a list of GATT services offered by the remote device.
      *
      * <p>This function requires that service discovery has been completed
      * for the given device.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return List of services on the remote device. Returns an empty list
      *         if service discovery has not yet been performed.
      */
    CARAPI GetServices(
        /* [out] */ IList** result);// BluetoothGattService

    /**
      * Returns a {@link BluetoothGattService}, if the requested UUID is
      * supported by the remote device.
      *
      * <p>This function requires that service discovery has been completed
      * for the given device.
      *
      * <p>If multiple instances of the same service (as identified by UUID)
      * exist, the first instance of the service is returned.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param uuid UUID of the requested service
      * @return BluetoothGattService if supported, or null if the requested
      *         service is not offered by the remote device.
      */
    CARAPI GetService(
        /* [in] */ IUUID* uuid,
        /* [out] */ IBluetoothGattService** result);

    /**
      * Reads the requested characteristic from the associated remote device.
      *
      * <p>This is an asynchronous operation. The result of the read operation
      * is reported by the {@link BluetoothGattCallback#onCharacteristicRead}
      * callback.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param characteristic Characteristic to read from the remote device
      * @return true, if the read operation was initiated successfully
      */
    CARAPI ReadCharacteristic(
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [out] */ Boolean* result);

    /**
      * Writes a given characteristic and its values to the associated remote device.
      *
      * <p>Once the write operation has been completed, the
      * {@link BluetoothGattCallback#onCharacteristicWrite} callback is invoked,
      * reporting the result of the operation.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param characteristic Characteristic to write on the remote device
      * @return true, if the write operation was initiated successfully
      */
    CARAPI WriteCharacteristic(
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [out] */ Boolean* result);

    /**
      * Reads the value for a given descriptor from the associated remote device.
      *
      * <p>Once the read operation has been completed, the
      * {@link BluetoothGattCallback#onDescriptorRead} callback is
      * triggered, signaling the result of the operation.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param descriptor Descriptor value to read from the remote device
      * @return true, if the read operation was initiated successfully
      */
    CARAPI ReadDescriptor(
        /* [in] */ IBluetoothGattDescriptor* descriptor,
        /* [out] */ Boolean* result);

    /**
      * Write the value of a given descriptor to the associated remote device.
      *
      * <p>A {@link BluetoothGattCallback#onDescriptorWrite} callback is
      * triggered to report the result of the write operation.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param descriptor Descriptor to write to the associated remote device
      * @return true, if the write operation was initiated successfully
      */
    CARAPI WriteDescriptor(
        /* [in] */ IBluetoothGattDescriptor* descriptor,
        /* [out] */ Boolean* result);

    /**
      * Initiates a reliable write transaction for a given remote device.
      *
      * <p>Once a reliable write transaction has been initiated, all calls
      * to {@link #writeCharacteristic} are sent to the remote device for
      * verification and queued up for atomic execution. The application will
      * receive an {@link BluetoothGattCallback#onCharacteristicWrite} callback
      * in response to every {@link #writeCharacteristic} call and is responsible
      * for verifying if the value has been transmitted accurately.
      *
      * <p>After all characteristics have been queued up and verified,
      * {@link #executeReliableWrite} will execute all writes. If a characteristic
      * was not written correctly, calling {@link #abortReliableWrite} will
      * cancel the current transaction without commiting any values on the
      * remote device.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return true, if the reliable write transaction has been initiated
      */
    CARAPI BeginReliableWrite(
        /* [out] */ Boolean* result);

    /**
      * Executes a reliable write transaction for a given remote device.
      *
      * <p>This function will commit all queued up characteristic write
      * operations for a given remote device.
      *
      * <p>A {@link BluetoothGattCallback#onReliableWriteCompleted} callback is
      * invoked to indicate whether the transaction has been executed correctly.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return true, if the request to execute the transaction has been sent
      */
    CARAPI ExecuteReliableWrite(
        /* [out] */ Boolean* result);

    /**
      * Cancels a reliable write transaction for a given device.
      *
      * <p>Calling this function will discard all queued characteristic write
      * operations for a given remote device.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      */
    CARAPI AbortReliableWrite();

    /**
      * @deprecated Use {@link #abortReliableWrite()}
      */
    CARAPI AbortReliableWrite(
        /* [in] */ IBluetoothDevice* mDevice);

    /**
      * Enable or disable notifications/indications for a given characteristic.
      *
      * <p>Once notifications are enabled for a characteristic, a
      * {@link BluetoothGattCallback#onCharacteristicChanged} callback will be
      * triggered if the remote device indicates that the given characteristic
      * has changed.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param characteristic The characteristic for which to enable notifications
      * @param enable Set to true to enable notifications/indications
      * @return true, if the requested notification status was set successfully
      */
    CARAPI SetCharacteristicNotification(
        /* [in] */ IBluetoothGattCharacteristic* characteristic,
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    /**
      * Clears the internal cache and forces a refresh of the services from the
      * remote device.
      * @hide
      */
    CARAPI Refresh(
        /* [out] */ Boolean* result);

    /**
      * Read the RSSI for a connected remote device.
      *
      * <p>The {@link BluetoothGattCallback#onReadRemoteRssi} callback will be
      * invoked when the RSSI value has been read.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return true, if the RSSI value has been requested successfully
      */
    CARAPI ReadRemoteRssi(
        /* [out] */ Boolean* result);

    /**
      * Request an MTU size used for a given connection.
      *
      * <p>When performing a write request operation (write without response),
      * the data sent is truncated to the MTU size. This function may be used
      * to request a larger MTU size to be able to send more data at once.
      *
      * <p>A {@link BluetoothGattCallback#onMtuChanged} callback will indicate
      * whether this operation was successful.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @return true, if the new MTU value has been requested successfully
      */
    CARAPI RequestMtu(
        /* [in] */ Int32 mtu,
        /* [out] */ Boolean* result);

    /**
      * Request a connection parameter update.
      *
      * <p>This function will send a connection parameter update request to the
      * remote device.
      *
      * @param connectionPriority Request a specific connection priority. Must be one of
      *          {@link BluetoothGatt#CONNECTION_PRIORITY_BALANCED},
      *          {@link BluetoothGatt#CONNECTION_PRIORITY_HIGH}
      *          or {@link BluetoothGatt#CONNECTION_PRIORITY_LOW_POWER}.
      * @throws IllegalArgumentException If the parameters are outside of their
      *                                  specified range.
      */
    CARAPI RequestConnectionPriority(
        /* [in] */ Int32 connectionPriority,
        /* [out] */ Boolean* result);

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
        /* [out] */ IList** devices);// BluetoothDevice

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
        /* [out] */ IList** devices
        );// IBluetoothDevice

private:
    /**
      * Register an application callback to start using GATT.
      *
      * <p>This is an asynchronous call. The callback {@link BluetoothGattCallback#onAppRegistered}
      * is used to notify success or failure if the function returns true.
      *
      * <p>Requires {@link android.Manifest.permission#BLUETOOTH} permission.
      *
      * @param callback GATT callback handler that will receive asynchronous callbacks.
      * @return If true, the callback will be called to notify success or failure,
      *         false on immediate error
      */
    CARAPI_(Boolean) RegisterApp(
        /* [in] */ IBluetoothGattCallback* callback);

    /**
      * Unregister the current application and callbacks.
      */
    CARAPI_(void) UnregisterApp();

public:
    /**
       * No authentication required.
       * @hide
       */
    /*package*/ static const Int32 AUTHENTICATION_NONE = 0;

    /**
      * Authentication requested; no man-in-the-middle protection required.
      * @hide
      */
    /*package*/ static const Int32 AUTHENTICATION_NO_MITM = 1;

    /**
      * Authentication with man-in-the-middle protection requested.
      * @hide
    */
    /*package*/ static const Int32 AUTHENTICATION_MITM = 2;

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    AutoPtr<IContext> mContext;
    AutoPtr<IIBluetoothGatt> mService;
    AutoPtr<IBluetoothGattCallback> mCallback;
    Int32 mClientIf;
    Boolean mAuthRetry;
    AutoPtr<IBluetoothDevice> mDevice;
    Boolean mAutoConnect;
    Int32 mConnState;
    AutoPtr<Object> mStateLock;
    AutoPtr<Object> mDeviceBusyLock;
    Boolean mDeviceBusy;
    Int32 mTransport;
    static const Int32 CONN_STATE_IDLE = 0;
    static const Int32 CONN_STATE_CONNECTING = 1;
    static const Int32 CONN_STATE_CONNECTED = 2;
    static const Int32 CONN_STATE_DISCONNECTING = 3;
    static const Int32 CONN_STATE_CLOSED = 4;
    AutoPtr<IList> mServices;// BluetoothGattService
    /**
      * Bluetooth GATT callbacks. Overrides the default BluetoothGattCallback implementation.
      */
    AutoPtr<IIBluetoothGattCallback> mBluetoothGattCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHGATT_H__

