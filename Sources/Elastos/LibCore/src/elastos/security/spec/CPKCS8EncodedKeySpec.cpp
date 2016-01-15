
#include "CPKCS8EncodedKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CPKCS8EncodedKeySpec);
CAR_INTERFACE_IMPL(CPKCS8EncodedKeySpec, EncodedKeySpec, IPKCS8EncodedKeySpec);
ECode CPKCS8EncodedKeySpec::GetFormat(
    /* [out] */ String *format)
{
    VALIDATE_NOT_NULL(format)
    *format = String("PKCS#8");
    return NOERROR;
}

ECode CPKCS8EncodedKeySpec::constructor(
    /* [in] */ ArrayOf<Byte> *encodedKey)
{
    return EncodedKeySpec::constructor(encodedKey);
}

}
}
}

