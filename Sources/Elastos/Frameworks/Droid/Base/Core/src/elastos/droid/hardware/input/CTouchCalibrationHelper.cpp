
#include "elastos/droid/hardware/input/CTouchCalibrationHelper.h"
#include "elastos/droid/hardware/input/CTouchCalibration.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

CAR_INTERFACE_IMPL(CTouchCalibrationHelper, Singleton, ITouchCalibrationHelper)

CAR_SINGLETON_IMPL(CTouchCalibrationHelper)

ECode CTouchCalibrationHelper::GetIDENTITY(
    /* [out] */ ITouchCalibration** tc)
{
    VALIDATE_NOT_NULL(tc)
    *tc = CTouchCalibration::IDENTITY;
    REFCOUNT_ADD(*tc)
    return NOERROR;
}

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
