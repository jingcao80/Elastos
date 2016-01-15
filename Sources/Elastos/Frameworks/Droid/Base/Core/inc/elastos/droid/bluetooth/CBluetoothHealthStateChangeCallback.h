
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHealthStateChangeCallback.h"
#include "BluetoothHealth.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHealthStateChangeCallback)
{
public:
    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* host);

private:
    BluetoothHealth* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHSTATECHANGECALLBACK_H__
