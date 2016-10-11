#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYCHAIN_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYCHAIN_H__

#include "_Elastos_Droid_KeyStore_Security_CKeyChain.h"
#include "elastos/droid/keystore/security/KeyChain.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CKeyChain)
    , public Singleton
    , public IKeyChain
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI CreateInstallIntent(
        /* [out]*/  IIntent** intent);

    CARAPI ChoosePrivateKeyAlias(
        /* [in] */ IActivity* activity,
        /* [in] */ IKeyChainAliasCallback* response,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& alias);

    CARAPI GetPrivateKey(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& alias,
        /* [out] */ IPrivateKey** result);

    CARAPI GetCertificateChain(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& alias,
        /* [out, callee] */ ArrayOf<IX509Certificate*>** chain);

    CARAPI IsKeyAlgorithmSupported(
        /* [in] */ const String& algorithm,
        /* [out] */ Boolean* sopported);

    CARAPI IsBoundKeyAlgorithm(
        /* [in] */ const String& algorithm,
        /* [out] */ Boolean* ret);

    CARAPI ToCertificate(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ IX509Certificate** certificate);

    CARAPI Bind(
        /* [in] */ IContext* ctx,
        /* [out] */ IKeyChainConnection** connection);

    CARAPI BindAsUser(
        /* [in] */ IContext* ctx,
        /* [in] */ IUserHandle* user,
        /* [out] */ IKeyChainConnection** connection);
};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CKEYCHAIN_H__
