#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYSTORE_H__
#define _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYSTORE_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.Droid.Security.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/security/KeyStoreSpi.h"

using Elastos::Security::IKey;
using Elastos::Security::IKeyStoreEntry;
using Elastos::Security::IKeyStoreProtectionParameter;
using Elastos::Security::KeyStoreSpi;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::ICollection;
using Elastos::Utility::IDate;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * A java.security.KeyStore interface for the Android KeyStore. An instance of
 * it can be created via the {@link java.security.KeyStore#getInstance(String)
 * KeyStore.getInstance("AndroidKeyStore")} interface. This returns a
 * java.security.KeyStore backed by this "AndroidKeyStore" implementation.
 * <p>
 * This is built on top of Android's keystore daemon. The convention of alias
 * use is:
 * <p>
 * PrivateKeyEntry will have a Credentials.USER_PRIVATE_KEY as the private key,
 * Credentials.USER_CERTIFICATE as the first certificate in the chain (the one
 * that corresponds to the private key), and then a Credentials.CA_CERTIFICATE
 * entry which will have the rest of the chain concatenated in BER format.
 * <p>
 * TrustedCertificateEntry will just have a Credentials.CA_CERTIFICATE entry
 * with a single certificate.
 *
 * @hide
 */
class ElastosKeyStore
    : public KeyStoreSpi
    , public IElastosKeyStore
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();

    CARAPI EngineGetKey(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Char32>* password,
        /* [out] */ IKey** key);

    CARAPI EngineGetCertificateChain(
        /* [in] */ const String& alias,
        /* [out, callee] */ ArrayOf<ICertificate*>** certChain);

    CARAPI EngineGetCertificate(
        /* [in] */ const String& alias,
        /* [out] */ ICertificate** cert);

    CARAPI EngineGetCreationDate(
        /* [in] */ const String& alias,
        /* [out] */ IDate** creationDate);

    CARAPI EngineSetKeyEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKey* key,
        /* [in] */ ArrayOf<Char32>* password,
        /* [in] */ ArrayOf<ICertificate*>* chain);

    CARAPI EngineSetKeyEntry(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Byte>* key,
        /* [in] */ ArrayOf<ICertificate*>* chain);

    CARAPI EngineSetCertificateEntry(
        /* [in] */ const String& alias,
        /* [in] */ ICertificate* cert);

    CARAPI EngineDeleteEntry(
        /* [in] */ const String& alias);

    CARAPI EngineAliases(
        /* [out] */ IEnumeration** aliases);

    CARAPI EngineContainsAlias(
        /* [in] */ const String& alias,
        /* [out] */ Boolean* result);

    CARAPI EngineSize(
        /* [out] */ Int32* size);

    CARAPI EngineIsKeyEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean* result);

    CARAPI EngineIsCertificateEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean* result);

    CARAPI EngineGetCertificateAlias(
        /* [in] */ ICertificate* cert,
        /* [out] */ String* alias);

    CARAPI EngineStore(
        /* [in] */ IOutputStream* stream,
        /* [in] */ ArrayOf<Char32>* password);

    CARAPI EngineLoad(
        /* [in] */ IInputStream* stream,
        /* [in] */ ArrayOf<Char32>* password);

    CARAPI EngineSetEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKeyStoreEntry* entry,
        /* [in] */ IKeyStoreProtectionParameter* protParam);

private:
    static CARAPI_(AutoPtr<IX509Certificate>) ToCertificate(
        /* [in] */ ArrayOf<Byte>* bytes);

    //@SuppressWarnings("unchecked") Collection<X509Certificate>
    static CARAPI_(AutoPtr<ICollection>) ToCertificates(
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI_(AutoPtr<IDate>) GetModificationDate(
        /* [in] */ const String& alias);

    CARAPI SetPrivateKeyEntry(
        /* [in] */ const String& alias,
        /* [in] */ IPrivateKey* key,
        /* [in] */ ArrayOf<ICertificate*>* chain,
        /* [in] */ IKeyStoreParameter* params);

    //Set<String>
    CARAPI_(AutoPtr<ISet>) GetUniqueAliases();

    CARAPI_(Boolean) IsKeyEntry(
        /* [in] */ const String& alias);

    CARAPI_(Boolean) IsCertificateEntry(
        /* [in] */ const String& alias);

private:
    AutoPtr<IKeyStore> mKeyStore;

};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYSTORE_H__
