
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CVpnService.h"
#include "elastos/droid/net/CVpnServiceHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CVpnServiceHelper, Singleton, IVpnServiceHelper)

CAR_SINGLETON_IMPL(CVpnServiceHelper)

ECode CVpnServiceHelper::Prepare(
    /* [in] */ IContext* context,
    /* [out] */ IIntent** result)
{
    return VpnService::Prepare(context, result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
