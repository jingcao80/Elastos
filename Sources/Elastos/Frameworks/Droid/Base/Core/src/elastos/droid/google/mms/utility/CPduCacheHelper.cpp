#include "elastos/droid/google/mms/utility/CPduCacheHelper.h"
#include "elastos/droid/google/mms/utility/PduCache.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CAR_INTERFACE_IMPL(CPduCacheHelper, Singleton, IPduCacheHelper)
CAR_SINGLETON_IMPL(CPduCacheHelper)

ECode CPduCacheHelper::GetInstance(
    /* [out] */ IPduCache** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IPduCache> pc = PduCache::GetInstance();
    *result  = pc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
