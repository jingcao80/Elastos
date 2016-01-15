
#ifndef __ELASTOS_SECURITY_CERT_CPKIXCERTPATHBUILDERRESULT_H__
#define __ELASTOS_SECURITY_CERT_CPKIXCERTPATHBUILDERRESULT_H__

#include "_Elastos_Security_Cert_CPKIXCertPathBuilderResult.h"
#include "PKIXCertPathValidatorResult.h"

using Elastos::Security::IPublicKey;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPKIXCertPathBuilderResult)
    , public PKIXCertPathValidatorResult
    , public IPKIXCertPathBuilderResult
    , public ICertPathBuilderResult
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI ToString(
        /* [out] */ String * pStr);

    CARAPI GetCertPath(
        /* [out] */ ICertPath **certPath);

    CARAPI constructor(
        /* [in] */ ICertPath *certPath,
        /* [in] */ ITrustAnchor *trustAnchor,
        /* [in] */ IPolicyNode *policyTree,
        /* [in] */ IPublicKey *subjectPublicKey);

private:
    // Built and validated certification path
    AutoPtr<ICertPath> mCertPath;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPKIXCERTPATHBUILDERRESULT_H__
