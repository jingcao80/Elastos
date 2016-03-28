#include "elastos/droid/bluetooth/BluetoothDun.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CBluetoothDunStateChangeCallback.h"
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

const String BluetoothDun::TAG("BluetoothDun");
const Boolean BluetoothDun::DBG;
const Boolean BluetoothDun::VDBG;

CAR_INTERFACE_IMPL(BluetoothDun::ServiceConnection, Object, IServiceConnection)
CAR_INTERFACE_IMPL_2(BluetoothDun, Object, IBluetoothDun, IBluetoothProfile)

BluetoothDun::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothDun* host)
{
    mHost = host;
}

ECode BluetoothDun::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "BluetoothDUN Proxy object connected");
    mHost->mDunService = IIBluetoothDun::Probe(service);

    if (mHost->mServiceListener != NULL) {
        return mHost->mServiceListener->OnServiceConnected(IBluetoothProfile::DUN,
                                            mHost);
    }
    return NOERROR;
}

ECode BluetoothDun::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DBG) Logger::D(TAG, "BluetoothPAN Proxy object connected");
    mHost->mDunService = NULL;

    if (mHost->mServiceListener != NULL) {
        return mHost->mServiceListener->OnServiceDisconnected(IBluetoothProfile::DUN);
    }
    return NOERROR;
}

BluetoothDun::BluetoothDun(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    mContext = context;
    mServiceListener = l;
    CBluetoothDunStateChangeCallback::New((IBluetoothDun*)this, (IIBluetoothStateChangeCallback**)&mStateChangeCallback);
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    // try {
        CBluetoothAdapter* adapterImpl = (CBluetoothAdapter*)mAdapter.Get();

        adapterImpl->GetBluetoothManager()->RegisterStateChangeCallback(mStateChangeCallback);
    // } catch (RemoteException re) {
        // Log.w(TAG,"Unable to register BluetoothStateChangeCallback",re);
    // }
    Logger::D(TAG, "BluetoothDun() call bindService");
    Boolean res;
    DoBind(&res);
}

ECode BluetoothDun::DoBind(
    /* [out] */ Boolean* bind)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.bluetooth.IBluetoothDun"), (IIntent**)&intent);
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

ECode BluetoothDun::Close()
{
    if (VDBG) Logger::D(TAG, "close()");

    mServiceListener = NULL;
    AutoPtr<IIBluetoothManager> mgr =
        ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        // try {
        if (FAILED(mgr->UnregisterStateChangeCallback(mStateChangeCallback)))
        {
            Logger::W(TAG, "Unable to unregister BluetoothStateChangeCallback for BluetoothDun");
        }
        // } catch (RemoteException re) {
            // Log.w(TAG,"Unable to unregister BluetoothStateChangeCallback",re);
        // }
    }

    AutoLock lock(mConnectionLock);
    if (mDunService != NULL) {
        // try {
        mDunService = NULL;
        mContext->UnbindService(mConnection);
        // } catch (Exception re) {
            // Log.e(TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode BluetoothDun::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    if (device != NULL && DBG) {
        String deviceStr;
        IObject::Probe(device)->ToString(&deviceStr);
        Logger::D(TAG, String("disconnect(") + deviceStr + ")");
    }

    if (mDunService != NULL && IsEnabled() && IsValidDevice(device)) {
        // try {
        if (FAILED(mDunService->Disconnect(device, result)))
        {
            Logger::E(TAG, "BluetoothDun::Disconnect Exception");
            *result = FALSE;
            return NOERROR;
        }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            // return false;
        // }
    }
    if (mDunService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothDun::GetConnectedDevices(
    /* [out] */ IList** list)
{
    if (VDBG) Logger::D(TAG, "getConnectedDevices()");
    if (mDunService != NULL && IsEnabled()) {
        // try {
        if (FAILED(mDunService->GetConnectedDevices(list)))
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
    if (mDunService == NULL) Logger::W(TAG, "Proxy not attached to service");
    return CArrayList::New(list);
}

ECode BluetoothDun::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** list)
{
    if (VDBG) Logger::D(TAG, "GetDevicesMatchingConnectionStates()");
    if (mDunService != NULL && IsEnabled()) {
        // try {
        if (FAILED(mDunService->GetDevicesMatchingConnectionStates(states, list)))
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
    if (mDunService == NULL) Logger::W(TAG, "Proxy not attached to service");
    return CArrayList::New(list);
}

ECode BluetoothDun::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    if (device != NULL && DBG) {
        String deviceStr;
        IObject::Probe(device)->ToString(&deviceStr);
        Logger::D(TAG, "disconnect(%s)", deviceStr.string());
    }

    if (mDunService != NULL && IsEnabled() &&
        IsValidDevice(device)) {
        // try {
        if (FAILED(mDunService->GetConnectionState(device, result)))
        {
            Logger::E(TAG, "BluetoothDun::GetConnectionState Exception");
            *result = IBluetoothProfile::STATE_DISCONNECTED;
            return NOERROR;
        }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            // return false;
        // }
    }
    if (mDunService == NULL) Logger::W(TAG, "Proxy not attached to service");
    *result = IBluetoothProfile::STATE_DISCONNECTED;
    return NOERROR;
}

Boolean BluetoothDun::IsEnabled()
{
    Int32 state;
    mAdapter->GetState(&state);
   if (state == IBluetoothAdapter::STATE_ON) return TRUE;
   return FALSE;
}

Boolean BluetoothDun::IsValidDevice(
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