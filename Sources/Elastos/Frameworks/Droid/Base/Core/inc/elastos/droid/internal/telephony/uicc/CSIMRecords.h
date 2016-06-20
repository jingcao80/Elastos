#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMRECORDS_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CSIMRecords.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/SIMRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CSIMRecords)
    , public SIMRecords
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CSIMRECORDS_H__
