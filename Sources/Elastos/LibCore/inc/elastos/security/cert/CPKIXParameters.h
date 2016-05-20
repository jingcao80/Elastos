
#ifndef __ELASTOS_SECURITY_CERT_CPKIXPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CPKIXPARAMETERS_H__

#include "_Elastos_Security_Cert_CPKIXParameters.h"
#include "PKIXParameters.h"

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPKIXParameters), public PKIXParameters
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISet *trustAnchors);

    CARAPI constructor(
        /* [in] */ IKeyStore *keyStore);
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPKIXPARAMETERS_H__
