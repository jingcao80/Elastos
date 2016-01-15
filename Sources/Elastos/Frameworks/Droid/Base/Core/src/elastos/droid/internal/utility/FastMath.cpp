
#include "elastos/droid/internal/utility/FastMath.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

Int32 FastMath::Round(
    /* [in] */ Float value)
{
    Int64 lx = (Int64) (value * (65536 * 256.0f));
    return (Int32) ((lx + 0x800000) >> 24);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
