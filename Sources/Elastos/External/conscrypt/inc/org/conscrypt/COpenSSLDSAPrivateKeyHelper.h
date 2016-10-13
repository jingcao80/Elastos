
#ifndef __ORG_CONSCRYPT_COPENSSLDSAPRIVATEKEYHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLDSAPRIVATEKEYHELPER_H__

#include "_Org_Conscrypt_COpenSSLDSAPrivateKeyHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Security::Interfaces::IDSAPrivateKey;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLDSAPrivateKeyHelper)
    , public Singleton
    , public IOpenSSLDSAPrivateKeyHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ IDSAPrivateKey* dsaPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    CARAPI WrapPlatformKey(
        /* [in] */ IDSAPrivateKey* dsaPrivateKey,
        /* [out] */ IOpenSSLKey** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLDSAPRIVATEKEYHELPER_H__
