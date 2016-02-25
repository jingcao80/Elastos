#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMASINSTANCE_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMASINSTANCE_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothMasInstance.h"
#include "elastos/droid/bluetooth/BluetoothMasInstance.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothMasInstance)
    , public BluetoothMasInstance
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMASINSTANCE_H__
