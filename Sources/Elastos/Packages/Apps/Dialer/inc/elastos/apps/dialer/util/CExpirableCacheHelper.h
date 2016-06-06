#ifndef __ELASTOS_APPS_DIALER_UTIL_CEMPTYLOADERHELPER_H__
#define __ELASTOS_APPS_DIALER_UTIL_CEMPTYLOADERHELPER_H__

#include "_Elastos_Apps_Dialer_Util_CExpirableCacheHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Util {

CarClass(CExpirableCacheHelper)
    , public Singleton
    , public IExpirableCacheHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates a new {@link ExpirableCache} that wraps the given {@link LruCache}.
     * <p>
     * The created cache takes ownership of the cache passed in as an argument.
     *
     * @param <K> the type of the keys
     * @param <V> the type of the values
     * @param cache the cache to store the value in
     * @return the newly created expirable cache
     * @throws IllegalArgumentException if the cache is not empty
     */
    CARAPI Create(
        /* [in] */ IInterface* cache,
        /* [out] */ IExpirableCache** result);

    /**
     * Creates a new {@link ExpirableCache} with the given maximum size.
     *
     * @param <K> the type of the keys
     * @param <V> the type of the values
     * @return the newly created expirable cache
     */
    CARAPI Create(
        /* [in] */ Int32 maxSize,
        /* [out] */ IExpirableCache** result);
};

} // Util
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_UTIL_CEMPTYLOADERHELPER_H__
