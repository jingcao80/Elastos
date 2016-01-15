
#include "CRSAPrivateKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CRSAPrivateKeySpec);
ECode CRSAPrivateKeySpec::constructor(
    /* [in] */ IBigInteger *modulus,
    /* [in] */ IBigInteger *privateExponent)
{
    return RSAPrivateKeySpec::constructor(modulus, privateExponent);
}

}
}
}
