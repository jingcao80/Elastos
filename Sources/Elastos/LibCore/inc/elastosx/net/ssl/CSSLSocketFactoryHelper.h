
#ifndef __ELASTOSX_NET_SSL_CSSLSOCKETFACTORYHELPER_H__
#define __ELASTOSX_NET_SSL_CSSLSOCKETFACTORYHELPER_H__

#include "_Elastosx_Net_Ssl_CSSLSocketFactoryHelper.h"
#include "Singleton.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLSocketFactoryHelper)
    , public Singleton
    , public ISSLSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the default {@code SSLSocketFactory} instance. The default is
     * defined by the security property {@code 'ssl.SocketFactory.provider'}.
     *
     * @return the default ssl socket factory instance.
     */
    GetDefault(
        /* [out] */ ISocketFactory** factory);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLSOCKETFACTORYHELPER_H__
