
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CLOCALLOG_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CLOCALLOG_H__

#include "_Elastos_Droid_Internal_Utility_CLocalLog.h"
#include "elastos/droid/internal/utility/LocalLog.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CLocalLog)
    , public LocalLog
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos


#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CLOCALLOG_H__
