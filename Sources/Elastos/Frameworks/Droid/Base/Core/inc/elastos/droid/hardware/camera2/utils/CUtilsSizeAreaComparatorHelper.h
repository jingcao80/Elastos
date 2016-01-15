
#ifndef  __ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_CUTILSSIZEAREACOMPARATORHELPER_H__
#define  __ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_CUTILSSIZEAREACOMPARATORHELPER_H__

#include "_Elastos_Droid_Hardware_Camera2_Utils_CUtilsSizeAreaComparatorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Utility::ISize;
using Elastos::Utility::IList;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CarClass(CUtilsSizeAreaComparatorHelper)
    , public Singleton
    , public IUtilsSizeAreaComparatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Get the largest {@code Size} from the list by comparing each size's area
     * by each other using {@link SizeAreaComparator}.
     *
     * @param sizes a non-{@code null} list of non-{@code null} sizes
     * @return a non-{@code null} size
     *
     * @throws NullPointerException if {@code sizes} or any elements in it were {@code null}
     */
    CARAPI FindLargestByArea(
        /* [in] */ IList* sizes,
        /* [out] */ ISize** outsz);
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_CUTILSSIZEAREACOMPARATORHELPER_H__
