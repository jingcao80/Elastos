
#include "elastos/droid/webkit/CUrlInterceptRegistry.h"
#include "elastos/droid/webkit/UrlInterceptRegistry.h"
#include <elastos/utility/etl/Algorithm.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CUrlInterceptRegistry, Object, IUrlInterceptRegistry);

CAR_SINGLETON_IMPL(CUrlInterceptRegistry);

ECode CUrlInterceptRegistry::SetUrlInterceptDisabled(
    /* [in] */ Boolean disabled)
{
    return UrlInterceptRegistry::SetUrlInterceptDisabled(disabled);
}

ECode CUrlInterceptRegistry::UrlInterceptDisabled(
    /* [out] */ Boolean* disabled)
{
    VALIDATE_NOT_NULL(disabled);
    *disabled = UrlInterceptRegistry::UrlInterceptDisabled();
    return NOERROR;
}

ECode CUrlInterceptRegistry::RegisterHandler(
    /* [in] */ IUrlInterceptHandler* handler,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(handler);
    *result = UrlInterceptRegistry::RegisterHandler(handler);
    return NOERROR;
}

ECode CUrlInterceptRegistry::UnregisterHandler(
    /* [in] */ IUrlInterceptHandler* handler,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(handler);
    *result = UrlInterceptRegistry::UnregisterHandler(handler);
    return NOERROR;
}

ECode CUrlInterceptRegistry::GetSurrogate(
    /* [in] */ const String& url,
    /* [in] */ IMap* headers,
    /* [out] */ ICacheManagerCacheResult** cacheResult)
{
    VALIDATE_NOT_NULL(headers);
    VALIDATE_NOT_NULL(cacheResult);
    AutoPtr<ICacheManagerCacheResult> result = UrlInterceptRegistry::GetSurrogate(url, headers);
    *cacheResult = result;
    REFCOUNT_ADD(*cacheResult);
    return NOERROR;
}

ECode CUrlInterceptRegistry::GetPluginData(
    /* [in] */ const String& url,
    /* [in] */ IMap* headers,
    /* [out] */ IPluginData** pluginData)
{
    VALIDATE_NOT_NULL(headers);
    AutoPtr<IPluginData> pd = UrlInterceptRegistry::GetPluginData(url, headers);
    *pluginData = pd;
    REFCOUNT_ADD(*pluginData);
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
