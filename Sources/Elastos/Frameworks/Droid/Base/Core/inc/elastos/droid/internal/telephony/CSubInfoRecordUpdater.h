
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATER_H__

#include "_Elastos_Droid_Internal_Telephony_CSubInfoRecordUpdater.h"
#include "elastos/droid/internal/telephony/SubInfoRecordUpdater.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSubInfoRecordUpdater), public SubInfoRecordUpdater
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CSUBINFORECORDUPDATER_H__
