#include "elastos/droid/keystore/security/CKeyChain.h"
#include "elastos/droid/keystore/security/KeyChain.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(CKeyChain, Singleton, IKeyChain)
CAR_SINGLETON_IMPL(CKeyChain)

ECode CKeyChain::CreateInstallIntent(
    /* [out]*/  IIntent** intent)
{
    return KeyChain::CreateInstallIntent(intent);
}

ECode CKeyChain::ChoosePrivateKeyAlias(
    /* [in] */ IActivity* activity,
    /* [in] */ IKeyChainAliasCallback* response,
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& alias)
{
    return KeyChain::ChoosePrivateKeyAlias(activity, response, keyTypes, issuers, host, port, alias);
}

ECode CKeyChain::GetPrivateKey(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& alias,
    /* [out] */ IPrivateKey** result)
{
    return KeyChain::GetPrivateKey(ctx, alias, result);
}

ECode CKeyChain::GetCertificateChain(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& alias,
    /* [out, callee] */ ArrayOf<IX509Certificate*>** chain)
{
    return KeyChain::GetCertificateChain(ctx, alias, chain);
}

ECode CKeyChain::IsKeyAlgorithmSupported(
    /* [in] */ const String& algorithm,
    /* [out] */ Boolean* sopported)
{
    return KeyChain::IsKeyAlgorithmSupported(algorithm, sopported);
}

ECode CKeyChain::IsBoundKeyAlgorithm(
    /* [in] */ const String& algorithm,
    /* [out] */ Boolean* ret)
{
    return KeyChain::IsBoundKeyAlgorithm(algorithm, ret);
}

ECode CKeyChain::ToCertificate(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ IX509Certificate** certificate)
{
    return KeyChain::ToCertificate(bytes, certificate);
}

ECode CKeyChain::Bind(
    /* [in] */ IContext* ctx,
    /* [out] */ IKeyChainConnection** connection)
{
    return KeyChain::Bind(ctx, connection);
}

ECode CKeyChain::BindAsUser(
    /* [in] */ IContext* ctx,
    /* [in] */ IUserHandle* user,
    /* [out] */ IKeyChainConnection** connection)
{
    return KeyChain::BindAsUser(ctx, user, connection);
}


} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos
