
#ifndef __ELASTOS_DROID_NET_SSLSESSIONCACHE_H__
#define __ELASTOS_DROID_NET_SSLSESSIONCACHE_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Core.h"
#include "_Org.Conscrypt.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

using Elastos::IO::IFile;
using Elastosx::Net::Ssl::ISSLContext;
using Org::Conscrypt::ISSLClientSessionCache;

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
class SSLSessionCache
    : public Object
    , public ISSLSessionCache
{
public:
    CAR_INTERFACE_DECL()

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
    static CARAPI Install(
        /* [in] */ ISSLSessionCache* cache,
        /* [in] */ ISSLContext* context);

    /**
     * NOTE: This needs to be Object (and not SSLClientSessionCache) because apps
     * that build directly against the framework (and not the SDK) might not declare
     * a dependency on conscrypt. Javac will then has fail while resolving constructors.
     *
     * @hide For unit test use only
     */
    CARAPI constructor(
        /* [in] */ IInterface* cache);

    /**
     * Create a session cache using the specified directory.
     * Individual session entries will be files within the directory.
     * Multiple instances for the same directory share data internally.
     *
     * @param dir to store session files in (created if necessary)
     * @throws IOException if the cache can't be opened
     */
    CARAPI constructor(
        /* [in] */ IFile* dir);

    /**
     * Create a session cache at the default location for this app.
     * Multiple instances share data internally.
     *
     * @param context for the application
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /* package */
    CARAPI GetSessionCache(
        /* [out] */ ISSLClientSessionCache** result);

public:
    /* package */
    AutoPtr<ISSLClientSessionCache> mSessionCache;

private:
    static const String TAG;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_SSLSESSIONCACHE_H__
