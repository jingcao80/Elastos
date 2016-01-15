
#ifndef __ELASTOSX_NET_SSL_CDEFAULTSSLSOCKETFACTORY_H__
#define __ELASTOSX_NET_SSL_CDEFAULTSSLSOCKETFACTORY_H__

#include "_Elastosx_Net_Ssl_CDefaultSSLSocketFactory.h"
#include "DefaultSSLSocketFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CDefaultSSLSocketFactory)
    , public DefaultSSLSocketFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CDEFAULTSSLSOCKETFACTORY_H__
