
#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHeadsetStateChangeCallback.h"
#include "BluetoothHeadset.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHeadsetStateChangeCallback)
{
public:
    CARAPI OnBluetoothStateChange(
        /* [in] */ Boolean on);

    CARAPI constructor(
        /* [in] */ IInterface* host);

private:
    BluetoothHeadset* mHost;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETSTATECHANGECALLBACK_H__
