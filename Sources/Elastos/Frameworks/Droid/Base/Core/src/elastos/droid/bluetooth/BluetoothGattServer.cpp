
#include "elastos/droid/bluetooth/BluetoothGattServer.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//      BluetoothGattServer::BluetoothGattServerCallbackStub
//=====================================================================

CAR_INTERFACE_IMPL_2(BluetoothGattServer::BluetoothGattServerCallbackStub, Object, IIBluetoothGattServerCallback, IBinder);

BluetoothGattServer::BluetoothGattServerCallbackStub::BluetoothGattServerCallbackStub()
{
}

BluetoothGattServer::BluetoothGattServerCallbackStub::BluetoothGattServerCallbackStub(
    /* [in] */ IBluetoothGattServer* owner)
{
    mOwner = (BluetoothGattServer*)owner;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnServerRegistered(
    /* [in] */ Int32 status,
    /* [in] */ Int32 serverIf)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onServerRegistered() - status=" + status
    //     + " serverIf=" + serverIf);
    // synchronized(mServerIfLock) {
    //     if (mCallback != null) {
    //         mServerIf = serverIf;
    //         mServerIfLock.notify();
    //     } else {
    //         // registration timeout
    //         Log.e(TAG, "onServerRegistered: mCallback is null");
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnScanResult(
    /* [in] */ const String& address,
    /* [in] */ Int32 rssi,
    /* [in] */ ArrayOf<Byte>* advData)
{
    VALIDATE_NOT_NULL(advData);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onScanResult() - Device=" + address + " RSSI=" +rssi);
    // // no op
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnServerConnectionState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 serverIf,
    /* [in] */ Boolean connected,
    /* [in] */ const String& address)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onServerConnectionState() - status=" + status
    //     + " serverIf=" + serverIf + " device=" + address);
    // try {
    //     mCallback.onConnectionStateChange(mAdapter.getRemoteDevice(address), status,
    //                                       connected ? BluetoothProfile.STATE_CONNECTED :
    //                                       BluetoothProfile.STATE_DISCONNECTED);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnServiceAdded(
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcId)
{
    VALIDATE_NOT_NULL(srvcId);
    // ==================before translated======================
    // UUID srvcUuid = srvcId.getUuid();
    // if (DBG) Log.d(TAG, "onServiceAdded() - service=" + srvcUuid
    //     + "status=" + status);
    //
    // BluetoothGattService service = getService(srvcUuid, srvcInstId, srvcType);
    // if (service == null) return;
    //
    // try {
    //     mCallback.onServiceAdded((int)status, service);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnCharacteristicReadRequest(
    /* [in] */ const String& address,
    /* [in] */ Int32 transId,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean isLong,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcId,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charId)
{
    VALIDATE_NOT_NULL(srvcId);
    VALIDATE_NOT_NULL(charId);
    // ==================before translated======================
    // UUID srvcUuid = srvcId.getUuid();
    // UUID charUuid = charId.getUuid();
    // if (VDBG) Log.d(TAG, "onCharacteristicReadRequest() - "
    //     + "service=" + srvcUuid + ", characteristic=" + charUuid);
    //
    // BluetoothDevice device = mAdapter.getRemoteDevice(address);
    // BluetoothGattService service = getService(srvcUuid, srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(charUuid);
    // if (characteristic == null) return;
    //
    // try {
    //     mCallback.onCharacteristicReadRequest(device, transId, offset, characteristic);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnDescriptorReadRequest(
    /* [in] */ const String& address,
    /* [in] */ Int32 transId,
    /* [in] */ Int32 offset,
    /* [in] */ Boolean isLong,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcId,
    /* [in] */ Int32 charInstId,
    /* [in] */ IParcelUuid* charId,
    /* [in] */ IParcelUuid* descrId)
{
    VALIDATE_NOT_NULL(srvcId);
    VALIDATE_NOT_NULL(charId);
    VALIDATE_NOT_NULL(descrId);
    // ==================before translated======================
    // UUID srvcUuid = srvcId.getUuid();
    // UUID charUuid = charId.getUuid();
    // UUID descrUuid = descrId.getUuid();
    // if (VDBG) Log.d(TAG, "onCharacteristicReadRequest() - "
    //     + "service=" + srvcUuid + ", characteristic=" + charUuid
    //     + "descriptor=" + descrUuid);
    //
    // BluetoothDevice device = mAdapter.getRemoteDevice(address);
    // BluetoothGattService service = getService(srvcUuid, srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(charUuid);
    // if (characteristic == null) return;
    //
    // BluetoothGattDescriptor descriptor = characteristic.getDescriptor(descrUuid);
    // if (descriptor == null) return;
    //
    // try {
    //     mCallback.onDescriptorReadRequest(device, transId, offset, descriptor);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnCharacteristicWriteRequest(
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
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(srvcId);
    VALIDATE_NOT_NULL(charId);
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // UUID srvcUuid = srvcId.getUuid();
    // UUID charUuid = charId.getUuid();
    // if (VDBG) Log.d(TAG, "onCharacteristicWriteRequest() - "
    //     + "service=" + srvcUuid + ", characteristic=" + charUuid);
    //
    // BluetoothDevice device = mAdapter.getRemoteDevice(address);
    // BluetoothGattService service = getService(srvcUuid, srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(charUuid);
    // if (characteristic == null) return;
    //
    // try {
    //     mCallback.onCharacteristicWriteRequest(device, transId, characteristic,
    //                                            isPrep, needRsp, offset, value);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnDescriptorWriteRequest(
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
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(srvcId);
    VALIDATE_NOT_NULL(charId);
    VALIDATE_NOT_NULL(descrId);
    VALIDATE_NOT_NULL(value);
    // ==================before translated======================
    // UUID srvcUuid = srvcId.getUuid();
    // UUID charUuid = charId.getUuid();
    // UUID descrUuid = descrId.getUuid();
    // if (VDBG) Log.d(TAG, "onDescriptorWriteRequest() - "
    //     + "service=" + srvcUuid + ", characteristic=" + charUuid
    //     + "descriptor=" + descrUuid);
    //
    // BluetoothDevice device = mAdapter.getRemoteDevice(address);
    //
    // BluetoothGattService service = getService(srvcUuid, srvcInstId, srvcType);
    // if (service == null) return;
    //
    // BluetoothGattCharacteristic characteristic = service.getCharacteristic(charUuid);
    // if (characteristic == null) return;
    //
    // BluetoothGattDescriptor descriptor = characteristic.getDescriptor(descrUuid);
    // if (descriptor == null) return;
    //
    // try {
    //     mCallback.onDescriptorWriteRequest(device, transId, descriptor,
    //                                        isPrep, needRsp, offset, value);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnExecuteWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 transId,
    /* [in] */ Boolean execWrite)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onExecuteWrite() - "
    //     + "device=" + address + ", transId=" + transId
    //     + "execWrite=" + execWrite);
    //
    // BluetoothDevice device = mAdapter.getRemoteDevice(address);
    // if (device == null) return;
    //
    // try {
    //     mCallback.onExecuteWrite(device, transId, execWrite);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception in callback", ex);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnNotificationSent(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "onNotificationSent() - "
    //     + "device=" + address + ", status=" + status);
    //
    // BluetoothDevice device = mAdapter.getRemoteDevice(address);
    // if (device == null) return;
    //
    // try {
    //     mCallback.onNotificationSent(device, status);
    // } catch (Exception ex) {
    //     Log.w(TAG, "Unhandled exception: " + ex);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                         BluetoothGattServer
