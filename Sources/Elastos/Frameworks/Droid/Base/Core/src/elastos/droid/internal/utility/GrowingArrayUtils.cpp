
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

Int32 GrowingArrayUtils::GrowSize(
    /* [in] */ Int32 currentSize)
{
    return currentSize <= 4 ? 8 : currentSize * 2;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos