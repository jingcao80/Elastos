
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DOSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DOSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothA2dpStateChangeCallback.h"
#include "BluetoothA2dp.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothA2dpStateChangeCallback)
{
public:
    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* host);

private:
    BluetoothA2dp* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHA2DOSTATECHANGECALLBACK_H__
