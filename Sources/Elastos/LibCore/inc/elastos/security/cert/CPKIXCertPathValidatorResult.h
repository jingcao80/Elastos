
#ifndef __ELASTOS_SECURITY_CERT_CPKIXCERTPATHVALIDATORRESULT_H__
#define __ELASTOS_SECURITY_CERT_CPKIXCERTPATHVALIDATORRESULT_H__

#include "_Elastos_Security_Cert_CPKIXCertPathValidatorResult.h"
#include "PKIXCertPathValidatorResult.h"

using Elastos::Security::IPublicKey;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPKIXCertPathValidatorResult), public PKIXCertPathValidatorResult
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ITrustAnchor *trustAnchor,
        /* [in] */ IPolicyNode *policyTree,
        /* [in] */ IPublicKey *subjectPublicKey);
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPKIXCERTPATHVALIDATORRESULT_H__
