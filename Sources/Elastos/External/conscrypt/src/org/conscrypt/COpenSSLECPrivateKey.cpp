#include "org/conscrypt/COpenSSLECPrivateKey.h"
#include "org/conscrypt/COpenSSLECGroupContext.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/utility/Arrays.h>

using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;
using Elastos::Math::CBigInteger;
using Elastos::Security::IKey;
using Elastos::Security::Interfaces::EIID_IECPrivateKey;
using Elastos::Security::Interfaces::IECKey;
using Elastos::Security::Spec::IECPoint;
using Elastos::Security::Spec::IEllipticCurve;
using Elastos::Utility::Arrays;

namespace Org {
namespace Conscrypt {

const Int64 COpenSSLECPrivateKey::serialVersionUID;

const String COpenSSLECPrivateKey::ALGORITHM("EC");

CAR_OBJECT_IMPL(COpenSSLECPrivateKey)

CAR_INTERFACE_IMPL_3(COpenSSLECPrivateKey, Object, IOpenSSLECPrivateKey,
    IECPrivateKey, IOpenSSLKeyHolder)

ECode COpenSSLECPrivateKey::constructor()
{
    return NOERROR;
}

ECode COpenSSLECPrivateKey::constructor(
    /* [in] */ IOpenSSLECGroupContext* group,
    /* [in] */ IOpenSSLKey* key)
{
    mGroup = group;
    mKey = (OpenSSLKey*)key;
    return NOERROR;
}

ECode COpenSSLECPrivateKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    Int64 ctx;
    key->GetPkeyContext(&ctx);
    Int64 origGroup;
    NativeCrypto::EC_KEY_get0_group(ctx, &origGroup);
    Int64 val;
    NativeCrypto::EC_GROUP_dup(origGroup, &val);
    COpenSSLECGroupContext::New(val, (IOpenSSLECGroupContext**)&mGroup);
    mKey = (OpenSSLKey*)key;
    return NOERROR;
}

ECode COpenSSLECPrivateKey::constructor(
    /* [in] */ IECPrivateKeySpec* ecKeySpec)
{
    // try {
    AutoPtr<IECParameterSpec> spec;
    ecKeySpec->GetParams((IECParameterSpec**)&spec);
    COpenSSLECGroupContext::GetInstance(spec, (IOpenSSLECGroupContext**)&mGroup);

    AutoPtr<IBigInteger> privKey;
    ecKeySpec->GetS((IBigInteger**)&privKey);
    Int64 ctx;
    mGroup->GetContext(&ctx);
    AutoPtr<ArrayOf<Byte> > array;
    privKey->ToByteArray((ArrayOf<Byte>**)&array);
    Int64 val;
    NativeCrypto::EVP_PKEY_new_EC_KEY(ctx, 0, array, &val);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(val);
    // } catch (Exception e) {
    //     throw new InvalidKeySpecException(e);
    // }
    return NOERROR;
}

ECode COpenSSLECPrivateKey::WrapPlatformKey(
    /* [in] */ IECPrivateKey* ecPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLECGroupContext> group;
    // try {
    AutoPtr<IECParameterSpec> spec;
    IECKey::Probe(ecPrivateKey)->GetParams((IECParameterSpec**)&spec);
    COpenSSLECGroupContext::GetInstance(spec, (IOpenSSLECGroupContext**)&group);
    // } catch (InvalidAlgorithmParameterException e) {
    //     throw new InvalidKeyException("Unknown group parameters", e);
    // }
    return WrapPlatformKey(ecPrivateKey, group, result);
}

ECode COpenSSLECPrivateKey::GetInstance(
    /* [in] */ IECPrivateKey* ecPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IECParameterSpec> spec;
    IECKey::Probe(ecPrivateKey)->GetParams((IECParameterSpec**)&spec);
    AutoPtr<IOpenSSLECGroupContext> group;
    COpenSSLECGroupContext::GetInstance(spec, (IOpenSSLECGroupContext**)&group);

    /**
     * If the key is not encodable (PKCS11-like key), then wrap it and
     * use JNI upcalls to satisfy requests.
     */
    String format;
    IKey::Probe(ecPrivateKey)->GetFormat(&format);
    if (format.IsNull()) {
        return WrapPlatformKey(ecPrivateKey, group, result);
    }

    AutoPtr<IBigInteger> privKey;
    ecPrivateKey->GetS((IBigInteger**)&privKey);
    Int64 ctx;
    group->GetContext(&ctx);
    AutoPtr<ArrayOf<Byte> > array;
    privKey->ToByteArray((ArrayOf<Byte>**)&array);
    Int64 val;
    NativeCrypto::EVP_PKEY_new_EC_KEY(ctx, 0, array, &val);
    assert(0 && "TODO");
    AutoPtr<OpenSSLKey> key;// = new OpenSSLKey(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new InvalidKeyException(e);
    // }
    return NOERROR;
}

ECode COpenSSLECPrivateKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ALGORITHM;
    return NOERROR;
}

ECode COpenSSLECPrivateKey::GetFormat(
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
        *result = NULL;
        return NOERROR;
    }

    *result = String("PKCS#8");
    return NOERROR;
}

ECode COpenSSLECPrivateKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * If we're using an OpenSSL ENGINE, there's no guarantee we can export
     * the key. Returning {@code null} tells the caller that there's no
     * encoded format.
     */
     Boolean b;
     if (mKey->IsEngineBased(&b), b) {
        *result = NULL;
        return NOERROR;
    }

    Int64 val;
    mKey->GetPkeyContext(&val);
    return NativeCrypto::I2d_PKCS8_PRIV_KEY_INFO(val, result);
}

