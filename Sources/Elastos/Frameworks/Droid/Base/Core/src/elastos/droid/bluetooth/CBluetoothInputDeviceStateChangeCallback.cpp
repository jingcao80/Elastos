
#include "CBluetoothInputDeviceStateChangeCallback.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

ECode CBluetoothInputDeviceStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    if (BluetoothInputDevice::DBG) Logger::D(BluetoothInputDevice::TAG, "onBluetoothStateChange: up=%d", on);
    if (!on) {
        if (BluetoothInputDevice::VDBG) Logger::D(BluetoothInputDevice::TAG,"Unbinding service...");
        AutoLock lock(mHost->mConnectionLock);
        // try {
        mHost->mService = NULL;
        ECode ec = mHost->mContext->UnbindService(mHost->mConnection);
        if (FAILED(ec)) {
            Logger::E(BluetoothInputDevice::TAG, "0x%08x", ec);
        }
        // } catch (Exception re) {
        //     Logger::E(BluetoothInputDevice::TAG,"",re);
        // }
    }
    else {
        AutoLock lock(mHost->mConnectionLock);
        // try {
        if (mHost->mService == NULL) {
            if (BluetoothInputDevice::VDBG) Logger::D(BluetoothInputDevice::TAG, "Binding service...");
            AutoPtr<IIntent> intent;
            CIntent::New(String("IBluetoothInputDevice")/*IBluetoothInputDevice.class.getName()*/, (IIntent**)&intent);
            Boolean result;
            if (mHost->mContext->BindService(intent, mHost->mConnection, 0, &result), !result) {
                Logger::E(BluetoothInputDevice::TAG, "Could not bind to Bluetooth HID Service");
            }
        }
        // } catch (Exception re) {
        //     Logger::E(BluetoothInputDevice::TAG,"",re);
        // }
    }
    return NOERROR;
}

ECode CBluetoothInputDeviceStateChangeCallback::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (BluetoothInputDevice*)host;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
