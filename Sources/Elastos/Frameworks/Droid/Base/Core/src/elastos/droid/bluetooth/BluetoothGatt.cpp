#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/BluetoothGatt.h"
#include "elastos/droid/bluetooth/BluetoothGattService.h"
#include "elastos/droid/bluetooth/BluetoothGattCharacteristic.h"
#include "elastos/droid/bluetooth/BluetoothGattDescriptor.h"
#include "elastos/droid/os/CParcelUuid.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//           BluetoothGatt::InnerBluetoothGattCallbackWrapper
//=====================================================================
BluetoothGatt::InnerBluetoothGattCallbackWrapper::InnerBluetoothGattCallbackWrapper()
{
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::constructor(
    /* [in] */ IBluetoothGatt* owner)
{
    mOwner = (BluetoothGatt*)owner;
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnClientRegistered(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf)
{
    if (DBG) Logger::D(TAG, "onClientRegistered() - status=%d, clientIf=%d ", status, clientIf);
    if (VDBG) {
        {
            AutoLock lock(mOwner->mStateLock);
            if (mOwner->mConnState != CONN_STATE_CONNECTING) {
                Logger::E(TAG, "Bad connection state: %d", mOwner->mConnState);
            }
        }
    }
    mOwner->mClientIf = clientIf;
    if (status != GATT_SUCCESS) {
        mOwner->mCallback->OnConnectionStateChange(mOwner, GATT_FAILURE,
                                          IBluetoothProfile::STATE_DISCONNECTED);
        {
            AutoLock lock(mOwner->mStateLock);
            mOwner->mConnState = CONN_STATE_IDLE;
        }
        return NOERROR;
    }
    //try {
    String address;
    mOwner->mDevice->GetAddress(&address);
    mOwner->mService->ClientConnect(mOwner->mClientIf, address, !mOwner->mAutoConnect, mOwner->mTransport);
    // autoConnect is inverse of "isDirect"
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //}
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnClientConnectionState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf,
    /* [in] */ Boolean connected,
    /* [in] */ const String& address)
{
    if (DBG) Logger::D(TAG, "onClientConnectionState() - status=%d, clientIf=%d, devices=%s",
            status, clientIf, address.string());
    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }
    Int32 profileState = connected ? IBluetoothProfile::STATE_CONNECTED :
                                   IBluetoothProfile::STATE_DISCONNECTED;
    //try {
    mOwner->mCallback->OnConnectionStateChange(mOwner, status, profileState);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}

    {
        AutoLock lock(mOwner->mStateLock);
        if (connected) {
            mOwner->mConnState = CONN_STATE_CONNECTED;
        } else {
            mOwner->mConnState = CONN_STATE_IDLE;
        }
    }

    {
        AutoLock lock(mOwner->mDeviceBusyLock);
        mOwner->mDeviceBusy = FALSE;
    }
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnGetService(
    /* [in] */ const String& address,
    /* [in] */ Int32 srvcType,
    /* [in] */ Int32 srvcInstId,
    /* [in] */ IParcelUuid* srvcUuid)
{
    if (VDBG) Logger::D(TAG, "onGetService() - Device=%s, UUID= xx", address.string()); //+ " UUID=" + srvcUuid;
    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);
    AutoPtr<IBluetoothGattService> bgService =
        new BluetoothGattService(mOwner->mDevice, uuid, srvcInstId, srvcType);
    mOwner->mServices->Add(bgService);
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
    if (VDBG) Logger::D(TAG, "onGetIncludedService() - Device=%s", address.string());
        //+ " UUID=" + srvcUuid + " Included=" + inclSrvcUuid;

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }
    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);

    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);

    AutoPtr<IUUID> iuuid;
    inclSrvcUuid->GetUuid((IUUID**)&iuuid);
    AutoPtr<IBluetoothGattService> includedService;
    mOwner->GetService(mOwner->mDevice, iuuid, inclSrvcInstId, inclSrvcType, (IBluetoothGattService**)&includedService);

    if (service != NULL && includedService != NULL) {
        service->AddIncludedService(includedService);
    }
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
    if (VDBG) Logger::D(TAG, "onGetCharacteristic() - Device=%s", address.string());
    //+ " UUID=" + charUuid;

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);

    AutoPtr<IUUID> cuuid;
    srvcUuid->GetUuid((IUUID**)&cuuid);
    if (service != NULL) {
        AutoPtr<BluetoothGattCharacteristic> bgc = new BluetoothGattCharacteristic(
                service, cuuid, charInstId, charProps, 0);
        Boolean res;
        service->AddCharacteristic(bgc, &res);
    }
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
    if (VDBG) Logger::D(TAG, "onGetDescriptor() - Device=%s", address.string());// + " UUID=" + descUuid;

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);

    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);

    if (service == NULL) return NOERROR;

    AutoPtr<IUUID> cuuid;
    charUuid->GetUuid((IUUID**)&cuuid);
    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(cuuid, charInstId, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    AutoPtr<IUUID> duuid;
    descUuid->GetUuid((IUUID**)&duuid);
    Boolean addok;
    characteristic->AddDescriptor(new BluetoothGattDescriptor(
        characteristic, duuid, descrInstId, 0), &addok);
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnSearchComplete(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    if (DBG) Logger::D(TAG, "onSearchComplete() = Device=%s", address.string());// + " Status=" + status;
    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    //try {
    mOwner->mCallback->OnServicesDiscovered(mOwner, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    if (VDBG) Logger::D(TAG, "onCharacteristicRead() - Device=%s", address.string());
               // + " UUID=" + charUuid + " Status=" + status);

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    {
        AutoLock lock(mOwner->mDeviceBusyLock);
        mOwner->mDeviceBusy = FALSE;
    }

    if ((status == GATT_INSUFFICIENT_AUTHENTICATION
      || status == GATT_INSUFFICIENT_ENCRYPTION)
      && mOwner->mAuthRetry == FALSE) {
        //try {
            mOwner->mAuthRetry = TRUE;
            mOwner->mService->ReadCharacteristic(mOwner->mClientIf, address,
                srvcType, srvcInstId, srvcUuid,
                charInstId, charUuid, AUTHENTICATION_MITM);
            return NOERROR;
        //} catch (RemoteException e) {
        //    Logger::E(TAG,"",e);
        //}
    }

    mOwner->mAuthRetry = FALSE;

    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IUUID> cuuid;
    charUuid->GetUuid((IUUID**)&cuuid);
    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(cuuid, charInstId, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    Boolean setok;
    if (status == 0) characteristic->SetValue(value, &setok);

    //try {
    mOwner->mCallback->OnCharacteristicRead(mOwner, characteristic, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}

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
    if (VDBG) Logger::D(TAG, "onCharacteristicWrite() - Device=%s", address.string());
                //+ " UUID=" + charUuid + " Status=" + status);

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    {
        AutoLock lock(mOwner->mDeviceBusyLock);
        mOwner->mDeviceBusy = FALSE;
    }

    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IUUID> cuuid;
    charUuid->GetUuid((IUUID**)&cuuid);
    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(cuuid, charInstId, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    if ((status == GATT_INSUFFICIENT_AUTHENTICATION
      || status == GATT_INSUFFICIENT_ENCRYPTION)
      && mOwner->mAuthRetry == FALSE) {
        //try {
            mOwner->mAuthRetry = TRUE;
            Int32 writeType;
            characteristic->GetWriteType(&writeType);
            AutoPtr<ArrayOf<Byte> > value;
            characteristic->GetValue((ArrayOf<Byte>**)&value);
            mOwner->mService->WriteCharacteristic(mOwner->mClientIf, address,
                srvcType, srvcInstId, srvcUuid, charInstId, charUuid,
                writeType, AUTHENTICATION_MITM,
                value);
            return NOERROR;
        //} catch (RemoteException e) {
        //    Logger::E(TAG,"",e);
        //}
    }

    mOwner->mAuthRetry = FALSE;

    //try {
    mOwner->mCallback->OnCharacteristicWrite(mOwner, characteristic, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    if (VDBG) Logger::D(TAG, "onNotify() - Device=%s", address.string());// + " UUID=" + charUuid);

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }
    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);

    if (service == NULL) return NOERROR;

    AutoPtr<IUUID> cuuid;
    charUuid->GetUuid((IUUID**)&cuuid);
    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(cuuid, charInstId, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    Boolean setok;
    characteristic->SetValue(value, &setok);

    //try {
    mOwner->mCallback->OnCharacteristicChanged(mOwner, characteristic);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    if (VDBG) Logger::D(TAG, "onDescriptorRead() - Device=%s",  address.string());// + " UUID=" + charUuid);

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    {
        AutoLock lock(mOwner->mDeviceBusyLock);
        mOwner->mDeviceBusy = FALSE;
    }

    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IUUID> cuuid;
    charUuid->GetUuid((IUUID**)&cuuid);
    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(cuuid, charInstId, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    AutoPtr<IUUID> duuid;
    descrUuid->GetUuid((IUUID**)&duuid);
    AutoPtr<IBluetoothGattDescriptor> descriptor;
    characteristic->GetDescriptor(duuid, descrInstId, (IBluetoothGattDescriptor**)&descriptor);
    if (descriptor == NULL) return NOERROR;

    Boolean setok;
    if (status == 0) descriptor->SetValue(value, &setok);

    if ((status == GATT_INSUFFICIENT_AUTHENTICATION
      || status == GATT_INSUFFICIENT_ENCRYPTION)
      && mOwner->mAuthRetry == FALSE) {
        //try {
            mOwner->mAuthRetry = TRUE;
            mOwner->mService->ReadDescriptor(mOwner->mClientIf, address,
                srvcType, srvcInstId, srvcUuid, charInstId, charUuid,
                descrInstId, descrUuid, AUTHENTICATION_MITM);
            return NOERROR;
        //} catch (RemoteException e) {
        //    Logger::E(TAG,"",e);
        //}
    }

    mOwner->mAuthRetry = TRUE;

    //try {
    mOwner->mCallback->OnDescriptorRead(mOwner, descriptor, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
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
    if (VDBG) Logger::D(TAG, "onDescriptorWrite() - Device=%s", address.string());// + " UUID=" + charUuid);

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    {
        AutoLock lock(mOwner->mDeviceBusyLock);
        mOwner->mDeviceBusy = FALSE;
    }

    AutoPtr<IUUID> uuid;
    srvcUuid->GetUuid((IUUID**)&uuid);
    AutoPtr<IBluetoothGattService> service;
    mOwner->GetService(mOwner->mDevice, uuid, srvcInstId, srvcType, (IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IUUID> cuuid;
    charUuid->GetUuid((IUUID**)&cuuid);
    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    service->GetCharacteristic(cuuid, charInstId, (IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    AutoPtr<IUUID> duuid;
    descrUuid->GetUuid((IUUID**)&duuid);
    AutoPtr<IBluetoothGattDescriptor> descriptor;
    characteristic->GetDescriptor(duuid, descrInstId, (IBluetoothGattDescriptor**)&descriptor);
    if (descriptor == NULL) return NOERROR;

    if ((status == GATT_INSUFFICIENT_AUTHENTICATION
      || status == GATT_INSUFFICIENT_ENCRYPTION)
      && mOwner->mAuthRetry == FALSE) {
        //try {
            mOwner->mAuthRetry = TRUE;
            Int32 writeType;
            characteristic->GetWriteType(&writeType);
            AutoPtr<ArrayOf<Byte> > value;
            descriptor->GetValue((ArrayOf<Byte>**)&value);
            mOwner->mService->WriteDescriptor(mOwner->mClientIf, address,
                srvcType, srvcInstId, srvcUuid, charInstId, charUuid,
                descrInstId, descrUuid, writeType,
                AUTHENTICATION_MITM, value);
            return NOERROR;
        //} catch (RemoteException e) {
        //    Logger::E(TAG,"",e);
        //}
    }

    mOwner->mAuthRetry = FALSE;

    //try {
    mOwner->mCallback->OnDescriptorWrite(mOwner, descriptor, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnExecuteWrite(
    /* [in] */ const String& address,
    /* [in] */ Int32 status)
{
    if (VDBG) Logger::D(TAG, "onExecuteWrite() - Device=%s", address.string());
        //+ " status=" + status);

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }

    {
        AutoLock lock(mOwner->mDeviceBusyLock);
        mOwner->mDeviceBusy = FALSE;
    }

    //try {
    mOwner->mCallback->OnReliableWriteCompleted(mOwner, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnReadRemoteRssi(
    /* [in] */ const String& address,
    /* [in] */ Int32 rssi,
    /* [in] */ Int32 status)
{
    if (VDBG) Logger::D(TAG, "onReadRemoteRssi() - Device=%s,rssi=%d,status=%d", address.string(), rssi, status);

    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }
    //try {
    mOwner->mCallback->OnReadRemoteRssi(mOwner, rssi, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
    return NOERROR;
}

ECode BluetoothGatt::InnerBluetoothGattCallbackWrapper::OnConfigureMTU(
    /* [in] */ const String& address,
    /* [in] */ Int32 mtu,
    /* [in] */ Int32 status)
{
    if (DBG) Logger::D(TAG, "onConfigureMTU() - Device=%s,mtu=%d,status=%d", address.string(), mtu, status);
    String tAddress;
    mOwner->mDevice->GetAddress(&tAddress);
    if (!address.Equals(tAddress)) {
        return NOERROR;
    }
    //try {
    mOwner->mCallback->OnMtuChanged(mOwner, mtu, status);
    //} catch (Exception ex) {
    //    Log.w(TAG, "Unhandled exception in callback", ex);
    //}
    return NOERROR;
}

//=====================================================================
//                            BluetoothGatt
//=====================================================================
const String BluetoothGatt::TAG("BluetoothGatt");
const Boolean BluetoothGatt::DBG = TRUE;
const Boolean BluetoothGatt::VDBG = FALSE;
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
    /* [in] */ IIBluetoothGatt* iGatt,
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 transport)
{
    mContext = context;
    mService = iGatt;
    mDevice = device;
    mTransport = transport;
    //mServices = new ArrayList<BluetoothGattService>();
    CArrayList::New((IList**)&mServices);

    mConnState = CONN_STATE_IDLE;
}

ECode BluetoothGatt::Close()
{
    if (DBG) Logger::D(TAG, "close()");

    AutoPtr<IBluetoothAdapter> adapter = CBluetoothAdapter::GetDefaultAdapter();
    Int32 state;
    adapter->GetState(&state);
    if (state != IBluetoothAdapter::STATE_ON) return NOERROR;
    UnregisterApp();
    mConnState = CONN_STATE_CLOSED;
    return NOERROR;
}

ECode BluetoothGatt::GetService(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IUUID* uuid,
    /* [in] */ Int32 instanceId,
    /* [in] */ Int32 type,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    mServices->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        IBluetoothGattService* svc = IBluetoothGattService::Probe(obj);
        AutoPtr<IBluetoothDevice> d;
        svc->GetDevice((IBluetoothDevice**)&d);
        Int32 t;
        svc->GetType(&t);
        Int32 id;
        svc->GetInstanceId(&id);
        AutoPtr<IUUID> u;
        svc->GetUuid((IUUID**)&u);

        Boolean de = FALSE, ue = FALSE;
        if ((IObject::Probe(d)->Equals(device, &de), de) &&
            t == type &&
            id == instanceId &&
            (u->Equals(uuid, &ue), ue)) {
            *result = svc;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode BluetoothGatt::Connect(
    /* [in] */ Boolean autoConnect,
    /* [in] */ IBluetoothGattCallback* callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    String tAddress;
    mDevice->GetAddress(&tAddress);
    if (DBG) Logger::D(TAG, "connect() - device: %s, auto: %d", tAddress.string(), autoConnect);
    {
        AutoLock lock(mStateLock);
        if (mConnState != CONN_STATE_IDLE) {
            //throw new IllegalStateException("Not idle");
            Logger::E(TAG, "Not idle");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mConnState = CONN_STATE_CONNECTING;
    }
    if (!RegisterApp(callback)) {
        {
            AutoLock lock(mStateLock);
            mConnState = CONN_STATE_IDLE;
        }
        Logger::E(TAG, "Failed to register callback");
        return FALSE;
    }

    // the connection will continue after successful callback registration
    mAutoConnect = autoConnect;
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::Disconnect()
{
    String tAddress;
    mDevice->GetAddress(&tAddress);
    if (DBG) Logger::D(TAG, "cancelOpen() - device: %s", tAddress.string());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    AutoPtr<IBluetoothAdapter> adapter = CBluetoothAdapter::GetDefaultAdapter();
    Int32 state;
    adapter->GetState(&state);
    if (state != IBluetoothAdapter::STATE_ON) return NOERROR;

    //try {
    mService->ClientDisconnect(mClientIf, tAddress);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //}
    return NOERROR;
}

ECode BluetoothGatt::Connect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    //try {
        String tAddress;
        mDevice->GetAddress(&tAddress);
        ECode ec = mService->ClientConnect(mClientIf, tAddress,
                               FALSE, mTransport); // autoConnect is inverse of "isDirect"
        if (!FAILED(ec)) {
            *result = TRUE;
        }
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //    return FALSE;
    //}
    return NOERROR;
}

ECode BluetoothGatt::GetDevice(
    /* [out] */ IBluetoothDevice** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDevice;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode BluetoothGatt::DiscoverServices(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    String tAddress;
    mDevice->GetAddress(&tAddress);
    if (DBG) Logger::D(TAG, "discoverServices() - device: %s", tAddress.string());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    mServices->Clear();

    //try {
    ECode ec = mService->DiscoverServices(mClientIf, tAddress);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //    return FALSE;
    //}
    if (!FAILED(ec)) {
        *result = TRUE;
    }
    return NOERROR;
}

ECode BluetoothGatt::GetServices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    //List<BluetoothGattService> result = new ArrayList<BluetoothGattService>();
    CArrayList::New(result);

    Int32 size;
    mServices->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        IBluetoothGattService* svc = IBluetoothGattService::Probe(obj);
        AutoPtr<IBluetoothDevice> d;
        svc->GetDevice((IBluetoothDevice**)&d);
        Boolean eq = FALSE;
        if (IObject::Probe(d)->Equals(mDevice, &eq), eq) {
            (*result)->Add(svc);
        }
    }

    return NOERROR;
}

ECode BluetoothGatt::GetService(
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothGattService** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    Int32 size;
    mServices->GetSize(&size);
    for(Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        mServices->Get(i, (IInterface**)&obj);
        IBluetoothGattService* svc = IBluetoothGattService::Probe(obj);
        AutoPtr<IBluetoothDevice> d;
        svc->GetDevice((IBluetoothDevice**)&d);
        AutoPtr<IUUID> u;
        svc->GetUuid((IUUID**)&u);
        Boolean de = FALSE, ue = FALSE;
        if ((IObject::Probe(d)->Equals(mDevice, &de), de) &&
            (IObject::Probe(d)->Equals(uuid, &ue), ue)) {
            *result = svc;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode BluetoothGatt::ReadCharacteristic(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 properties;
    if (((characteristic->GetProperties(&properties), properties) &
            IBluetoothGattCharacteristic::PROPERTY_READ) == 0) return NOERROR;

    if (VDBG) Logger::D(TAG, "readCharacteristic() - uuid: ");// + characteristic.getUuid());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    AutoPtr<IBluetoothGattService> service;
    characteristic->GetService((IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothDevice> device;
    service->GetDevice((IBluetoothDevice**)&device);
    if (device == NULL) return NOERROR;

    {
        AutoLock lock(mDeviceBusyLock);
        if (mDeviceBusy) return NOERROR;
        mDeviceBusy = TRUE;
    }

    //try {
        String tAddress;
        device->GetAddress(&tAddress);
        Int32 type;
        service->GetType(&type);
        Int32 instanceId;
        service->GetInstanceId(&instanceId);
        AutoPtr<IUUID> uuid;
        service->GetUuid((IUUID**)&uuid);
        AutoPtr<IParcelUuid> puuid;
        CParcelUuid::New(uuid, (IParcelUuid**)&puuid);
        Int32 cid;
        characteristic->GetInstanceId(&cid);
        AutoPtr<IUUID> cuuid;
        characteristic->GetUuid((IUUID**)&cuuid);
        AutoPtr<IParcelUuid> pcuuid;
        CParcelUuid::New(cuuid, (IParcelUuid**)&pcuuid);

        ECode ec = mService->ReadCharacteristic(mClientIf, tAddress,
            type, instanceId,
            puuid, cid,
            pcuuid, AUTHENTICATION_NONE);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
        if (FAILED(ec)) {
            mDeviceBusy = FALSE;
            return NOERROR;
        }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::WriteCharacteristic(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 properties;
    characteristic->GetProperties(&properties);
    if ((properties & IBluetoothGattCharacteristic::PROPERTY_WRITE) == 0
        && (properties &
            IBluetoothGattCharacteristic::PROPERTY_WRITE_NO_RESPONSE) == 0) return NOERROR;

    if (VDBG) Logger::D(TAG, "writeCharacteristic() - uuid: ");// + characteristic.getUuid());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    AutoPtr<IBluetoothGattService> service;
    characteristic->GetService((IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothDevice> device;
    service->GetDevice((IBluetoothDevice**)&device);
    if (device == NULL) return NOERROR;

    {
        AutoLock lock(mDeviceBusyLock);
        if (mDeviceBusy) return NOERROR;
        mDeviceBusy = TRUE;
    }

    //try {
    String tAddress;
    device->GetAddress(&tAddress);
    Int32 type;
    service->GetType(&type);
    Int32 instanceId;
    service->GetInstanceId(&instanceId);
    Int32 cInstanceId;
    characteristic->GetInstanceId(&cInstanceId);
    Int32 writeType;
    characteristic->GetWriteType(&writeType);

    AutoPtr<IUUID> uuid;
    service->GetUuid((IUUID**)&uuid);
    AutoPtr<IParcelUuid> puuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&puuid);

    AutoPtr<IUUID> cuuid;
    characteristic->GetUuid((IUUID**)&cuuid);
    AutoPtr<IParcelUuid> pcuuid;
    CParcelUuid::New(cuuid, (IParcelUuid**)&pcuuid);

    AutoPtr<ArrayOf<Byte> > value;
    characteristic->GetValue((ArrayOf<Byte>**)&value);

        ECode ec = mService->WriteCharacteristic(mClientIf, tAddress,
            type, instanceId,
            puuid, cInstanceId,
            pcuuid,
            writeType, AUTHENTICATION_NONE,
            value);
    //} catch (RemoteException e) {
        if (FAILED(ec)) {
            //Logger::E(TAG,"",e);
            mDeviceBusy = FALSE;
            return NOERROR;
        }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::ReadDescriptor(
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (VDBG) Logger::D(TAG, "readDescriptor() - uuid: ");// + descriptor.getUuid());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    descriptor->GetCharacteristic((IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    AutoPtr<IBluetoothGattService> service;
    characteristic->GetService((IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothDevice> device;
    service->GetDevice((IBluetoothDevice**)&device);
    if (device == NULL) return NOERROR;

    {
        AutoLock lock(mDeviceBusyLock);
        if (mDeviceBusy) return NOERROR;
        mDeviceBusy = TRUE;
    }

    //try {
    String tAddress;
    device->GetAddress(&tAddress);
    Int32 type;
    service->GetType(&type);
    Int32 instanceId;
    service->GetInstanceId(&instanceId);
    Int32 cInstanceId;
    characteristic->GetInstanceId(&cInstanceId);
    Int32 dInstanceId;
    descriptor->GetInstanceId(&dInstanceId);

    AutoPtr<IUUID> uuid;
    service->GetUuid((IUUID**)&uuid);
    AutoPtr<IParcelUuid> puuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&puuid);

    AutoPtr<IUUID> cuuid;
    characteristic->GetUuid((IUUID**)&cuuid);
    AutoPtr<IParcelUuid> pcuuid;
    CParcelUuid::New(cuuid, (IParcelUuid**)&pcuuid);

    AutoPtr<IUUID> duuid;
    characteristic->GetUuid((IUUID**)&duuid);
    AutoPtr<IParcelUuid> pduuid;
    CParcelUuid::New(duuid, (IParcelUuid**)&pduuid);

        ECode ec = mService->ReadDescriptor(mClientIf, tAddress, type,
            instanceId, puuid,
            cInstanceId, pcuuid,
            dInstanceId, pduuid,
            AUTHENTICATION_NONE);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        mDeviceBusy = FALSE;
        return NOERROR;
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::WriteDescriptor(
    /* [in] */ IBluetoothGattDescriptor* descriptor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (VDBG) Logger::D(TAG, "writeDescriptor() - uuid: ");// + descriptor.getUuid());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    AutoPtr<IBluetoothGattCharacteristic> characteristic;
    descriptor->GetCharacteristic((IBluetoothGattCharacteristic**)&characteristic);
    if (characteristic == NULL) return NOERROR;

    AutoPtr<IBluetoothGattService> service;
    characteristic->GetService((IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothDevice> device;
    service->GetDevice((IBluetoothDevice**)&device);
    if (device == NULL) return NOERROR;

    {
        AutoLock lock(mDeviceBusyLock);
        if (mDeviceBusy) return NOERROR;
        mDeviceBusy = TRUE;
    }

    //try {
    String tAddress;
    device->GetAddress(&tAddress);
    Int32 type;
    service->GetType(&type);
    Int32 instanceId;
    service->GetInstanceId(&instanceId);
    Int32 cInstanceId;
    characteristic->GetInstanceId(&cInstanceId);
    Int32 dInstanceId;
    descriptor->GetInstanceId(&dInstanceId);
    Int32 writeType;
    characteristic->GetWriteType(&writeType);
    AutoPtr<ArrayOf<Byte> > value;
    descriptor->GetValue((ArrayOf<Byte>**)&value);

    AutoPtr<IUUID> uuid;
    service->GetUuid((IUUID**)&uuid);
    AutoPtr<IParcelUuid> puuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&puuid);

    AutoPtr<IUUID> cuuid;
    characteristic->GetUuid((IUUID**)&cuuid);
    AutoPtr<IParcelUuid> pcuuid;
    CParcelUuid::New(cuuid, (IParcelUuid**)&pcuuid);

    AutoPtr<IUUID> duuid;
    characteristic->GetUuid((IUUID**)&duuid);
    AutoPtr<IParcelUuid> pduuid;
    CParcelUuid::New(duuid, (IParcelUuid**)&pduuid);

        ECode ec = mService->WriteDescriptor(mClientIf, tAddress, type,
            instanceId, puuid,
            cInstanceId, pcuuid,
            dInstanceId, pduuid,
            writeType, AUTHENTICATION_NONE,
            value);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        mDeviceBusy = FALSE;
        return NOERROR;
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::BeginReliableWrite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (VDBG) Logger::D(TAG, "beginReliableWrite() - device: ");// + mDevice.getAddress());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    String address;
    mDevice->GetAddress(&address);
    //try {
        ECode ec = mService->BeginReliableWrite(mClientIf, address);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec))
        return NOERROR;
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::ExecuteReliableWrite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (VDBG) Logger::D(TAG, "executeReliableWrite() - device: ");// + mDevice.getAddress());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    {
        AutoLock lock(mDeviceBusyLock);
        if (mDeviceBusy) return NOERROR;
        mDeviceBusy = TRUE;
    }

    String address;
    mDevice->GetAddress(&address);
    //try {
        ECode ec = mService->EndReliableWrite(mClientIf, address, TRUE);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        mDeviceBusy = FALSE;
        return NOERROR;
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::AbortReliableWrite()
{
    if (VDBG) Logger::D(TAG, "abortReliableWrite() - device: ");// + mDevice.getAddress());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    String address;
    mDevice->GetAddress(&address);
    //try {
        mService->EndReliableWrite(mClientIf, address, FALSE);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //}
    return NOERROR;
}

ECode BluetoothGatt::AbortReliableWrite(
    /* [in] */ IBluetoothDevice* mDevice)
{
    AbortReliableWrite();
    return NOERROR;
}

ECode BluetoothGatt::SetCharacteristicNotification(
    /* [in] */ IBluetoothGattCharacteristic* characteristic,
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (DBG) Logger::D(TAG, "setCharacteristicNotification() - uuid: ");
    // + characteristic.getUuid() + " enable: " + enable);
    if (mService == NULL || mClientIf == 0) return NOERROR;

    AutoPtr<IBluetoothGattService> service;
    characteristic->GetService((IBluetoothGattService**)&service);
    if (service == NULL) return NOERROR;

    AutoPtr<IBluetoothDevice> device;
    service->GetDevice((IBluetoothDevice**)&device);
    if (device == NULL) return NOERROR;

    String address;
    device->GetAddress(&address);
    Int32 type;
    service->GetType(&type);
    Int32 instanceId;
    service->GetInstanceId(&instanceId);
    Int32 cInstanceId;
    characteristic->GetInstanceId(&cInstanceId);

    AutoPtr<IUUID> uuid;
    service->GetUuid((IUUID**)&uuid);
    AutoPtr<IParcelUuid> puuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&puuid);

    AutoPtr<IUUID> cuuid;
    characteristic->GetUuid((IUUID**)&cuuid);
    AutoPtr<IParcelUuid> pcuuid;
    CParcelUuid::New(cuuid, (IParcelUuid**)&pcuuid);

    //try {
        ECode ec = mService->RegisterForNotification(mClientIf, address,
            type, instanceId,
            puuid, cInstanceId,
            pcuuid,
            enable);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec))
        return FALSE;
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::Refresh(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (DBG) Logger::D(TAG, "refresh() - device: ");// + mDevice.getAddress());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    String address;
    mDevice->GetAddress(&address);
    //try {
        ECode ec = mService->RefreshDevice(mClientIf, address);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec)) {
        return NOERROR;
    }
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::ReadRemoteRssi(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (DBG) Logger::D(TAG, "readRssi() - device: ");// + mDevice.getAddress());
    if (mService == NULL || mClientIf == 0) return NOERROR;

    String address;
    mDevice->GetAddress(&address);
    //try {
        ECode ec = mService->ReadRemoteRssi(mClientIf, address);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec))
        return FALSE;
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::RequestMtu(
    /* [in] */ Int32 mtu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (DBG) Logger::D(TAG, "configureMTU() - device: ");//
    //+ mDevice.getAddress() + " mtu: " + mtu);
    if (mService == NULL || mClientIf == 0) return NOERROR;

    String address;
    mDevice->GetAddress(&address);
    //try {
        ECode ec = mService->ConfigureMTU(mClientIf, address, mtu);
    //} catch (RemoteException e) {
        //Logger::E(TAG,"",e);
    if (FAILED(ec))
        return FALSE;
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::RequestConnectionPriority(
    /* [in] */ Int32 connectionPriority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (connectionPriority < CONNECTION_PRIORITY_BALANCED ||
        connectionPriority > CONNECTION_PRIORITY_LOW_POWER) {
        //throw new IllegalArgumentException("connectionPriority not within valid range");
        Logger::E(TAG, "connectionPriority not within valid range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (DBG) Logger::D(TAG, "requestConnectionPriority() - params: %d", connectionPriority);
    if (mService == NULL || mClientIf == 0) return NOERROR;

    String address;
    mDevice->GetAddress(&address);
    //try {
        ECode ec = mService->ConnectionParameterUpdate(mClientIf, address, connectionPriority);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec))
        return FALSE;
    //}

    *result = TRUE;
    return NOERROR;
}

ECode BluetoothGatt::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    //VALIDATE_NOT_NULL(state);
    // throw new UnsupportedOperationException("Use BluetoothManager#getConnectionState instead.");
    Logger::E(TAG, "Use BluetoothManager#getConnectionState instead.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode BluetoothGatt::GetConnectedDevices(
    /* [out] */ IList** devices)
{
    //VALIDATE_NOT_NULL(devices);
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getConnectedDevices instead.");
    Logger::E(TAG, "Use BluetoothManager#getConnectedDevices instead.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode BluetoothGatt::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    //VALIDATE_NOT_NULL(devices);
    // throw new UnsupportedOperationException
    //     ("Use BluetoothManager#getDevicesMatchingConnectionStates instead.");
    Logger::E(TAG, "Use BluetoothManager#getDevicesMatchingConnectionStates instead.");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

Boolean BluetoothGatt::RegisterApp(
    /* [in] */ IBluetoothGattCallback* callback)
{
    if (DBG) Logger::D(TAG, "registerApp()");
    if (mService == NULL) return FALSE;

    mCallback = callback;
    AutoPtr<IUUIDHelper> uuidHelper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uuidHelper);
    AutoPtr<IUUID> uuid;
    uuidHelper->RandomUUID((IUUID**)&uuid);
    if (DBG) Logger::D(TAG, "registerApp() - UUID=");// + uuid);

    AutoPtr<IParcelUuid> puuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&puuid);
    //try {
        ECode ec = mService->RegisterClient(puuid, mBluetoothGattCallback);
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    if (FAILED(ec))
        return FALSE;
    //}

    return TRUE;
}

void BluetoothGatt::UnregisterApp()
{
    if (DBG) Logger::D(TAG, "unregisterApp() - mClientIf=%d", mClientIf);
    if (mService == NULL || mClientIf == 0) return;

    //try {
        mCallback = NULL;
        mService->UnregisterClient(mClientIf);
        mClientIf = 0;
    //} catch (RemoteException e) {
    //    Logger::E(TAG,"",e);
    //}
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
