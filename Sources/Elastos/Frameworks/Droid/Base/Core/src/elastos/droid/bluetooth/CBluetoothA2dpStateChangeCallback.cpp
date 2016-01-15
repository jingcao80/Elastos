
#include "CBluetoothA2dpStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

ECode CBluetoothA2dpStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean up)
{
    if (BluetoothA2dp::DBG) Logger::D(BluetoothA2dp::TAG, "onBluetoothStateChange: up=%d", up);
    if (!up) {
        if (BluetoothA2dp::VDBG) Logger::D(BluetoothA2dp::TAG, "Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothA2dp::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    else {
        AutoLock lock(mHost->mConnectionLock);
        // try {
        if (mHost->mService == NULL) {
            if (BluetoothA2dp::VDBG) Logger::D(BluetoothA2dp::TAG, "Binding service...");
            AutoPtr<IIntent> intent;
            CIntent::New(String("IBluetoothHeadset")/*IBluetoothHeadset.class.getName()*/, (IIntent**)&intent);
            Boolean result;
            ECode ec = mHost->mContext->BindService(intent, mHost->mConnection, 0, &result);
            if (FAILED(ec)) {
                Logger::E(BluetoothA2dp::TAG, "0x%08x", ec);
            }
            else if (!result) {
                Logger::E(BluetoothA2dp::TAG, "Could not bind to Bluetooth Headset Service");
            }
        }
        // } catch (Exception re) {
        //     Log.e(TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothA2dpStateChangeCallback::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (BluetoothA2dp*)host;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
