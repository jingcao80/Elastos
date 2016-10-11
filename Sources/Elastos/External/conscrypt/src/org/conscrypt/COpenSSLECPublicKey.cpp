#include "org/conscrypt/COpenSSLECPublicKey.h"
#include "org/conscrypt/COpenSSLECGroupContext.h"
#include "org/conscrypt/OpenSSLECPointContext.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/utility/Arrays.h>

using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;
using Elastos::Math::CBigInteger;
using Elastos::Security::Interfaces::EIID_IECPublicKey;
using Elastos::Security::Interfaces::IECKey;
using Elastos::Security::Spec::IECPoint;
using Elastos::Security::Spec::IEllipticCurve;
using Elastos::Utility::Arrays;

namespace Org {
namespace Conscrypt {

const Int64 COpenSSLECPublicKey::serialVersionUID;

const String COpenSSLECPublicKey::ALGORITHM("EC");

CAR_OBJECT_IMPL(COpenSSLECPublicKey)

CAR_INTERFACE_IMPL_3(COpenSSLECPublicKey, Object, IOpenSSLECPublicKey,
    IECPublicKey, IOpenSSLKeyHolder)

ECode COpenSSLECPublicKey::constructor()
{
    return NOERROR;
}

ECode COpenSSLECPublicKey::constructor(
    /* [in] */ IOpenSSLECGroupContext* group,
    /* [in] */ IOpenSSLKey* key)
{
    mGroup = group;
    mKey = (OpenSSLKey*)key;
    return NOERROR;
}

ECode COpenSSLECPublicKey::constructor(
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

ECode COpenSSLECPublicKey::constructor(
    /* [in] */ IECPublicKeySpec* ecKeySpec)
{
    // try {
    AutoPtr<IECParameterSpec> spec;
    ecKeySpec->GetParams((IECParameterSpec**)&spec);
    COpenSSLECGroupContext::GetInstance(spec, (IOpenSSLECGroupContext**)&mGroup);

    Int64 ctx;
    mGroup->GetContext(&ctx);
    Int32 type;
    NativeCrypto::Get_EC_GROUP_type(ctx, &type);
    AutoPtr<IECPoint> w;
    ecKeySpec->GetW((IECPoint**)&w);
    AutoPtr<IOpenSSLECPointContext> pubKey;
    OpenSSLECPointContext::GetInstance(type, mGroup, w, (IOpenSSLECPointContext**)&pubKey);
    Int64 pubKeyCtx;
    pubKey->GetContext(&pubKeyCtx);
    Int64 val;
    NativeCrypto::EVP_PKEY_new_EC_KEY(ctx, pubKeyCtx, NULL, &val);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(val);
    // } catch (Exception e) {
    //     throw new InvalidKeySpecException(e);
    // }
    return NOERROR;
}

ECode COpenSSLECPublicKey::GetInstance(
    /* [in] */ IECPublicKey* ecPublicKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IECParameterSpec> spec;
    IECKey::Probe(ecPublicKey)->GetParams((IECParameterSpec**)&spec);
    AutoPtr<IOpenSSLECGroupContext> group;
    COpenSSLECGroupContext::GetInstance(spec, (IOpenSSLECGroupContext**)&group);

    Int64 ctx;
    group->GetContext(&ctx);
    Int32 type;
    NativeCrypto::Get_EC_GROUP_type(ctx, &type);
    AutoPtr<IECPoint> point;
    ecPublicKey->GetW((IECPoint**)&point);
    AutoPtr<IOpenSSLECPointContext> pubKey;
    OpenSSLECPointContext::GetInstance(type, group, point, (IOpenSSLECPointContext**)&pubKey);
    Int64 pubKeyCtx;
    pubKey->GetContext(&pubKeyCtx);
    Int64 val;
    NativeCrypto::EVP_PKEY_new_EC_KEY(ctx, pubKeyCtx, NULL, &val);
    assert(0 && "TODO");
    AutoPtr<OpenSSLKey> key;// = new OpenSSLKey(val);
    *result = IOpenSSLKey::Probe(key);
    REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new InvalidKeyException(e);
    // }
    return NOERROR;
}

ECode COpenSSLECPublicKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ALGORITHM;
    return NOERROR;
}

ECode COpenSSLECPublicKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = "X.509";
    return NOERROR;
}

ECode COpenSSLECPublicKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    return NativeCrypto::I2d_PUBKEY(ctx, result);
}

ECode COpenSSLECPublicKey::GetParams(
    /* [out] */ IECParameterSpec** result)
{
    VALIDATE_NOT_NULL(result)
    return mGroup->GetECParameterSpec(result);
}

ECode COpenSSLECPublicKey::GetW(
    /* [out] */ IECPoint** result)
{
    VALIDATE_NOT_NULL(result)
    *result = GetPublicKey();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode COpenSSLECPublicKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IOpenSSLKey::Probe(mKey);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode COpenSSLECPublicKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    if (IOpenSSLECPublicKey::Probe(o) != NULL) {
        AutoPtr<COpenSSLECPublicKey> other =
                (COpenSSLECPublicKey*)IOpenSSLECPublicKey::Probe(o);
        *result = Object::Equals(mKey, other->mKey);
        return NOERROR;
    }

    if (IECPublicKey::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IECPublicKey> other = IECPublicKey::Probe(o);
    AutoPtr<IECPoint> pk = GetPublicKey();
    AutoPtr<IECPoint> w;
    other->GetW((IECPoint**)&w);

    Boolean b;
    if (IObject::Probe(pk)->Equals(w, &b), !b) {
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

ECode COpenSSLECPublicKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    AutoPtr<ArrayOf<Byte> > array;
    NativeCrypto::I2d_PUBKEY(ctx, (ArrayOf<Byte>**)&array);
    *result = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode COpenSSLECPublicKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    return NativeCrypto::EVP_PKEY_print_public(ctx, result);
}

AutoPtr<IECPoint> COpenSSLECPublicKey::GetPublicKey()
{
    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    Int64 val;
    NativeCrypto::EC_KEY_get_public_key(ctx, &val);
    AutoPtr<OpenSSLECPointContext> pubKey = new OpenSSLECPointContext();
    pubKey->constructor(mGroup, val);

    AutoPtr<IECPoint> point;
    pubKey->GetECPoint((IECPoint**)&point);
    return point;
}

void COpenSSLECPublicKey::ReadObject(
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
    NativeCrypto::D2i_PUBKEY(encoded, &val);
    assert(0 && "TODO");
    // mKey = new OpenSSLKey(val);

    Int64 ctx;
    mKey->GetPkeyContext(&ctx);
    Int64 origGroup;
    NativeCrypto::EC_KEY_get0_group(ctx, &origGroup);

    NativeCrypto::EC_GROUP_dup(origGroup, &val);
    COpenSSLECGroupContext::New(val, (IOpenSSLECGroupContext**)&mGroup);
}

ECode COpenSSLECPublicKey::WriteObject(
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
