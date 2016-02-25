#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DPSINKSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DPSINKSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothA2dpSinkStateChangeCallback.h"
#include "elastos/droid/bluetooth/BluetoothA2dpSink.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothA2dpSinkStateChangeCallback)
    , public BluetoothA2dpSink::BluetoothStateChangeCallbackStub
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DPSINKSTATECHANGECALLBACK_H__
