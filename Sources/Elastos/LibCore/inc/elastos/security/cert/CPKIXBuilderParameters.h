
#ifndef __ELASTOS_SECURITY_CERT_CPKIXBUILDERPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CPKIXBUILDERPARAMETERS_H__

#include "_Elastos_Security_Cert_CPKIXBuilderParameters.h"
#include "PKIXParameters.h"

using Elastos::Utility::IDate;
using Elastos::Security::IKeyStore;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CPKIXBuilderParameters), public PKIXParameters
{
public:
    CAR_OBJECT_DECL();

    CPKIXBuilderParameters();

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI GetMaxPathLength(
        /* [out] */ Int32 *length);

    CARAPI SetMaxPathLength(
        /* [in] */ Int32 maxPathLength);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISet *trustAnchors,
        /* [in] */ ICertSelector *targetConstraints);

    CARAPI constructor(
        /* [in] */ IKeyStore *keyStore,
        /* [in] */ ICertSelector *targetConstraints);

private:
    // Maximum certificate path length (5 by default)
    Int32 mMaxPathLength;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CPKIXBUILDERPARAMETERS_H__
