#include "elastos/droid/bluetooth/BluetoothSap.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CBluetoothSapStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Process.h"
#include "Elastos.CoreLibrary.Utility.h"

#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

const String BluetoothSap::TAG("BluetoothSap");
const Boolean BluetoothSap::DBG;
const Boolean BluetoothSap::VDBG;

CAR_INTERFACE_IMPL(BluetoothSap::ServiceConnection, Object, IServiceConnection)
CAR_INTERFACE_IMPL_2(BluetoothSap, Object, IBluetoothSap, IBluetoothProfile)

BluetoothSap::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothSap* host)
{
    mHost = host;
}

ECode BluetoothSap::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "BluetoothSap Proxy object connected");
    mHost->mSapService = IIBluetoothSap::Probe(service);

    if (mHost->mServiceListener != NULL) {
        return mHost->mServiceListener->OnServiceConnected(IBluetoothProfile::SAP,
                                            mHost);
    }
    return NOERROR;
}

ECode BluetoothSap::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DBG) Logger::D(TAG, "BluetoothPAN Proxy object connected");
    mHost->mSapService = NULL;

    if (mHost->mServiceListener != NULL) {
        return mHost->mServiceListener->OnServiceDisconnected(IBluetoothProfile::SAP);
    }
    return NOERROR;
}

BluetoothSap::BluetoothSap(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    mContext = context;
    mServiceListener = l;
    CBluetoothSapStateChangeCallback::New((IBluetoothSap*)this, (IIBluetoothStateChangeCallback**)&mStateChangeCallback);
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    // try {
        CBluetoothAdapter* adapterImpl = (CBluetoothAdapter*)mAdapter.Get();
        if (FAILED(adapterImpl->GetBluetoothManager()->RegisterStateChangeCallback(mStateChangeCallback)))
        {
            Logger::E(TAG, "Unable to register BluetoothStateChangeCallback");
        }
    // } catch (RemoteException re) {
        // Log.w(TAG,"Unable to register BluetoothStateChangeCallback",re);
    // }
    Logger::D(TAG, "BluetoothSap() call bindService");
    Boolean res;
    DoBind(&res);
}
BluetoothSap::~BluetoothSap()
{
    Close();
}

ECode BluetoothSap::DoBind(
    /* [out] */ Boolean* bind)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.bluetooth.IBluetoothSap"), (IIntent**)&intent);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> comp;
    intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    intent->SetComponent(comp);
    AutoPtr<IUserHandle> handle;
    Process::MyUserHandle((IUserHandle**)&handle);
    Boolean bindResult;
    mContext->BindServiceAsUser(intent, mConnection, 0, handle, &bindResult);
    if (comp == NULL || !bindResult) {
        String str;
        intent->ToString(&str);
        Logger::E(TAG, "Could not bind to Bluetooth Dun Service with %s", str.string());
        *bind = FALSE;
        return NOERROR;
    }
    *bind = TRUE;
    return NOERROR;
}

ECode BluetoothSap::Close()
{
    if (VDBG) Logger::D(TAG, "close()");

    mServiceListener = NULL;
    AutoPtr<IIBluetoothManager> mgr =
        ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        // try {
        if (FAILED(mgr->UnregisterStateChangeCallback(mStateChangeCallback)))
        {
            Logger::W(TAG, "Unable to unregister BluetoothStateChangeCallback for BluetoothSap");
        }
        // } catch (RemoteException re) {
            // Log.w(TAG,"Unable to unregister BluetoothStateChangeCallback",re);
        // }
    }

    AutoLock lock(mConnectionLock);
    if (mSapService != NULL) {
        // try {
        mSapService = NULL;
        mContext->UnbindService(mConnection);
        // } catch (Exception re) {
            // Log.e(TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode BluetoothSap::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    if (device != NULL && DBG) {
        String deviceStr;
        IObject::Probe(device)->ToString(&deviceStr);
        Logger::D(TAG, String("disconnect(") + deviceStr + ")");
    }

    if (mSapService != NULL && IsEnabled() && IsValidDevice(device)) {
        // try {
        if (FAILED(mSapService->Disconnect(device, result)))
        {
            Logger::E(TAG, "BluetoothSap::Disconnect Exception");
            *result = FALSE;
            return NOERROR;
        }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            // return false;
        // }
    }
    if (mSapService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothSap::GetConnectedDevices(
    /* [out] */ IList** list)
{
    if (VDBG) Logger::D(TAG, "getConnectedDevices()");
    if (mSapService != NULL && IsEnabled()) {
        // try {
        if (FAILED(mSapService->GetConnectedDevices(list)))
        {
            Logger::E(TAG, "GetConnectedDevices Service GetConnectedDevices Error");
            CArrayList::New(list);
            return NOERROR;
        }
        // } catch (RemoteException e) {
        // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        // return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mSapService == NULL) Logger::W(TAG, "Proxy not attached to service");
    return CArrayList::New(list);
}

ECode BluetoothSap::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** list)
{
    if (VDBG) Logger::D(TAG, "GetDevicesMatchingConnectionStates()");
    if (mSapService != NULL && IsEnabled()) {
        // try {
        if (FAILED(mSapService->GetDevicesMatchingConnectionStates(states, list)))
        {
            Logger::E(TAG, "GetConnectedDevices Service GetDevicesMatchingConnectionStates Error");
            CArrayList::New(list);
            return NOERROR;
        }
        // } catch (RemoteException e) {
        // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        // return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mSapService == NULL) Logger::W(TAG, "Proxy not attached to service");
    return CArrayList::New(list);
}

ECode BluetoothSap::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    if (device != NULL && DBG) {
        String deviceStr;
        IObject::Probe(device)->ToString(&deviceStr);
        Logger::D(TAG, "disconnect(%s)", deviceStr.string());
    }

    if (mSapService != NULL && IsEnabled() &&
        IsValidDevice(device)) {
        // try {
        if (FAILED(mSapService->GetConnectionState(device, result)))
        {
            Logger::E(TAG, "BluetoothSap::GetConnectionState Exception");
            *result = IBluetoothProfile::STATE_DISCONNECTED;
            return NOERROR;
        }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            // return false;
        // }
    }
    if (mSapService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

Boolean BluetoothSap::IsEnabled()
{
    Int32 state;
    mAdapter->GetState(&state);
   if (state == IBluetoothAdapter::STATE_ON) return TRUE;
   return FALSE;
}

Boolean BluetoothSap::IsValidDevice(
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