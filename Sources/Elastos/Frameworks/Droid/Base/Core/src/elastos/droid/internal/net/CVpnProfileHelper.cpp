
#include "elastos/droid/internal/net/CVpnProfile.h"
#include "elastos/droid/internal/net/CVpnProfileHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CAR_INTERFACE_IMPL(CVpnProfileHelper, Singleton, IVpnProfileHelper)

CAR_SINGLETON_IMPL(CVpnProfileHelper)

ECode CVpnProfileHelper::Decode(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ IVpnProfile** profile)
{
    return CVpnProfile::Decode(key, value, profile);
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
