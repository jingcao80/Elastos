#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/bluetooth/BluetoothPan.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CBluetoothPanStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/Process.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//====================================================
// BluetoothPan::ServiceConnection
//====================================================
CAR_INTERFACE_IMPL(BluetoothPan::ServiceConnection, Object, IServiceConnection);

BluetoothPan::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothPan* host)
    : mHost(host)
{}

ECode BluetoothPan::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "BluetoothPAN Proxy object connected");
    mHost->mPanService = IIBluetoothPan::Probe(service);

    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceConnected(IBluetoothProfile::PAN, mHost);
    }
    return NOERROR;
}

ECode BluetoothPan::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DBG) Logger::D(TAG, "BluetoothPAN Proxy object disconnected");
    mHost->mPanService = NULL;
    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceDisconnected(IBluetoothProfile::PAN);
    }
    return NOERROR;
}


//====================================================
// BluetoothPan
//====================================================

const String BluetoothPan::TAG("BluetoothPan");
const Boolean BluetoothPan::DBG = TRUE;
const Boolean BluetoothPan::VDBG = FALSE;
//const String BluetoothPan::ACTION_CONNECTION_STATE_CHANGED("android.bluetooth.pan.profile.action.CONNECTION_STATE_CHANGED");
//const String BluetoothPan::EXTRA_LOCAL_ROLE("android.bluetooth.pan.extra.LOCAL_ROLE");
//const Int32 BluetoothPan::PAN_ROLE_NONE;
//const Int32 BluetoothPan::LOCAL_NAP_ROLE;
//const Int32 BluetoothPan::REMOTE_NAP_ROLE;
//const Int32 BluetoothPan::LOCAL_PANU_ROLE;
//const Int32 BluetoothPan::REMOTE_PANU_ROLE;
//const Int32 BluetoothPan::PAN_DISCONNECT_FAILED_NOT_CONNECTED;
//const Int32 BluetoothPan::PAN_CONNECT_FAILED_ALREADY_CONNECTED;
//const Int32 BluetoothPan::PAN_CONNECT_FAILED_ATTEMPT_FAILED;
//const Int32 BluetoothPan::PAN_OPERATION_GENERIC_FAILURE;
//const Int32 BluetoothPan::PAN_OPERATION_SUCCESS;
CAR_INTERFACE_IMPL_2(BluetoothPan, Object, IBluetoothPan, IBluetoothProfile);

BluetoothPan::BluetoothPan()
{
}

BluetoothPan::BluetoothPan(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* listener)
    : mContext(context)
    , mServiceListener(listener)
{
    CBluetoothPanStateChangeCallback::New(TO_IINTERFACE(this), (IIBluetoothStateChangeCallback**)&mStateChangeCallback);
    mConnection = new ServiceConnection(this);

    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    //if(mAdapter != NULL){
        AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
        if (mgr != NULL) {
//            try {
            ECode ec = mgr->RegisterStateChangeCallback(mStateChangeCallback);
            if (FAILED(ec)) {
                Logger::W(TAG, "Unable to register BluetoothStateChangeCallback, 0x%08x", ec);
            }
//            } catch (RemoteException re) {
//                Log.w(TAG,"Unable to register BluetoothStateChangeCallback",re);
//            }
        }

        Logger::D(TAG, "BluetoothPan() call bindService");
        //AutoPtr<IIntent> intent;
        //CIntent::New(String("IBluetoothPan")/*IBluetoothPan.class.getName()*/, (IIntent**)&intent);
        //Boolean result;
        //if (context->BindService(intent, mConnection, 0, &result), !result) {
        //    Logger::E(TAG, "Could not bind to Bluetooth HID Service");
        //}
        Boolean bind;
        DoBind(&bind);
        Logger::D(TAG, "BluetoothPan(), bindService called");
    //}
}

ECode BluetoothPan::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //Intent intent = new Intent(IBluetoothPan.class.getName());
    AutoPtr<IIntent> intent;
    CIntent::New(String("IBluetoothPan")/*IBluetoothPan.class.getName()*/, (IIntent**)&intent);
    //ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
    //intent.setComponent(comp);
    AutoPtr<IComponentName> comp;
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    intent->SetComponent(comp);
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);

    Boolean succeeded = FALSE;
    if (comp == NULL || !(mContext->BindServiceAsUser(intent, mConnection, 0,
                userHandle, &succeeded), userHandle)) {
        Logger::E(TAG, "Could not bind to Bluetooth Pan Service with ");// + intent);
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothPan::Close()
{
    if (VDBG) {
        Logger::D(TAG, "close()");
    }

    AutoPtr<IIBluetoothManager> mgr = ((CBluetoothAdapter*)mAdapter.Get())->GetBluetoothManager();
    if (mgr != NULL) {
        //try {
        mgr->UnregisterStateChangeCallback(mStateChangeCallback);
        //} catch (RemoteException re) {
        //    Log.w(TAG,"Unable to unregister BluetoothStateChangeCallback",re);
        //}
    }

    {
        AutoLock lock(mConnection);
        if (mPanService != NULL) {
            //try {
            mPanService = NULL;
            mContext->UnbindService(mConnection);
            //} catch (Exception re) {
            //    Log.e(TAG,"",re);
            //}
        }
    }
    mServiceListener = NULL;

    return NOERROR;
}

BluetoothPan::~BluetoothPan()
{
    Close();
}

ECode BluetoothPan::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (DBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "connect(%s)", str.string());
    }

    if (mPanService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mPanService->Connect(device, result);
//        } catch (RemoteException e) {
//            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
//            return false;
//        }
    }
    if (mPanService == NULL) {
        Logger::W(TAG, "Proxy not attached to service");
    }
    return NOERROR;
}

ECode BluetoothPan::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (DBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "disconnect(%s)", str.string());
    }
    if (mPanService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mPanService->Disconnect(device, result);
//        } catch (RemoteException e) {
//            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
//            return false;
//        }
    }
    if (mPanService == NULL) {
        Logger::W(TAG, "Proxy not attached to service");
    }
    return NOERROR;
}

ECode BluetoothPan::GetConnectedDevices(
    /* [out, callee] */ IList** devices)
{
    VALIDATE_NOT_NULL(devices)
    *devices = NULL;

    if (VDBG) Logger::D(TAG, "getConnectedDevices()");
    if (mPanService != NULL && IsEnabled()) {
        // try {
        return mPanService->GetConnectedDevices(devices);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //     return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mPanService == NULL) Logger::W(TAG, "Proxy not attached to service");
    AutoPtr<IList> l;
    CArrayList::New((IList**)&l);
    *devices = l;
    REFCOUNT_ADD(*devices)
    return NOERROR;
}

ECode BluetoothPan::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    VALIDATE_NOT_NULL(devices)
    *devices = NULL;

    if (VDBG) Logger::D(TAG, "getDevicesMatchingStates()");
    if (mPanService != NULL && IsEnabled()) {
        // try {
        return mPanService->GetDevicesMatchingConnectionStates(states, devices);
        // } catch (RemoteException e) {
        //     Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        //     return new ArrayList<BluetoothDevice>();
        // }
    }
    if (mPanService == NULL) Logger::W(TAG, "Proxy not attached to service");
    AutoPtr<IList> l;
    CArrayList::New((IList**)&l);
    *devices = l;
    REFCOUNT_ADD(*devices)
    return NOERROR;
}

ECode BluetoothPan::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = IBluetoothProfile::STATE_DISCONNECTED;

    if (VDBG) {
        String str;
        IObject::Probe(device)->ToString(&str);
        Logger::D(TAG, "getState(%s)", str.string());
    }

    if (mPanService != NULL && IsEnabled() && IsValidDevice(device)) {
//        try {
        return mPanService->GetConnectionState(device, state);
//        } catch (RemoteException e) {
//            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
//            return BluetoothProfile.STATE_DISCONNECTED;
//        }
    }
    if (mPanService == NULL) {
        Logger::W(TAG, "Proxy not attached to service");
    }
    return NOERROR;
}

ECode BluetoothPan::SetBluetoothTethering(
    /* [in] */ Boolean value)
{
    if (DBG) {
        Logger::D(TAG, "setBluetoothTethering(%d)", value);
    }
//    try {
    return mPanService->SetBluetoothTethering(value);
//    } catch (RemoteException e) {
//        Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
//    }
}

ECode BluetoothPan::IsTetheringOn(
    /* [out] */ Boolean* isTetheringOn)
{
    VALIDATE_NOT_NULL(isTetheringOn)
    *isTetheringOn = FALSE;

    if (VDBG) Logger::D(TAG, "isTetheringOn()");
//    try {
    return mPanService->IsTetheringOn(isTetheringOn);
//    } catch (RemoteException e) {
//        Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
//    }
}

Boolean BluetoothPan::IsEnabled()
{
    if (mAdapter != NULL){
        Int32 state;
        if (mAdapter->GetState(&state), state == IBluetoothAdapter::STATE_ON) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean BluetoothPan::IsValidDevice(
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

} // Bluetooth
} // Droid
} // Elastos
