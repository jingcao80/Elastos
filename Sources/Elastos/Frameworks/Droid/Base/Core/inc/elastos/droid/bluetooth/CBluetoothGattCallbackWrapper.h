#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTCALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTCALLBACKWRAPPER_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothGattCallbackWrapper.h"
#include "elastos/droid/bluetooth/BluetoothGattCallbackWrapper.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothGattCallbackWrapper)
    , public BluetoothGattCallbackWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTCALLBACKWRAPPER_H__
