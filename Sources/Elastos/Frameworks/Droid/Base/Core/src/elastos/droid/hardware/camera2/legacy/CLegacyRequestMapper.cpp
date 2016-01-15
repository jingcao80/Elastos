
#include "elastos/droid/hardware/camera2/legacy/CLegacyRequestMapper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyRequestMapper.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CLegacyRequestMapper, Singleton, ILegacyRequestMapper)

CAR_SINGLETON_IMPL(CLegacyRequestMapper)

ECode CLegacyRequestMapper::ConvertRequestMetadata(
    /* [in] */ ILegacyRequest* legacyRequest)
{
    return LegacyRequestMapper::ConvertRequestMetadata(legacyRequest);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
