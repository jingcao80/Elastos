#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMANAGERCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHMANAGERCALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothManagerCallback.h"
#include "elastos/droid/bluetooth/QBluetoothAdapter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothManagerCallback)
    , public QBluetoothAdapter::BluetoothManagerCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif