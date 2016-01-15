
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPANSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPANSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothPanStateChangeCallback.h"
#include "BluetoothPan.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothPanStateChangeCallback)
{
public:
    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* host);

private:
    BluetoothPan* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHPANSTATECHANGECALLBACK_H__
