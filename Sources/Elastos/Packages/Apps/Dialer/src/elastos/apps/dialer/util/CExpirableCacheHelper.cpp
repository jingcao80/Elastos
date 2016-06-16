
#include "util/CExpirableCacheHelper.h"
#include "util/ExpirableCache.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Util {

CAR_INTERFACE_IMPL(CExpirableCacheHelper, Singleton, IExpirableCacheHelper);

CAR_SINGLETON_IMPL(CExpirableCacheHelper);

ECode CExpirableCacheHelper::Create(
    /* [in] */ IInterface* cache,
    /* [out] */ IExpirableCache** result)
{
    VALUE_NOT_NULL(result);
    AutoPtr<IExpirableCache> obj = ExpirableCache::Create(cache);
    *result = obj;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CExpirableCacheHelper::Create(
    /* [in] */ Int32 maxSize,
    /* [out] */ IExpirableCache** result);
{
    VALUE_NOT_NULL(result);
    AutoPtr<IExpirableCache> obj = ExpirableCache::Create(maxSize);
    *result = obj;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // Util
} // Dialer
} // Apps
} // Elastos
