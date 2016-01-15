#include "elastos/droid/os/CParcelUuidHelper.h"
#include "elastos/droid/os/CParcelUuid.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CParcelUuidHelper, Singleton, IParcelUuidHelper)

CAR_SINGLETON_IMPL(CParcelUuidHelper)

ECode CParcelUuidHelper::FromString(
    /* [in] */ const String& uuid,
    /* [out] */ IParcelUuid** id)
{
    return CParcelUuid::FromString(uuid, id);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos