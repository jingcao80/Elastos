
#ifndef __ORG_CONSCRYPT_OPENSSLDSAKEYFACTORY_H__
#define __ORG_CONSCRYPT_OPENSSLDSAKEYFACTORY_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
// TODO: Need KeyFactorySpi
// #include <elastos/security/KeyFactorySpi.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::Security::IKey;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
// TODO: Need KeyFactorySpi
// using Elastos::Security::KeyFactorySpi;
using Elastos::Security::Spec::IKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDSAKeyFactory
    : public IOpenSSLDSAKeyFactory
    , public Object
// TODO: Need KeyFactorySpi
    // , public KeyFactorySpi
{
public:
    CAR_INTERFACE_DECL()

    CARAPI EngineGeneratePublic(
       /* [in] */ IKeySpec* keySpec,
       /* [out] */ IPublicKey** result);

    CARAPI EngineGeneratePrivate(
        /* [in] */ IKeySpec* keySpec,
        /* [out] */ IPrivateKey** result);

    CARAPI EngineGetKeySpec(
        /* [in] */ IKey* key,
        /* [in] */ ClassID keySpec,
        /* [out] */ IKeySpec** result);

    CARAPI EngineTranslateKey(
        /* [in] */ IKey* key,
        /* [out] */ IKey** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDSAKEYFACTORY_H__
