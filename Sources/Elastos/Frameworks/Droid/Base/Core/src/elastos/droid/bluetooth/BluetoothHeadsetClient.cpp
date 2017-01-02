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
#include "elastos/droid/bluetooth/BluetoothHeadsetClient.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Process.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//    BluetoothHeadsetClient::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothHeadsetClient::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothHeadsetClient::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

ECode BluetoothHeadsetClient::BluetoothStateChangeCallbackStub::constructor(
    /* [in] */ IBluetoothHeadsetClient* owner)
{
    mOwner = (BluetoothHeadsetClient*)owner;
    return NOERROR;
}

ECode BluetoothHeadsetClient::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
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
            //    Logger::E(TAG,"",re);
            //}
        }
    } else {
        {
            AutoLock lock(mOwner->mConnection);
            //try {
                if (mOwner->mService == NULL) {
                    if (VDBG) Logger::D(TAG,"Binding service...");
                    //Intent intent = new Intent(IBluetoothHeadsetClient.class.getName());
                    AutoPtr<IIntent> intent;
                    CIntent::New(String("BluetoothHeadsetClient")/*IBluetoothHeadsetClient.class.getName()*/, (IIntent**)&intent);
                    Boolean bind;
                    mOwner->DoBind(&bind);
                }
            //} catch (Exception re) {
            //    Logger::E(TAG,"",re);
            //}
        }
    }
    return NOERROR;
}

//=====================================================================
//            BluetoothHeadsetClient::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothHeadsetClient::InnerServiceConnection, Object, IServiceConnection);

BluetoothHeadsetClient::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ BluetoothHeadsetClient* owner)
    : mOwner(owner)
{
}

ECode BluetoothHeadsetClient::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mOwner->mService = IIBluetoothHeadsetClient::Probe(service);

    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceConnected(IBluetoothProfile::HEADSET_CLIENT,
                mOwner);
    }
    return NOERROR;
}

ECode BluetoothHeadsetClient::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    if (DBG) Logger::D(TAG, "Proxy object disconnected");
    mOwner->mService = NULL;
    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceDisconnected(IBluetoothProfile::HEADSET_CLIENT);
    }
    return NOERROR;
}

//=====================================================================
//                        BluetoothHeadsetClient
//=====================================================================
const String BluetoothHeadsetClient::TAG("BluetoothHeadsetClient");
const Boolean BluetoothHeadsetClient::DBG = TRUE;
const Boolean BluetoothHeadsetClient::VDBG = FALSE;


CAR_INTERFACE_IMPL_2(BluetoothHeadsetClient, Object, IBluetoothHeadsetClient, IBluetoothProfile);

BluetoothHeadsetClient::BluetoothHeadsetClient()
{
}

BluetoothHeadsetClient::BluetoothHeadsetClient(
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
        //   Logger::E(TAG,"",e);
        //}
    }

    Boolean bind;
    DoBind(&bind);
}

ECode BluetoothHeadsetClient::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //Intent intent = new Intent(IBluetoothHeadsetClient.class.getName());
    AutoPtr<IIntent> intent;
    CIntent::New(String("BluetoothHeadsetClient")/*IBluetoothHeadsetClient.class.getName()*/, (IIntent**)&intent);
    //ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
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
        Logger::E(TAG, "Could not bind to Bluetooth Headset Client Service with ");// + intent);
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::Close()
{
    if (VDBG) Logger::D(TAG, "close()");

    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        //try {
        mgr->UnregisterStateChangeCallback(mBluetoothStateChangeCallback);
        //} catch (Exception e) {
        //    Logger::E(TAG,"",e);
        //}
    }

    {
        AutoLock lock(mConnection);
        if (mService != NULL) {
            //try {
                mService = NULL;
                mContext->UnbindService(mConnection);
            //} catch (Exception re) {
            //    Logger::E(TAG,"",re);
            //}
        }
    }
    mServiceListener = NULL;
    return NOERROR;
}

ECode BluetoothHeadsetClient::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //if (DBG) log("connect(" + device + ")");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->Connect(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return FALSE;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //if (DBG) log("disconnect(" + device + ")");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->Disconnect(device, result);
        //} catch (RemoteException e) {
        //  Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //  return FALSE;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetConnectedDevices(
    /* [out] */ IList** devices)
{
    if (VDBG) Logger::D(TAG, "getConnectedDevices()");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetConnectedDevices(devices);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return new ArrayList<BluetoothDevice>();
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    //return new ArrayList<BluetoothDevice>();
    AutoPtr<IList> l;
    CArrayList::New((IList**)&l);
    *devices = l;
    REFCOUNT_ADD(*devices);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    VALIDATE_NOT_NULL(devices);
    if (VDBG) Logger::D(TAG, "getDevicesMatchingStates()");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetDevicesMatchingConnectionStates(states, devices);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return new ArrayList<BluetoothDevice>();
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    //return new ArrayList<BluetoothDevice>();
    AutoPtr<IList> l;
    CArrayList::New((IList**)&l);
    *devices = l;
    REFCOUNT_ADD(*devices);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    if (VDBG) Logger::D(TAG, "getConnectionState(");// + device + ")");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->GetConnectionState(device, state);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return BluetoothProfile.STATE_DISCONNECTED;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *state = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothHeadsetClient::SetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "setPriority(");// + device + ", " + priority + ")");
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
        if (priority != IBluetoothProfile::PRIORITY_OFF &&
                priority != IBluetoothProfile::PRIORITY_ON) {
          *result = FALSE;
          return NOERROR;
        }
        //try {
        return mService->SetPriority(device, priority, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return FALSE;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getPriority(");// + device + ")");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->GetPriority(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return PRIORITY_OFF;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = PRIORITY_OFF;
    return NOERROR;
}

ECode BluetoothHeadsetClient::StartVoiceRecognition(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "startVoiceRecognition()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        Boolean start;
        return mService->StartVoiceRecognition(device, &start);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::StopVoiceRecognition(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "stopVoiceRecognition()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        Boolean stop;
        return mService->StopVoiceRecognition(device, &stop);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetCurrentCalls(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "getCurrentCalls()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->GetCurrentCalls(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = NULL;
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetCurrentAgEvents(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "getCurrentCalls()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->GetCurrentAgEvents(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = NULL;
    return NOERROR;
}

ECode BluetoothHeadsetClient::AcceptCall(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 flag,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "acceptCall()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->AcceptCall(device, flag, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::HoldCall(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "holdCall()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->HoldCall(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::RejectCall(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "rejectCall()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->RejectCall(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::TerminateCall(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "terminateCall()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->TerminateCall(device, index, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::EnterPrivateMode(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "enterPrivateMode()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->EnterPrivateMode(device, index, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::ExplicitCallTransfer(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "explicitCallTransfer()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->ExplicitCallTransfer(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::Redial(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "redial()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->Redial(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::Dial(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ const String& number,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "dial()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->Dial(device, number, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::DialMemory(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 location,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "dialMemory()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->DialMemory(device, location, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::SendDTMF(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Byte code,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "sendDTMF()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        return mService->SendDTMF(device, code, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetLastVoiceTagNumber(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "getLastVoiceTagNumber()");
    if (mService != NULL && IsEnabled() &&
            IsValidDevice(device)) {
        //try {
        Boolean getRes;
        return mService->GetLastVoiceTagNumber(device, &getRes);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,  Log.getStackTraceString(new Throwable()));
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::AcceptIncomingConnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "acceptIncomingConnect");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->AcceptIncomingConnect(device, result);
        //} catch (RemoteException e) {Logger::E(TAG, e.toString());}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) Logger::D(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::RejectIncomingConnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "rejectIncomingConnect");
    if (mService != NULL) {
        //try {
        return mService->RejectIncomingConnect(device, result);
        //} catch (RemoteException e) {Logger::E(TAG, e.toString());}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) Logger::D(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetAudioState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getAudioState");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetAudioState(device, result);
        //} catch (RemoteException e) {Logger::E(TAG, e.toString());}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) Logger::D(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = IBluetoothHeadsetClient::STATE_AUDIO_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothHeadsetClient::ConnectAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService != NULL && IsEnabled()) {
        //try {
        Boolean connect;
        return mService->ConnectAudio(&connect);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, e.toString());
        //}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) Logger::D(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::DisconnectAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->DisconnectAudio(result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, e.toString());
        //}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) Logger::D(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetCurrentAgFeatures(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result);
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetCurrentAgFeatures(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, e.toString());
        //}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) Logger::D(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = NULL;
    return NOERROR;
}

Boolean BluetoothHeadsetClient::IsEnabled()
{
    Int32 state;
    if ((mAdapter->GetState(&state), state) == IBluetoothAdapter::STATE_ON) return TRUE;
    return FALSE;
}

Boolean BluetoothHeadsetClient::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) return FALSE;

    String address;
    device->GetAddress(&address);
    if (CBluetoothAdapter::CheckBluetoothAddress(address)) return TRUE;
    return FALSE;
}

void BluetoothHeadsetClient::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg.string());
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
