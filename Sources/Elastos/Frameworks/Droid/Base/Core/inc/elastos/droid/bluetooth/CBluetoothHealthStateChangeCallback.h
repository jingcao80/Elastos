
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHealthStateChangeCallback.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "BluetoothHealth.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHealthStateChangeCallback)
    , public Object
    , public IIBluetoothStateChangeCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CBluetoothHealthStateChangeCallback();

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
    BluetoothHealth* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHSTATECHANGECALLBACK_H__
