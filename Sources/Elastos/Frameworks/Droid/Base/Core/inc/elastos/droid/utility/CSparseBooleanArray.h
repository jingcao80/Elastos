
#ifndef __ELASTOS_DROID_UTILITY_CSPARSEBOOLEANARRAY_H__
#define __ELASTOS_DROID_UTILITY_CSPARSEBOOLEANARRAY_H__

#include "_Elastos_Droid_Utility_CSparseBooleanArray.h"
#include "elastos/droid/utility/SparseBooleanArray.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CSparseBooleanArray)
    , public SparseBooleanArray
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CSPARSEBOOLEANARRAY_H__
