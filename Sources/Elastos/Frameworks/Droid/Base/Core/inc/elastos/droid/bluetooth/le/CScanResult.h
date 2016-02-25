#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CSCANRESULT_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CSCANRESULT_H__

#include "_Elastos_Droid_Bluetooth_LE_CScanResult.h"
#include "elastos/droid/bluetooth/le/ScanResult.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CScanResult)
    , public ScanResult
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CSCANRESULT_H__
