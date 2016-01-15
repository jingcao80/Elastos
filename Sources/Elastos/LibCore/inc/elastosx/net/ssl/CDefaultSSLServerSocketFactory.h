
#ifndef __ELASTOSX_NET_SSL_CDEFAULTSSLSERVERSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_CDEFAULTSSLSERVERSOCKETFACTORY_H__

#include "_Elastosx_Net_Ssl_CDefaultSSLServerSocketFactory.h"
#include "DefaultSSLServerSocketFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CDefaultSSLServerSocketFactory)
    , public DefaultSSLServerSocketFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CDEFAULTSSLSERVERSOCKETFACTORY_H__
