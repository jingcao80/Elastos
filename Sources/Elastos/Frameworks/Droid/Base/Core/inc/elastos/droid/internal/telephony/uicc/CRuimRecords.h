#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CRUIMRECORDS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CRUIMRECORDS_H__

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Internal_Telephony_Uicc_CRuimRecords.h"
#include "elastos/droid/internal/telephony/uicc/RuimRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CRuimRecords)
    , public RuimRecords
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CRUIMRECORDS_H__
