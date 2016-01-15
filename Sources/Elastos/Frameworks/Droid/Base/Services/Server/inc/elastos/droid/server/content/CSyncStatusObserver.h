
#ifndef __ELASTOS_DROID_SERVER_CONTENT_CSYNC_STATUS_OBSERVER_H__
#define __ELASTOS_DROID_SERVER_CONTENT_CSYNC_STATUS_OBSERVER_H__

#include "_Elastos_Droid_Server_Content_CSyncStatusObserver.h"
#include "elastos/droid/server/content/SyncManager.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

CarClass(CSyncStatusObserver)
    , public SyncManager::SyncStatusObserver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_CONTENT_CSYNC_STATUS_OBSERVER_H__
