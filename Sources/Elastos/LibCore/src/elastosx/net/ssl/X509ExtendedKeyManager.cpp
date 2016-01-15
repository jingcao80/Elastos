
#include "X509ExtendedKeyManager.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL_3(X509ExtendedKeyManager, Object, IX509ExtendedKeyManager, IX509KeyManager, IKeyManager)

X509ExtendedKeyManager::X509ExtendedKeyManager()
{}

ECode X509ExtendedKeyManager::ChooseEngineClientAlias(
    /* [in] */ ArrayOf<String>* keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* alias)
{
    VALIDATE_NOT_NULL(alias)

    *alias = NULL;
    return NOERROR;
}

ECode X509ExtendedKeyManager::ChooseEngineServerAlias(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* alias)
{
    VALIDATE_NOT_NULL(alias)

    *alias = NULL;
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
