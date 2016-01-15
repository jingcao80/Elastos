
#ifndef __ELASTOS_DROID_HARDWARE_CCARRIERFREQUENCYRANGE_H__
#define __ELASTOS_DROID_HARDWARE_CCARRIERFREQUENCYRANGE_H__

#include "_Elastos_Droid_Hardware_CCarrierFrequencyRange.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CCarrierFrequencyRange)
    , public Object
    , public ICarrierFrequencyRange
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

     /**
     * Create a segment of a carrier frequency range.
     *
     * @param min The minimum transmittable frequency in this range segment.
     * @param max The maximum transmittable frequency in this range segment.
     */
    CARAPI constructor(
        /* [in] */ Int32 min,
        /* [in] */ Int32 max);

    /**
     * Get the minimum (inclusive) frequency in this range segment.
     */
    CARAPI GetMinFrequency(
        /* [out] */ Int32* value);

    /**
     * Get the maximum (inclusive) frequency in this range segment.
     */
    CARAPI GetMaxFrequency(
        /* [out] */ Int32* value);

private:
    Int32 mMinFrequency;
    Int32 mMaxFrequency;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CCARRIERFREQUENCYRANGE_H__
