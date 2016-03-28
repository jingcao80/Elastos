#ifndef __ELASTOS_DROID_BLUETOOTH_CQBLUETOOTHMANAGERCALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_CQBLUETOOTHMANAGERCALLBACK_H__

#include "_Elastos_Droid_Bluetooth_CQBluetoothManagerCallback.h"
#include "elastos/droid/bluetooth/QBluetoothAdapter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CQBluetoothManagerCallback)
    , public QBluetoothAdapter::QBluetoothManagerCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif