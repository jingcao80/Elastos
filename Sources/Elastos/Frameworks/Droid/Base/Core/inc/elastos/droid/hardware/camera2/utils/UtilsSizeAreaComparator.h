
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_UTILSSIZEAREACOMPARATOR_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_UTILSSIZEAREACOMPARATOR_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISize;
using Elastos::Core::Object;
using Elastos::Core::IComparator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class UtilsSizeAreaComparator
    : public Object
    , public IUtilsSizeAreaComparator
    , public IComparator
{
public:
    CAR_INTERFACE_DECL()

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI Compare(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs,
        /* [out] */ Int32* result);

    /**
     * Get the largest {@code Size} from the list by comparing each size's area
     * by each other using {@link SizeAreaComparator}.
     *
     * @param sizes a non-{@code null} list of non-{@code null} sizes
     * @return a non-{@code null} size
     *
     * @throws NullPointerException if {@code sizes} or any elements in it were {@code null}
     */
    static CARAPI FindLargestByArea(
        /* [in] */ IList* sizes,
        /* [out] */ ISize** outsz);
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_UTILS_UTILSSIZEAREACOMPARATOR_H__
