#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTSERVERCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTSERVERCALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothGattServerCallback.h"
#include "elastos/droid/bluetooth/BluetoothGattServer.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothGattServerCallback)
    , public BluetoothGattServer::BluetoothGattServerCallbackStub
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHGATTSERVERCALLBACK_H__
