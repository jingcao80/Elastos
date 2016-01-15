
#include "elastos/droid/internal/utility/CFastMath.h"
#include "elastos/droid/internal/utility/FastMath.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CFastMath, Singleton, IFastMath)

CAR_SINGLETON_IMPL(CFastMath)

ECode CFastMath::Round(
    /* [in] */ Float value,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FastMath::Round(value);
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
