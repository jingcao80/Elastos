#include "org/conscrypt/OpenSSLDSAPrivateKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>

using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;
using Elastos::Core::StringBuilder;
using Elastos::Math::CBigInteger;
using Elastos::Security::EIID_IPrivateKey;
using Elastos::Security::IKey;
using Elastos::Security::Interfaces::EIID_IDSAKey;
using Elastos::Security::Interfaces::EIID_IDSAPrivateKey;

namespace Org {
namespace Conscrypt {

const Int64 OpenSSLDSAPrivateKey::serialVersionUID;

CAR_INTERFACE_IMPL_5(OpenSSLDSAPrivateKey, Object, IOpenSSLDSAPrivateKey,
        IDSAPrivateKey, IDSAKey, IPrivateKey, IOpenSSLKeyHolder)

OpenSSLDSAPrivateKey::OpenSSLDSAPrivateKey()
{
}

ECode OpenSSLDSAPrivateKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    mKey = (OpenSSLKey*)key;
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::constructor(
    /* [in] */ IDSAPrivateKeySpec* dsaKeySpec)
{
    // try {
    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bQ;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bX;
    dsaKeySpec->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dsaKeySpec->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dsaKeySpec->GetQ((IBigInteger**)&bQ);
    AutoPtr<ArrayOf<Byte> > arrayQ;
    bQ->ToByteArray((ArrayOf<Byte>**)&arrayQ);
    dsaKeySpec->GetX((IBigInteger**)&bX);
    AutoPtr<ArrayOf<Byte> > arrayX;
    bX->ToByteArray((ArrayOf<Byte>**)&arrayX);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DSA(
            arrayP, arrayQ, arrayG, NULL, arrayX, &val);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(val);
    // } catch (Exception e) {
    //     throw new InvalidKeySpecException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IOpenSSLKey::Probe(mKey);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::GetInstance(
    /* [in] */ IDSAPrivateKey* dsaPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    /**
     * If the key is not encodable (PKCS11-like key), then wrap it and use
     * JNI upcalls to satisfy requests.
     */
    String format;
    IKey::Probe(dsaPrivateKey)->GetFormat(&format);
    if (format.IsNull()) {
        return WrapPlatformKey(dsaPrivateKey, result);
    }

    // try {
    AutoPtr<IDSAParams> dsaParams;
    IDSAKey::Probe(dsaPrivateKey)->GetParams((IDSAParams**)&dsaParams);

    AutoPtr<IBigInteger> bP;
    AutoPtr<IBigInteger> bG;
    AutoPtr<IBigInteger> bQ;
    dsaParams->GetP((IBigInteger**)&bP);
    AutoPtr<ArrayOf<Byte> > arrayP;
    bP->ToByteArray((ArrayOf<Byte>**)&arrayP);
    dsaParams->GetG((IBigInteger**)&bG);
    AutoPtr<ArrayOf<Byte> > arrayG;
    bG->ToByteArray((ArrayOf<Byte>**)&arrayG);
    dsaParams->GetQ((IBigInteger**)&bQ);
    AutoPtr<ArrayOf<Byte> > arrayQ;
    bQ->ToByteArray((ArrayOf<Byte>**)&arrayQ);

    AutoPtr<IBigInteger> bX;
    dsaPrivateKey->GetX((IBigInteger**)&bX);
    AutoPtr<ArrayOf<Byte> > arrayX;
    bX->ToByteArray((ArrayOf<Byte>**)&arrayX);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DSA(
                arrayP, arrayQ, arrayG, NULL, arrayX, &val);
    assert(0 && "TODO");
    AutoPtr<OpenSSLKey> key;// = new OpenSSLKey(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new InvalidKeyException(e);
    // }
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::WrapPlatformKey(
    /* [in] */ IDSAPrivateKey* dsaPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 val;
    NativeCrypto::GetDSAPrivateKeyWrapper(dsaPrivateKey, &val);
    assert(0 && "TODO");
    AutoPtr<OpenSSLKey> key;// = new OpenSSLKey(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::GetParams(
    /* [out] */ IDSAParams** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mParams;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("DSA");
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * If we're using an OpenSSL ENGINE, there's no guarantee we can export
     * the key. Returning {@code null} tells the caller that there's no
     * encoded format.
     */
    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        *result = String(NULL);
    }

    *result = String("PKCS#8");
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::GetEncoded(
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

ECode OpenSSLDSAPrivateKey::GetX(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        // throw new UnsupportedOperationException("private key value X cannot be extracted");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    EnsureReadParams();
    return mParams->GetX(result);
}

ECode OpenSSLDSAPrivateKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean b;
    if (IOpenSSLDSAPrivateKey::Probe(o) != NULL) {
        AutoPtr<IOpenSSLDSAPrivateKey> other = IOpenSSLDSAPrivateKey::Probe(o);

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

    if (IDSAPrivateKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    EnsureReadParams();

    AutoPtr<IBigInteger> x;
    mParams->GetX((IBigInteger**)&x);
    if (x == NULL) {
        /*
         * If our X is null, we can't tell if these two private keys are
         * equivalent. This usually happens if this key is ENGINE-based. If
         * the other key was ENGINE-based, we should have caught it in the
         * OpenSSLDSAPrivateKey case.
         */
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IDSAPrivateKey> other = IDSAPrivateKey::Probe(o);
    AutoPtr<IBigInteger> bX;
    other->GetX((IBigInteger**)&bX);
    AutoPtr<IDSAParams> params;
    IDSAKey::Probe(other)->GetParams((IDSAParams**)&params);
    Boolean b1;
    IObject::Probe(x)->Equals(bX, &b1);
    Boolean b2;
    mParams->Equals(params, &b2);

    *result = b1 && b2;
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    Int32 hash = 1;

    AutoPtr<IBigInteger> x;
    GetX((IBigInteger**)&x);
    if (x != NULL) {
        Int32 xCode;
        IObject::Probe(x)->GetHashCode(&xCode);
        hash = hash * 3 + xCode;
    }

    Int32 paramsCode;
    mParams->GetHashCode(&paramsCode);
    hash = hash * 7 + paramsCode;

    *result = hash;
    return NOERROR;
}

ECode OpenSSLDSAPrivateKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("OpenSSLDSAPrivateKey{");

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
    mParams->GetX((IBigInteger**)&bX);
    String str;
    bX->ToString(16, &str);
    sb.Append(str);
    sb.Append(',');
    sb.Append("params=");
    mParams->ToString(&str);
    sb.Append(str);
    sb.Append('}');

    return sb.ToString(result);
}

void OpenSSLDSAPrivateKey::EnsureReadParams()
{
    if (mParams == NULL) {
        mParams = new OpenSSLDSAParams();
        mParams->constructor(IOpenSSLKey::Probe(mKey));
    }
}

void OpenSSLDSAPrivateKey::ReadObject(
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
    AutoPtr<IBigInteger> x = IBigInteger::Probe(obj);

    AutoPtr<ArrayOf<Byte> > arrayP;
    p->ToByteArray((ArrayOf<Byte>**)&arrayP);
    AutoPtr<ArrayOf<Byte> > arrayG;
    g->ToByteArray((ArrayOf<Byte>**)&arrayG);
    AutoPtr<ArrayOf<Byte> > arrayQ;
    q->ToByteArray((ArrayOf<Byte>**)&arrayQ);
    AutoPtr<ArrayOf<Byte> > arrayX;
    x->ToByteArray((ArrayOf<Byte>**)&arrayX);

    Int64 val;
    NativeCrypto::EVP_PKEY_new_DSA(arrayP, arrayQ, arrayG, NULL, arrayX, &val);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(val);
}

ECode OpenSSLDSAPrivateKey::WriteObject(
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
    AutoPtr<IBigInteger> bX;
    mParams->GetG((IBigInteger**)&bG);
    IObjectOutput::Probe(stream)->WriteObject(bG);
    mParams->GetP((IBigInteger**)&bP);
    IObjectOutput::Probe(stream)->WriteObject(bP);
    mParams->GetQ((IBigInteger**)&bQ);
    IObjectOutput::Probe(stream)->WriteObject(bQ);
    mParams->GetX((IBigInteger**)&bX);
    IObjectOutput::Probe(stream)->WriteObject(bX);

    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
