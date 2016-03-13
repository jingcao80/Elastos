
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYLIST_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYLIST_H__

#include "_Elastos_Utility_Concurrent_CCopyOnWriteArrayList.h"
#include "CopyOnWriteArrayList.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CCopyOnWriteArrayList)
    , public CopyOnWriteArrayList
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CCOPYONWRITEARRAYLIST_H__
