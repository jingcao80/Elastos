#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CSCANFILTER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CSCANFILTER_H__

#include "_Elastos_Droid_Bluetooth_LE_CScanFilter.h"
#include "elastos/droid/bluetooth/le/ScanFilter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CScanFilter)
    , public ScanFilter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CSCANFILTER_H__
