#include "org/conscrypt/COpenSSLECGroupContext.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLECPointContext.h"
#include "org/conscrypt/Platform.h"

using Elastos::Core::INumber;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Math::CBigIntegerHelper;
using Elastos::Math::IBigIntegerHelper;
using Elastos::Security::Spec::CECFieldF2m;
using Elastos::Security::Spec::CECFieldFp;
using Elastos::Security::Spec::CEllipticCurve;
using Elastos::Security::Spec::CECParameterSpec;
using Elastos::Security::Spec::IECField;
using Elastos::Security::Spec::IECFieldF2m;
using Elastos::Security::Spec::IECFieldFp;
using Elastos::Security::Spec::IECParameterSpec;
using Elastos::Security::Spec::IECPoint;
using Elastos::Security::Spec::IEllipticCurve;

namespace Org {
namespace Conscrypt {

CAR_OBJECT_IMPL(COpenSSLECGroupContext)

CAR_INTERFACE_IMPL(COpenSSLECGroupContext, Object,
        IOpenSSLECGroupContext)

COpenSSLECGroupContext::COpenSSLECGroupContext()
    : mGroupCtx(0)
{
}

COpenSSLECGroupContext::~COpenSSLECGroupContext()
{
    // try {
    if (mGroupCtx != 0) {
        NativeCrypto::EC_GROUP_clear_free(mGroupCtx);
    }
    // } finally {
    //     super.finalize();
    // }
}

ECode COpenSSLECGroupContext::constructor(
    /* [in] */ Int64 groupCtx)
{
    mGroupCtx = groupCtx;
    return NOERROR;
}

ECode COpenSSLECGroupContext::GetCurveByName(
    /* [in] */ String curveName,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    VALIDATE_NOT_NULL(result)
    // Workaround for OpenSSL not supporting SECG names for NIST P-192 and P-256
    // (aka ANSI X9.62 prime192v1 and prime256v1) curve names.
    if (curveName.Equals("secp256r1")) {
        curveName = String("prime256v1");
    }
    else if (curveName.Equals("secp192r1")) {
        curveName = String("prime192v1");
    }

    Int64 ctx;
    NativeCrypto::EC_GROUP_new_by_curve_name(curveName, &ctx);
    if (ctx == 0) {
        *result = NULL;
        return NOERROR;
    }

    NativeCrypto::EC_GROUP_set_point_conversion_form(ctx,
            INativeCrypto::POINT_CONVERSION_UNCOMPRESSED);
    NativeCrypto::EC_GROUP_set_asn1_flag(ctx, INativeCrypto::OPENSSL_EC_NAMED_CURVE);

    return COpenSSLECGroupContext::New(ctx, result);
}

ECode COpenSSLECGroupContext::GetInstance(
    /* [in] */ Int32 type,
    /* [in] */ IBigInteger* p,
    /* [in] */ IBigInteger* a,
    /* [in] */ IBigInteger* b,
    /* [in] */ IBigInteger* x,
    /* [in] */ IBigInteger* y,
    /* [in] */ IBigInteger* n,
    /* [in] */ IBigInteger* h,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > arrayP;
    AutoPtr<ArrayOf<Byte> > arrayA;
    AutoPtr<ArrayOf<Byte> > arrayB;
    p->ToByteArray((ArrayOf<Byte>**)&arrayP);
    a->ToByteArray((ArrayOf<Byte>**)&arrayA);
    b->ToByteArray((ArrayOf<Byte>**)&arrayB);
    Int64 ctx;
    NativeCrypto::EC_GROUP_new_curve(type, arrayP, arrayA,
            arrayB, &ctx);
    if (ctx == 0) {
        *result = NULL;
        return NOERROR;
    }

    NativeCrypto::EC_GROUP_set_point_conversion_form(ctx,
            INativeCrypto::POINT_CONVERSION_UNCOMPRESSED);

    AutoPtr<IOpenSSLECGroupContext> group;
    COpenSSLECGroupContext::New(ctx, (IOpenSSLECGroupContext**)&group);

    Int64 val;
    NativeCrypto::EC_POINT_new(ctx, &val);
    AutoPtr<OpenSSLECPointContext> generator = new OpenSSLECPointContext();
    generator->constructor(group, val);

    AutoPtr<ArrayOf<Byte> > arrayX;
    x->ToByteArray((ArrayOf<Byte>**)&arrayX);
    AutoPtr<ArrayOf<Byte> > arrayY;
    y->ToByteArray((ArrayOf<Byte>**)&arrayY);
    AutoPtr<ArrayOf<Byte> > arrayN;
    n->ToByteArray((ArrayOf<Byte>**)&arrayN);
    AutoPtr<ArrayOf<Byte> > arrayH;
    h->ToByteArray((ArrayOf<Byte>**)&arrayH);

    generator->GetContext(&ctx);
    NativeCrypto::EC_POINT_set_affine_coordinates(ctx, ctx, arrayX, arrayY);

    NativeCrypto::EC_GROUP_set_generator(ctx, ctx, arrayN, arrayH);

    *result = group;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode COpenSSLECGroupContext::GetInstance(
    /* [in] */ IECParameterSpec* params,
    /* [out] */ IOpenSSLECGroupContext** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    String curveName;
    Platform::GetCurveName(params, &curveName);
    if (curveName.IsNull()) {
        return COpenSSLECGroupContext::GetCurveByName(curveName, result);
    }

    AutoPtr<IEllipticCurve> curve;
    params->GetCurve((IEllipticCurve**)&curve);
    AutoPtr<IECField> field;
    curve->GetField((IECField**)&field);

    Int32 type;
    AutoPtr<IBigInteger> p;
    if (IECFieldFp::Probe(field) != NULL) {
        type = INativeCrypto::EC_CURVE_GFP;
        IECFieldFp::Probe(field)->GetP((IBigInteger**)&p);
    }
    else if (IECFieldF2m::Probe(field) != NULL) {
        type = INativeCrypto::EC_CURVE_GF2M;
        IECFieldF2m::Probe(field)->GetReductionPolynomial((IBigInteger**)&p);
    }
    else {
        // throw new InvalidParameterException("unhandled field class "
        //         + field.getClass().getName());
        return E_INVALID_PARAMETER_EXCEPTION;
    }

    AutoPtr<IECPoint> generator;
    params->GetGenerator((IECPoint**)&generator);
    AutoPtr<IBigInteger> a;
    curve->GetA((IBigInteger**)&a);
    AutoPtr<IBigInteger> b;
    curve->GetB((IBigInteger**)&b);
    AutoPtr<IBigInteger> x;
    generator->GetAffineX((IBigInteger**)&x);
    AutoPtr<IBigInteger> y;
    generator->GetAffineY((IBigInteger**)&y);
    AutoPtr<IBigInteger> n;
    params->GetOrder((IBigInteger**)&n);
    Int32 val;
    params->GetCofactor(&val);
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> h;
    helper->ValueOf(val, (IBigInteger**)&h);

    return COpenSSLECGroupContext::GetInstance(type, p, a, b,
            x, y, n, h, result);
}

ECode COpenSSLECGroupContext::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // TODO Auto-generated method stub
    *result = Object::GetHashCode(this);
    return NOERROR;
}

ECode COpenSSLECGroupContext::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IOpenSSLECGroupContext::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<COpenSSLECGroupContext> other = (COpenSSLECGroupContext*)IOpenSSLECGroupContext::Probe(o);
    return NativeCrypto::EC_GROUP_cmp(mGroupCtx, other->mGroupCtx, result);
}

ECode COpenSSLECGroupContext::GetContext(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGroupCtx;
    return NOERROR;
}

ECode COpenSSLECGroupContext::GetECParameterSpec(
    /* [out] */ IECParameterSpec** result)
{
    VALIDATE_NOT_NULL(result)
    String curveName;
    NativeCrypto::EC_GROUP_get_curve_name(mGroupCtx, &curveName);

    AutoPtr<ArrayOf<Byte> > arrayP;
    AutoPtr<ArrayOf<Byte> > arrayA;
    AutoPtr<ArrayOf<Byte> > arrayB;
    NativeCrypto::EC_GROUP_get_curve(mGroupCtx, (ArrayOf<Byte>**)&arrayP,
            (ArrayOf<Byte>**)&arrayA, (ArrayOf<Byte>**)&arrayB);

    AutoPtr<IBigInteger> p;
    CBigInteger::New(*arrayP, (IBigInteger**)&p);
    AutoPtr<IBigInteger> a;
    CBigInteger::New(*arrayA, (IBigInteger**)&a);
    AutoPtr<IBigInteger> b;
    CBigInteger::New(*arrayB, (IBigInteger**)&b);

    AutoPtr<IECField> field;
    Int32 type;
    NativeCrypto::Get_EC_GROUP_type(mGroupCtx, &type);
    if (type == INativeCrypto::EC_CURVE_GFP) {
        CECFieldFp::New(p, (IECField**)&field);
    }
    else if (type == INativeCrypto::EC_CURVE_GF2M) {
        Int32 len;
        p->BitLength(&len);
        CECFieldF2m::New(len - 1, p, (IECField**)&field);
    }
    else {
        // throw new RuntimeException("unknown curve type " + type);
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IEllipticCurve> curve;
    CEllipticCurve::New(field, a, b, (IEllipticCurve**)&curve);

    Int64 val;
    NativeCrypto::EC_GROUP_get_generator(mGroupCtx, &val);
    AutoPtr<OpenSSLECPointContext> generatorCtx = new OpenSSLECPointContext();
    generatorCtx->constructor(this, val);
    AutoPtr<IECPoint> generator;
    generatorCtx->GetECPoint((IECPoint**)&generator);

    AutoPtr<ArrayOf<Byte> > array;
    NativeCrypto::EC_GROUP_get_order(mGroupCtx, (ArrayOf<Byte>**)&array);
    AutoPtr<IBigInteger> order;
    CBigInteger::New(*array, (IBigInteger**)&order);
    array = NULL;
    NativeCrypto::EC_GROUP_get_cofactor(mGroupCtx, (ArrayOf<Byte>**)&array);
    AutoPtr<IBigInteger> cofactor;
    CBigInteger::New(*array, (IBigInteger**)&cofactor);

    Int32 val32;
    INumber::Probe(cofactor)->Int32Value(&val32);
    AutoPtr<IECParameterSpec> spec;
    CECParameterSpec::New(curve, generator, order, val32, (IECParameterSpec**)&spec);
    Platform::SetCurveName(spec, curveName);
    *result = spec;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
