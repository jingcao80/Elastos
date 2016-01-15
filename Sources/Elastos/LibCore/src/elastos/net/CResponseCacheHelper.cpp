
#include "CResponseCacheHelper.h"
#include "ResponseCache.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CResponseCacheHelper, Singleton, IResponseCacheHelper)

CAR_SINGLETON_IMPL(CResponseCacheHelper)

ECode CResponseCacheHelper::GetDefault(
    /* [out] */ IResponseCache** responseCache)
{
    return ResponseCache::GetDefault(responseCache);
}

ECode CResponseCacheHelper::SetDefault(
    /* [in] */ IResponseCache* responseCache)
{
    return ResponseCache::SetDefault(responseCache);
}

} // namespace Net
} // namespace Elastos