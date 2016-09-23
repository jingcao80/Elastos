
#ifndef __ORG_CONSCRYPT_KEYMANAGERIMPL_H__
#define __ORG_CONSCRYPT_KEYMANAGERIMPL_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include "elastosx/net/ssl/X509ExtendedKeyManager.h"
#include "_Org.Conscrypt.h"

using Elastos::Net::ISocket;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IKeyStore;
using Elastos::Security::IPrincipal;
using Elastos::Security::IPrivateKey;
using Elastos::Utility::IHashTable;
using Elastosx::Net::Ssl::ISSLEngine;
using Elastosx::Net::Ssl::X509ExtendedKeyManager;

namespace Org {
namespace Conscrypt {

/**
 * KeyManager implementation.
 *
 * This implementation uses hashed key store information. It works faster than retrieving all of the
 * data from the key store. Any key store changes, that happen after key manager was created, have
 * no effect. The implementation does not use peer information (host, port) that may be obtained
 * from socket or engine.
 *
 * @see javax.net.ssl.KeyManager
 *
 */
class KeyManagerImpl
    : public IKeyManagerImpl
    , public X509ExtendedKeyManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates Key manager
     *
     * @param keyStore
     * @param pwd
     */
    CARAPI constructor(
        /* [in] */ IKeyStore* keyStore,
        /* [in] */ ArrayOf<Char32>* pwd);

    // @Override
    CARAPI ChooseClientAlias(
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISocket* socket,
        /* [out] */ String* result);

    // @Override
    CARAPI ChooseServerAlias(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISocket* socket,
        /* [out] */ String* result);

    // @Override
    CARAPI GetCertificateChain(
        /* [in] */ const String& alias,
        /* [out] */ ArrayOf<IX509Certificate*>** result);

    // @Override
    CARAPI GetClientAliases(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [out] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetServerAliases(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [out] */ ArrayOf<String>** result);

    // @Override
    CARAPI GetPrivateKey(
        /* [in] */ const String& alias,
        /* [out] */ IPrivateKey** result);

    // @Override
    CARAPI ChooseEngineClientAlias(
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* result);

    // @Override
    CARAPI ChooseEngineServerAlias(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* result);

private:
    CARAPI_(AutoPtr<ArrayOf<String> >) ChooseAlias(
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal*>* issuers);

private:
    // hashed key store information
    AutoPtr<IHashTable/*<String, PrivateKeyEntry>*/> mHash;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_KEYMANAGERIMPL_H__
