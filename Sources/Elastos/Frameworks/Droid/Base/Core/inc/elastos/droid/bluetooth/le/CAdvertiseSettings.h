#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CADVERTISESETTINGS_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CADVERTISESETTINGS_H__

#include "_Elastos_Droid_Bluetooth_LE_CAdvertiseSettings.h"
#include "elastos/droid/bluetooth/le/AdvertiseSettings.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CAdvertiseSettings)
    , public AdvertiseSettings
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CADVERTISESETTINGS_H__
