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

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/bluetooth/le/CScanSettings.h"
#include "elastos/droid/bluetooth/le/ScanFilter.h"
#include "elastos/droid/bluetooth/le/BluetoothLeAdvertiser.h"
#include "elastos/droid/bluetooth/le/BluetoothLeScanner.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CBluetoothDevice.h"
#include "elastos/droid/bluetooth/BluetoothServerSocket.h"
#include "elastos/droid/bluetooth/BluetoothHeadset.h"
#include "elastos/droid/bluetooth/BluetoothA2dp.h"
#include "elastos/droid/bluetooth/BluetoothA2dpSink.h"
#include "elastos/droid/bluetooth/BluetoothAvrcpController.h"
#include "elastos/droid/bluetooth/BluetoothHeadsetClient.h"
#include "elastos/droid/bluetooth/BluetoothHealth.h"
#include "elastos/droid/bluetooth/BluetoothInputDevice.h"
#include "elastos/droid/bluetooth/BluetoothMap.h"
#include "elastos/droid/bluetooth/BluetoothPan.h"
#include "elastos/droid/bluetooth/BluetoothSocket.h"
#include "elastos/droid/bluetooth/BluetoothDun.h"
#include "elastos/droid/bluetooth/BluetoothSap.h"
#include "elastos/droid/bluetooth/BluetoothGatt.h"
#include "elastos/droid/bluetooth/BluetoothGattServer.h"
#include "elastos/droid/bluetooth/BluetoothHidDevice.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::Bluetooth::LE::CScanSettings;
using Elastos::Droid::Bluetooth::LE::IBluetoothLeAdvertiser;
using Elastos::Droid::Bluetooth::LE::BluetoothLeAdvertiser;
using Elastos::Droid::Bluetooth::LE::IBluetoothLeScanner;
using Elastos::Droid::Bluetooth::LE::BluetoothLeScanner;
using Elastos::Droid::Bluetooth::LE::IScanCallback;
using Elastos::Droid::Bluetooth::LE::IScanFilter;
using Elastos::Droid::Bluetooth::LE::IScanRecord;
using Elastos::Droid::Bluetooth::LE::IScanResult;
using Elastos::Droid::Bluetooth::LE::IScanSettings;
using Elastos::Droid::Bluetooth::LE::ScanFilter;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

const String CBluetoothAdapter::TAG("BluetoothAdapter");
const Boolean CBluetoothAdapter::DBG = TRUE;
const Boolean CBluetoothAdapter::VDBG = FALSE;
const Int32 CBluetoothAdapter::ADDRESS_LENGTH;
AutoPtr<IBluetoothAdapter> CBluetoothAdapter::sAdapter;
AutoPtr<IBluetoothLeScanner> CBluetoothAdapter::sBluetoothLeScanner;
AutoPtr<IBluetoothLeAdvertiser> CBluetoothAdapter::sBluetoothLeAdvertiser;
Object CBluetoothAdapter::sLock;

CBluetoothAdapter::BluetoothAdapterScanCallback::BluetoothAdapterScanCallback(
    /* [in] */ CBluetoothAdapter* owner,
    /* [in] */ ArrayOf<IUUID*>* serviceUuids,
    /* [in] */ IBluetoothAdapterLeScanCallback* callback)
    : mOwner(owner)
    , mServiceUuids(serviceUuids)
    , mCallback(callback)
{
}

ECode CBluetoothAdapter::BluetoothAdapterScanCallback::OnScanResult(
    /* [in] */ Int32 callbackType,
    /* [in] */ IScanResult* result)
{
    if (callbackType != IScanSettings::CALLBACK_TYPE_ALL_MATCHES) {
        // Should not happen.
        Logger::E(TAG, "LE Scan has already started");
        return NOERROR;
    }
    AutoPtr<IScanRecord> scanRecord;
    result->GetScanRecord((IScanRecord**)&scanRecord);
    if (scanRecord == NULL) {
        return NOERROR;
    }
    if (mServiceUuids != NULL) {
        //List<ParcelUuid> uuids = new ArrayList<ParcelUuid>();
        AutoPtr<IList> uuids;
        CArrayList::New((IList**)&uuids);
        //for (UUID uuid : serviceUuids) {
        //    uuids.add(new ParcelUuid(uuid));
        //}
        for (Int32 i = 0; i < mServiceUuids->GetLength(); ++i) {
            AutoPtr<IUUID> uuid = (*mServiceUuids)[i];
            AutoPtr<IParcelUuid> puuid;
            CParcelUuid::New(uuid, (IParcelUuid**)&puuid);
            uuids->Add(TO_IINTERFACE(puuid));
        }

        //List<ParcelUuid> scanServiceUuids = scanRecord.getServiceUuids();
        AutoPtr<IList> scanServiceUuids;
        scanRecord->GetServiceUuids((IList**)&scanServiceUuids);
        Boolean contains;
        if (scanServiceUuids == NULL || !(scanServiceUuids->ContainsAll(Elastos::Utility::ICollection::Probe(uuids), &contains), contains)) {
            if (DBG) Logger::D(TAG, "uuids does not match");
            return NOERROR;
        }
    }
    AutoPtr<IBluetoothDevice> device;
    result->GetDevice((IBluetoothDevice**)&device);
    Int32 rssi;
    result->GetRssi(&rssi);
    AutoPtr<ArrayOf<Byte> > bytes;
    scanRecord->GetBytes((ArrayOf<Byte>**)&bytes);
    mCallback->OnLeScan(device, rssi, bytes);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CBluetoothAdapter, Object, IBluetoothAdapter);

CAR_OBJECT_IMPL(CBluetoothAdapter);

CBluetoothAdapter::CBluetoothAdapter()
{
    CBluetoothAdapterManagerCallback::New(this, (IIBluetoothManagerCallback**)&mManagerCallback);
}

CBluetoothAdapter::~CBluetoothAdapter()
{
    // try {
    mManagerService->UnregisterAdapter(mManagerCallback);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "", e);
    // } finally {
    //     super.finalize();
    // }
}

AutoPtr<IBluetoothAdapter> CBluetoothAdapter::GetDefaultAdapter()
{
    AutoLock lock(sLock);
    if (sAdapter == NULL) {

        AutoPtr<IInterface> b = ServiceManager::GetService(BLUETOOTH_MANAGER_SERVICE);
        if (b != NULL) {
            AutoPtr<IIBluetoothManager> managerService = IIBluetoothManager::Probe(b);
            CBluetoothAdapter::New(managerService, (IBluetoothAdapter**)&sAdapter);
        }
        else {
            Logger::E(TAG, "Bluetooth binder is null");
        }
    }

    return sAdapter;
}

ECode CBluetoothAdapter::GetRemoteDevice(
    /* [in] */ const String& address,
    /* [out] */ IBluetoothDevice** device)
{
    VALIDATE_NOT_NULL(device)
    return CBluetoothDevice::New(address, device);
}

ECode CBluetoothAdapter::GetRemoteDevice(
    /* [in] */ ArrayOf<Byte>* address,
    /* [out] */ IBluetoothDevice** device)
{
    VALIDATE_NOT_NULL(device)

    if (address == NULL || address->GetLength() != 6) {
//        throw new IllegalArgumentException("Bluetooth address must have 6 bytes");
        Logger::E(TAG, "Bluetooth address must have 6 bytes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String add;
    add.AppendFormat(/*Local.US, */"%02X:%02X:%02X:%02X:%02X:%02X", (*address)[0], (*address)[1], (*address)[2], (*address)[3], (*address)[4], (*address)[5]);
    return CBluetoothDevice::New(add, device);
}

ECode CBluetoothAdapter::GetBluetoothLeAdvertiser(
    /* [out] */ IBluetoothLeAdvertiser** btAdvertiser)
{
    VALIDATE_NOT_NULL(btAdvertiser);
    Int32 state;
    if ((GetState(&state), state) != STATE_ON) {
        *btAdvertiser = NULL;
        return NOERROR;
    }
    Boolean support = FALSE;
    if (!(IsMultipleAdvertisementSupported(&support), support)) {
        *btAdvertiser = NULL;
        return NOERROR;
    }
    {
        AutoLock lock(mLock);
        if (sBluetoothLeAdvertiser == NULL) {
            AutoPtr<BluetoothLeAdvertiser> bla = new BluetoothLeAdvertiser();//TODO check if use Cxx + constructor
            bla->constructor(mManagerService);
            sBluetoothLeAdvertiser = bla;
        }
    }
    *btAdvertiser = sBluetoothLeAdvertiser;
    REFCOUNT_ADD(*btAdvertiser)
    return NOERROR;
}

ECode CBluetoothAdapter::GetBluetoothLeScanner(
    /* [out] */ IBluetoothLeScanner** btLeScanner)
{
    VALIDATE_NOT_NULL(btLeScanner);
    Int32 state;
    if ((GetState(&state), state) != STATE_ON) {
        *btLeScanner = NULL;
        return NOERROR;
    }
    {
        AutoLock lock(mLock);
        if (sBluetoothLeScanner == NULL) {
            //TODO check if use Cxx + constructor
            AutoPtr<BluetoothLeScanner> bls = new BluetoothLeScanner();
            bls->constructor(mManagerService);
            sBluetoothLeScanner = bls;
        }
    }
    *btLeScanner = sBluetoothLeScanner;
    REFCOUNT_ADD(*btLeScanner)
    return NOERROR;
}

ECode CBluetoothAdapter::IsEnabled(
    /* [out] */ Boolean* isEnable)
{
    VALIDATE_NOT_NULL(isEnable)
    *isEnable = FALSE;
//    try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) {
        ECode ec = mService->IsEnabled(isEnable);
        if(FAILED(ec)) {
            Logger::E(TAG, "0x%08x", ec);
            *isEnable = FALSE;
        }
    }
//   } catch (RemoteException e) {Log.e(TAG, "", e);}
    return NOERROR;
}

ECode CBluetoothAdapter::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = STATE_OFF;

//    try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) {
        return mService->GetState(state);
        // if (VDBG) Log.d(TAG, "" + hashCode() + ": getState(). Returning " + state);
        // return state;
    }
        // TODO(BT) there might be a small gap during STATE_TURNING_ON that
        //          mService is null, handle that case
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    if (DBG) Logger::D(TAG, /*"" + hashCode() + */": getState() :  mService = null. Returning STATE_OFF");
    return NOERROR;
}

ECode CBluetoothAdapter::Enable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean isEnable;
    if (IsEnabled(&isEnable), isEnable) {
        if (DBG) Logger::D(TAG, "enable(): BT is already enabled..!");
        *result = TRUE;
        return NOERROR;
    }
    // try {
    return mManagerService->Enable(CActivityThread::GetCurrentPackageName(), result);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
}

ECode CBluetoothAdapter::Disable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mManagerService->Disable(TRUE, result);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
}

ECode CBluetoothAdapter::Disable(
    /* [in] */ Boolean persist,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    return mManagerService->Disable(persist, result);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
}

ECode CBluetoothAdapter::GetAddress(
    /* [out] */ String* address)
{
    VALIDATE_NOT_NULL(address)
    // try {
    return mManagerService->GetAddress(address);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return null;
}

ECode CBluetoothAdapter::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    // try {
    return mManagerService->GetName(name);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return null;
}

ECode CBluetoothAdapter::ConfigHciSnoopLog(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
    {
        AutoLock lock(mManagerCallback);
        if (mService != NULL) return mService->ConfigHciSnoopLog(enabled, result);
    }
    //} catch (RemoteException e) {Log.e(TAG, "", e);}
    *result = FALSE;
    return NOERROR;
}

ECode CBluetoothAdapter::GetUuids(
    /* [out, callee] */ ArrayOf<Elastos::Droid::Os::IParcelUuid *>** uuids)
{
    VALIDATE_NOT_NULL(uuids)
    *uuids = NULL;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->GetUuids(uuids);
    // }
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    return NOERROR;
}

ECode CBluetoothAdapter::SetName(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->SetName(name, result);
    // }
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
    return NOERROR;
}

ECode CBluetoothAdapter::GetScanMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode)
    *mode = SCAN_MODE_NONE;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->GetScanMode(mode);
    // }
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return SCAN_MODE_NONE;
    return NOERROR;
}

ECode CBluetoothAdapter::SetScanMode(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 duration,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->SetScanMode(mode, duration, result);
    // }
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
    return NOERROR;
}

ECode CBluetoothAdapter::SetScanMode(
    /* [in] */ Int32 mode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    /* getDiscoverableTimeout() to use the latest from NV than use 0 */
    Int32 timeout;
    GetDiscoverableTimeout(&timeout);
    return SetScanMode(mode, timeout, result);
}

ECode CBluetoothAdapter::GetDiscoverableTimeout(
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    *timeout = -1;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->GetDiscoverableTimeout(timeout);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return -1;
    return NOERROR;
}

ECode CBluetoothAdapter::SetDiscoverableTimeout(
    /* [in] */ Int32 timeout)
{
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    Boolean result;
    if (mService != NULL) return mService->SetDiscoverableTimeout(timeout, &result);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    return NOERROR;
}

ECode CBluetoothAdapter::StartDiscovery(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->StartDiscovery(result);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
    return NOERROR;
}

ECode CBluetoothAdapter::CancelDiscovery(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->CancelDiscovery(result);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
    return NOERROR;
}

ECode CBluetoothAdapter::IsDiscovering(
    /* [out] */ Boolean* isDiscovering)
{
    VALIDATE_NOT_NULL(isDiscovering)
    *isDiscovering = FALSE;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->IsDiscovering(isDiscovering);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    return NOERROR;
}

ECode CBluetoothAdapter::IsMultipleAdvertisementSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 state;
    if ((GetState(&state), state) != STATE_ON) return NOERROR;
    //try {
    return mService->IsMultiAdvertisementSupported(result);
    //} catch (RemoteException e) {
    //    Log.e(TAG, "failed to get isMultipleAdvertisementSupported, error: ", e);
    //}
    return NOERROR;
}

ECode CBluetoothAdapter::IsOffloadedFilteringSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 state;
    if ((GetState(&state), state) != STATE_ON) return NOERROR;
    //try {
    return mService->IsOffloadedFilteringSupported(result);
    //} catch (RemoteException e) {
    //    Log.e(TAG, "failed to get isOffloadedFilteringSupported, error: ", e);
    //}
    return NOERROR;
}

ECode CBluetoothAdapter::IsOffloadedScanBatchingSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Int32 state;
    if ((GetState(&state), state) != STATE_ON) return NOERROR;
    //try {
    return mService->IsOffloadedScanBatchingSupported(result);
    //} catch (RemoteException e) {
    //    Log.e(TAG, "failed to get isOffloadedScanBatchingSupported, error: ", e);
    //}
    return NOERROR;
}

ECode CBluetoothAdapter::GetControllerActivityEnergyInfo(
    /* [in] */ Int32 updateType,
    /* [out] */ IBluetoothActivityEnergyInfo** btActivityEnergyInfo)
{
    VALIDATE_NOT_NULL(btActivityEnergyInfo);
    *btActivityEnergyInfo = NULL;
    Int32 state;
    if ((GetState(&state), state) != STATE_ON) return NOERROR;
    //try {
        AutoPtr<IBluetoothActivityEnergyInfo> record;
        Boolean supported = FALSE;
        if (!(mService->IsActivityAndEnergyReportingSupported(&supported), supported)) {
            return NOERROR;
        }
        {
            AutoLock lock(this);
            if (updateType == ACTIVITY_ENERGY_INFO_REFRESHED) {
                mService->GetActivityEnergyInfoFromController();
                Wait(CONTROLLER_ENERGY_UPDATE_TIMEOUT_MILLIS);
            }
            mService->ReportActivityInfo((IBluetoothActivityEnergyInfo**)&record);
            Boolean isValid;
            if (record->IsValid(&isValid)) {
                *btActivityEnergyInfo = record;
                REFCOUNT_ADD(*btActivityEnergyInfo)
                return NOERROR;
            }
            else {
                return NOERROR;
            }
        }
    //} catch (InterruptedException e) {
    //    Log.e(TAG, "getControllerActivityEnergyInfoCallback wait interrupted: " + e);
    //} catch (RemoteException e) {
    //    Log.e(TAG, "getControllerActivityEnergyInfoCallback: " + e);
    //}
    return NOERROR;
}

ECode CBluetoothAdapter::GetBondedDevices(
    /* [out, callee] */ ArrayOf<IBluetoothDevice*>** device)
{
    VALIDATE_NOT_NULL(device)
    *device = NULL;

    Int32 state;
    if (GetState(&state), state != STATE_ON) {
        *device = ArrayOf<IBluetoothDevice*>::Alloc(0);
        REFCOUNT_ADD(*device)
        return NOERROR;
    }
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->GetBondedDevices(device);
    // return toDeviceSet(new BluetoothDevice[0]);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return null;
    return NOERROR;
}

ECode CBluetoothAdapter::GetConnectionState(
    /* [out] */ Int32* connetionState)
{
    VALIDATE_NOT_NULL(connetionState)
    *connetionState = STATE_DISCONNECTED;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->GetAdapterConnectionState(connetionState);
    // } catch (RemoteException e) {Log.e(TAG, "getConnectionState:", e);}
    // return BluetoothAdapter.STATE_DISCONNECTED;
    return NOERROR;
}

ECode CBluetoothAdapter::GetProfileConnectionState(
    /* [in] */ Int32 profile,
    /* [out] */ Int32* connetionState)
{
    VALIDATE_NOT_NULL(connetionState)
    *connetionState = IBluetoothProfile::STATE_DISCONNECTED;
    Int32 state;
    if (GetState(&state), state != STATE_ON) return NOERROR;
    // try {
    AutoLock lock(mManagerCallbackLock);
    if (mService != NULL) return mService->GetProfileConnectionState(profile, connetionState);
    // } catch (RemoteException e) {
    //     Log.e(TAG, "getProfileConnectionState:", e);
    // }
    // return BluetoothProfile.STATE_DISCONNECTED;
    return NOERROR;
}

ECode CBluetoothAdapter::ListenUsingRfcommOn(
    /* [in] */ Int32 channel,
    /* [out] */ IBluetoothServerSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;
    AutoPtr<BluetoothServerSocket> serverSocket = new BluetoothServerSocket(
            BluetoothSocket::TYPE_RFCOMM, TRUE, TRUE, channel);
    Int32 terrno = serverSocket->mSocket->BindListen();
    if (terrno != 0) {
        //TODO(BT): Throw the same exception error code
        // that the previous code was using.
        //socket.mSocket.throwErrnoNative(errno);
        // throw new IOException("Error: " + errno);
        Logger::E(TAG, "Error: %d", terrno);
        return E_IO_EXCEPTION;
    }
    *socket = serverSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CBluetoothAdapter::ListenUsingRfcommWithServiceRecord(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothServerSocket** socket)
{
    return CreateNewRfcommSocketAndRecord(name, uuid, TRUE, TRUE, socket);
}

ECode CBluetoothAdapter::ListenUsingL2capWithServiceRecord(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothServerSocket** socket)
{
    return CreateNewL2capSocketAndRecord(name, uuid, TRUE, TRUE, socket);
}

ECode CBluetoothAdapter::ListenUsingInsecureL2capWithServiceRecord(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothServerSocket** socket)
{
    return CreateNewL2capSocketAndRecord(name, uuid, FALSE, FALSE, socket);
}

ECode CBluetoothAdapter::ListenUsingInsecureRfcommWithServiceRecord(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothServerSocket** socket)
{
    return CreateNewRfcommSocketAndRecord(name, uuid, FALSE, FALSE, socket);
}

ECode CBluetoothAdapter::ListenUsingEncryptedRfcommWithServiceRecord(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [out] */ IBluetoothServerSocket** socket)
{
    return CreateNewRfcommSocketAndRecord(name, uuid, FALSE, TRUE, socket);
}

ECode CBluetoothAdapter::CreateNewRfcommSocketAndRecord(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [in] */ Boolean auth,
    /* [in] */ Boolean encrypt,
    /* [out] */ IBluetoothServerSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;

    AutoPtr<IParcelUuid> parcelUuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);

    AutoPtr<BluetoothServerSocket> serverSocket = new BluetoothServerSocket(
            BluetoothSocket::TYPE_RFCOMM, auth, encrypt, parcelUuid);
    serverSocket->SetServiceName(name);
    Int32 terrno = serverSocket->mSocket->BindListen();
    if (terrno != 0) {
        //TODO(BT): Throw the same exception error code
        // that the previous code was using.
        //socket.mSocket.throwErrnoNative(errno);
        // throw new IOException("Error: " + errno);
        Logger::E(TAG, "Error: %d", terrno);
        return E_IO_EXCEPTION;
    }
    *socket = (IBluetoothServerSocket*)serverSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CBluetoothAdapter::CreateNewL2capSocketAndRecord(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [in] */ Boolean auth,
    /* [in] */ Boolean encrypt,
    /* [out] */ IBluetoothServerSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;

    AutoPtr<IParcelUuid> parcelUuid;
    CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);

    AutoPtr<BluetoothServerSocket> serverSocket = new BluetoothServerSocket(
            BluetoothSocket::TYPE_L2CAP, auth, encrypt, parcelUuid);
    serverSocket->SetServiceName(name);
    Int32 terrno = serverSocket->mSocket->BindListen();
    if (terrno != 0) {
        //TODO(BT): Throw the same exception error code
        // that the previous code was using.
        //socket.mSocket.throwErrnoNative(errno);
        // throw new IOException("Error: " + errno);
        Logger::E(TAG, "Error: %d", terrno);
        return E_IO_EXCEPTION;
    }
    *socket = (IBluetoothServerSocket*)serverSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CBluetoothAdapter::ListenUsingInsecureRfcommOn(
    /* [in] */ Int32 port,
    /* [out] */ IBluetoothServerSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;
    AutoPtr<BluetoothServerSocket> serverSocket = new BluetoothServerSocket(
            BluetoothSocket::TYPE_RFCOMM, FALSE, FALSE, port);
    Int32 terrno = serverSocket->mSocket->BindListen();
    if (terrno != 0) {
        //TODO(BT): Throw the same exception error code
        // that the previous code was using.
        //socket.mSocket.throwErrnoNative(errno);
        // throw new IOException("Error: " + errno);
        Logger::E(TAG, "Error: %d", terrno);
        return E_IO_EXCEPTION;
    }
    *socket = serverSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CBluetoothAdapter::ListenUsingEncryptedRfcommOn(
    /* [in] */ Int32 port,
    /* [out] */ IBluetoothServerSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;
    AutoPtr<BluetoothServerSocket> serverSocket = new BluetoothServerSocket(
            BluetoothSocket::TYPE_RFCOMM, FALSE, TRUE, port);
    Int32 terrno = serverSocket->mSocket->BindListen();
    if (terrno < 0) {
        //TODO(BT): Throw the same exception error code
        // that the previous code was using.
        //socket.mSocket.throwErrnoNative(errno);
        Logger::E(TAG, "Error: %d", terrno);
        return E_IO_EXCEPTION;
    }
    *socket = serverSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CBluetoothAdapter::ListenUsingScoOn(
    /* [out] */ IBluetoothServerSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    *socket = NULL;
    AutoPtr<BluetoothServerSocket> serverSocket = new BluetoothServerSocket(
            BluetoothSocket::TYPE_SCO, FALSE, FALSE, -1);
    Int32 terrno = serverSocket->mSocket->BindListen();
    if (terrno < 0) {
        //TODO(BT): Throw the same exception error code
        // that the previous code was using.
        //socket.mSocket.throwErrnoNative(errno);
    }
    *socket = serverSocket;
    REFCOUNT_ADD(*socket)
    return NOERROR;
}

ECode CBluetoothAdapter::ReadOutOfBandData(
    /* [out] */ IInterface** data)
{
    VALIDATE_NOT_NULL(data)
    *data = NULL;
    return NOERROR;
    // if (getState() != STATE_ON) return null;
    //TODO(BT
    /*
    try {
        byte[] hash;
        byte[] randomizer;

        byte[] ret = mService.readOutOfBandData();

        if (ret  == null || ret.length != 32) return null;

        hash = Arrays.copyOfRange(ret, 0, 16);
        randomizer = Arrays.copyOfRange(ret, 16, 32);

        if (DBG) {
            Log.d(TAG, "readOutOfBandData:" + Arrays.toString(hash) +
              ":" + Arrays.toString(randomizer));
        }
        return new Pair<byte[], byte[]>(hash, randomizer);

    } catch (RemoteException e) {Log.e(TAG, "", e);}*/
    // return null;
}

ECode CBluetoothAdapter::GetProfileProxy(
    /* [in] */ Elastos::Droid::Content::IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* listener,
    /* [in] */ Int32 profile,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (context == NULL || listener == NULL) {
        return NOERROR;
    }

    if (profile == IBluetoothProfile::HEADSET) {
        AutoPtr<BluetoothHeadset> obj = new BluetoothHeadset(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::A2DP) {
        AutoPtr<BluetoothA2dp> obj = new BluetoothA2dp(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::A2DP_SINK) {
        AutoPtr<IBluetoothA2dpSink> a2dpSink = new BluetoothA2dpSink(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::AVRCP_CONTROLLER) {
        AutoPtr<IBluetoothAvrcpController> avrcp = new BluetoothAvrcpController(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::INPUT_DEVICE) {
        AutoPtr<BluetoothInputDevice> obj = new BluetoothInputDevice(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::PAN) {
        AutoPtr<BluetoothPan> obj = new BluetoothPan(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::DUN) {
        AutoPtr<BluetoothDun> obj = new BluetoothDun(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::SAP) {
        AutoPtr<BluetoothSap> obj = new BluetoothSap(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::HEALTH) {
        AutoPtr<BluetoothHealth> obj = new BluetoothHealth(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::MAP) {
        AutoPtr<IBluetoothMap> map = new BluetoothMap(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::HEADSET_CLIENT) {
        AutoPtr<IBluetoothHeadsetClient> headsetClient = new BluetoothHeadsetClient(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else if (profile == IBluetoothProfile::HID_DEVICE) {
        AutoPtr<BluetoothHidDevice> headsetClient = new BluetoothHidDevice(context, listener);
        *result = TRUE;
        return NOERROR;
    }
    else {
        return NOERROR;
    }
}

ECode CBluetoothAdapter::CloseProfileProxy(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    if (proxy == NULL) {
        return NOERROR;
    }

    switch (profile) {
        case IBluetoothProfile::HEADSET:
            ((BluetoothHeadset*)proxy)->Close();
            break;
        case IBluetoothProfile::A2DP:
            ((BluetoothA2dp*)proxy)->Close();
            break;
        case IBluetoothProfile::A2DP_SINK:
            ((BluetoothA2dpSink*)proxy)->Close();
            break;
        case IBluetoothProfile::AVRCP_CONTROLLER:
            ((BluetoothAvrcpController*)proxy)->Close();
            break;
        case IBluetoothProfile::INPUT_DEVICE:
            ((BluetoothInputDevice*)proxy)->Close();
            break;
        case IBluetoothProfile::PAN:
            ((BluetoothPan*)proxy)->Close();
            break;
        case IBluetoothProfile::DUN:
            ((BluetoothDun*)proxy)->Close();
            break;
        case IBluetoothProfile::SAP:
            ((BluetoothSap*)proxy)->Close();
            break;
        case IBluetoothProfile::HEALTH:
            ((BluetoothHealth*)proxy)->Close();
            break;
        case IBluetoothProfile::GATT:
            ((BluetoothGatt*)proxy)->Close();
            break;
        case IBluetoothProfile::GATT_SERVER:
            ((BluetoothGattServer*)proxy)->Close();
            break;
        case IBluetoothProfile::MAP:
            ((BluetoothMap*)proxy)->Close();
            break;
        case IBluetoothProfile::HEADSET_CLIENT:
            ((BluetoothHeadsetClient*)proxy)->Close();
            break;
        case IBluetoothProfile::HID_DEVICE:
            ((BluetoothHidDevice*)proxy)->Close();
            break;
    }

    return NOERROR;
}

ECode CBluetoothAdapter::EnableNoAutoConnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    Boolean isEnabled;
    if (IsEnabled(&isEnabled), isEnabled){
        if (DBG) Logger::D(TAG, "enableNoAutoConnect(): BT is already enabled..!");
        return NOERROR;
    }
    // try {
    return mManagerService->EnableNoAutoConnect(result);
    // } catch (RemoteException e) {Log.e(TAG, "", e);}
    // return false;
}

ECode CBluetoothAdapter::ChangeApplicationBluetoothState(
    /* [in] */ Boolean on,
    /* [in] */ IIBluetoothStateChangeCallback* stateChangeCallback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
    // if (callback == null) return false;

    //TODO(BT)
    /*
    try {
       return mService.changeApplicationBluetoothState(on, new
               StateChangeCallbackWrapper(callback), new Binder());
    } catch (RemoteException e) {
       Log.e(TAG, "changeBluetoothState", e);
    }*/
    // return false;
}

ECode CBluetoothAdapter::StartLeScan(
    /* [in] */ IBluetoothAdapterLeScanCallback* cb,
    /* [out] */ Boolean* result)
{
    Int32 state;
    GetState(&state);
    if (state != STATE_ON) {
        *result = FALSE;
        return NOERROR;
    }
    return StartLeScan(NULL, cb, result);
}

ECode CBluetoothAdapter::StartLeScan(
    /* [in] */ ArrayOf<IUUID*>* serviceUuids,
    /* [in] */ IBluetoothAdapterLeScanCallback* cb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "startLeScan(): ");// + serviceUuids;
    if (cb == NULL) {
        if (DBG) Logger::E(TAG, "startLeScan: null callback");
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IBluetoothLeScanner> scanner;
    GetBluetoothLeScanner((IBluetoothLeScanner**)&scanner);
    if (scanner == NULL) {
        if (DBG) Logger::E(TAG, "startLeScan: cannot get BluetoothLeScanner");
        *result = FALSE;
        return NOERROR;
    }

    {
        AutoLock lock(mLeScanClients);
        Boolean contains = FALSE;
        if (mLeScanClients->ContainsKey(TO_IINTERFACE(cb), &contains), contains) {
            if (DBG) Logger::E(TAG, "LE Scan has already started");
            *result = FALSE;
            return NOERROR;
        }

        //try {
            AutoPtr<IIBluetoothGatt> iGatt;
            mManagerService->GetBluetoothGatt((IIBluetoothGatt**)&iGatt);
            if (iGatt == NULL) {
                // BLE is not supported
                *result = FALSE;
                return NOERROR;
            }

            AutoPtr<IScanCallback> scanCallback = new BluetoothAdapterScanCallback(this, serviceUuids, cb);

            //ScanSettings settings = new ScanSettings.Builder()
            //    .setCallbackType(ScanSettings.CALLBACK_TYPE_ALL_MATCHES)
            //    .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY).build();
            AutoPtr<IScanSettings> settings;
            CScanSettings::New(IScanSettings::SCAN_MODE_LOW_LATENCY, IScanSettings::CALLBACK_TYPE_ALL_MATCHES,
                    IScanSettings::SCAN_RESULT_TYPE_FULL, 0, (IScanSettings**)&settings);

            //List<ScanFilter> filters = new ArrayList<ScanFilter>();
            AutoPtr<IList> filters;
            CArrayList::New((IList**)&filters);
            if (serviceUuids != NULL && serviceUuids->GetLength() > 0) {
                // Note scan filter does not support matching an UUID array so we put one
                // UUID to hardware and match the whole array in callback.
                //ScanFilter filter = new ScanFilter.Builder().setServiceUuid(
                //        new ParcelUuid(serviceUuids[0])).build();
                AutoPtr<IParcelUuid> puuid;
                CParcelUuid::New((*serviceUuids)[0], (IParcelUuid**)&puuid);
                AutoPtr<ScanFilter::Builder> builder = new ScanFilter::Builder();
                builder->SetServiceUuid(puuid);
                AutoPtr<IScanFilter> filter;
                builder->Build((IScanFilter**)&filter);
                filters->Add(TO_IINTERFACE(filter));
            }
            scanner->StartScan(filters, settings, scanCallback);

            mLeScanClients->Put(TO_IINTERFACE(cb), TO_IINTERFACE(scanCallback));
            *result = TRUE;
            return NOERROR;

        //} catch (RemoteException e) {
        //    Log.e(TAG,"",e);
        //}
    }
    *result = FALSE;
    return NOERROR;
}

ECode CBluetoothAdapter::StopLeScan(
    /* [in] */ IBluetoothAdapterLeScanCallback* cb)
{
    if (DBG) Logger::D(TAG, "stopLeScan()");
    Int32 state;
    GetState(&state);
    if (state != STATE_ON) {
        return NOERROR;
    }
    AutoPtr<IBluetoothLeScanner> scanner;
    GetBluetoothLeScanner((IBluetoothLeScanner**)&scanner);
    if (scanner == NULL) {
        return NOERROR;
    }
    {
        AutoLock lock(mLeScanClients);
        AutoPtr<IInterface> value;
        mLeScanClients->Remove(TO_IINTERFACE(cb), (IInterface**)&value);
        AutoPtr<IScanCallback> scanCallback = IScanCallback::Probe(value);
        if (scanCallback == NULL) {
            if (DBG) Logger::D(TAG, "scan not started yet");
            return NOERROR;
        }
        scanner->StopScan(scanCallback);
    }
    return NOERROR;
}

Boolean CBluetoothAdapter::CheckBluetoothAddress(
    /* [in] */ const String& address)
{
    if (address.IsNull() || address.GetLength() != (UInt32)ADDRESS_LENGTH) {
        return FALSE;
    }
    for (Int32 i = 0; i < ADDRESS_LENGTH; i++) {
        Char32 c = address.GetChar(i);
        switch (i % 3) {
            case 0:
            case 1:
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
                    // hex character, OK
                    break;
                }
                return FALSE;
            case 2:
                if (c == ':') {
                    break;  // OK
                }
                return FALSE;
        }
    }
    return TRUE;
}

AutoPtr<IIBluetoothManager> CBluetoothAdapter::GetBluetoothManager()
{
    return mManagerService;
}

AutoPtr<IIBluetooth> CBluetoothAdapter::GetBluetoothService(
    /* [in] */ IIBluetoothManagerCallback* _cb)
{
    AutoPtr<IIBluetoothManagerCallback> cb = _cb;
    AutoLock lock(mManagerCallbackLock);
    if (cb == NULL) {
        Logger::W(TAG, "getBluetoothService() called with no BluetoothManagerCallback");
    }
    else if (Find(mProxyServiceStateCallbacks.Begin(),
            mProxyServiceStateCallbacks.End(), cb) != mProxyServiceStateCallbacks.End()) {
        mProxyServiceStateCallbacks.PushBack(cb);
    }
    return mService;
}

void CBluetoothAdapter::RemoveServiceStateCallback(
    /* [in] */ IIBluetoothManagerCallback* cb)
{
    AutoLock lock(mManagerCallbackLock);
    mProxyServiceStateCallbacks.Remove(cb);
}

ECode CBluetoothAdapter::constructor(
    /* [in] */ IIBluetoothManager* managerService)
{
    if (managerService == NULL) {
        // throw new IllegalArgumentException("bluetooth manager service is null");
        Logger::E(TAG, "bluetooth manager service is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

//    try {
    ECode ec = managerService->RegisterAdapter(mManagerCallback, (IIBluetooth**)&mService);
    if (FAILED(ec)) {
        Logger::E(TAG, "0x%08x", ec);
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "", e);
//    }
    mManagerService = managerService;
    //mServiceRecordHandler = NULL;
    CHashMap::New((IMap**)&mLeScanClients);
    return NOERROR;
}

}
}
}

