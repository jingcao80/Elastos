
#include "CPKIXCertPathBuilderResult.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPKIXCertPathBuilderResult);
CAR_INTERFACE_IMPL(CPKIXCertPathBuilderResult, PKIXCertPathValidatorResult, IPKIXCertPathBuilderResult,ICertPathBuilderResult);
ECode CPKIXCertPathBuilderResult::ToString(
    /* [out] */ String *str)
{
    FAIL_RETURN(PKIXCertPathValidatorResult::ToString(str))
    StringBuilder sb(*str);
    sb.AppendCStr("\n Certification Path: ");
    String certPath;
    mCertPath->ToString(&certPath);
    sb.AppendString(certPath);
    sb.AppendCStr("\n]");
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
