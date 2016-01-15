
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/http/CHttpResponseCache.h"
#include "elastos/droid/net/http/CHttpResponseCacheHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

CAR_INTERFACE_IMPL(CHttpResponseCacheHelper, Singleton, IHttpResponseCacheHelper)

CAR_SINGLETON_IMPL(CHttpResponseCacheHelper)

ECode CHttpResponseCacheHelper::GetInstalled(
    /* [out] */ IHttpResponseCache** result)
{
    return HttpResponseCache::GetInstalled(result);
}

ECode CHttpResponseCacheHelper::Install(
    /* [in] */ IFile* directory,
    /* [in] */ Int64 maxSize,
    /* [out] */ IHttpResponseCache** result)
{
    return HttpResponseCache::Install(directory, maxSize, result);
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
