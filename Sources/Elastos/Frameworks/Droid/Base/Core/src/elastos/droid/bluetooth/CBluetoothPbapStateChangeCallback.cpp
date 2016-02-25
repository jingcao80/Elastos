
#include "elastos/droid/bluetooth/CBluetoothPbapStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(CBluetoothPbapStateChangeCallback, Object, IIBluetoothStateChangeCallback, IBinder);

CAR_OBJECT_IMPL(CBluetoothPbapStateChangeCallback);

CBluetoothPbapStateChangeCallback::CBluetoothPbapStateChangeCallback()
{
}

ECode CBluetoothPbapStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean up)
{
    if (BluetoothPbap::DBG) Logger::D(BluetoothPbap::TAG, "onBluetoothStateChange: up=%d", up);
    if (!up) {
        if (BluetoothPbap::VDBG) Logger::D(BluetoothPbap::TAG, "Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothPbap::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(BluetoothPbap::TAG,"",re);
        // }
    }
    else {
        AutoLock lock(mHost->mConnectionLock);
        // try {
        if (mHost->mService == NULL) {
            if (BluetoothPbap::VDBG) Logger::D(BluetoothPbap::TAG, "Binding service...");
            //AutoPtr<IIntent> intent;
            //CIntent::New(String("IBluetoothPbap")/*IBluetoothPbap.class.getName()*/, (IIntent**)&intent);
            //Boolean result;
            //ECode ec = mHost->mContext->BindService(intent, mHost->mConnection, 0, &result);
            //if (FAILED(ec)) {
            //    Logger::E(BluetoothPbap::TAG, "0x%08x", ec);
            //}
            //else if (!result) {
            //    Logger::E(BluetoothPbap::TAG, "Could not bind to Bluetooth PBAP Service");
            //}
            Boolean bind;
            mHost->DoBind(&bind);
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothPbapStateChangeCallback::constructor(
    /* [in] */ IBluetoothPbap* host)
{
    mHost = (BluetoothPbap*)(host);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
