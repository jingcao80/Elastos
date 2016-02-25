#include "elastos/droid/bluetooth/le/CScanRecordHelper.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

CAR_INTERFACE_IMPL(CScanRecordHelper, Singleton, IScanRecordHelper)
CAR_SINGLETON_IMPL(CScanRecordHelper)

ECode CScanRecordHelper::ParseFromBytes(
    /* [in] */ ArrayOf<Byte>* scanRecord,
    /* [out] */ IScanRecord** record)
{
    VALIDATE_NOT_NULL(record);
    AutoPtr<IScanRecord> result = ScanRecord::ParseFromBytes(scanRecord);
    *record = result;
    REFCOUNT_ADD(*record);
    return NOERROR;
}


} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
