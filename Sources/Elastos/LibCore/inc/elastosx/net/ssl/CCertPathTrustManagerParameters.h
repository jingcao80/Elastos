
#ifndef __ELASTOSX_NET_SSL_CCERTPATHTRUSTMANAGERPARAMETERS_H__
#define __ELASTOSX_NET_SSL_CCERTPATHTRUSTMANAGERPARAMETERS_H__

#include "_Elastosx_Net_Ssl_CCertPathTrustManagerParameters.h"
#include "CertPathTrustManagerParameters.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CCertPathTrustManagerParameters)
    , public CertPathTrustManagerParameters
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CCERTPATHTRUSTMANAGERPARAMETERS_H__
