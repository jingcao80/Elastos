#ifndef __ELASTOS_UTILITY_CLINKEDHASHMAP_H__
#define __ELASTOS_UTILITY_CLINKEDHASHMAP_H__

#include "_Elastos_Utility_CLinkedHashMap.h"
#include "LinkedHashMap.h"

namespace Elastos {
namespace Utility {

CarClass(CLinkedHashMap)
    , public LinkedHashMap
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_CLINKEDHASHMAP_H__