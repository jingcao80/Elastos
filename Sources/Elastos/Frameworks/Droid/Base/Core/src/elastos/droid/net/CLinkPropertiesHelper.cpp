
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CLinkPropertiesHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CLinkPropertiesHelper, Singleton, ILinkPropertiesHelper)

CAR_SINGLETON_IMPL(CLinkPropertiesHelper)

ECode CLinkPropertiesHelper::IsValidMtu(
    /* [in] */ Int32 mtu,
    /* [in] */ Boolean ipv6,
    /* [out] */ Boolean* result)
{
    return LinkProperties::IsValidMtu(mtu, ipv6, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
