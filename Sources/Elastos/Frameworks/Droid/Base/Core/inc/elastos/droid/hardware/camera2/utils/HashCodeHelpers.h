
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_HASHCODEHELPERS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_HASHCODEHELPERS_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class HashCodeHelpers
{
public:
    /**
     * Hash every element uniformly using the Modified Bernstein hash.
     *
     * <p>Useful to implement a {@link Object#hashCode} for uniformly distributed data.</p>
     *
     * @param array a non-{@code null} array of integers
     *
     * @return the numeric hash code
     */
    static CARAPI GetHashCode(
        /* [in] */ ArrayOf<Int32>* array,
        /* [out] */ Int32* value);

    /**
     * Hash every element uniformly using the Modified Bernstein hash.
     *
     * <p>Useful to implement a {@link Object#hashCode} for uniformly distributed data.</p>
     *
     * @param array a non-{@code null} array of floats
     *
     * @return the numeric hash code
     */
    static CARAPI GetHashCode(
        /* [in] */ ArrayOf<Float>* array,
        /* [out] */ Int32* value);

    /**
     * Hash every element uniformly using the Modified Bernstein hash.
     *
     * <p>Useful to implement a {@link Object#hashCode} for uniformly distributed data.</p>
     *
     * @param array a non-{@code null} array of objects
     *
     * @return the numeric hash code
     */
    static CARAPI GetHashCode(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b,
        /* [in] */ IInterface* c,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ IInterface* a,
        /* [in] */ IInterface* b,
        /* [in] */ IInterface* c,
        /* [in] */ IInterface* d,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w,
        /* [out] */ Int32* value);

    static CARAPI GetHashCode(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 z,
        /* [in] */ Int32 w,
        /* [in] */ Int32 t,
        /* [out] */ Int32* value);
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_HASHCODEHELPERS_H__
