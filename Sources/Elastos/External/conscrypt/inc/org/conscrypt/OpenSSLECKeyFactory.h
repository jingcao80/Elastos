
#ifndef __ORG_CONSCRYPT_OPENSSLECKEYFACTORY_H__
#define __ORG_CONSCRYPT_OPENSSLECKEYFACTORY_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
// TODO: Need KeyFactorySpi
// #include <elastos/security/KeyFactorySpi.h>
#include "_Org.Conscrypt.h"

using Elastos::Security::IKey;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
// TODO: Need KeyFactorySpi
// using Elastos::Security::KeyFactorySpi;
using Elastos::Security::Spec::IKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLECKeyFactory
    : public IOpenSSLECKeyFactory
    , public Object
// TODO: Need KeyFactorySpi
    // , public KeyFactorySpi
{
public:
    CAR_INTERFACE_DECL()

    constructor();

    CARAPI EngineGeneratePublic(
       /* [in] */ IKeySpec* keySpec,
       /* [out] */ IPublicKey** pubKey);

    CARAPI EngineGeneratePrivate(
        /* [in] */ IKeySpec* keySpec,
        /* [out] */ IPrivateKey** priKey);

    CARAPI EngineGetKeySpec(
        /* [in] */ IKey* key,
        /* [in] */ ClassID keySpec,
        /* [out] */ IKeySpec** retkeySpec);

    CARAPI EngineTranslateKey(
        /* [in] */ IKey* key,
        /* [out] */ IKey** translatedKey);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLECKEYFACTORY_H__