ECode COpenSSLECPrivateKey::GetParams(
    /* [out] */ IECParameterSpec** result)
{
    VALIDATE_NOT_NULL(result)
    return mGroup->GetECParameterSpec(result);
}

ECode COpenSSLECPrivateKey::GetS(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        // throw new UnsupportedOperationException("private key value S cannot be extracted");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *result = GetPrivateKey();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode COpenSSLECPrivateKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IOpenSSLKey::Probe(mKey);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode COpenSSLECPrivateKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    if (IOpenSSLECPrivateKey::Probe(o) != NULL) {
        AutoPtr<COpenSSLECPrivateKey> other =
                (COpenSSLECPrivateKey*)IOpenSSLECPrivateKey::Probe(o);
        *result = Object::Equals(mKey, other->mKey);
        return NOERROR;
    }

    if (IECPrivateKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IECPrivateKey> other = IECPrivateKey::Probe(o);
    AutoPtr<IBigInteger> bi = GetPrivateKey();
    AutoPtr<IBigInteger> s;
    other->GetS((IBigInteger**)&s);

    Boolean b;
    if (IObject::Probe(bi)->Equals(s, &b), !b) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IECParameterSpec> spec;
    GetParams((IECParameterSpec**)&spec);
    AutoPtr<IECParameterSpec> otherSpec;
    IECKey::Probe(other)->GetParams((IECParameterSpec**)&otherSpec);

    Boolean b1, b2, b3;
    AutoPtr<IEllipticCurve> curve;
    spec->GetCurve((IEllipticCurve**)&curve);
    AutoPtr<IEllipticCurve> curve2;
    otherSpec->GetCurve((IEllipticCurve**)&curve2);
    IObject::Probe(curve)->Equals(curve2, &b1);

    AutoPtr<IECPoint> point;
    spec->GetGenerator((IECPoint**)&point);
    AutoPtr<IECPoint> point2;
    otherSpec->GetGenerator((IECPoint**)&point2);
    IObject::Probe(point)->Equals(point2, &b2);

    AutoPtr<IBigInteger> order;
    spec->GetOrder((IBigInteger**)&order);
    AutoPtr<IBigInteger> order2;
    otherSpec->GetOrder((IBigInteger**)&order2);
    IObject::Probe(order)->Equals(order2, &b3);

    Int32 cofactor;
    spec->GetCofactor(&cofactor);
    Int32 cofactor2;
    otherSpec->GetCofactor(&cofactor2);

    *result = b1 && b2 && b3 && (cofactor == cofactor2);
    return NOERROR;
}

ECode COpenSSLECPrivateKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    AutoPtr<ArrayOf<Byte> > array;
    NativeCrypto::I2d_PKCS8_PRIV_KEY_INFO(ctx, (ArrayOf<Byte>**)&array);
    *result = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode COpenSSLECPrivateKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    return NativeCrypto::EVP_PKEY_print_private(ctx, result);
}

ECode COpenSSLECPrivateKey::WrapPlatformKey(
    /* [in] */ IECPrivateKey* ecPrivateKey,
    /* [in] */ IOpenSSLECGroupContext* group,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    group->GetContext(&ctx);
    Int64 val;
    NativeCrypto::GetECPrivateKeyWrapper(ecPrivateKey, ctx, &val);
    assert(0 && "TODO");
    AutoPtr<OpenSSLKey> key;// = new OpenSSLKey(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IBigInteger> COpenSSLECPrivateKey::GetPrivateKey()
{
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    AutoPtr<ArrayOf<Byte> > array;
    NativeCrypto::EC_KEY_get_private_key(ctx, (ArrayOf<Byte>**)&array);

    AutoPtr<IBigInteger> bi;
    CBigInteger::New(*array, (IBigInteger**)&bi);
    return bi;
}

void COpenSSLECPrivateKey::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();

    AutoPtr<IInterface> obj;
    IObjectInput::Probe(stream)->ReadObject((IInterface**)&obj);
    AutoPtr<IArrayOf> iArray = IArrayOf::Probe(obj);

    Int32 len;
    iArray->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > encoded = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        iArray->Get(i, (IInterface**)&o);
        IByte::Probe(o)->GetValue(&(*encoded)[i]);
    }

    Int64 val;
    NativeCrypto::D2i_PKCS8_PRIV_KEY_INFO(encoded, &val);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(val);

    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    Int64 origGroup;
    NativeCrypto::EC_KEY_get0_group(ctx, &origGroup);

    NativeCrypto::EC_GROUP_dup(origGroup, &val);
    COpenSSLECGroupContext::New(val, (IOpenSSLECGroupContext**)&mGroup);
}

ECode COpenSSLECPrivateKey::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    Boolean b;
    if (mKey->IsEngineBased(&b), b) {
        // throw new NotSerializableException("engine-based keys can not be serialized");
        return E_NOT_SERIALIZABLE_EXCEPTION;
    }

    stream->DefaultWriteObject();
    AutoPtr<ArrayOf<Byte> > array;
    GetEncoded((ArrayOf<Byte>**)&array);
    AutoPtr<IBigInteger> bi;
    CBigInteger::New(*array, (IBigInteger**)&bi);
    IObjectOutput::Probe(stream)->WriteObject(bi);
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
