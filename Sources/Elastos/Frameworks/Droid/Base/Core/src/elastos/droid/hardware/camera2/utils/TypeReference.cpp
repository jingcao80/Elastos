
#include "elastos/droid/hardware/camera2/utils/TypeReference.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(TypeReference, Object, ITypeReference)

TypeReference::TypeReference()
    : mHash(0)
{
}

ECode TypeReference::constructor()
{
    assert(0);
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
