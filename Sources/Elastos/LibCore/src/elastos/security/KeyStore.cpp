
#include "KeyStore.h"
#include "CSecurity.h"
#include "org/apache/harmony/security/fortress/CEngine.h"

using Org::Apache::Harmony::Security::Fortress::CEngine;
using Org::Apache::Harmony::Security::Fortress::ISpiAndProvider;

namespace Elastos {
namespace Security {

static AutoPtr<IEngine> InitEngine()
{
    AutoPtr<CEngine> e;
    CEngine::NewByFriend(String("KeyStore")/*SERVICE*/, (CEngine**)&e);
    return e;
}

const String KeyStore::SERVICE("KeyStore");

AutoPtr<IEngine> KeyStore::ENGINE = InitEngine();

const String KeyStore::PROPERTYNAME("keystore.type");

//  Store default KeyStore type
const String KeyStore::DEFAULT_KEYSTORE_TYPE("jks");

CAR_INTERFACE_IMPL(KeyStore, Object, IKeyStore)

KeyStore::KeyStore(
    /* [in] */ IKeyStoreSpi *keyStoreSpi,
    /* [in] */ IProvider *provider,
    /* [in] */ const String& type)
    : mIsInit(FALSE)
    , mImplSpi(keyStoreSpi)
    , mProvider(provider)
    , mType(type)
{}

ECode KeyStore::ThrowNotInitialized()
{
    return E_KEY_STORE_EXCEPTION;
}

ECode KeyStore::GetInstance(
    /* [in] */ const String& type,
    /* [out] */ IKeyStore **instance)
{
    if (type.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
    AutoPtr<ISpiAndProvider> sap;
    ENGINE->GetInstance(type, NULL, (ISpiAndProvider**)&sap);
    AutoPtr<IInterface> spi;
    sap->GetSpi((IInterface**)&spi);
    AutoPtr<IProvider> provider;
    sap->GetProvider((IProvider**)&provider);
    *instance = new KeyStore(IKeyStoreSpi::Probe(spi), provider, type);
    REFCOUNT_ADD(*instance);
    // } catch (NoSuchAlgorithmException e) {
    //     throw new KeyStoreException(e);
    // }
    return NOERROR;
}

ECode KeyStore::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ const String& provider,
    /* [out] */ IKeyStore **instance)
{
    if (provider.IsNullOrEmpty()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IProvider> impProvider;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProvider(provider, (IProvider**)&impProvider);
    if (impProvider == NULL) {
        return E_NO_SUCH_PROVIDER_EXCEPTION;
    }
    return GetInstance(type, impProvider, instance);
}

ECode KeyStore::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IProvider *provider,
    /* [out] */ IKeyStore **instance)
{
    // check parameters
    if (provider == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (type.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    //Apache...Todo later
    /*
    // return KeyStore instance
    try {
        Object spi = ENGINE.getInstance(type, provider, null);
        return new KeyStore((KeyStoreSpi) spi, provider, type);
    } catch (Exception e) {
        // override exception
        throw new KeyStoreException(e);
    }
    */
    return E_NOT_IMPLEMENTED;
}

ECode KeyStore::GetDefaultType(
    /* [out] */ String *defaultType)
{
    VALIDATE_NOT_NULL(defaultType)
    String dt;
    AutoPtr<ISecurity> security;
    CSecurity::AcquireSingleton((ISecurity**)&security);
    security->GetProperty(PROPERTYNAME, &dt);
    *defaultType = dt.IsNull() ? DEFAULT_KEYSTORE_TYPE : dt;
    return NOERROR;
}

ECode KeyStore::GetProvider(
    /* [out] */ IProvider **provider)
{
    VALIDATE_NOT_NULL(provider)
    *provider = mProvider;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode KeyStore::GetType(
    /* [out] */ String *type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode KeyStore::GetKey(
    /* [in] */ const String& alias,
    /* [in] */ ArrayOf<Char32> *password,
    /* [out] */ IKey **key)
{
    if (!mIsInit) {
        return ThrowNotInitialized();
    }
    return mImplSpi->EngineGetKey(alias, password, key);
}

ECode KeyStore::GetCertificateChain(
    /* [in] */ const String& alias,
    /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate*> **certChain)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineGetCertificateChain(alias, certChain);
}

ECode KeyStore::GetCertificate(
    /* [in] */ const String& alias,
    /* [out] */ Elastos::Security::Cert::ICertificate **cert)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineGetCertificate(alias, cert);
}

ECode KeyStore::GetCreationDate(
    /* [in] */ const String& alias,
    /* [out] */ IDate **creationDate)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineGetCreationDate(alias, creationDate);
}

ECode KeyStore::SetKeyEntry(
    /* [in] */ const String& alias,
    /* [in] */ IKey *key,
    /* [in] */ ArrayOf<Char32> *password,
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }

    // Certificate chain is required for PrivateKey
    if (key != NULL && key->Probe(EIID_IPrivateKey) && (chain == NULL || !chain->GetLength())) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mImplSpi->EngineSetKeyEntry(alias, key, password, chain);
}

ECode KeyStore::SetKeyEntryEx(
    /* [in] */ const String& alias,
    /* [in] */ ArrayOf<Byte> *key,
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineSetKeyEntry(alias, key, chain);
}

ECode KeyStore::SetCertificateEntry(
    /* [in] */ const String& alias,
    /* [in] */ Elastos::Security::Cert::ICertificate *cert)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineSetCertificateEntry(alias, cert);
}

ECode KeyStore::DeleteEntry(
    /* [in] */ const String& alias)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineDeleteEntry(alias);
}

