#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASERVICESTATETRACKER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASERVICESTATETRACKER_H__

#include "_Elastos_Droid_Internal_Telephony_Cdma_CCdmaServiceStateTracker.h"
#include "elastos/droid/internal/telephony/cdma/CdmaServiceStateTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CarClass(CCdmaServiceStateTracker)
    , public CdmaServiceStateTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_CCDMASERVICESTATETRACKER_H__
