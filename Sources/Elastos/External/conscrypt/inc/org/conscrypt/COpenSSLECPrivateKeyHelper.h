
#ifndef __ORG_CONSCRYPT_COPENSSLECPRIVATEKEYHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLECPRIVATEKEYHELPER_H__

#include "_Org_Conscrypt_COpenSSLECPrivateKeyHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Security::Interfaces::IECPrivateKey;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECPrivateKeyHelper)
    , public Singleton
    , public IOpenSSLECPrivateKeyHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI WrapPlatformKey(
        /* [in] */ IECPrivateKey* ecPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    CARAPI GetInstance(
        /* [in] */ IECPrivateKey* ecPrivateKey,
        /* [out] */ IOpenSSLKey** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECPRIVATEKEYHELPER_H__
