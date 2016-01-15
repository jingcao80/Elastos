
#ifndef __ELASTOS_DROID_CONTENT_CSYNCADAPTERSCACHE_H__
#define __ELASTOS_DROID_CONTENT_CSYNCADAPTERSCACHE_H__

#include "_Elastos_Droid_Content_CSyncAdaptersCache.h"
#include "elastos/droid/content/SyncAdaptersCache.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncAdaptersCache)
    , public SyncAdaptersCache
{
public:
    CAR_OBJECT_DECL()
};

} // Content
} // Droid
} // Elastos

#endif//__ELASTOS_DROID_CONTENT_CSYNCADAPTERSCACHE_H__