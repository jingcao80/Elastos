#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMANAGER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMANAGER_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothManager.h"
#include "elastos/droid/bluetooth/BluetoothManager.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothManager)
    , public BluetoothManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMANAGER_H__
