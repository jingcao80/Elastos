
#include "CKeyStorePrivateKeyEntry.h"
#include "StringBuilder.h"
#include "StringUtils.h"

using Elastos::Security::Cert::IX509Certificate;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStorePrivateKeyEntry);
CAR_INTERFACE_IMPL_2(CKeyStorePrivateKeyEntry, Object, IKeyStorePrivateKeyEntry, IKeyStoreEntry);
ECode CKeyStorePrivateKeyEntry::GetPrivateKey(
    /* [out] */ IPrivateKey **privateKey)
{
    VALIDATE_NOT_NULL(privateKey)
    *privateKey = mPrivateKey;
    REFCOUNT_ADD(*privateKey)
    return NOERROR;
}

ECode CKeyStorePrivateKeyEntry::GetCertificateChain(
    /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate*> **cc)
{
    VALIDATE_NOT_NULL(cc)
    *cc = mChain->Clone();
    REFCOUNT_ADD(*cc)
    return NOERROR;
}

ECode CKeyStorePrivateKeyEntry::GetCertificate(
            /* [out] */ Elastos::Security::Cert::ICertificate **cert)
{
    VALIDATE_NOT_NULL(cert)
    *cert = (*mChain)[0];
    REFCOUNT_ADD(*cert)
    return NOERROR;
}

ECode CKeyStorePrivateKeyEntry::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("PrivateKeyEntry: number of elements in certificate chain is ");
    sb.AppendString(StringUtils::Int32ToString(mChain->GetLength()));
    sb.AppendCStr("\n");
    for (Int32 i = 0; i < mChain->GetLength(); i++) {
        String cert;
        (*mChain)[i]->ToString(&cert);
        sb.AppendString(cert);
        sb.AppendCStr("\n");
    }
    return sb.ToString(str);
}

ECode CKeyStorePrivateKeyEntry::constructor(
    /* [in] */ IPrivateKey *privateKey,
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain)
{
    if (privateKey == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (chain == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (chain->GetLength() == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Match algorithm of private key and algorithm of public key from
    // the end certificate
    String s, pubAlgo, priAlgo;
    (*chain)[0]->GetType(&s);
    AutoPtr<IPublicKey> pubKey;
    (*chain)[0]->GetPublicKey((IPublicKey**)&pubKey);
    pubKey->GetAlgorithm(&pubAlgo);
    privateKey->GetAlgorithm(&priAlgo);
    if (!pubAlgo.Equals(priAlgo)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // Match certificate types
    for (Int32 i = 0; i < chain->GetLength(); i++) {
        String type;
        (*chain)[i]->GetType(&type);
        if (!s.Equals(type)) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    // clone chain - this.chain = (Certificate[])chain.clone();
    Boolean isAllX509Certificates = TRUE;
    // assert chain length > 0
    for (Int32 i = 0; i < chain->GetLength(); i++) {
        if (!IX509Certificate::Probe((*chain)[i])) {
            isAllX509Certificates = FALSE;
            break;
        }
    }

    if (isAllX509Certificates) {
       AutoPtr<ArrayOf<IX509Certificate*> > tmp = ArrayOf<IX509Certificate*>::Alloc(chain->GetLength());
       mChain =ArrayOf<Elastos::Security::Cert::ICertificate*>::Alloc(chain->GetLength());
       for (Int32 i = 0; i < mChain->GetLength(); i++) {
            mChain->Set(i, (*tmp)[i]);
       }
    }
    else {
        mChain = ArrayOf<Elastos::Security::Cert::ICertificate*>::Alloc(chain->GetLength());
    }
    FAIL_RETURN(mChain->Copy(0, chain, 0, chain->GetLength()))
    mPrivateKey = privateKey;
    return NOERROR;
}

}
}

