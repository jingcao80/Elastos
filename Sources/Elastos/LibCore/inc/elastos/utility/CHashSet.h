#ifndef __ELASTOS_UTILITY_CHASHSET_H__
#define __ELASTOS_UTILITY_CHASHSET_H__

#include "_Elastos_Utility_CHashSet.h"
#include "HashSet.h"

namespace Elastos {
namespace Utility {

CarClass(CHashSet)
    , public HashSet
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY__CHASHSET_H__