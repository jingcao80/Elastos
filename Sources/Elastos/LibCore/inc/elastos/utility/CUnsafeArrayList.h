#ifndef __ELASTOS_UTILITY_CUNSAFEARRAYLIST_H__
#define __ELASTOS_UTILITY_CUNSAFEARRAYLIST_H__

#include "_Elastos_Utility_CUnsafeArrayList.h"
#include "UnsafeArrayList.h"

namespace Elastos {
namespace Utility {

CarClass(CUnsafeArrayList)
    , public UnsafeArrayList
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CUNSAFEARRAYLIST_H__
