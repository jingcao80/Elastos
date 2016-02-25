#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTDESCRIPTOR_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTDESCRIPTOR_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothGattDescriptor.h"
#include "elastos/droid/bluetooth/BluetoothGattDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothGattDescriptor)
    , public BluetoothGattDescriptor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTDESCRIPTOR_H__
