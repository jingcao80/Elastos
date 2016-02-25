#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CADVERTISEDATA_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CADVERTISEDATA_H__

#include "_Elastos_Droid_Bluetooth_LE_CAdvertiseData.h"
#include "elastos/droid/bluetooth/le/AdvertiseData.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CAdvertiseData)
    , public AdvertiseData
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CADVERTISEDATA_H__
