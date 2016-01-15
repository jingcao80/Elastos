
#ifndef __ELASTOS_DROID_CONTENT_CSYNC_ACTIVITY_TOO_MANY_DELETES_H__
#define __ELASTOS_DROID_CONTENT_CSYNC_ACTIVITY_TOO_MANY_DELETES_H__

#include "_Elastos_Droid_Content_CSyncActivityTooManyDeletes.h"
#include "elastos/droid/content/SyncActivityTooManyDeletes.h"

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncActivityTooManyDeletes)
    , public SyncActivityTooManyDeletes
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNC_ACTIVITY_TOO_MANY_DELETES_H__
