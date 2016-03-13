
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Accessibility_CAccessibilityManagerService.h"
#include "elastos/droid/server/accessibility/AccessibilityManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

CarClass(CAccessibilityManagerService)
    , public AccessibilityManagerService
{
public:
    CAR_OBJECT_DECL();
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYMANAGERSERVICE_H__
