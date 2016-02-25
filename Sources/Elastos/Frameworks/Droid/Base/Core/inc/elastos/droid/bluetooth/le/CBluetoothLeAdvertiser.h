#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CBLUETOOTHLEADVERTISER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CBLUETOOTHLEADVERTISER_H__

#include "_Elastos_Droid_Bluetooth_LE_CBluetoothLeAdvertiser.h"
#include "elastos/droid/bluetooth/le/BluetoothLeAdvertiser.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CBluetoothLeAdvertiser)
    , public BluetoothLeAdvertiser
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CBLUETOOTHLEADVERTISER_H__
