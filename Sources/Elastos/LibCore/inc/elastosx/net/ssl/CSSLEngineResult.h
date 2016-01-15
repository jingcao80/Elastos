
#ifndef __ELASTOSX_NET_SSL_CSSLENGINERESULT_H__
#define __ELASTOSX_NET_SSL_CSSLENGINERESULT_H__

#include "_Elastosx_Net_Ssl_CSSLEngineResult.h"
#include "SSLEngineResult.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLEngineResult)
    , public SSLEngineResult
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLENGINERESULT_H__
