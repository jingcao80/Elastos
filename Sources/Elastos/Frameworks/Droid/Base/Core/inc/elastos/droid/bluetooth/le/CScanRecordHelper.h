#ifndef __ELASTOS_DROID_BLUETOOTH_LE_CSCANRECORDHELPER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_CSCANRECORDHELPER_H__

#include "_Elastos_Droid_Bluetooth_LE_CScanRecordHelper.h"
#include <elastos/core/Singleton.h>
#include "elastos/droid/bluetooth/le/ScanRecord.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CarClass(CScanRecordHelper)
    , public Singleton
    , public IScanRecordHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ParseFromBytes(
        /* [in] */ ArrayOf<Byte>* scanRecord,
        /* [out] */ IScanRecord** record);
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_CSCANRECORDHELPER_H__
