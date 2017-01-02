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

#include "org/conscrypt/OpenSSLDHPrivateKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;
using Elastos::Math::CBigInteger;
using Elastos::Security::EIID_IPrivateKey;
using Elastos::Security::EIID_IKey;
using Elastos::Utility::Arrays;
using Elastosx::Crypto::Interfaces::EIID_IDHKey;
using Elastosx::Crypto::Interfaces::EIID_IDHPrivateKey;
using Elastosx::Crypto::Spec::CDHParameterSpec;

namespace Org {
namespace Conscrypt {

const Int64 OpenSSLDHPrivateKey::serialVersionUID;

CAR_INTERFACE_IMPL_6(OpenSSLDHPrivateKey, Object, IOpenSSLDHPrivateKey,
        IDHPrivateKey, IDHKey, IKey, IPrivateKey, IOpenSSLKeyHolder)

OpenSSLDHPrivateKey::OpenSSLDHPrivateKey()
    : mReadParams(FALSE)
{
}

ECode OpenSSLDHPrivateKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    mKey = (OpenSSLKey*)key;
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::constructor(
    /* [in] */ IDHPrivateKeySpec* dhKeySpec)
{
    // try {
    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bX;
    dhKeySpec->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dhKeySpec->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dhKeySpec->GetX((IBigInteger**)&bX);
    AutoPtr<ArrayOf<Byte> > arrayX;
    bX->ToByteArray((ArrayOf<Byte>**)&arrayX);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DH(
            arrayP, arrayG, NULL, arrayX, &val);
    mKey = new OpenSSLKey();
    mKey->constructor(val);
    // } catch (Exception e) {
    //     throw new InvalidKeySpecException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IOpenSSLKey::Probe(mKey);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::GetInstance(
    /* [in] */ IDHPrivateKey* dhPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IDHParameterSpec> dhParams;
    IDHKey::Probe(dhPrivateKey)->GetParams((IDHParameterSpec**)&dhParams);

    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bX;
    dhParams->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dhParams->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dhPrivateKey->GetX((IBigInteger**)&bX);
    AutoPtr<ArrayOf<Byte> > arrayX;
    bX->ToByteArray((ArrayOf<Byte>**)&arrayX);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DH(
                arrayP, arrayG, NULL, arrayX, &val);
    AutoPtr<OpenSSLKey> key = new OpenSSLKey();
    key->constructor(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new InvalidKeyException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("DH");
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * If we're using an OpenSSL ENGINE, there's no guarantee we can export
     * the key. Returning {@code NULL} tells the caller that there's no
     * encoded format.
     */
    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        *result = String(NULL);
    }

    *result = String("PKCS#8");
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * If we're using an OpenSSL ENGINE, there's no guarantee we can export
     * the key. Returning {@code NULL} tells the caller that there's no
     * encoded format.
     */
    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        *result = NULL;
        return NOERROR;
    }

    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    return NativeCrypto::I2d_PKCS8_PRIV_KEY_INFO(ctx, result);
}

ECode OpenSSLDHPrivateKey::GetParams(
    /* [out] */ IDHParameterSpec** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bG;
    CBigInteger::New(*mP, (IBigInteger**)&bP);
    CBigInteger::New(*mG, (IBigInteger**)&bG);

    return CDHParameterSpec::New(bP, bG, result);
}

ECode OpenSSLDHPrivateKey::GetX(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        // throw new UnsupportedOperationException("private key value X cannot be extracted");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    EnsureReadParams();
    return CBigInteger::New(*mX, result);
}

ECode OpenSSLDHPrivateKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    Int32 hash = 1;
    Boolean b;
    if (mKey->IsEngineBased(&b), !b) {
        Int32 val = Arrays::GetHashCode(mX);
        hash = hash * 3 + val;
    }
    Int32 pCode = Arrays::GetHashCode(mP);
    Int32 gCode = Arrays::GetHashCode(mG);
    hash = hash * 7 + pCode;
    hash = hash * 13 + gCode;
    *result = hash;
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean b;
    if (IOpenSSLDHPrivateKey::Probe(o) != NULL) {
        AutoPtr<IOpenSSLDHPrivateKey> other = IOpenSSLDHPrivateKey::Probe(o);

        /*
         * We can shortcut the true case, but it still may be equivalent but
         * different copies.
         */
        AutoPtr<IOpenSSLKey> key;
        IOpenSSLKeyHolder::Probe(other)->GetOpenSSLKey((IOpenSSLKey**)&key);
        if (mKey->Equals(key, &b), b) {
            *result = TRUE;
            return NOERROR;
        }
    }

    if (IDHPrivateKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    EnsureReadParams();

    AutoPtr<IDHPrivateKey> other = IDHPrivateKey::Probe(o);
    AutoPtr<IBigInteger> bX;
    other->GetX((IBigInteger**)&bX);
    AutoPtr<ArrayOf<Byte> > arrayX;
    bX->ToByteArray((ArrayOf<Byte>**)&arrayX);
    if (!Arrays::Equals(mX, arrayX)) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IDHParameterSpec> spec;
    IDHKey::Probe(other)->GetParams((IDHParameterSpec**)&spec);
    AutoPtr<IBigInteger> bG;
    spec->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    AutoPtr<IBigInteger> bP;
    spec->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    Boolean b1 = Arrays::Equals(mG, arrayG);
    Boolean b2 = Arrays::Equals(mP, arrayP);
    *result = b1 && b2;
    return NOERROR;
}

ECode OpenSSLDHPrivateKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("OpenSSLDHPrivateKey{");

    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        sb.Append("key=");
        sb.Append(mKey);
        sb.Append('}');
        return sb.ToString(result);
    }

    EnsureReadParams();
    sb.Append("X=");
    AutoPtr<IBigInteger> bX;
    CBigInteger::New(*mX, (IBigInteger**)&bX);
    String str;
    bX->ToString(16, &str);
    sb.Append(str);
    sb.Append(',');
    sb.Append("P=");
    AutoPtr<IBigInteger> bP;
    CBigInteger::New(*mP, (IBigInteger**)&bP);
    bP->ToString(16, &str);
    sb.Append(str);
    sb.Append(',');
    sb.Append("G=");
    AutoPtr<IBigInteger> bG;
    CBigInteger::New(*mG, (IBigInteger**)&bG);
    bG->ToString(16, &str);
    sb.Append(str);
    sb.Append('}');

    return sb.ToString(result);
}

void OpenSSLDHPrivateKey::EnsureReadParams()
{
    AutoLock lock(mParamsLock);
    if (mReadParams) {
        return;
    }

    Int64 val;
    mKey->GetPkeyContext(&val);
    AutoPtr<ArrayOf<Byte> > y;
    NativeCrypto::Get_DH_params(val, (ArrayOf<Byte>**)&mP, (ArrayOf<Byte>**)&mG,
            (ArrayOf<Byte>**)&y, (ArrayOf<Byte>**)&mX);

    mReadParams = TRUE;
}

void OpenSSLDHPrivateKey::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();

    AutoPtr<IInterface> obj;
    IObjectInput::Probe(stream)->ReadObject((IInterface**)&obj);
    AutoPtr<IBigInteger> g = IBigInteger::Probe(obj);
    obj = NULL;
    IObjectInput::Probe(stream)->ReadObject((IInterface**)&obj);
    AutoPtr<IBigInteger> p = IBigInteger::Probe(obj);
    obj = NULL;
    IObjectInput::Probe(stream)->ReadObject((IInterface**)&obj);
    AutoPtr<IBigInteger> x = IBigInteger::Probe(obj);

    AutoPtr<ArrayOf<Byte> > arrayP;
    p->ToByteArray((ArrayOf<Byte>**)&arrayP);
    AutoPtr<ArrayOf<Byte> > arrayG;
    g->ToByteArray((ArrayOf<Byte>**)&arrayG);
    AutoPtr<ArrayOf<Byte> > arrayX;
    x->ToByteArray((ArrayOf<Byte>**)&arrayX);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DH(arrayP, arrayG, NULL, arrayX, &val);
    mKey = new OpenSSLKey();
    mKey->constructor(val);
}

ECode OpenSSLDHPrivateKey::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    AutoPtr<IOpenSSLKey> key;
    GetOpenSSLKey((IOpenSSLKey**)&key);
    Boolean b;
    if (((OpenSSLKey*)key.Get())->IsEngineBased(&b), b) {
        // throw new NotSerializableException("engine-based keys can not be serialized");
        return E_NOT_SERIALIZABLE_EXCEPTION;
    }

    stream->DefaultWriteObject();

    EnsureReadParams();
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bX;
    CBigInteger::New(*mG, (IBigInteger**)&bG);
    IObjectOutput::Probe(stream)->WriteObject(bG);
    CBigInteger::New(*mP, (IBigInteger**)&bP);
    IObjectOutput::Probe(stream)->WriteObject(bP);
    CBigInteger::New(*mX, (IBigInteger**)&bX);
    IObjectOutput::Probe(stream)->WriteObject(bX);

    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
