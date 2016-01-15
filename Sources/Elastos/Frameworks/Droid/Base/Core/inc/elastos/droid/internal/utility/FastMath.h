#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_FASTMATH_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_FASTMATH_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

/**
 * Fast and loose math routines.
 */
class FastMath
{
public:
    /**
     * Fast round from float to int. This is faster than Math.round()
     * thought it may return slightly different results. It does not try to
     * handle (in any meaningful way) NaN or infinities.
     */
    static CARAPI_(Int32) Round(
        /* [in] */ Float value);

private:
    FastMath();
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_FASTMATH_H__
