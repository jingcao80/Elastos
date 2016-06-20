#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORD_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORD_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CAdnRecord.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecord.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CAdnRecord)
    , public AdnRecord
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORD_H__
