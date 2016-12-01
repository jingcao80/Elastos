
#ifndef __ELASTOS_DROID_UTILITY_CSPARSEINT64ARRAY_H__
#define __ELASTOS_DROID_UTILITY_CSPARSEINT64ARRAY_H__

#include "_Elastos_Droid_Utility_CSparseInt64Array.h"
#include "elastos/droid/utility/SparseInt64Array.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CSparseInt64Array)
    , public SparseInt64Array
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CSPARSEINT64ARRAY_H__
