#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CPDUCACHEENTRY_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CPDUCACHEENTRY_H__

#include "_Elastos_Droid_Google_Mms_Utility_CPduCacheEntry.h"
#include "elastos/droid/google/mms/utility/PduCacheEntry.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CarClass(CPduCacheEntry)
    , public PduCacheEntry
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CPDUCACHEENTRY_H__
