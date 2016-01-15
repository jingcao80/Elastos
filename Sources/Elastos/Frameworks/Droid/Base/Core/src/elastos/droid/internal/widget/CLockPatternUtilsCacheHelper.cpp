
#include "elastos/droid/internal/widget/CLockPatternUtilsCacheHelper.h"
#include "elastos/droid/internal/widget/CLockPatternUtilsCache.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CAR_SINGLETON_IMPL(CLockPatternUtilsCacheHelper);
CAR_INTERFACE_IMPL(CLockPatternUtilsCacheHelper, Singleton, ILockPatternUtilsCacheHelper);

ECode CLockPatternUtilsCacheHelper::GetInstance(
    /* [in] */ IILockSettings* service,
    /* [out] */ ILockPatternUtilsCache** instance)
{
    AutoLock lock(this);
    VALIDATE_NOT_NULL(instance);
    AutoPtr<ILockPatternUtilsCache> cache = CLockPatternUtilsCache::GetInstance(service);
    *instance = cache;
    REFCOUNT_ADD(*instance);

    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
