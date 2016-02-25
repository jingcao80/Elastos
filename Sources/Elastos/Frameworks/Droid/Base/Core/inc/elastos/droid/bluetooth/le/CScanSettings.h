#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CSCANSETTINGS_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CSCANSETTINGS_H__

#include "_Elastos_Droid_Bluetooth_LE_CScanSettings.h"
#include "elastos/droid/bluetooth/le/ScanSettings.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CScanSettings)
    , public ScanSettings
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CSCANSETTINGS_H__
