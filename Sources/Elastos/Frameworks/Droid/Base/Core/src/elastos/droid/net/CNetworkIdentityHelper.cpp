
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/CNetworkIdentity.h"
#include "elastos/droid/net/CNetworkIdentityHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkIdentityHelper, Singleton, INetworkIdentityHelper)

CAR_SINGLETON_IMPL(CNetworkIdentityHelper)

ECode CNetworkIdentityHelper::ScrubSubscriberId(
    /* [in] */ const String& subscriberId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkIdentity::ScrubSubscriberId(subscriberId, result);
}

ECode CNetworkIdentityHelper::BuildNetworkIdentity(
    /* [in] */ IContext* context,
    /* [in] */ INetworkState* state,
    /* [out] */ INetworkIdentity** result)
{
    VALIDATE_NOT_NULL(result);
    return CNetworkIdentity::BuildNetworkIdentity(context, state, result);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
