#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTSERVICE_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTSERVICE_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothGattService.h"
#include "elastos/droid/bluetooth/BluetoothGattService.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothGattService)
    , public BluetoothGattService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTSERVICE_H__
