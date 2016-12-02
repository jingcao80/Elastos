#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Org.Conscrypt.h"
#include "elastos/droid/keystore/security/ElastosKeyStore.h"
#include "elastos/droid/keystore/security/Credentials.h"
#include "elastos/droid/keystore/security/KeyStore.h"
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Logger.h>

using Org::Conscrypt::IOpenSSLEngine;
using Org::Conscrypt::IOpenSSLEngineHelper;
using Org::Conscrypt::COpenSSLEngineHelper;
using Org::Conscrypt::IOpenSSLKey;
using Org::Conscrypt::IOpenSSLKeyHolder;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IInputStream;
using Elastos::Security::IKeyStoreTrustedCertificateEntry;
using Elastos::Security::IKeyStorePrivateKeyEntry;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CDate;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(ElastosKeyStore, KeyStoreSpi, IElastosKeyStore);

ECode ElastosKeyStore::constructor()
{
    return NOERROR;
}

ECode ElastosKeyStore::EngineGetKey(
    /* [in] */ const String& alias,
    /* [in] */ ArrayOf<Char32>* password,
    /* [out] */ IKey** key)
{
    VALIDATE_NOT_NULL(key);
    *key = NULL;
    if (!IsKeyEntry(alias)) {
        return NOERROR;
    }

    AutoPtr<IOpenSSLEngine> engine;
    AutoPtr<IOpenSSLEngineHelper> oseHelper;
    COpenSSLEngineHelper::AcquireSingleton((IOpenSSLEngineHelper**)&oseHelper);
    oseHelper->GetInstance(String("keystore"), (IOpenSSLEngine**)&engine);
    //try {
    AutoPtr<IPrivateKey> _key;
    ECode ec = engine->GetPrivateKeyById(Credentials::USER_PRIVATE_KEY + alias, (IPrivateKey**)&_key);
    //} catch (InvalidKeyException e) {
    if (ec == (ECode)E_INVALID_KEY_EXCEPTION) {
        //UnrecoverableKeyException t = new UnrecoverableKeyException("Can't get key");
        //t.initCause(e);
        //throw t;
        Logger::E("ElastosKeyStore", "Can't get key");
        return E_INVALID_KEY_EXCEPTION;
    }
    *key = IKey::Probe(_key);
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode ElastosKeyStore::EngineGetCertificateChain(
    /* [in] */ const String& alias,
    /* [out, callee] */ ArrayOf<ICertificate*>** certChain)
{
    VALIDATE_NOT_NULL(certChain);
    *certChain = NULL;
    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("ElastosKeyStore", "EngineGetCertificateChain, alias == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ICertificate> certificate;
    EngineGetCertificate(alias, (ICertificate**)&certificate);
    AutoPtr<IX509Certificate> leaf = IX509Certificate::Probe(certificate);
    if (leaf == NULL) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<ICertificate*> > caList;

    AutoPtr<ArrayOf<Byte> > caBytes;
    mKeyStore->Get(Credentials::CA_CERTIFICATE + alias, (ArrayOf<Byte>**)&caBytes);
    if (caBytes != NULL) {
        //Collection<X509Certificate> caChain;
        AutoPtr<ICollection> caChain = ToCertificates(caBytes);

        Int32 size;
        caChain->GetSize(&size);
        caList = ArrayOf<ICertificate*>::Alloc(size + 1);

        //final Iterator<X509Certificate> it = caChain.iterator();
        AutoPtr<IIterator> it;
        caChain->GetIterator((IIterator**)&it);
        Int32 i = 1;
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            //caList[i++] = it.next();
            caList->Set(i++, ICertificate::Probe(obj));
        }
    } else {
        caList = ArrayOf<ICertificate*>::Alloc(1);
    }

    caList->Set(0, certificate);

    *certChain = caList;
    REFCOUNT_ADD(*certChain);
    return NOERROR;
}

ECode ElastosKeyStore::EngineGetCertificate(
    /* [in] */ const String& alias,
    /* [out] */ ICertificate** cert)
{
    VALIDATE_NOT_NULL(cert);
    *cert = NULL;

    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("ElastosKeyStore", "EngineGetCertificate, alias == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > certificate;
    mKeyStore->Get(Credentials::USER_CERTIFICATE + alias, (ArrayOf<Byte>**)&certificate);
    if (certificate != NULL) {
        AutoPtr<IX509Certificate> x509 = ToCertificate(certificate);
        *cert = ICertificate::Probe(x509);
        REFCOUNT_ADD(*cert);
        return NOERROR;
    }

    mKeyStore->Get(Credentials::CA_CERTIFICATE + alias, (ArrayOf<Byte>**)&certificate);
    if (certificate != NULL) {
        AutoPtr<IX509Certificate> x509 = ToCertificate(certificate);
        *cert = ICertificate::Probe(x509);
        REFCOUNT_ADD(*cert);
        return NOERROR;
    }

    return NOERROR;
}

AutoPtr<IX509Certificate> ElastosKeyStore::ToCertificate(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    //try {
    AutoPtr<ICertificateFactory> certFactory;
    AutoPtr<ICertificateFactoryHelper> cfHelper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cfHelper);
    cfHelper->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(bytes, (IByteArrayInputStream**)&bais);
    AutoPtr<ICertificate> cert;
    certFactory->GenerateCertificate(IInputStream::Probe(bais), (ICertificate**)&cert);
    return IX509Certificate::Probe(cert);
    //} catch (CertificateException e) {
    //    Log.w(NAME, "Couldn't parse certificate in keystore", e);
    //    return null;
    //}
}

AutoPtr<ICollection> ElastosKeyStore::ToCertificates(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    //try {
    AutoPtr<ICertificateFactory> certFactory;
    AutoPtr<ICertificateFactoryHelper> cfHelper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cfHelper);
    cfHelper->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(bytes, (IByteArrayInputStream**)&bais);
    AutoPtr<ICollection> result;
    certFactory->GenerateCertificates(IInputStream::Probe(bais), (ICollection**)&result);
    //} catch (CertificateException e) {
    //    Log.w(NAME, "Couldn't parse certificates in keystore", e);
    //    return new ArrayList<X509Certificate>();
    //}
    return result;
}

AutoPtr<IDate> ElastosKeyStore::GetModificationDate(
    /* [in] */ const String& alias)
{
    Int64 epochMillis;
    mKeyStore->Getmtime(alias, &epochMillis);
    if (epochMillis == -1L) {
        return NULL;
    }

    AutoPtr<IDate> result;
    CDate::New(epochMillis, (IDate**)&result);
    return result;
}

ECode ElastosKeyStore::EngineGetCreationDate(
    /* [in] */ const String& alias,
    /* [out] */ IDate** creationDate)
{
    VALIDATE_NOT_NULL(creationDate);
    if (alias == NULL) {
        //throw new NullPointerException("alias == null");
        Logger::E("ElastosKeyStore", "EngineGetCreationDate alias == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IDate> d = GetModificationDate(Credentials::USER_PRIVATE_KEY + alias);
    if (d != NULL) {
        *creationDate = d;
        REFCOUNT_ADD(*creationDate);
        return NOERROR;
    }

    d = GetModificationDate(Credentials::USER_CERTIFICATE + alias);
    if (d != NULL) {
        *creationDate = d;
        REFCOUNT_ADD(*creationDate);
        return NOERROR;
    }

    d = GetModificationDate(Credentials::CA_CERTIFICATE + alias);
    *creationDate = d;
    REFCOUNT_ADD(*creationDate);
    return NOERROR;
}

ECode ElastosKeyStore::EngineSetKeyEntry(
    /* [in] */ const String& alias,
    /* [in] */ IKey* key,
    /* [in] */ ArrayOf<Char32>* password,
    /* [in] */ ArrayOf<ICertificate*>* chain)
{
    if ((password != NULL) && (password->GetLength() > 0)) {
        //throw new KeyStoreException("entries cannot be protected with passwords");
        Logger::E("ElastosKeyStore", "EngineSetKeyEntry entries cannot be protected with passwords");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (IPrivateKey::Probe(key) != NULL) {
        return SetPrivateKeyEntry(alias, IPrivateKey::Probe(key), chain, NULL);
    }
    else {
        //throw new KeyStoreException("Only PrivateKeys are supported");
        Logger::E("ElastosKeyStore", "EngineSetKeyEntry Only PrivateKeys are supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ElastosKeyStore::SetPrivateKeyEntry(
    /* [in] */ const String& alias,
    /* [in] */ IPrivateKey* key,
    /* [in] */ ArrayOf<ICertificate*>* chain,
    /* [in] */ IKeyStoreParameter* params)
{
    AutoPtr<ArrayOf<Byte> > keyBytes;

    String pkeyAlias;
    if (IOpenSSLKeyHolder::Probe(key) != NULL) {
        AutoPtr<IOpenSSLKey> opensslKey;
        IOpenSSLKeyHolder::Probe(key)->GetOpenSSLKey((IOpenSSLKey**)&opensslKey);
        opensslKey->GetAlias(&pkeyAlias);
    }
    else {
        pkeyAlias = String(NULL);
    }

    Boolean shouldReplacePrivateKey;
    if (!pkeyAlias.IsNull() && pkeyAlias.StartWith(Credentials::USER_PRIVATE_KEY)) {
        String keySubalias = pkeyAlias.Substring(Credentials::USER_PRIVATE_KEY.GetLength());
        if (!alias.Equals(keySubalias)) {
            //throw new KeyStoreException("Can only replace keys with same alias: " + alias + " != " + keySubalias);
            Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Can only replace keys with same alias:%s != %s", alias.string(), keySubalias.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        shouldReplacePrivateKey = FALSE;
    }
    else {
        // Make sure the PrivateKey format is the one we support.
        String keyFormat;
        IKey::Probe(key)->GetFormat(&keyFormat);
        if ((keyFormat.IsNull()) || (!String("PKCS#8").Equals(keyFormat))) {
            //throw new KeyStoreException( "Only PrivateKeys that can be encoded into PKCS#8 are supported");
            Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Only PrivateKeys that can be encoded into PKCS#8 are supported");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        // Make sure we can actually encode the key.
        IKey::Probe(key)->GetEncoded((ArrayOf<Byte>**)&keyBytes);
        if (keyBytes == NULL) {
            //throw new KeyStoreException("PrivateKey has no encoding");
            Logger::E("ElastosKeyStore", "SetPrivateKeyEntry PrivateKey has no encoding");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        shouldReplacePrivateKey = TRUE;
    }

    // Make sure the chain exists since this is a PrivateKey
    if ((chain == NULL) || (chain->GetLength() == 0)) {
        //throw new KeyStoreException("Must supply at least one Certificate with PrivateKey");
        Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Must supply at least one Certificate with PrivateKey");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Do chain type checking.
    AutoPtr<ArrayOf<IX509Certificate*> > x509chain = ArrayOf<IX509Certificate*>::Alloc(chain->GetLength());
    for (Int32 i = 0; i < chain->GetLength(); i++) {
        String type;
        if (!String("X.509").Equals(((*chain)[i]->GetType(&type), type))) {
            //throw new KeyStoreException("Certificates must be in X.509 format: invalid cert #" + i);
            Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Certificates must be in X.509 format: invalid cert #%d", i);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (IX509Certificate::Probe((*chain)[i]) == NULL) {
            //throw new KeyStoreException("Certificates must be in X.509 format: invalid cert #" + i);
            Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Certificates must be in X.509 format: invalid cert #%d", i);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        x509chain->Set(i, IX509Certificate::Probe((*chain)[i]));
    }

    AutoPtr<ArrayOf<Byte> > userCertBytes;
    //try {
    ECode ec = ICertificate::Probe((*x509chain)[0])->GetEncoded((ArrayOf<Byte>**)&userCertBytes);
    //} catch (CertificateEncodingException e) {
    if (ec == (ECode)E_CERTIFICATE_ENCODING_EXCEPTION) {
        //throw new KeyStoreException("Couldn't encode certificate #1", e);
        Logger::E("ElastosKeyStore", "Couldn't encode certificate #1");
        return E_KEY_STORE_EXCEPTION;
    }

    /*
     * If we have a chain, store it in the CA certificate slot for this
     * alias as concatenated DER-encoded certificates. These can be
     * deserialized by {@link CertificateFactory#generateCertificates}.
     */
    AutoPtr<ArrayOf<Byte> > chainBytes;
    if (chain->GetLength() > 1) {
        /*
         * The chain is passed in as {user_cert, ca_cert_1, ca_cert_2, ...}
         * so we only need the certificates starting at index 1.
         */
        AutoPtr<ArrayOf<ArrayOf<Byte>*> > certsBytes = ArrayOf<ArrayOf<Byte>* >::Alloc(x509chain->GetLength() - 1);
        Int32 totalCertLength = 0;
        for (Int32 i = 0; i < certsBytes->GetLength(); i++) {
            //try
            ECode ec = NOERROR;
            {
                AutoPtr<ArrayOf<Byte> > encoded;
                FAIL_GOTO(ec = ICertificate::Probe((*x509chain)[i+1])->GetEncoded((ArrayOf<Byte>**)&encoded), ERROR)
                certsBytes->Set(i, encoded);
                totalCertLength += (*certsBytes)[i]->GetLength();
            }
        ERROR:
            //} catch (CertificateEncodingException e) {
            if (ec == (ECode)E_CERTIFICATE_ENCODING_EXCEPTION) {
                //throw new KeyStoreException("Can't encode Certificate #" + i, e);
                Logger::E("ElastosKeyStore", "Can't encode Certificate #%d", i);
                return E_KEY_STORE_EXCEPTION;
            }
        }

        /*
         * Serialize this into one byte array so we can later call
         * CertificateFactory#generateCertificates to recover them.
         */
        chainBytes = ArrayOf<Byte>::Alloc(totalCertLength);
        Int32 outputOffset = 0;
        for (Int32 i = 0; i < certsBytes->GetLength(); i++) {
            Int32 certLength = (*certsBytes)[i]->GetLength();
            //System.arraycopy(certsBytes[i], 0, chainBytes, outputOffset, certLength);
            //memcpy((*certsBytes)[i]->GetPayload(), chainBytes->GetPayload() + outputOffset, certLength);
            chainBytes->Copy(outputOffset, (*certsBytes)[i], 0, certLength);

            outputOffset += certLength;
            certsBytes->Set(i, NULL);
        }
    }
    else {
        chainBytes = NULL;
    }

    /*
     * Make sure we clear out all the appropriate types before trying to
     * write.
     */
    if (shouldReplacePrivateKey) {
        Boolean tmp;
        Credentials::DeleteAllTypesForAlias(mKeyStore, alias, &tmp);
    }
    else {
        Boolean tmp;
        Credentials::DeleteCertificateTypesForAlias(mKeyStore, alias, &tmp);
    }

    Int32 flags = 0;
    if (params != NULL) {
        params->GetFlags(&flags);
    }

    Boolean bTmp;
    if (shouldReplacePrivateKey
            && !(mKeyStore->ImportKey(Credentials::USER_PRIVATE_KEY + alias, keyBytes,
                    KeyStore::UID_SELF, flags, &bTmp), bTmp)) {
        Boolean tmp;
        Credentials::DeleteAllTypesForAlias(mKeyStore, alias, &tmp);
        //throw new KeyStoreException("Couldn't put private key in keystore");
        Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Couldn't put private key in keystore");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (!(mKeyStore->Put(Credentials::USER_CERTIFICATE + alias, userCertBytes,
            KeyStore::UID_SELF, flags, &bTmp), bTmp)) {
        Boolean tmp;
        Credentials::DeleteAllTypesForAlias(mKeyStore, alias, &tmp);
        //throw new KeyStoreException("Couldn't put certificate #1 in keystore");
        Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Couldn't put certificate #1 in keystore");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (chainBytes != NULL
            && !(mKeyStore->Put(Credentials::CA_CERTIFICATE + alias, chainBytes,
                    KeyStore::UID_SELF, flags, &bTmp), bTmp)) {
        Boolean tmp;
        Credentials::DeleteAllTypesForAlias(mKeyStore, alias, &tmp);
        //throw new KeyStoreException("Couldn't put certificate chain in keystore");
        Logger::E("ElastosKeyStore", "SetPrivateKeyEntry Couldn't put certificate chain in keystore");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ElastosKeyStore::EngineSetKeyEntry(
    /* [in] */ const String& alias,
    /* [in] */ ArrayOf<Byte>* key,
    /* [in] */ ArrayOf<ICertificate*>* chain)
{
    //throw new KeyStoreException("Operation not supported because key encoding is unknown");
    Logger::E("ElastosKeyStore", "EngineSetKeyEntry Operation not supported because key encoding is unknown");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode ElastosKeyStore::EngineSetCertificateEntry(
    /* [in] */ const String& alias,
    /* [in] */ ICertificate* cert)
{
    if (IsKeyEntry(alias)) {
        //throw new KeyStoreException("Entry exists and is not a trusted certificate");
        Logger::E("ElastosKeyStore", "EngineSetCertificateEntry Entry exists and is not a trusted certificate");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // We can't set something to null.
    if (cert == NULL) {
        //throw new NullPointerException("cert == null");
        Logger::E("ElastosKeyStore", "EngineSetCertificateEntry cert == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > encoded;
    //try {
    ECode ec = cert->GetEncoded((ArrayOf<Byte>**)&encoded);
    //} catch (CertificateEncodingException e) {
    if (ec == (ECode)E_CERTIFICATE_ENCODING_EXCEPTION) {
        //throw new KeyStoreException(e);
        Logger::E("ElastosKeyStore", "KeyStoreException");
        return E_KEY_STORE_EXCEPTION;
    }

    Boolean bTmp;
    if (!(mKeyStore->Put(Credentials::CA_CERTIFICATE + alias, encoded, KeyStore::UID_SELF, KeyStore::FLAG_NONE, &bTmp), bTmp)) {
        //throw new KeyStoreException("Couldn't insert certificate; is KeyStore initialized?");
        Logger::E("ElastosKeyStore", "EngineSetCertificateEntry Couldn't insert certificate; is KeyStore initialized?");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ElastosKeyStore::EngineDeleteEntry(
    /* [in] */ const String& alias)
{
    if (!IsKeyEntry(alias) && !IsCertificateEntry(alias)) {
        return NOERROR;
    }

    Boolean bTmp;
    if (!(Credentials::DeleteAllTypesForAlias(mKeyStore, alias, &bTmp), bTmp)) {
        //throw new KeyStoreException("No such entry " + alias);
        Logger::E("ElastosKeyStore", "EngineDeleteEntry No such entry %s", alias.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

AutoPtr<ISet> ElastosKeyStore::GetUniqueAliases()
{
    AutoPtr<ArrayOf<String> > rawAliases;
    mKeyStore->Saw(String(""), (ArrayOf<String>**)&rawAliases);
    if (rawAliases == NULL) {
        AutoPtr<ISet> result;
        CHashSet::New((ISet**)&result);
        return result;
    }

    AutoPtr<ISet> aliases;
    CHashSet::New(rawAliases->GetLength(), (ISet**)&aliases);
    for (Int32 i = 0; i < rawAliases->GetLength(); ++i) {
        String alias = (*rawAliases)[i];
        Int32 idx = alias.IndexOf('_');
        if ((idx == -1) || (alias.GetLength() <= idx)) {
            //Log.e(NAME, "invalid alias: " + alias);
            Logger::E(NAME, "GetUniqueAliases invalid alias: %s", alias.string());
            continue;
        }

        aliases->Add(CoreUtils::Convert(alias.Substring(idx + 1)));
    }

    return aliases;
}

ECode ElastosKeyStore::EngineAliases(
    /* [out] */ IEnumeration** aliases)
{
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->Enumeration(ICollection::Probe(GetUniqueAliases()), aliases);
    return NOERROR;
}

ECode ElastosKeyStore::EngineContainsAlias(
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("ElastosKeyStore", "EngineContainsAlias alias == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Boolean b1, b2, b3;
    mKeyStore->Contains(Credentials::USER_PRIVATE_KEY + alias, &b1);
    mKeyStore->Contains(Credentials::USER_CERTIFICATE + alias, &b2);
    mKeyStore->Contains(Credentials::CA_CERTIFICATE + alias, &b3);

    *result = b1 || b2 || b3;
    return NOERROR;
}

ECode ElastosKeyStore::EngineSize(
    /* [out] */ Int32* size)
{
    AutoPtr<ISet> set = GetUniqueAliases();
    set->GetSize(size);
    return NOERROR;
}

ECode ElastosKeyStore::EngineIsKeyEntry(
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IsKeyEntry(alias);
    return NOERROR;
}

Boolean ElastosKeyStore::IsKeyEntry(
    /* [in] */ const String& alias)
{
    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("ElastosKeyStore", "IsKeyEntry alias == null");
        //return E_NULL_POINTER_EXCEPTION;
        return FALSE;
    }

    Boolean b;
    mKeyStore->Contains(Credentials::USER_PRIVATE_KEY + alias, &b);
    return b;
}

Boolean ElastosKeyStore::IsCertificateEntry(
    /* [in] */ const String& alias)
{
    if (alias.IsNull()) {
        //throw new NullPointerException("alias == null");
        Logger::E("ElastosKeyStore", "IsKeyEntry alias == null");
        //return E_NULL_POINTER_EXCEPTION;
        return FALSE;
    }

    Boolean b;
    mKeyStore->Contains(Credentials::CA_CERTIFICATE + alias, &b);
    return b;
}

ECode ElastosKeyStore::EngineIsCertificateEntry(
    /* [in] */ const String& alias,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !IsKeyEntry(alias) && IsCertificateEntry(alias);
    return NOERROR;
}

ECode ElastosKeyStore::EngineGetCertificateAlias(
    /* [in] */ ICertificate* cert,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    if (cert == NULL) {
        return NOERROR;
    }

    AutoPtr<ISet> nonCaEntries;//<String>
    CHashSet::New((ISet**)&nonCaEntries);

    /*
     * First scan the PrivateKeyEntry types. The KeyStoreSpi documentation
     * says to only compare the first certificate in the chain which is
     * equivalent to the USER_CERTIFICATE prefix for the Android keystore
     * convention.
     */
    AutoPtr<ArrayOf<String> > certAliases;
    mKeyStore->Saw(Credentials::USER_CERTIFICATE, (ArrayOf<String>**)&certAliases);
    if (certAliases != NULL) {
        Int32 len = certAliases->GetLength();
        for (Int32 i = 0; i < len; ++i) {
            String alias = (*certAliases)[i];
            AutoPtr<ArrayOf<Byte> > certBytes;
            mKeyStore->Get(Credentials::USER_CERTIFICATE + alias, (ArrayOf<Byte>**)&certBytes);
            if (certBytes == NULL) {
                continue;
            }

            AutoPtr<IX509Certificate> x509 = ToCertificate(certBytes);
            ICertificate* c = ICertificate::Probe(x509);
            nonCaEntries->Add(CoreUtils::Convert(alias));

            Boolean bTmp;
            if (IObject::Probe(cert)->Equals(c, &bTmp), bTmp) {
                *result = alias;
                return NOERROR;
            }
        }
    }

    /*
     * Look at all the TrustedCertificateEntry types. Skip all the
     * PrivateKeyEntry we looked at above.
     */
    AutoPtr<ArrayOf<String> > caAliases;
    mKeyStore->Saw(Credentials::CA_CERTIFICATE, (ArrayOf<String>**)&caAliases);
    if (certAliases != NULL) {
        Int32 len = caAliases->GetLength();
        for (Int32 i = 0; i < len; ++i) {
            String alias = (*caAliases)[i];
            Boolean b;
            if (nonCaEntries->Contains(CoreUtils::Convert(alias), &b), b) {
                continue;
            }

            AutoPtr<ArrayOf<Byte> > certBytes;
            mKeyStore->Get(Credentials::CA_CERTIFICATE + alias, (ArrayOf<Byte>**)&certBytes);
            if (certBytes == NULL) {
                continue;
            }

            AutoPtr<IX509Certificate> x509 = ToCertificate(certBytes);
            ICertificate* c = ICertificate::Probe(x509);

            Boolean bTmp;
            if (IObject::Probe(cert)->Equals(c, &bTmp), bTmp) {
                *result = alias;
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

ECode ElastosKeyStore::EngineStore(
    /* [in] */ IOutputStream* stream,
    /* [in] */ ArrayOf<Char32>* password)
{
    //throw new UnsupportedOperationException("Can not serialize AndroidKeyStore to OutputStream");
    Logger::E("ElastosKeyStore", "Can not serialize AndroidKeyStore to OutputStream");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode ElastosKeyStore::EngineLoad(
    /* [in] */ IInputStream* stream,
    /* [in] */ ArrayOf<Char32>* password)
{
    if (stream != NULL) {
        //throw new IllegalArgumentException("InputStream not supported");
        Logger::E("ElastosKeyStore", "EngineLoad InputStream not supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (password != NULL) {
        //throw new IllegalArgumentException("password not supported");
        Logger::E("ElastosKeyStore", "EngineLoad password not supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Unfortunate name collision.
    mKeyStore = KeyStore::GetInstance();
    return NOERROR;
}

ECode ElastosKeyStore::EngineSetEntry(
    /* [in] */ const String& alias,
    /* [in] */ IKeyStoreEntry* entry,
    /* [in] */ IKeyStoreProtectionParameter* param)
{
    if (entry == NULL) {
        //throw new KeyStoreException("entry == null");
        Logger::E("ElastosKeyStore", "EngineSetEntry entry == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean b;
    if (EngineContainsAlias(alias, &b), b) {
        FAIL_RETURN(EngineDeleteEntry(alias))
    }

    if (IKeyStoreTrustedCertificateEntry::Probe(entry) != NULL) {
        IKeyStoreTrustedCertificateEntry* trE = IKeyStoreTrustedCertificateEntry::Probe(entry);
        AutoPtr<ICertificate> cert;
        trE->GetTrustedCertificate((ICertificate**)&cert);
        return EngineSetCertificateEntry(alias, cert);
    }

    if (param != NULL && (IKeyStoreParameter::Probe(param) == NULL)) {
        //throw new KeyStoreException( "protParam should be android.security.KeyStoreParameter; was: " + param.getClass().getName());
        Logger::E("ElastosKeyStore", "EngineSetEntry protParam should be android.security.KeyStoreParameter");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (IKeyStorePrivateKeyEntry::Probe(entry) != NULL) {
        IKeyStorePrivateKeyEntry* prE = IKeyStorePrivateKeyEntry::Probe(entry);
        AutoPtr<IPrivateKey> privatekey;
        prE->GetPrivateKey((IPrivateKey**)&privatekey);
        AutoPtr<ArrayOf<ICertificate*> > cc;
        prE->GetCertificateChain((ArrayOf<ICertificate*>**)&cc);
        return SetPrivateKeyEntry(alias, privatekey, cc, IKeyStoreParameter::Probe(param));
    }

    //throw new KeyStoreException( "Entry must be a PrivateKeyEntry or TrustedCertificateEntry; was " + entry);
    Logger::E("ElastosKeyStore", "EngineSetEntry ntry must be a PrivateKeyEntry or TrustedCertificateEntry");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}


}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security
