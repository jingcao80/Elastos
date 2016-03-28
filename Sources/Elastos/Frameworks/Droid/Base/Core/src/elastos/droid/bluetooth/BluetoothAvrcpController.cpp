#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/BluetoothAvrcpController.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Process.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Core::AutoLock;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//   BluetoothAvrcpController::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothAvrcpController::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothAvrcpController::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

ECode BluetoothAvrcpController::BluetoothStateChangeCallbackStub::constructor (
    /* [in] */ IBluetoothAvrcpController* owner)
{
    mOwner = (BluetoothAvrcpController*)owner;
    return NOERROR;
}

ECode BluetoothAvrcpController::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
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
                Boolean bind;
                mOwner->DoBind(&bind);
            }
            //} catch (Exception re) {
            //    Log.e(TAG,"",re);
            //}
        }
    }
    return NOERROR;
}

//=====================================================================
//           BluetoothAvrcpController::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothAvrcpController::InnerServiceConnection, Object, IServiceConnection);

BluetoothAvrcpController::InnerServiceConnection::InnerServiceConnection()
{
}

BluetoothAvrcpController::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ BluetoothAvrcpController* owner)
{
    mOwner = owner;
}

ECode BluetoothAvrcpController::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mOwner->mService = IIBluetoothAvrcpController::Probe(service);

    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceConnected(IBluetoothProfile::AVRCP_CONTROLLER,
                mOwner);
    }
    return NOERROR;
}

ECode BluetoothAvrcpController::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    if (DBG) Logger::D(TAG, "Proxy object disconnected");
    mOwner->mService = NULL;
    if (mOwner->mServiceListener != NULL) {
        mOwner->mServiceListener->OnServiceDisconnected(IBluetoothProfile::AVRCP_CONTROLLER);
    }
    return NOERROR;
}

//=====================================================================
//                       BluetoothAvrcpController
//=====================================================================
const String BluetoothAvrcpController::TAG("BluetoothAvrcpController");
const Boolean BluetoothAvrcpController::DBG = TRUE;
const Boolean BluetoothAvrcpController::VDBG = FALSE;

CAR_INTERFACE_IMPL_2(BluetoothAvrcpController, Object, IBluetoothAvrcpController, IBluetoothProfile);

BluetoothAvrcpController::BluetoothAvrcpController()
{
}

BluetoothAvrcpController::~BluetoothAvrcpController()
{
    Close();
}

BluetoothAvrcpController::BluetoothAvrcpController(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    mContext = context;
    mServiceListener = l;
    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    //IBluetoothManager mgr = mAdapter.getBluetoothManager();
    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        //try {
        mgr->RegisterStateChangeCallback(mBluetoothStateChangeCallback);
        //} catch (RemoteException e) {
        //    Log.e(TAG,"",e);
        //}
    }

    Boolean bind;
    DoBind(&bind);
}

ECode BluetoothAvrcpController::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIntent> intent;
    CIntent::New(String("IBluetoothAvrcpController")/*IBluetoothAvrcpController.class.getName()*/, (IIntent**)&intent);
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
        Logger::E(TAG, "Could not bind to Bluetooth AVRCP Controller Service with ");// + intent;
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothAvrcpController::Close()
{
    mServiceListener = NULL;
    //IBluetoothManager mgr = mAdapter->GetBluetoothManager();
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

ECode BluetoothAvrcpController::GetConnectedDevices(
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

ECode BluetoothAvrcpController::GetDevicesMatchingConnectionStates(
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

ECode BluetoothAvrcpController::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (VDBG) Logger::D(TAG, "getState( device )");
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

ECode BluetoothAvrcpController::SendPassThroughCmd(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 keyCode,
    /* [in] */ Int32 keyState)
{
    if (DBG) Logger::D(TAG, "sendPassThroughCmd");
    if (mService != NULL && IsEnabled()) {
        //try {
        mService->SendPassThroughCmd(device, keyCode, keyState);
        return NOERROR;
        //} catch (RemoteException e) {
        //    Log.e(TAG, "Error talking to BT service in sendPassThroughCmd()", e);
        //    return;
        //}
    }
    if (mService == NULL) Logger::W(TAG, "Proxy not attached to service");
    return NOERROR;
}

Boolean BluetoothAvrcpController::IsEnabled()
{
    Int32 state;
    if ((mAdapter->GetState(&state), state) == IBluetoothAdapter::STATE_ON) return TRUE;
    return FALSE;
}

Boolean BluetoothAvrcpController::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) return FALSE;

    String address;
    device->GetAddress(&address);
    if (CBluetoothAdapter::CheckBluetoothAddress(address)) return TRUE;
    return FALSE;
}

void BluetoothAvrcpController::Log(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, msg);
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
