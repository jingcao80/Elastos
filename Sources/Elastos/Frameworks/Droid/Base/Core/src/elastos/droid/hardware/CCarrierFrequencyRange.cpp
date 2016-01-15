
#include "elastos/droid/hardware/CCarrierFrequencyRange.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(CCarrierFrequencyRange, Object, ICarrierFrequencyRange)

CAR_OBJECT_IMPL(CCarrierFrequencyRange)

ECode CCarrierFrequencyRange::constructor(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    mMinFrequency = min;
    mMaxFrequency = max;
    return NOERROR;
}

ECode CCarrierFrequencyRange::GetMinFrequency(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mMinFrequency;
    return NOERROR;
}

ECode CCarrierFrequencyRange::GetMaxFrequency(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mMaxFrequency;
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
