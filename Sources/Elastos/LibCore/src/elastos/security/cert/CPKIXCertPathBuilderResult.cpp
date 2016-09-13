
#include "CPKIXCertPathBuilderResult.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPKIXCertPathBuilderResult)
CAR_INTERFACE_IMPL_2(CPKIXCertPathBuilderResult, PKIXCertPathValidatorResult, \
        IPKIXCertPathBuilderResult,ICertPathBuilderResult)
ECode CPKIXCertPathBuilderResult::ToString(
    /* [out] */ String *str)
{
    FAIL_RETURN(PKIXCertPathValidatorResult::ToString(str))
    StringBuilder sb(*str);
    sb.Append("\n Certification Path: ");
    String certPath;
    IObject::Probe(mCertPath)->ToString(&certPath);
    sb.Append(certPath);
    sb.Append("\n]");
    return sb.ToString(str);
}

ECode CPKIXCertPathBuilderResult::GetCertPath(
    /* [out] */ ICertPath **certPath)
{
    VALIDATE_NOT_NULL(certPath)
    *certPath = mCertPath;
    REFCOUNT_ADD(*certPath)
    return NOERROR;
}

ECode CPKIXCertPathBuilderResult::constructor(
    /* [in] */ ICertPath *certPath,
    /* [in] */ ITrustAnchor *trustAnchor,
    /* [in] */ IPolicyNode *policyTree,
    /* [in] */ IPublicKey *subjectPublicKey)
{
    PKIXCertPathValidatorResult::constructor(trustAnchor, policyTree, subjectPublicKey);
    if (!certPath) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mCertPath = certPath;
    return NOERROR;
}

}
}
}
