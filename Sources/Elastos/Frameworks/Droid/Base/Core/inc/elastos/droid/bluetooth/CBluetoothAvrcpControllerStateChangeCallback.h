#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHAVRCPCONTROLLERSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHAVRCPCONTROLLERSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothAvrcpControllerStateChangeCallback.h"
#include "elastos/droid/bluetooth/BluetoothAvrcpController.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothAvrcpControllerStateChangeCallback)
    , public BluetoothAvrcpController::BluetoothStateChangeCallbackStub
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHAVRCPCONTROLLERSTATECHANGECALLBACK_H__
