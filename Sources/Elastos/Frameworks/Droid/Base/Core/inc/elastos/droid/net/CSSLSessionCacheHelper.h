
#ifndef __ELASTOS_DROID_NET_CSSLSESSIONCACHEHELPER_H__
#define __ELASTOS_DROID_NET_CSSLSESSIONCACHEHELPER_H__

#include "_Elastos_Droid_Net_CSSLSessionCacheHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

using Elastosx::Net::Ssl::ISSLContext;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * File-based cache of established SSL sessions.  When re-establishing a
 * connection to the same server, using an SSL session cache can save some time,
 * power, and bandwidth by skipping directly to an encrypted stream.
 * This is a persistent cache which can span executions of the application.
 *
 * @see SSLCertificateSocketFactory
 */
CarClass(CSSLSessionCacheHelper)
    , public Singleton
    , public ISSLSessionCacheHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Installs a {@link SSLSessionCache} on a {@link SSLContext}. The cache will
     * be used on all socket factories created by this context (including factories
     * created before this call).
     *
     * @param cache the cache instance to install, or {@code null} to uninstall any
     *         existing cache.
     * @param context the context to install it on.
     * @throws IllegalArgumentException if the context does not support a session
     *         cache.
     *
     * @hide candidate for public API
     */
    CARAPI Install(
        /* [in] */ ISSLSessionCache* cache,
        /* [in] */ ISSLContext* context);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CSSLSESSIONCACHEHELPER_H__
