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

#include "elastos/droid/bluetooth/BluetoothGattServer.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;

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

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::constructor(
    /* [in] */ IBluetoothGattServer* owner)
{
    mOwner = (BluetoothGattServer*)owner;
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnServerRegistered(
    /* [in] */ Int32 status,
    /* [in] */ Int32 serverIf)
{
    if (DBG) Logger::D(TAG, "onServerRegistered() - status=%d, serverIf=%d", status, serverIf);
    {
        AutoLock lock(mOwner->mServerIfLock);
        if (mOwner->mCallback != NULL) {
            mOwner->mServerIf = serverIf;
            mOwner->mServerIfLock->Notify();
        } else {
            // registration timeout
            Logger::E(TAG, "onServerRegistered: mCallback is NULL");
        }
    }
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnScanResult(
    /* [in] */ const String& address,
    /* [in] */ Int32 rssi,
    /* [in] */ ArrayOf<Byte>* advData)
{
    VALIDATE_NOT_NULL(advData);
    if (VDBG) Logger::D(TAG, "onScanResult() - Device=%s, RSSI=%d", address.string(), rssi);
    // // no op
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnServerConnectionState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 serverIf,
    /* [in] */ Boolean connected,
    /* [in] */ const String& address)
{
    if (DBG) Logger::D(TAG, "onServerConnectionState() - status=%d, serverIf=%d, device=%s",
            status, serverIf, address.string());
    //try {
        AutoPtr<IBluetoothDevice> device;
        mOwner->mAdapter->GetRemoteDevice(address, (IBluetoothDevice**)&device);
        mOwner->mCallback->OnConnectionStateChange(device, status,
                                          connected ? IBluetoothProfile::STATE_CONNECTED :
                                          IBluetoothProfile::STATE_DISCONNECTED);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnServiceAdded(
    /* [in] */ Int32 status,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcId)
{
    VALIDATE_NOT_NULL(srvcId);
    AutoPtr<IUUID> srvcUuid;
    srvcId->GetUuid((IUUID**)&srvcUuid);
    if (DBG) Logger::D(TAG, "onServiceAdded() - service=");// + srvcUuid + "status=" + status);

    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(srvcUuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    //try {
    mOwner->mCallback->OnServiceAdded(status, service);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    AutoPtr<IUUID> srvcUuid;
    srvcId->GetUuid((IUUID**)&srvcUuid);
    AutoPtr<IUUID> charUuid;
    charId->GetUuid((IUUID**)&charUuid);
    if (VDBG) Logger::D(TAG, "onCharacteristicReadRequest() - ");
        //+ "service=" + srvcUuid + ", characteristic=" + charUuid);

    AutoPtr<IBluetoothDevice> device;
    mOwner->mAdapter->GetRemoteDevice(address, (IBluetoothDevice**)&device);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(srvcUuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(charUuid, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    //try {
    mOwner->mCallback->OnCharacteristicReadRequest(device, transId, offset, characteristic);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    AutoPtr<IUUID> srvcUuid;
    srvcId->GetUuid((IUUID**)&srvcUuid);
    AutoPtr<IUUID> charUuid;
    charId->GetUuid((IUUID**)&charUuid);
    AutoPtr<IUUID> descrUuid;
    descrId->GetUuid((IUUID**)&descrUuid);
    if (VDBG) Logger::D(TAG, "onCharacteristicReadRequest() - ");
        //+ "service=" + srvcUuid + ", characteristic=" + charUuid
        //+ "descriptor=" + descrUuid);

    AutoPtr<IBluetoothDevice> device;
    mOwner->mAdapter->GetRemoteDevice(address, (IBluetoothDevice**)&device);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(srvcUuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(charUuid, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    AutoPtr<IBluetoothGattDescriptor> descriptor;
    characteristic->GetDescriptor(descrUuid, (IBluetoothGattDescriptor**)&descriptor);
    if (descriptor == NULL) return NOERROR;

    //try {
    mOwner->mCallback->OnDescriptorReadRequest(device, transId, offset, descriptor);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    AutoPtr<IUUID> srvcUuid;
    srvcId->GetUuid((IUUID**)&srvcUuid);
    AutoPtr<IUUID> charUuid;
    charId->GetUuid((IUUID**)&charUuid);
    if (VDBG) Logger::D(TAG, "onCharacteristicWriteRequest() - ");
        //+ "service=" + srvcUuid + ", characteristic=" + charUuid);

    AutoPtr<IBluetoothDevice> device;
    mOwner->mAdapter->GetRemoteDevice(address, (IBluetoothDevice**)&device);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(srvcUuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(charUuid, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    //try {
    mOwner->mCallback->OnCharacteristicWriteRequest(device, transId, characteristic,
                                               isPrep, needRsp, offset, value);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    AutoPtr<IUUID> srvcUuid;
    srvcId->GetUuid((IUUID**)&srvcUuid);
    AutoPtr<IUUID> charUuid;
    charId->GetUuid((IUUID**)&charUuid);
    AutoPtr<IUUID> descrUuid;
    descrId->GetUuid((IUUID**)&descrUuid);
    if (VDBG) Logger::D(TAG, "onDescriptorWriteRequest() - ");
        //+ "service=" + srvcUuid + ", characteristic=" + charUuid
        //+ "descriptor=" + descrUuid);

    AutoPtr<IBluetoothDevice> device;
    mOwner->mAdapter->GetRemoteDevice(address, (IBluetoothDevice**)&device);

    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(srvcUuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(charUuid, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    AutoPtr<IBluetoothGattDescriptor> descriptor;
    characteristic->GetDescriptor(descrUuid, (IBluetoothGattDescriptor**)&descriptor);
    if (descriptor == NULL) return NOERROR;

    //try {
    mOwner->mCallback->OnDescriptorWriteRequest(device, transId, descriptor,
                                           isPrep, needRsp, offset, value);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnExecuteWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 transId,
    /* [in] */ Boolean execWrite)
{
    if (DBG) Logger::D(TAG, "onExecuteWrite() - ");
        //+ "device=" + address + ", transId=" + transId
        //+ "execWrite=" + execWrite);

    AutoPtr<IBluetoothDevice> device;
    mOwner->mAdapter->GetRemoteDevice(address, (IBluetoothDevice**)&device);
    if (device == NULL) return NOERROR;

    //try {
    mOwner->mCallback->OnExecuteWrite(device, transId, execWrite);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
    return NOERROR;
}

ECode BluetoothGattServer::BluetoothGattServerCallbackStub::OnNotificationSent(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    if (VDBG) Logger::D(TAG, "onNotificationSent() - ");
        //+ "device=" + address + ", status=" + status);

    AutoPtr<IBluetoothDevice> device;
    mOwner->mAdapter->GetRemoteDevice(address, (IBluetoothDevice**)&device);
    if (device == NULL) return NOERROR;

    //try {
    mOwner->mCallback->OnNotificationSent(device, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception: " + ex);
    //}
    return NOERROR;
}

//=====================================================================
//                         BluetoothGattServer
//=====================================================================
const String BluetoothGattServer::TAG("BluetoothGattServer");
const Boolean BluetoothGattServer::DBG = TRUE;
const Boolean BluetoothGattServer::VDBG = FALSE;
const Int32 BluetoothGattServer::CALLBACK_REG_TIMEOUT;

CAR_INTERFACE_IMPL_2(BluetoothGattServer, Object, IBluetoothGattServer, IBluetoothProfile);

BluetoothGattServer::BluetoothGattServer()
{
}

BluetoothGattServer::BluetoothGattServer(
    /* [in] */ IContext* context,
    /* [in] */ IIBluetoothGatt* iGatt,
    /* [in] */ Int32 transport)
{
    mContext = context;
    mService = iGatt;
    //mAdapter = BluetoothAdapter.getDefaultAdapter();
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    mCallback = NULL;
    mServerIf = 0;
    mTransport = transport;
    //mServices = new ArrayList<BluetoothGattService>();
    CArrayList::New((IList**)&mServices);
}

ECode BluetoothGattServer::Close()
{
    if (DBG) Logger::D(TAG, "close()");
    UnregisterCallback();
    return NOERROR;
}

ECode BluetoothGattServer::RegisterCallback(
    /* [in] */ IBluetoothGattServerCallback* callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "registerCallback()");
    if (mService == NULL) {
        Logger::E(TAG, "GATT service not available");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IUUIDHelper> uuidHelper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uuidHelper);
    AutoPtr<IUUID> uuid;
    uuidHelper->RandomUUID((IUUID**)&uuid);
    if (DBG) Logger::D(TAG, "registerCallback() - UUID=");// + uuid);

    {
        AutoLock lock(mServerIfLock);
        if (mCallback != NULL) {
            Logger::E(TAG, "App can register callback only once");
            *result = FALSE;
            return NOERROR;
        }

        mCallback = callback;
        //try {
        AutoPtr<IParcelUuid> parcelUuid;
        CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);
        ECode ec = mService->RegisterServer(parcelUuid, mBluetoothGattServerCallback);
        //} catch (RemoteException e) {
        if (FAILED(ec)) {
            //Logger::E(TAG,"",e);
            mCallback = NULL;
            *result = FALSE;
            return NOERROR;
        }
        //}

        //try {
        ec = mServerIfLock->Wait(CALLBACK_REG_TIMEOUT);
        //} catch (InterruptedException e) {
        //    Logger::E(TAG, "" + e);
        if (FAILED(ec)) {
            mCallback = NULL;
        }
        //}

        if (mServerIf == 0) {
            mCallback = NULL;
            *result = FALSE;
            return NOERROR;
        } else {
            *result = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode BluetoothGattServer::GetService(
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 type,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mServices->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        IBluetoothGattService* svc = IBluetoothGattService::Probe(obj);
        Int32 sType;
        Int32 sInstanceId;
        AutoPtr<IUUID> sUuid;
        svc->GetType(&sType);
        svc->GetInstanceId(&sInstanceId);
        svc->GetUuid((IUUID**)&sUuid);
        Boolean eq = FALSE;
        if (sType == type &&
            sInstanceId == instanceId &&
            (sUuid->Equals(uuid, &eq), eq)) {
            *result = svc;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode BluetoothGattServer::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Boolean autoConnect,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (DBG) Logger::D(TAG, "connect() - device: ");// + device.getAddress() + ", auto: " + autoConnect);
    if (mService == NULL || mServerIf == 0) return NOERROR;

    String address;
    device->GetAddress(&address);
    //try {
    ECode ec = mService->ServerConnect(mServerIf, address,
                           autoConnect ? FALSE : TRUE, mTransport); // autoConnect is inverse of "isDirect"
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        return NOERROR;
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattServer::CancelConnection(
    /* [in] */ IBluetoothDevice* device)
{
    if (DBG) Logger::D(TAG, "cancelConnection() - device: ");// + device.getAddress());
    if (mService == NULL || mServerIf == 0) return NOERROR;

    String address;
    device->GetAddress(&address);
    //try {
    mService->ServerDisconnect(mServerIf, address);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //}
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
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (VDBG) Logger::D(TAG, "sendResponse() - device: ");// + device.getAddress());
    if (mService == NULL || mServerIf == 0) return NOERROR;

    String address;
    device->GetAddress(&address);
    //try {
    ECode ec = mService->SendResponse(mServerIf, address, requestId,
                              status, offset, value);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        return FALSE;
    }
    //}
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattServer::NotifyCharacteristicChanged(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Boolean confirm,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (VDBG) Logger::D(TAG, "notifyCharacteristicChanged() - device: ");// + device.getAddress());
    if (mService == NULL || mServerIf == 0) return NOERROR;

    AutoPtr<IBluetoothGattService> service;
    characteristic->GetService((IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<ArrayOf<Byte> > value;
    characteristic->GetValue((ArrayOf<Byte>**)&value);
    if (value == NULL) {
        //throw new IllegalArgumentException("Chracteristic value is empty. Use "
        //        + "BluetoothGattCharacteristic#setvalue to update");
        Logger::E(TAG, "Chracteristic value is empty. Use BluetoothGattCharacteristic#setvalue to update");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //try {
    String address;
    device->GetAddress(&address);
    Int32 type;
    service->GetType(&type);
    Int32 instanceId;
    service->GetInstanceId(&instanceId);
    AutoPtr<IUUID> uuid;
    service->GetUuid((IUUID**)&uuid);
    AutoPtr<IParcelUuid> parcelUuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);
    Int32 cInstanceId;
    characteristic->GetInstanceId(&cInstanceId);
    AutoPtr<IUUID> cuuid;
    characteristic->GetUuid((IUUID**)&cuuid);
    AutoPtr<IParcelUuid> pcUuid;
    CParcelUuid::New(cuuid, (IParcelUuid**)&pcUuid);
    ECode ec = mService->SendNotification(mServerIf, address,
                type, instanceId,
                parcelUuid, cInstanceId,
                pcUuid, confirm,
                value);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        return FALSE;
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattServer::AddService(
    /* [in] */ IBluetoothGattService* service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (DBG) Logger::D(TAG, "addService() - service: ");// + service.getUuid());
    if (mService == NULL || mServerIf == 0) return NOERROR;

    mServices->Add(TO_IINTERFACE(service));

    Int32 type;
    service->GetType(&type);
    Int32 instanceId;
    service->GetInstanceId(&instanceId);
    AutoPtr<IUUID> uuid;
    service->GetUuid((IUUID**)&uuid);
    AutoPtr<IParcelUuid> parcelUuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);
    Boolean preferred;
    service->IsAdvertisePreferred(&preferred);
    Int32 handles = 0;
    service->GetHandles(&handles);
    //try {
        mService->BeginServiceDeclaration(mServerIf, type,
            instanceId, handles,
            parcelUuid, preferred);

        //List<BluetoothGattService> includedServices = service.getIncludedServices();
        AutoPtr<IList> includedServices;
        service->GetIncludedServices((IList**)&includedServices);
        Int32 size;
        includedServices->GetSize(&size);
        for(Int32 i = 0; i < size; ++i) {
            AutoPtr<IInterface> obj;
            includedServices->Get(i, (IInterface**)&obj);
            IBluetoothGattService* includedService = IBluetoothGattService::Probe(obj);
            Int32 iType, iInstanceId;
            includedService->GetType(&iType);
            includedService->GetInstanceId(&iInstanceId);
            AutoPtr<IUUID> uuid;
            includedService->GetUuid((IUUID**)&uuid);
            AutoPtr<IParcelUuid> parcelUuid;
            CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);
            mService->AddIncludedService(mServerIf,
                iType, iInstanceId,
                parcelUuid);
        }

        //List<BluetoothGattCharacteristic> characteristics = service.getCharacteristics();
        AutoPtr<IList> characteristics;
        service->GetCharacteristics((IList**)&characteristics);
        Int32 csize;
        characteristics->GetSize(&csize);
        for(Int32 i = 0; i < csize; ++i) {
            AutoPtr<IInterface> obj;
            characteristics->Get(i, (IInterface**)&obj);
            IBluetoothGattCharacteristic* characteristic = IBluetoothGattCharacteristic::Probe(obj);
            Int32 keySize = 0, permissions = 0;
            characteristic->GetKeySize(&keySize);
            characteristic->GetPermissions(&permissions);
            Int32 permission = ((keySize - 7) << 12)
                                + permissions;
            AutoPtr<IUUID> uuid;
            characteristic->GetUuid((IUUID**)&uuid);
            AutoPtr<IParcelUuid> parcelUuid;
            CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);

            Int32 properties;
            characteristic->GetProperties(&properties);
            mService->AddCharacteristic(mServerIf,
                parcelUuid,
                properties, permission);

            //List<BluetoothGattDescriptor> descriptors = characteristic.getDescriptors();
            AutoPtr<IList> descriptors;
            characteristic->GetDescriptors((IList**)&descriptors);
            Int32 size;
            descriptors->GetSize(&size);
            for(Int32 i = 0; i < size; ++i) {
                AutoPtr<IInterface> obj;
                IBluetoothGattDescriptor* descriptor = IBluetoothGattDescriptor::Probe(obj);
                Int32 dPermissions;
                descriptor->GetPermissions(&dPermissions);
                permission = ((keySize - 7) << 12)
                                    + dPermissions;
                AutoPtr<IUUID> uuid;
                descriptor->GetUuid((IUUID**)&uuid);
                AutoPtr<IParcelUuid> parcelUuid;
                CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);
                mService->AddDescriptor(mServerIf, parcelUuid, permission);
            }
        }

        mService->EndServiceDeclaration(mServerIf);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //    return FALSE;
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattServer::RemoveService(
    /* [in] */ IBluetoothGattService* service,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (DBG) Logger::D(TAG, "removeService() - service: ");// + service.getUuid());
    if (mService == NULL || mServerIf == 0) return NOERROR;

    AutoPtr<IBluetoothGattService> intService;
    AutoPtr<IUUID> uuid;
    service->GetUuid((IUUID**)&uuid);
    Int32 instanceId, type;
    service->GetInstanceId(&instanceId);
    service->GetType(&type);
    GetService(uuid, instanceId, type, (IBluetoothGattService**)&intService);
    if (intService == NULL) return NOERROR;

    AutoPtr<IParcelUuid> parcelUuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);
    //try {
    ECode ec = mService->RemoveService(mServerIf, type,
            instanceId, parcelUuid);
    mServices->Remove(intService);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        return FALSE;
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGattServer::ClearServices()
{
    if (DBG) Logger::D(TAG, "clearServices()");
    if (mService == NULL || mServerIf == 0) return NOERROR;

    //try {
    mService->ClearServices(mServerIf);
    mServices->Clear();
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //}
    return NOERROR;
}

ECode BluetoothGattServer::GetServices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServices;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothGattServer::GetService(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mServices->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        IBluetoothGattService* service = IBluetoothGattService::Probe(obj);
        AutoPtr<IUUID> sUuid;
        service->GetUuid((IUUID**)&sUuid);
        Boolean eq = FALSE;
        if (sUuid->Equals(uuid, &eq), eq) {
            *result = service;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
     }

    *result = NULL;
    return NOERROR;
}

ECode BluetoothGattServer::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    // throw new UnsupportedOperationException("Use BluetoothManager#getConnectionState instead.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode BluetoothGattServer::GetConnectedDevices(
    /* [out] */ IList** devices)
{
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getConnectedDevices instead.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode BluetoothGattServer::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getDevicesMatchingConnectionStates instead.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

void BluetoothGattServer::UnregisterCallback()
{
    if (DBG) Logger::D(TAG, "unregisterCallback() - mServerIf=");// + mServerIf);
    if (mService == NULL || mServerIf == 0) return;

    //try {
        mCallback = NULL;
        mService->UnregisterServer(mServerIf);
        mServerIf = 0;
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //}
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
