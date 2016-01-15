#ifndef __ELASTOS_UTILITY_CHASHMAP_H__
#define __ELASTOS_UTILITY_CHASHMAP_H__

#include "_Elastos_Utility_CHashMap.h"
#include "HashMap.h"

namespace Elastos {
namespace Utility {

CarClass(CHashMap)
    , public HashMap
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CHASHMAP_H__
