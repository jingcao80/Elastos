#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CBLUETOOTHLESCANNER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CBLUETOOTHLESCANNER_H__

#include "_Elastos_Droid_Bluetooth_LE_CBluetoothLeScanner.h"
#include "elastos/droid/bluetooth/le/BluetoothLeScanner.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CBluetoothLeScanner)
    , public BluetoothLeScanner
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CBLUETOOTHLESCANNER_H__
