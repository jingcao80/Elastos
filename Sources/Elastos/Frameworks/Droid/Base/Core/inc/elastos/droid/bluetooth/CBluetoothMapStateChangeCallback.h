#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMAPSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMAPSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothMapStateChangeCallback.h"
#include "elastos/droid/bluetooth/BluetoothMap.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothMapStateChangeCallback)
    , public BluetoothMap::BluetoothStateChangeCallbackStub
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMAPSTATECHANGECALLBACK_H__
