
#include "elastos/droid/hardware/camera2/utils/CUtilsSizeAreaComparatorHelper.h"
#include "elastos/droid/hardware/camera2/utils/UtilsSizeAreaComparator.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(CUtilsSizeAreaComparatorHelper, Singleton, IUtilsSizeAreaComparatorHelper)

CAR_SINGLETON_IMPL(CUtilsSizeAreaComparatorHelper)

ECode CUtilsSizeAreaComparatorHelper::FindLargestByArea(
    /* [in] */ IList* sizes,
    /* [out] */ ISize** outsz)
{
    VALIDATE_NOT_NULL(outsz);

    return UtilsSizeAreaComparator::FindLargestByArea(sizes, outsz);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos