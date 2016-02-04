#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/BluetoothGatt.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//           BluetoothGatt::InnerBluetoothGattCallbackWrapper
//=====================================================================
BluetoothGatt::InnerBluetoothGattCallbackWrapper::InnerBluetoothGattCallbackWrapper(
    /* [in] */ BluetoothGatt* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnClientRegistered(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onClientRegistered() - status=" + status
    //     + " clientIf=" + clientIf);
    // if (VDBG) {
    //     synchronized(mStateLock) {
    //         if (mConnState != CONN_STATE_CONNECTING) {
    //             Log.e(TAG, "Bad connection state: " + mConnState);
    //         }
    //     }
    // }
    // mClientIf = clientIf;
    // if (status != GATT_SUCCESS) {
    //     mCallback.onConnectionStateChange(BluetoothGatt.this, GATT_FAILURE,
    //                                       BluetoothProfile.STATE_DISCONNECTED);
    //     synchronized(mStateLock) {
    //         mConnState = CONN_STATE_IDLE;
    //     }
    //     return;
    // }
    // try {
    //     mService.clientConnect(mClientIf, mDevice.getAddress(),
    //                            !mAutoConnect, mTransport); // autoConnect is inverse of "isDirect"
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnClientConnectionState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf,
    /* [in] */ Boolean connected,
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onClientConnectionState() - status=" + status
    //                  + " clientIf=" + clientIf + " device=" + address);
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // int profileState = connected ? BluetoothProfile.STATE_CONNECTED :
    //                                BluetoothProfile.STATE_DISCONNECTED;
    // try {
    //     mCallback.onConnectionStateChange(BluetoothGatt.this, status, profileState);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    //
    // synchronized(mStateLock) {
    //     if (connected) {
    //         mConnState = CONN_STATE_CONNECTED;
    //     } else {
    //         mConnState = CONN_STATE_IDLE;
    //     }
    // }
    //
    // synchronized(mDeviceBusy) {
    //     mDeviceBusy = false;
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnGetService(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid)
{
    VALIDATE_NOT_NULL(srvcUuid);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onGetService() - Device=" + address + " UUID=" + srvcUuid);
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // mServices.add(new BluetoothGattService(mDevice, srvcUuid.getUuid(),
    //                                        srvcInstId, srvcType));
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnGetIncludedService(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 inclSrvcType,
    /* [in] */ Int32 inclSrvcInstId,
    /* [in] */ IParcelUuid* inclSrvcUuid)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(inclSrvcUuid);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onGetIncludedService() - Device=" + address
    //     + " UUID=" + srvcUuid + " Included=" + inclSrvcUuid);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // BluetoothGattService service = getService(mDevice,
    //         srvcUuid.getUuid(), srvcInstId, srvcType);
    // BluetoothGattService includedService = getService(mDevice,
    //         inclSrvcUuid.getUuid(), inclSrvcInstId, inclSrvcType);
    //
    // if (service != null && includedService != null) {
    //     service.addIncludedService(includedService);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnGetCharacteristic(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 charProps)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(charUuid);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onGetCharacteristic() - Device=" + address + " UUID=" +
    //                charUuid);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // BluetoothGattService service = getService(mDevice, srvcUuid.getUuid(),
    //                                           srvcInstId, srvcType);
    // if (service != null) {
    //     service.addCharacteristic(new BluetoothGattCharacteristic(
    //            service, charUuid.getUuid(), charInstId, charProps, 0));
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnGetDescriptor(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 descrInstId,
    /* [in] */ IParcelUuid* descUuid)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(charUuid);
    VALIDATE_NOT_NULL(descUuid);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onGetDescriptor() - Device=" + address + " UUID=" + descUuid);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // BluetoothGattService service = getService(mDevice, srvcUuid.getUuid(),
    //                                           srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(
    //     charUuid.getUuid(), charInstId);
    // if (characteristic == null) return;
    //
    // characteristic.addDescriptor(new BluetoothGattDescriptor(
    //     characteristic, descUuid.getUuid(), descrInstId, 0));
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnSearchComplete(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onSearchComplete() = Device=" + address + " Status=" + status);
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // try {
    //     mCallback.onServicesDiscovered(BluetoothGatt.this, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnCharacteristicRead(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(charUuid);
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onCharacteristicRead() - Device=" + address
    //             + " UUID=" + charUuid + " Status=" + status);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    //
    // synchronized(mDeviceBusy) {
    //     mDeviceBusy = false;
    // }
    //
    // if ((status == GATT_INSUFFICIENT_AUTHENTICATION
    //   || status == GATT_INSUFFICIENT_ENCRYPTION)
    //   && mAuthRetry == false) {
    //     try {
    //         mAuthRetry = true;
    //         mService.readCharacteristic(mClientIf, address,
    //             srvcType, srvcInstId, srvcUuid,
    //             charInstId, charUuid, AUTHENTICATION_MITM);
    //         return;
    //     } catch (RemoteException e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // mAuthRetry = false;
    //
    // BluetoothGattService service = getService(mDevice, srvcUuid.getUuid(),
    //                                           srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(
    //         charUuid.getUuid(), charInstId);
    // if (characteristic == null) return;
    //
    // if (status == 0) characteristic.setValue(value);
    //
    // try {
    //     mCallback.onCharacteristicRead(BluetoothGatt.this, characteristic, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnCharacteristicWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(charUuid);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onCharacteristicWrite() - Device=" + address
    //             + " UUID=" + charUuid + " Status=" + status);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    //
    // synchronized(mDeviceBusy) {
    //     mDeviceBusy = false;
    // }
    //
    // BluetoothGattService service = getService(mDevice, srvcUuid.getUuid(),
    //                                           srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(
    //         charUuid.getUuid(), charInstId);
    // if (characteristic == null) return;
    //
    // if ((status == GATT_INSUFFICIENT_AUTHENTICATION
    //   || status == GATT_INSUFFICIENT_ENCRYPTION)
    //   && mAuthRetry == false) {
    //     try {
    //         mAuthRetry = true;
    //         mService.writeCharacteristic(mClientIf, address,
    //             srvcType, srvcInstId, srvcUuid, charInstId, charUuid,
    //             characteristic.getWriteType(), AUTHENTICATION_MITM,
    //             characteristic.getValue());
    //         return;
    //     } catch (RemoteException e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // mAuthRetry = false;
    //
    // try {
    //     mCallback.onCharacteristicWrite(BluetoothGatt.this, characteristic, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnNotify(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(charUuid);
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onNotify() - Device=" + address + " UUID=" + charUuid);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // BluetoothGattService service = getService(mDevice, srvcUuid.getUuid(),
    //                                           srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(
    //         charUuid.getUuid(), charInstId);
    // if (characteristic == null) return;
    //
    // characteristic.setValue(value);
    //
    // try {
    //     mCallback.onCharacteristicChanged(BluetoothGatt.this, characteristic);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnDescriptorRead(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 descrInstId,
    /* [in] */ IParcelUuid* descrUuid,
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(charUuid);
    VALIDATE_NOT_NULL(descrUuid);
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onDescriptorRead() - Device=" + address + " UUID=" + charUuid);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    //
    // synchronized(mDeviceBusy) {
    //     mDeviceBusy = false;
    // }
    //
    // BluetoothGattService service = getService(mDevice, srvcUuid.getUuid(),
    //                                           srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(
    //         charUuid.getUuid(), charInstId);
    // if (characteristic == null) return;
    //
    // BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
    //         descrUuid.getUuid(), descrInstId);
    // if (descriptor == null) return;
    //
    // if (status == 0) descriptor.setValue(value);
    //
    // if ((status == GATT_INSUFFICIENT_AUTHENTICATION
    //   || status == GATT_INSUFFICIENT_ENCRYPTION)
    //   && mAuthRetry == false) {
    //     try {
    //         mAuthRetry = true;
    //         mService.readDescriptor(mClientIf, address,
    //             srvcType, srvcInstId, srvcUuid, charInstId, charUuid,
    //             descrInstId, descrUuid, AUTHENTICATION_MITM);
    //         return;
    //     } catch (RemoteException e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // mAuthRetry = true;
    //
    // try {
    //     mCallback.onDescriptorRead(BluetoothGatt.this, descriptor, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnDescriptorWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charUuid,
    /* [in] */ Int32 descrInstId,
    /* [in] */ IParcelUuid* descrUuid)
{
    VALIDATE_NOT_NULL(srvcUuid);
    VALIDATE_NOT_NULL(charUuid);
    VALIDATE_NOT_NULL(descrUuid);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onDescriptorWrite() - Device=" + address + " UUID=" + charUuid);
    //
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    //
    // synchronized(mDeviceBusy) {
    //     mDeviceBusy = false;
    // }
    //
    // BluetoothGattService service = getService(mDevice, srvcUuid.getUuid(),
    //                                           srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(
    //         charUuid.getUuid(), charInstId);
    // if (characteristic == null) return;
    //
    // BluetoothGattDescriptor descriptor = characteristic.getDescriptor(
    //         descrUuid.getUuid(), descrInstId);
    // if (descriptor == null) return;
    //
    // if ((status == GATT_INSUFFICIENT_AUTHENTICATION
    //   || status == GATT_INSUFFICIENT_ENCRYPTION)
    //   && mAuthRetry == false) {
    //     try {
    //         mAuthRetry = true;
    //         mService.writeDescriptor(mClientIf, address,
    //             srvcType, srvcInstId, srvcUuid, charInstId, charUuid,
    //             descrInstId, descrUuid, characteristic.getWriteType(),
    //             AUTHENTICATION_MITM, descriptor.getValue());
    //         return;
    //     } catch (RemoteException e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // mAuthRetry = false;
    //
    // try {
    //     mCallback.onDescriptorWrite(BluetoothGatt.this, descriptor, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnExecuteWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onExecuteWrite() - Device=" + address
    //     + " status=" + status);
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    //
    // synchronized(mDeviceBusy) {
    //     mDeviceBusy = false;
    // }
    //
    // try {
    //     mCallback.onReliableWriteCompleted(BluetoothGatt.this, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnReadRemoteRssi(
    /* [in] */ const String& address,
    /* [in] */ Int32 rssi,
    /* [in] */ Int32 status)
{
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onReadRemoteRssi() - Device=" + address +
    //             " rssi=" + rssi + " status=" + status);
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // try {
    //     mCallback.onReadRemoteRssi(BluetoothGatt.this, rssi, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnConfigureMTU(
    /* [in] */ const String& address,
    /* [in] */ Int32 mtu,
    /* [in] */ Int32 status)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onConfigureMTU() - Device=" + address +
    //             " mtu=" + mtu + " status=" + status);
    // if (!address.equals(mDevice.getAddress())) {
    //     return;
    // }
    // try {
    //     mCallback.onMtuChanged(BluetoothGatt.this, mtu, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                            BluetoothGatt
//=====================================================================
const String BluetoothGatt::TAG("BluetoothGatt");
const Boolean BluetoothGatt::DBG = true;
const Boolean BluetoothGatt::VDBG = false;
const Int32 BluetoothGatt::CONN_STATE_IDLE;
const Int32 BluetoothGatt::CONN_STATE_CONNECTING;
const Int32 BluetoothGatt::CONN_STATE_CONNECTED;
const Int32 BluetoothGatt::CONN_STATE_DISCONNECTING;
const Int32 BluetoothGatt::CONN_STATE_CLOSED;

CAR_INTERFACE_IMPL_2(BluetoothGatt, Object, IBluetoothGatt, IBluetoothProfile);

BluetoothGatt::BluetoothGatt()
{
}

BluetoothGatt::BluetoothGatt(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothGatt* iGatt,
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 transport)
{
    // ==================before translated======================
    // mContext = context;
    // mService = iGatt;
    // mDevice = device;
    // mTransport = transport;
    // mServices = new ArrayList<BluetoothGattService>();
    //
    // mConnState = CONN_STATE_IDLE;
}

ECode BluetoothGatt::Close()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "close()");
    //
    // unregisterApp();
    // mConnState = CONN_STATE_CLOSED;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::GetService(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 type,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for(BluetoothGattService svc : mServices) {
    //     if (svc.getDevice().equals(device) &&
    //         svc.getType() == type &&
    //         svc.getInstanceId() == instanceId &&
    //         svc.getUuid().equals(uuid)) {
    //         return svc;
    //     }
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::Connect(
    /* [in] */ Boolean* autoConnect,
    /* [in] */ IBluetoothGattCallback* callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(autoConnect);
    VALIDATE_NOT_NULL(callback);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "connect() - device: " + mDevice.getAddress() + ", auto: " + autoConnect);
    // synchronized(mStateLock) {
    //     if (mConnState != CONN_STATE_IDLE) {
    //         throw new IllegalStateException("Not idle");
    //     }
    //     mConnState = CONN_STATE_CONNECTING;
    // }
    // if (!registerApp(callback)) {
    //     synchronized(mStateLock) {
    //         mConnState = CONN_STATE_IDLE;
    //     }
    //     Log.e(TAG, "Failed to register callback");
    //     return false;
    // }
    //
    // // the connection will continue after successful callback registration
    // mAutoConnect = autoConnect;
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::Disconnect()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "cancelOpen() - device: " + mDevice.getAddress());
    // if (mService == null || mClientIf == 0) return;
    //
    // try {
    //     mService.clientDisconnect(mClientIf, mDevice.getAddress());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::Connect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // try {
    //     mService.clientConnect(mClientIf, mDevice.getAddress(),
    //                            false, mTransport); // autoConnect is inverse of "isDirect"
    //     return true;
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::GetDevice(
    /* [out] */ IBluetoothDevice** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDevice;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::DiscoverServices(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "discoverServices() - device: " + mDevice.getAddress());
    // if (mService == null || mClientIf == 0) return false;
    //
    // mServices.clear();
    //
    // try {
    //     mService.discoverServices(mClientIf, mDevice.getAddress());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::GetServices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // List<BluetoothGattService> result =
    //         new ArrayList<BluetoothGattService>();
    //
    // for (BluetoothGattService service : mServices) {
    //     if (service.getDevice().equals(mDevice)) {
    //         result.add(service);
    //     }
    // }
    //
    // return result;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::GetService(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for (BluetoothGattService service : mServices) {
    //     if (service.getDevice().equals(mDevice) &&
    //         service.getUuid().equals(uuid)) {
    //         return service;
    //     }
    // }
    //
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::ReadCharacteristic(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(characteristic);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if ((characteristic.getProperties() &
    //         BluetoothGattCharacteristic.PROPERTY_READ) == 0) return false;
    //
    // if (VDBG) Log.d(TAG, "readCharacteristic() - uuid: " + characteristic.getUuid());
    // if (mService == null || mClientIf == 0) return false;
    //
    // BluetoothGattService service = characteristic.getService();
    // if (service == null) return false;
    //
    // BluetoothDevice device = service.getDevice();
    // if (device == null) return false;
    //
    // synchronized(mDeviceBusy) {
    //     if (mDeviceBusy) return false;
    //     mDeviceBusy = true;
    // }
    //
    // try {
    //     mService.readCharacteristic(mClientIf, device.getAddress(),
    //         service.getType(), service.getInstanceId(),
    //         new ParcelUuid(service.getUuid()), characteristic.getInstanceId(),
    //         new ParcelUuid(characteristic.getUuid()), AUTHENTICATION_NONE);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     mDeviceBusy = false;
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::WriteCharacteristic(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(characteristic);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if ((characteristic.getProperties() & BluetoothGattCharacteristic.PROPERTY_WRITE) == 0
    //     && (characteristic.getProperties() &
    //         BluetoothGattCharacteristic.PROPERTY_WRITE_NO_RESPONSE) == 0) return false;
    //
    // if (VDBG) Log.d(TAG, "writeCharacteristic() - uuid: " + characteristic.getUuid());
    // if (mService == null || mClientIf == 0) return false;
    //
    // BluetoothGattService service = characteristic.getService();
    // if (service == null) return false;
    //
    // BluetoothDevice device = service.getDevice();
    // if (device == null) return false;
    //
    // synchronized(mDeviceBusy) {
    //     if (mDeviceBusy) return false;
    //     mDeviceBusy = true;
    // }
    //
    // try {
    //     mService.writeCharacteristic(mClientIf, device.getAddress(),
    //         service.getType(), service.getInstanceId(),
    //         new ParcelUuid(service.getUuid()), characteristic.getInstanceId(),
    //         new ParcelUuid(characteristic.getUuid()),
    //         characteristic.getWriteType(), AUTHENTICATION_NONE,
    //         characteristic.getValue());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     mDeviceBusy = false;
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::ReadDescriptor(
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(descriptor);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "readDescriptor() - uuid: " + descriptor.getUuid());
    // if (mService == null || mClientIf == 0) return false;
    //
    // BluetoothGattCharacteristic characteristic = descriptor.getCharacteristic();
    // if (characteristic == null) return false;
    //
    // BluetoothGattService service = characteristic.getService();
    // if (service == null) return false;
    //
    // BluetoothDevice device = service.getDevice();
    // if (device == null) return false;
    //
    // synchronized(mDeviceBusy) {
    //     if (mDeviceBusy) return false;
    //     mDeviceBusy = true;
    // }
    //
    // try {
    //     mService.readDescriptor(mClientIf, device.getAddress(), service.getType(),
    //         service.getInstanceId(), new ParcelUuid(service.getUuid()),
    //         characteristic.getInstanceId(), new ParcelUuid(characteristic.getUuid()),
    //         descriptor.getInstanceId(), new ParcelUuid(descriptor.getUuid()),
    //         AUTHENTICATION_NONE);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     mDeviceBusy = false;
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::WriteDescriptor(
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(descriptor);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "writeDescriptor() - uuid: " + descriptor.getUuid());
    // if (mService == null || mClientIf == 0) return false;
    //
    // BluetoothGattCharacteristic characteristic = descriptor.getCharacteristic();
    // if (characteristic == null) return false;
    //
    // BluetoothGattService service = characteristic.getService();
    // if (service == null) return false;
    //
    // BluetoothDevice device = service.getDevice();
    // if (device == null) return false;
    //
    // synchronized(mDeviceBusy) {
    //     if (mDeviceBusy) return false;
    //     mDeviceBusy = true;
    // }
    //
    // try {
    //     mService.writeDescriptor(mClientIf, device.getAddress(), service.getType(),
    //         service.getInstanceId(), new ParcelUuid(service.getUuid()),
    //         characteristic.getInstanceId(), new ParcelUuid(characteristic.getUuid()),
    //         descriptor.getInstanceId(), new ParcelUuid(descriptor.getUuid()),
    //         characteristic.getWriteType(), AUTHENTICATION_NONE,
    //         descriptor.getValue());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     mDeviceBusy = false;
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::BeginReliableWrite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "beginReliableWrite() - device: " + mDevice.getAddress());
    // if (mService == null || mClientIf == 0) return false;
    //
    // try {
    //     mService.beginReliableWrite(mClientIf, mDevice.getAddress());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::ExecuteReliableWrite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "executeReliableWrite() - device: " + mDevice.getAddress());
    // if (mService == null || mClientIf == 0) return false;
    //
    // synchronized(mDeviceBusy) {
    //     if (mDeviceBusy) return false;
    //     mDeviceBusy = true;
    // }
    //
    // try {
    //     mService.endReliableWrite(mClientIf, mDevice.getAddress(), true);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     mDeviceBusy = false;
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::AbortReliableWrite()
{
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "abortReliableWrite() - device: " + mDevice.getAddress());
    // if (mService == null || mClientIf == 0) return;
    //
    // try {
    //     mService.endReliableWrite(mClientIf, mDevice.getAddress(), false);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::AbortReliableWrite(
    /* [in] */ IBluetoothDevice* mDevice)
{
    VALIDATE_NOT_NULL(mDevice);
    // ==================before translated======================
    // abortReliableWrite();
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::SetCharacteristicNotification(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(characteristic);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "setCharacteristicNotification() - uuid: " + characteristic.getUuid()
    //                  + " enable: " + enable);
    // if (mService == null || mClientIf == 0) return false;
    //
    // BluetoothGattService service = characteristic.getService();
    // if (service == null) return false;
    //
    // BluetoothDevice device = service.getDevice();
    // if (device == null) return false;
    //
    // try {
    //     mService.registerForNotification(mClientIf, device.getAddress(),
    //         service.getType(), service.getInstanceId(),
    //         new ParcelUuid(service.getUuid()), characteristic.getInstanceId(),
    //         new ParcelUuid(characteristic.getUuid()),
    //         enable);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::Refresh(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "refresh() - device: " + mDevice.getAddress());
    // if (mService == null || mClientIf == 0) return false;
    //
    // try {
    //     mService.refreshDevice(mClientIf, mDevice.getAddress());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::ReadRemoteRssi(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "readRssi() - device: " + mDevice.getAddress());
    // if (mService == null || mClientIf == 0) return false;
    //
    // try {
    //     mService.readRemoteRssi(mClientIf, mDevice.getAddress());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::RequestMtu(
    /* [in] */ Int32 mtu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "configureMTU() - device: " + mDevice.getAddress()
    //                     + " mtu: " + mtu);
    // if (mService == null || mClientIf == 0) return false;
    //
    // try {
    //     mService.configureMTU(mClientIf, mDevice.getAddress(), mtu);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::RequestConnectionPriority(
    /* [in] */ Int32 connectionPriority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (connectionPriority < CONNECTION_PRIORITY_BALANCED ||
    //     connectionPriority > CONNECTION_PRIORITY_LOW_POWER) {
    //     throw new IllegalArgumentException("connectionPriority not within valid range");
    // }
    //
    // if (DBG) Log.d(TAG, "requestConnectionPriority() - params: " + connectionPriority);
    // if (mService == null || mClientIf == 0) return false;
    //
    // try {
    //     mService.connectionParameterUpdate(mClientIf, mDevice.getAddress(), connectionPriority);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    // ==================before translated======================
    // throw new UnsupportedOperationException("Use BluetoothManager#getConnectionState instead.");
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::GetConnectedDevices(
    /* [out] */ IList** devices)
{
    VALIDATE_NOT_NULL(devices);
    // ==================before translated======================
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getConnectedDevices instead.");
    assert(0);
    return NOERROR;
}

ECode BluetoothGatt::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    VALIDATE_NOT_NULL(devices);
    // ==================before translated======================
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getDevicesMatchingConnectionStates instead.");
    assert(0);
    return NOERROR;
}

Boolean BluetoothGatt::RegisterApp(
    /* [in] */ IBluetoothGattCallback* callback)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "registerApp()");
    // if (mService == null) return false;
    //
    // mCallback = callback;
    // UUID uuid = UUID.randomUUID();
    // if (DBG) Log.d(TAG, "registerApp() - UUID=" + uuid);
    //
    // try {
    //     mService.registerClient(new ParcelUuid(uuid), mBluetoothGattCallback);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return FALSE;
}

void BluetoothGatt::UnregisterApp()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "unregisterApp() - mClientIf=" + mClientIf);
    // if (mService == null || mClientIf == 0) return;
    //
    // try {
    //     mCallback = null;
    //     mService.unregisterClient(mClientIf);
    //     mClientIf = 0;
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    assert(0);
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
