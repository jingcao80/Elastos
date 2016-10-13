
#ifndef __ORG_CONSCRYPT_OPENSSLDHKEYFACTORY_H__
#define __ORG_CONSCRYPT_OPENSSLDHKEYFACTORY_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Extensions.h>
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

class OpenSSLDHKeyFactory
    : public IOpenSSLDHKeyFactory
    , public Object
// TODO: Need KeyFactorySpi
    // , public KeyFactorySpi
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

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

#endif //__ORG_CONSCRYPT_OPENSSLDHKEYFACTORY_H__
