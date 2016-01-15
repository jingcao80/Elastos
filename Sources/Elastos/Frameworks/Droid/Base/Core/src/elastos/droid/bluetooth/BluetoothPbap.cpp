
#include "BluetoothPbap.h"
#include "CBluetoothAdapter.h"
#include "CBluetoothPbapStateChangeCallback.h"
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
// BluetoothPbap::ServiceConnection
//====================================================

BluetoothPbap::ServiceConnection::ServiceConnection(
    /* [in] */ BluetoothPbap* host)
    : mHost(host)
{}

CAR_INTERFACE_IMPL(BluetoothPbap::ServiceConnection, IServiceConnection)

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
const String BluetoothPbap::PBAP_STATE("android.bluetooth.pbap.intent.PBAP_STATE");
const String BluetoothPbap::PBAP_PREVIOUS_STATE("android.bluetooth.pbap.intent.PBAP_PREVIOUS_STATE");
const String BluetoothPbap::PBAP_STATE_CHANGED_ACTION("android.bluetooth.pbap.intent.action.PBAP_STATE_CHANGED");
const Int32 BluetoothPbap::STATE_ERROR;
const Int32 BluetoothPbap::STATE_DISCONNECTED;
const Int32 BluetoothPbap::STATE_CONNECTING;
const Int32 BluetoothPbap::STATE_CONNECTED;
const Int32 BluetoothPbap::RESULT_FAILURE;
const Int32 BluetoothPbap::RESULT_SUCCESS;
const Int32 BluetoothPbap::RESULT_CANCELED;

BluetoothPbap::BluetoothPbap(
    /* [in] */ IContext* context,
    /* [in] */ IServiceListener* listener)
    : mContext(context)
    , mServiceListener(listener)
{
    CBluetoothPbapStateChangeCallback::New((Handle32)this, (IIBluetoothStateChangeCallback**)&mBluetoothStateChangeCallback);
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
    CIntent::New(String("IBluetoothPbap")/*IBluetoothPbap.class.getName()*/, (IIntent**)&intent);
    Boolean result;
    if (context->BindService(intent, mConnection, 0, &result), !result) {
        Logger::E(TAG, "Could not bind to Bluetooth Pbap Service");
    }
}

BluetoothPbap::~BluetoothPbap()
{
    // try {
    Close();
    // } finally {
    //     super.finalize();
    // }
}

void BluetoothPbap::Close()
{
    AutoLock lock(mLock);
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

    {
        AutoLock lock(mConnectionLock);
        if (mService != NULL) {
            // try {
            mService = NULL;
            ECode ec = mContext->UnbindService(mConnection);
            if (FAILED(ec)) {
                Logger::E(TAG, "0x%08x", ec);
            }
            mConnection = NULL;
            // } catch (Exception re) {
            //     Log.e(TAG,"",re);
            // }
        }
    }
    mServiceListener = NULL;
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
