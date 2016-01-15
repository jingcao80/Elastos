
#ifndef __ELASTOSX_NET_SSL_SSLSERVERSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_SSLSERVERSOCKETFACTORY_H__

#include "ServerSocketFactory.h"

using Elastosx::Net::IServerSocketFactory;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The factory for SSL server sockets.
 */
class SSLServerSocketFactory
    : public ServerSocketFactory
    , public ISSLServerSocketFactory
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the default {@code SSLServerSocketFactory} instance. The default
     * implementation is defined by the security property
     * "ssl.ServerSocketFactory.provider".
     *
     * @return the default {@code SSLServerSocketFactory} instance.
     */
    static CARAPI GetDefault(
        /* [out] */ IServerSocketFactory** factory);

private:
    // TODO EXPORT CONTROL

    // The default SSL socket factory
    static AutoPtr<IServerSocketFactory> sDefaultServerSocketFactory;

    static String sDefaultName;

    static Int32 sLastCacheVersion;

    static Object sLock;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_SSLSERVERSOCKETFACTORY_H__
