
#ifndef __ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORYHELPER_H__
#define __ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORYHELPER_H__

#include "_Elastosx_Net_Ssl_CTrustManagerFactoryHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Security::IProvider;

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CTrustManagerFactoryHelper)
    , public Singleton
    , public ITrustManagerFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDefaultAlgorithm(
        /* [out] */ String* algorithm);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ ITrustManagerFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ ITrustManagerFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ ITrustManagerFactory** factory);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CTRUSTMANAGERFACTORYHELPER_H__
