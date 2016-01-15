
#ifndef __ELASTOS_DROID_UTILITY_CINT64SPARSEARRAY_H__
#define __ELASTOS_DROID_UTILITY_CINT64SPARSEARRAY_H__

#include "_Elastos_Droid_Utility_CInt64SparseArray.h"
#include "elastos/droid/utility/Int64SparseArray.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CInt64SparseArray)
    , public Int64SparseArray
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CINT64SPARSEARRAY_H__
