#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETCLIENTCALL_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETCLIENTCALL_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothHeadsetClientCall.h"
#include "elastos/droid/bluetooth/BluetoothHeadsetClientCall.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHeadsetClientCall)
    , public BluetoothHeadsetClientCall
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEADSETCLIENTCALL_H__
