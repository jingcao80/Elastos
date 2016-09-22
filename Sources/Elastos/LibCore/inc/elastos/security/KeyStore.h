
#ifndef __ELASTOS_SECURITY_KEYSTORE_H__
#define __ELASTOS_SECURITY_KEYSTORE_H__

#include "Elastos.CoreLibrary.Security.h"
#include "core/Object.h"

using Elastos::Core::Object;
using Elastos::Utility::IDate;
using Elastos::Utility::IEnumeration;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Security {

class KeyStore
    : public Object
    , public IKeyStore
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [out] */ IKeyStore **instance);

    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ const String& provider,
        /* [out] */ IKeyStore **instance);

    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider *provider,
        /* [out] */ IKeyStore **instance);

    static CARAPI GetDefaultType(
        /* [out] */ String *defaultType);

    CARAPI GetProvider(
        /* [out] */ IProvider **provider) const;

    CARAPI GetType(
        /* [out] */ String *type) const;

    CARAPI GetKey(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Char32> *password,
        /* [out] */ IKey **key) const;

    CARAPI GetCertificateChain(
        /* [in] */ const String& alias,
        /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate*> **certChain);

    CARAPI GetCertificate(
        /* [in] */ const String& alias,
        /* [out] */ Elastos::Security::Cert::ICertificate **cert);

    CARAPI GetCreationDate(
        /* [in] */ const String& alias,
        /* [out] */ IDate **creationDate);

    CARAPI SetKeyEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKey *key,
        /* [in] */ ArrayOf<Char32> *password,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain);

    CARAPI SetKeyEntryEx(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Byte> *key,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain);

    CARAPI SetCertificateEntry(
        /* [in] */ const String& alias,
        /* [in] */ Elastos::Security::Cert::ICertificate *cert);

    CARAPI DeleteEntry(
        /* [in] */ const String& alias);

    CARAPI Aliases(
        /* [out] */ IEnumeration **enumeration);

    CARAPI ContainsAlias(
        /* [in] */ const String& alias,
        /* [out] */ Boolean *result);

    CARAPI GetSize(
        /* [out] */ Int32 *size);

    CARAPI IsKeyEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean *result);

    CARAPI IsCertificateEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean *result);

    CARAPI GetCertificateAlias(
        /* [in] */ Elastos::Security::Cert::ICertificate *cert,
        /* [out] */ String *alias);

    CARAPI Store(
        /* [in] */ IOutputStream *stream,
        /* [in] */ ArrayOf<Char32> *password);

    CARAPI StoreEx(
        /* [in] */ IKeyStoreLoadStoreParameter *param);

    CARAPI Load(
        /* [in] */ IInputStream *stream,
        /* [in] */ ArrayOf<Char32> *password);

    CARAPI LoadEx(
        /* [in] */ IKeyStoreLoadStoreParameter *param);

    CARAPI GetEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKeyStoreProtectionParameter *param,
        /* [out] */ IKeyStoreEntry **entry);

    CARAPI SetEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKeyStoreEntry *entry,
        /* [in] */ IKeyStoreProtectionParameter *param);

    CARAPI EntryInstanceOf(
        /* [in] */ const String& alias,
        /* [in] */ const ClassID& entryClass,
        /* [out] */ Boolean *result);

    CARAPI_(Boolean) IsInit();

protected:
    /**
     * Constructs a new instance of {@code KeyStore} with the given arguments.
     *
     * @param keyStoreSpi
     *            the concrete key store.
     * @param provider
     *            the provider.
     * @param type
     *            the type of the {@code KeyStore} to be constructed.
     */
    KeyStore(
        /* [in] */ IKeyStoreSpi *keyStoreSpi,
        /* [in] */ IProvider *provider,
        /* [in] */ const String& type);

private:
    /**
     * Throws the standard "keystore not initialized" exception.
     */
    static CARAPI ThrowNotInitialized();

private:
    // Store KeyStore SERVICE name
    static const String SERVICE;// = "KeyStore";

    //Apache...Todo later
    // Used to access common engine functionality
    //private static final Engine ENGINE = new Engine(SERVICE);

    //  Store KeyStore property name
    static const String PROPERTYNAME;// = "keystore.type";

    //  Store default KeyStore type
    static const String DEFAULT_KEYSTORE_TYPE;// = "jks";

    // Store KeyStore state (initialized or not)
    Boolean mIsInit;

    // Store used KeyStoreSpi
    const AutoPtr<IKeyStoreSpi> mImplSpi;

    // Store used provider
    const AutoPtr<IProvider> mProvider;

    // Store used type
    const String mType;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_KEYSTORE_H__
