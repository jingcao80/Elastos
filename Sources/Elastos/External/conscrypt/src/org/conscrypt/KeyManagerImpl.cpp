#include "org/conscrypt/KeyManagerImpl.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Security::Cert::ICertificate;
// TODO: Need CKeyStorePasswordProtection
// using Elastos::Security::CKeyStorePasswordProtection;
using Elastos::Security::IKey;
using Elastos::Security::IKeyStoreEntry;
using Elastos::Security::IKeyStorePasswordProtection;
using Elastos::Security::IKeyStorePrivateKeyEntry;
using Elastos::Security::IPublicKey;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashTable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::IList;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(KeyManagerImpl, X509ExtendedKeyManager, IKeyManagerImpl)

ECode KeyManagerImpl::constructor(
    /* [in] */ IKeyStore* keyStore,
    /* [in] */ ArrayOf<Char32>* pwd)
{
    CHashTable::New((IHashTable**)&mHash);
    AutoPtr<IEnumeration/*<String>*/> aliases;
    // try {
    keyStore->Aliases((IEnumeration**)&aliases);
    // } catch (KeyStoreException e) {
    //     return;
    // }
    Boolean b;
    for (; (aliases->HasMoreElements(&b), b);) {
        AutoPtr<IInterface> obj;
        aliases->GetNextElement((IInterface**)&obj);
        String alias;
        ICharSequence::Probe(obj)->ToString(&alias);
        // try {
// TODO: Need KeyStore.PrivateKeyEntry.class
//         if (keyStore->EntryInstanceOf(alias, /*TODO: KeyStore.PrivateKeyEntry.class*/ NULL, &b), b) {
//             AutoPtr<IKeyStorePasswordProtection> kspp;
// // TODO: Need CKeyStorePasswordProtection
//             // CKeyStorePasswordProtection::New(pwd, (IKeyStorePasswordProtection**)&kspp);
//             AutoPtr<IKeyStoreEntry> kse;
//             keyStore->GetEntry(alias, IKeyStoreProtectionParameter::Probe(kspp), (IKeyStoreEntry**)&kse);
//             AutoPtr<IKeyStorePrivateKeyEntry> entry = IKeyStorePrivateKeyEntry::Probe(kse);
//             mHash->Put(CoreUtils::Convert(alias), entry);
//         }
        // } catch (KeyStoreException e) {
        //     continue;
        // } catch (UnrecoverableEntryException e) {
        //     continue;
        // } catch (NoSuchAlgorithmException e) {
        //     continue;
        // }
    }
    return NOERROR;
}

ECode KeyManagerImpl::ChooseClientAlias(
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISocket* socket,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > al = ChooseAlias(keyTypes, issuers);
    *result = (al == NULL ? String(NULL) : (*al)[0]);
    return NOERROR;
}

ECode KeyManagerImpl::ChooseServerAlias(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISocket* socket,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = keyType;
    AutoPtr<ArrayOf<String> > al = ChooseAlias(array, issuers);
    *result = (al == NULL ? String(NULL) : (*al)[0]);
    return NOERROR;
}

ECode KeyManagerImpl::GetCertificateChain(
    /* [in] */ const String& alias,
    /* [out] */ ArrayOf<IX509Certificate*>** result)
{
    VALIDATE_NOT_NULL(result)
    if (alias.IsNull()) {
        *result = NULL;
        return NOERROR;
    }
    Boolean b;
    if (mHash->ContainsKey(CoreUtils::Convert(alias), &b), b) {
        AutoPtr<IInterface> obj;
        mHash->Get(CoreUtils::Convert(alias), (IInterface**)&obj);
        AutoPtr<ArrayOf<ICertificate*> > certs;
        IKeyStorePrivateKeyEntry::Probe(obj)->GetCertificateChain(
                (ArrayOf<ICertificate*>**)&certs);
        if (IX509Certificate::Probe((*certs)[0]) != NULL) {
            AutoPtr<ArrayOf<IX509Certificate*> > xcerts =
                    ArrayOf<IX509Certificate*>::Alloc(certs->GetLength());
            for (Int32 i = 0; i < certs->GetLength(); i++) {
                xcerts->Set(i, IX509Certificate::Probe((*certs)[i]));
            }
            *result = xcerts;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode KeyManagerImpl::GetClientAliases(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = keyType;
    *result = ChooseAlias(array, issuers);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode KeyManagerImpl::GetServerAliases(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = keyType;
    *result = ChooseAlias(array, issuers);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode KeyManagerImpl::GetPrivateKey(
    /* [in] */ const String& alias,
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result)
    if (alias.IsNull()) {
        *result = NULL;
        return NOERROR;
    }

    Boolean b;
    if (mHash->ContainsKey(CoreUtils::Convert(alias), &b), b) {
        AutoPtr<IInterface> obj;
        mHash->Get(CoreUtils::Convert(alias), (IInterface**)&obj);
        return IKeyStorePrivateKeyEntry::Probe(obj)->GetPrivateKey(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode KeyManagerImpl::ChooseEngineClientAlias(
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > al = ChooseAlias(keyTypes, issuers);
    *result = (al == NULL ? String(NULL) : (*al)[0]);
    return NOERROR;
}

ECode KeyManagerImpl::ChooseEngineServerAlias(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<IPrincipal*>* issuers,
    /* [in] */ ISSLEngine* engine,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = keyType;
    AutoPtr<ArrayOf<String> > al = ChooseAlias(array, issuers);
    *result = (al == NULL ? String(NULL) : (*al)[0]);
    return NOERROR;
}

AutoPtr<ArrayOf<String> > KeyManagerImpl::ChooseAlias(
    /* [in] */ ArrayOf<String>* keyTypes,
    /* [in] */ ArrayOf<IPrincipal*>* issuers)
{
    if (keyTypes == NULL || keyTypes->GetLength() == 0) {
        return NULL;
    }

    AutoPtr<IList> issuersList;
    if (issuers == NULL) {
        issuersList = NULL;
    }
    else {
        Arrays::AsList(issuers, (IList**)&issuersList);
    }
    AutoPtr<IArrayList> found;
    CArrayList::New((IArrayList**)&found);

    AutoPtr<IEnumeration> aliases;
    mHash->GetKeys((IEnumeration**)&aliases);
    Boolean b;
    for (; (aliases->HasMoreElements(&b), b);) {
        AutoPtr<IInterface> key;
        aliases->GetNextElement((IInterface**)&key);
        AutoPtr<IInterface> value;
        mHash->Get(key, (IInterface**)&value);
        String alias;
        ICharSequence::Probe(key)->ToString(&alias);
        AutoPtr<IKeyStorePrivateKeyEntry> entry = IKeyStorePrivateKeyEntry::Probe(value);
        AutoPtr<ArrayOf<ICertificate*> > chain;
        entry->GetCertificateChain((ArrayOf<ICertificate*>**)&chain);
        AutoPtr<ICertificate> cert = (*chain)[0];
        AutoPtr<IPublicKey> pk;
        cert->GetPublicKey((IPublicKey**)&pk);
        String certKeyAlg;
        IKey::Probe(pk)->GetAlgorithm(&certKeyAlg);

        String certSigAlg;
        if (IX509Certificate::Probe(cert) != NULL) {
            IX509Certificate::Probe(cert)->GetSigAlgName(&certSigAlg);
            certSigAlg = certSigAlg.ToUpperCase(/*Locale.US*/);
        }
        else {
            certSigAlg = String(NULL);
        }

        for (Int32 i = 0; i < keyTypes->GetLength(); ++i) {
            String keyAlgorithm = (*keyTypes)[i];

            if (keyAlgorithm.IsNull()) {
                continue;
            }
            String sigAlgorithm;
            // handle cases like EC_EC and EC_RSA
            Int32 index = keyAlgorithm.IndexOf('_');
            if (index == -1) {
                sigAlgorithm = String(NULL);
            }
            else {
                sigAlgorithm = keyAlgorithm.Substring(index + 1);
                keyAlgorithm = keyAlgorithm.Substring(0, index);
            }
            // key algorithm does not match
            if (!certKeyAlg.Equals(keyAlgorithm)) {
                continue;
            }
            /*
             * TODO find a more reliable test for signature
             * algorithm. Unfortunately value varies with
             * provider. For example for "EC" it could be
             * "SHA1WithECDSA" or simply "ECDSA".
             */
            // sig algorithm does not match
            if (!sigAlgorithm.IsNull() && !certSigAlg.IsNull()
                    && !certSigAlg.Contains(sigAlgorithm)) {
                continue;
            }
            // no issuers to match, just add to return list and continue
            if (issuers == NULL || issuers->GetLength() == 0) {
                found->Add(CoreUtils::Convert(alias));
                continue;
            }
            // check that a certificate in the chain was issued by one of the specified issuers
            for (Int32 i = 0; i < chain->GetLength(); ++i) {
                AutoPtr<ICertificate> certFromChain = (*chain)[i];
                if (IX509Certificate::Probe(certFromChain) == NULL) {
                    // skip non-X509Certificates
                    continue;
                }
                AutoPtr<IX509Certificate> xcertFromChain =
                        IX509Certificate::Probe(certFromChain);
                /*
                 * Note use of X500Principal from
                 * getIssuerX500Principal as opposed to Principal
                 * from getIssuerDN. Principal.equals test does
                 * not work in the case where
                 * xcertFromChain.getIssuerDN is a bouncycastle
                 * org.bouncycastle.jce.X509Principal.
                 */
                AutoPtr<IX500Principal> issuerFromChain;
                xcertFromChain->GetIssuerX500Principal((IX500Principal**)&issuerFromChain);
                if (issuersList->Contains(issuerFromChain, &b), b) {
                    found->Add(CoreUtils::Convert(alias));
                }
            }
        }
    }

    if (found->IsEmpty(&b), !b) {
        Int32 size;
        found->GetSize(&size);
        AutoPtr<ArrayOf<IInterface*> > tmpArray = ArrayOf<IInterface*>::Alloc(size);
        AutoPtr<ArrayOf<IInterface*> > array;
        found->ToArray(tmpArray, (ArrayOf<IInterface*>**)&array);
        AutoPtr<ArrayOf<String> > retArray = ArrayOf<String>::Alloc(array->GetLength());
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            ICharSequence::Probe((*array)[i])->ToString(&(*retArray)[i]);
        }
    }
    return NULL;
}

} // namespace Conscrypt
} // namespace Org
