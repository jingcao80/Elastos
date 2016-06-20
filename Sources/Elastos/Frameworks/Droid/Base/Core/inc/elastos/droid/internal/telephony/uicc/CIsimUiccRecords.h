#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CISIMUICCRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CISIMUICCRECORDS_H__

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Internal_Telephony_Uicc_CIsimUiccRecords.h"
#include "elastos/droid/internal/telephony/uicc/IsimUiccRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CIsimUiccRecords)
    , public IsimUiccRecords
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CISIMUICCRECORDS_H__
