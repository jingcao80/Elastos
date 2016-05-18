
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHINPUTDEVICESTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHINPUTDEVICESTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothInputDeviceStateChangeCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/bluetooth/BluetoothInputDevice.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothInputDeviceStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothInputDeviceStateChangeCallback();

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* host);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    BluetoothInputDevice* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__CBLUETOOTHINPUTDEVICSTATECHANGECALLBACK_H__
