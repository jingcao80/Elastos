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

#include "elastos/droid/bluetooth/le/BluetoothLeScanner.h"
#include "elastos/droid/bluetooth/le/ScanSettings.h"
#include "elastos/droid/bluetooth/le/BluetoothLeUtils.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Bluetooth::CBluetoothAdapter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::CParcelUuid;
using Elastos::Core::AutoLock;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//      BluetoothLeScanner::BleScanCallbackWrapper::ScanResultRunnable
//=====================================================================
BluetoothLeScanner::BleScanCallbackWrapper::ScanResultRunnable::ScanResultRunnable(
    /* [in] */ IScanResult* scanResult,
    /* [in] */ BleScanCallbackWrapper* owner)
    : mScanResult(scanResult)
    , mOwner(owner)
{
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::ScanResultRunnable::Run()
{
    mOwner->mScanCallback->OnScanResult(IScanSettings::CALLBACK_TYPE_ALL_MATCHES, mScanResult);
    return NOERROR;
}

//=====================================================================
//      BluetoothLeScanner::BleScanCallbackWrapper::BatchScanResultsRunnable
//=====================================================================
BluetoothLeScanner::BleScanCallbackWrapper::BatchScanResultsRunnable::BatchScanResultsRunnable(
    /* [in] */ IList* results,
    /* [in] */ BleScanCallbackWrapper* owner)
    : mResults(results)
    , mOwner(owner)
{
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::BatchScanResultsRunnable::Run()
{
    mOwner->mScanCallback->OnBatchScanResults(mResults);
    return NOERROR;
}

//=====================================================================
//      BluetoothLeScanner::BleScanCallbackWrapper::FoundOrLostRunnable
//=====================================================================
BluetoothLeScanner::BleScanCallbackWrapper::FoundOrLostRunnable::FoundOrLostRunnable(
    /* [in] */ IScanResult* scanResult,
    /* [in] */ Boolean onFound,
    /* [in] */ BleScanCallbackWrapper* owner)
    : mScanResult(scanResult)
    , mOnFound(onFound)
    , mOwner(owner)
{
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::FoundOrLostRunnable::Run()
{
    if (mOnFound) {
        mOwner->mScanCallback->OnScanResult(IScanSettings::CALLBACK_TYPE_FIRST_MATCH,
                mScanResult);
    } else {
        mOwner->mScanCallback->OnScanResult(IScanSettings::CALLBACK_TYPE_MATCH_LOST,
                mScanResult);
    }
    return NOERROR;
}

//=====================================================================
//              BluetoothLeScanner::BleScanCallbackWrapper
//=====================================================================
const Int32 BluetoothLeScanner::BleScanCallbackWrapper::REGISTRATION_CALLBACK_TIMEOUT_MILLIS;
Object BluetoothLeScanner::BleScanCallbackWrapper::sLock;

BluetoothLeScanner::BleScanCallbackWrapper::BleScanCallbackWrapper(
    /* [in] */ IIBluetoothGatt* bluetoothGatt,
    /* [in] */ IList* filters,
    /* [in] */ IScanSettings* settings,
    /* [in] */ IScanCallback* scanCallback,
    /* [in] */ IList* resultStorages,
    /* [in] */ BluetoothLeScanner* owner)
{
    mBluetoothGatt = bluetoothGatt;
    mFilters = filters;
    mSettings = settings;
    mScanCallback = scanCallback;
    mClientIf = 0;
    mResultStorages = resultStorages;
    mOwner = owner;
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::StartRegisteration()
{
    {
        AutoLock lock(sLock);
        // Scan stopped.
        if (mClientIf == -1) return NOERROR;
        //try {
            AutoPtr<IUUIDHelper> uuidHelper;
            CUUIDHelper::AcquireSingleton((IUUIDHelper**)&uuidHelper);
            AutoPtr<IUUID> uuid;
            uuidHelper->RandomUUID((IUUID**)&uuid);
            AutoPtr<IParcelUuid> parcelUuid;
            CParcelUuid::New(uuid, (IParcelUuid**)&parcelUuid);

            ECode ec1 = mBluetoothGatt->RegisterClient(parcelUuid, this);
            ECode ec2 = Wait(REGISTRATION_CALLBACK_TIMEOUT_MILLIS);
        //} catch (InterruptedException | RemoteException e) {
        if (FAILED(ec1) || FAILED(ec2)) {//need check, maybe only the specific exception
            //Log.e(TAG, "application registeration exception", e);
            Logger::E(TAG, "application registeration exception");
            mOwner->PostCallbackError(mScanCallback, IScanCallback::SCAN_FAILED_INTERNAL_ERROR);
        }
        //}
        if (mClientIf > 0) {
            mOwner->mLeScanClients->Put(TO_IINTERFACE(mScanCallback), TO_IINTERFACE(this));
        } else {
            mOwner->PostCallbackError(mScanCallback,
                    IScanCallback::SCAN_FAILED_APPLICATION_REGISTRATION_FAILED);
        }
    }
    return NOERROR;
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::StopLeScan()
{
    {
        AutoLock lock(sLock);
        if (mClientIf <= 0) {
            //Log.e(TAG, "Error state, mLeHandle: " + mClientIf);
            Logger::E(TAG, "Error state, mLeHandle: ");
            return NOERROR;
        }
        //try {
            mBluetoothGatt->StopScan(mClientIf, FALSE);
            mBluetoothGatt->UnregisterClient(mClientIf);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Failed to stop scan and unregister", e);
        //}
        mClientIf = -1;
    }
    return NOERROR;
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::FlushPendingBatchResults()
{
    {
        AutoLock lock(sLock);
        if (mClientIf <= 0) {
            //Log.e(TAG, "Error state, mLeHandle: " + mClientIf);
            Logger::E(TAG, "Error state, mLeHandle: %d", mClientIf);
            return NOERROR;
        }
        //try {
            mBluetoothGatt->FlushPendingBatchResults(mClientIf, FALSE);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Failed to get pending scan results", e);
        //}
    }
    return NOERROR;
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::OnClientRegistered(
    /* [in] */ Int32 status,
    /* [in] */ Int32 clientIf)
{
    Logger::D(TAG, "onClientRegistered() - status=%d clientIf=%d", status, clientIf);
    {
        AutoLock lock(sLock);
        if (mClientIf == -1) {
            if (DBG)
                Logger::D(TAG, "onClientRegistered LE scan canceled");
        }

        if (status == IBluetoothGatt::GATT_SUCCESS) {
            mClientIf = clientIf;
            //try {
            ECode ec = mBluetoothGatt->StartScan(mClientIf, FALSE, mSettings, mFilters,
                        mResultStorages);
            //} catch (RemoteException e) {
            if (FAILED(ec)) {//need check, maybe only if E_REMOTE_EXCEPTION
                Logger::E(TAG, "fail to start le scan: ");
                mClientIf = -1;
            }
            //}
        } else {
            // registration failed
            mClientIf = -1;
        }
        NotifyAll();
    }
    return NOERROR;
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::OnScanResult(
    /* [in] */ IScanResult* scanResult)
{
    if (DBG) Logger::D(TAG, "onScanResult() - ");// + scanResult.toString());

    // Check null in case the scan has been stopped
    {
        AutoLock lock(sLock);
        if (mClientIf <= 0) return NOERROR;
    }
    AutoPtr<ILooper> mainLooper = Looper::GetMainLooper();
    AutoPtr<IHandler> handler;
    CHandler::New(mainLooper, (IHandler**)&handler);
    AutoPtr<ScanResultRunnable> run = new ScanResultRunnable(scanResult, this);
    Boolean result;
    handler->Post(run, &result);
    return NOERROR;
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::OnBatchScanResults(
    /* [in] */ IList* results)
{
    AutoPtr<ILooper> mainLooper = Looper::GetMainLooper();
    AutoPtr<IHandler> handler;
    CHandler::New(mainLooper, (IHandler**)&handler);
    AutoPtr<BatchScanResultsRunnable> run = new BatchScanResultsRunnable(results, this);
    Boolean result;
    handler->Post(run, &result);
    return NOERROR;
}

ECode BluetoothLeScanner::BleScanCallbackWrapper::OnFoundOrLost(
    /* [in] */ Boolean onFound,
    /* [in] */ IScanResult* scanResult)
{
    if (DBG) {
        Logger::D(TAG, "onFoundOrLost() - onFound = %d", onFound);//, scanResult.toString());
    }

    // Check null in case the scan has been stopped
    {
        AutoLock lock(sLock);
        if (mClientIf <= 0)
            return NOERROR;
    }
    AutoPtr<ILooper> mainLooper = Looper::GetMainLooper();
    AutoPtr<IHandler> handler;
    CHandler::New(mainLooper, (IHandler**)&handler);
    AutoPtr<FoundOrLostRunnable> run = new FoundOrLostRunnable(scanResult, onFound, this);
    Boolean result;
    handler->Post(run, &result);
    return NOERROR;
}

//=====================================================================
//                  BluetoothLeScanner::CallbackErrorRunnable
//=====================================================================
BluetoothLeScanner::CallbackErrorRunnable::CallbackErrorRunnable(
    /* [in] */ IScanCallback* callback,
    /* [in] */ Int32 errorCode,
    /* [in] */ BluetoothLeScanner* owner)
    : mCallback(callback)
    , mErrorCode(errorCode)
    , mOwner(owner)
{
}

ECode BluetoothLeScanner::CallbackErrorRunnable::Run()
{
    mCallback->OnScanFailed(mErrorCode);
    return NOERROR;
}

//=====================================================================
//                          BluetoothLeScanner
//=====================================================================
const String BluetoothLeScanner::TAG("BluetoothLeScanner");
const Boolean BluetoothLeScanner::DBG = TRUE;

CAR_INTERFACE_IMPL(BluetoothLeScanner, Object, IBluetoothLeScanner);

BluetoothLeScanner::BluetoothLeScanner()
{
}

ECode BluetoothLeScanner::constructor(
    /* [in] */ IIBluetoothManager* bluetoothManager)
{
    mBluetoothManager = bluetoothManager;
    mBluetoothAdapter = CBluetoothAdapter::GetDefaultAdapter();
    AutoPtr<ILooper> mainLooper = Looper::GetMainLooper();
    CHandler::New(mainLooper, (IHandler**)&mHandler);
    //mLeScanClients = new HashMap<ScanCallback, BleScanCallbackWrapper>();
    CHashMap::New((IMap**)&mLeScanClients);
    return NOERROR;
}

ECode BluetoothLeScanner::StartScan(
    /* [in] */ IScanCallback* callback)
{
    if (callback == NULL) {
        //throw new IllegalArgumentException("callback is null");
        Logger::E("BluetoothLeScanner::StartScan", "callback is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IScanSettings> scanSettings;
    AutoPtr<ScanSettings::Builder> builder = new ScanSettings::Builder();
    builder->Build((IScanSettings**)&scanSettings);

    StartScan(NULL, scanSettings, callback);
    return NOERROR;
}

ECode BluetoothLeScanner::StartScan(
    /* [in] */ IList* filters,
    /* [in] */ IScanSettings* settings,
    /* [in] */ IScanCallback* callback)
{
    StartScan(filters, settings, callback, NULL);
    return NOERROR;
}

ECode BluetoothLeScanner::StopScan(
    /* [in] */ IScanCallback* callback)
{
    BluetoothLeUtils::CheckAdapterStateOn(mBluetoothAdapter);
    {
        AutoLock lock(mLeScanClients);
        AutoPtr<IInterface> value;
        mLeScanClients->Remove(TO_IINTERFACE(callback), (IInterface**)&value);
        AutoPtr<BleScanCallbackWrapper> wrapper = (BleScanCallbackWrapper*)(IIBluetoothGattCallback::Probe(value));
        if (wrapper == NULL) {
            if (DBG) Logger::D(TAG, "could not find callback wrapper");
            return NOERROR;
        }
        wrapper->StopLeScan();
    }
    return NOERROR;
}

ECode BluetoothLeScanner::FlushPendingScanResults(
    /* [in] */ IScanCallback* callback)
{
    BluetoothLeUtils::CheckAdapterStateOn(mBluetoothAdapter);
    if (callback == NULL) {
        //throw new IllegalArgumentException("callback cannot be null!");
        Logger::E("BluetoothLeScanner::FlushPendingScanResults", "callback cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {
        AutoLock lock(mLeScanClients);
        AutoPtr<IInterface> value;
        mLeScanClients->Get(TO_IINTERFACE(callback), (IInterface**)&value);
        AutoPtr<BleScanCallbackWrapper> wrapper = (BleScanCallbackWrapper*)(IIBluetoothGattCallback::Probe(value));
        if (wrapper == NULL) {
            return NOERROR;
        }
        wrapper->FlushPendingBatchResults();
    }
    return NOERROR;
}

ECode BluetoothLeScanner::StartTruncatedScan(
    /* [in] */ IList* truncatedFilters,
    /* [in] */ IScanSettings* settings,
    /* [in] */ IScanCallback* callback)
{
    Int32 filterSize;
    truncatedFilters->GetSize(&filterSize);
    //List<ScanFilter> scanFilters = new ArrayList<ScanFilter>(filterSize);
    AutoPtr<IList> scanFilters;
    CArrayList::New((IList**)&scanFilters);
    //List<List<ResultStorageDescriptor>> scanStorages =
    //        new ArrayList<List<ResultStorageDescriptor>>(filterSize);
    AutoPtr<IList> scanStorages;
    CArrayList::New((IList**)&scanStorages);
    //for (TruncatedFilter filter : truncatedFilters) {
    Int32 size;
    truncatedFilters->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        truncatedFilters->Get(i, (IInterface**)&obj);
        ITruncatedFilter* filter = ITruncatedFilter::Probe(obj);
        AutoPtr<IScanFilter> scanFilter;
        filter->GetFilter((IScanFilter**)&scanFilter);
        scanFilters->Add(TO_IINTERFACE(scanFilters));
        AutoPtr<IList> list;
        filter->GetStorageDescriptors((IList**)&list);
        scanStorages->Add(TO_IINTERFACE(list));
    }
    StartScan(scanFilters, settings, callback, scanStorages);
    return NOERROR;
}

ECode BluetoothLeScanner::Cleanup()
{
    mLeScanClients->Clear();
    return NOERROR;
}

ECode BluetoothLeScanner::StartScan(
    /* [in] */ IList* filters,
    /* [in] */ IScanSettings* settings,
    /* [in] */ IScanCallback* callback,
    /* [in] */ IList* resultStorages)
{
    BluetoothLeUtils::CheckAdapterStateOn(mBluetoothAdapter);
    if (settings == NULL|| callback == NULL) {
        //throw new IllegalArgumentException("settings or callback is null");
        Logger::E("BluetoothLeScanner::StartScan", "settings or callback is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    {
        AutoLock lock(mLeScanClients);
        Boolean containsKey;
        if (mLeScanClients->ContainsKey(TO_IINTERFACE(callback), &containsKey), containsKey) {
            PostCallbackError(callback, IScanCallback::SCAN_FAILED_ALREADY_STARTED);
            return NOERROR;
        }
        AutoPtr<IIBluetoothGatt> gatt;
        //try {
            mBluetoothManager->GetBluetoothGatt((IIBluetoothGatt**)&gatt);
        //} catch (RemoteException e) {
        //    gatt = NULL;
        //}
        if (gatt == NULL) {
            PostCallbackError(callback, IScanCallback::SCAN_FAILED_INTERNAL_ERROR);
            return NOERROR;
        }
        if (!IsSettingsConfigAllowedForScan(settings)) {
            PostCallbackError(callback,
                    IScanCallback::SCAN_FAILED_FEATURE_UNSUPPORTED);
            return NOERROR;
        }
        AutoPtr<BleScanCallbackWrapper> wrapper = new BleScanCallbackWrapper(gatt, filters,
                settings, callback, resultStorages, this);
        wrapper->StartRegisteration();
    }
    return NOERROR;
}

void BluetoothLeScanner::PostCallbackError(
    /* [in] */ IScanCallback* callback,
    /* [in] */ Int32 errorCode)
{
    AutoPtr<CallbackErrorRunnable> run = new CallbackErrorRunnable(callback, errorCode, this);
    Boolean result;
    mHandler->Post(run, &result);
}

Boolean BluetoothLeScanner::IsSettingsConfigAllowedForScan(
    /* [in] */ IScanSettings* settings)
{
    Boolean supported = FALSE;
    if (mBluetoothAdapter->IsOffloadedFilteringSupported(&supported), supported) {
        return TRUE;
    }
    Int32 callbackType;
    settings->GetCallbackType(&callbackType);
    // Only support regular scan if no offloaded filter support.
    Int64 delayMillis = 0;
    if (callbackType == IScanSettings::CALLBACK_TYPE_ALL_MATCHES
            && (settings->GetReportDelayMillis(&delayMillis), delayMillis) == 0) {
        return TRUE;
    }
    return FALSE;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
