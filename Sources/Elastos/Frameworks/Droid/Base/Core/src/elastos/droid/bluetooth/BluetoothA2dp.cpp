
#include "BluetoothA2dp.h"
#include "CBluetoothA2dpStateChangeCallback.h"
#include "CBluetoothAdapter.h"
#include "BluetoothUuid.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IParcelUuid;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//====================================================
// BluetoothA2dp::ServiceConnection
//====================================================

BluetoothA2dp::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothA2dp* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(BluetoothA2dp::ServiceConnection, IServiceConnection)

ECode BluetoothA2dp::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mHost->mService = IIBluetoothA2dp::Probe(service);

    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceConnected(IBluetoothProfile::A2DP, (IBluetoothProfile*)mHost);
    }
    return NOERROR;
}

ECode BluetoothA2dp::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DBG) Logger::D(TAG, "Proxy object disconnected");
    mHost->mService = NULL;
    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceDisconnected(IBluetoothProfile::A2DP);
    }
    return NOERROR;
}


//====================================================
// BluetoothA2dp
//====================================================

const String BluetoothA2dp::TAG("BluetoothA2dp");
const Boolean BluetoothA2dp::DBG = TRUE;
const Boolean BluetoothA2dp::VDBG = FALSE;

BluetoothA2dp::BluetoothA2dp(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* listener)
    : mContext(context)
    , mServiceListener(listener)
{
    CBluetoothA2dpStateChangeCallback::New(this, (IIBluetoothStateChangeCallback**)&mBluetoothStateChangeCallback);
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
    CIntent::New(String("IBluetoothA2dp")/*IBluetoothA2dp.class.getName()*/, (IIntent**)&intent);
    Boolean result;
    if (context->BindService(intent, mConnection, 0, &result), !result) {
        Logger::E(TAG, "Could not bind to Bluetooth A2DP Service");
    }
}

PInterface BluetoothA2dp::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IBluetoothA2dp*)this;
    }
    else if (riid == EIID_IBluetoothA2dp) {
        return (IBluetoothA2dp*)this;
    }
    else if (riid == EIID_IBluetoothProfile) {
        return (IBluetoothProfile*)this;
    }
    return NULL;
}

BluetoothA2dp::~BluetoothA2dp()
{
    Close();
}

UInt32 BluetoothA2dp::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BluetoothA2dp::Release()
{
    return ElRefBase::Release();
}

ECode BluetoothA2dp::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID)

    if (pObject == (IInterface*)(IBluetoothA2dp*)this) {
        *pIID = EIID_IBluetoothA2dp;
        return NOERROR;
    }
    else if (pObject == (IInterface*)(IBluetoothProfile*)this) {
        *pIID = EIID_IBluetoothProfile;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode BluetoothA2dp::Close()
{
    mServiceListener = NULL;

    if (mAdapter != NULL) {
        AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
        if (mgr != NULL) {
            // try {
            ECode ec = mgr->UnregisterStateChangeCallback(mBluetoothStateChangeCallback);
            if (FAILED(ec)) {
                Logger::E(TAG, "0x%08x", ec);
            }
            // } catch (Exception e) {
            //     Log.e(TAG,"",e);
            // }
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
    return NOERROR;
}

ECode BluetoothA2dp::Connect(
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

ECode BluetoothA2dp::Disconnect(
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

ECode BluetoothA2dp::GetConnectedDevices(
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

ECode BluetoothA2dp::GetDevicesMatchingConnectionStates(
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

ECode BluetoothA2dp::GetConnectionState(
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

ECode BluetoothA2dp::SetPriority(
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

ECode BluetoothA2dp::GetPriority(
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

ECode BluetoothA2dp::IsA2dpPlaying(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
        // try {
        return mService->IsA2dpPlaying(device, result);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //     return false;
        // }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothA2dp::ShouldSendVolumeKeys(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IsEnabled() && IsValidDevice(device)) {
        AutoPtr< ArrayOf<IParcelUuid*> > uuids;
        device->GetUuids((ArrayOf<IParcelUuid*>**)&uuids);
        if (uuids == NULL) {
            *result = FALSE;
            return NOERROR;
        }

        for (Int32 i = 0; i < uuids->GetLength(); ++i) {
            AutoPtr<IParcelUuid> uuid = (*uuids)[i];
            if (BluetoothUuid::IsAvrcpTarget(uuid)) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

String BluetoothA2dp::StateToString(
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
        {
            StringBuilder sb("<unknown state ");
            sb += state;
            sb += ">";
            return sb.ToString();
        }
    }
}

Boolean BluetoothA2dp::IsEnabled()
{
    if(mAdapter != NULL) {
        Int32 state;
        if (mAdapter->GetState(&state), state == IBluetoothAdapter::STATE_ON) {
           return TRUE;
        }
    }
    return FALSE;
}

Boolean BluetoothA2dp::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) return FALSE;
    String address;
    device->GetAddress(&address);
    if (CBluetoothAdapter::CheckBluetoothAddress(address)) return TRUE;
    return FALSE;
}

} // Bluetooth
} // Droid
} // Elastos

