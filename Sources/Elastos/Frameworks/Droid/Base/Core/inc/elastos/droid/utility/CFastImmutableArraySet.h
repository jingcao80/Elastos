
#ifndef __ELASTOS_DROID_UTILITY_CFASTIMMUTABLEARRAYSET_H__
#define __ELASTOS_DROID_UTILITY_CFASTIMMUTABLEARRAYSET_H__

#include "_Elastos_Droid_Utility_CFastImmutableArraySet.h"
#include "elastos/droid/utility/FastImmutableArraySet.h"

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * A fast immutable set wrapper for an array that is optimized for non-concurrent iteration.
 * The same iterator instance is reused each time to avoid creating lots of garbage.
 * Iterating over an array in this fashion is 2.5x faster than iterating over a {@link HashSet}
 * so it is worth copying the contents of the set to an array when iterating over it
 * hundreds of times.
 * @hide
 */
CarClass(CFastImmutableArraySet)
    , public FastImmutableArraySet
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CFASTIMMUTABLEARRAYSET_H__
