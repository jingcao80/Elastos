
#ifndef __ELASTOSX_NET_SSL_X509EXTENDEDKEYMANAGER_H__
#define __ELASTOSX_NET_SSL_X509EXTENDEDKEYMANAGER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::IPrincipal;
using Elastosx::Net::Ssl::ISSLEngine;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::IX509KeyManager;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The abstract extension for the {@code X509KeyManager} interface.
 */
class X509ExtendedKeyManager
    : public Object
    , public IX509ExtendedKeyManager
    , public IX509KeyManager
    , public IKeyManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Chooses an alias for the client side of an SSL connection to authenticate
     * it with the specified public key type and certificate issuers.
     *
     * @param keyType
     *            the list of public key algorithm names.
     * @param issuers
     *            the list of certificate issuers, or {@code null} if any issuer
     *            will do.
     * @param engine
     *            the {@code SSLEngine} for the connection, or {@code null} if
     *            no engine is predefined.
     * @return the alias name of a matching key or {@code null} if there are no
     *         matches.
     */
    CARAPI ChooseEngineClientAlias(
        /* [in] */ ArrayOf<String>* keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* alias);

    /**
     * Chooses an alias for the server side of an SSL connection to authenticate
     * it with the specified public key type and certificate issuers.
     *
     * @param keyType
     *            the list of public key algorithm names.
     * @param issuers
     *            the list of certificate issuers, or {@code null} if any issuer
     *            will do.
     * @param engine
     *            the {@code SSLEngine} for the connection, or {@code null} if
     *            no engine is predefined.
     * @return the alias name of a matching key or {@code null} if there are no
     *         matches.
     */
    CARAPI ChooseEngineServerAlias(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* alias);

protected:
    /**
     * To be used by subclasses only.
     * <p>
     * Creates a new {@code X509ExtendedKeyManager} instance.
     */
    X509ExtendedKeyManager();
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_X509EXTENDEDKEYMANAGER_H__
