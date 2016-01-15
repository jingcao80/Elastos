
#ifndef __ELASTOS_DROID_WEBKIT_URLINTERCEPTHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_URLINTERCEPTHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * @hide
 * @deprecated This interface was inteded to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
//@Deprecated
class UrlInterceptHandler : public Object
{
public:
    /**
     * Given an URL, returns the CacheResult which contains the
     * surrogate response for the request, or null if the handler is
     * not interested.
     *
     * @param url URL string.
     * @param headers The headers associated with the request. May be null.
     * @return The CacheResult containing the surrogate response.
     *
     * @hide
     * @deprecated Do not use, this interface is deprecated.
     */
    //@Deprecated
    virtual CARAPI_(AutoPtr<ICacheManagerCacheResult>) Service(
        /* [in] */ const String& url,
        /* [in] */ IObjectStingMap* headers) = 0;

    /**
     * Given an URL, returns the PluginData which contains the
     * surrogate response for the request, or null if the handler is
     * not interested.
     *
     * @param url URL string.
     * @param headers The headers associated with the request. May be null.
     * @return The PluginData containing the surrogate response.
     *
     * @hide
     * @deprecated Do not use, this interface is deprecated.
     */
    //@Deprecated
    virtual CARAPI_(AutoPtr<PluginData>) GetPluginData(
        /* [in] */ const String& url,
        /* [in] */ IObjectStingMap* headers) = 0;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_URLINTERCEPTHANDLER_H__
