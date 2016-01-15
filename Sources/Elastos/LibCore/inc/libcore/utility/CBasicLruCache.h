#ifndef __LIBCORE_UTILITY_CBASICLRUCACHE_H__
#define __LIBCORE_UTILITY_CBASICLRUCACHE_H__

#include "_Libcore_Utility_CBasicLruCache.h"
#include "BasicLruCache.h"

namespace Libcore {
namespace Utility {

CarClass(CBasicLruCache)
    , public BasicLruCache
{
public:
    CAR_OBJECT_DECL()
};

}
}

#endif // __LIBCORE_UTILITY_CBASICLRUCACHE_H__