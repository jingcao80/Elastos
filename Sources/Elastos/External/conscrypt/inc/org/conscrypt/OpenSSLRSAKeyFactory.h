
#ifndef __ORG_CONSCRYPT_OPENSSLRSAKEYFACTORY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAKEYFACTORY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Spec::IKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAKeyFactory
    : public Object    // : public KeyFactorySpi
{
public:
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

#endif //__ORG_CONSCRYPT_OPENSSLRSAKEYFACTORY_H__