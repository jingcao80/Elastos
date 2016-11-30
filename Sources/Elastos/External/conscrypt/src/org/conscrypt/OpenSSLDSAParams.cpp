#include "org/conscrypt/OpenSSLDSAParams.h"
#include "org/conscrypt/NativeCrypto.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Math::CBigInteger;
using Elastos::Security::Interfaces::EIID_IDSAParams;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL_3(OpenSSLDSAParams, Object, IOpenSSLDSAParams,
        IDSAParams, IAlgorithmParameterSpec)

OpenSSLDSAParams::OpenSSLDSAParams()
    : mFetchedParams(FALSE)
{
}

ECode OpenSSLDSAParams::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    mKey = key;
    return NOERROR;
}

ECode OpenSSLDSAParams::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mKey;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAParams::GetG(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mG;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAParams::GetP(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mP;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAParams::GetQ(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mQ;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAParams::HasParams(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = (mG != NULL) && (mP != NULL) && (mQ != NULL);
    return NOERROR;
}

ECode OpenSSLDSAParams::GetY(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mY;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAParams::GetX(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mX;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLDSAParams::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == o) {
        *result = TRUE;
        return NOERROR;
    }

    if (IOpenSSLDSAParams::Probe(o) != NULL) {
        AutoPtr<IOpenSSLDSAParams> other = IOpenSSLDSAParams::Probe(o);

        /*
         * We can shortcut the true case, but it still may be equivalent but
         * different copies.
         */
        AutoPtr<IOpenSSLKey> key;
        ((OpenSSLDSAParams*)other.Get())->GetOpenSSLKey((IOpenSSLKey**)&key);
        if (mKey == key) {
            *result = TRUE;
            return NOERROR;
        }
    }

    if (IDSAParams::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    EnsureReadParams();

    AutoPtr<IDSAParams> other = IDSAParams::Probe(o);
    AutoPtr<IBigInteger> g;
    other->GetG((IBigInteger**)&g);
    AutoPtr<IBigInteger> p;
    other->GetP((IBigInteger**)&p);
    AutoPtr<IBigInteger> q;
    other->GetQ((IBigInteger**)&q);

    Boolean b1, b2, b3;
    IObject::Probe(mG)->Equals(g, &b1);
    IObject::Probe(mP)->Equals(p, &b2);
    IObject::Probe(mQ)->Equals(q, &b3);
    *result = b1 && b2 && b3;
    return NOERROR;
}

ECode OpenSSLDSAParams::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    Int32 gCode, pCode, qCode;
    IObject::Probe(mG)->GetHashCode(&gCode);
    IObject::Probe(mP)->GetHashCode(&pCode);
    IObject::Probe(mQ)->GetHashCode(&qCode);
    *result = gCode ^ pCode ^ qCode;
    return NOERROR;
}

ECode OpenSSLDSAParams::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();

    StringBuilder sb("OpenSSLDSAParams{");
    sb.Append("G=");
    String str;
    sb.Append((mG->ToString(16, &str), str));
    sb.Append(",P=");
    sb.Append((mP->ToString(16, &str), str));
    sb.Append(",Q=");
    sb.Append((mQ->ToString(16, &str), str));
    sb.Append('}');

    return sb.ToString(result);
}

void OpenSSLDSAParams::EnsureReadParams()
{
    if (mFetchedParams) {
        return;
    }

    Int64 ctx;
    mKey->GetPkeyContext(&ctx);

    AutoPtr<ArrayOf<Byte> > g;
    AutoPtr<ArrayOf<Byte> > p;
    AutoPtr<ArrayOf<Byte> > q;
    AutoPtr<ArrayOf<Byte> > y;
    AutoPtr<ArrayOf<Byte> > x;
    NativeCrypto::Get_DSA_params(ctx, (ArrayOf<Byte>**)&g, (ArrayOf<Byte>**)&p,
            (ArrayOf<Byte>**)&q, (ArrayOf<Byte>**)&y, (ArrayOf<Byte>**)&x);

    if (g != NULL) {
        CBigInteger::New(*g, (IBigInteger**)&mG);
    }
    if (p != NULL) {
        CBigInteger::New(*p, (IBigInteger**)&mP);
    }
    if (q != NULL) {
        CBigInteger::New(*q, (IBigInteger**)&mQ);
    }
    if (y != NULL) {
        CBigInteger::New(*y, (IBigInteger**)&mY);
    }
    if (x != NULL) {
        CBigInteger::New(*x, (IBigInteger**)&mX);
    }

    mFetchedParams = TRUE;
}

} // namespace Conscrypt
} // namespace Org
