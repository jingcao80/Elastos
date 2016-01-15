
#ifndef __ELASTOS_DROID_SERVER_CONTENT_CACTIVE_SYNC_CONTEXT_H__
#define __ELASTOS_DROID_SERVER_CONTENT_CACTIVE_SYNC_CONTEXT_H__

#include "_Elastos_Droid_Server_Content_CActiveSyncContext.h"
#include "elastos/droid/server/content/SyncManager.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

CarClass(CActiveSyncContext)
    , public SyncManager::ActiveSyncContext
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_CONTENT_CACTIVE_SYNC_CONTEXT_H__
