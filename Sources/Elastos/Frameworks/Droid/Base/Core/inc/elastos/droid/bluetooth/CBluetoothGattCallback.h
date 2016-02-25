#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTCALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothGattCallback.h"
#include "elastos/droid/bluetooth/BluetoothGatt.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothGattCallback)
    , public BluetoothGatt::InnerBluetoothGattCallbackWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTCALLBACK_H__
