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

#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/wifi/CWifiManager.h"
// #include "elastos/droid/wifi/CWifiConfiguration.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/CWorkSource.h"
#include "elastos/droid/os/CHandlerThread.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::AutoLock;
using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Wifi {

//==============================================================
// CWifiManager::WifiLock
//==============================================================

CAR_INTERFACE_IMPL(CWifiManager::WifiLock, Object, IWifiManagerWifiLock)

CWifiManager::WifiLock::WifiLock(
    /* [in] */ Int32 lockType,
    /* [in] */ const String& tag,
    /* [in] */ CWifiManager* owner)
    : mTag(tag)
    , mRefCount(0)
    , mLockType(lockType)
    , mRefCounted(TRUE)
    , mHeld(FALSE)
    , mOwner(owner)
{
    CBinder::New((IBinder**)&mBinder);
}

CWifiManager::WifiLock::~WifiLock()
{
    AutoLock lock(mBinderLock);
    if (mHeld) {
        // try {
        Boolean result = FALSE;
        mOwner->mService->ReleaseWifiLock(mBinder, &result);
        {
            AutoLock lock(mOwner);
            mOwner->mActiveLockCount--;
        }
        // } catch (RemoteException ignore) {
        // }
    }
}

ECode CWifiManager::WifiLock::AcquireLock()
{
    AutoLock lock(mBinderLock);
    if (mRefCounted ? (++mRefCount == 1) : (!mHeld)) {
        // try {
        Boolean result = FALSE;
        mOwner->mService->AcquireWifiLock(mBinder, mLockType, mTag, mWorkSource, &result);
        {
            AutoLock lock(mOwner);

            if (mOwner->mActiveLockCount >= MAX_ACTIVE_LOCKS) {
                mOwner->mService->ReleaseWifiLock(mBinder, &result);
                // throw new UnsupportedOperationException(
                //             "Exceeded maximum number of wifi locks");
                return E_UNSUPPORTED_OPERATION_EXCEPTION;
            }
            mOwner->mActiveLockCount++;
        }
        // } catch (RemoteException ignore) {
        // }
        mHeld = TRUE;
    }
    return NOERROR;
}

ECode CWifiManager::WifiLock::ReleaseLock()
{
    AutoLock lock(mBinderLock);
    if (mRefCounted ? (--mRefCount == 0) : (mHeld)) {
        // try {
        Boolean result = FALSE;
        mOwner->mService->ReleaseWifiLock(mBinder, &result);
        {
            AutoLock lock(mOwner);
            mOwner->mActiveLockCount--;
        }
        // } catch (RemoteException ignore) {
        // }
        mHeld = FALSE;
    }
    if (mRefCount < 0) {
        // throw new RuntimeException("WifiLock under-locked " + mTag);
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CWifiManager::WifiLock::SetReferenceCounted(
    /* [in] */ Boolean refCounted)
{
    mRefCounted = refCounted;
    return NOERROR;
}

ECode CWifiManager::WifiLock::IsHeld(
    /* [out] */ Boolean* held)
{
    VALIDATE_NOT_NULL(held);
    AutoLock lock(mBinderLock);
    *held = mHeld;
    return NOERROR;
}

ECode CWifiManager::WifiLock::SetWorkSource(
    /* [in] */ IWorkSource* ws)
{
    AutoLock lock(mBinderLock);

    Int32 size = 0;
    if (ws != NULL && (ws->GetSize(&size), size == 0)) {
        ws = NULL;
    }
    Boolean changed = TRUE;
    if (ws == NULL) {
        mWorkSource = NULL;
    }
    else if (mWorkSource == NULL) {
        ws->ClearNames();
        if (mWorkSource == NULL) {
            changed = mWorkSource != NULL;
            CWorkSource::New(ws, (IWorkSource**)&mWorkSource);
        }
        else {
            mWorkSource->Diff(ws, &changed);
            if (changed) {
                mWorkSource->Set(ws);
            }
        }
    }
    if (changed && mHeld) {
        // try {
        mOwner->mService->UpdateWifiLockWorkSource(mBinder, mWorkSource);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}


//==============================================================
// CWifiManager::MulticastLock
//==============================================================

CAR_INTERFACE_IMPL(CWifiManager::MulticastLock, Object, IWifiManagerMulticastLock)

CWifiManager::MulticastLock::MulticastLock(
    /* [in] */ const String& tag,
    /* [in] */ CWifiManager* owner)
    : mTag(tag)
    , mRefCount(0)
    , mRefCounted(TRUE)
    , mHeld(FALSE)
    , mOwner(owner)
{
    CBinder::New((IBinder**)&mBinder);
}

CWifiManager::MulticastLock::~MulticastLock()
{
    SetReferenceCounted(FALSE);
    ReleaseLock();
}

ECode CWifiManager::MulticastLock::AcquireLock()
{
    {
        AutoLock lock(mBinderLock);

        if (mRefCounted ? (++mRefCount == 1) : (!mHeld)) {
            // try {
            mOwner->mService->AcquireMulticastLock(mBinder, mTag);
            {
                AutoLock lock(mOwner);

                if (mOwner->mActiveLockCount >= MAX_ACTIVE_LOCKS) {
                    mOwner->mService->ReleaseMulticastLock();
                    // throw new UnsupportedOperationException(
                    //         "Exceeded maximum number of wifi locks");
                    return E_UNSUPPORTED_OPERATION_EXCEPTION;
                }
                mOwner->mActiveLockCount++;
            }
            // } catch (RemoteException ignore) {
            // }
            mHeld = TRUE;
        }
    }
    return NOERROR;
}

ECode CWifiManager::MulticastLock::ReleaseLock()
{
    AutoLock lock(mBinderLock);

    if (mRefCounted ? (--mRefCount == 0) : (mHeld)) {
        // try {
        mOwner->mService->ReleaseMulticastLock();
        {
            AutoLock lock(mOwner);
            mOwner->mActiveLockCount--;
        }
        // } catch (RemoteException ignore) {
        // }
        mHeld = FALSE;
    }
    if (mRefCount < 0) {
        // throw new RuntimeException("MulticastLock under-locked "
        //         + mTag);
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CWifiManager::MulticastLock::SetReferenceCounted(
    /* [in] */ Boolean refCounted)
{
    mRefCounted = refCounted;
    return NOERROR;
}

ECode CWifiManager::MulticastLock::IsHeld(
    /* [in] */ Boolean* held)
{
    VALIDATE_NOT_NULL(held);
    AutoLock lock(mBinderLock);
    *held = mHeld;
    return NOERROR;
}


//==============================================================
// CWifiManager::ServiceHandler
//==============================================================

ECode CWifiManager::ServiceHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 arg2;
    message->GetArg2(&arg2);
    AutoPtr<IInterface> listener = RemoveListener(arg2);
    Int32 what;
    message->GetWhat(&what);
    switch (what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
        {
            // Logger::D(TAG, "ServiceHandler HandleMessage CMD_CHANNEL_HALF_CONNECTED");
            Int32 arg1;
            message->GetArg1(&arg1);
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                sAsyncChannel->SendMessage(AsyncChannel::CMD_CHANNEL_FULL_CONNECTION);
            } else {
                Logger::E(TAG, "Failed to set up channel connection");
                // This will cause all further async API calls on the WifiManager
                // to fail and throw an exception
                if (sAsyncChannel) {
                    //sAsyncChannel->Disconnected();
                    sAsyncChannel = NULL;
                }
            }
            sConnected->CountDown();
            break;
        }
        case AsyncChannel::CMD_CHANNEL_FULLY_CONNECTED:
            // Ignore
            break;
        case AsyncChannel::CMD_CHANNEL_DISCONNECTED:
            //Logger::E(TAG, "ServiceHandler HandleMessage CMD_CHANNEL_DISCONNECTED Channel connection lost");
            // This will cause all further async API calls on the WifiManager
            // to fail and throw an exception
            if (sAsyncChannel) {
                //sAsyncChannel->Disconnected();
                sAsyncChannel = NULL;
            }
            mLooper->Quit();
            break;
            /* ActionListeners grouped together */
        case IWifiManager::CONNECT_NETWORK_FAILED:
        case IWifiManager::FORGET_NETWORK_FAILED:
        case IWifiManager::SAVE_NETWORK_FAILED:
        case IWifiManager::DISABLE_NETWORK_FAILED:
        {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (listener != NULL) {
                IWifiManagerActionListener::Probe(listener)->OnFailure(arg1);
            }
            break;
        }
            /* ActionListeners grouped together */
        case IWifiManager::CONNECT_NETWORK_SUCCEEDED:
        case IWifiManager::FORGET_NETWORK_SUCCEEDED:
        case IWifiManager::SAVE_NETWORK_SUCCEEDED:
        case IWifiManager::DISABLE_NETWORK_SUCCEEDED:
            if (listener != NULL) {
                IWifiManagerActionListener::Probe(listener)->OnSuccess();
            }
            break;
        case IWifiManager::START_WPS_SUCCEEDED:
        {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            if (listener != NULL) {
                AutoPtr<IWpsResult> result = IWpsResult::Probe(obj);
                String pin;
                result->GetPin(&pin);
                IWifiManagerWpsCallback::Probe(listener)->OnStarted(pin);

                //Listener needs to stay until completion or failure
                AutoLock lock(sListenerMapLock);
                sListenerMap->Put(arg2, listener);
            }
            break;
        }
        case IWifiManager::WPS_COMPLETED:
            if (listener != NULL) {
                IWifiManagerWpsCallback::Probe(listener)->OnSucceeded();
            }
            break;
        case IWifiManager::WPS_FAILED:
        {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (listener != NULL) {
                IWifiManagerWpsCallback::Probe(listener)->OnFailed(arg1);
            }
            break;
        }
        case IWifiManager::CANCEL_WPS_SUCCEDED:
            if (listener != NULL) {
                IWifiManagerWpsCallback::Probe(listener)->OnSucceeded();
            }
            break;
        case IWifiManager::CANCEL_WPS_FAILED:
            if (listener != NULL) {
                Int32 arg1;
                message->GetArg1(&arg1);
                IWifiManagerWpsCallback::Probe(listener)->OnFailed(arg1);
            }
            break;
        case IWifiManager::RSSI_PKTCNT_FETCH_SUCCEEDED:
        {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            if (listener != NULL) {
                AutoPtr<IRssiPacketCountInfo> info = IRssiPacketCountInfo::Probe(obj);
                if (info != NULL) {
                    Int32 txgood, txbad;
                    info->GetTxgood(&txgood);
                    info->GetTxbad(&txbad);
                    IWifiManagerTxPacketCountListener::Probe(listener)->OnSuccess(txgood + txbad);
                }
                else {
                    IWifiManagerTxPacketCountListener::Probe(listener)->OnFailure(IWifiManager::ERROR);
                }
            }
            break;
        }
        case IWifiManager::RSSI_PKTCNT_FETCH_FAILED:
        {
            Int32 arg1;
            message->GetArg1(&arg1);
            if (listener != NULL) {
                IWifiManagerTxPacketCountListener::Probe(listener)->OnFailure(arg1);
            }
            break;
        }
        default:
            //ignore
            break;
    }
    return NOERROR;
}

//==============================================================
// CWifiManager
//==============================================================

static AutoPtr<ISparseArray> sListenerMap_Init()
{
    AutoPtr<ISparseArray> array;
    CSparseArray::New((ISparseArray**)&array);
    return array;
}

const String CWifiManager::TAG("WifiManager");
const Int32 CWifiManager::MIN_RSSI = -100;
const Int32 CWifiManager::MAX_RSSI = -55;
const Int32 CWifiManager::MAX_ACTIVE_LOCKS = 50;
const Int32 CWifiManager::INVALID_KEY = 0;
const Int32 CWifiManager::BASE = IProtocol::BASE_WIFI_MANAGER;

Int32 CWifiManager::sListenerKey = 1;
AutoPtr<ISparseArray> CWifiManager::sListenerMap = sListenerMap_Init();
Object CWifiManager::sListenerMapLock;

AutoPtr<IAsyncChannel> CWifiManager::sAsyncChannel;
AutoPtr<ICountDownLatch> CWifiManager::sConnected;

Object CWifiManager::sThreadRefLock;
Int32 CWifiManager::sThreadRefCount = 0;
AutoPtr<IHandlerThread> CWifiManager::sHandlerThread;

CAR_INTERFACE_IMPL(CWifiManager, Object, IWifiManager)

CAR_OBJECT_IMPL(CWifiManager)

CWifiManager::CWifiManager()
    : mActiveLockCount(0)
{
    CCountDownLatch::New(1, (ICountDownLatch**)&sConnected);
}

CWifiManager::~CWifiManager()
{
    //try {
    {
        AutoLock lock(sThreadRefLock);
        if (--sThreadRefCount == 0 && sAsyncChannel != NULL) {
            sAsyncChannel->Disconnect();
        }
    }
    //} finally {
    //super.finalize();
    //}
}

ECode CWifiManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIWifiManager* service)
{
    mContext = context;
    mService = service;
    Init();
    AutoPtr<IInterface> p;
    context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&p);
    mAppOps = IAppOpsManager::Probe(p);
    return NOERROR;
}

ECode CWifiManager::GetConfiguredNetworks(
    /* [out] */ IList** networks)
{
    VALIDATE_NOT_NULL(networks);
    // try {
    return mService->GetConfiguredNetworks(networks);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiManager::AddNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    if (config == NULL) {
        *id = -1;
        return NOERROR;
    }
    config->SetNetworkId(-1);
    *id = AddOrUpdateNetwork(config);
    return NOERROR;
}

ECode CWifiManager::UpdateNetwork(
    /* [in] */ IWifiConfiguration* config,
    /* [out] */ Int32* networkId)
{
    VALIDATE_NOT_NULL(networkId);
    Int32 netId;
    if (config == NULL || (config->GetNetworkId(&netId), netId < 0)) {
        *networkId = -1;
        return NOERROR;
    }
    *networkId = AddOrUpdateNetwork(config);
    return NOERROR;
}

Int32 CWifiManager::AddOrUpdateNetwork(
    /* [in] */ IWifiConfiguration* config)
{
    // try {
    Int32 id = 0;
    ECode ec = mService->AddOrUpdateNetwork(config, &id);
    if (FAILED(ec)) {
        return -1;
    }
    return id;
    // } catch (RemoteException e) {
    //     return -1;
    // }
}

ECode CWifiManager::RemoveNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    // try {
    return mService->RemoveNetwork(netId, succeeded);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::EnableNetwork(
    /* [in] */ Int32 netId,
    /* [in] */ Boolean disableOthers,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    // try {
    return mService->EnableNetwork(netId, disableOthers, succeeded);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::DisableNetwork(
    /* [in] */ Int32 netId,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    // try {
    return mService->DisableNetwork(netId, succeeded);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::Disconnect(
    /*[out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = TRUE;
    ECode ec = mService->Disconnect();
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
}

ECode CWifiManager::Reconnect(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = TRUE;
    ECode ec = mService->Reconnect();
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
}

ECode CWifiManager::Reassociate(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    *succeeded = TRUE;
    ECode ec = mService->Reassociate();
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
}

ECode CWifiManager::PingSupplicant(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    if (mService == NULL) {
        *succeeded = FALSE;
        return NOERROR;
    }
    // try {
    return mService->PingSupplicant(succeeded);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::StartScan(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);

    // try {
        mService->StartScan(NULL, NULL);
        *succeeded = TRUE;
        return NOERROR;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::GetConnectionInfo(
    /* [out] */ IWifiInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    AutoPtr<IWifiInfo> temp;
    FAIL_RETURN(mService->GetConnectionInfo((IWifiInfo**)&temp));
    *info = temp;
    REFCOUNT_ADD(*info);
    return NOERROR;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CWifiManager::GetScanResults(
    /* [out] */ IList** results)
{
    VALIDATE_NOT_NULL(results);
    *results = NULL;

    String packageName;
    mContext->GetOpPackageName(&packageName);
    FAIL_RETURN(mService->GetScanResults(packageName, results));
    return NOERROR;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CWifiManager::SaveConfiguration(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    // try {
    ECode ec = mService->SaveConfiguration(succeeded);
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode CWifiManager::SetCountryCode(
    /* [in] */ const String& country,
    /* [in] */ Boolean persist)
{
    // try {
    return mService->SetCountryCode(country, persist);
    // } catch (RemoteException e) { }
}

ECode CWifiManager::SetFrequencyBand(
    /* [in] */ Int32 band,
    /* [in] */ Boolean persist)
{
    // try {
    return mService->SetFrequencyBand(band, persist);
    // } catch (RemoteException e) { }
}

ECode CWifiManager::GetFrequencyBand(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    // try {
    ECode ec = mService->GetFrequencyBand(state);
    if (FAILED(ec)) {
        *state = -1;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     return -1;
    // }
}

ECode CWifiManager::IsDualBandSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);
    // try {
    ECode ec = mService->IsDualBandSupported(supported);
    if (FAILED(ec)) {
        *supported = FALSE;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode CWifiManager::IsIbssSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);
    // try {
    ECode ec = mService->IsIbssSupported(supported);
    if (FAILED(ec)) {
        *supported = FALSE;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::GetDhcpInfo(
    /* [out] */ IDhcpInfo** info)
{
    VALIDATE_NOT_NULL(info);
    // try {
    ECode ec = mService->GetDhcpInfo(info);
    if (FAILED(ec)) {
        *info = NULL;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     return NULL;
    // }
}

ECode CWifiManager::SetWifiEnabled(
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    Int32 result = 0;
    mAppOps->NoteOp(IAppOpsManager::OP_WIFI_CHANGE, &result);
    if (result != IAppOpsManager::MODE_ALLOWED) {
        *succeeded = FALSE;
        return NOERROR;
    }
    // try {
    ECode ec = mService->SetWifiEnabled(enabled, succeeded);
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     return FALSE;
    // }
}

ECode CWifiManager::GetWifiState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    // try {
    ECode ec = mService->GetWifiEnabledState(state);
    if (FAILED(ec)) {
        *state = IWifiManager::WIFI_STATE_UNKNOWN;
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     return WIFI_STATE_UNKNOWN;
    // }
}

ECode CWifiManager::IsWifiEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    Int32 state = 0;
    *enabled = (GetWifiState(&state), state) == WIFI_STATE_ENABLED;
    return NOERROR;
}

ECode CWifiManager::GetTxPacketCount(
    /* [in] */ IWifiManagerTxPacketCountListener* listener)
{
    FAIL_RETURN(ValidateChannel());
    sAsyncChannel->SendMessage(IWifiManager::RSSI_PKTCNT_FETCH, 0, PutListener(listener));
    return NOERROR;
}

Int32 CWifiManager::CalculateSignalLevel(
    /* [in] */ Int32 rssi,
    /* [in] */ Int32 numLevels)
{
    if (rssi <= MIN_RSSI) {
        return 0;
    }
    else if (rssi >= MAX_RSSI) {
        return numLevels - 1;
    }
    else {
        Float inputRange = (MAX_RSSI - MIN_RSSI);
        Float outputRange = (numLevels - 1);
        return (Int32)((Float)(rssi - MIN_RSSI) * outputRange / inputRange);
    }
}

Int32 CWifiManager::CompareSignalLevel(
    /* [in] */ Int32 rssiA,
    /* [in] */ Int32 rssiB)
{
    return rssiA - rssiB;
}

ECode CWifiManager::SetWifiApEnabled(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [in] */ Boolean enabled,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    //try {
    *succeeded = TRUE;
    ECode ec = mService->SetWifiApEnabled(wifiConfig, enabled);
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    //    return TRUE;
    //} catch (RemoteException e) {
    //    return FALSE;
    //}
}

ECode CWifiManager::GetWifiApState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    //try {
    ECode ec = mService->GetWifiApEnabledState(state);
    if (FAILED(ec)) {
        *state = WIFI_AP_STATE_FAILED;
    }
    return NOERROR;
    //} catch (RemoteException e) {
    //    return WIFI_AP_STATE_FAILED;
    //}
}

ECode CWifiManager::IsWifiApEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    Int32 state = 0;
    *enabled = (GetWifiApState(&state), state) == WIFI_AP_STATE_ENABLED;
    return NOERROR;
}

ECode CWifiManager::GetWifiApConfiguration(
    /* [out] */ IWifiConfiguration** config)
{
    VALIDATE_NOT_NULL(config);
    //try {
    ECode ec = mService->GetWifiApConfiguration(config);
    if (FAILED(ec)) {
        *config = NULL;
    }
    return NOERROR;
    //} catch (RemoteException e) {
    //    return NULL;
    //}
}

ECode CWifiManager::SetWifiApConfiguration(
    /* [in] */ IWifiConfiguration* wifiConfig,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    //try {
    *succeeded = TRUE;
    ECode ec = mService->SetWifiApConfiguration(wifiConfig);
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    //    return TRUE;
    //} catch (RemoteException e) {
    //    return FALSE;
    //}
}

ECode CWifiManager::StartWifi(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    //try {
    *succeeded = TRUE;
    ECode ec = mService->StartWifi();
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    //    return TRUE;
    //} catch (RemoteException e) {
    //    return FALSE;
    //}
}

ECode CWifiManager::StopWifi(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    //try {
    *succeeded = TRUE;
    ECode ec = mService->StopWifi();
    if (ec != NOERROR) {
        *succeeded = FALSE;
    }
    return NOERROR;
    //    return TRUE;
    //} catch (RemoteException e) {
    //    return FALSE;
    //}
}

ECode CWifiManager::AddToBlacklist(
    /* [in] */ const String& bssid,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    //try {
    *succeeded = TRUE;
    ECode ec = mService->AddToBlacklist(bssid);
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    //    return TRUE;
    //} catch (RemoteException e) {
    //    return FALSE;
    //}
}

ECode CWifiManager::ClearBlacklist(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    //try {
    *succeeded = TRUE;
    ECode ec = mService->ClearBlacklist();
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    //    return TRUE;
    //} catch (RemoteException e) {
    //    return FALSE;
    //}
}

Int32 CWifiManager::PutListener(
    /* [in] */ IInterface* listener)
{
    if (listener == NULL) return INVALID_KEY;
    Int32 key = 0;
    {
        AutoLock lock(sListenerMapLock);

        do {
            key = sListenerKey++;
        } while (key == INVALID_KEY);
        sListenerMap->Put(key, listener);
    }
    return key;
}

AutoPtr<IInterface> CWifiManager::RemoveListener(
    /* [in] */ Int32 key)
{
    if (key == INVALID_KEY) return NULL;
    {
        AutoLock lock(sListenerMapLock);

        AutoPtr<IInterface> listener;
        sListenerMap->Get(key, (IInterface**)&listener);
        sListenerMap->Remove(key);
        return listener;
    }
}

void CWifiManager::Init()
{
    AutoLock lock(sThreadRefLock);
    if (++sThreadRefCount == 1) {
        AutoPtr<IMessenger> messenger;
        GetWifiServiceMessenger((IMessenger**)&messenger);
        if (messenger == NULL) {
            sAsyncChannel = NULL;
            return;
        }

        CHandlerThread::New(String("WifiManager"), (IHandlerThread**)&sHandlerThread);
        CAsyncChannel::New((IAsyncChannel**)&sAsyncChannel);
        CCountDownLatch::New(1, (ICountDownLatch**)&sConnected);

        IThread::Probe(sHandlerThread)->Start();
        AutoPtr<ILooper> looper;
        sHandlerThread->GetLooper((ILooper**)&looper);
        AutoPtr<IHandler> handler = new ServiceHandler(looper);
        sAsyncChannel->Connect(mContext, handler, messenger);
        // try {
        sConnected->Await();
        // } catch (InterruptedException e) {
        //     Log.e(TAG, "interrupted wait at init");
        // }
    }
}

ECode CWifiManager::ValidateChannel()
{
    if (sAsyncChannel == NULL) {
        //throw new IllegalStateException
        Logger::E(TAG,
           "No permission to access and change wifi or a bad initialization");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

Int32 CWifiManager::GetSupportedFeatures()
{
    Int32 features;
    // try {
        mService->GetSupportedFeatures(&features);
    // } catch (RemoteException e) {
    //     return 0;
    // }
    return features;
}

Boolean CWifiManager::IsFeatureSupported(
    /* [in] */ Int32 feature)
{
    return (GetSupportedFeatures() & feature) == feature;
}

ECode CWifiManager::Connect(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ IWifiManagerActionListener* listener)
{
    if (config == NULL) {
        // throw new IllegalArgumentException("config cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(ValidateChannel());
    // Use INVALID_NETWORK_ID for arg1 when passing a config object
    // arg1 is used to pass network id when the network already exists
    sAsyncChannel->SendMessage(CONNECT_NETWORK, IWifiConfiguration::INVALID_NETWORK_ID,
           PutListener(listener), config);
    return NOERROR;
}

ECode CWifiManager::Connect(
    /* [in] */ Int32 networkId,
    /* [in] */ IWifiManagerActionListener* listener)
{
    if (networkId < 0) {
        // throw new IllegalArgumentException("Network id cannot be negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(ValidateChannel());
    sAsyncChannel->SendMessage(CONNECT_NETWORK, networkId, PutListener(listener));
    return NOERROR;
}

ECode CWifiManager::Save(
    /* [in] */ IWifiConfiguration* config,
    /* [in] */ IWifiManagerActionListener* listener)
{
    if (config == NULL) {
        // throw new IllegalArgumentException("config cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(ValidateChannel());
    sAsyncChannel->SendMessage(SAVE_NETWORK, 0, PutListener(listener), config);
    return NOERROR;
}

ECode CWifiManager::Forget(
    /* [in] */ Int32 netId,
    /* [in] */ IWifiManagerActionListener* listener)
{
    if (netId < 0) {
        // throw new IllegalArgumentException("Network id cannot be negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(ValidateChannel());
    sAsyncChannel->SendMessage(FORGET_NETWORK, netId, PutListener(listener));
    return NOERROR;
}

ECode CWifiManager::Disable(
    /* [in] */ Int32 netId,
    /* [in] */ IWifiManagerActionListener* listener)
{
    if (netId < 0) {
        // throw new IllegalArgumentException("Network id cannot be negative");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(ValidateChannel());
    sAsyncChannel->SendMessage(DISABLE_NETWORK, netId, PutListener(listener));
    return NOERROR;
}

ECode CWifiManager::StartWps(
    /* [in] */ IWpsInfo* config,
    /* [in] */ IWifiManagerWpsCallback* listener)
{
    if (config == NULL) {
        // throw new IllegalArgumentException("config cannot be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    FAIL_RETURN(ValidateChannel());
    return sAsyncChannel->SendMessage(START_WPS, 0, PutListener(listener), config);
}

ECode CWifiManager::CancelWps(
    /* [in] */ IWifiManagerWpsCallback* listener)
{
    FAIL_RETURN(ValidateChannel());
    return sAsyncChannel->SendMessage(CANCEL_WPS, 0, PutListener(listener));
}

ECode CWifiManager::GetWifiServiceMessenger(
    /* [out] */ IMessenger** msgr)
{
    VALIDATE_NOT_NULL(msgr);
    //try {
    ECode ec = mService->GetWifiServiceMessenger(msgr);
    if (FAILED(ec)) {
        *msgr = NULL;
    }
    return NOERROR;
    //} catch (RemoteException e) {
    //    return NULL;
    //} catch (SecurityException e) {
    //    return NULL;
    //}
}

ECode CWifiManager::GetConfigFile(
    /* [out] */ String* cfgFile)
{
    VALIDATE_NOT_NULL(cfgFile);
    //try {
    return mService->GetConfigFile(cfgFile);
    //} catch (RemoteException e) {
    //    return NULL;
    //}
}

ECode CWifiManager::CreateWifiLock(
    /* [in] */ Int32 lockType,
    /* [in] */ const String& tag,
    /* [out] */ IWifiManagerWifiLock** lock)
{
    VALIDATE_NOT_NULL(lock);
    *lock = new WifiLock(lockType, tag, this);
    REFCOUNT_ADD(*lock);
    return NOERROR;
}

ECode CWifiManager::CreateWifiLock(
    /* [in] */ const String& tag,
    /* [out] */ IWifiManagerWifiLock** lock)
{
    VALIDATE_NOT_NULL(lock);
    *lock = new WifiLock(WIFI_MODE_FULL, tag, this);
    REFCOUNT_ADD(*lock);
    return NOERROR;
}

ECode CWifiManager::CreateMulticastLock(
    /* [in] */ const String& tag,
    /* [out] */ IWifiManagerMulticastLock** lock)
{
    VALIDATE_NOT_NULL(lock);
    *lock = new MulticastLock(tag, this);
    REFCOUNT_ADD(*lock);
    return NOERROR;
}

ECode CWifiManager::IsMulticastEnabled(
    /* [out] */ Boolean* allowed)
{
    VALIDATE_NOT_NULL(allowed);
    //try {
    ECode ec = mService->IsMulticastEnabled(allowed);
    if (FAILED(ec)) {
        *allowed = FALSE;
    }
    return NOERROR;
    //} catch (RemoteException e) {
    //    return FALSE;
    //}
}

ECode CWifiManager::InitializeMulticastFiltering(
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    //try {
    *succeeded = TRUE;
    ECode ec = mService->InitializeMulticastFiltering();
    if (FAILED(ec)) {
        *succeeded = FALSE;
    }
    return NOERROR;
    //    return TRUE;
    //} catch (RemoteException e) {
    //     return FALSE;
    //}
}

ECode CWifiManager::GetPrivilegedConfiguredNetworks(
    /* [out] */ IList** result)
{
    // try {
        return mService->GetPrivilegedConfiguredNetworks(result);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiManager::GetConnectionStatistics(
    /* [out] */ IWifiConnectionStatistics** result)
{
    // try {
        return mService->GetConnectionStatistics(result);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiManager::GetChannelList(
    /* [out] */ IList** result)
{
    // try {
        return mService->GetChannelList(result);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiManager::Is5GHzBandSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_INFRA_5G);
    return NOERROR;
}

ECode CWifiManager::IsPasspointSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_PASSPOINT);
    return NOERROR;
}

ECode CWifiManager::IsP2pSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_P2P);
    return NOERROR;
}

ECode CWifiManager::IsPortableHotspotSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_MOBILE_HOTSPOT);
    return NOERROR;
}

ECode CWifiManager::IsWifiScannerSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_SCANNER);
    return NOERROR;
}

ECode CWifiManager::IsNanSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_NAN);
    return NOERROR;
}

ECode CWifiManager::IsDeviceToDeviceRttSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_D2D_RTT);
    return NOERROR;
}

ECode CWifiManager::IsDeviceToApRttSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_D2AP_RTT);
    return NOERROR;
}

ECode CWifiManager::IsPreferredNetworkOffloadSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_PNO);
    return NOERROR;
}

ECode CWifiManager::IsAdditionalStaSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_ADDITIONAL_STA);
    return NOERROR;
}

ECode CWifiManager::IsTdlsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_TDLS);
    return NOERROR;
}

ECode CWifiManager::IsOffChannelTdlsSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_TDLS_OFFCHANNEL);
    return NOERROR;
}

ECode CWifiManager::IsEnhancedPowerReportingSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsFeatureSupported(WIFI_FEATURE_EPR);
    return NOERROR;
}

ECode CWifiManager::GetControllerActivityEnergyInfo(
    /* [in] */ Int32 updateType,
    /* [out] */ IWifiActivityEnergyInfo** result)
{
    VALIDATE_NOT_NULL(result);

    if (mService == NULL) {
        *result = NULL;
        return NOERROR;
    }

    // try {
        Boolean bFlag;
        IsEnhancedPowerReportingSupported(&bFlag);
        if (!bFlag) {
            *result = NULL;
            return NOERROR;
        }

        {
            AutoLock lock(this);
            mService->ReportActivityInfo(result);
            Boolean bFlag;
            (*result)->IsValid(&bFlag);
            if (bFlag) {
                return NOERROR;
            }
            else {
                *result = NULL;
                return NOERROR;
            }
        }
    // } catch (RemoteException e) {
    //     Log.e(TAG, "getControllerActivityEnergyInfo: " + e);
    // }

    *result = NULL;

    return NOERROR;
}

ECode CWifiManager::StartScan(
    /* [in] */ IWorkSource* workSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        mService->StartScan(NULL, workSource);
        *result = TRUE;
        return NOERROR;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::StartCustomizedScan(
    /* [in] */ IScanSettings* requested,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        mService->StartScan(requested, NULL);
        *result = TRUE;
        return NOERROR;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::StartCustomizedScan(
    /* [in] */ IScanSettings* requested,
    /* [in] */ IWorkSource* workSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        mService->StartScan(requested, workSource);
        *result = TRUE;
        return NOERROR;
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::RequestBatchedScan(
    /* [in] */ IBatchedScanSettings* requested,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        AutoPtr<IBinder> binder;
        CBinder::New((IBinder**)&binder);
        return mService->RequestBatchedScan(requested, binder, NULL, result);
    // } catch (RemoteException e) { return false; }
}

ECode CWifiManager::RequestBatchedScan(
    /* [in] */ IBatchedScanSettings* requested,
    /* [in] */ IWorkSource* workSource,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        AutoPtr<IBinder> binder;
        CBinder::New((IBinder**)&binder);
        return mService->RequestBatchedScan(requested, binder, workSource, result);
    // } catch (RemoteException e) { return false; }
}

ECode CWifiManager::IsBatchedScanSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        return mService->IsBatchedScanSupported(result);
    // } catch (RemoteException e) { return false; }
}

ECode CWifiManager::StopBatchedScan(
    /* [in] */ IBatchedScanSettings* requested)
{
    // try {
        return mService->StopBatchedScan(requested);
    // } catch (RemoteException e) {}
}

ECode CWifiManager::GetBatchedScanResults(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        String packageName;
        mContext->GetOpPackageName(&packageName);
        return mService->GetBatchedScanResults(packageName, result);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiManager::PollBatchedScan()
{
    // try {
        return mService->PollBatchedScan();
    // } catch (RemoteException e) { }
}

ECode CWifiManager::GetWpsNfcConfigurationToken(
    /* [in] */ Int32 netId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        return mService->GetWpsNfcConfigurationToken(netId, result);
    // } catch (RemoteException e) {
    //     return null;
    // }
}

ECode CWifiManager::IsScanAlwaysAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        return mService->IsScanAlwaysAvailable(result);
    // } catch (RemoteException e) {
    //     return false;
    // }
}

ECode CWifiManager::SetTdlsEnabled(
    /* [in] */ IInetAddress* remoteIPAddress,
    /* [in] */ Boolean enable)
{
    // try {
        String address;
        remoteIPAddress->GetHostAddress(&address);
        return mService->EnableTdls(address, enable);
    // } catch (RemoteException e) {
    //     // Just ignore the exception
    // }
}

ECode CWifiManager::SetTdlsEnabledWithMacAddress(
    /* [in] */ const String& remoteMacAddress,
    /* [in] */ Boolean enable)
{
    // try {
        return mService->EnableTdlsWithMacAddress(remoteMacAddress, enable);
    // } catch (RemoteException e) {
    //     // Just ignore the exception
    // }
}

ECode CWifiManager::EnableVerboseLogging(
    /* [in] */ Int32 verbose)
{
    // try {
        return mService->EnableVerboseLogging(verbose);
    // } catch (Exception e) {
    //     //ignore any failure here
    //     Log.e(TAG, "enableVerboseLogging " + e.toString());
    // }
}

ECode CWifiManager::GetVerboseLoggingLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    //try {
        return mService->GetVerboseLoggingLevel(result);
    // } catch (RemoteException e) {
    //     return 0;
    // }
}

ECode CWifiManager::EnableAggressiveHandover(
    /* [in] */ Int32 enabled)
{
    // try {
        return mService->EnableAggressiveHandover(enabled);
    // } catch (RemoteException e) {

    // }
}

ECode CWifiManager::GetAggressiveHandover(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
        return mService->GetAggressiveHandover(result);
    // } catch (RemoteException e) {
    //     return 0;
    // }
}

ECode CWifiManager::SetAllowScansWithTraffic(
    /* [in] */ Int32 enabled)
{
    // try {
        return mService->SetAllowScansWithTraffic(enabled);
    // } catch (RemoteException e) {

    // }
}

ECode CWifiManager::GetAllowScansWithTraffic(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // try {
        return mService->GetAllowScansWithTraffic(result);
    // } catch (RemoteException e) {
    //     return 0;
    // }
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
