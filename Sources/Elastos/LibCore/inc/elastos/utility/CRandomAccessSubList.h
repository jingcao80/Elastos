#ifndef __ELASTOS_UTILITY_CRANDOM_ACCESS_SUBLIST_H__
#define __ELASTOS_UTILITY_CRANDOM_ACCESS_SUBLIST_H__

#include "_Elastos_Utility_CRandomAccessSubList.h"
#include "elastos/utility/AbstractList.h"

namespace Elastos {
namespace Utility {

CarClass(CRandomAccessSubList)
    , public RandomAccessSubList
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CRANDOM_ACCESS_SUBLIST_H__
