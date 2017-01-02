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

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.IO.h"
#include "OpenSSLRSAPublicKey.h"
#include "NativeCrypto.h"
#include "COpenSSLKey.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;
using Elastos::Math::CBigInteger;
using Elastos::Security::EIID_IPublicKey;
using Elastos::Security::EIID_IKey;
using Elastos::Security::Interfaces::IRSAKey;
using Elastos::Security::Interfaces::EIID_IRSAKey;
using Elastos::Security::Interfaces::EIID_IRSAPublicKey;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLRSAPublicKey::
//=========================================
Int64 OpenSSLRSAPublicKey::mSerialVersionUID = 123125005824688292L;

CAR_INTERFACE_IMPL_7(OpenSSLRSAPublicKey, Object, IRSAPublicKey, IPublicKey, \
    IKey, IRSAKey, ISerializable, IOpenSSLKeyHolder, IOpenSSLRSAPublicKey)

ECode OpenSSLRSAPublicKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    mKey = key;
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mKey;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::constructor(
    /* [in] */ IRSAPublicKeySpec* spec)
{
    AutoPtr<IBigInteger> modulus;
    spec->GetModulus((IBigInteger**)&modulus);
    AutoPtr<ArrayOf<Byte> > arr;
    modulus->ToByteArray((ArrayOf<Byte>**)&arr);
    AutoPtr<IBigInteger> exponent;
    spec->GetPublicExponent((IBigInteger**)&exponent);
    AutoPtr<ArrayOf<Byte> > arrExponent;
    exponent->ToByteArray((ArrayOf<Byte>**)&arrExponent);
    Int64 rsa = 0;
    NativeCrypto::EVP_PKEY_new_RSA(
            arr,
            arrExponent,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &rsa);
    COpenSSLKey::New(rsa, (IOpenSSLKey**)&mKey);
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::GetInstance(
    /* [in] */ IRSAPublicKey* rsaPublicKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBigInteger> modulus;
    IRSAKey::Probe(rsaPublicKey)->GetModulus((IBigInteger**)&modulus);
    AutoPtr<ArrayOf<Byte> > arr;
    modulus->ToByteArray((ArrayOf<Byte>**)&arr);
    AutoPtr<IBigInteger> exponent;
    rsaPublicKey->GetPublicExponent((IBigInteger**)&exponent);
    AutoPtr<ArrayOf<Byte> > arrExponent;
    exponent->ToByteArray((ArrayOf<Byte>**)&arrExponent);
    Int64 rsa = 0;
    NativeCrypto::EVP_PKEY_new_RSA(
            arr,
            arrExponent,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &rsa);
    AutoPtr<IOpenSSLKey> res;
    COpenSSLKey::New(rsa, (IOpenSSLKey**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("RSA");
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("X.509");
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 context = 0;
    mKey->GetPkeyContext(&context);
    return NativeCrypto::I2d_PUBKEY(context, result);
}

void OpenSSLRSAPublicKey::EnsureReadParams()
{
    if (mFetchedParams) {
        return;
    }

    Int64 context = 0;
    mKey->GetPkeyContext(&context);
    AutoPtr<ArrayOf<Byte> > param0;
    AutoPtr<ArrayOf<Byte> > param1;
    NativeCrypto::Get_RSA_public_params(
            context,
            (ArrayOf<Byte>**)&param0,
            (ArrayOf<Byte>**)&param1);
    CBigInteger::New(*param0, (IBigInteger**)&mModulus);
    CBigInteger::New(*param1, (IBigInteger**)&mPublicExponent);

    mFetchedParams = TRUE;
}

ECode OpenSSLRSAPublicKey::GetModulus(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mModulus;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::GetPublicExponent(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mPublicExponent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (o == IRSAPublicKey::Probe(this)->Probe(EIID_IInterface)) {
        *result = TRUE;
        return NOERROR;
    }

    if (IOpenSSLRSAPublicKey::Probe(o) != NULL) {
        AutoPtr<IOpenSSLRSAPublicKey> other = IOpenSSLRSAPublicKey::Probe(o);

        /*
         * We can shortcut the true case, but it still may be equivalent but
         * different copies.
         */
        AutoPtr<IOpenSSLKey> sslkey;
        IOpenSSLKeyHolder::Probe(other)->GetOpenSSLKey((IOpenSSLKey**)&sslkey);
        Boolean b = FALSE;
        IObject::Probe(mKey)->Equals(sslkey, &b);
        if (b) {
            *result = TRUE;
            return NOERROR;
        }
    }

    if (IRSAPublicKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    EnsureReadParams();

    AutoPtr<IRSAPublicKey> other = IRSAPublicKey::Probe(o);
    AutoPtr<IBigInteger> publicExponent;
    other->GetPublicExponent((IBigInteger**)&publicExponent);
    AutoPtr<IBigInteger> modulus;
    IRSAKey::Probe(other)->GetModulus((IBigInteger**)&modulus);
    Boolean bModulus = FALSE;
    mModulus->Equals(modulus, &bModulus);
    Boolean bExponent = FALSE;
    mPublicExponent->Equals(publicExponent, &bExponent);
    *result = bModulus && bExponent;
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    Int32 modulusHC = 0, peHC = 0;
    IObject::Probe(mModulus)->GetHashCode(&modulusHC);
    IObject::Probe(mPublicExponent)->GetHashCode(&peHC);
    *result = modulusHC ^ peHC;
    return NOERROR;
}

ECode OpenSSLRSAPublicKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    StringBuilder sb("OpenSSLRSAPublicKey{");
    sb.Append("modulus=");
    String strModulus;
    mModulus->ToString(16, &strModulus);
    sb.Append(strModulus);
    sb.Append(',');
    sb.Append("publicExponent=");
    String strPE;
    mPublicExponent->ToString(16, &strPE);
    sb.Append(strPE);
    sb.Append('}');

    *result = sb.ToString();
    return NOERROR;
}

void OpenSSLRSAPublicKey::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();

    AutoPtr<ArrayOf<Byte> > arrModulus;
    mModulus->ToByteArray((ArrayOf<Byte>**)&arrModulus);
    AutoPtr<ArrayOf<Byte> > arrExponent;
    mPublicExponent->ToByteArray((ArrayOf<Byte>**)&arrExponent);
    Int64 rsa = 0;
    NativeCrypto::EVP_PKEY_new_RSA(
            arrModulus,
            arrExponent,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &rsa);
    COpenSSLKey::New(rsa, (IOpenSSLKey**)&mKey);
    mFetchedParams = TRUE;
}

void OpenSSLRSAPublicKey::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    EnsureReadParams();
    stream->DefaultWriteObject();
}

} // namespace Conscrypt
} // namespace Org
