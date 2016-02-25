#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETCLIENTSTATECHANGECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETCLIENTSTATECHANGECALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHeadsetClientStateChangeCallback.h"
#include "elastos/droid/bluetooth/BluetoothHeadsetClient.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHeadsetClientStateChangeCallback)
    , public BluetoothHeadsetClient::BluetoothStateChangeCallbackStub
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETCLIENTSTATECHANGECALLBACK_H__
