
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CPACKAGEPRIORITYEXTRACTOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CPACKAGEPRIORITYEXTRACTOR_H__

#include "_Elastos_Droid_Server_Notification_CPackagePriorityExtractor.h"
#include "elastos/droid/server/notification/PackagePriorityExtractor.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CarClass(CPackagePriorityExtractor)
    , public PackagePriorityExtractor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NOTIFICATION_CPACKAGEPRIORITYEXTRACTOR_H__
