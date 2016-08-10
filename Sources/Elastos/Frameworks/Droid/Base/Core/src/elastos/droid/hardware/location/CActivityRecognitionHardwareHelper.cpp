
#include "elastos/droid/hardware/location/CActivityRecognitionHardwareHelper.h"
#include "elastos/droid/hardware/location/ActivityRecognitionHardware.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Location {

CAR_INTERFACE_IMPL(CActivityRecognitionHardwareHelper, Singleton, IActivityRecognitionHardwareHelper)

CAR_SINGLETON_IMPL(CActivityRecognitionHardwareHelper)

ECode CActivityRecognitionHardwareHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IActivityRecognitionHardware** instance)
{
    return ActivityRecognitionHardware::GetInstance(context, instance);
}

ECode CActivityRecognitionHardwareHelper::IsSupported(
    /* [out] */ Boolean* result)
{
    return ActivityRecognitionHardware::IsSupported(result);
}

} // namespace Location
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
