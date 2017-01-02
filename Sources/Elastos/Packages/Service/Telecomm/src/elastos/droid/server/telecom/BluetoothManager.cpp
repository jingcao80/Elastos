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

#include "elastos/droid/server/telecom/BluetoothManager.h"
#include "elastos/droid/server/telecom/CallAudioManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/os/SystemClock.h>

using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::EIID_IBluetoothProfileServiceListener;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Os::SystemClock;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// BluetoothManager::SubBluetoothProfileServiceListener
//=============================================================================
CAR_INTERFACE_IMPL(BluetoothManager::SubBluetoothProfileServiceListener, Object, IBluetoothProfileServiceListener)

BluetoothManager::SubBluetoothProfileServiceListener::SubBluetoothProfileServiceListener(
    /* [in] */ BluetoothManager* host)
    : mHost(host)
{}

ECode BluetoothManager::SubBluetoothProfileServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    mHost->mBluetoothHeadset = IBluetoothHeadset::Probe(proxy);
    Log::V("BluetoothManager", "- Got BluetoothHeadset: %s", TO_CSTR(mHost->mBluetoothHeadset));
    return NOERROR;
}

ECode BluetoothManager::SubBluetoothProfileServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    mHost->mBluetoothHeadset = NULL;
    return NOERROR;
}

//=============================================================================
// BluetoothManager::SubBroadcastReceiver
//=============================================================================
BluetoothManager::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ BluetoothManager* host)
    : mHost(host)
{}

ECode BluetoothManager::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Int32 bluetoothState = 0;
    if (action.Equals(IBluetoothHeadset::ACTION_CONNECTION_STATE_CHANGED)) {
        intent->GetInt32Extra(IBluetoothProfile::EXTRA_STATE,
                IBluetoothProfile::STATE_DISCONNECTED, &bluetoothState);
        Log::D("BluetoothManager", "mReceiver: HEADSET_STATE_CHANGED_ACTION");
        Log::D("BluetoothManager", "==> new state: %d ", bluetoothState);
    } else if (action.Equals(IBluetoothHeadset::ACTION_AUDIO_STATE_CHANGED)) {
        intent->GetInt32Extra(IBluetoothProfile::EXTRA_STATE,
                IBluetoothHeadset::STATE_AUDIO_DISCONNECTED, &bluetoothState);
        Log::D("BluetoothManager", "mReceiver: HEADSET_AUDIO_STATE_CHANGED_ACTION");
        Log::D("BluetoothManager", "==> new state: %d", bluetoothState);
    }
    if (bluetoothState == IBluetoothProfile::STATE_DISCONNECTED ||
            bluetoothState == IBluetoothHeadset::STATE_AUDIO_DISCONNECTED) {
        mHost->mBluetoothConnectionPending = FALSE;
    }
    mHost->UpdateBluetoothState();
    return NOERROR;
}

//=============================================================================
// BluetoothManager
//=============================================================================
BluetoothManager::BluetoothManager()
    : mBluetoothConnectionPending(FALSE)
    , mBluetoothConnectionRequestTime(0)
{
    mBluetoothProfileServiceListener = new SubBluetoothProfileServiceListener(this);
    mReceiver = new SubBroadcastReceiver(this);
}

ECode BluetoothManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ CallAudioManager* callAudioManager)
{
    AutoPtr<IBluetoothAdapterHelper> bluetoothAdapterHelper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&bluetoothAdapterHelper);
    bluetoothAdapterHelper->GetDefaultAdapter((IBluetoothAdapter**)&mBluetoothAdapter);

    IWeakReferenceSource* source = IWeakReferenceSource::Probe(TO_IINTERFACE(callAudioManager));
    source->GetWeakReference((IWeakReference**)&mCallAudioManager);

    if (mBluetoothAdapter != NULL) {
        Boolean bNoUse;
        mBluetoothAdapter->GetProfileProxy(context, mBluetoothProfileServiceListener,
                IBluetoothProfile::HEADSET, &bNoUse);
    }
    // Register for misc other intent broadcasts.
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IBluetoothHeadset::ACTION_CONNECTION_STATE_CHANGED, (IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IBluetoothHeadset::ACTION_AUDIO_STATE_CHANGED);
    AutoPtr<IIntent> iNoUse;
    context->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&iNoUse);
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Log::V("BluetoothManager", "isBluetoothAvailable()...");
    // There's no need to ask the Bluetooth system service if BT is enabled:
    //
    //    BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
    //    if ((adapter == null) || !adapter.isEnabled()) {
    //        Log.d(this, "  ==> FALSE (BT not enabled)");
    //        return FALSE;
    //    }
    //    Log.d(this, "  - BT enabled!  device name " + adapter.getName()
    //                 + ", address " + adapter.getAddress());
    //
    // ...since we already have a BluetoothHeadset instance.  We can just
    // call isConnected() on that, and assume it'll be false if BT isn't
    // enabled at all.
    // Check if there's a connected headset, using the BluetoothHeadset API.
    Boolean isConnected = FALSE;
    if (mBluetoothHeadset != NULL) {
        AutoPtr<IList> deviceList;
        mBluetoothHeadset->GetConnectedDevices((IList**)&deviceList);
        Int32 size;
        deviceList->GetSize(&size);
        if (size > 0) {
            isConnected = TRUE;
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                deviceList->Get(i, (IInterface**)&obj);
                AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(obj);
                Int32 state;
                mBluetoothHeadset->GetConnectionState(device, &state);
                Log::V("BluetoothManager", "state = %d for headset: %s", state, TO_CSTR(device));
            }
        }
    }
    Log::V("BluetoothManager", "  ==> %d", isConnected);
    *result = isConnected;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAudioConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mBluetoothHeadset == NULL) {
        Log::V("BluetoothManager", "isBluetoothAudioConnected: ==> FALSE (null mBluetoothHeadset)");
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IList> deviceList;
    mBluetoothHeadset->GetConnectedDevices((IList**)&deviceList);
    Boolean isEmpty;
    deviceList->IsEmpty(&isEmpty);
    if (isEmpty) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 size;
    deviceList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        deviceList->Get(i, (IInterface**)&obj);
        AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(obj);
        Boolean isAudioOn;
        mBluetoothHeadset->IsAudioConnected(device, &isAudioOn);
        Log::V("BluetoothManager", "isBluetoothAudioConnected: ==> isAudioOn = %d for headset: %s", isAudioOn, TO_CSTR(device));
        if (isAudioOn) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::IsBluetoothAudioConnectedOrPending(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isBluetoothAudioConnected;
    IsBluetoothAudioConnected(&isBluetoothAudioConnected);
    if (isBluetoothAudioConnected) {
        Log::V("BluetoothManager", "isBluetoothAudioConnectedOrPending: ==> TRUE (really connected)");
        *result = TRUE;
        return NOERROR;
    }
    // If we issued a connectAudio() call "recently enough", even
    // if BT isn't actually connected yet, let's still pretend BT is
    // on.  This makes the onscreen indication more responsive.
    if (mBluetoothConnectionPending) {
        Int64 timeSinceRequest =
                SystemClock::GetElapsedRealtime() - mBluetoothConnectionRequestTime;
        if (timeSinceRequest < 5000 /* 5 seconds */) {
            Log::V("BluetoothManager", "isBluetoothAudioConnectedOrPending: ==> TRUE (requested %lld msec ago)", timeSinceRequest);
            *result = TRUE;
            return NOERROR;
        } else {
            Log::V("BluetoothManager", "isBluetoothAudioConnectedOrPending: ==> FALSE (request too old: %lld msec ago)", timeSinceRequest);
            mBluetoothConnectionPending = FALSE;
            *result = FALSE;
            return NOERROR;
        }
    }
    Log::V("BluetoothManager", "isBluetoothAudioConnectedOrPending: ==> FALSE");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothManager::UpdateBluetoothState()
{
    AutoPtr<IInterface> ws;
    mCallAudioManager->Resolve(EIID_IInterface, (IInterface**)&ws);
    if (ws != NULL) {
        ((CallAudioManager*) IObject::Probe(ws))->OnBluetoothStateChange(this);
    } else {
        Log::E("BluetoothManager", "mCallAudioManager is NULL");
        assert(0);
    }
    return NOERROR;
}

ECode BluetoothManager::ConnectBluetoothAudio()
{
    Log::V("BluetoothManager", "connectBluetoothAudio()...");
    if (mBluetoothHeadset != NULL) {
        Boolean bNoUse;
        mBluetoothHeadset->ConnectAudio(&bNoUse);
    }
    // Watch out: The bluetooth connection doesn't happen instantly;
    // the connectAudio() call returns instantly but does its real
    // work in another thread.  The mBluetoothConnectionPending flag
    // is just a little trickery to ensure that the onscreen UI updates
    // instantly. (See isBluetoothAudioConnectedOrPending() above.)
    mBluetoothConnectionPending = TRUE;
    mBluetoothConnectionRequestTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode BluetoothManager::DisconnectBluetoothAudio()
{
    Log::V("BluetoothManager", "disconnectBluetoothAudio()...");
    if (mBluetoothHeadset != NULL) {
        Boolean bNoUse;
        mBluetoothHeadset->DisconnectAudio(&bNoUse);
    }
    mBluetoothConnectionPending = FALSE;
    return NOERROR;
}

ECode BluetoothManager::DumpBluetoothState()
{
    Log::D("BluetoothManager", "============== dumpBluetoothState() =============");
    Boolean isBluetoothAvailable;
    IsBluetoothAvailable(&isBluetoothAvailable);
    Log::D("BluetoothManager", "= isBluetoothAvailable: %d", isBluetoothAvailable);
    Boolean isBluetoothAudioConnected;
    IsBluetoothAudioConnected(&isBluetoothAudioConnected);
    Log::D("BluetoothManager", "= isBluetoothAudioConnected: %d", isBluetoothAudioConnected);
    Boolean isBluetoothAudioConnectedOrPending;
    IsBluetoothAudioConnectedOrPending(&isBluetoothAudioConnectedOrPending);
    Log::D("BluetoothManager", "= isBluetoothAudioConnectedOrPending: %d", isBluetoothAudioConnectedOrPending);
    Log::D("BluetoothManager", "=");
    if (mBluetoothAdapter != NULL) {
        if (mBluetoothHeadset != NULL) {
            AutoPtr<IList> deviceList;
            mBluetoothHeadset->GetConnectedDevices((IList**)&deviceList);
            Int32 size;
            deviceList->GetSize(&size);
            if (size > 0) {
                AutoPtr<IInterface> obj;
                deviceList->Get(0, (IInterface**)&obj);
                AutoPtr<IBluetoothDevice> device = IBluetoothDevice::Probe(obj);
                Log::D("BluetoothManager", "= BluetoothHeadset.getCurrentDevice: %s", TO_CSTR(device));
                Int32 state;
                mBluetoothHeadset->GetConnectionState(device, &state);
                Log::D("BluetoothManager", "= BluetoothHeadset.State: %d", state);
                Boolean isAudioConnected;
                mBluetoothHeadset->IsAudioConnected(device, &isAudioConnected);
                Log::D("BluetoothManager", "= BluetoothHeadset audio connected: %d", isAudioConnected);
            }
        } else {
            Log::D("BluetoothManager", "= mBluetoothHeadset is null");
        }
    } else {
        Log::D("BluetoothManager", "= mBluetoothAdapter is null; device is not BT capable");
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
