//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLSecretKey.h"
#include "NativeCrypto.h"
#include "COpenSSLKey.h"
#include <elastos/utility/Arrays.h>

using Elastosx::Crypto::EIID_ISecretKey;
using Elastos::Security::EIID_IKey;
using Elastos::Security::IKey;
using Elastos::Utility::Arrays;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSecretKey::
//=========================================
Int64 OpenSSLSecretKey::mSerialVersionUID = 1831053062911514589L;

CAR_INTERFACE_IMPL_4(OpenSSLSecretKey, Object, IKey, ISecretKey, IOpenSSLSecretKey, IOpenSSLKeyHolder)

ECode OpenSSLSecretKey::constructor(
    /* [in] */ String algorithm,
    /* [in] */ ArrayOf<Byte>* encoded)
{
    mAlgorithm = algorithm;
    mEncoded = encoded;

    mType = INativeCrypto::EVP_PKEY_HMAC;
    Int64 key = 0;
    NativeCrypto::EVP_PKEY_new_mac_key(mType, encoded, &key);
    COpenSSLKey::New(key, (IOpenSSLKey**)&mKey);
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
    COpenSSLKey::New(mac_key, (IOpenSSLKey**)&res);
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
    COpenSSLKey::New(mac_key, (IOpenSSLKey**)&mKey);
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
