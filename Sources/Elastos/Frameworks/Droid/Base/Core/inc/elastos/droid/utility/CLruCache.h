
#ifndef __ELASTOS_DROID_UTILITY_CLRUCACHE_H__
#define __ELASTOS_DROID_UTILITY_CLRUCACHE_H__

#include "_Elastos_Droid_Utility_CLruCache.h"
#include "elastos/droid/utility/LruCache.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CLruCache)
    , public LruCache
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CLRUCACHE_H__
