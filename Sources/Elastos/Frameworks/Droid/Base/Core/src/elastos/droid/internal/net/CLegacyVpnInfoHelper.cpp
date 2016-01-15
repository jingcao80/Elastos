
#include "elastos/droid/internal/net/CLegacyVpnInfoHelper.h"
#include "elastos/droid/internal/net/CLegacyVpnInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CAR_INTERFACE_IMPL(CLegacyVpnInfoHelper, Singleton, ILegacyVpnInfoHelper)

CAR_SINGLETON_IMPL(CLegacyVpnInfoHelper)

ECode CLegacyVpnInfoHelper::StateFromNetworkInfo(
        /* [in] */ INetworkInfo* info,
        /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    *state = CLegacyVpnInfo::StateFromNetworkInfo(info);
    return NOERROR;
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
