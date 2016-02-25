#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHACTIVITYENERGYINFO_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHACTIVITYENERGYINFO_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothActivityEnergyInfo.h"
#include "elastos/droid/bluetooth/BluetoothActivityEnergyInfo.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothActivityEnergyInfo)
    , public BluetoothActivityEnergyInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHACTIVITYENERGYINFO_H__
