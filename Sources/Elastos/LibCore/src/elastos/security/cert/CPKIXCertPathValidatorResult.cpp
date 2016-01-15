
#include "CPKIXCertPathValidatorResult.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPKIXCertPathValidatorResult);
ECode CPKIXCertPathValidatorResult::constructor(
    /* [in] */ ITrustAnchor *trustAnchor,
    /* [in] */ IPolicyNode *policyTree,
    /* [in] */ IPublicKey *subjectPublicKey)
{
    return PKIXCertPathValidatorResult::constructor(trustAnchor, policyTree, subjectPublicKey);
}

}
}
}
