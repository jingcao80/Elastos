
#ifndef __ELASTOSX_NET_SSL_CSSLSERVERSOCKETFACTORYHELPER_H__
#define __ELASTOSX_NET_SSL_CSSLSERVERSOCKETFACTORYHELPER_H__

#include "_Elastosx_Net_Ssl_CSSLServerSocketFactoryHelper.h"
#include "Singleton.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLServerSocketFactoryHelper)
    , public Singleton
    , public ISSLServerSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the default {@code SSLServerSocketFactory} instance. The default
     * implementation is defined by the security property
     * "ssl.ServerSocketFactory.provider".
     *
     * @return the default {@code SSLServerSocketFactory} instance.
     */
    CARAPI GetDefault(
        /* [out] */ IServerSocketFactory** factory);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLSERVERSOCKETFACTORYHELPER_H__
