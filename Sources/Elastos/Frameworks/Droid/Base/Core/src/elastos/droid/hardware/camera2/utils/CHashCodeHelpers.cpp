
#include "elastos/droid/hardware/camera2/utils/CHashCodeHelpers.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CHashCodeHelpers, Singleton, IHashCodeHelpers)

CAR_SINGLETON_IMPL(CHashCodeHelpers)

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ ArrayOf<Int32>* array,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(array, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ ArrayOf<Float>* array,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(array, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(array, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, b, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [in] */ IInterface* c,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, b, c, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [in] */ IInterface* c,
    /* [in] */ IInterface* d,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(a, b, c, d, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, z, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 w,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, z, w, value);
}

ECode CHashCodeHelpers::GetHashCode(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 z,
    /* [in] */ Int32 w,
    /* [in] */ Int32 t,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return HashCodeHelpers::GetHashCode(x, y, z, w, t, value);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos