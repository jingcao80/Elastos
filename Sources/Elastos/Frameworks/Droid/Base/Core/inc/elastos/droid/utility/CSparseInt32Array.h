
#ifndef __ELASTOS_DROID_UTILITY_CSPARSEINT32ARRAY_H__
#define __ELASTOS_DROID_UTILITY_CSPARSEINT32ARRAY_H__

#include "_Elastos_Droid_Utility_CSparseInt32Array.h"
#include "elastos/droid/utility/SparseInt32Array.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CSparseInt32Array)
    , public SparseInt32Array
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CSPARSEINT32ARRAY_H__
