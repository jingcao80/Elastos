
#ifndef __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_CTAKESCREENSHOTSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_CTAKESCREENSHOTSERVICE_H__

#include "_Elastos_Droid_SystemUI_Screenshot_CTakeScreenshotService.h"
#include "elastos/droid/systemui/screenshot/TakeScreenshotService.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

CarClass(CTakeScreenshotService)
    , public TakeScreenshotService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SCREENSHOT_CTAKESCREENSHOTSERVICE_H__