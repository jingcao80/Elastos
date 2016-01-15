
#include "BluetoothHealth.h"
#include "CBluetoothAdapter.h"
#include "CBluetoothHealthAppConfiguration.h"
#include "CBluetoothHealthStateChangeCallback.h"
#include "CBluetoothHealthCallbackWrapper.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//====================================================
// BluetoothHealth::ServiceConnection
//====================================================

BluetoothHealth::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothHealth* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(BluetoothHealth::ServiceConnection, IServiceConnection)

ECode BluetoothHealth::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mHost->mService = IIBluetoothHealth::Probe(service);

    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceConnected(IBluetoothProfile::HEALTH, (IBluetoothProfile*)mHost);
    }
    return NOERROR;
}

ECode BluetoothHealth::ServiceConnection::OnServiceDisconnected(
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
// BluetoothHealth
//====================================================

const String BluetoothHealth::TAG("BluetoothHealth");
const Boolean BluetoothHealth::DBG = TRUE;
const Boolean BluetoothHealth::VDBG = FALSE;

BluetoothHealth::BluetoothHealth(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* listener)
    : mContext(context)
    , mServiceListener(listener)
{
    CBluetoothHealthStateChangeCallback::New(this, (IIBluetoothStateChangeCallback**)&mBluetoothStateChangeCallback);
    mConnection = new ServiceConnection(this);

    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    if(mAdapter == NULL) {
        AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
        if (mgr != NULL) {
    //        try {
            ECode ec = mgr->RegisterStateChangeCallback(mBluetoothStateChangeCallback);
            if (FAILED(ec)) {
                Logger::E(TAG, "0x%08x", ec);
            }
    //        } catch (RemoteException e) {
    //            Log.e(TAG,"",e);
    //        }
        }
    }

    AutoPtr<IIntent> intent;
    CIntent::New(String("IBluetoothHealth")/*IBluetoothHealth.class.getName()*/, (IIntent**)&intent);
    Boolean result;
    if (context->BindService(intent, mConnection, 0, &result), !result) {
        Logger::E(TAG, "Could not bind to Bluetooth Headset Service");
    }
}

PInterface BluetoothHealth::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IBluetoothHealth*)this;
    }
    else if (riid == EIID_IBluetoothHealth) {
        return (IBluetoothHealth*)this;
    }
    else if (riid == EIID_IBluetoothProfile) {
        return (IBluetoothProfile*)this;
    }
    return NULL;
}

UInt32 BluetoothHealth::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BluetoothHealth::Release()
{
    return ElRefBase::Release();
}

ECode BluetoothHealth::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    VALIDATE_NOT_NULL(pIID)

    if (pObject == (IInterface*)(IBluetoothHealth*)this) {
        *pIID = EIID_IBluetoothHealth;
        return NOERROR;
    }
    else if (pObject == (IInterface*)(IBluetoothProfile*)this) {
        *pIID = EIID_IBluetoothProfile;
        return NOERROR;
    }

    return E_INVALID_ARGUMENT;
}

ECode BluetoothHealth::RegisterSinkAppConfiguration(
    /* [in] */ const String& name,
    /* [in] */ Int32 dataType,
    /* [in] */ IBluetoothHealthCallback* healthCallback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!IsEnabled() || name.IsNull()) return FALSE;

    if (VDBG) Logger::D(TAG, "registerSinkApplication(%s:%d)", name.string(), dataType);
    return RegisterAppConfiguration(name, dataType, SINK_ROLE,
            CHANNEL_TYPE_ANY, healthCallback, result);
}

ECode BluetoothHealth::RegisterAppConfiguration(
    /* [in] */ const String& name,
    /* [in] */ Int32 dataType,
    /* [in] */ Int32 role,
    /* [in] */ Int32 channelType,
    /* [in] */ IBluetoothHealthCallback* callback,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (!IsEnabled() || !CheckAppParam(name, role, channelType, callback)) return FALSE;

    if (VDBG) Logger::D(TAG, "RegisterAppConfiguration(%s:%d)", name.string(), dataType);
    AutoPtr<IIBluetoothHealthCallback> wrapper;
    CBluetoothHealthCallbackWrapper::New(callback, (IIBluetoothHealthCallback**)&wrapper);
    AutoPtr<IBluetoothHealthAppConfiguration> config;
    CBluetoothHealthAppConfiguration::New(name, dataType, role, channelType, (IBluetoothHealthAppConfiguration**)&config);

    if (mService != NULL) {
        // try {
        return mService->RegisterAppConfiguration(config, wrapper, result);
        // } catch (RemoteException e) {
        //     Log.e(TAG, e.toString());
        // }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
        // if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHealth::UnregisterAppConfiguration(
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mService != NULL && IsEnabled() && config != NULL) {
        // try {
        return mService->UnregisterAppConfiguration(config, result);
        // } catch (RemoteException e) {
        //     Log.e(TAG, e.toString());
        // }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
        // if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothHealth::ConnectChannelToSource(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mService != NULL && IsEnabled() && IsValidDevice(device) && config != NULL) {
//        try {
        return mService->ConnectChannelToSource(device, config, result);
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

ECode BluetoothHealth::ConnectChannelToSink(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ Int32 channelType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mService != NULL && IsEnabled() && IsValidDevice(device) && config != NULL) {
//        try {
        return mService->ConnectChannelToSink(device, config, channelType, result);
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

ECode BluetoothHealth::DisconnectChannel(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [in] */ Int32 channelId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mService != NULL && IsEnabled() && IsValidDevice(device) && config != NULL) {
//        try {
        return mService->DisconnectChannel(device, config, channelId, result);
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

ECode BluetoothHealth::GetMainChannelFd(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IBluetoothHealthAppConfiguration* config,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd)
    if (mService != NULL && IsEnabled() && IsValidDevice(device) && config != NULL) {
//        try {
        return mService->GetMainChannelFd(device, config, pfd);
//        } catch (RemoteException e) {
//            Log.e(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    }
    *pfd = NULL;
    return NOERROR;
}

ECode BluetoothHealth::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    if (mService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mService->GetHealthDeviceConnectionState(device, state);
//        } catch (RemoteException e) {
//            Logger::E(TAG, e.toString());
//        }
    }
    else {
        Logger::W(TAG, "Proxy not attached to service");
//        if (DBG) {
//            Logger::D(TAG, Log.getStackTraceString(new Throwable()));
//        }
    }
    *state = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothHealth::GetConnectedDevices(
    /* [out, callee] */ ArrayOf<IBluetoothDevice*>** devices)
{
    VALIDATE_NOT_NULL(devices)
    if (mService != NULL && IsEnabled()) {
        // try {
        return mService->GetConnectedHealthDevices(devices);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //     return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *devices = ArrayOf<IBluetoothDevice*>::Alloc(0);
    REFCOUNT_ADD(*devices)
    return NOERROR;
}

ECode BluetoothHealth::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out, callee] */ ArrayOf<IBluetoothDevice *>** devices)
{
    VALIDATE_NOT_NULL(devices)
    if (mService != NULL && IsEnabled()) {
        // try {
        return mService->GetHealthDevicesMatchingConnectionStates(states, devices);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //     return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *devices = ArrayOf<IBluetoothDevice*>::Alloc(0);
    REFCOUNT_ADD(*devices)
    return NOERROR;
}

ECode BluetoothHealth::Close()
{
    if (VDBG) Logger::D(TAG, "close()");
    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mAdapter != NULL) {
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
        // try {
        mService = NULL;
        ECode ec = mContext->UnbindService(mConnection);
        if (FAILED(ec)) {
            Logger::E(TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    mServiceListener = NULL;
    return NOERROR;
}

Boolean BluetoothHealth::IsEnabled()
{
    AutoPtr<IBluetoothAdapter> adapter = CBluetoothAdapter::GetDefaultAdapter();
    if (adapter != NULL){
        Int32 state;
        if (adapter->GetState(&state), state == IBluetoothAdapter::STATE_ON) {
            return TRUE;
        }
    }
    Logger::D(TAG, "Bluetooth is Not enabled");
    return FALSE;
}

Boolean BluetoothHealth::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) {
        return FALSE;
    }
    String address;
    device->GetAddress(&address);
    if (CBluetoothAdapter::CheckBluetoothAddress(address)) {
        return TRUE;
    }
    return FALSE;
}

Boolean BluetoothHealth::CheckAppParam(
    /* [in] */ const String& name,
    /* [in] */ Int32 role,
    /* [in] */ Int32 channelType,
    /* [in] */ IBluetoothHealthCallback* callback)
{
    if (name.IsNull() || (role != IBluetoothHealth::SOURCE_ROLE && role != IBluetoothHealth::SINK_ROLE) ||
            (channelType != IBluetoothHealth::CHANNEL_TYPE_RELIABLE &&
            channelType != IBluetoothHealth::CHANNEL_TYPE_STREAMING &&
            channelType != IBluetoothHealth::CHANNEL_TYPE_ANY) || callback == NULL) {
        return FALSE;
    }
    if (role == IBluetoothHealth::SOURCE_ROLE && channelType == IBluetoothHealth::CHANNEL_TYPE_ANY) {
        return FALSE;
    }
    return TRUE;
}

}
}
}

