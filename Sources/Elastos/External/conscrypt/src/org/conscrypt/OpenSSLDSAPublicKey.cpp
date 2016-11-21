#include "org/conscrypt/OpenSSLDSAPublicKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLKey.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;
using Elastos::Core::StringBuilder;
using Elastos::Math::CBigInteger;
using Elastos::Security::Interfaces::EIID_IDSAKey;
using Elastos::Security::Interfaces::EIID_IDSAPublicKey;
using Elastos::Security::EIID_IPublicKey;
using Elastos::Security::EIID_IKey;

namespace Org {
namespace Conscrypt {

const Int64 OpenSSLDSAPublicKey::serialVersionUID;

CAR_INTERFACE_IMPL_6(OpenSSLDSAPublicKey, Object, IOpenSSLDSAPublicKey,
        IDSAPublicKey, IDSAKey, IKey, IPublicKey, IOpenSSLKeyHolder)

OpenSSLDSAPublicKey::OpenSSLDSAPublicKey()
{
}

ECode OpenSSLDSAPublicKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    mKey = (OpenSSLKey*)key;
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::constructor(
    /* [in] */ IDSAPublicKeySpec* dsaKeySpec)
{
    // try {
    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bQ;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bY;
    dsaKeySpec->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dsaKeySpec->GetQ((IBigInteger**)&bQ);
    AutoPtr<ArrayOf<Byte> > arrayQ;
    bQ->ToByteArray((ArrayOf<Byte>**)&arrayQ);
    dsaKeySpec->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dsaKeySpec->GetY((IBigInteger**)&bY);
    AutoPtr<ArrayOf<Byte> > arrayY;
    bY->ToByteArray((ArrayOf<Byte>**)&arrayY);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DSA(
            arrayP, arrayQ, arrayG, arrayY, NULL, &val);
    mKey = new OpenSSLKey();
    mKey->constructor(val);
    // } catch (Exception e) {
    //     throw new InvalidKeySpecException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IOpenSSLKey::Probe(mKey);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::GetInstance(
    /* [in] */ IDSAPublicKey* dsaPublicKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IDSAParams> dsaParams;
    IDSAKey::Probe(dsaPublicKey)->GetParams((IDSAParams**)&dsaParams);

    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bQ;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bY;
    dsaParams->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dsaParams->GetQ((IBigInteger**)&bQ);
    AutoPtr<ArrayOf<Byte> > arrayQ;
    bQ->ToByteArray((ArrayOf<Byte>**)&arrayQ);
    dsaParams->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dsaPublicKey->GetY((IBigInteger**)&bY);
    AutoPtr<ArrayOf<Byte> > arrayY;
    bY->ToByteArray((ArrayOf<Byte>**)&arrayY);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DSA(
                arrayP, arrayQ, arrayG, arrayY, NULL, &val);
    AutoPtr<OpenSSLKey> key = new OpenSSLKey();
    key->constructor(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new InvalidKeyException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::GetParams(
    /* [out] */ IDSAParams** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    /*
     * DSA keys can lack parameters if they're part of a certificate
     * chain. In this case, we just return null.
     */
    Boolean b;
    if (mParams->HasParams(&b), !b) {
        *result = NULL;
        return NOERROR;
    }

    *result = mParams;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("DSA");
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("X.509");
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    return NativeCrypto::I2d_PUBKEY(ctx, result);
}

ECode OpenSSLDSAPublicKey::GetY(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    return mParams->GetY(result);
}

ECode OpenSSLDSAPublicKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean b;
    if (IOpenSSLDSAPublicKey::Probe(o) != NULL ) {
        AutoPtr<IOpenSSLDSAPublicKey> other = IOpenSSLDSAPublicKey::Probe(o);

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

    if (IDSAPublicKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    EnsureReadParams();

    AutoPtr<IDSAPublicKey> other = IDSAPublicKey::Probe(o);

    AutoPtr<IBigInteger> bY;
    other->GetY((IBigInteger**)&bY);
    AutoPtr<IDSAParams> params;
    IDSAKey::Probe(other)->GetParams((IDSAParams**)&params);
    AutoPtr<IBigInteger> y;
    mParams->GetY((IBigInteger**)&y);

    Boolean b1, b2;
    IObject::Probe(y)->Equals(bY, &b1);
    mParams->Equals(params, &b2);
    *result = b1 && b2;
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    AutoPtr<IBigInteger> y;
    mParams->GetY((IBigInteger**)&y);
    Int32 yCode;
    IObject::Probe(y)->GetHashCode(&yCode);

    Int32 paramsCode;
    mParams->GetHashCode(&paramsCode);

    *result = yCode ^ paramsCode;
    return NOERROR;
}

ECode OpenSSLDSAPublicKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    StringBuilder sb("OpenSSLDSAPublicKey{");
    sb.Append("Y=");
    AutoPtr<IBigInteger> bY;
    mParams->GetY((IBigInteger**)&bY);
    String str;
    bY->ToString(16, &str);
    sb.Append(str);
    sb.Append(',');
    sb.Append("params=");
    mParams->ToString(&str);
    sb.Append(str);
    sb.Append('}');

    return sb.ToString(result);
}

void OpenSSLDSAPublicKey::EnsureReadParams()
{
    if (mParams == NULL) {
        mParams = new OpenSSLDSAParams();
        mParams->constructor(IOpenSSLKey::Probe(mKey));
    }
}

void OpenSSLDSAPublicKey::ReadObject(
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
    AutoPtr<IBigInteger> q = IBigInteger::Probe(obj);
    obj = NULL;
    IObjectInput::Probe(stream)->ReadObject((IInterface**)&obj);
    AutoPtr<IBigInteger> y = IBigInteger::Probe(obj);

    AutoPtr<ArrayOf<Byte> > arrayP;
    p->ToByteArray((ArrayOf<Byte>**)&arrayP);
    AutoPtr<ArrayOf<Byte> > arrayG;
    g->ToByteArray((ArrayOf<Byte>**)&arrayG);
    AutoPtr<ArrayOf<Byte> > arrayQ;
    q->ToByteArray((ArrayOf<Byte>**)&arrayQ);
    AutoPtr<ArrayOf<Byte> > arrayY;
    y->ToByteArray((ArrayOf<Byte>**)&arrayY);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DSA(arrayP, arrayQ, arrayG, arrayY, NULL, &val);
    mKey = new OpenSSLKey();
    mKey->constructor(val);
}

ECode OpenSSLDSAPublicKey::WriteObject(
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
    AutoPtr<IBigInteger> bQ;
    AutoPtr<IBigInteger> bY;
    mParams->GetG((IBigInteger**)&bG);
    IObjectOutput::Probe(stream)->WriteObject(bG);
    mParams->GetP((IBigInteger**)&bP);
    IObjectOutput::Probe(stream)->WriteObject(bP);
    mParams->GetQ((IBigInteger**)&bQ);
    IObjectOutput::Probe(stream)->WriteObject(bQ);
    mParams->GetY((IBigInteger**)&bY);
    IObjectOutput::Probe(stream)->WriteObject(bY);

    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
