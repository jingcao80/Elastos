
#include "elastos/droid/hardware/camera2/legacy/CPerfMeasurementHelper.h"
#include "elastos/droid/hardware/camera2/legacy/PerfMeasurement.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CPerfMeasurementHelper, Singleton, IPerfMeasurementHelper)

CAR_SINGLETON_IMPL(CPerfMeasurementHelper)

ECode CPerfMeasurementHelper::IsGlTimingSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return PerfMeasurement::IsGlTimingSupported(result);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
