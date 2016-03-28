#include "elastos/droid/bluetooth/CBluetoothHidDeviceStateChangeCallback.h"

#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothHidDeviceStateChangeCallback)

CAR_INTERFACE_IMPL_2(CBluetoothHidDeviceStateChangeCallback,Object, IIBluetoothStateChangeCallback, IBinder)

CBluetoothHidDeviceStateChangeCallback::CBluetoothHidDeviceStateChangeCallback()
{}

ECode CBluetoothHidDeviceStateChangeCallback::constructor(
    /* [in] */ IBluetoothHidDevice* cb)
{
    mHost = (BluetoothHidDevice*)cb;
    return NOERROR;
}

ECode CBluetoothHidDeviceStateChangeCallback::OnBluetoothStateChange(
    /* [in] */ Boolean on)
{
    SLOGGERD(BluetoothHidDevice::TAG, "onBluetoothStateChange: on= %d", on)
    AutoLock lock(mHost->mConnectionLock);
    if (!on) {
        SLOGGERD(BluetoothHidDevice::TAG,"Unbinding service...")
        if (mHost->mService != NULL) {
            mHost->mService = NULL;
            // try {
                if (FAILED(mHost->mContext->UnbindService(mHost->mConnection))) {
                    SLOGGERE(BluetoothHidDevice::TAG,"onBluetoothStateChange: could not unbind service: IIBluetoothHidDevice")
                }
            // } catch (IllegalArgumentException e) {
            // }
        }
    } else {
        // try {
            if (mHost->mService == NULL) {
                SLOGGERD(BluetoothHidDevice::TAG,"Binding HID Device service...");
                Boolean res;
                mHost->DoBind(&res);
            }
        // } catch (IllegalStateException e) {
            SLOGGERE(BluetoothHidDevice::TAG,"onBluetoothStateChange: could not bind to HID Dev service: IIBluetoothHidDevice")
        // } catch (SecurityException e) {
            // Log.e(TAG,"onBluetoothStateChange: could not bind to HID Dev service: ", e);
        // }
    }
    return NOERROR;
}

ECode CBluetoothHidDeviceStateChangeCallback::ToString(
    /* [out] */ String* str)
{
    *str = "CBluetoothHidDeviceStateChangeCallback";
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos    CAR_INTERFACE_DECL()

