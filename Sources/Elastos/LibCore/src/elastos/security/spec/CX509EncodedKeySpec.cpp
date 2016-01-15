
#include "CX509EncodedKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CX509EncodedKeySpec);
CAR_INTERFACE_IMPL(CX509EncodedKeySpec, EncodedKeySpec, IX509EncodedKeySpec);
ECode CX509EncodedKeySpec::GetFormat(
    /* [out] */ String *format)
{
    VALIDATE_NOT_NULL(format)
    *format = String("X.509");
    return NOERROR;
}

ECode CX509EncodedKeySpec::constructor(
    /* [in] */ ArrayOf<Byte> *encodedKey)
{
    return EncodedKeySpec::constructor(encodedKey);
}

}
}
}
