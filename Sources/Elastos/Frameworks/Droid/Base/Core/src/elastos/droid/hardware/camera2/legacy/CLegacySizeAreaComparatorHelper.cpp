
#include "elastos/droid/hardware/camera2/legacy/CLegacySizeAreaComparatorHelper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacySizeAreaComparator.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(CLegacySizeAreaComparatorHelper, Singleton, ILegacySizeAreaComparatorHelper)

CAR_SINGLETON_IMPL(CLegacySizeAreaComparatorHelper)

ECode CLegacySizeAreaComparatorHelper::FindLargestByArea(
    /* [in] */ IList* sizes,
    /* [out] */ ICameraSize** outcs)
{
    VALIDATE_NOT_NULL(outcs);

    return LegacySizeAreaComparator::FindLargestByArea(sizes, outcs);
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
