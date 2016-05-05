
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_CPACKAGEVISIBILITYEXTRACTOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_CPACKAGEVISIBILITYEXTRACTOR_H__

#include "_Elastos_Droid_Server_Notification_CPackageVisibilityExtractor.h"
#include "elastos/droid/server/notification/PackageVisibilityExtractor.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

CarClass(CPackageVisibilityExtractor)
    , public PackageVisibilityExtractor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Notification
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NOTIFICATION_CPACKAGEVISIBILITYEXTRACTOR_H__
