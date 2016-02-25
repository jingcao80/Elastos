#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHAUDIOCONFIG_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHAUDIOCONFIG_H__

#include "_Elastos_Droid_Bluetooth_CBluetoothAudioConfig.h"
#include "elastos/droid/bluetooth/BluetoothAudioConfig.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothAudioConfig)
    , public BluetoothAudioConfig
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHAUDIOCONFIG_H__
