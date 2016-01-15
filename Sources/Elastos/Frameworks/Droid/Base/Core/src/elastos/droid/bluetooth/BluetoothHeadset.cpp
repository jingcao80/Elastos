
#include "BluetoothHeadset.h"
#include "CBluetoothHeadsetStateChangeCallback.h"
#include "CBluetoothAdapter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::R;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//====================================================
// BluetoothHeadset::ServiceConnection
//====================================================

BluetoothHeadset::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothHeadset* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(BluetoothHeadset::ServiceConnection, IServiceConnection)

ECode BluetoothHeadset::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mHost->mService = IIBluetoothHeadset::Probe(service);

    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceConnected(IBluetoothProfile::HEADSET, (IBluetoothProfile*)mHost);
    }
    return NOERROR;
}

ECode BluetoothHeadset::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DBG) Logger::D(TAG, "Proxy object disconnected");
    mHost->mService = NULL;
    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceDisconnected(IBluetoothProfile::HEADSET);
    }
    return NOERROR;
}


//====================================================
// BluetoothHeadset
//====================================================

const String BluetoothHeadset::TAG("BluetoothHeadset");
const Boolean BluetoothHeadset::DBG = TRUE;
const Boolean BluetoothHeadset::VDBG = FALSE;

BluetoothHeadset::BluetoothHeadset(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* listener)
    : mContext(context)
    , mServiceListener(listener)
{
    CBluetoothHeadsetStateChangeCallback::New(this, (IIBluetoothStateChangeCallback**)&mBluetoothStateChangeCallback);
    mConnection = new ServiceConnection(this);

    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    if (mAdapter != NULL) {
        AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
        if (mgr != NULL) {
            // try {
            ECode ec = mgr->RegisterStateChangeCallback(mBluetoothStateChangeCallback);
            if (FAILED(ec)) {
                Logger::E(TAG, "0x%08x", ec);
            }
            // } catch (RemoteException e) {
            //     Log.e(TAG,"",e);
            // }
        }
    }

    AutoPtr<IIntent> intent;
    CIntent::New(String("IBluetoothHeadset")/*IBluetoothHeadset.class.getName()*/, (IIntent**)&intent);
    Boolean result;
    if (context->BindService(intent, mConnection, 0, &result), !result) {
        Logger::E(TAG, "Could not bind to Bluetooth Headset Service");
    }
}

PInterface BluetoothHeadset::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IBluetoothHeadset*)this;
    }
    else if (riid == EIID_IBluetoothHeadset) {
        return (IBluetoothHeadset*)this;
    }
    else if (riid == EIID_IBluetoothProfile) {
        return (IBluetoothProfile*)this;
    }
    return NULL;
}

UInt32 BluetoothHeadset::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BluetoothHeadset::Release()
{
    return ElRefBase::Release();
}

ECode BluetoothHeadset::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID)

    if (pObject == (IInterface*)(IBluetoothHeadset*)this) {
        *pIID = EIID_IBluetoothHeadset;
        return NOERROR;
    }
    else if (pObject == (IInterface*)(IBluetoothProfile*)this) {
        *pIID = EIID_IBluetoothProfile;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode BluetoothHeadset::Close()
{
    if (VDBG) Logger::D(TAG, "close()");

    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mAdapter != NULL) {
        if (mgr != NULL) {
    //        try {
            ECode ec = mgr->UnregisterStateChangeCallback(mBluetoothStateChangeCallback);
            if (FAILED(ec)) {
                Logger::E(TAG, "0x%08x", ec);
            }
    //        } catch (Exception e) {
    //            Log.e(TAG,"",e);
    //        }
        }
    }

    AutoLock lock(mConnectionLock);
    if (mService != NULL) {
//            try {
        mService = NULL;
        ECode ec = mContext->UnbindService(mConnection);
        if (FAILED(ec)) {
            Logger::E(TAG, "0x%08x", ec);
        }
//            } catch (Exception re) {
//                Log.e(TAG,"",re);
//            }
    }
    mServiceListener = NULL;
    return NOERROR;
}

ECode BluetoothHeadset::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "connect(%s)", str.string());
    }
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->Connect(device, result);
//        } catch (RemoteException e) {
//            Log.e(TAG, Log.getStackTraceString(new Throwable()));
//            return false;
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "disconnect(%s)", str.string());
    }
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->Disconnect(device, result);
//        } catch (RemoteException e) {
//          Log.e(TAG, Log.getStackTraceString(new Throwable()));
//          return false;
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::GetConnectedDevices(
    /* [out, callee] */ ArrayOf<IBluetoothDevice*>** devices)
{
    VALIDATE_NOT_NULL(devices)
    if (VDBG) Logger::D(TAG, "getConnectedDevices()");
    if (mService != NULL && IsEnabled()) {
        // try {
        return mService->GetConnectedDevices(devices);
        // } catch (RemoteException e) {
        //     Log.e(TAG, Log.getStackTraceString(new Throwable()));
        //     return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *devices = ArrayOf<IBluetoothDevice*>::Alloc(0);
    REFCOUNT_ADD(*devices);
    return NOERROR;
}

ECode BluetoothHeadset::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out, callee] */ ArrayOf<IBluetoothDevice*>** devices)
{
    VALIDATE_NOT_NULL(devices)
    if (VDBG) Logger::D(TAG, "getDevicesMatchingStates()");
    if (mService != NULL && IsEnabled()) {
        // try {
        return mService->GetDevicesMatchingConnectionStates(states, devices);
        // } catch (RemoteException e) {
        //     Log.e(TAG, Log.getStackTraceString(new Throwable()));
        //     return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *devices = ArrayOf<IBluetoothDevice*>::Alloc(0);
    REFCOUNT_ADD(*devices);
    return NOERROR;
}

ECode BluetoothHeadset::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    if (VDBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "getConnectionState(%s)", str.string());
    }
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->GetConnectionState(device, state);
//        } catch (RemoteException e) {
//            Log.e(TAG, Log.getStackTraceString(new Throwable()));
//            return BluetoothProfile.STATE_DISCONNECTED;
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *state = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothHeadset::SetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "setPriority(%s, %d)", str.string(), priority);
    }
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
        if (priority != IBluetoothProfile::PRIORITY_OFF &&
            priority != IBluetoothProfile::PRIORITY_ON) {
            *result = FALSE;
            return NOERROR;
        }

//        try {
        return mService->SetPriority(device, priority, result);
//        } catch (RemoteException e) {
//            Log.e(TAG, Log.getStackTraceString(new Throwable()));
//            return false;
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::GetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* priority)
{
    VALIDATE_NOT_NULL(priority)
    if (VDBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "getPriority(%s)", str.string());
    }
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->GetPriority(device, priority);
//        } catch (RemoteException e) {
//            Log.e(TAG, Log.getStackTraceString(new Throwable()));
//            return PRIORITY_OFF;
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *priority = IBluetoothProfile::PRIORITY_OFF;
    return NOERROR;
}

ECode BluetoothHeadset::StartVoiceRecognition(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "startVoiceRecognition()");

    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->StartVoiceRecognition(device, result);
//        } catch (RemoteException e) {
//            Log.e(TAG,  Log.getStackTraceString(new Throwable()));
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::StopVoiceRecognition(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "stopVoiceRecognition()");
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->StopVoiceRecognition(device, result);
//        } catch (RemoteException e) {
//            Log.e(TAG,  Log.getStackTraceString(new Throwable()));
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::IsAudioConnected(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* isConnected)
{
    VALIDATE_NOT_NULL(isConnected)
    if (VDBG) Logger::D(TAG, "isAudioConnected()");
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->IsAudioConnected(device, isConnected);
//        } catch (RemoteException e) {
//          Log.e(TAG,  Log.getStackTraceString(new Throwable()));
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *isConnected = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::GetBatteryUsageHint(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* batteryHint)
{
    VALIDATE_NOT_NULL(batteryHint)
    if (VDBG) Logger::D(TAG, "getBatteryUsageHint()");

    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->GetBatteryUsageHint(device, batteryHint);
//        } catch (RemoteException e) {
//            Log.e(TAG,  Log.getStackTraceString(new Throwable()));
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *batteryHint = -1;
    return NOERROR;
}

Boolean BluetoothHeadset::IsBluetoothVoiceDialingEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean result;
    res->GetBoolean(R::bool_::config_bluetooth_sco_off_call, &result);
    return result;
}

ECode BluetoothHeadset::AcceptIncomingConnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "acceptIncomingConnect");
    if (mService != NULL && IsEnabled()) {
//        try {
        return mService->AcceptIncomingConnect(device, result);
//        } catch (RemoteException e) {Log.e(TAG, e.toString());}
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::RejectIncomingConnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "rejectIncomingConnect");
    if (mService != NULL) {
//        try {
        return mService->RejectIncomingConnect(device, result);
//        } catch (RemoteException e) {Log.e(TAG, e.toString());}
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::GetAudioState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    if (VDBG) Logger::D(TAG, "getAudioState");
    if (mService != NULL && !IsDisabled()) {
//        try {
        return mService->GetAudioState(device, state);
//        } catch (RemoteException e) {Log.e(TAG, e.toString());}
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *state = IBluetoothHeadset::STATE_AUDIO_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothHeadset::IsAudioOn(
    /* [out] */ Boolean* isAudioOn)
{
    VALIDATE_NOT_NULL(isAudioOn)
    if (VDBG) Logger::D(TAG, "isAudioOn()");
    if (mService != NULL && IsEnabled()) {
//        try {
        return mService->IsAudioOn(isAudioOn);
//        } catch (RemoteException e) {
//          Log.e(TAG,  Log.getStackTraceString(new Throwable()));
//        }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *isAudioOn = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::ConnectAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mService != NULL && IsEnabled()) {
//        try {
        return mService->ConnectAudio(result);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::DisconnectAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mService != NULL && IsEnabled()) {
//        try {
        return mService->DisconnectAudio(result);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::StartScoUsingVirtualVoiceCall(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "startScoUsingVirtualVoiceCall()");
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->StartScoUsingVirtualVoiceCall(device, result);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::StopScoUsingVirtualVoiceCall(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "stopScoUsingVirtualVoiceCall()");
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->StopScoUsingVirtualVoiceCall(device, result);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHeadset::PhoneStateChanged(
    /* [in] */ Int32 numActive,
    /* [in] */ Int32 numHeld,
    /* [in] */ Int32 callState,
    /* [in] */ const String& number,
    /* [in] */ Int32 type)
{
    if (mService != NULL && IsEnabled()) {
//        try {
        return mService->PhoneStateChanged(numActive, numHeld, callState, number, type);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    return NOERROR;
}

ECode BluetoothHeadset::RoamChanged(
    /* [in] */ Boolean roaming)
{
    if (mService != NULL && IsEnabled()) {
//        try {
        return mService->RoamChanged(roaming);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    return NOERROR;
}

ECode BluetoothHeadset::ClccResponse(
    /* [in] */ Int32 index,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 status,
    /* [in] */ Int32 mode,
    /* [in] */ Boolean mpty,
    /* [in] */ const String& number,
    /* [in] */ Int32 type)
{
    if (mService != NULL && IsEnabled()) {
//        try {
        return mService->ClccResponse(index, direction, status, mode, mpty, number, type);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    return NOERROR;
}

Boolean BluetoothHeadset::IsEnabled()
{
    if(mAdapter != NULL) {
        Int32 state = IBluetoothAdapter::STATE_OFF;

        mAdapter->GetState(&state);

        if (state == IBluetoothAdapter::STATE_ON) {
           return TRUE;
        }
    }

    return FALSE;
}

Boolean BluetoothHeadset::IsDisabled()
{
    if (mAdapter != NULL) {
        Int32 state;
        if (mAdapter->GetState(&state), state == IBluetoothAdapter::STATE_OFF) return TRUE;
    }

   return FALSE;
}

Boolean BluetoothHeadset::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) return FALSE;

    String address;
    device->GetAddress(&address);
    if (CBluetoothAdapter::CheckBluetoothAddress(address)) return TRUE;
}

} // Bluetooth
} // Droid
} // Elastos

