#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORDLOADER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORDLOADER_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CAdnRecordLoader.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecordLoader.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CAdnRecordLoader)
    , public AdnRecordLoader
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORDLOADER_H__
