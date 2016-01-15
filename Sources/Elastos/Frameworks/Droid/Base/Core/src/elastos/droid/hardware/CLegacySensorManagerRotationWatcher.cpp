#include "elastos/droid/hardware/CLegacySensorManagerRotationWatcher.h"
#include "elastos/droid/hardware/LegacySensorManager.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(CLegacySensorManagerRotationWatcher, Object, IRotationWatcher);

CAR_OBJECT_IMPL(CLegacySensorManagerRotationWatcher);

CLegacySensorManagerRotationWatcher::CLegacySensorManagerRotationWatcher()
{}

ECode CLegacySensorManagerRotationWatcher::constructor()
{
    return NOERROR;
}

ECode CLegacySensorManagerRotationWatcher::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    LegacySensorManager::OnRotationChanged(rotation);
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos