#ifndef __ELASTOS_NET_CRESPONSE_CACHE_HELPER_H__
#define __ELASTOS_NET_CRESPONSE_CACHE_HELPER_H__

#include <_Elastos_Net_CResponseCacheHelper.h>
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CResponseCacheHelper)
    , public Singleton
    , public IResponseCacheHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the system's default response cache, or null.
     */
    CARAPI GetDefault(
        /* [out] */ IResponseCache** responseCache);

    /**
     * Sets the system's default response cache. Use null to remove the response cache.
     */
    CARAPI SetDefault(
        /* [in] */ IResponseCache* responseCache);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CRESPONSE_CACHE_HELPER_H__
