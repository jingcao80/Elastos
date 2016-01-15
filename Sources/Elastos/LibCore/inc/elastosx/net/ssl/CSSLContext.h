
#ifndef __ELASTOSX_NET_SSL_CSSLCONTEXT_H__
#define __ELASTOSX_NET_SSL_CSSLCONTEXT_H__

#include "_Elastosx_Net_Ssl_CSSLContext.h"
#include "SSLContext.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLContext)
    , public SSLContext
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLCONTEXT_H__
