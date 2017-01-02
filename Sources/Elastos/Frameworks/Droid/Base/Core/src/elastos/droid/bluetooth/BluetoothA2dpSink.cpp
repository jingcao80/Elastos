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
#include "elastos/droid/bluetooth/BluetoothA2dpSink.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Process.h"
#include "elastos/core/AutoLock.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//      BluetoothA2dpSink::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothA2dpSink::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothA2dpSink::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

ECode BluetoothA2dpSink::BluetoothStateChangeCallbackStub::constructor(
    /* [in] */ IInterface* owner)
{
    mOwner = (BluetoothA2dpSink*)(IBluetoothA2dpSink::Probe(owner));
    return NOERROR;
}

ECode BluetoothA2dpSink::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
    /* [in] */ Boolean up)
{
    if (DBG) Logger::D(TAG, "onBluetoothStateChange: up=%d", up);
    if (!up) {
        if (VDBG) Logger::D(TAG,"Unbinding service...");
        {
            AutoLock lock(mOwner->mConnection);
            //try {
                mOwner->mService = NULL;
                mOwner->mContext->UnbindService(mOwner->mConnection);
            //} catch (Exception re) {
            //    Log.e(TAG,"",re);
            //}
        }
    } else {
        {
            AutoLock lock(mOwner->mConnection);
            //try {
            if (mOwner->mService == NULL) {
                if (VDBG) Logger::D(TAG,"Binding service...");
                Boolean bindResult;
                mOwner->DoBind(&bindResult);
            }
            //} catch (Exception re) {
            //    Log.e(TAG,"",re);
            //}
        }
    }
    return NOERROR;
}

//=====================================================================
//              BluetoothA2dpSink::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothA2dpSink::InnerServiceConnection, Object, IServiceConnection);

BluetoothA2dpSink::InnerServiceConnection::InnerServiceConnection()
{
}

BluetoothA2dpSink::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ IBluetoothA2dpSink* owner)
{
    mOwner = (BluetoothA2dpSink*)owner;
}

ECode BluetoothA2dpSink::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mOwner->mService = IIBluetoothA2dpSink::Probe(service);

    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceConnected(IBluetoothProfile::A2DP_SINK,
                mOwner);
    }
    return NOERROR;
}

ECode BluetoothA2dpSink::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    if (DBG) Logger::D(TAG, "Proxy object disconnected");
    mOwner->mService = NULL;
    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceDisconnected(IBluetoothProfile::A2DP_SINK);
    }
    return NOERROR;
}

//=====================================================================
//                          BluetoothA2dpSink
//=====================================================================
const String BluetoothA2dpSink::TAG("BluetoothA2dpSink");
const Boolean BluetoothA2dpSink::DBG = TRUE;
const Boolean BluetoothA2dpSink::VDBG = FALSE;

CAR_INTERFACE_IMPL_2(BluetoothA2dpSink, Object, IBluetoothA2dpSink, IBluetoothProfile);

BluetoothA2dpSink::BluetoothA2dpSink()
{
}

BluetoothA2dpSink::~BluetoothA2dpSink()
{
    Close();
}

BluetoothA2dpSink::BluetoothA2dpSink(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    mContext = context;
    mServiceListener = l;
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        //try {
        mgr->RegisterStateChangeCallback(mBluetoothStateChangeCallback);
        //} catch (RemoteException e) {
        //    Log.e(TAG,"",e);
        //}
    }


    Boolean bindResult;
    DoBind(&bindResult);
}

ECode BluetoothA2dpSink::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIntent> intent;
    CIntent::New(String("IBluetoothA2dpSink")/*IBluetoothA2dpSink.class.getName()*/, (IIntent**)&intent);
    AutoPtr<IComponentName> comp;
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    intent->SetComponent(comp);
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    Boolean succeeded = FALSE;

    if (comp == NULL || !(mContext->BindServiceAsUser(intent, mConnection, 0,
            userHandle, &succeeded), succeeded)) {
        Logger::E(TAG, "Could not bind to Bluetooth A2DP Service with ");// + intent;
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothA2dpSink::Close()
{
    mServiceListener = NULL;
    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        //try {
        mgr->UnregisterStateChangeCallback(mBluetoothStateChangeCallback);
        //} catch (Exception e) {
        //    Log.e(TAG,"",e);
        //}
    }

    {
        AutoLock lock(mConnection);
        if (mService != NULL) {
            //try {
            mService = NULL;
            mContext->UnbindService(mConnection);
            //} catch (Exception re) {
            //    Log.e(TAG,"",re);
            //}
        }
    }
    return NOERROR;
}

ECode BluetoothA2dpSink::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "connect( device )");
    if (mService != NULL && IsEnabled() &&
        IsValidDevice(device)) {
        //try {
        return mService->Connect(device, result);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //    return false;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothA2dpSink::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "disconnect( device )");
    if (mService != NULL && IsEnabled() &&
        IsValidDevice(device)) {
        //try {
        return mService->Disconnect(device, result);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //    return false;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothA2dpSink::GetConnectedDevices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getConnectedDevices()");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetConnectedDevices(result);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //    return new ArrayList<BluetoothDevice>();
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    *result = list;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothA2dpSink::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getDevicesMatchingStates()");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetDevicesMatchingConnectionStates(states, result);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //    return new ArrayList<BluetoothDevice>();
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    AutoPtr<IList> list;
    CArrayList::New((IList**)&list);
    *result = list;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothA2dpSink::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getState(device)");
    if (mService != NULL && IsEnabled()
        && IsValidDevice(device)) {
        //try {
        return mService->GetConnectionState(device, result);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //    return BluetoothProfile.STATE_DISCONNECTED;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothA2dpSink::GetAudioConfig(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IBluetoothAudioConfig** result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getAudioConfig( device )");
    if (mService != NULL && IsEnabled()
        && IsValidDevice(device)) {
        //try {
        return mService->GetAudioConfig(device, result);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //    return null;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = NULL;
    return NOERROR;
}

ECode BluetoothA2dpSink::SetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    String str;
    IObject::Probe(device)->ToString(&str);
    if (VDBG) Logger::D(TAG, String("setPriority(") + str + ", " + StringUtils::ToString(priority) + ")");
    *result = FALSE;
    if (mService != NULL && IsEnabled()
        && IsValidDevice(device)) {
        if (priority != IBluetoothProfile::PRIORITY_OFF &&
            priority != IBluetoothProfile::PRIORITY_ON){
            return NOERROR;
        }
        // try {
            if (FAILED(mService->SetPriority(device, priority, result))) {
                Logger::E(TAG, "SetPriority: Remote invoke error");
            }
        // } catch (RemoteException e) {
        //    Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //    *result = false;
        //     return NOERROR;
        // }
    }
    if (mService == NULL)
        Logger::W(TAG, "Proxy not attached to service");
    return NOERROR;
}

ECode BluetoothA2dpSink::GetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* priority)
{
    String str;
    IObject::Probe(device)->ToString(&str);
    if (VDBG) Logger::D(TAG, String("GetPriority(") + str);
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
        // try {
            if (FAILED(mService->GetPriority(device, priority))) {
                Logger::E(TAG, "GetPriority: Remote invoke error");
                *priority = IBluetoothProfile::PRIORITY_OFF;
            }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            // return BluetoothProfile.PRIORITY_OFF;
        // }
    }
    if (mService == NULL)
        Logger::W(TAG, "Proxy not attached to service");
    *priority = IBluetoothProfile::PRIORITY_OFF;
    return NOERROR;
}

ECode BluetoothA2dpSink::IsA2dpPlaying(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
        // try {
            if (FAILED(mService->IsA2dpPlaying(device, result))) {
                Logger::E(TAG, "IsA2dpPlaying: Remote invoke error");
            }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            // return false;
        // }
    }
    if (mService == NULL)
        Logger::W(TAG, "Proxy not attached to service");
    return NOERROR;
}

String BluetoothA2dpSink::StateToString(
    /* [in] */ Int32 state)
{
    switch (state) {
    case STATE_DISCONNECTED:
        return String("disconnected");
    case STATE_CONNECTING:
        return String("connecting");
    case STATE_CONNECTED:
        return String("connected");
    case STATE_DISCONNECTING:
        return String("disconnecting");
    case STATE_PLAYING:
        return String("playing");
    case STATE_NOT_PLAYING:
      return String("not playing");
    default:
        return String("<unknown state ") + StringUtils::ToString(state) + String(">");
    }
    return String("");
}

Boolean BluetoothA2dpSink::IsEnabled()
{
    Int32 state;
    if ((mAdapter->GetState(&state), state) == IBluetoothAdapter::STATE_ON) return TRUE;
    return FALSE;
}

Boolean BluetoothA2dpSink::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) return FALSE;

    String address;
    device->GetAddress(&address);
    if (CBluetoothAdapter::CheckBluetoothAddress(address)) return TRUE;
    return FALSE;
}

void BluetoothA2dpSink::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg.string());
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
