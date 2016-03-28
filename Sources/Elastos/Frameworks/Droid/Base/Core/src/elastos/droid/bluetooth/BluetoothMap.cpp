#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/BluetoothMap.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Process.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//         BluetoothMap::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothMap::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothMap::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

ECode BluetoothMap::BluetoothStateChangeCallbackStub::constructor(
    /* [in] */ IBluetoothMap* owner)
{
    mOwner = (BluetoothMap*)owner;
    return NOERROR;
}

ECode BluetoothMap::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
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
//                 BluetoothMap::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothMap::InnerServiceConnection, Object, IServiceConnection);

BluetoothMap::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ BluetoothMap* owner)
    : mOwner(owner)
{
}

ECode BluetoothMap::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mOwner->mService = IIBluetoothMap::Probe(service);
    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceConnected(IBluetoothProfile::MAP, mOwner);
    }
    return NOERROR;
}

ECode BluetoothMap::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    if (DBG) Logger::D(TAG, "Proxy object disconnected");
    mOwner->mService = NULL;
    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceDisconnected(IBluetoothProfile::MAP);
    }
    return NOERROR;
}

//=====================================================================
//                             BluetoothMap
//=====================================================================
const String BluetoothMap::TAG("BluetoothMap");
const Boolean BluetoothMap::DBG = TRUE;
const Boolean BluetoothMap::VDBG = FALSE;

CAR_INTERFACE_IMPL_2(BluetoothMap, Object, IBluetoothMap, IBluetoothProfile);

BluetoothMap::BluetoothMap(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    if (DBG) Logger::D(TAG, "Create BluetoothMap proxy object");
    mContext = context;
    mServiceListener = l;
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        //try {
        mgr->RegisterStateChangeCallback(mBluetoothStateChangeCallback);
        //} catch (RemoteException e) {
        //    Logger::E(TAG,"",e);
        //}
    }
    Boolean bind;
    DoBind(&bind);
}

BluetoothMap::~BluetoothMap()
{
    Close();
}

ECode BluetoothMap::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    AutoPtr<IIntent> intent;
    CIntent::New(String("IBluetoothMap")/*IBluetoothMap.class.getName()*/, (IIntent**)&intent);
    AutoPtr<IComponentName> comp;
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    intent->SetComponent(comp);
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);

    //Intent intent = new Intent(IBluetoothMap.class.getName());
    //ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
    //intent.setComponent(comp);
    Boolean succeeded = FALSE;
    if (comp == NULL || !(mContext->BindServiceAsUser(intent, mConnection, 0,
            userHandle, &succeeded), succeeded)) {
        Logger::E(TAG, "Could not bind to Bluetooth MAP Service with ");// + intent);
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

// synchronized
ECode BluetoothMap::Close()
{
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

ECode BluetoothMap::GetState(
    /* [out] */ Int32* result)
{
    if (VDBG) Logger::D(TAG, "getState()");
    if (mService != NULL) {
        //try {
        return mService->GetState(result);
        //} catch (RemoteException e) {Logger::E(TAG, e.toString());}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) log(Log.getStackTraceString(new Throwable()));
    }
    *result = IBluetoothMap::STATE_ERROR;
    return NOERROR;
}

ECode BluetoothMap::GetClient(
    /* [out] */ IBluetoothDevice** result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getClient()");
    if (mService != NULL) {
        //try {
        return mService->GetClient(result);
        //} catch (RemoteException e) {Logger::E(TAG, e.toString());}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) log(Log.getStackTraceString(new Throwable()));
    }
    *result = NULL;
    return NOERROR;
}

ECode BluetoothMap::IsConnected(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "isConnected(");// + device + ")");
    if (mService != NULL) {
        //try {
        return mService->IsConnected(device, result);
        //} catch (RemoteException e) {Logger::E(TAG, e.toString());}
    } else {
        Logger::W(TAG, "Proxy not attached to service");
        //if (DBG) log(Log.getStackTraceString(new Throwable()));
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothMap::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "connect(");// + device + ")" + "not supported for MAPS");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothMap::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "disconnect(");// + device + ")");
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

Boolean BluetoothMap::DoesClassMatchSink(
    /* [in] */ IBluetoothClass* btClass)
{
    // // TODO optimize the rule
    Int32 deviceClass;
    btClass->GetDeviceClass(&deviceClass);
    switch (deviceClass) {
    case IBluetoothClassDevice::COMPUTER_DESKTOP:
    case IBluetoothClassDevice::COMPUTER_LAPTOP:
    case IBluetoothClassDevice::COMPUTER_SERVER:
    case IBluetoothClassDevice::COMPUTER_UNCATEGORIZED:
        return TRUE;
    default:
        return FALSE;
    }
    return FALSE;
}

ECode BluetoothMap::GetConnectedDevices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "getConnectedDevices()");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetConnectedDevices(result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return new ArrayList<BluetoothDevice>();
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");

    AutoPtr<IList> l;
    CArrayList::New((IList**)&l);
    *result = l;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothMap::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "getDevicesMatchingStates()");
    if (mService != NULL && IsEnabled()) {
        //try {
        return mService->GetDevicesMatchingConnectionStates(states, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return new ArrayList<BluetoothDevice>();
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");

    AutoPtr<IList> l;
    CArrayList::New((IList**)&l);
    *result = l;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode BluetoothMap::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "getConnectionState(");// + device + ")");
    if (mService != NULL && IsEnabled() &&
        IsValidDevice(device)) {
        //try {
        return mService->GetConnectionState(device, result);
        //} catch (RemoteException e) {
        //    Logger::E(TAG, Log.getStackTraceString(new Throwable()));
        //    return BluetoothProfile.STATE_DISCONNECTED;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

ECode BluetoothMap::SetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DBG) Logger::D(TAG, "setPriority(");// + device + ", " + priority + ")");
    if (mService != NULL && IsEnabled() &&
        IsValidDevice(device)) {
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

ECode BluetoothMap::GetPriority(
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

void BluetoothMap::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg.string());
}

Boolean BluetoothMap::IsEnabled()
{
    AutoPtr<IBluetoothAdapter> adapter = CBluetoothAdapter::GetDefaultAdapter();
    Int32 state;
    if (adapter != NULL && adapter->GetState(&state) == IBluetoothAdapter::STATE_ON) return TRUE;
    Logger::D(TAG, "Bluetooth is Not enabled");
    return FALSE;
}

Boolean BluetoothMap::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) return FALSE;

    String address;
    device->GetAddress(&address);
    if (CBluetoothAdapter::CheckBluetoothAddress(address)) return TRUE;
    return FALSE;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
