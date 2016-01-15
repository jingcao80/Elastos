
#include "ResponseCache.h"

namespace Elastos {
namespace Net {

AutoPtr<IResponseCache> ResponseCache::sDefaultResponseCache;

CAR_INTERFACE_IMPL(ResponseCache, Object, IResponseCache)

ResponseCache::ResponseCache()
{
}

ResponseCache::~ResponseCache()
{
}

ECode ResponseCache::GetDefault(
    /* [out] */ IResponseCache** responseCache)
{
    VALIDATE_NOT_NULL(responseCache);
    // SecurityManager sm = System.getSecurityManager();
    // if (sm != null) {
    //     sm.checkPermission(new NetPermission("getResponseCache"));
    // }
    *responseCache = sDefaultResponseCache;
    REFCOUNT_ADD(*responseCache);
    return NOERROR;
}

/**
 * Sets the system's default response cache. Use null to remove the response cache.
 */
ECode ResponseCache::SetDefault(
    /* [in] */ IResponseCache* responseCache)
{
    // SecurityManager sm = System.getSecurityManager();
    // if (sm != null) {
    //     sm.checkPermission(new NetPermission("setResponseCache"));
    // }
    sDefaultResponseCache = responseCache;
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
