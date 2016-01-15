#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CBinderDeathRecipient.h"

namespace Elastos {
namespace Droid {
namespace Os {

ECode CBinderDeathRecipient::ProxyDied()
{
    return NOERROR;
}
} // namespace Os
} // namespace Droid
} // namespace Elastos
