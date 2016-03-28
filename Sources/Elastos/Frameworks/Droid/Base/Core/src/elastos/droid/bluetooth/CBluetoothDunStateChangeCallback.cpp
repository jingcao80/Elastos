
#include "elastos/droid/bluetooth/CBluetoothDunStateChangeCallback.h"
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

CAR_INTERFACE_IMPL_2(CBluetoothDunStateChangeCallback, Object, IIBluetoothStateChangeCallback, IBinder)

CAR_OBJECT_IMPL(CBluetoothDunStateChangeCallback)

CBluetoothDunStateChangeCallback::CBluetoothDunStateChangeCallback()
{
}

ECode CBluetoothDunStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    if (BluetoothDun::DBG) Logger::D(BluetoothDun::TAG, "onBluetoothStateChange: on=%d", on);
    if (on) {
        if (mHost->mDunService == NULL) {
            if (BluetoothDun::VDBG) Logger::D(BluetoothDun::TAG, "onBluetoothStateChange call bindService");
            Boolean bind;
            ECode ec;
            if(FAILED(ec = mHost->DoBind(&bind))) {
                Logger::D(BluetoothDun::TAG, "onBluetoothStateChange: could not bind to DUN service");
            }
        }
    }
    else {
        if (BluetoothDun::VDBG) Logger::D(BluetoothDun::TAG, "Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mDunService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothDun::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothDunStateChangeCallback::constructor(
    /* [in] */ IBluetoothDun* host)
{
    mHost = (BluetoothDun*)host;
    return NOERROR;
}

ECode CBluetoothDunStateChangeCallback::ToString(
    /* [out] */ String* str)
{
    *str = "CBluetoothDunStateChangeCallback";
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
