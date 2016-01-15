
#include "elastos/droid/internal/utility/CDumpUtils.h"
#include "elastos/droid/internal/utility/DumpUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CDumpUtils, Singleton, IDumpUtils)

CAR_SINGLETON_IMPL(CDumpUtils)

ECode CDumpUtils::DumpAsync(
    /* [in] */ IHandler* handler,
    /* [in] */ IDump* dump,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 timeout)
{
    return DumpUtils::DumpAsync(handler, dump, pw, timeout);
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
