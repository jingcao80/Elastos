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

#include "Elastos.Droid.Utility.h"
#include "elastos/droid/bluetooth/le/BluetoothLeAdvertiser.h"
#include "elastos/droid/bluetooth/le/BluetoothLeUtils.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/BluetoothUuid.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Bluetooth::BluetoothUuid;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::CBluetoothAdapter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Core::AutoLock;
using Elastos::Core::IArrayOf;
using Elastos::Utility::ISet;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//           BluetoothLeAdvertiser::AdvertiseCallbackWrapper
//=====================================================================
const Int32 BluetoothLeAdvertiser::AdvertiseCallbackWrapper::LE_CALLBACK_TIMEOUT_MILLIS;
Object BluetoothLeAdvertiser::AdvertiseCallbackWrapper::sLock;

BluetoothLeAdvertiser::AdvertiseCallbackWrapper::AdvertiseCallbackWrapper(
    /* [in] */ BluetoothLeAdvertiser* owner)
{
    mClientIf = 0;
    mIsAdvertising = FALSE;
    mOwner = owner;
}

BluetoothLeAdvertiser::AdvertiseCallbackWrapper::AdvertiseCallbackWrapper(
    /* [in] */ IAdvertiseCallback* advertiseCallback,
    /* [in] */ IAdvertiseData* advertiseData,
    /* [in] */ IAdvertiseData* scanResponse,
    /* [in] */ IAdvertiseSettings* settings,
    /* [in] */ IIBluetoothGatt* bluetoothGatt,
    /* [in] */ BluetoothLeAdvertiser* owner)
{
    mAdvertiseCallback = advertiseCallback;
    mAdvertisement = advertiseData;
    mScanResponse = scanResponse;
    mSettings = settings;
    mBluetoothGatt = bluetoothGatt;
    mClientIf = 0;
    mIsAdvertising = FALSE;
    mOwner = owner;
}

ECode BluetoothLeAdvertiser::AdvertiseCallbackWrapper::StartRegisteration()
{
    {
        AutoLock lock(sLock);
        if (mClientIf == -1) return NOERROR;

        //try {
        AutoPtr<IUUIDHelper> uuidHelper;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uuidHelper);
        AutoPtr<IUUID> uuid;
        uuidHelper->RandomUUID((IUUID**)&uuid);
        AutoPtr<IParcelUuid> parcelUuid;
        CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);
        mBluetoothGatt->RegisterClient(parcelUuid, this);
        Wait(LE_CALLBACK_TIMEOUT_MILLIS);
        //} catch (InterruptedException | RemoteException e) {
        //    Log.e(TAG, "Failed to start registeration", e);
        //}
        if (mClientIf > 0 && mIsAdvertising) {
            mOwner->mLeAdvertisers->Put(mAdvertiseCallback, TO_IINTERFACE(this));
        } else if (mClientIf <= 0) {
            // Post internal error if registration failed.
            mOwner->PostStartFailure(mAdvertiseCallback,
                    IAdvertiseCallback::ADVERTISE_FAILED_INTERNAL_ERROR);
        } else {
            // Unregister application if it's already registered but advertise failed.
            //try {
            mBluetoothGatt->UnregisterClient(mClientIf);
            mClientIf = -1;
            //} catch (RemoteException e) {
            //    Log.e(TAG, "remote exception when unregistering", e);
            //}
        }
    }
    return NOERROR;
}

ECode BluetoothLeAdvertiser::AdvertiseCallbackWrapper::StopAdvertising()
{
    {
        AutoLock lock(sLock);
        //try {
        mBluetoothGatt->StopMultiAdvertising(mClientIf);
        Wait(LE_CALLBACK_TIMEOUT_MILLIS);
        //} catch (InterruptedException | RemoteException e) {
        //    Log.e(TAG, "Failed to stop advertising", e);
        //}
        // Advertise callback should have been removed from LeAdvertisers when
        // onMultiAdvertiseCallback was called. In case onMultiAdvertiseCallback is never
        // invoked and wait timeout expires, remove callback here.
        Boolean bTemp = FALSE;
        if (mOwner->mLeAdvertisers->ContainsKey(TO_IINTERFACE(mAdvertiseCallback), &bTemp), bTemp) {
            mOwner->mLeAdvertisers->Remove(mAdvertiseCallback);
        }
    }
    return NOERROR;
}

ECode BluetoothLeAdvertiser::AdvertiseCallbackWrapper::OnClientRegistered(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf)
{
    Logger::D(TAG, "onClientRegistered() - status=%d clientIf=%d", status, clientIf);
    {
        AutoLock lock(sLock);
        if (status == IBluetoothGatt::GATT_SUCCESS) {
            mClientIf = clientIf;
            //try {
                mBluetoothGatt->StartMultiAdvertising(mClientIf, mAdvertisement,
                        mScanResponse, mSettings);
                return NOERROR;
            //} catch (RemoteException e) {
            //    Log.e(TAG, "failed to start advertising", e);
            //}
        }
        // Registration failed.
        mClientIf = -1;
        NotifyAll();
    }
    return NOERROR;
}

ECode BluetoothLeAdvertiser::AdvertiseCallbackWrapper::OnMultiAdvertiseCallback(
    /* [in] */ Int32 status,
    /* [in] */ Boolean isStart,
    /* [in] */ IAdvertiseSettings* settings)
{
    {
        AutoLock lock(sLock);
        if (isStart) {
            if (status == IAdvertiseCallback::ADVERTISE_SUCCESS) {
                // Start success
                mIsAdvertising = TRUE;
                mOwner->PostStartSuccess(mAdvertiseCallback, settings);
            } else {
                // Start failure.
                mOwner->PostStartFailure(mAdvertiseCallback, status);
            }
        } else {
            // unregister client for stop.
            //try {
                mBluetoothGatt->UnregisterClient(mClientIf);
                mClientIf = -1;
                mIsAdvertising = FALSE;
                mOwner->mLeAdvertisers->Remove(mAdvertiseCallback);
            //} catch (RemoteException e) {
            //    Log.e(TAG, "remote exception when unregistering", e);
            //}
        }
        NotifyAll();
    }
    return NOERROR;
}

//=====================================================================
//                BluetoothLeAdvertiser::InnerRunnable1
//=====================================================================
BluetoothLeAdvertiser::StartFailureRunnable::StartFailureRunnable(
    /* [in] */ IAdvertiseCallback* callback,
    /* [in] */ Int32 error,
    /* [in] */ BluetoothLeAdvertiser* owner)
    : mCallback(callback)
    , mError(error)
    , mOwner(owner)
{
}

ECode BluetoothLeAdvertiser::StartFailureRunnable::Run()
{
    mCallback->OnStartFailure(mError);
    return NOERROR;
}

//=====================================================================
//                BluetoothLeAdvertiser::InnerRunnable3
//=====================================================================
BluetoothLeAdvertiser::StartSuccessRunnable::StartSuccessRunnable(
    /* [in] */ IAdvertiseCallback* callback,
    /* [in] */ IAdvertiseSettings* settings,
    /* [in] */ BluetoothLeAdvertiser* owner)
    : mCallback(callback)
    , mSettings(settings)
    , mOwner(owner)
{
}

ECode BluetoothLeAdvertiser::StartSuccessRunnable::Run()
{
    mCallback->OnStartSuccess(mSettings);
    return NOERROR;
}

//=====================================================================
//                        BluetoothLeAdvertiser
//=====================================================================
const String BluetoothLeAdvertiser::TAG("BluetoothLeAdvertiser");
const Int32 BluetoothLeAdvertiser::MAX_ADVERTISING_DATA_BYTES;
const Int32 BluetoothLeAdvertiser::OVERHEAD_BYTES_PER_FIELD;
const Int32 BluetoothLeAdvertiser::FLAGS_FIELD_BYTES;
const Int32 BluetoothLeAdvertiser::MANUFACTURER_SPECIFIC_DATA_LENGTH;
const Int32 BluetoothLeAdvertiser::SERVICE_DATA_UUID_LENGTH;

CAR_INTERFACE_IMPL(BluetoothLeAdvertiser, Object, IBluetoothLeAdvertiser);

BluetoothLeAdvertiser::BluetoothLeAdvertiser()
{
}

ECode BluetoothLeAdvertiser::constructor(
    /* [in] */ IIBluetoothManager* bluetoothManager)
{
    mBluetoothManager = bluetoothManager;
    mBluetoothAdapter = CBluetoothAdapter::GetDefaultAdapter();
    AutoPtr<ILooper> mainLooper = Looper::GetMainLooper();
    CHandler::New(mainLooper, (IHandler**)&mHandler);
    return NOERROR;
}

ECode BluetoothLeAdvertiser::StartAdvertising(
    /* [in] */ IAdvertiseSettings* settings,
    /* [in] */ IAdvertiseData* advertiseData,
    /* [in] */ IAdvertiseCallback* callback)
{
    return StartAdvertising(settings, advertiseData, NULL, callback);
}

ECode BluetoothLeAdvertiser::StartAdvertising(
    /* [in] */ IAdvertiseSettings* settings,
    /* [in] */ IAdvertiseData* advertiseData,
    /* [in] */ IAdvertiseData* scanResponse,
    /* [in] */ IAdvertiseCallback* callback)
{
    {
        AutoLock lock(mLeAdvertisers);
        BluetoothLeUtils::CheckAdapterStateOn(mBluetoothAdapter);
        if (callback == NULL) {
            //throw new IllegalArgumentException("callback cannot be null");
            Logger::E("BluetoothLeAdvertiser::StartAdvertising", "callback cannot be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Boolean supported;
        if (!(mBluetoothAdapter->IsMultipleAdvertisementSupported(&supported), supported)) {
            PostStartFailure(callback,
                    IAdvertiseCallback::ADVERTISE_FAILED_FEATURE_UNSUPPORTED);
            return NOERROR;
        }
        if (TotalBytes(advertiseData) > MAX_ADVERTISING_DATA_BYTES ||
                TotalBytes(scanResponse) > MAX_ADVERTISING_DATA_BYTES) {
            PostStartFailure(callback, IAdvertiseCallback::ADVERTISE_FAILED_DATA_TOO_LARGE);
            return NOERROR;
        }
        Boolean bTemp;
        if (mLeAdvertisers->ContainsKey(TO_IINTERFACE(callback), &bTemp), bTemp) {
            PostStartFailure(callback, IAdvertiseCallback::ADVERTISE_FAILED_ALREADY_STARTED);
            return NOERROR;
        }

        AutoPtr<IIBluetoothGatt> gatt;
        //try {
        ECode ec = mBluetoothManager->GetBluetoothGatt((IIBluetoothGatt**)&gatt);
        //} catch (RemoteException e) {
        if (FAILED(ec)) {//need check, maybe only if E_REMOTE_EXCEPTION
            //Log.e(TAG, "Failed to get Bluetooth gatt - ", e);
            Logger::E(TAG, "Failed to get Bluetooth gatt - ");
            PostStartFailure(callback, IAdvertiseCallback::ADVERTISE_FAILED_INTERNAL_ERROR);
            return NOERROR;
        }
        //}
        AutoPtr<AdvertiseCallbackWrapper> wrapper = new AdvertiseCallbackWrapper(callback, advertiseData,
                scanResponse, settings, gatt, this);
        wrapper->StartRegisteration();
    }
    return NOERROR;
}

ECode BluetoothLeAdvertiser::StopAdvertising(
    /* [in] */ IAdvertiseCallback* callback)
{
    {
        AutoLock lock(mLeAdvertisers);
        BluetoothLeUtils::CheckAdapterStateOn(mBluetoothAdapter);
        if (callback == NULL) {
            //throw new IllegalArgumentException("callback cannot be null");
            Logger::E("BluetoothLeAdvertiser::StopAdvertising", "callback cannot be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IInterface> value;
        mLeAdvertisers->Get(TO_IINTERFACE(callback), (IInterface**)&value);
        AutoPtr<AdvertiseCallbackWrapper> wrapper = (AdvertiseCallbackWrapper*)(IIBluetoothGattCallback::Probe(value));
        if (wrapper == NULL) return NOERROR;
        wrapper->StopAdvertising();
    }
    return NOERROR;
}

ECode BluetoothLeAdvertiser::Cleanup()
{
    mLeAdvertisers->Clear();
    return NOERROR;
}

Int32 BluetoothLeAdvertiser::TotalBytes(
    /* [in] */ IAdvertiseData* data)
{
    if (data == NULL) return 0;
    Int32 size = FLAGS_FIELD_BYTES; // flags field is always set.
    AutoPtr<IList> uuids;
    data->GetServiceUuids((IList**)&uuids);
    if (uuids != NULL) {
        Int32 num16BitUuids = 0;
        Int32 num32BitUuids = 0;
        Int32 num128BitUuids = 0;
        Int32 listSize;
        uuids->GetSize(&listSize);
        for (Int32 i = 0; i < listSize; ++i) {
            AutoPtr<IInterface> obj;
            uuids->Get(i, (IInterface**)&obj);
            IParcelUuid* uuid = IParcelUuid::Probe(obj);

            if (BluetoothUuid::Is16BitUuid(uuid)) {
                ++num16BitUuids;
            } else if (BluetoothUuid::Is32BitUuid(uuid)) {
                ++num32BitUuids;
            } else {
                ++num128BitUuids;
            }
        }
        // 16 bit service uuids are grouped into one field when doing advertising.
        if (num16BitUuids != 0) {
            size += OVERHEAD_BYTES_PER_FIELD +
                    num16BitUuids * BluetoothUuid::UUID_BYTES_16_BIT;
        }
        // 32 bit service uuids are grouped into one field when doing advertising.
        if (num32BitUuids != 0) {
            size += OVERHEAD_BYTES_PER_FIELD +
                    num32BitUuids * BluetoothUuid::UUID_BYTES_32_BIT;
        }
        // 128 bit service uuids are grouped into one field when doing advertising.
        if (num128BitUuids != 0) {
            size += OVERHEAD_BYTES_PER_FIELD +
                    num128BitUuids * BluetoothUuid::UUID_BYTES_128_BIT;
        }
    }
    AutoPtr<IMap> serviceData;
    data->GetServiceData((IMap**)&serviceData);
    AutoPtr<ISet> keySet;
    serviceData->GetKeySet((ISet**)&keySet);
    AutoPtr<ArrayOf<IInterface*> > keyArray;
    keySet->ToArray((ArrayOf<IInterface*>**)&keyArray);
    for (Int32 i = 0; i < keyArray->GetLength(); ++i) {
        AutoPtr<IInterface> key = (*keyArray)[i];
        AutoPtr<IInterface> value;
        serviceData->Get(key, (IInterface**)&value);
        IArrayOf* vArrayOf = IArrayOf::Probe(value);
        Int32 length;
        vArrayOf->GetLength(&length);
        //for the fucntion *ByteLength* only to get length of the array
        //AutoPtr<ArrayOf<Byte> > arrayByte = ArrayOf<Byte>::Alloc(length);
        //for (Int32 j = 0; j < length; ++j) {
        //    AutoPtr<IInterface> obj;
        //    vArrayOf->Get(j, (IInterface**)&obj);
        //    IByte* iByte = IByte::Probe(obj);
        //    Byte bValue;
        //    iByte->GetValue(&bValue);
        //    arrayByte->Set(j, bValue);
        //}

        size += OVERHEAD_BYTES_PER_FIELD +
            SERVICE_DATA_UUID_LENGTH +
            length;
        //        ByteLength(arrayByte);
    }
    AutoPtr<ISparseArray> mSpecData;
    data->GetManufacturerSpecificData((ISparseArray**)&mSpecData);
    Int32 msdSize;
    mSpecData->GetSize(&msdSize);
    for (Int32 i = 0; i < msdSize; ++i) {
        AutoPtr<IInterface> value;
        mSpecData->ValueAt(i, (IInterface**)&value);
        AutoPtr<IArrayOf> vArrayOf = IArrayOf::Probe(value);
        Int32 length;
        vArrayOf->GetLength(&length);
        //for the fucntion *ByteLength* only to get length of the array
        //AutoPtr<ArrayOf<Byte> > arrayByte = ArrayOf<Byte>::Alloc(length);
        //for (Int32 j = 0; j < length; ++j) {
        //    AutoPtr<IInterface> obj;
        //    vArrayOf->Get(j, (IInterface**)&obj);
        //    IByte* iByte = IByte::Probe(obj);
        //    Byte bValue;
        //    iByte->GetValue(&bValue);
        //    arrayByte->Set(j, bValue);
        //}

        size += OVERHEAD_BYTES_PER_FIELD +
            MANUFACTURER_SPECIFIC_DATA_LENGTH +
            length;
        //        ByteLength(arrayByte); this function to get the length of the array!
    }
    Boolean bTemp = FALSE;
    if (data->GetIncludeTxPowerLevel(&bTemp), bTemp) {
        size += OVERHEAD_BYTES_PER_FIELD + 1; // tx power level value is one byte.
    }
    bTemp = FALSE;
    String name;
    if ((data->GetIncludeDeviceName(&bTemp), bTemp)
            && (mBluetoothAdapter->GetName(&name), !name.IsNullOrEmpty())) {
        size += OVERHEAD_BYTES_PER_FIELD + name.GetLength();
    }
    return size;
}

Int32 BluetoothLeAdvertiser::ByteLength(
    /* [in] */ ArrayOf<Byte>* array)
{
    return array == NULL? 0 : array->GetLength();
}

void BluetoothLeAdvertiser::PostStartFailure(
    /* [in] */ IAdvertiseCallback* callback,
    /* [in] */ Int32 error)
{
    AutoPtr<StartFailureRunnable> run = new StartFailureRunnable(callback, error, this);
    Boolean result;
    mHandler->Post(run, &result);
}

void BluetoothLeAdvertiser::PostStartSuccess(
    /* [in] */ IAdvertiseCallback* callback,
    /* [in] */ IAdvertiseSettings* settings)
{
    AutoPtr<StartSuccessRunnable> run = new StartSuccessRunnable(callback, settings, this);
    Boolean result;
    mHandler->Post(run, &result);
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
