#include "org/conscrypt/OpenSSLDHPublicKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;
using Elastos::Math::CBigInteger;
using Elastos::Security::EIID_IPublicKey;
using Elastos::Utility::Arrays;
using Elastosx::Crypto::Interfaces::EIID_IDHKey;
using Elastosx::Crypto::Interfaces::EIID_IDHPublicKey;
using Elastosx::Crypto::Spec::CDHParameterSpec;

namespace Org {
namespace Conscrypt {

const Int64 OpenSSLDHPublicKey::serialVersionUID;

CAR_INTERFACE_IMPL_5(OpenSSLDHPublicKey, Object, IOpenSSLDHPublicKey,
        IDHPublicKey, IDHKey, IPublicKey, IOpenSSLKeyHolder)

OpenSSLDHPublicKey::OpenSSLDHPublicKey()
    : mReadParams(FALSE)
{
}

ECode OpenSSLDHPublicKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    mKey = (OpenSSLKey*)key;
    return NOERROR;
}

ECode OpenSSLDHPublicKey::constructor(
    /* [in] */ IDHPublicKeySpec* dhKeySpec)
{
    // try {
    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bY;
    dhKeySpec->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dhKeySpec->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dhKeySpec->GetY((IBigInteger**)&bY);
    AutoPtr<ArrayOf<Byte> > arrayY;
    bY->ToByteArray((ArrayOf<Byte>**)&arrayY);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DH(
            arrayP, arrayG, arrayY, NULL, &val);
    mKey = new OpenSSLKey();
    mKey->constructor(val);
    // } catch (Exception e) {
    //     throw new InvalidKeySpecException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDHPublicKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IOpenSSLKey::Probe(mKey);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDHPublicKey::GetInstance(
    /* [in] */ IDHPublicKey* dhPublicKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IDHParameterSpec> dhParams;
    IDHKey::Probe(dhPublicKey)->GetParams((IDHParameterSpec**)&dhParams);

    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bY;
    dhParams->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dhParams->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dhPublicKey->GetY((IBigInteger**)&bY);
    AutoPtr<ArrayOf<Byte> > arrayY;
    bY->ToByteArray((ArrayOf<Byte>**)&arrayY);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DH(
                arrayP, arrayG, arrayY, NULL, &val);
    AutoPtr<OpenSSLKey> key = new OpenSSLKey();
    key->constructor(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new InvalidKeyException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDHPublicKey::GetParams(
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

ECode OpenSSLDHPublicKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("DH");
    return NOERROR;
}

ECode OpenSSLDHPublicKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("X.509");
    return NOERROR;
}

ECode OpenSSLDHPublicKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    return NativeCrypto::I2d_PUBKEY(ctx, result);
}

ECode OpenSSLDHPublicKey::GetY(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    return CBigInteger::New(*mY, result);
}

ECode OpenSSLDHPublicKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    Int32 hash = 1;

    Int32 yCode = Arrays::GetHashCode(mY);
    Int32 pCode = Arrays::GetHashCode(mP);
    Int32 gCode = Arrays::GetHashCode(mG);

    hash = hash * 3 + yCode;
    hash = hash * 7 + pCode;
    hash = hash * 13 + gCode;
    *result = hash;
    return NOERROR;
}

ECode OpenSSLDHPublicKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean b;
    if (IOpenSSLDHPublicKey::Probe(o) != NULL ) {
        AutoPtr<IOpenSSLDHPublicKey> other = IOpenSSLDHPublicKey::Probe(o);

        /*
         * We can shortcut the TRUE case, but it still may be equivalent but
         * different copies.
         */
        AutoPtr<IOpenSSLKey> key;
        IOpenSSLKeyHolder::Probe(other)->GetOpenSSLKey((IOpenSSLKey**)&key);
        if (mKey->Equals(key, &b), b) {
            *result = TRUE;
            return NOERROR;
        }
    }

    if (IDHPublicKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    EnsureReadParams();

    AutoPtr<IDHPublicKey> other = IDHPublicKey::Probe(o);
    AutoPtr<IBigInteger> bY;
    other->GetY((IBigInteger**)&bY);
    AutoPtr<ArrayOf<Byte> > arrayY;
    bY->ToByteArray((ArrayOf<Byte>**)&arrayY);
    if (!Arrays::Equals(mY, arrayY)) {
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

ECode OpenSSLDHPublicKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    StringBuilder sb("OpenSSLDHPublicKey{");
    sb.Append("Y=");
    AutoPtr<IBigInteger> bY;
    CBigInteger::New(*mY, (IBigInteger**)&bY);
    String str;
    bY->ToString(16, &str);
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

void OpenSSLDHPublicKey::EnsureReadParams()
{
    AutoLock lock(mParamsLock);
    if (mReadParams) {
        return;
    }

    Int64 val;
    mKey->GetPkeyContext(&val);
    AutoPtr<ArrayOf<Byte> > x;
    NativeCrypto::Get_DH_params(val, (ArrayOf<Byte>**)&mP, (ArrayOf<Byte>**)&mG,
            (ArrayOf<Byte>**)&mY, (ArrayOf<Byte>**)&x);

    mReadParams = TRUE;
}

void OpenSSLDHPublicKey::ReadObject(
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
    AutoPtr<IBigInteger> y = IBigInteger::Probe(obj);

    AutoPtr<ArrayOf<Byte> > arrayP;
    p->ToByteArray((ArrayOf<Byte>**)&arrayP);
    AutoPtr<ArrayOf<Byte> > arrayG;
    g->ToByteArray((ArrayOf<Byte>**)&arrayG);
    AutoPtr<ArrayOf<Byte> > arrayY;
    y->ToByteArray((ArrayOf<Byte>**)&arrayY);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DH(arrayP, arrayG, arrayY, NULL, &val);
    mKey = new OpenSSLKey();
    mKey->constructor(val);
}

void OpenSSLDHPublicKey::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    stream->DefaultWriteObject();

    EnsureReadParams();
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bY;
    CBigInteger::New(*mG, (IBigInteger**)&bG);
    IObjectOutput::Probe(stream)->WriteObject(bG);
    CBigInteger::New(*mP, (IBigInteger**)&bP);
    IObjectOutput::Probe(stream)->WriteObject(bP);
    CBigInteger::New(*mY, (IBigInteger**)&bY);
    IObjectOutput::Probe(stream)->WriteObject(bY);
}

} // namespace Conscrypt
} // namespace Org
