#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CTRUNCATEDFILTER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CTRUNCATEDFILTER_H__

#include "_Elastos_Droid_Bluetooth_LE_CTruncatedFilter.h"
#include "elastos/droid/bluetooth/le/TruncatedFilter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CTruncatedFilter)
    , public TruncatedFilter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CTRUNCATEDFILTER_H__
