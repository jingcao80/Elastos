
#include "elastos/droid/bluetooth/BluetoothPbap.h"
#include "elastos/droid/bluetooth/CBluetoothAdapter.h"
#include "elastos/droid/bluetooth/CBluetoothPbapStateChangeCallback.h"
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
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//====================================================
// BluetoothPbap::ServiceConnection
//====================================================
CAR_INTERFACE_IMPL(BluetoothPbap::ServiceConnection, Object, IServiceConnection)

BluetoothPbap::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothPbap* host)
    : mHost(host)
{}

ECode BluetoothPbap::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DBG) Logger::D(TAG, "Proxy object connected");
    mHost->mService = IIBluetoothPbap::Probe(service);
    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceConnected(mHost);
    }
    return NOERROR;
}

ECode BluetoothPbap::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DBG) Logger::D(TAG, "Proxy object disconnected");
    mHost->mService = NULL;
    if (mHost->mServiceListener != NULL) {
        mHost->mServiceListener->OnServiceDisconnected();
    }
    return NOERROR;
}


//====================================================
// BluetoothPbap
//====================================================

const String BluetoothPbap::TAG("BluetoothPbap");
const Boolean BluetoothPbap::DBG = TRUE;
const Boolean BluetoothPbap::VDBG = FALSE;
//const String BluetoothPbap::PBAP_STATE("android.bluetooth.pbap.intent.PBAP_STATE");
//const String BluetoothPbap::PBAP_PREVIOUS_STATE("android.bluetooth.pbap.intent.PBAP_PREVIOUS_STATE");
//const String BluetoothPbap::PBAP_STATE_CHANGED_ACTION("android.bluetooth.pbap.intent.action.PBAP_STATE_CHANGED");
//const Int32 BluetoothPbap::STATE_ERROR;
//const Int32 BluetoothPbap::STATE_DISCONNECTED;
//const Int32 BluetoothPbap::STATE_CONNECTING;
//const Int32 BluetoothPbap::STATE_CONNECTED;
//const Int32 BluetoothPbap::RESULT_FAILURE;
//const Int32 BluetoothPbap::RESULT_SUCCESS;
//const Int32 BluetoothPbap::RESULT_CANCELED;

CAR_INTERFACE_IMPL(BluetoothPbap, Object, IBluetoothPbap);

BluetoothPbap::BluetoothPbap()
{
}

BluetoothPbap::BluetoothPbap(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothPbapServiceListener* listener)
    : mContext(context)
    , mServiceListener(listener)
{
    CBluetoothPbapStateChangeCallback::New((IBluetoothPbap*)this, (IIBluetoothStateChangeCallback**)&mBluetoothStateChangeCallback);
    mConnection = new ServiceConnection(this);

    mAdapter = CBluetoothAdapter::GetDefaultAdapter();
    //if (mAdapter != NULL) {
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
    //}

    //AutoPtr<IIntent> intent;
    //CIntent::New(String("IBluetoothPbap")/*IBluetoothPbap.class.getName()*/, (IIntent**)&intent);
    //Boolean result;
    //if (context->BindService(intent, mConnection, 0, &result), !result) {
    //    Logger::E(TAG, "Could not bind to Bluetooth Pbap Service");
    //}
    Boolean bind;
    DoBind(&bind);
}

BluetoothPbap::~BluetoothPbap()
{
    // try {
    Close();
    // } finally {
    //     super.finalize();
    // }
}

ECode BluetoothPbap::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    //Intent intent = new Intent(IBluetoothPbap.class.getName());
    AutoPtr<IIntent> intent;
    CIntent::New(String("IBluetoothPbap")/*IBluetoothPbap.class.getName()*/, (IIntent**)&intent);
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
                userHandle, &succeeded), succeeded)) {
        Logger::E(TAG, "Could not bind to Bluetooth Pbap Service with ");// + intent);
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode BluetoothPbap::Close()
{
    AutoLock lock(mLock);
    //if (mAdapter != NULL) {
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
    //}

    {
        AutoLock lock(mConnectionLock);
        if (mService != NULL) {
            // try {
            mService = NULL;
            ECode ec = mContext->UnbindService(mConnection);
            if (FAILED(ec)) {
                Logger::E(TAG, "0x%08x", ec);
            }
            //mConnection = NULL;
            // } catch (Exception re) {
            //     Log.e(TAG,"",re);
            // }
        }
    }
    mServiceListener = NULL;
    return NOERROR;
}

ECode BluetoothPbap::GetState(
    /* [out] */ Int32* state)
{
    return NOERROR;
}

ECode BluetoothPbap::GetClient(
    /* [out] */ IBluetoothDevice** client)
{
    return NOERROR;
}

ECode BluetoothPbap::IsConnected(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* isConnected)
{
    return NOERROR;
}

ECode BluetoothPbap::Disconnect(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

Boolean BluetoothPbap::DoesClassMatchSink(
    /* [in] */ IBluetoothClass* btClass)
{
    return FALSE;
}

} // Bluetooth
} // Droid
} // Elastos
