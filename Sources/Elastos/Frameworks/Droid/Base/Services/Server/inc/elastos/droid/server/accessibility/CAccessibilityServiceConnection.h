
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__

#include "_Elastos_Droid_Server_Accessibility_CAccessibilityServiceConnection.h"
#include "elastos/droid/server/accessibility/AccessibilityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

/**
 * This class represents an accessibility service. It stores all per service
 * data required for the service management, provides API for starting/stopping the
 * service and is responsible for adding/removing the service in the data structures
 * for service management. The class also exposes configuration interface that is
 * passed to the service it represents as soon it is bound. It also serves as the
 * connection for the service.
 */
CarClass(CAccessibilityServiceConnection)
    , public AccessibilityManagerService::Service
{
public:
    CAR_OBJECT_DECL()
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__