//=====================================================================
const String BluetoothGattServer::TAG("BluetoothGattServer");
const Boolean BluetoothGattServer::DBG = true;
const Boolean BluetoothGattServer::VDBG = false;
const Int32 BluetoothGattServer::CALLBACK_REG_TIMEOUT;

CAR_INTERFACE_IMPL_2(BluetoothGattServer, Object, IBluetoothGattServer, IBluetoothProfile);

BluetoothGattServer::BluetoothGattServer()
{
}

BluetoothGattServer::BluetoothGattServer(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothGatt* iGatt,
    /* [in] */ Int32 transport)
{
    // ==================before translated======================
    // mContext = context;
    // mService = iGatt;
    // mAdapter = BluetoothAdapter.getDefaultAdapter();
    // mCallback = null;
    // mServerIf = 0;
    // mTransport = transport;
    // mServices = new ArrayList<BluetoothGattService>();
}

ECode BluetoothGattServer::Close()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "close()");
    // unregisterCallback();
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::RegisterCallback(
    /* [in] */ IBluetoothGattServerCallback* callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(callback);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "registerCallback()");
    // if (mService == null) {
    //     Log.e(TAG, "GATT service not available");
    //     return false;
    // }
    // UUID uuid = UUID.randomUUID();
    // if (DBG) Log.d(TAG, "registerCallback() - UUID=" + uuid);
    //
    // synchronized(mServerIfLock) {
    //     if (mCallback != null) {
    //         Log.e(TAG, "App can register callback only once");
    //         return false;
    //     }
    //
    //     mCallback = callback;
    //     try {
    //         mService.registerServer(new ParcelUuid(uuid), mBluetoothGattServerCallback);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,"",e);
    //         mCallback = null;
    //         return false;
    //     }
    //
    //     try {
    //         mServerIfLock.wait(CALLBACK_REG_TIMEOUT);
    //     } catch (InterruptedException e) {
    //         Log.e(TAG, "" + e);
    //         mCallback = null;
    //     }
    //
    //     if (mServerIf == 0) {
    //         mCallback = null;
    //         return false;
    //     } else {
    //         return true;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::GetService(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 type,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for(BluetoothGattService svc : mServices) {
    //     if (svc.getType() == type &&
    //         svc.getInstanceId() == instanceId &&
    //         svc.getUuid().equals(uuid)) {
    //         return svc;
    //     }
    // }
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Boolean autoConnect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "connect() - device: " + device.getAddress() + ", auto: " + autoConnect);
    // if (mService == null || mServerIf == 0) return false;
    //
    // try {
    //     mService.serverConnect(mServerIf, device.getAddress(),
    //                        autoConnect ? false : true,mTransport); // autoConnect is inverse of "isDirect"
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::CancelConnection(
    /* [in] */ IBluetoothDevice* device)
{
    VALIDATE_NOT_NULL(device);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "cancelConnection() - device: " + device.getAddress());
    // if (mService == null || mServerIf == 0) return;
    //
    // try {
    //     mService.serverDisconnect(mServerIf, device.getAddress());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::SendResponse(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 requestId,
    /* [in] */ Int32 status,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(value);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "sendResponse() - device: " + device.getAddress());
    // if (mService == null || mServerIf == 0) return false;
    //
    // try {
    //     mService.sendResponse(mServerIf, device.getAddress(), requestId,
    //                           status, offset, value);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::NotifyCharacteristicChanged(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Boolean confirm,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(characteristic);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) Log.d(TAG, "notifyCharacteristicChanged() - device: " + device.getAddress());
    // if (mService == null || mServerIf == 0) return false;
    //
    // BluetoothGattService service = characteristic.getService();
    // if (service == null) return false;
    //
    // if (characteristic.getValue() == null) {
    //     throw new IllegalArgumentException("Chracteristic value is empty. Use "
    //             + "BluetoothGattCharacteristic#setvalue to update");
    // }
    //
    // try {
    //     mService.sendNotification(mServerIf, device.getAddress(),
    //             service.getType(), service.getInstanceId(),
    //             new ParcelUuid(service.getUuid()), characteristic.getInstanceId(),
    //             new ParcelUuid(characteristic.getUuid()), confirm,
    //             characteristic.getValue());
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::AddService(
    /* [in] */ IBluetoothGattService* service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(service);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "addService() - service: " + service.getUuid());
    // if (mService == null || mServerIf == 0) return false;
    //
    // mServices.add(service);
    //
    // try {
    //     mService.beginServiceDeclaration(mServerIf, service.getType(),
    //         service.getInstanceId(), service.getHandles(),
    //         new ParcelUuid(service.getUuid()), service.isAdvertisePreferred());
    //
    //     List<BluetoothGattService> includedServices = service.getIncludedServices();
    //     for (BluetoothGattService includedService : includedServices) {
    //         mService.addIncludedService(mServerIf,
    //             includedService.getType(),
    //             includedService.getInstanceId(),
    //             new ParcelUuid(includedService.getUuid()));
    //     }
    //
    //     List<BluetoothGattCharacteristic> characteristics = service.getCharacteristics();
    //     for (BluetoothGattCharacteristic characteristic : characteristics) {
    //         int permission = ((characteristic.getKeySize() - 7) << 12)
    //                             + characteristic.getPermissions();
    //         mService.addCharacteristic(mServerIf,
    //             new ParcelUuid(characteristic.getUuid()),
    //             characteristic.getProperties(), permission);
    //
    //         List<BluetoothGattDescriptor> descriptors = characteristic.getDescriptors();
    //         for (BluetoothGattDescriptor descriptor: descriptors) {
    //             permission = ((characteristic.getKeySize() - 7) << 12)
    //                                 + descriptor.getPermissions();
    //             mService.addDescriptor(mServerIf,
    //                 new ParcelUuid(descriptor.getUuid()), permission);
    //         }
    //     }
    //
    //     mService.endServiceDeclaration(mServerIf);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::RemoveService(
    /* [in] */ IBluetoothGattService* service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(service);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "removeService() - service: " + service.getUuid());
    // if (mService == null || mServerIf == 0) return false;
    //
    // BluetoothGattService intService = getService(service.getUuid(),
    //                         service.getInstanceId(), service.getType());
    // if (intService == null) return false;
    //
    // try {
    //     mService.removeService(mServerIf, service.getType(),
    //         service.getInstanceId(), new ParcelUuid(service.getUuid()));
    //     mServices.remove(intService);
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::ClearServices()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "clearServices()");
    // if (mService == null || mServerIf == 0) return;
    //
    // try {
    //     mService.clearServices(mServerIf);
    //     mServices.clear();
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::GetServices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mServices;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::GetService(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(uuid);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // for (BluetoothGattService service : mServices) {
    //     if (service.getUuid().equals(uuid)) {
    //         return service;
    //     }
    // }
    //
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    // ==================before translated======================
    // throw new UnsupportedOperationException("Use BluetoothManager#getConnectionState instead.");
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::GetConnectedDevices(
    /* [out] */ IList** devices)
{
    // ==================before translated======================
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getConnectedDevices instead.");
    assert(0);
    return NOERROR;
}

ECode BluetoothGattServer::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    // ==================before translated======================
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getDevicesMatchingConnectionStates instead.");
    assert(0);
    return NOERROR;
}

void BluetoothGattServer::UnregisterCallback()
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "unregisterCallback() - mServerIf=" + mServerIf);
    // if (mService == null || mServerIf == 0) return;
    //
    // try {
    //     mCallback = null;
    //     mService.unregisterServer(mServerIf);
    //     mServerIf = 0;
    // } catch (RemoteException e) {
    //     Log.e(TAG,"",e);
    // }
    assert(0);
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
