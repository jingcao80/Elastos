#ifndef __ELASTOS_DROID_BLUETOOTH_CLELPPCLIENTWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CLELPPCLIENTWRAPPER_H__

#include "_Elastos_Droid_Bluetooth_CLeLppClientWrapper.h"
#include "elastos/droid/bluetooth/QBluetoothAdapter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CLeLppClientWrapper)
    , public QBluetoothAdapter::LeLppClientWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
#endif