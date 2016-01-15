
#include "elastos/droid/internal/os/CRuntimeInit.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CRuntimeInit, RuntimeInit, IRuntimeInit)

CAR_SINGLETON_IMPL(CRuntimeInit)

ECode CRuntimeInit::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    return RuntimeInit::Main(args);
}

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos
