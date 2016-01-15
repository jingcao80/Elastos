
#ifndef __ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORY_H__
#define __ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORY_H__

#include "_Elastosx_Net_Ssl_CTrustManagerFactory.h"
#include "TrustManagerFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CTrustManagerFactory)
    , public TrustManagerFactory
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORY_H__
