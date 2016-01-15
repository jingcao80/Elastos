
#ifndef __ELASTOSX_NET_SSL_CKEYMANAGERFACTORYHELPER_H__
#define __ELASTOSX_NET_SSL_CKEYMANAGERFACTORYHELPER_H__

#include "_Elastosx_Net_Ssl_CKeyManagerFactoryHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Security::IProvider;

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CKeyManagerFactoryHelper)
    , public Singleton
    , public IKeyManagerFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDefaultAlgorithm(
        /* [out] */ String* algorithm);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [out] */ IKeyManagerFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ const String& provider,
        /* [out] */ IKeyManagerFactory** factory);

    CARAPI GetInstance(
        /* [in] */ const String& algorithm,
        /* [in] */ IProvider* provider,
        /* [out] */ IKeyManagerFactory** factory);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CKEYMANAGERFACTORYHELPER_H__
