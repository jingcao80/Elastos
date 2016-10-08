
#include "security/spec/CDSAParameterSpec.h"

using Elastos::Security::Interfaces::EIID_IDSAParams;

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CDSAParameterSpec)
CAR_INTERFACE_IMPL_3(CDSAParameterSpec, Object, IDSAParameterSpec, IDSAParams, IAlgorithmParameterSpec)
ECode CDSAParameterSpec::GetG(
    /* [out] */ IBigInteger **base)
{
    VALIDATE_NOT_NULL(base)
    *base = mG;
    REFCOUNT_ADD(*base)
    return NOERROR;
}

ECode CDSAParameterSpec::GetP(
    /* [out] */ IBigInteger **prime)
{
    VALIDATE_NOT_NULL(prime)
    *prime = mP;
    REFCOUNT_ADD(*prime)
    return NOERROR;
}

ECode CDSAParameterSpec::GetQ(
    /* [out] */ IBigInteger **subprime)
{
    VALIDATE_NOT_NULL(subprime)
    *subprime = mQ;
    REFCOUNT_ADD(*subprime)
    return NOERROR;
}

ECode CDSAParameterSpec::constructor(
    /* [in] */ IBigInteger *p,
    /* [in] */ IBigInteger *q,
    /* [in] */ IBigInteger *g)
{
    mP = p;
    mQ = q;
    mG = g;
    return NOERROR;
}

} // namespace Spec
} // namespace Security
} // namespace Security