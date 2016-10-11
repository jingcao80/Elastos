
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLSecretKey.h"
#include "NativeCrypto.h"
#include <elastos/utility/Arrays.h>

using Elastosx::Crypto::EIID_ISecretKey;
using Elastos::Security::IKey;
using Elastos::Utility::Arrays;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSecretKey::
//=========================================
Int64 OpenSSLSecretKey::mSerialVersionUID = 1831053062911514589L;

CAR_INTERFACE_IMPL_3(OpenSSLSecretKey, Object, ISecretKey, IOpenSSLSecretKey, IOpenSSLKeyHolder)

ECode OpenSSLSecretKey::constructor(
    /* [in] */ String algorithm,
    /* [in] */ ArrayOf<Byte>* encoded)
{
    mAlgorithm = algorithm;
    mEncoded = encoded;

    mType = INativeCrypto::EVP_PKEY_HMAC;
    Int64 key = 0;
    NativeCrypto::EVP_PKEY_new_mac_key(mType, encoded, &key);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(key);
    return NOERROR;
}

ECode OpenSSLSecretKey::constructor(
    /* [in] */ String algorithm,
    /* [in] */ IOpenSSLKey* key)
{
    mAlgorithm = algorithm;
    mKey = key;

    Int64 keyContext = 0;
    key->GetPkeyContext(&keyContext);
    NativeCrypto::EVP_PKEY_type(keyContext, &mType);
    mEncoded = NULL;
    return NOERROR;
}

ECode OpenSSLSecretKey::GetInstance(
    /* [in] */ ISecretKey* key,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(key)->GetEncoded((ArrayOf<Byte>**)&encoded);
    Int64 mac_key = 0;
    NativeCrypto::EVP_PKEY_new_mac_key(INativeCrypto::EVP_PKEY_HMAC,
            encoded, &mac_key);
    AutoPtr<IOpenSSLKey> res;
    assert(0 && "TODO");
    // new OpenSSLKey(mac_key);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSecretKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAlgorithm;
    return NOERROR;
}

ECode OpenSSLSecretKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bBased = FALSE;
    mKey->IsEngineBased(&bBased);
    if (bBased) {
        *result = String(NULL);
        return NOERROR;
    }

    *result = String("RAW");
    return NOERROR;
}

ECode OpenSSLSecretKey::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bBased = FALSE;
    mKey->IsEngineBased(&bBased);
    if (bBased) {
        *result = NULL;
        return NOERROR;
    }

    *result = mEncoded;
    return NOERROR;
}

ECode OpenSSLSecretKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mKey;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLSecretKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (o == this->Probe(EIID_IInterface)) {
        *result = TRUE;
        return NOERROR;
    }

    if (ISecretKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ISecretKey> other = ISecretKey::Probe(o);
    String otherAlgorithm;
    IKey::Probe(other)->GetAlgorithm(&otherAlgorithm);
    if (!mAlgorithm.Equals(otherAlgorithm)) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean bBased = FALSE;
    if (IOpenSSLSecretKey::Probe(o) != NULL) {
        AutoPtr<IOpenSSLSecretKey> otherOpenSSL = IOpenSSLSecretKey::Probe(o);
        AutoPtr<IOpenSSLKey> otherKey;
        IOpenSSLKeyHolder::Probe(otherOpenSSL)->GetOpenSSLKey((IOpenSSLKey**)&otherKey);
        return IObject::Probe(mKey)->Equals(otherKey, result);
    }
    else if ((mKey->IsEngineBased(&bBased), bBased)) {
        *result = FALSE;
        return NOERROR;
    }

    String fm, otherFm;
    GetFormat(&fm);
    IKey::Probe(other)->GetFormat(&otherFm);
    if (!fm.Equals(otherFm)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > encoded;
    IKey::Probe(other)->GetEncoded((ArrayOf<Byte>**)&encoded);
    *result = Arrays::Equals(mEncoded, encoded);
    return NOERROR;
}

ECode OpenSSLSecretKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return IObject::Probe(mKey)->GetHashCode(result);
}

void OpenSSLSecretKey::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();

    Int64 mac_key = 0;
    NativeCrypto::EVP_PKEY_new_mac_key(mType, mEncoded, &mac_key);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(mac_key);
}

void OpenSSLSecretKey::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    AutoPtr<IOpenSSLKey> openkey;
    GetOpenSSLKey((IOpenSSLKey**)&openkey);
    Boolean bBased = FALSE;
    openkey->IsEngineBased(&bBased);
    if (bBased) {
        // throw new NotSerializableException("engine-based keys can not be serialized");
        return;
    }

    stream->DefaultWriteObject();
}

} // namespace Conscrypt
} // namespace Org
