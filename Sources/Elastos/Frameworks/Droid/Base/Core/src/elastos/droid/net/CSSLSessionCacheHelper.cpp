
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CSSLSessionCache.h"
#include "elastos/droid/net/CSSLSessionCacheHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CSSLSessionCacheHelper, Singleton, ISSLSessionCacheHelper)

CAR_SINGLETON_IMPL(CSSLSessionCacheHelper)

ECode CSSLSessionCacheHelper::Install(
        /* [in] */ ISSLSessionCache* cache,
        /* [in] */ ISSLContext* context)
{
    return CSSLSessionCache::Install(cache, context);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
