
#include <Elastos.Droid.Content.h>
#include "elastos/droid/settings/deviceinfo/StorageMeasurement.h"

using Elastos::Droid::Content::CComponentName;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

static AutoPtr<IComponentName> InitDEFAULT_CONTAINER_COMPONENT()
{
    AutoPtr<IComponentName> comp;
    CComponentName::New(String("Elastos.Droid.DefContainer")/*DEFAULT_CONTAINER_PACKAGE*/,
            String("Elastos.Droid.DefContainer.CDefaultContainerService"), (IComponentName**)&comp);
    return comp;
}

const AutoPtr<IComponentName> StorageMeasurement::DEFAULT_CONTAINER_COMPONENT = InitDEFAULT_CONTAINER_COMPONENT();

CAR_INTERFACE_IMPL(StorageMeasurement, Object, IStorageMeasurement)

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos
