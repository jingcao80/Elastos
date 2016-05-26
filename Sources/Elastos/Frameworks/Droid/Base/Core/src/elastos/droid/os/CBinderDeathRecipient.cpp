#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CBinderDeathRecipient.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CBinderDeathRecipient, Object, IBinderDeathRecipient)

CAR_OBJECT_IMPL(CBinderDeathRecipient)

ECode CBinderDeathRecipient::ProxyDied()
{
    return NOERROR;
}
} // namespace Os
} // namespace Droid
} // namespace Elastos