ECode KeyStore::Aliases(
    /* [out] */ IEnumeration **enumeration)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineAliases(enumeration);
}

ECode KeyStore::ContainsAlias(
    /* [in] */ const String& alias,
    /* [out] */ Boolean *result)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineContainsAlias(alias, result);
}

ECode KeyStore::GetSize(
    /* [out] */ Int32 *size)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineSize(size);
}

ECode KeyStore::IsKeyEntry(
    /* [in] */ const String& alias,
    /* [out] */ Boolean *result)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineIsKeyEntry(alias, result);
}

ECode KeyStore::IsCertificateEntry(
    /* [in] */ const String& alias,
    /* [out] */ Boolean *result)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineIsCertificateEntry(alias, result);
}

ECode KeyStore::GetCertificateAlias(
    /* [in] */ Elastos::Security::Cert::ICertificate *cert,
    /* [out] */ String *alias)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineGetCertificateAlias(cert, alias);
}

ECode KeyStore::Store(
    /* [in] */ IOutputStream *stream,
    /* [in] */ ArrayOf<Char32> *password)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }

    //Just delegate stream and password to implSpi
    return mImplSpi->EngineStore(stream, password);
}

ECode KeyStore::StoreEx(
    /* [in] */ IKeyStoreLoadStoreParameter *param)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineStore(param);
}

ECode KeyStore::Load(
    /* [in] */ IInputStream *stream,
    /* [in] */ ArrayOf<Char32> *password)
{
    FAIL_RETURN(mImplSpi->EngineLoad(stream, password))
    mIsInit = TRUE;
    return NOERROR;
}

ECode KeyStore::LoadEx(
    /* [in] */ IKeyStoreLoadStoreParameter *param)
{
    FAIL_RETURN(mImplSpi->EngineLoad(param))
    mIsInit = TRUE;
    return NOERROR;
}

ECode KeyStore::GetEntry(
    /* [in] */ const String& alias,
    /* [in] */ IKeyStoreProtectionParameter *param,
    /* [out] */ IKeyStoreEntry **entry)
{
    if (alias.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    return mImplSpi->EngineGetEntry(alias, param, entry);
}

ECode KeyStore::SetEntry(
    /* [in] */ const String& alias,
    /* [in] */ IKeyStoreEntry *entry,
    /* [in] */ IKeyStoreProtectionParameter *param)
{
    if (!mIsInit) {
        return E_KEY_STORE_EXCEPTION;
    }
    if (!alias || !entry) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return mImplSpi->EngineSetEntry(alias, entry, param);
}

ECode KeyStore::EntryInstanceOf(
    /* [in] */ const String& alias,
    /* [in] */ const ClassID& entryClass,
    /* [out] */ Boolean *result)
{
    if (alias.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    //Todo later
    /*
    if (entryClas == null) {
        throw new NullPointerException("entryClass == null");
    }

    if (!isInit) {
        throwNotInitialized();
    }
    return implSpi.engineEntryInstanceOf(alias, entryClass);
    */
    return E_NOT_IMPLEMENTED;
}

Boolean KeyStore::IsInit()
{
    return mIsInit;
}

} // namespace Security
} // namespace Elastos