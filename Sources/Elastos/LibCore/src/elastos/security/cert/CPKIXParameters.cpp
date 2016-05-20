
#include "CPKIXParameters.h"

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CPKIXParameters)

ECode CPKIXParameters::constructor()
{
    return NOERROR;
}

ECode CPKIXParameters::constructor(
    /* [in] */ ISet *trustAnchors)
{
    return PKIXParameters::constructor(trustAnchors);
}

ECode CPKIXParameters::constructor(
    /* [in] */ IKeyStore *keyStore)
{
    return PKIXParameters::constructor(keyStore);
}

}
}
}
