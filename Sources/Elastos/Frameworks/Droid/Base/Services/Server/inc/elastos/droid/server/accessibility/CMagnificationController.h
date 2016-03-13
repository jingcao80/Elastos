
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONCONTROLLER_H__

#include "_Elastos_Droid_Server_Accessibility_CMagnificationController.h"
#include "elastos/droid/server/accessibility/ScreenMagnifier.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

CarClass(CMagnificationController)
    , public ScreenMagnifier::MagnificationController
{
public:
    CAR_OBJECT_DECL();
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONCONTROLLER_H__
