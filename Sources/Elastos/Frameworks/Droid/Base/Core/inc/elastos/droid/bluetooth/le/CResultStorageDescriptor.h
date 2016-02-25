#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CRESULTSTORAGEDESCRIPTOR_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CRESULTSTORAGEDESCRIPTOR_H__

#include "_Elastos_Droid_Bluetooth_LE_CResultStorageDescriptor.h"
#include "elastos/droid/bluetooth/le/ResultStorageDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CResultStorageDescriptor)
    , public ResultStorageDescriptor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CRESULTSTORAGEDESCRIPTOR_H__
