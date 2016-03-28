#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICESTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHIDDEVICESTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHidDeviceStateChangeCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/bluetooth/BluetoothHidDevice.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHidDeviceStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBluetoothHidDeviceStateChangeCallback();

    CARAPI constructor(
        /* [in] */ IBluetoothHidDevice* cb);

    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    BluetoothHidDevice* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif