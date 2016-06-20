#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORDCACHE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORDCACHE_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CAdnRecordCache.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/internal/telephony/uicc/AdnRecordCache.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CAdnRecordCache)
    , public AdnRecordCache
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CADNRECORDCACHE_H__
