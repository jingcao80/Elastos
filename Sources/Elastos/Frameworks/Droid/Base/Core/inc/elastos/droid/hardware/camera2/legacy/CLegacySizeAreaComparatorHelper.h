
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYSIZEAREACOMPARATORHELPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYSIZEAREACOMPARATORHELPER_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CLegacySizeAreaComparatorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Hardware::ICameraSize;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CarClass(CLegacySizeAreaComparatorHelper)
    , public Singleton
    , public ILegacySizeAreaComparatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Get the largest api1 {@code Camera.Size} from the list by comparing each size's area
     * by each other using {@link SizeAreaComparator}.
     *
     * @param sizes a non-{@code null} list of non-{@code null} sizes
     * @return a non-{@code null} size
     *
     * @throws NullPointerException if {@code sizes} or any elements in it were {@code null}
     */
    CARAPI FindLargestByArea(
        /* [in] */ IList* sizes,
        /* [out] */ ICameraSize** outcs);
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CLEGACYSIZEAREACOMPARATORHELPER_H__
