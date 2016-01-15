
#include "CBluetoothPanStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

ECode CBluetoothPanStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    //Handle enable request to bind again.
    if (on) {
        Logger::D(BluetoothPan::TAG, "onBluetoothStateChange(on) call bindService");
        AutoPtr<IIntent> intent;
        CIntent::New(String("IBluetoothPan"), (IIntent**)&intent);
        Boolean result;
        if (mHost->mContext->BindService(intent, mHost->mConnection, 0, &result), !result) {
            Logger::E(BluetoothPan::TAG, "Could not bind to Bluetooth HID Service");
        }
        Logger::D(BluetoothPan::TAG, "BluetoothPan(), bindService called");
    }
    else {
        if (BluetoothPan::VDBG) Logger::D(BluetoothPan::TAG,"Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mPanService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothPan::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(BluetoothPan::TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothPanStateChangeCallback::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (BluetoothPan*)host;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
